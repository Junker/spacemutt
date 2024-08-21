/**
 * @file
 * Test code for mutt_addrlist_write_list()
 *
 * @authors
 * Copyright (C) 2020-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2023 Pietro Cerutti <gahr@gahr.ch>
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
#include <stdio.h>
#include "mutt/lib.h"
#include "address/lib.h"
#include "config/lib.h"
#include "core/lib.h"
#include "test_common.h"

static struct ConfigDef Vars[] = {
  // clang-format off
  { "idn_decode", DT_BOOL, 0, 0, NULL, },
  { NULL },
  // clang-format on
};

void test_mutt_addrlist_write_list(void)
{
  {
    TEST_CHECK(cs_register_variables(NeoMutt->sub->cs, Vars));
    AddressList *al = mutt_addrlist_new();
    const char in[] = "some-group: first@example.com,second@example.com; John Doe <john@doe.org>, \"Foo J. Bar\" <foo-j-bar@baz.com>";
    mutt_addrlist_parse(al, in);
    GSList *l = NULL;
    const size_t written = mutt_addrlist_write_list(al, &l);
    TEST_CHECK(written == 5);

    char out[1024] = { 0 };
    size_t off = 0;
    for (GSList *ln = l; ln != NULL; ln = ln->next)
    {
      off += snprintf(out + off, sizeof(out) - off, "|%s|", (char*)ln->data);
    }
    TEST_CHECK_STR_EQ(out, "|some-group: ||first@example.com||second@example.com||John Doe <john@doe.org>||\"Foo J. Bar\" <foo-j-bar@baz.com>|");
    mutt_addrlist_free_full(al);
    g_slist_free_full(l, g_free);
  }
}
