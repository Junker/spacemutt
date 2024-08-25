/**
 * @file
 * Window reflowing
 *
 * @authors
 * Copyright (C) 2019-2022 Richard Russon <rich@flatcap.org>
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
 * @page gui_reflow Window reflowing
 *
 * Window reflowing
 */

#include "config.h"
#include <stddef.h>
#include "mutt/lib.h"
#include "reflow.h"
#include "mutt_window.h"

/**
 * window_reflow_horiz - Reflow Windows using all the available horizontal space
 * @param win Window
 */
static void window_reflow_horiz(struct MuttWindow *win)
{
  if (!win)
    return;

  int max_count = 0;
  int space = win->state.cols;

  // Pass one - minimal allocation
  for (GList *np = win->children->head; np != NULL; np = np->next)
  {
    struct MuttWindow *mw = np->data;
    if (!mw->state.visible)
      continue;

    switch (mw->size)
    {
      case MUTT_WIN_SIZE_FIXED:
      {
        const int avail = MIN(space, mw->req_cols);
        mw->state.cols = avail;
        mw->state.rows = win->state.rows;
        space -= avail;
        break;
      }
      case MUTT_WIN_SIZE_MAXIMISE:
      {
        mw->state.cols = 1;
        mw->state.rows = win->state.rows;
        max_count++;
        space -= 1;
        break;
      }
      case MUTT_WIN_SIZE_MINIMISE:
      {
        mw->state.rows = win->state.rows;
        mw->state.cols = win->state.cols;
        mw->state.row_offset = win->state.row_offset;
        mw->state.col_offset = win->state.col_offset;
        window_reflow(mw);
        space -= mw->state.cols;
        break;
      }
    }
  }

  // Pass two - sharing
  if ((max_count > 0) && (space > 0))
  {
    int alloc = (space + max_count - 1) / max_count;
    for (GList *np = win->children->head; np != NULL; np = np->next)
    {
        struct MuttWindow *mw = np->data;
      if (space == 0)
        break;
      if (!mw->state.visible)
        continue;
      if (mw->size != MUTT_WIN_SIZE_MAXIMISE)
        continue;

      alloc = MIN(space, alloc);
      mw->state.cols += alloc;
      space -= alloc;
    }
  }

  // Pass three - position and recursion
  int col = win->state.col_offset;
  for (GList *np = win->children->head; np != NULL; np = np->next)
  {
    struct MuttWindow *mw = np->data;
    if (!mw->state.visible)
      continue;

    mw->state.col_offset = col;
    mw->state.row_offset = win->state.row_offset;
    col += mw->state.cols;

    window_reflow(mw);
  }

  if ((space > 0) && (win->size == MUTT_WIN_SIZE_MINIMISE))
  {
    win->state.cols -= space;
  }
}

/**
 * window_reflow_vert - Reflow Windows using all the available vertical space
 * @param win Window
 */
static void window_reflow_vert(struct MuttWindow *win)
{
  if (!win)
    return;

  int max_count = 0;
  int space = win->state.rows;

  // Pass one - minimal allocation
  for (GList *np = win->children->head; np != NULL; np = np->next)
  {
    struct MuttWindow *mw = np->data;
    if (!mw->state.visible)
      continue;

    switch (mw->size)
    {
      case MUTT_WIN_SIZE_FIXED:
      {
        const int avail = MIN(space, mw->req_rows);
        mw->state.rows = avail;
        mw->state.cols = win->state.cols;
        space -= avail;
        break;
      }
      case MUTT_WIN_SIZE_MAXIMISE:
      {
        mw->state.rows = 1;
        mw->state.cols = win->state.cols;
        max_count++;
        space -= 1;
        break;
      }
      case MUTT_WIN_SIZE_MINIMISE:
      {
        mw->state.rows = win->state.rows;
        mw->state.cols = win->state.cols;
        mw->state.row_offset = win->state.row_offset;
        mw->state.col_offset = win->state.col_offset;
        window_reflow(mw);
        space -= mw->state.rows;
        break;
      }
    }
  }

  // Pass two - sharing
  if ((max_count > 0) && (space > 0))
  {
    int alloc = (space + max_count - 1) / max_count;
    for (GList *np = win->children->head; np != NULL; np = np->next)
    {
      struct MuttWindow *mw = np->data;
      if (space == 0)
        break;
      if (!mw->state.visible)
        continue;
      if (mw->size != MUTT_WIN_SIZE_MAXIMISE)
        continue;

      alloc = MIN(space, alloc);
      mw->state.rows += alloc;
      space -= alloc;
    }
  }

  // Pass three - position and recursion
  int row = win->state.row_offset;
  for (GList *np = win->children->head; np != NULL; np = np->next)
  {
    struct MuttWindow *mw = np->data;
    if (!mw->state.visible)
      continue;

    mw->state.row_offset = row;
    mw->state.col_offset = win->state.col_offset;
    row += mw->state.rows;

    window_reflow(mw);
  }

  if ((space > 0) && (win->size == MUTT_WIN_SIZE_MINIMISE))
  {
    win->state.rows -= space;
  }
}

/**
 * window_reflow - Reflow Windows
 * @param win Root Window
 *
 * Using the rules coded into the Windows, such as Fixed or Maximise, allocate
 * space to a set of nested Windows.
 */
void window_reflow(struct MuttWindow *win)
{
  if (!win)
    return;
  if (win->orient == MUTT_WIN_ORIENT_VERTICAL)
    window_reflow_vert(win);
  else
    window_reflow_horiz(win);
}
