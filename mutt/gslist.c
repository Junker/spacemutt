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

#include <stdbool.h>
#include <glib.h>
#include "mutt/buffer.h"
#include "string2.h"

/**
 * g_slist_match - Is the string in the list (see notes)
 * @param s String to match
 * @param h Head of the List
 * @retval true String matches a List item (or List contains "*")
 *
 * This is a very specific function.  It searches a List of strings looking for
 * a match.  If the list contains a string "*", then it match any input string.
 *
 * @note The strings are compared to the length of the List item, e.g.
 *       List: "Red" matches Param: "Redwood", but not the other way around.
 * @note The case of the strings is ignored.
 */
bool g_slist_match(GSList *h, const char *s)
{
  if (!h)
    return false;

  for (GSList *np = h; np != NULL; np = np->next)
  {
    if ((((char*)np->data)[0] == '*') || mutt_istr_startswith(s, np->data))
      return true;
  }
  return false;
}

/**
 * g_slist_find_str - Find a string in a GSList
 * @param list    Head of the List
 * @param data String to find
 * @retval GSList containing the string
 * @retval NULL The string isn't found
 */
GSList *g_slist_find_str(GSList *list, const char *data, bool ignore_case)
{
  if (!data)
    return NULL;

  return g_slist_find_custom(list, data, (GCompareFunc)(ignore_case ? mutt_istr_cmp : mutt_str_cmp));
}


GSList* g_slist_insert_after(GSList *slist, GSList *sibling, gpointer data) {
    if (sibling == NULL || slist == NULL) {
        return slist;
    }
    GSList *new_node = g_slist_alloc();
    new_node->data = data;
    new_node->next = sibling->next;
    sibling->next = new_node;
    return slist;
}

bool g_slist_equal_custom(GSList *list1, GSList *list2, GCompareFunc cmp_func)
{
  while (list1 != NULL && list2 != NULL)
  {
    if (cmp_func(list1->data, list2->data) != 0)
    {
      return false;
    }
    list1 = list1->next;
    list2 = list2->next;
  }

  return list1 == list2;
}
