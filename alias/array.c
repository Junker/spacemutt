/**
 * @file
 * Array of Alias Views
 *
 * @authors
 * Copyright (C) 2020-2022 Richard Russon <rich@flatcap.org>
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
 * @page alias_array Array of Alias Views
 *
 * Array of Alias Views
 */

#include "config.h"
#include <stdbool.h>
#include <stddef.h>
#include "mutt/lib.h"
#include "gui.h"

struct Alias;

/**
 * alias_array_alias_add - Add an Alias to the AliasViewArray
 * @param ava Array of Aliases
 * @param alias Alias to add
 * @retval num Size of array
 * @retval -1  Error
 *
 * @note The Alias is wrapped in an AliasView
 * @note Call alias_array_sort() to sort and reindex the AliasViewArray
 */
int alias_array_alias_add(AliasViewArray *ava, struct Alias *alias)
{
  if (!ava || !alias)
    return -1;

  
  struct AliasView *av = g_new0(struct AliasView, 1);
  av->num = 0;
  av->orig_seq = ava->len;
  av->is_tagged = false;
  av->is_deleted = false;
  av->is_visible = true;
  av->alias = alias;

  g_ptr_array_add(ava, av);
  return ava->len;
}

/**
 * alias_array_alias_delete - Delete an Alias from the AliasViewArray
 * @param ava    Array of Aliases
 * @param alias Alias to remove
 * @retval num Size of array
 * @retval -1  Error
 *
 * @note Call alias_array_sort() to sort and reindex the AliasViewArray
 */
int alias_array_alias_delete(AliasViewArray *ava, const struct Alias *alias)
{
  if (!ava || !alias)
    return -1;

  for (guint i = 0; i < ava->len; i++)
  {
    struct AliasView *avp = g_ptr_array_index(ava, i);
    if (avp->alias != alias)
      continue;

    g_ptr_array_remove_index(ava, i);
    break;
  }

  return ava->len;
}

/**
 * alias_array_count_visible - Count number of visible Aliases
 * @param ava Array of Aliases
 */
int alias_array_count_visible(AliasViewArray *ava)
{
  int count = 0;

  for (guint i = 0; i < ava->len; i++)
  {
    struct AliasView *avp = g_ptr_array_index(ava, i);
    if (avp->is_visible)
      count++;
  }

  return count;
}
