/**
 * @file
 * Representation of an email
 *
 * @authors
 * Copyright (C) 2018-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2019-2020 Pietro Cerutti <gahr@gahr.ch>
 * Copyright (C) 2020 Matthew Hughes <matthewhughes934@gmail.com>
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
 * @page email_email Email object
 *
 * Representation of an email
 */

#include "config.h"
#include <stdbool.h>
#include <string.h>
#include "mutt/lib.h"
#include "email.h"
#include "body.h"
#include "envelope.h"
#include "tags.h"

void nm_edata_free(void **ptr);

/**
 * email_free - Free an Email
 * @param[out] ptr Email to free
 */
void email_free(struct Email **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct Email *e = *ptr;

  log_notify("NT_EMAIL_DELETE: %p", (void *) e);
  struct EventEmail ev_e = { 1, &e };
  notify_send(e->notify, NT_EMAIL, NT_EMAIL_DELETE, &ev_e);

  if (e->edata_free && e->edata)
    e->edata_free(&e->edata);

  mutt_env_free(&e->env);
  mutt_body_free(&e->body);
  FREE(&e->tree);
  FREE(&e->path);
#ifdef USE_NOTMUCH
  nm_edata_free(&e->nm_edata);
#endif
  driver_tags_free(e->tags);
  notify_free(&e->notify);

  FREE(ptr);
}

/**
 * email_new - Create a new Email
 * @retval ptr Newly created Email
 */
struct Email *email_new(void)
{
  static size_t sequence = 0;

  struct Email *e = g_new0(struct Email, 1);
  e->tags = NULL;
  e->visible = true;
  e->sequence = sequence++;
  e->notify = notify_new();

  return e;
}

/**
 * email_cmp_strict - Strictly compare message emails
 * @param e1 First Email
 * @param e2 Second Email
 * @retval true Emails are strictly identical
 */
bool email_cmp_strict(const struct Email *e1, const struct Email *e2)
{
  if (e1 && e2)
  {
    if ((e1->received != e2->received) || (e1->date_sent != e2->date_sent) ||
        (e1->body->length != e2->body->length) || (e1->lines != e2->lines) ||
        (e1->zhours != e2->zhours) || (e1->zminutes != e2->zminutes) ||
        (e1->zoccident != e2->zoccident) || (e1->mime != e2->mime) ||
        !mutt_env_cmp_strict(e1->env, e2->env) || !mutt_body_cmp_strict(e1->body, e2->body))
    {
      return false;
    }
    return true;
  }
  else
  {
    return (!e1 && !e2);
  }
}

/**
 * email_size - Compute the size of an email
 * @param e Email
 * @retval num Size of the email, in bytes
 */
size_t email_size(const struct Email *e)
{
  if (!e || !e->body)
    return 0;
  return e->body->length + e->body->offset - e->body->hdr_offset;
}

/**
 * header_find - Find a header, matching on its field, in a list of headers
 * @param hdrlist List of headers to search
 * @param header  The header to search for
 * @retval ptr    The node in the list matching the header
 * @retval NULL   No matching header is found
 *
 * The header should either of the form "X-Header:" or "X-Header: value"
 */
GList *header_find(GQueue *hdrlist, const char *header)
{
  const char *key_end = strchr(header, ':');
  if (!key_end)
    return NULL;

  const int keylen = key_end - header + 1;

  for (GList *n = hdrlist->head; n != NULL; n = n->next)
  {
    if (mutt_istrn_equal(n->data, header, keylen))
      return n;
  }
  return NULL;
}

/**
 * header_add - Add a header to a list
 * @param hdrlist List of headers to search
 * @param header  String to set as the header
 * @retval ptr    The created header
 */
GList *header_add(GQueue *hdrlist, const char *header)
{
  g_queue_push_tail(hdrlist, mutt_str_dup(header));

  return hdrlist->tail;
}

/**
 * header_update - Update an existing header
 * @param hdr     The header to update
 * @param header  String to update the header with
 * @retval ptr    The updated header
 */
GList *header_update(GList *hdr, const char *header)
{
  g_free(hdr->data);
  hdr->data = mutt_str_dup(header);

  return hdr;
}

/**
 * header_set - Set a header value in a list
 * @param hdrlist List of headers to search
 * @param header  String to set the value of the header to
 * @retval ptr    The updated or created header
 *
 * If a header exists with the same field, update it, otherwise add a new header.
 */
GList *header_set(GQueue *hdrlist, const char *header)
{
  GList *n = header_find(hdrlist, header);

  return n ? header_update(n, header) : header_add(hdrlist, header);
}

/**
 * header_free - Free and remove a header from a header list
 * @param hdrlist List to free the header from
 * @param target  The header to free
 */
void header_free(GQueue *hdrlist, GList *target)
{
  g_free(target->data);
  g_queue_delete_link(hdrlist, target);
}
