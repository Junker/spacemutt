/**
 * @file
 * Test code for mutt_grouplist_add_regex()
 *
 * @authors
 * Copyright (C) 2019 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2023 Dennis Schön <mail@dennis-schoen.de>
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
#include "address/lib.h"

void test_mutt_grouplist_add_regex(void)
{
  // int mutt_grouplist_add_regex(GroupList *gl, const char *s, uint16_t flags, struct Buffer *err);

  {
    struct Buffer *err = buf_pool_get();
    TEST_CHECK(mutt_grouplist_add_regex(NULL, "apple", 0, err) == 0);
    buf_pool_release(&err);
  }

  {
    GroupList *head = g_slist_alloc();
    struct Buffer *err = buf_pool_get();
    TEST_CHECK(mutt_grouplist_add_regex(head, NULL, 0, err) == -1);
    buf_pool_release(&err);
    g_slist_free_1(head);
  }

  {
    GroupList *head = NULL;
    TEST_CHECK(mutt_grouplist_add_regex(head, "apple", 0, NULL) == 0);
  }
}
