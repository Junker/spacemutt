/**
 * @file
 * Test code for mutt_addrlist_parse2()
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

void test_mutt_addrlist_parse2(void)
{
  // int mutt_addrlist_parse2(AddressList *al, const char *s);

  {
    AddressList *alist = mutt_addrlist_new();
    int parsed = mutt_addrlist_parse2(alist, NULL);
    TEST_CHECK(parsed == 0);
    TEST_CHECK(g_queue_is_empty(alist));
    mutt_addrlist_free_full(alist);
  }

  {
    AddressList *alist = mutt_addrlist_new();
    int parsed = mutt_addrlist_parse2(alist, "");
    TEST_CHECK(parsed == 0);
    TEST_CHECK(g_queue_is_empty(alist));
    mutt_addrlist_free_full(alist);
  }

  {
    AddressList *alist = mutt_addrlist_new();
    int parsed = mutt_addrlist_parse2(alist, "apple");
    TEST_CHECK(parsed == 1);
    TEST_CHECK(!g_queue_is_empty(alist));
    TEST_CHECK_STR_EQ(buf_string(((struct Address*)g_queue_peek_head(alist))->mailbox), "apple");
    mutt_addrlist_free_full(alist);
  }

  {
    /* Not extremely nice, but this is the way it works... */
    AddressList *alist = mutt_addrlist_new();
    int parsed = mutt_addrlist_parse2(alist, "test@example.com John Doe <john@doe.org>");
    TEST_CHECK(parsed == 1);
    TEST_CHECK(!g_queue_is_empty(alist));
    struct Address *a = g_queue_peek_head(alist);
    TEST_CHECK_STR_EQ(buf_string(a->personal), "test@example.com John Doe");
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "john@doe.org");
    mutt_addrlist_free_full(alist);
  }

  {
    AddressList *alist = mutt_addrlist_new();
    int parsed = mutt_addrlist_parse2(alist, "test@example.com john@doe.org foo@bar.baz");
    TEST_CHECK(parsed == 3);
    TEST_CHECK(!g_queue_is_empty(alist));
    struct Address *a = g_queue_peek_nth(alist, 0);
    TEST_CHECK(a->personal == NULL);
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "test@example.com");
    a = g_queue_peek_nth(alist, 1);
    TEST_CHECK(a->personal == NULL);
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "john@doe.org");
    a = g_queue_peek_nth(alist, 2);
    TEST_CHECK(a->personal == NULL);
    TEST_CHECK_STR_EQ(buf_string(a->mailbox), "foo@bar.baz");
    a = g_queue_peek_nth(alist, 3);
    TEST_CHECK(a == NULL);
    mutt_addrlist_free_full(alist);
  }
}
