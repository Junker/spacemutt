/**
 * @file
 * Log everything to the terminal
 *
 * @authors
 * Copyright (C) 2023 Richard Russon <rich@flatcap.org>
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
 * @page debug_logging Log everything to the terminal
 *
 * Log everything to the terminal
 */

#include "config.h"
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mutt/lib.h"
#include "lib.h"


/**
 * log_disp_debug - Display a log line on screen
 */
GLogWriterOutput log_writer_debug(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data)
{
  char *msg = log_writer_format_fields(level, fields, n_fields);
  fputs(msg, stdout);
  FREE(&msg);

  return G_LOG_WRITER_HANDLED;
}
