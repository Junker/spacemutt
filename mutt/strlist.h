/**
 * @file
 * A separated list of strings
 *
 * @authors
 * Copyright (C) 2019-2023 Richard Russon <rich@flatcap.org>
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

#ifndef MUTT_MUTT_STRLIST_H
#define MUTT_MUTT_STRLIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <glib.h>

struct Buffer;

/**
 * struct StrList - String list
 */
struct StrList
{
  GSList *head;         ///< List containing values
  size_t count;         ///< Number of values in list
  uint32_t flags;       ///< Flags controlling list, e.g. #D_STRLIST_SEP_SPACE
};

struct StrList *strlist_add_string    (struct StrList *list, const char *str);
struct StrList *strlist_dup           (const struct StrList *list);
bool            strlist_equal         (const struct StrList *a, const struct StrList *b);
void            strlist_free          (struct StrList **ptr);
bool            strlist_is_empty      (const struct StrList *list);
bool            strlist_is_member     (const struct StrList *list, const char *str);
struct StrList *strlist_new           (uint32_t flags);
struct StrList *strlist_parse         (const char *str, uint32_t flags);
struct StrList *strlist_remove_string (struct StrList *list, const char *str);
int             strlist_to_buffer     (const struct StrList *list, struct Buffer *buf);

#endif /* MUTT_MUTT_STRLIST_H */
