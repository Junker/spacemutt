/**
 * @file
 * Test code for mutt_replacelist_apply()
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

void test_mutt_replacelist_apply(void)
{
  // char *mutt_replacelist_apply(ReplaceList *rl, const char *str);

  {
    TEST_CHECK(mutt_replacelist_apply(NULL, "apple") == NULL);
  }

  {
    ReplaceList *replacelist = NULL;
    const char *str = mutt_replacelist_apply(replacelist, NULL);
    TEST_CHECK(str == NULL);
  }

  {
    ReplaceList *replacelist = NULL;
    const char *str = mutt_replacelist_apply(replacelist, "apple");
    TEST_CHECK(str == NULL);
    FREE(&str);
  }
}
