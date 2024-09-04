/**
 * @file
 * Test code for spacemutt_free()
 *
 * @authors
 * Copyright (C) 2020 Richard Russon <rich@flatcap.org>
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
#include "core/lib.h"

void test_spacemutt_free(void)
{
  // void spacemutt_free(struct SpaceMutt **ptr);

  {
    spacemutt_free(NULL);
    TEST_CHECK_(1, "spacemutt_free(NULL)");
  }

  {
    struct SpaceMutt *n = NULL;
    spacemutt_free(&n);
    TEST_CHECK_(1, "spacemutt_free(&n)");
  }

  {
    struct SpaceMutt *n = mutt_mem_calloc(1, sizeof(*n));
    spacemutt_free(&n);
    TEST_CHECK_(1, "spacemutt_free(&n)");
  }
}
