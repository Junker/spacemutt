/**
 * @file
 * NeoMutt Logging
 *
 * @authors
 * Copyright (C) 2018-2023 Richard Russon <rich@flatcap.org>
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

#ifndef MUTT_MUTT_LOGGING_H
#define MUTT_MUTT_LOGGING_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <glib.h>
#include "mutt/lib.h"

struct ConfigDef;
struct ConfigSet;

GLogWriterOutput log_writer_curses(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data);

short log_level_to_debug_level(GLogLevelFlags level);
GLogLevelFlags debug_level_to_log_level(short debug_level);

void mutt_log_prep(void);
int  mutt_log_start(void);
void mutt_log_stop(void);
int  mutt_log_set_level(GLogLevelFlags level, bool verbose);
int  mutt_log_set_file(const char *file);

int  main_log_observer(struct NotifyCallback *nc);
int  level_validator(const struct ConfigSet *cs, const struct ConfigDef *cdef, intptr_t value, struct Buffer *err);

void mutt_clear_error(void);

#endif /* MUTT_MUTT_LOGGING_H */
