/**
 * @file
 * Test code for mutt_addrlist_equal()
 *
 * @authors
 * Copyright (C) 2019 Pietro Cerutti <gahr@gahr.ch>
 * Copyright (C) 2019-2022 Richard Russon <rich@flatcap.org>
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

void test_mutt_addrlist_equal(void)
{
  // bool mutt_addrlist_equal(const AddressList *ala, const AddressList *alb);

  {
    AddressList *al = mutt_addrlist_new();
    TEST_CHECK(!mutt_addrlist_equal(NULL, al));
    mutt_addrlist_free_full(al);
  }

  {
    AddressList *al = mutt_addrlist_new();
    TEST_CHECK(!mutt_addrlist_equal(al, NULL));
    mutt_addrlist_free_full(al);
  }

  {
    TEST_CHECK(mutt_addrlist_equal(NULL, NULL));
  }

  {
    /* It is not enough for two AddressLists to contain the same addresses,
     * although in different order, for them to be equal. */
    AddressList *al1 = mutt_addrlist_new();
    int parsed1 = mutt_addrlist_parse(al1, "test@example.com, foo@bar.baz, john@doe.org");
    TEST_CHECK(parsed1 == 3);
    AddressList *al2 = mutt_addrlist_new();
    int parsed2 = mutt_addrlist_parse(al2, "foo@bar.baz, test@example.com, johbn@doe.org, foo@bar.baz, john@doe.org");
    TEST_CHECK(parsed2 == 5);
    TEST_CHECK(!mutt_addrlist_equal(al1, al2));
    mutt_addrlist_free_full(al1);
    mutt_addrlist_free_full(al2);
  }

  {
    /* It is not enough for two AddressLists to contain the same mailboxes,
     * although for them to be equal. */
    AddressList *al1 = mutt_addrlist_new();
    int parsed1 = mutt_addrlist_parse(al1, "Name 1 <test@example.com>, foo@bar.baz, john@doe.org");
    TEST_CHECK(parsed1 == 3);
    AddressList *al2 = mutt_addrlist_new();
    int parsed2 = mutt_addrlist_parse(al1, "Name 2 <test@example.com>, foo@bar.baz, john@doe.org");
    TEST_CHECK(parsed2 == 3);
    TEST_CHECK(!mutt_addrlist_equal(al1, al2));
    mutt_addrlist_free_full(al1);
    mutt_addrlist_free_full(al2);
  }

  {
    /* Two equal AddressLists contain the same mailboxes and personal names in
     * the same order. */
    AddressList *al1 = mutt_addrlist_new();
    int parsed1 = mutt_addrlist_parse(al1, "Same Name <test@example.com>, foo@bar.baz, john@doe.org");
    TEST_CHECK(parsed1 == 3);
    AddressList *al2 = mutt_addrlist_new();
    int parsed2 = mutt_addrlist_parse(al1, "Same Name <test@example.com>, foo@bar.baz, john@doe.org");
    TEST_CHECK(parsed2 == 3);
    TEST_CHECK(!mutt_addrlist_equal(al1, al2));
    mutt_addrlist_free_full(al1);
    mutt_addrlist_free_full(al2);
  }
}
