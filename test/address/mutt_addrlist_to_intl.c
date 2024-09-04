/**
 * @file
 * Test code for mutt_addrlist_to_intl()
 *
 * @authors
 * Copyright (C) 2019 Pietro Cerutti <gahr@gahr.ch>
 * Copyright (C) 2019-2023 Richard Russon <rich@flatcap.org>
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
#include "config/lib.h"
#include "core/lib.h"
#include "test_common.h"

static struct ConfigDef Vars[] = {
  // clang-format off
  { "idn_decode", DT_BOOL, true, 0, NULL, },
  { "idn_encode", DT_BOOL, true, 0, NULL, },
  { NULL },
  // clang-format on
};

void test_mutt_addrlist_to_intl(void)
{
  // int mutt_addrlist_to_intl(AddressList *al, char **err);

  {
    char *err = NULL;
    TEST_CHECK(mutt_addrlist_to_intl(NULL, &err) == 0);
  }

  {
    AddressList *al = mutt_addrlist_new();
    TEST_CHECK(mutt_addrlist_to_intl(al, NULL) == 0);
    mutt_addrlist_free_full(al);
  }

  {
    struct
    {
      const char *local;
      const char *intl;
    } local2intl[] = { { .local = "test@äöüss.com", .intl = "test@xn--ss-uia6e4a.com" },
                       { .local = "test@nixieröhre.nixieclock-tube.com",
                         .intl = "test@xn--nixierhre-57a.nixieclock-tube.com" },
                       { .local = "test@வலைப்பூ.com", .intl = "test@xn--xlcawl2e7azb.com" } };

    TEST_CHECK(cs_register_variables(SpaceMutt->sub->cs, Vars));

    cs_subset_str_string_set(SpaceMutt->sub, "charset", "utf-8", NULL);
#ifdef HAVE_LIBIDN
    cs_subset_str_native_set(SpaceMutt->sub, "idn_encode", true, NULL);
    cs_subset_str_native_set(SpaceMutt->sub, "idn_decode", true, NULL);
#endif

    for (size_t i = 0; i < mutt_array_size(local2intl); ++i)
    {
      AddressList *al = mutt_addrlist_new();
      mutt_addrlist_append(al, mutt_addr_create(NULL, local2intl[i].local));
      mutt_addrlist_to_intl(al, NULL);
      struct Address *a = g_queue_peek_head(al);
#ifdef HAVE_LIBIDN
      TEST_CHECK_STR_EQ(buf_string(a->mailbox), local2intl[i].intl);
#else
      TEST_CHECK_STR_EQ(buf_string(a->mailbox), local2intl[i].local);
#endif
      mutt_addrlist_to_local(al);
      TEST_CHECK_STR_EQ(buf_string(a->mailbox), local2intl[i].local);
      mutt_addrlist_free_full(al);
    }
  }
}
