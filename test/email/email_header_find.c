/**
 * @file
 * Test code for test_email_header_find()
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
#include "mutt/lib.h"
#include "email/lib.h"

void test_email_header_find(void)
{
  // GList *header_find(GQueue *hdrlist, const char *header)
  char *header = "X-TestHeader: 123";

  GQueue *hdrlist = g_queue_new();
  GList *n = g_list_alloc();
  n->data = mutt_str_dup(header);
  g_queue_push_tail_link(hdrlist, n);

  {
    GList *found = header_find(hdrlist, header);
    TEST_CHECK(found == n);
  }

  {
    const char *not_found = "X-NotIncluded: foo";
    GList *found = header_find(hdrlist, not_found);
    TEST_CHECK(found == NULL);
  }

  {
    const char *field_only = "X-TestHeader:";
    TEST_CHECK(header_find(hdrlist, field_only) == n);
  }

  {
    const char *invalid_header = "Not a header";
    TEST_CHECK(header_find(hdrlist, invalid_header) == NULL);
  }
  g_queue_free_full(hdrlist, g_free);
}
