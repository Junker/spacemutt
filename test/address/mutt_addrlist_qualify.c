/**
 * @file
 * Test code for mutt_addrlist_qualify()
 *
 * @authors
 * Copyright (C) 2019 Pietro Cerutti <gahr@gahr.ch>
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

void test_mutt_addrlist_qualify(void)
{
  // void mutt_addrlist_qualify(AddressList *al, const char *host);

  {
    mutt_addrlist_qualify(NULL, "example.com");
    TEST_CHECK_(1, "mutt_addrlist_qualify(NULL, \"example.com\")");
  }

  {
    AddressList *al = mutt_addrlist_new();
    mutt_addrlist_qualify(al, NULL);
    TEST_CHECK_(1, "mutt_addrlist_qualify(addr, NULL)");
    mutt_addrlist_free_full(al);
  }

  {
    AddressList *al = mutt_addrlist_new();
    mutt_addrlist_parse(al, "john@doe.org, user1, user2, test@example.com");
    mutt_addrlist_qualify(al, "local.domain");
    struct Address *a = g_queue_peek_nth(al, 0);
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "john@doe.org");
    a = g_queue_peek_nth(al, 1);
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "user1@local.domain");
    a = g_queue_peek_nth(al, 2);
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "user2@local.domain");
    a = g_queue_peek_nth(al, 3);
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "test@example.com");
    a = g_queue_peek_nth(al, 4);
    TEST_CHECK(a == NULL);
    mutt_addrlist_free_full(al);
  }
}
