/**
 * @file
 * Test code for strlist_is_empty()
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

#define TEST_NO_MAIN
#include "config.h"
#include "acutest.h"
#include <stddef.h>
#include "mutt/lib.h"
#include "config/lib.h"

void test_strlist_is_empty(void)
{
  // bool strlist_is_empty(const struct StrList *list);

  TEST_CHECK(strlist_is_empty(NULL));

  struct StrList *slist = strlist_new(D_STRLIST_SEP_COMMA);
  TEST_CHECK(strlist_is_empty(slist));
  strlist_free(&slist);
}
