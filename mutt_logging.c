/**
 * @file
 * NeoMutt Logging
 *
 * @authors
 * Copyright (C) 2018-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2019 Pietro Cerutti <gahr@gahr.ch>
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
 * @page neo_mutt_logging NeoMutt Logging
 *
 * NeoMutt Logging
 */

#include "config.h"
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "mutt/lib.h"
#include "config/lib.h"
#include "core/lib.h"
#include "gui/lib.h"
#include "mutt_logging.h"
#include "color/lib.h"
#include "globals.h"
#include "muttlib.h"

static uint64_t LastError = 0; ///< Time of the last error message (in milliseconds since the Unix epoch)

static char *CurrentFile = NULL; ///< The previous log file name
static const int NumOfLogs = 5;  ///< How many log files to rotate

#define S_TO_MS 1000L

/**
 * error_pause - Wait for an error message to be read
 *
 * If '$sleep_time' seconds hasn't elapsed since LastError, then wait
 */
static void error_pause(void)
{
  const short c_sleep_time = cs_subset_number(NeoMutt->sub, "sleep_time");
  const uint64_t elapsed = mutt_date_now_ms() - LastError;
  const uint64_t sleep = c_sleep_time * S_TO_MS;
  if ((LastError == 0) || (elapsed >= sleep))
    return;

  mutt_refresh();
  mutt_date_sleep_ms(sleep - elapsed);
}

/**
 * mutt_clear_error - Clear the message line (bottom line of screen)
 */
void mutt_clear_error(void)
{
  /* Make sure the error message has had time to be read */
  if (OptMsgErr)
    error_pause();

  ErrorBufMessage = false;
  if (!OptNoCurses)
    msgwin_clear_text(NULL);
}

/**
 * log_disp_curses - Display a log line in the message line
 */
GLogWriterOutput log_writer_curses(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer from_queue)
{
  const short c_debug_level = cs_subset_number(NeoMutt->sub, "debug_level");

  if (log_level_to_debug_level(level) > c_debug_level)
    return G_LOG_WRITER_HANDLED;

  g_autoptr(GString) message = g_string_new(log_get_field(fields, n_fields, "MESSAGE"));
  const long _errno = (long)log_get_field(fields, n_fields, "ERRNO");

  if (_errno)
  {
    const char *p = strerror(errno);
    if (!p)
      p = _("unknown error");

    g_string_append_printf(message, ": %s (errno = %d)", p, errno);
  }
  const bool dupe = (mutt_str_equal(message->str, ErrorBuf));
  if (!dupe)
  {
    /* Only log unique messages */
    log_writer_file(level, fields, n_fields, NULL);
    if (!from_queue)
      log_writer_queue(level, fields, n_fields, NULL);
  }

  /* Don't display debugging message on screen */
  if (level > LOG_LEVEL_FAULT)
    return 0;

  /* Only pause if this is a message following an error */
  if ((level == LOG_LEVEL_FAULT) && OptMsgErr && !dupe)
    error_pause();

  mutt_str_copy(ErrorBuf, message->str, sizeof(ErrorBuf));
  ErrorBufMessage = true;

  if (!OptKeepQuiet)
  {
    enum ColorId cid = MT_COLOR_NORMAL;
    switch (level)
    {
      case LOG_LEVEL_FAULT:
        mutt_beep(false);
        cid = MT_COLOR_ERROR;
        break;
      case G_LOG_LEVEL_WARNING:
        cid = MT_COLOR_WARNING;
        break;
      default:
        cid = MT_COLOR_MESSAGE;
        break;
    }
    /* log_fatal(ErrorBuf); */
    /* printf("\n\nD:%d L:%d LD:%d M:%s EB:%s \n\n", c_debug_level, level, log_level_to_debug_level(level), message->str, ErrorBuf); */

    msgwin_set_text(NULL, ErrorBuf, cid);
  }

  if ((level == LOG_LEVEL_FAULT) && !dupe)
  {
    OptMsgErr = true;
    LastError = mutt_date_now_ms();
  }
  else
  {
    OptMsgErr = false;
    LastError = 0;
  }

  window_redraw(msgwin_get_window());
  return G_LOG_WRITER_HANDLED;
}

/**
 * mutt_log_prep - Prepare to log
 */
void mutt_log_prep(void)
{
  char ver[64] = { 0 };
  snprintf(ver, sizeof(ver), "-%s%s", PACKAGE_VERSION, GitVer);
  log_file_set_version(ver);
}

/**
 * mutt_log_stop - Close the log file
 */
void mutt_log_stop(void)
{
  log_file_close(false);
  FREE(&CurrentFile);
}

/**
 * mutt_log_set_file - Change the logging file
 * @param file Name to use
 * @retval  0 Success, file opened
 * @retval -1 Error, see errno
 *
 * Close the old log, rotate the new logs and open the new log.
 */
int mutt_log_set_file(const char *file)
{
  const char *const c_debug_file = cs_subset_path(NeoMutt->sub, "debug_file");
  if (!mutt_str_equal(CurrentFile, c_debug_file))
  {
    struct Buffer *expanded = buf_pool_get();
    buf_addstr(expanded, c_debug_file);
    buf_expand_path(expanded);

    const char *name = mutt_file_rotate(buf_string(expanded), NumOfLogs);
    buf_pool_release(&expanded);
    if (!name)
      return -1;

    log_file_set_filename(name, false);
    FREE(&name);
    mutt_str_replace(&CurrentFile, c_debug_file);
  }

  cs_subset_str_string_set(NeoMutt->sub, "debug_file", file, NULL);

  return 0;
}

short log_level_to_debug_level(GLogLevelFlags level)
{
  return (short)log2(level - G_LOG_LEVEL_DEBUG) - G_LOG_LEVEL_USER_SHIFT;
}

GLogLevelFlags debug_level_to_log_level(short debug_level)
{
  return (1 << (G_LOG_LEVEL_USER_SHIFT + debug_level)) + G_LOG_LEVEL_DEBUG;
}

/**
 * mutt_log_set_level - Change the logging level
 * @param level Logging level
 * @param verbose If true, then log the event
 * @retval  0 Success
 * @retval -1 Error, level is out of range
 */
int mutt_log_set_level(GLogLevelFlags level, bool verbose)
{
  if (!CurrentFile)
  {
    const char *const c_debug_file = cs_subset_path(NeoMutt->sub, "debug_file");
    mutt_log_set_file(c_debug_file);
  }

  if (log_file_set_level(level, verbose) != 0)
    return -1;

  cs_subset_str_native_set(NeoMutt->sub, "debug_level", log_level_to_debug_level(level), NULL);
  return 0;
}

/**
 * mutt_log_start - Enable file logging
 * @retval  0 Success, or already running
 * @retval -1 Failed to start
 *
 * This also handles file rotation.
 */
int mutt_log_start(void)
{
  const short c_debug_level = cs_subset_number(NeoMutt->sub, "debug_level");
  if (c_debug_level < 1)
    return 0;

  if (log_file_running())
    return 0;

  const char *const c_debug_file = cs_subset_path(NeoMutt->sub, "debug_file");
  mutt_log_set_file(c_debug_file);

  /* This will trigger the file creation */
  if (log_file_set_level(debug_level_to_log_level(c_debug_level), true) < 0)
    return -1;

  return 0;
}

/**
 * level_validator - Validate the "debug_level" config variable - Implements ConfigDef::validator() - @ingroup cfg_def_validator
 */
int level_validator(const struct ConfigSet *cs, const struct ConfigDef *cdef,
                    intptr_t value, struct Buffer *err)
{
  if ((value < 0) || (value > DEBUG_LEVEL_MAX))
  {
    buf_printf(err, _("Invalid value for option %s: %ld"), cdef->name, (long) value);
    return CSR_ERR_INVALID;
  }

  return CSR_SUCCESS;
}

/**
 * main_log_observer - Notification that a Config Variable has changed - Implements ::observer_t - @ingroup observer_api
 */
int main_log_observer(struct NotifyCallback *nc)
{
  if (nc->event_type != NT_CONFIG)
    return 0;
  if (!nc->event_data)
    return -1;

  struct EventConfig *ev_c = nc->event_data;

  if (mutt_str_equal(ev_c->name, "debug_file"))
  {
    const char *const c_debug_file = cs_subset_path(NeoMutt->sub, "debug_file");
    mutt_log_set_file(c_debug_file);
  }
  else if (mutt_str_equal(ev_c->name, "debug_level"))
  {
    const short c_debug_level = cs_subset_number(NeoMutt->sub, "debug_level");
    mutt_log_set_level(debug_level_to_log_level(c_debug_level), true);
  }
  else
  {
    return 0;
  }

  log_debug5("log done");
  return 0;
}
