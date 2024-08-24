/**
 * @file
 * Test code for log_disp_terminal()
 *
 * @authors
 * Copyright (C) 2019 Richard Russon <rich@flatcap.org>
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

#define TEST_NO_MAIN
#include "config.h"
#include "acutest.h"
#include <stddef.h>
#include "mutt/lib.h"

void test_log_disp_terminal(void)
{
  // int log_disp_terminal(time_t stamp, const char *file, int line, const char *function, int level, ...);

  {
    const GLogField fields[] = {
      { "GLIB_DOMAIN", G_LOG_DOMAIN, -1 },
      { "MESSAGE", "fmt", -1 }
    };
    TEST_CHECK(log_writer_terminal(G_LOG_LEVEL_MESSAGE, fields, G_N_ELEMENTS(fields), NULL) == G_LOG_WRITER_HANDLED);
  }

  {
    const GLogField fields[] = {
      { "GLIB_DOMAIN", G_LOG_DOMAIN, -1 },
      { "MESSAGE", "fmt", -1 },
      { "CODE_FILE", "", -1 },
      { "CODE_LINE", "0", -1 },
      { "CODE_FUNC", "banana", -1 },
    };
    TEST_CHECK(log_writer_terminal(G_LOG_LEVEL_WARNING, fields, G_N_ELEMENTS(fields), NULL) == G_LOG_WRITER_HANDLED);
  }
}
