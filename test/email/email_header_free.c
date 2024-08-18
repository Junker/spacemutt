/**
 * @file
 * Test code for test_email_header_free()
 *
 * @authors
 * Copyright (C) 2020 Matthew Hughes <matthewhughes934@gmail.com>
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
#include <glib.h>
#include "mutt/lib.h"
#include "email/lib.h"

void test_email_header_free(void)
{
  char *first_header = "X-First: 0";
  char *second_header = "X-Second: 1";

  GQueue *hdrlist = g_queue_new();
  GList *first = header_add(hdrlist, first_header);
  GList *second = header_add(hdrlist, second_header);

  {
    header_free(hdrlist, first);
    TEST_CHECK(header_find(hdrlist, first_header) == NULL); /* Removed expected header */
    TEST_CHECK(header_find(hdrlist, second_header) != NULL); /* Other headers untouched */
  }

  {
    header_free(hdrlist, second);
    TEST_CHECK(header_find(hdrlist, second_header) == NULL);
    TEST_CHECK(hdrlist->head == NULL); /* List now empty */
  }
  g_free(hdrlist);
}
