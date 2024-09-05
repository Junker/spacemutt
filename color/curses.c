/**
 * @file
 * Curses Colour
 *
 * @authors
 * Copyright (C) 2021-2023 Richard Russon <rich@flatcap.org>
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
 * @page color_curses Curses Colour
 *
 * A wrapper that represents a colour in Curses.
 */

#include "config.h"
#include <stdbool.h>
#include <stddef.h>
#include "mutt/lib.h"
#include "gui/lib.h"
#include "color.h"
#include "curses2.h"
#include "mutt/gslist.h"
#include "debug.h"

CursesColorList *CursesColors = NULL; ///< List of all Curses colours
int NumCursesColors; ///< Number of ncurses colours left to allocate

/**
 * curses_colors_init - Initialise the Curses colours
 */
void curses_colors_init(void)
{
  log_color_debug("init CursesColors\n");
  NumCursesColors = 0;
}

/**
 * curses_colors_find - Find a Curses colour by foreground/background
 * @param fg Foreground colour
 * @param bg Background colour
 * @retval ptr Curses colour
 */
struct CursesColor *curses_colors_find(color_t fg, color_t bg)
{
  for (GSList *np = CursesColors; np != NULL; np = np->next)
  {
    struct CursesColor *cc = np->data;
    if ((cc->fg == fg) && (cc->bg == bg))
    {
      curses_color_dump(cc, "find");
      return cc;
    }
  }

  return NULL;
}

/**
 * curses_color_init - Initialise a new Curses colour
 * @param fg Foreground colour
 * @param bg Background colour
 * @retval num Index of Curses colour
 */
static int curses_color_init(color_t fg, color_t bg)
{
  log_color_debug("find lowest index\n");
  int index = 16;
  for (GSList *np = CursesColors; np != NULL; np = np->next)
  {
    struct CursesColor *cc = np->data;
    if (cc->index == index)
      index++;
    else
      break;
  }
  log_color_debug("lowest index = %d\n", index);
  if (index >= COLOR_PAIRS)
  {
    if (COLOR_PAIRS > 0)
    {
      static bool warned = false;
      if (!warned)
      {
        log_fault(_("Too many colors: %d / %d"), index, COLOR_PAIRS);
        warned = true;
      }
    }
    return 0;
  }

#ifdef SPACEMUTT_DIRECT_COLORS
  int rc = init_extended_pair(index, fg, bg);
  log_color_debug("init_extended_pair(%d,%d,%d) -> %d\n", index, fg, bg, rc);
#else
  int rc = init_pair(index, fg, bg);
  log_color_debug("init_pair(%d,%d,%d) -> %d\n", index, fg, bg, rc);
#endif

  return index;
}

/**
 * curses_color_free - Free a CursesColor
 * @param ptr CursesColor to be freed
 */
void curses_color_free(struct CursesColor **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct CursesColor *cc = *ptr;

  cc->ref_count--;
  if (cc->ref_count > 0)
  {
    curses_color_dump(cc, "curses rc--");
    *ptr = NULL;
    return;
  }

  curses_color_dump(cc, "curses free");
  CursesColors = g_slist_remove(CursesColors, cc);
  NumCursesColors--;
  log_color_debug("CursesColors: %d\n", NumCursesColors);
  FREE(ptr);
}

/**
 * curses_color_new - Create a new CursesColor
 * @param fg Foreground colour
 * @param bg Background colour
 * @retval ptr New CursesColor
 *
 * If the colour already exists, this function will return a pointer to the
 * object (and increase its ref-count).
 */
struct CursesColor *curses_color_new(color_t fg, color_t bg)
{
  log_color_debug("fg %d, bg %d\n", fg, bg);
  if ((fg == COLOR_DEFAULT) && (bg == COLOR_DEFAULT))
  {
    log_color_debug("both unset\n");
    return NULL;
  }

  struct CursesColor *cc = curses_colors_find(fg, bg);
  if (cc)
  {
    cc->ref_count++;
    curses_color_dump(cc, "curses rc++");
    return cc;
  }

  log_color_debug("new curses\n");
  int index = curses_color_init(fg, bg);
  if (index == 0)
    return NULL;

  struct CursesColor *cc_new = g_new0(struct CursesColor, 1);
  NumCursesColors++;
  log_color_debug("CursesColor %p\n", (void *) cc_new);
  cc_new->fg = fg;
  cc_new->bg = bg;
  cc_new->ref_count = 1;
  cc_new->index = index;

  // insert curses colour
  for (GSList *np = CursesColors; np != NULL; np = np->next)
  {
    cc = np->data;
    if (cc->index > index)
    {
      log_color_debug("insert\n");
      CursesColors = g_slist_insert_before(CursesColors, np, cc_new);
      goto done;
    }
  }

  CursesColors = g_slist_append(CursesColors, cc_new);
  log_color_debug("tail\n");

done:
  curses_color_dump(cc_new, "curses new");
  log_color_debug("CursesColors: %d\n", NumCursesColors);
  return cc_new;
}
