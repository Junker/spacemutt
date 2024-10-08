/**
 * @file
 * Extension for GSList
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

#ifndef MUTT_GSLIST_H
#define MUTT_GSLIST_H

#include <stdbool.h>
#include <glib.h>

bool g_slist_match(GSList *h, const char *s);
GSList *g_slist_find_str(GSList *list, const char *data, bool ignore_case);
GSList* g_slist_insert_after(GSList *slist, GSList *sibling, gpointer data);
bool g_slist_equal_custom(GSList *list1, GSList *list2, GCompareFunc cmp_func);

#endif /* MUTT_GSLIST_H */
