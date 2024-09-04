/**
 * @file
 * A separated list of strings
 *
 * @authors
 * Copyright (C) 2019-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2020 Pietro Cerutti <gahr@gahr.ch>
 * Copyright (C) 2020 Yousef Akbar <yousef@yhakbar.com>
 * Copyright (C) 2023 наб <nabijaczleweli@nabijaczleweli.xyz>
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
 * @page mutt_slist A separated list of strings
 *
 * A separated list of strings
 *
 * The following unused functions were removed:
 * - slist_add_list()
 * - slist_empty()
 */

#include "config.h"
#include <stddef.h>
#include "config/types.h"
#include "strlist.h"
#include "buffer.h"
#include "memory.h"
#include "string2.h"
#include "mutt/gslist.h"


/**
 * strlist_new - Create a new string list
 * @param flags Flag to set, e.g. #D_STRLIST_SEP_COMMA
 * @retval ptr New string list
 */
struct StrList *strlist_new(uint32_t flags)
{
  struct StrList *list = mutt_mem_calloc(1, sizeof(*list));
  list->flags = flags;
  list->head = NULL;

  return list;
}

/**
 * strlist_add_string - Add a string to a list
 * @param list List to modify
 * @param str  String to add
 * @retval ptr Modified list
 */
struct StrList *strlist_add_string(struct StrList *list, const char *str)
{
  if (!list)
    return NULL;

  if (str && (str[0] == '\0'))
    str = NULL;

  if (!str && !(list->flags & D_STRLIST_ALLOW_EMPTY))
    return list;

  list->head = g_slist_append(list->head, mutt_str_dup(str));
  list->count++;

  return list;
}

/**
 * strlist_equal - Compare two string lists
 * @param a First list
 * @param b Second list
 * @retval true They are identical
 */
bool strlist_equal(const struct StrList *a, const struct StrList *b)
{
  if (!a && !b) /* both empty */
    return true;
  if (!a ^ !b) /* one is empty, but not the other */
    return false;
  if (a->count != b->count)
    return false;

  return g_slist_equal_custom(a->head, b->head, (GCompareFunc)mutt_str_cmp);
}

/**
 * strlist_dup - Create a copy of a StrList object
 * @param list StrList to duplicate
 * @retval ptr New StrList object
 */
struct StrList *strlist_dup(const struct StrList *list)
{
  if (!list)
    return NULL;

  struct StrList *list_new = strlist_new(list->flags);

  list_new->head = g_slist_copy_deep(list->head, (GCopyFunc)mutt_str_dup, NULL);
  list_new->count = list->count;
  return list_new;
}

/**
 * strlist_free - Free a StrList object
 * @param ptr StrList to free
 */
void strlist_free(struct StrList **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct StrList *slist = *ptr;
  g_slist_free_full(g_steal_pointer(&slist->head), g_free);

  FREE(ptr);
}

/**
 * strlist_is_empty - Is the strlist empty?
 * @param list List to check
 * @retval true List is empty
 */
bool strlist_is_empty(const struct StrList *list)
{
  if (!list)
    return true;

  return list->count == 0;
}

/**
 * strlist_is_member - Is a string a member of a list?
 * @param list List to modify
 * @param str  String to find
 * @retval true String is in the list
 */
bool strlist_is_member(const struct StrList *list, const char *str)
{
  if (!list)
    return false;

  if (!str && !(list->flags & D_STRLIST_ALLOW_EMPTY))
    return false;

  for (GSList *np = list->head; np != NULL; np = np->next)
  {
    if (mutt_str_equal(np->data, str))
      return true;
  }
  return false;
}

/**
 * strlist_parse - Parse a list of strings into a list
 * @param str   String of strings
 * @param flags Flags, e.g. #D_STRLIST_ALLOW_EMPTY
 * @retval ptr New Slist object
 */
struct StrList *strlist_parse(const char *str, uint32_t flags)
{
  char *src = mutt_str_dup(str);
  if (!src && !(flags & D_STRLIST_ALLOW_EMPTY))
    return NULL;

  char sep = ' ';
  if ((flags & D_STRLIST_SEP_MASK) == D_STRLIST_SEP_COMMA)
    sep = ',';
  else if ((flags & D_STRLIST_SEP_MASK) == D_STRLIST_SEP_COLON)
    sep = ':';

  struct StrList *list = mutt_mem_calloc(1, sizeof(struct StrList));
  list->flags = flags;

  if (!src)
    return list;

  char *start = src;
  for (char *p = start; *p; p++)
  {
    if ((p[0] == '\\') && (p[1] != '\0'))
    {
      p++;
      continue;
    }

    if (p[0] == sep)
    {
      p[0] = '\0';
      if (strlist_is_member(list, start))
      {
        start = p + 1;
        continue;
      }
      list->head = g_slist_append(list->head, mutt_str_dup(start));
      list->count++;
      start = p + 1;
    }
  }

  if (!strlist_is_member(list, start))
  {
    list->head = g_slist_append(list->head, mutt_str_dup(start));
    list->count++;
  }

  FREE(&src);
  return list;
}

/**
 * strlist_remove_string - Remove a string from a list
 * @param list List to modify
 * @param str  String to remove
 * @retval ptr Modified list
 */
struct StrList *strlist_remove_string(struct StrList *list, const char *str)
{
  if (!list)
    return NULL;
  if (!str && !(list->flags & D_STRLIST_ALLOW_EMPTY))
    return list;

  for (GSList *np = list->head; np != NULL;)
  {
    GSList *next = np->next;
    if (mutt_str_equal(np->data, str))
    {
      FREE(&np->data);
      list->head = g_slist_delete_link(list->head, np);
      list->count--;
      break;
    }
    np = next;
  }
  return list;
}

/**
 * strlist_to_buffer - Export a StrList to a Buffer
 * @param list List to export
 * @param buf  Buffer for the results
 * @retval num Number of strings written to Buffer
 */
int strlist_to_buffer(const struct StrList *list, struct Buffer *buf)
{
  if (!list || !buf || (list->count == 0))
    return 0;

  for (GSList *np = list->head; np != NULL; np = np->next)
  {
    buf_addstr(buf, np->data);
    if (np->next)
    {
      const int sep = (list->flags & D_STRLIST_SEP_MASK);
      if (sep == D_STRLIST_SEP_COMMA)
        buf_addch(buf, ',');
      else if (sep == D_STRLIST_SEP_COLON)
        buf_addch(buf, ':');
      else
        buf_addch(buf, ' ');
    }
  }

  return list->count;
}
