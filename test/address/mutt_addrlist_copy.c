/**
 * @file
 * Test code for mutt_addrlist_copy()
 *
 * @authors
 * Copyright (C) 2019 Richard Russon <rich@flatcap.org>
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
#include <stdbool.h>
#include <stddef.h>
#include "mutt/lib.h"
#include "address/lib.h"
#include "test_common.h"

void test_mutt_addrlist_copy(void)
{
  // void mutt_addrlist_copy(AddressList *dst, const AddressList *src, bool prune);

  {
    AddressList *al = NULL;
    mutt_addrlist_copy(NULL, al, false);
    TEST_CHECK_(1, "mutt_addrlist_copy(NULL, al, false)");
  }

  {
    AddressList *al = NULL;
    mutt_addrlist_copy(al, NULL, false);
    TEST_CHECK_(1, "mutt_addrlist_copy(al, NULL, false)");
  }

  {
    AddressList *src = mutt_addrlist_new();
    AddressList *dst = mutt_addrlist_new();
    mutt_addrlist_copy(dst, src, false);
    TEST_CHECK(g_queue_is_empty(src));
    TEST_CHECK(g_queue_is_empty(dst));
    mutt_addrlist_free_full(src);
    mutt_addrlist_free_full(dst);
  }

  {
    AddressList *src = mutt_addrlist_new();
    AddressList *dst = mutt_addrlist_new();
    mutt_addrlist_append(src, mutt_addr_create(NULL, "test@example.com"));
    mutt_addrlist_append(src, mutt_addr_create(NULL, "john@doe.org"));
    mutt_addrlist_append(src, mutt_addr_create(NULL, "the-who@stage.co.uk"));
    mutt_addrlist_copy(dst, src, false);
    TEST_CHECK(!g_queue_is_empty(src));
    TEST_CHECK(!g_queue_is_empty(dst));
    struct Address *adst = g_queue_peek_nth(dst, 0);
    TEST_CHECK_STR_EQ(buf_string(adst->mailbox), "test@example.com");
    adst = g_queue_peek_nth(dst, 1);
    TEST_CHECK_STR_EQ(buf_string(adst->mailbox), "john@doe.org");
    adst = g_queue_peek_nth(dst, 2);
    TEST_CHECK_STR_EQ(buf_string(adst->mailbox), "the-who@stage.co.uk");
    mutt_addrlist_free_full(src);
    mutt_addrlist_free_full(dst);
  }
}
