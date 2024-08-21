/**
 * @file
 * Test code for mutt_addrlist_remove_xrefs()
 *
 * @authors
 * Copyright (C) 2019 Pietro Cerutti <gahr@gahr.ch>
 * Copyright (C) 2019-2022 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2023 Anna Figueiredo Gomes <navi@vlhl.dev>
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
#include "test_common.h"

void test_mutt_addrlist_remove_xrefs(void)
{
  // void mutt_addrlist_remove_xrefs(const AddressList *a, AddressList *b);

  {
    AddressList *al = NULL;
    mutt_addrlist_remove_xrefs(NULL, al);
    TEST_CHECK_(1, "mutt_addrlist_remove_xrefs(NULL, al)");
  }

  {
    AddressList *al = NULL;
    mutt_addrlist_remove_xrefs(al, NULL);
    TEST_CHECK_(1, "mutt_addrlist_remove_xrefs(al, NULL)");
  }

  {
    AddressList *al1 = mutt_addrlist_new();
    AddressList *al2 = mutt_addrlist_new();
    mutt_addrlist_append(al1, mutt_addr_create("Name 1", "foo@example.com"));
    mutt_addrlist_append(al2, mutt_addr_create("Name 2", "foo@example.com"));
    mutt_addrlist_remove_xrefs(al1, al2);
    struct Address *a = g_queue_peek_nth(al1, 0);;
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "foo@example.com");
    a = g_queue_peek_nth(al1, 1);
    TEST_CHECK(a == NULL);
    TEST_CHECK(g_queue_is_empty(al2));
    mutt_addrlist_free_full(al1);
    mutt_addrlist_free_full(al2);
  }

  {
    AddressList *al1 = mutt_addrlist_new();
    AddressList *al2 = mutt_addrlist_new();
    mutt_addrlist_append(al1, mutt_addr_create("Name 1", "foo@example.com"));
    mutt_addrlist_append(al2, mutt_addr_create("Name 2", "foo@example.com"));
    mutt_addrlist_append(al1, mutt_addr_create(NULL, "john@doe.org"));
    mutt_addrlist_append(al1, mutt_addr_create(NULL, "foo@bar.baz"));
    mutt_addrlist_append(al2, mutt_addr_create(NULL, "foo@bar.baz"));
    mutt_addrlist_append(al2, mutt_addr_create(NULL, "mr.pink@reservoir.movie"));
    mutt_addrlist_remove_xrefs(al1, al2);
    struct Address *a = g_queue_peek_nth(al1, 0);
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "foo@example.com");
    a = g_queue_peek_nth(al1, 1);
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "john@doe.org");
    a = g_queue_peek_nth(al1, 2);
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "foo@bar.baz");
    a = g_queue_peek_nth(al1, 3);
    TEST_CHECK(a == NULL);
    a = g_queue_peek_nth(al2, 0);
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "mr.pink@reservoir.movie");
    a = g_queue_peek_nth(al2, 1);
    TEST_CHECK(a == NULL);
    mutt_addrlist_free_full(al1);
    mutt_addrlist_free_full(al2);
  }
}
