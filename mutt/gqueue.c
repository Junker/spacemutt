/**
 * @file
 * Extension for GQueue
 *
 * @authors
 * Copyright (C) 2024 Dmitrii Kosenkov
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

#include <stdbool.h>
#include <glib.h>
#include "mutt/buffer.h"
#include "string2.h"


void g_queue_copy_tail(GQueue *dst, const GQueue *src)
{
	GList *l;
	if (!src || !dst)
		return;
	for (l = src->head; l != NULL; l = l->next)
		g_queue_push_tail(dst, l->data);
}

/**
 * g_queue_write - Write a GQueue to a buffer
 * @param h    GQueue to write
 * @param buf  Buffer for the list
 *
 * Elements separated by a space.  References, and In-Reply-To, use this
 * format.
 */
size_t g_queue_write(const GQueue *h, struct Buffer *buf)
{
  if (!buf || !h)
    return 0;

  for (GList *np = h->head; np != NULL; np = np->next)
  {
    buf_addstr(buf, np->data);
    if (np->next)
      buf_addstr(buf, " ");
  }

  return buf_len(buf);
}

bool g_queue_equal(const GQueue *ah, const GQueue *bh)
{
  if (!ah || !bh || (ah->length != bh->length))
    return false;

  GList *a = ah->head;
  GList *b = bh->head;

  while (a && b)
  {
    if (!mutt_str_equal(a->data, b->data))
      return false;

    a = a->next;
    b = b->next;
  }

  return true;
}
