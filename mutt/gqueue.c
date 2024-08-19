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

bool g_queue_equal_custom(GQueue *queue1, GQueue *queue2, GCompareFunc cmp_func)
{
  if (queue1->length != queue2->length)
  {
    return false;
  }

  GList *iter1 = queue1->head;
  GList *iter2 = queue2->head;

  while (iter1 != NULL && iter2 != NULL)
  {
    if (cmp_func(iter1->data, iter2->data) != 0)
    {
      return false;
    }
    iter1 = iter1->next;
    iter2 = iter2->next;
  }

  return true;
}

/**
 * g_queue_find_str - Find a string in a GQueue
 * @param list    Head of the List
 * @param data String to find
 * @retval GList containing the string
 * @retval NULL The string isn't found
 */
GList *g_queue_find_str(GQueue *queue, const char *data, bool ignore_case)
{
  if (!data)
    return NULL;

  return g_queue_find_custom(queue, data, (GCompareFunc)(ignore_case ? mutt_istr_cmp : mutt_str_cmp));
}

/**
 * g_queue_remove_all_custom - Remove all elements whose data equals data from queue compared by supplied function 
 * @param  queue   GQueue
 * @param  data    data to delete
 * @retval guint  The number of elements removed from queue.
 */

guint g_queue_remove_all_custom(GQueue *queue, gpointer data, GCompareFunc cmp_func)
{
  GList *current = queue->head;
  guint cnt = 0;
  while (current != NULL)
  {
    GList *next = current->next;
    if (cmp_func(current->data, data) == 0)
    {
      g_queue_delete_link(queue, current);
      cnt++;
    }
    current = next;
  }
  return cnt;
}
