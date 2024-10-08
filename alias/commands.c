/**
 * @file
 * Alias commands
 *
 * @authors
 * Copyright (C) 2020 Pietro Cerutti <gahr@gahr.ch>
 * Copyright (C) 2020-2023 Richard Russon <rich@flatcap.org>
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
 * @page alias_commands Alias commands
 *
 * Alias commands
 */

#include "config.h"
#include <stdint.h>
#include <stdio.h>
#include "mutt/lib.h"
#include "address/lib.h"
#include "config/lib.h"
#include "email/lib.h"
#include "core/lib.h"
#include "commands.h"
#include "lib.h"
#include "parse/lib.h"
#include "alias.h"
#include "reverse.h"

/**
 * alias_tags_to_buffer - Write a comma-separated list of tags to a Buffer
 * @param tl  Tags
 * @param buf Buffer for the result
 */
void alias_tags_to_buffer(TagList *tl, struct Buffer *buf)
{
  for (GSList *np = tl; np != NULL; np = np->next)
  {
    struct Tag *tag = np->data;
    buf_addstr(buf, tag->name);
    if (np->next)
      buf_addch(buf, ',');
  }
}

/**
 * parse_alias_tags - Parse a comma-separated list of tags
 * @param tags Comma-separated string
 * @param tl   TagList for the results
 */
void parse_alias_tags(const char *tags, TagList **tl)
{
  if (!tags || !tl)
    return;

  struct StrList *sl = strlist_parse(tags, D_STRLIST_SEP_COMMA);
  if (strlist_is_empty(sl))
  {
    strlist_free(&sl);
    return;
  }

  for (GSList *np = sl->head; np != NULL; np = np->next)
  {
    struct Tag *tag = tag_new();
    tag->name = np->data; // Transfer string
    np->data = NULL;
    *tl = g_slist_append(*tl, tag);
  }
  strlist_free(&sl);
}

/**
 * parse_alias_comments - Parse the alias/query comment field
 * @param alias Alias for the result
 * @param com   Comment string
 *
 * If the comment contains a 'tags:' field, the result will be put in alias.tags
 */
void parse_alias_comments(struct Alias *alias, const char *com)
{
  if (!com || (com[0] == '\0'))
    return;

  const regmatch_t *match = mutt_prex_capture(PREX_ALIAS_TAGS, com);
  if (match)
  {
    const regmatch_t *pre = &match[PREX_ALIAS_TAGS_MATCH_PRE];
    const regmatch_t *tags = &match[PREX_ALIAS_TAGS_MATCH_TAGS];
    const regmatch_t *post = &match[PREX_ALIAS_TAGS_MATCH_POST];

    struct Buffer *tmp = buf_pool_get();

    // Extract the tags
    buf_addstr_n(tmp, com + mutt_regmatch_start(tags),
                 mutt_regmatch_end(tags) - mutt_regmatch_start(tags));
    parse_alias_tags(buf_string(tmp), &alias->tags);
    buf_reset(tmp);

    // Collect all the other text as "comments"
    buf_addstr_n(tmp, com + mutt_regmatch_start(pre),
                 mutt_regmatch_end(pre) - mutt_regmatch_start(pre));
    buf_addstr_n(tmp, com + mutt_regmatch_start(post),
                 mutt_regmatch_end(post) - mutt_regmatch_start(post));
    alias->comment = buf_strdup(tmp);

    buf_pool_release(&tmp);
  }
  else
  {
    alias->comment = mutt_str_dup(com);
  }
}

/**
 * parse_alias - Parse the 'alias' command - Implements Command::parse() - @ingroup command_parse
 *
 * e.g. "alias jim James Smith <js@example.com> # Pointy-haired boss"
 */
enum CommandResult parse_alias(struct Buffer *buf, struct Buffer *s,
                               intptr_t data, struct Buffer *err)
{
  struct Alias *tmp = NULL;
  GroupList *gl = NULL;
  enum NotifyAlias event;

  if (!MoreArgs(s))
  {
    buf_strcpy(err, _("alias: no address"));
    return MUTT_CMD_WARNING;
  }

  /* name */
  parse_extract_token(buf, s, TOKEN_NO_FLAGS);
  log_debug5("First token is '%s'", buf->data);
  if (parse_grouplist(&gl, buf, s, err) == -1)
  {
    return MUTT_CMD_ERROR;
  }
  char *name = mutt_str_dup(buf->data);

  /* address list */
  parse_extract_token(buf, s, TOKEN_QUOTE | TOKEN_SPACE | TOKEN_SEMICOLON);
  log_debug5("Second token is '%s'", buf->data);
  AddressList *al = mutt_addrlist_new();
  int parsed = mutt_addrlist_parse2(al, buf->data);
  if (parsed == 0)
  {
    buf_printf(err, _("Warning: Bad address '%s' in alias '%s'"), buf->data, name);
    g_queue_free(al);
    FREE(&name);
    mutt_grouplist_free(gl);
    return MUTT_CMD_ERROR;
  }

  /* IDN */
  char *estr = NULL;
  if (mutt_addrlist_to_intl(al, &estr))
  {
    buf_printf(err, _("Warning: Bad IDN '%s' in alias '%s'"), estr, name);
    FREE(&name);
    FREE(&estr);
    mutt_grouplist_free(gl);
    return MUTT_CMD_ERROR;
  }

  /* check to see if an alias with this name already exists */
  for (GList *np = Aliases->head; np != NULL; np = np->next)
  {
    tmp = np->data;
    if (mutt_istr_equal(tmp->name, name))
      break;
    tmp = NULL;
  }

  if (tmp)
  {
    FREE(&name);
    alias_reverse_delete(tmp);
    /* override the previous value */
    mutt_addrlist_clear(tmp->addr);
    FREE(&tmp->comment);
    event = NT_ALIAS_CHANGE;
  }
  else
  {
    /* create a new alias */
    tmp = alias_new();
    tmp->name = name;
    g_queue_push_tail(Aliases, tmp);
    event = NT_ALIAS_ADD;
  }
  tmp->addr = al;

  mutt_grouplist_add_addrlist(gl, tmp->addr);

  const short c_debug_level = cs_subset_number(SpaceMutt->sub, "debug_level");
  if (c_debug_level > 4)
  {
    /* A group is terminated with an empty address, so check a->mailbox */
    for (GList *np = tmp->addr->head; np != NULL; np = np->next)
    {
      struct Address *a = np->data;
      if (!a->mailbox)
        break;

      if (a->group)
        log_debug5("  Group %s", buf_string(a->mailbox));
      else
        log_debug5("  %s", buf_string(a->mailbox));
    }
  }
  mutt_grouplist_free(g_steal_pointer(&gl));
  if (!MoreArgs(s) && (s->dptr[0] == '#'))
  {
    s->dptr++; // skip over the "# "
    if (*s->dptr == ' ')
      s->dptr++;

    parse_alias_comments(tmp, s->dptr);
    *s->dptr = '\0'; // We're done parsing
  }

  alias_reverse_add(tmp);

  log_notify("%s: %s",
             (event == NT_ALIAS_ADD) ? "NT_ALIAS_ADD" : "NT_ALIAS_CHANGE", tmp->name);
  struct EventAlias ev_a = { tmp };
  notify_send(SpaceMutt->notify, NT_ALIAS, event, &ev_a);

  return MUTT_CMD_SUCCESS;
}

/**
 * parse_unalias - Parse the 'unalias' command - Implements Command::parse() - @ingroup command_parse
 */
enum CommandResult parse_unalias(struct Buffer *buf, struct Buffer *s,
                                 intptr_t data, struct Buffer *err)
{
  do
  {
    parse_extract_token(buf, s, TOKEN_NO_FLAGS);

    if (mutt_str_equal("*", buf->data))
    {
      for (GList *np = Aliases->head; np != NULL; np = np->next)
      {
        struct Alias *alias = np->data;
        alias_reverse_delete(alias);
      }

      aliaslist_clear(Aliases);
      return MUTT_CMD_SUCCESS;
    }

    for (GList *np = Aliases->head; np != NULL; np = np->next)
    {
      struct Alias *alias = np->data;
      if (!mutt_istr_equal(buf->data, alias->name))
        continue;

      g_queue_remove(Aliases, alias);
      alias_reverse_delete(alias);
      alias_free(&alias);
      break;
    }
  } while (MoreArgs(s));
  return MUTT_CMD_SUCCESS;
}
