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

#define GQUEUE_SWAP(q1, q2) do {				\
	GList *swap_head = (q1)->head;		\
	GList *swap_tail = (q1)->tail;		\
	guint swap_length = (q1)->length;		\
  (q1)->head = (q2)->head;			\
  (q1)->tail = (q2)->tail;			\
  (q1)->length = (q2)->length;    \
  (q2)->head = swap_head;				\
  (q2)->tail = swap_tail;					\
  (q2)->length = swap_length;					\
if (g_queue_is_empty((q1)))					\
		(q1)->tail = (q1)->head;		\
if (g_queue_is_empty((q2)))					\
		(q2)->tail = (q2)->head;		\
} while (0)

void g_queue_copy_tail(GQueue *dst, const GQueue *src);
size_t g_queue_write(const GQueue *h, struct Buffer *buf);
bool g_queue_equal_custom(GQueue *queue1, GQueue *queue2, GCompareFunc cmp_func);
