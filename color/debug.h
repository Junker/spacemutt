/**
 * @file
 * Colour Debugging
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

#ifndef MUTT_COLOR_DEBUG_H
#define MUTT_COLOR_DEBUG_H

#include <glib.h>
#include "config.h"
#include "curses2.h"

struct Buffer;

#ifdef USE_DEBUG_COLOR

const char *color_log_color(color_t fg, color_t bg);

void curses_color_dump(struct CursesColor *cc, const char *prefix);

void ansi_colors_dump  (struct Buffer *buf);
void curses_colors_dump(struct Buffer *buf);
void merged_colors_dump(struct Buffer *buf);

/* TEMP : DISABLED CUZ CALED FROM log_writer_curses */
/* #define log_color_debug(...) log_debug5(__VA_ARGS__) ///< @ingroup logging_api */
#define log_color_debug(...)

#else

static inline const char *color_log_color(color_t fg, color_t bg) { return ""; }

static inline void curses_color_dump(struct CursesColor *cc, const char *prefix) {}

static inline void ansi_colors_dump  (struct Buffer *buf) {}
static inline void curses_colors_dump(struct Buffer *buf) {}
static inline void merged_colors_dump(struct Buffer *buf) {}

static inline int color_debug(GLogLevelFlags level, const char *format, ...) { return 0; }

#endif

#endif /* MUTT_COLOR_DEBUG_H */
