/**
 * @file
 * Logging Dispatcher
 *
 * @authors
 * Copyright (C) 2018-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2020 Pietro Cerutti <gahr@gahr.ch>
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
 * @page mutt_logging Logging Dispatcher
 *
 * Logging Dispatcher
 */

#include "config.h"
#include <errno.h>
#include <stdarg.h> // IWYU pragma: keep
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <glib.h>
#include "date.h"
#include "file.h"
#include "logging2.h"
#include "memory.h"
#include "message.h"
#include "string2.h"



static FILE *LogFileFP = NULL;      ///< Log file handle
static char *LogFileName = NULL;    ///< Log file name
static GLogLevelFlags LogFileLevel; ///< Log file level
static char *LogFileVersion = NULL; ///< Program version
GLogWriterFunc MuttLogWriter = log_writer_terminal;

/**
 * LogQueue - In-memory list of log lines
 */
static GQueue *LogQueue = NULL;
static int LogQueueMax = 0;   ///< Maximum number of entries in the log queue

/**
 * timestamp - Create a YYYY-MM-DD HH:MM:SS timestamp
 * @param stamp Unix time
 * @retval ptr Timestamp string
 *
 * If stamp is 0, then the current time will be used.
 *
 * @note This function returns a pointer to a static buffer.
 *       Do not free it.
 */
static const char *timestamp(time_t stamp)
{
  static char buf[23] = { 0 };
  static time_t last = 0;

  if (stamp == 0)
    stamp = mutt_date_now();

  if (stamp != last)
  {
    mutt_date_localtime_format(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", stamp);
    last = stamp;
  }

  return buf;
}


gconstpointer log_get_field(const GLogField* fields, gsize n_fields, char *name)
{
  size_t i;

  for (i = 0; i < n_fields; i++)
  {
    if (mutt_str_equal((fields + i)->key, name))
    {
      return (fields + i)->value;
    }
  }

  return NULL;
}

static const gchar* log_level_to_str(GLogLevelFlags lvl)
{
	switch (lvl & G_LOG_LEVEL_MASK)
  {
		case G_LOG_LEVEL_ERROR:
			return "EROR";
		case G_LOG_LEVEL_CRITICAL:
			return "CRIT";
		case G_LOG_LEVEL_WARNING:
			return "WARN";
		case G_LOG_LEVEL_MESSAGE:
			return "MESG";
		case G_LOG_LEVEL_INFO:
			return "INFO";
		case G_LOG_LEVEL_DEBUG:
			return "DBG";
		case LOG_LEVEL_FAULT:
			return "FALT";
		case LOG_LEVEL_DEBUG1:
			return "DBG1";
		case LOG_LEVEL_DEBUG2:
			return "DBG2";
		case LOG_LEVEL_DEBUG3:
			return "DBG3";
		case LOG_LEVEL_DEBUG4:
			return "DBG4";
		case LOG_LEVEL_DEBUG5:
			return "DBG5";
		case LOG_LEVEL_NOTIFY:
			return "NTFY";
	}
}

gchar * log_writer_format_fields(GLogLevelFlags level, const GLogField *fields, gsize n_fields)
{
  gsize i;
  const gchar *message = NULL;
  const gchar *log_domain = NULL;
  gssize message_length = -1;
  gssize log_domain_length = -1;
  const gchar *level_str;
  GString *gstring;
  gchar time_buf[128];

  /* Extract some common fields. */
  for (i = 0; (message == NULL || log_domain == NULL) && i < n_fields; i++)
  {
    const GLogField *field = &fields[i];

    if (mutt_str_equal(field->key, "MESSAGE"))
    {
      message = field->value;
      message_length = field->length;
    }
    else if (mutt_str_equal(field->key, "GLIB_DOMAIN"))
    {
      log_domain = field->value;
      log_domain_length = field->length;
    }
  }

  /* Format things. */
  level_str = log_level_to_str(level);

  gstring = g_string_new(NULL);
  if (!log_domain)
    g_string_append(gstring, "");
  else
  {
    g_string_append_len(gstring, log_domain, log_domain_length);
    g_string_append_c(gstring, '-');
  }
  g_string_append(gstring, level_str);
  g_string_append(gstring, ": ");

  /* Timestamp */
  mutt_date_localtime_format(time_buf, sizeof(time_buf), "%H:%M:%S", mutt_date_now());
  g_string_append(gstring, time_buf);
  g_string_append(gstring, ": ");

  if (message == NULL)
    g_string_append(gstring, "(NULL) message");
  else
    g_string_append_len(gstring, message, message_length);

  return g_string_free(gstring, FALSE);
}

static GLogWriterOutput log_writer(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data)
{
  if (level <= G_LOG_LEVEL_ERROR)
    return g_log_writer_default(level, fields, n_fields, user_data);

  /* return G_LOG_WRITER_HANDLED; */
  return MuttLogWriter(level, fields, n_fields, user_data);
}


void logging_init()
{
  LogQueue = g_queue_new();
  g_log_set_writer_func(log_writer, NULL, NULL);
}





/**
 * log_file_close - Close the log file
 * @param verbose If true, then log the event
 */
void log_file_close(bool verbose)
{
  if (!LogFileFP)
    return;

  fprintf(LogFileFP, "[%s] Closing log.\n", timestamp(0));
  fprintf(LogFileFP, "# vim: syntax=neomuttlog\n");
  mutt_file_fclose(&LogFileFP);
  if (verbose)
    log_message(_("Closed log file: %s"), LogFileName);
}

/**
 * log_file_open - Start logging to a file
 * @param verbose If true, then log the event
 * @retval  0 Success
 * @retval -1 Error, see errno
 *
 * Before opening a log file, call log_file_set_version(), log_file_set_level()
 * and log_file_set_filename().
 */
int log_file_open(bool verbose)
{
  if (!LogFileName)
    return -1;

  if (LogFileFP)
    log_file_close(false);

  if (LogFileLevel < LOG_LEVEL_DEBUG1)
    return -1;

  LogFileFP = mutt_file_fopen(LogFileName, "a+");
  if (!LogFileFP)
    return -1;
  setvbuf(LogFileFP, NULL, _IOLBF, 0);

  fprintf(LogFileFP, "[%s] NeoMutt%s debugging at level %d\n", timestamp(0),
          NONULL(LogFileVersion), LogFileLevel);
  if (verbose)
    log_message(_("Debugging at level %d to file '%s'"), LogFileLevel, LogFileName);
  return 0;
}

/**
 * log_file_set_filename - Set the filename for the log
 * @param file Name to use
 * @param verbose If true, then log the event
 * @retval  0 Success, file opened
 * @retval -1 Error, see errno
 */
int log_file_set_filename(const char *file, bool verbose)
{
  if (!file)
    return -1;

  /* also handles both being NULL */
  if (mutt_str_equal(LogFileName, file))
    return 0;

  mutt_str_replace(&LogFileName, file);

  if (!LogFileName)
    log_file_close(verbose);

  return log_file_open(verbose);
}

/**
 * log_file_set_level - Set the logging level
 * @param level Logging level
 * @param verbose If true, then log the event
 * @retval  0 Success
 * @retval -1 Error, level is out of range
 *
 * The level should be: LL_MESSAGE <= level < LL_MAX.
 */
int log_file_set_level(GLogLevelFlags level, bool verbose)
{
  if ((level < G_LOG_LEVEL_MESSAGE) || (level > LOG_LEVEL_NOTIFY))
    return -1;

  if (level == LogFileLevel)
    return 0;

  LogFileLevel = level;

  if (level == G_LOG_LEVEL_MESSAGE)
  {
    log_file_close(verbose);
  }
  else if (LogFileFP)
  {
    if (verbose)
      log_message(_("Logging at level %d to file '%s'"), LogFileLevel, LogFileName);
    fprintf(LogFileFP, "[%s] NeoMutt%s debugging at level %d\n", timestamp(0),
            NONULL(LogFileVersion), LogFileLevel);
  }
  else
  {
    log_file_open(verbose);
  }

  if (LogFileLevel >= LOG_LEVEL_DEBUG5)
  {
    fprintf(LogFileFP, "\n"
                       "WARNING:\n"
                       "    Logging at this level can reveal personal information.\n"
                       "    Review the log carefully before posting in bug reports.\n"
                       "\n");
  }

  return 0;
}

/**
 * log_file_set_version - Set the program's version number
 * @param version Version number
 *
 * The string will be appended directly to 'NeoMutt', so it should begin with a
 * hyphen.
 */
void log_file_set_version(const char *version)
{
  mutt_str_replace(&LogFileVersion, version);
}

/**
 * log_file_running - Is the log file running?
 * @retval true The log file is running
 */
bool log_file_running(void)
{
  return LogFileFP;
}

/**
 * log_writer_file - Save a log line to a file
 *
 * This log dispatcher saves a line of text to a file.  The format is:
 * * `[TIMESTAMP]<LEVEL> FUNCTION() FORMATTED-MESSAGE`
 *
 * The caller must first set #LogFileName and #LogFileLevel, then call
 * log_file_open().  Any logging above #LogFileLevel will be ignored.
 *
 * If stamp is 0, then the current time will be used.
 */
GLogWriterOutput log_writer_file(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer from_queue)
{
  if (!LogFileFP || level < G_LOG_LEVEL_ERROR || level < LogFileLevel)
    return G_LOG_WRITER_HANDLED;

  const gchar *function = log_get_field(fields, n_fields, "CODE_FUNC");
  const short _errno = GPOINTER_TO_INT(log_get_field(fields, n_fields, "ERRNO"));

  if (!function)
    function = "UNKNOWN";

  char *msg = log_writer_format_fields(level, fields, n_fields);
  fputs(msg, LogFileFP);
  FREE(&msg);

  if (_errno)
    fprintf(LogFileFP, ": %s\n", strerror(_errno));
  else
    fputs("\n", LogFileFP);

  return G_LOG_WRITER_HANDLED;
}

/**
 * log_queue_add - Add a LogLine to the queue
 * @param ll LogLine to add
 * @retval num Entries in the queue
 *
 * If #LogQueueMax is non-zero, the queue will be limited to this many items.
 */
int log_queue_add(LogLine *ll)
{
  if (!ll)
    return -1;

  g_queue_push_tail(LogQueue, ll);

  if ((LogQueueMax > 0) && (LogQueue->length >= LogQueueMax))
  {
    ll = g_queue_pop_head(LogQueue);
    FREE(&ll->message);
    FREE(&ll->line);
    FREE(&ll);

  }
  return LogQueue->length;
}

/**
 * log_queue_set_max_size - Set a upper limit for the queue length
 * @param size New maximum queue length
 *
 * @note size of 0 means unlimited
 */
void log_queue_set_max_size(int size)
{
  if (size < 0)
    size = 0;
  LogQueueMax = size;
}

/**
 * log_queue_empty - Free the contents of the queue
 *
 * Free any log lines in the queue.
 */
void log_queue_empty(void)
{
  LogLine *ll = NULL;

  while ((ll = g_queue_pop_tail(LogQueue)) != NULL)
  {
    FREE(&ll->message);
    FREE(&ll->line);
    FREE(&ll);
  }
}

/**
 * log_queue_flush - Replay the log queue
 * @param disp Log dispatcher - Implements ::log_dispatcher_t
 *
 * Pass all of the log entries in the queue to the log dispatcher provided.
 * The queue will be emptied afterwards.
 */
void log_queue_flush(GLogWriterFunc writer)
{
  log_debug5("LogQueue flushing...");
  for (GList *np = LogQueue->head; np != NULL; np = np->next)
  {
    LogLine *ll = np->data;
    const GLogField fields[] = {
      { "GLIB_DOMAIN", G_LOG_DOMAIN, -1 },
      { "MESSAGE", ll->message, -1 },
      { "CODE_FILE", ll->file, -1 },
      { "CODE_LINE", ll->line, -1 },
      { "CODE_FUNC", ll->func, -1 },
      { "ERRNO", GINT_TO_POINTER(ll->err_no), -1 }
    };
    writer(ll->level, fields, G_N_ELEMENTS (fields), (gpointer)true);
  }

  log_queue_empty();
}

/**
 * log_queue_save - Save the contents of the queue to a temporary file
 * @param fp Open file handle
 * @retval num Lines written to the file
 *
 * The queue is written to a temporary file.  The format is:
 * * `[HH:MM:SS]<LEVEL> FORMATTED-MESSAGE`
 *
 * @note The caller should delete the file
 */
int log_queue_save(FILE *fp)
{
  if (!fp)
    return 0;

  char buf[32] = { 0 };
  int count = 0;
  for (GList *np = LogQueue->head; np != NULL; np = np->next)
  {
    LogLine *ll = np->data;
    mutt_date_localtime_format(buf, sizeof(buf), "%H:%M:%S", ll->time);
    fprintf(fp, "[%s]<%s> %s", buf, log_level_to_str(ll->level), ll->message);
    if (ll->err_no)
      fprintf(fp, ": %s\n", strerror(ll->err_no));
    else
      fputs("\n", LogFileFP);
    count++;
  }

  return count;
}

/**
 * log_queue_writer - Save a log line to an internal queue
 *
 * This log dispatcher saves a line of text to a queue.
 * The format string and parameters are expanded and the other parameters are
 * stored as they are.
 *
 * @sa log_queue_set_max_size(), log_queue_flush(), log_queue_empty()
 *
 * @warning Log lines are limited to LOG_LINE_MAX_LEN bytes
 */
GLogWriterOutput log_writer_queue(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data)
{
  LogLine *ll = g_new0(LogLine, 1);
  ll->time = mutt_date_now();
  ll->level = level;

  for (int i = 0; i < n_fields; i++)
  {
    const char *key = (fields + i)->key;
    gconstpointer value = (fields + i)->value;

    if (mutt_str_equal(key, "MESSAGE"))
    {
      ll->message = mutt_str_dup(value);
    }
    else if (mutt_str_equal(key, "CODE_FILE"))
    {
      ll->file = value;
    }
    else if (mutt_str_equal(key, "CODE_LINE"))
    {
      ll->line = mutt_str_dup(value);
    }
    else if (mutt_str_equal(key, "CODE_FUNC"))
    {
      ll->func = value;
    }
    else if (mutt_str_equal(key, "ERRNO"))
    {
      ll->err_no = (long)value;
    }
  }

  log_queue_add(ll);

  return G_LOG_WRITER_HANDLED;
}

/**
 * log_terminal_writer - Save a log line to the terminal
 *
 * This log dispatcher saves a line of text to the terminal.
 * The format is:
 * * `[TIMESTAMP]<LEVEL> FUNCTION() FORMATTED-MESSAGE`
 *
 * @warning Log lines are limited to LOG_LINE_MAX_LEN bytes
 *
 * @note The output will be coloured using ANSI escape sequences,
 *       unless the output is redirected.
 */
GLogWriterOutput log_writer_terminal(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer from_queue)
{
  
  log_writer_file(level, fields, n_fields, from_queue);

  if ((level > G_LOG_LEVEL_MESSAGE) && (level != LOG_LEVEL_FAULT))
    return G_LOG_WRITER_HANDLED;

  FILE *stream;
  if (level & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | LOG_LEVEL_FAULT))
    stream = stderr;
  else
    stream = stdout;

  if (!stream || fileno(stream) < 0)
    return G_LOG_WRITER_UNHANDLED;

  char *msg = log_writer_format_fields(level, fields, n_fields);
  fputs(msg, stream);
  fputs("\n", stream);
  FREE(&msg);

  return G_LOG_WRITER_HANDLED;
}
