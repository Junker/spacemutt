/**
 * @file
 * Representation of a single alias to an email address
 *
 * @authors
 * Copyright (C) 2017-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2017-2023 Pietro Cerutti <gahr@gahr.ch>
 * Copyright (C) 2019 Federico Kircheis <federico.kircheis@gmail.com>
 * Copyright (C) 2023 Anna Figueiredo Gomes <navi@vlhl.dev>
 * Copyright (C) 2023 Dennis Schön <mail@dennis-schoen.de>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @page alias_alias Alias for an email address
 *
 * Representation of a single alias to an email address
 */

#include "config.h"
#include <pwd.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include "mutt/lib.h"
#include "address/lib.h"
#include "config/lib.h"
#include "email/lib.h"
#include "core/lib.h"
#include "gui/lib.h"
#include "mutt.h"
#include "alias.h"
#include "lib.h"
#include "browser/lib.h"
#include "editor/lib.h"
#include "history/lib.h"
#include "question/lib.h"
#include "send/lib.h"
#include "alternates.h"
#include "globals.h"
#include "alias.h"
#include "maillist.h"
#include "muttlib.h"
#include "reverse.h"

AliasList *Aliases;

/**
 * write_safe_address - Defang malicious email addresses
 * @param fp File to write to
 * @param s  Email address to defang
 *
 * if someone has an address like
 *      From: John `/bin/rm -f ~` Doe <john.doe@example.com>
 * and the user creates an alias for this, NeoMutt could wind up executing
 * the backticks because it writes aliases like
 *      alias me John `/bin/rm -f ~` Doe <john.doe@example.com>
 * To avoid this problem, use a backslash (\) to quote any backticks.  We also
 * need to quote backslashes as well, since you could defeat the above by
 * doing
 *      From: John \`/bin/rm -f ~\` Doe <john.doe@example.com>
 * since that would get aliased as
 *      alias me John \\`/bin/rm -f ~\\` Doe <john.doe@example.com>
 * which still gets evaluated because the double backslash is not a quote.
 *
 * Additionally, we need to quote ' and " characters, otherwise neomutt will
 * interpret them on the wrong parsing step.
 *
 * $ wants to be quoted since it may indicate the start of an environment
 * variable.
 */
static void write_safe_address(FILE *fp, const char *s)
{
  while (*s)
  {
    if ((*s == '\\') || (*s == '`') || (*s == '\'') || (*s == '"') || (*s == '$'))
      fputc('\\', fp);
    fputc(*s, fp);
    s++;
  }
}

/**
 * expand_aliases_r - Expand aliases, recursively
 * @param[in]  al   Address List
 * @param[out] expn Alias List
 *
 * GSList expn is used as temporary storage for already-expanded aliases.
 */
static void expand_aliases_r(AddressList *al, GSList **expn)
{
  GList *npa = al->head;
  while (npa)
  {
    struct Address *a = npa->data;
    if (!a->group && !a->personal && a->mailbox && !buf_find_char(a->mailbox, '@'))
    {
      AddressList *alias = alias_lookup(buf_string(a->mailbox));
      if (alias)
      {
        bool duplicate = false;
        for (GSList *np = *expn; np != NULL; np = np->next)
        {
          if (mutt_str_equal(buf_string(a->mailbox), np->data)) /* alias already found */
          {
            mutt_debug(LL_DEBUG1, "loop in alias found for '%s'\n", buf_string(a->mailbox));
            duplicate = true;
            break;
          }
        }

        if (duplicate)
        {
          // We've already seen this alias, so drop it
          GList *next = npa->next;
          g_queue_delete_link(al, npa);
          mutt_addr_free(&a);
          npa = next;
          continue;
        }

        // Keep a list of aliases that we've already seen
        *expn = g_slist_prepend(*expn, buf_strdup(a->mailbox));

        /* The alias may expand to several addresses,
         * some of which may themselves be aliases.
         * Create a copy and recursively expand any aliases within. */
        AddressList *copy = mutt_addrlist_new();
        mutt_addrlist_copy(copy, alias, false);
        expand_aliases_r(copy, expn);

        /* Next, move the expanded addresses
         * from the copy into the original list (before the alias) */
        for (GList *npc = copy->head; npc != NULL; npc = npc->next)
        {
          struct Address *a2 = npc->data;
          g_queue_insert_before(al, npa, a2);
        }
        g_queue_free(copy);
        npa = npa->prev;
        // Finally, remove the alias itself
        struct Address *anext = npa->next->data;
        g_queue_delete_link(al, npa->next);
        mutt_addr_free(&anext);
      }
      else
      {
        struct passwd *pw = getpwnam(buf_string(a->mailbox));
        if (pw)
        {
          char namebuf[256] = { 0 };

          mutt_gecos_name(namebuf, sizeof(namebuf), pw);
          a->personal = buf_new(namebuf);
        }
      }
    }
    npa = npa->next;
  }

  const char *fqdn = NULL;
  const bool c_use_domain = cs_subset_bool(NeoMutt->sub, "use_domain");
  if (c_use_domain && (fqdn = mutt_fqdn(true, NeoMutt->sub)))
  {
    /* now qualify all local addresses */
    mutt_addrlist_qualify(al, fqdn);
  }
}

/**
 * recode_buf - Convert some text between two character sets
 * @param buf Buffer to convert
 *
 * The 'from' charset is controlled by the 'charset'        config variable.
 * The 'to'   charset is controlled by the 'config_charset' config variable.
 */
static void recode_buf(struct Buffer *buf)
{
  const char *const c_config_charset = cs_subset_string(NeoMutt->sub, "config_charset");
  if (!c_config_charset || !cc_charset())
    return;

  char *s = buf_strdup(buf);
  if (!s)
    return;
  if (mutt_ch_convert_string(&s, cc_charset(), c_config_charset, MUTT_ICONV_NO_FLAGS) == 0)
    buf_strcpy(buf, s);
  FREE(&s);
}

/**
 * check_alias_name - Sanity-check an alias name
 * @param s       Alias to check
 * @param dest    Buffer for the result
 * @retval  0 Success
 * @retval -1 Error
 *
 * Only characters which are non-special to both the RFC822 and the neomutt
 * configuration parser are permitted.
 */
static int check_alias_name(const char *s, struct Buffer *dest)
{
  wchar_t wc = 0;
  mbstate_t mbstate = { 0 };
  size_t l;
  int rc = 0;
  bool dry = !dest; // Dry run

  if (!dry)
    buf_reset(dest);
  for (; s && *s && (l = mbrtowc(&wc, s, MB_CUR_MAX, &mbstate)) != 0; s += l)
  {
    bool bad = (l == ICONV_ILLEGAL_SEQ) || (l == ICONV_BUF_TOO_SMALL); /* conversion error */
    if (l == 1)
      bad = bad || (!strchr("-_+=.", *s) && !iswalnum(wc));
    else
      bad = bad || !iswalnum(wc);
    if (bad)
    {
      if (dry)
        return -1;
      if (l == ICONV_ILLEGAL_SEQ)
        memset(&mbstate, 0, sizeof(mbstate_t));
      buf_addch(dest, '_');
      rc = -1;
    }
    else if (!dry)
    {
      buf_addstr_n(dest, s, l);
    }
  }

  return rc;
}

/**
 * string_is_address - Does an email address match a user and domain?
 * @param str    Address string to test
 * @param user   User name
 * @param domain Domain name
 * @retval true They match
 */
static bool string_is_address(const char *str, const char *user, const char *domain)
{
  char buf[1024] = { 0 };

  snprintf(buf, sizeof(buf), "%s@%s", NONULL(user), NONULL(domain));
  if (mutt_istr_equal(str, buf))
    return true;

  return false;
}

/**
 * alias_lookup - Find an Alias
 * @param name Alias name to find
 * @retval ptr  Address for the Alias
 * @retval NULL No such Alias
 *
 * @note The search is case-insensitive
 */
AddressList *alias_lookup(const char *name)
{

  for (GList *np = Aliases->head; np != NULL; np = np->next)
  {
    struct Alias *a = np->data;
    if (mutt_istr_equal(name, a->name))
      return a->addr;
  }
  return NULL;
}

/**
 * mutt_expand_aliases - Expand aliases in a List of Addresses
 * @param al AddressList
 *
 * Duplicate addresses are dropped
 */
void mutt_expand_aliases(AddressList *al)
{
  // previously expanded aliases to avoid loops
  GSList *expn = NULL;

  expand_aliases_r(al, &expn);
  g_slist_free_full(g_steal_pointer(&expn), g_free);
  mutt_addrlist_dedupe(al);
}

/**
 * mutt_expand_aliases_env - Expand aliases in all the fields of an Envelope
 * @param env Envelope to expand
 */
void mutt_expand_aliases_env(struct Envelope *env)
{
  mutt_expand_aliases(env->from);
  mutt_expand_aliases(env->to);
  mutt_expand_aliases(env->cc);
  mutt_expand_aliases(env->bcc);
  mutt_expand_aliases(env->reply_to);
  mutt_expand_aliases(env->mail_followup_to);
}

/**
 * mutt_get_address - Get an Address from an Envelope
 * @param[in]  env    Envelope to examine
 * @param[out] prefix Prefix for the Address, e.g. "To:"
 * @retval ptr AddressList in the Envelope
 *
 * @note The caller must NOT free the returned AddressList
 */
AddressList *mutt_get_address(struct Envelope *env, const char **prefix)
{
  AddressList *al = NULL;
  const char *pfx = NULL;

  if (mutt_addr_is_user(g_queue_peek_head(env->from)))
  {
    if (!g_queue_is_empty(env->to) && !mutt_is_mail_list(g_queue_peek_head(env->to)))
    {
      pfx = "To";
      al = env->to;
    }
    else
    {
      pfx = "Cc";
      al = env->cc;
    }
  }
  else if (!g_queue_is_empty(env->reply_to) && !mutt_is_mail_list(g_queue_peek_head(env->reply_to)))
  {
    pfx = "Reply-To";
    al = env->reply_to;
  }
  else
  {
    al = env->from;
    pfx = "From";
  }

  if (prefix)
    *prefix = pfx;

  return al;
}

/**
 * alias_create - Create a new Alias from an Address
 * @param al Address to use
 * @param sub Config items
 */
void alias_create(AddressList *al, const struct ConfigSubset *sub)
{
  struct Buffer *buf = buf_pool_get();
  struct Buffer *fixed = buf_pool_get();
  struct Buffer *prompt = NULL;
  struct Buffer *tmp = buf_pool_get();

  struct Address *addr = NULL;
  char *pc = NULL;
  char *err = NULL;
  FILE *fp_alias = NULL;

  if (al)
  {
    addr = g_queue_peek_head(al);
    if (addr && addr->mailbox)
    {
      buf_copy(tmp, addr->mailbox);
      pc = strchr(buf_string(tmp), '@');
      if (pc)
        *pc = '\0';
    }
  }

  /* Don't suggest a bad alias name in the event of a strange local part. */
  check_alias_name(buf_string(tmp), buf);

retry_name:
  /* L10N: prompt to add a new alias */
  if ((mw_get_field(_("Alias as: "), buf, MUTT_COMP_NO_FLAGS, HC_OTHER, NULL, NULL) != 0) ||
      buf_is_empty(buf))
  {
    goto done;
  }

  /* check to see if the user already has an alias defined */
  if (alias_lookup(buf_string(buf)))
  {
    mutt_error(_("You already have an alias defined with that name"));
    goto done;
  }

  if (check_alias_name(buf_string(buf), fixed))
  {
    switch (query_yesorno(_("Warning: This alias name may not work.  Fix it?"), MUTT_YES))
    {
      case MUTT_YES:
        buf_copy(buf, fixed);
        goto retry_name;
      case MUTT_ABORT:
        goto done;
      default:; // do nothing
    }
  }

  struct Alias *alias = alias_new();
  alias->name = buf_strdup(buf);

  mutt_addrlist_to_local(al);

  if (addr && addr->mailbox)
    buf_copy(buf, addr->mailbox);
  else
    buf_reset(buf);

  mutt_addrlist_to_intl(al, NULL);

  do
  {
    if ((mw_get_field(_("Address: "), buf, MUTT_COMP_NO_FLAGS, HC_OTHER, NULL, NULL) != 0) ||
        buf_is_empty(buf))
    {
      alias_free(&alias);
      goto done;
    }

    mutt_addrlist_parse(alias->addr, buf_string(buf));
    if (g_queue_is_empty(alias->addr))
      mutt_beep(false);
    if (mutt_addrlist_to_intl(alias->addr, &err))
    {
      mutt_error(_("Bad IDN: '%s'"), err);
      FREE(&err);
      continue;
    }
  } while (g_queue_is_empty(alias->addr));

  if (addr && addr->personal && !mutt_is_mail_list(addr))
    buf_copy(buf, addr->personal);
  else
    buf_reset(buf);

  if (mw_get_field(_("Personal name: "), buf, MUTT_COMP_NO_FLAGS, HC_OTHER, NULL, NULL) != 0)
  {
    alias_free(&alias);
    goto done;
  }

  ((struct Address*)g_queue_peek_head(alias->addr))->personal = buf_new(buf_string(buf));

  buf_reset(buf);
  if (mw_get_field(_("Comment: "), buf, MUTT_COMP_NO_FLAGS, HC_OTHER, NULL, NULL) == 0)
  {
    mutt_str_replace(&alias->comment, buf_string(buf));
  }

  buf_reset(buf);
  if (mw_get_field(_("Tags (comma-separated): "), buf, MUTT_COMP_NO_FLAGS,
                   HC_OTHER, NULL, NULL) == 0)
  {
    parse_alias_tags(buf_string(buf), &alias->tags);
  }

  buf_reset(buf);
  mutt_addrlist_write(alias->addr, buf, true);
  prompt = buf_pool_get();

  buf_printf(prompt, "alias %s %s", alias->name, buf_string(buf));

  bool has_tags = STAILQ_FIRST(&alias->tags);

  if (alias->comment || has_tags)
    buf_addstr(prompt, " #");

  if (alias->comment)
    buf_add_printf(prompt, " %s", alias->comment);

  if (has_tags)
  {
    if (STAILQ_FIRST(&alias->tags))
    {
      buf_addstr(prompt, " tags:");
      alias_tags_to_buffer(&alias->tags, prompt);
    }
  }

  buf_add_printf(prompt, "\n%s", _("Accept?"));

  if (query_yesorno(buf_string(prompt), MUTT_YES) != MUTT_YES)
  {
    alias_free(&alias);
    goto done;
  }

  alias_reverse_add(alias);
  g_queue_push_tail(Aliases, alias);

  const char *const c_alias_file = cs_subset_path(sub, "alias_file");
  buf_strcpy(buf, c_alias_file);

  struct FileCompletionData cdata = { false, NULL, NULL, NULL };
  if (mw_get_field(_("Save to file: "), buf, MUTT_COMP_CLEAR, HC_FILE,
                   &CompleteFileOps, &cdata) != 0)
  {
    goto done;
  }
  buf_expand_path(buf);
  fp_alias = mutt_file_fopen(buf_string(buf), "a+");
  if (!fp_alias)
  {
    mutt_perror("%s", buf_string(buf));
    goto done;
  }

  /* terminate existing file with \n if necessary */
  if (!mutt_file_seek(fp_alias, 0, SEEK_END))
  {
    goto done;
  }
  if (ftell(fp_alias) > 0)
  {
    if (!mutt_file_seek(fp_alias, -1, SEEK_CUR))
    {
      goto done;
    }
    if (fread(buf->data, 1, 1, fp_alias) != 1)
    {
      mutt_perror(_("Error reading alias file"));
      goto done;
    }
    if (!mutt_file_seek(fp_alias, 0, SEEK_END))
    {
      goto done;
    }
    if (buf->data[0] != '\n')
      fputc('\n', fp_alias);
  }

  if (check_alias_name(alias->name, NULL))
    buf_quote_filename(buf, alias->name, true);
  else
    buf_strcpy(buf, alias->name);

  recode_buf(buf);
  fprintf(fp_alias, "alias %s ", buf_string(buf));
  buf_reset(buf);

  mutt_addrlist_write(alias->addr, buf, false);
  recode_buf(buf);
  write_safe_address(fp_alias, buf_string(buf));
  if (alias->comment)
    fprintf(fp_alias, " # %s", alias->comment);
  if (STAILQ_FIRST(&alias->tags))
  {
    fprintf(fp_alias, " tags:");

    struct Tag *tag = NULL;
    STAILQ_FOREACH(tag, &alias->tags, entries)
    {
      fprintf(fp_alias, "%s", tag->name);
      if (STAILQ_NEXT(tag, entries))
        fprintf(fp_alias, ",");
    }
  }
  fputc('\n', fp_alias);
  if (mutt_file_fsync_close(&fp_alias) != 0)
    mutt_perror(_("Trouble adding alias"));
  else
    mutt_message(_("Alias added"));

done:
  mutt_file_fclose(&fp_alias);
  buf_pool_release(&buf);
  buf_pool_release(&fixed);
  buf_pool_release(&prompt);
  buf_pool_release(&tmp);
}

/**
 * mutt_addr_is_user - Does the address belong to the user
 * @param addr Address to check
 * @retval true The given address belongs to the user
 */
bool mutt_addr_is_user(const struct Address *addr)
{
  if (!addr)
  {
    mutt_debug(LL_DEBUG5, "no, NULL address\n");
    return false;
  }
  if (!addr->mailbox)
  {
    mutt_debug(LL_DEBUG5, "no, no mailbox\n");
    return false;
  }

  if (mutt_istr_equal(buf_string(addr->mailbox), Username))
  {
    mutt_debug(LL_DEBUG5, "#1 yes, %s = %s\n", buf_string(addr->mailbox), Username);
    return true;
  }
  if (string_is_address(buf_string(addr->mailbox), Username, ShortHostname))
  {
    mutt_debug(LL_DEBUG5, "#2 yes, %s = %s @ %s\n", buf_string(addr->mailbox),
               Username, ShortHostname);
    return true;
  }
  const char *fqdn = mutt_fqdn(false, NeoMutt->sub);
  if (string_is_address(buf_string(addr->mailbox), Username, fqdn))
  {
    mutt_debug(LL_DEBUG5, "#3 yes, %s = %s @ %s\n", buf_string(addr->mailbox),
               Username, NONULL(fqdn));
    return true;
  }
  fqdn = mutt_fqdn(true, NeoMutt->sub);
  if (string_is_address(buf_string(addr->mailbox), Username, fqdn))
  {
    mutt_debug(LL_DEBUG5, "#4 yes, %s = %s @ %s\n", buf_string(addr->mailbox),
               Username, NONULL(fqdn));
    return true;
  }

  const struct Address *c_from = cs_subset_address(NeoMutt->sub, "from");
  if (c_from && mutt_istr_equal(buf_string(c_from->mailbox), buf_string(addr->mailbox)))
  {
    mutt_debug(LL_DEBUG5, "#5 yes, %s = %s\n", buf_string(addr->mailbox),
               buf_string(c_from->mailbox));
    return true;
  }

  if (mutt_alternates_match(buf_string(addr->mailbox)))
    return true;

  mutt_debug(LL_DEBUG5, "no, all failed\n");
  return false;
}

/**
 * alias_new - Create a new Alias
 * @retval ptr Newly allocated Alias
 *
 * Free the result with alias_free()
 */
struct Alias *alias_new(void)
{
  struct Alias *a = mutt_mem_calloc(1, sizeof(struct Alias));
  a->addr = mutt_addrlist_new();
  STAILQ_INIT(&a->tags);
  return a;
}

/**
 * alias_free - Free an Alias
 * @param[out] ptr Alias to free
 */
void alias_free(struct Alias **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct Alias *alias = *ptr;

  mutt_debug(LL_NOTIFY, "NT_ALIAS_DELETE: %s\n", alias->name);
  struct EventAlias ev_a = { alias };
  notify_send(NeoMutt->notify, NT_ALIAS, NT_ALIAS_DELETE, &ev_a);

  FREE(&alias->name);
  FREE(&alias->comment);
  driver_tags_free(&alias->tags);
  mutt_addrlist_free_full(g_steal_pointer(&alias->addr));

  FREE(ptr);
}

/**
 * aliaslist_new - creates a new AliasList
 * @retval AddressList
 */
AliasList *aliaslist_new()
{
  return g_queue_new();
}


/**
 * aliaslist_clear - Empty a List of Aliases
 * @param al AliasList to empty
 *
 * Each Alias will be freed and the AliasList will be left empty.
 */
void aliaslist_clear(AliasList *al)
{
  if (!al)
    return;

  struct Alias *a = NULL;
  while ((a = g_queue_pop_tail(al)) != NULL)
  {
    alias_free(&a);
  }
}

/**
 * aliaslist_free_full - free all Aliases and AliasList
 * @param al AliasList
 *
 */
void aliaslist_free_full(AliasList *al)
{
  if (!al)
    return;

  aliaslist_clear(al);
  g_queue_free(al);
}


/**
 * alias_init - Set up the Alias globals
 */
void alias_init(void)
{
  Aliases = aliaslist_new();
  alias_reverse_init();
}

/**
 * alias_cleanup - Clean up the Alias globals
 */
void alias_cleanup(void)
{
  for (GList *np = Aliases->head; np != NULL; np = np->next)
  {
    struct Alias *alias = np->data;
    alias_reverse_delete(alias);
  }
  aliaslist_free_full(Aliases);
  alias_reverse_shutdown();
}
