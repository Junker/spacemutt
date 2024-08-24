/**
 * @file
 * Logging Dispatcher
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

#ifndef MUTT_MUTT_LOGGING2_H
#define MUTT_MUTT_LOGGING2_H

#include <stdbool.h>
#include <stdio.h>
#include <glib.h>
#include <time.h>



/// Log lines longer than this will be truncated
#define LOG_LINE_MAX_LEN 10240


#define LOG_LEVEL_FAULT 1 << G_LOG_LEVEL_USER_SHIFT
#define LOG_LEVEL_DEBUG1 G_LOG_LEVEL_DEBUG + (1 << (G_LOG_LEVEL_USER_SHIFT + 1))
#define LOG_LEVEL_DEBUG2 G_LOG_LEVEL_DEBUG + (1 << (G_LOG_LEVEL_USER_SHIFT + 2))
#define LOG_LEVEL_DEBUG3 G_LOG_LEVEL_DEBUG + (1 << (G_LOG_LEVEL_USER_SHIFT + 3))
#define LOG_LEVEL_DEBUG4 G_LOG_LEVEL_DEBUG + (1 << (G_LOG_LEVEL_USER_SHIFT + 4))
#define LOG_LEVEL_DEBUG5 G_LOG_LEVEL_DEBUG + (1 << (G_LOG_LEVEL_USER_SHIFT + 5))
#define LOG_LEVEL_NOTIFY 1 << (G_LOG_LEVEL_USER_SHIFT + 6)


#define DEBUG_LEVEL_MAX 5

extern GLogWriterFunc MuttLogWriter;
/**
 * struct LogLine - A Log line
 */
typedef struct
{
  time_t time;                   ///< Timestamp of the message
  GLogLevelFlags level;
  const char *file;              ///< Source file
  const char *line;              ///< Line number in source file
  const char *func;              ///< C function
  char *message;                 ///< Message to be logged
  short err_no;
} LogLine;

#define log(LEVEL, ...)  g_log_structured_standard(G_LOG_DOMAIN, LEVEL, __FILE__, G_STRINGIFY(__LINE__), G_STRFUNC, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_notify(...)  log(LOG_LEVEL_NOTIFY, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_debug1(...)  log(LOG_LEVEL_DEBUG1, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_debug2(...)  log(LOG_LEVEL_DEBUG2, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_debug3(...)  log(LOG_LEVEL_DEBUG3, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_debug4(...)  log(LOG_LEVEL_DEBUG4, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_debug5(...)  log(LOG_LEVEL_DEBUG5, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_info(...)    log(G_LOG_LEVEL_INFO, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_message(...) log(G_LOG_LEVEL_MESSAGE, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_warning(...) log(G_LOG_LEVEL_WARNING, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_fault(...)   log(LOG_LEVEL_FAULT, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_critical(...) log(G_LOG_LEVEL_CRITICAL, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_error(...)   log(G_LOG_LEVEL_ERROR, __VA_ARGS__)    ///< @ingroup logging_api */
#define log_perror(...) g_log_structured(G_LOG_DOMAIN, LOG_LEVEL_FAULT, "ERRNO", errno, "CODE_FILE", __FILE__, "CODE_LINE", G_STRINGIFY(__LINE__), "CODE_FUNC", G_STRFUNC, "MESSAGE", __VA_ARGS__)

void logging_init();
gconstpointer log_get_field(const GLogField* fields, gsize n_fields, char *name);
gchar * log_writer_format_fields(GLogLevelFlags level, const GLogField *fields, gsize n_fields);
GLogWriterOutput log_writer_terminal(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data);
GLogWriterOutput log_writer_queue(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data);
GLogWriterOutput log_writer_file(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data);

int  log_queue_add(LogLine *ll);
void log_queue_empty(void);
void log_queue_flush(GLogWriterFunc disp);
int  log_queue_save(FILE *fp);
void log_queue_set_max_size(int size);

void log_file_close(bool verbose);
int  log_file_open(bool verbose);
bool log_file_running(void);
int  log_file_set_filename(const char *file, bool verbose);
int  log_file_set_level(GLogLevelFlags level, bool verbose);
void log_file_set_version(const char *version);

#endif /* MUTT_MUTT_LOGGING2_H */
