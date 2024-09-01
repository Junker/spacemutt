/**
 * @file
 * Dump an Email
 *
 * @authors
 * Copyright (C) 2020 Richard Russon <rich@flatcap.org>
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
 * @page debug_email Dump an Email
 *
 * Dump an Email
 */

#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "mutt/lib.h"
#include "address/lib.h"
#include "email/lib.h"
#include "lib.h"
#include "attach/lib.h"
#include "ncrypt/lib.h"

void dump_addr_list(char *buf, size_t buflen, AddressList *al, const char *name)
{
  if (!buf || !al)
    return;
  if (g_queue_is_empty(al))
    return;

  buf[0] = '\0';
  struct Buffer *tmpbuf = buf_pool_get();
  mutt_addrlist_write(al, tmpbuf, true);
  mutt_str_copy(buf, buf_string(tmpbuf), buflen);
  buf_pool_release(&tmpbuf);

  log_debug1("\t%s: %s", name, buf);
}

void dump_gqueue(GQueue *list, const char *name)
{
  if (!list || !name)
    return;
  if (g_queue_is_empty(list))
    return;

  struct Buffer *buf = buf_pool_get();

  for (GList *np = list->head; np != NULL; np = np->next)
  {
    if (!buf_is_empty(buf))
      buf_addch(buf, ',');
    buf_addstr(buf, np->data);
  }

  log_debug1("\t%s: %s", name, buf_string(buf));
  buf_pool_release(&buf);
}

void dump_envelope(const struct Envelope *env)
{
  log_debug1("Envelope");

  if (!env)
  {
    log_debug1("\tNULL pointer");
    return;
  }

  struct Buffer *buf = buf_pool_get();
  char arr[1024];

#define ADD_FLAG(F) add_flag(buf, (env->changed & F), #F)
  ADD_FLAG(MUTT_ENV_CHANGED_IRT);
  ADD_FLAG(MUTT_ENV_CHANGED_REFS);
  ADD_FLAG(MUTT_ENV_CHANGED_XLABEL);
  ADD_FLAG(MUTT_ENV_CHANGED_SUBJECT);
#undef ADD_FLAG
  log_debug1("\tchanged: %s", buf_is_empty(buf) ? "[NONE]" : buf_string(buf));

#define ADDR_LIST(AL) dump_addr_list(arr, sizeof(arr), env->AL, #AL)
  ADDR_LIST(return_path);
  ADDR_LIST(from);
  ADDR_LIST(to);
  ADDR_LIST(cc);
  ADDR_LIST(bcc);
  ADDR_LIST(sender);
  ADDR_LIST(reply_to);
  ADDR_LIST(mail_followup_to);
  ADDR_LIST(x_original_to);
#undef ADDR_LIST

#define OPT_STRING(S)                                                          \
  if (env->S)                                                                  \
  log_debug1("\t%s: %s", #S, env->S)
  OPT_STRING(list_post);
  OPT_STRING(list_subscribe);
  OPT_STRING(list_unsubscribe);
  OPT_STRING(subject);
  OPT_STRING(real_subj);
  OPT_STRING(disp_subj);
  OPT_STRING(message_id);
  OPT_STRING(supersedes);
  OPT_STRING(date);
  OPT_STRING(x_label);
  OPT_STRING(organization);
  OPT_STRING(newsgroups);
  OPT_STRING(xref);
  OPT_STRING(followup_to);
  OPT_STRING(x_comment_to);
#undef OPT_STRING

  dump_gqueue(env->references, "references");
  dump_gqueue(env->in_reply_to, "in_reply_to");
  dump_gqueue(env->userhdrs, "userhdrs");

  if (!buf_is_empty(&env->spam))
    log_debug1("\tspam: %s", buf_string(&env->spam));

#ifdef USE_AUTOCRYPT
  if (env->autocrypt)
    log_debug1("\tautocrypt: %p", (void *) env->autocrypt);
  if (env->autocrypt_gossip)
    log_debug1("\tautocrypt_gossip: %p", (void *) env->autocrypt_gossip);
#endif

  buf_pool_release(&buf);
}

void dump_email(const struct Email *e)
{
  log_debug1("Email");

  if (!e)
  {
    log_debug1("\tNULL pointer");
    return;
  }

  struct Buffer *buf = buf_pool_get();
  char arr[256];

  log_debug1("\tpath: %s", e->path);

#define ADD_FLAG(F) add_flag(buf, e->F, #F)
  ADD_FLAG(active);
  ADD_FLAG(attach_del);
  ADD_FLAG(attach_valid);
  ADD_FLAG(changed);
  ADD_FLAG(collapsed);
  ADD_FLAG(deleted);
  ADD_FLAG(display_subject);
  ADD_FLAG(expired);
  ADD_FLAG(flagged);
  ADD_FLAG(matched);
  ADD_FLAG(mime);
  ADD_FLAG(old);
  ADD_FLAG(purge);
  ADD_FLAG(quasi_deleted);
  ADD_FLAG(read);
  ADD_FLAG(recip_valid);
  ADD_FLAG(replied);
  ADD_FLAG(searched);
  ADD_FLAG(subject_changed);
  ADD_FLAG(superseded);
  ADD_FLAG(tagged);
  ADD_FLAG(threaded);
  ADD_FLAG(trash);
  ADD_FLAG(visible);
#undef ADD_FLAG
  log_debug1("\tFlags: %s", buf_is_empty(buf) ? "[NONE]" : buf_string(buf));

#define ADD_FLAG(F) add_flag(buf, (e->security & F), #F)
  buf_reset(buf);
  ADD_FLAG(SEC_ENCRYPT);
  ADD_FLAG(SEC_SIGN);
  ADD_FLAG(SEC_GOODSIGN);
  ADD_FLAG(SEC_BADSIGN);
  ADD_FLAG(SEC_PARTSIGN);
  ADD_FLAG(SEC_SIGNOPAQUE);
  ADD_FLAG(SEC_KEYBLOCK);
  ADD_FLAG(SEC_INLINE);
  ADD_FLAG(SEC_OPPENCRYPT);
  ADD_FLAG(SEC_AUTOCRYPT);
  ADD_FLAG(SEC_AUTOCRYPT_OVERRIDE);
  ADD_FLAG(APPLICATION_PGP);
  ADD_FLAG(APPLICATION_SMIME);
  ADD_FLAG(PGP_TRADITIONAL_CHECKED);
#undef ADD_FLAG
  log_debug1("\tSecurity: %s", buf_is_empty(buf) ? "[NONE]" : buf_string(buf));

  mutt_date_make_tls(arr, sizeof(arr), e->date_sent);
  log_debug1("\tSent: %s (%c%02u%02u)", arr,
             e->zoccident ? '-' : '+', e->zhours, e->zminutes);

  mutt_date_make_tls(arr, sizeof(arr), e->received);
  log_debug1("\tRecv: %s", arr);

  buf_pool_release(&buf);

  log_debug1("\tnum_hidden: %ld", e->num_hidden);
  log_debug1("\trecipient: %d", e->recipient);
  log_debug1("\toffset: %ld", e->offset);
  log_debug1("\tlines: %d", e->lines);
  log_debug1("\tindex: %d", e->index);
  log_debug1("\tmsgno: %d", e->msgno);
  log_debug1("\tvnum: %d", e->vnum);
  log_debug1("\tscore: %d", e->score);
  log_debug1("\tattach_total: %d", e->attach_total);
  // if (e->maildir_flags)
  //   log_debug1("\tmaildir_flags: %s", e->maildir_flags);

  // struct MuttThread *thread
  // struct Envelope *env
  // struct Body *content
  // TagList tags

  // void *edata
}

void dump_param_list(const ParameterList *pl)
{
  log_debug1("\tparameters");

  if (!pl)
  {
    log_debug1("\tNULL pointer");
    return;
  }

  if (g_queue_is_empty(pl))
  {
    log_debug1("\tempty");
    return;
  }

  for (GList *np = pl->head; np != NULL; np = np->next)
  {
    struct Parameter *p = np->data;
    log_debug1("\t\t%s = %s", NONULL(p->attribute), NONULL(p->value));
  }
}

void dump_body(const struct Body *body)
{
  log_debug1("Body");

  if (!body)
  {
    log_debug1("\tNULL pointer");
    return;
  }

  struct Buffer *buf = buf_pool_get();
  char arr[256];

#define ADD_FLAG(F) add_flag(buf, body->F, #F)
  ADD_FLAG(attach_qualifies);
  ADD_FLAG(badsig);
  ADD_FLAG(deleted);
  ADD_FLAG(force_charset);
  ADD_FLAG(goodsig);
#ifdef USE_AUTOCRYPT
  ADD_FLAG(is_autocrypt);
#endif
  ADD_FLAG(noconv);
  ADD_FLAG(tagged);
  ADD_FLAG(unlink);
  ADD_FLAG(use_disp);
  ADD_FLAG(warnsig);
#undef ADD_FLAG
  log_debug1("\tFlags: %s", buf_is_empty(buf) ? "[NONE]" : buf_string(buf));

#define OPT_STRING(S)                                                          \
  if (body->S)                                                                 \
  log_debug1("\t%s: %s", #S, body->S)
  OPT_STRING(charset);
  OPT_STRING(description);
  OPT_STRING(d_filename);
  OPT_STRING(filename);
  OPT_STRING(form_name);
  OPT_STRING(language);
  OPT_STRING(subtype);
  OPT_STRING(xtype);
#undef OPT_STRING

  log_debug1("\thdr_offset: %ld", body->hdr_offset);
  log_debug1("\toffset: %ld", body->offset);
  log_debug1("\tlength: %ld", body->length);
  log_debug1("\tattach_count: %d", body->attach_count);

  log_debug1("\tcontent type: %s", name_content_type(body->type));
  log_debug1("\tcontent encoding: %s", name_content_encoding(body->encoding));
  log_debug1("\tcontent disposition: %s",
             name_content_disposition(body->disposition));

  if (body->stamp != 0)
  {
    mutt_date_make_tls(arr, sizeof(arr), body->stamp);
    log_debug1("\tstamp: %s", arr);
  }

  dump_param_list(body->parameter);

  // struct Content *content;        ///< Detailed info about the content of the attachment.
  // struct Body *next;              ///< next attachment in the list
  // struct Body *parts;             ///< parts of a multipart or message/rfc822
  // struct Email *email;            ///< header information for message/rfc822
  // struct AttachPtr *aptr;         ///< Menu information, used in recvattach.c
  // struct Envelope *mime_headers;  ///< Memory hole protected headers

  if (body->next)
  {
    log_debug1("-NEXT-------------------------");
    dump_body(body->next);
  }
  if (body->parts)
  {
    log_debug1("-PARTS-------------------------");
    dump_body(body->parts);
  }
  if (body->next || body->parts)
    log_debug1("--------------------------");
  buf_pool_release(&buf);
}

void dump_attach(const struct AttachPtr *att)
{
  log_debug1("AttachPtr");

  if (!att)
  {
    log_debug1("\tNULL pointer");
    return;
  }

  struct Buffer *buf = buf_pool_get();

#define ADD_FLAG(F) add_flag(buf, att->F, #F)
  ADD_FLAG(unowned);
  ADD_FLAG(decrypted);
  ADD_FLAG(collapsed);
#undef ADD_FLAG

  if (att->fp)
    log_debug1("\tfp: %p (%d)", (void *) att->fp, fileno(att->fp));
  log_debug1("\tparent_type: %d", att->parent_type);
  log_debug1("\tlevel: %d", att->level);
  log_debug1("\tnum: %d", att->num);

  // struct Body *content; ///< Attachment
  buf_pool_release(&buf);
}

char body_name(const struct Body *b)
{
  if (!b)
    return '!';

  if (b->type == TYPE_MULTIPART)
    return '&';

  if (b->description)
    return b->description[0];

  if (b->filename)
  {
    const char *base = basename(b->filename);
    if (mutt_str_startswith(base, "neomutt-"))
      return '0';

    return base[0];
  }

  return '!';
}

void dump_body_next(struct Buffer *buf, const struct Body *b)
{
  if (!b)
    return;

  buf_addstr(buf, "<");
  for (; b; b = b->next)
  {
    buf_add_printf(buf, "%c", body_name(b));
    dump_body_next(buf, b->parts);
    if (b->next)
      buf_addch(buf, ',');
  }
  buf_addstr(buf, ">");
}

void dump_body_one_line(const struct Body *b)
{
  if (!b)
    return;

  struct Buffer *buf = buf_pool_get();
  buf_addstr(buf, "Body layout: ");
  dump_body_next(buf, b);

  log_message("%s", buf_string(buf));
  buf_pool_release(&buf);
}
