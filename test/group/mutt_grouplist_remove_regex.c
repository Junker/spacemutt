/**
 * @file
 * Test code for mutt_grouplist_remove_regex()
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
#include "address/lib.h"

void test_mutt_grouplist_remove_regex(void)
{
  // int mutt_grouplist_remove_regex(GroupList *head, const char *s);

  {
    TEST_CHECK(mutt_grouplist_remove_regex(NULL, "apple") == 0);
  }

  {
    GroupList *head = g_slist_alloc();
    TEST_CHECK(mutt_grouplist_remove_regex(head, NULL) == -1);
    g_slist_free_1(head);
  }
}
