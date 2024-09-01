/**
 * @file
 * Test code for mutt_grouplist_add_addrlist()
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

void test_mutt_grouplist_add_addrlist(void)
{
  // void mutt_grouplist_add_addrlist(GroupList *head, struct Address *a);

  {
    AddressList *addr = mutt_addrlist_new();
    mutt_grouplist_add_addrlist(NULL, addr);
    TEST_CHECK_(1, "mutt_grouplist_add_addrlist(NULL, addr)");
    mutt_addrlist_free_full(addr);
  }

  {
    GroupList *head = g_slist_alloc();
    mutt_grouplist_add_addrlist(head, NULL);
    TEST_CHECK_(1, "mutt_grouplist_add_addrlist(head, NULL)");
    g_slist_free_1(head);
  }
}
