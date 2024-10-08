/**
 * @file
 * Test code for mutt_addrlist_search()
 *
 * @authors
 * Copyright (C) 2019 Richard Russon <rich@flatcap.org>
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

#define TEST_NO_MAIN
#include "config.h"
#include "acutest.h"
#include <stddef.h>
#include "mutt/lib.h"
#include "address/lib.h"

void test_mutt_addrlist_search(void)
{
  // bool mutt_addrlist_search(const Address *needle, const AddressList *haystack);

  {
    AddressList *al = mutt_addrlist_new();
    TEST_CHECK(!mutt_addrlist_search(al, NULL));
    mutt_addrlist_free_full(al);
  }

  {
    struct Address a = { 0 };
    TEST_CHECK(!mutt_addrlist_search(NULL, &a));
  }
}
