/**
 * @file
 * Test code for the StrList object
 *
 * @authors
 * Copyright (C) 2019-2024 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2020-2023 Pietro Cerutti <gahr@gahr.ch>
 * Copyright (C) 2022 Michal Siedlaczek <michal@siedlaczek.me>
 * Copyright (C) 2023 наб <nabijaczleweli@nabijaczleweli.xyz>
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
#include <stdint.h>
#include <stdio.h>
#include "mutt/lib.h"
#include "config/lib.h"
#include "core/lib.h"
#include "common.h" // IWYU pragma: keep
#include "test_common.h"

// clang-format off
struct ConfigDef VarsColon[] = {
  { "Apple",      DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple",               0, NULL, }, /* test_initial_values */
  { "Banana",     DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple:banana",        0, NULL, },
  { "Cherry",     DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple:banana:cherry", 0, NULL, },
  { "Damson",     DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple:banana",        0, NULL, }, /* test_string_set */
  { "Elderberry", DT_STRLIST|D_STRLIST_SEP_COLON, 0,                        0, NULL, },
  { "Fig",        DT_STRLIST|D_STRLIST_SEP_COLON, IP ":apple",              0, NULL, }, /* test_string_get */
  { "Guava",      DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple::cherry",       0, NULL, },
  { "Hawthorn",   DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple:",              0, NULL, },
  { "Ilama",      DT_STRLIST|D_STRLIST_SEP_COLON|D_ON_STARTUP, IP "apple",  0, NULL, }, /* startup */
  { NULL },
};

struct ConfigDef VarsComma[] = {
  { "Apple",      DT_STRLIST|D_STRLIST_SEP_COMMA, IP "apple",               0, NULL, }, /* test_initial_values */
  { "Banana",     DT_STRLIST|D_STRLIST_SEP_COMMA, IP "apple,banana",        0, NULL, },
  { "Cherry",     DT_STRLIST|D_STRLIST_SEP_COMMA, IP "apple,banana,cherry", 0, NULL, },
  { "Damson",     DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple,banana",        0, NULL, }, /* test_string_set */
  { "Elderberry", DT_STRLIST|D_STRLIST_SEP_COLON, 0,                        0, NULL, },
  { "Fig",        DT_STRLIST|D_STRLIST_SEP_COLON, IP ",apple",              0, NULL, }, /* test_string_get */
  { "Guava",      DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple,,cherry",       0, NULL, },
  { "Hawthorn",   DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple,",              0, NULL, },
  { "Ilama",      DT_STRLIST|D_STRLIST_SEP_COMMA|D_ON_STARTUP, IP "apple",  0, NULL, }, /* startup */
  { NULL },
};

struct ConfigDef VarsSpace[] = {
  { "Apple",      DT_STRLIST|D_STRLIST_SEP_SPACE, IP "apple",               0, NULL, }, /* test_initial_values */
  { "Banana",     DT_STRLIST|D_STRLIST_SEP_SPACE, IP "apple banana",        0, NULL, },
  { "Cherry",     DT_STRLIST|D_STRLIST_SEP_SPACE, IP "apple banana cherry", 0, NULL, },
  { "Damson",     DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple banana",        0, NULL, }, /* test_string_set */
  { "Elderberry", DT_STRLIST|D_STRLIST_SEP_COLON, 0,                        0, NULL, },
  { "Fig",        DT_STRLIST|D_STRLIST_SEP_COLON, IP " apple",              0, NULL, }, /* test_string_get */
  { "Guava",      DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple  cherry",       0, NULL, },
  { "Hawthorn",   DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple ",              0, NULL, },
  { "Ilama",      DT_STRLIST|D_STRLIST_SEP_SPACE|D_ON_STARTUP, IP "apple",  0, NULL, }, /* startup */
  { NULL },
};

struct ConfigDef VarsOther[] = {
  { "Ilama",      DT_STRLIST|D_STRLIST_SEP_COLON, 0,                        0, NULL,                    }, /* test_native_set */
  { "Jackfruit",  DT_STRLIST|D_STRLIST_SEP_COLON, IP "apple:banana:cherry", 0, NULL,                    }, /* test_native_get */
  { "Lemon",      DT_STRLIST|D_STRLIST_SEP_COLON, IP "lemon",               0, NULL,                    }, /* test_reset */
  { "Mango",      DT_STRLIST|D_STRLIST_SEP_COLON, IP "mango",               0, validator_fail,          },
  { "Nectarine",  DT_STRLIST|D_STRLIST_SEP_COLON, IP "nectarine",           0, validator_succeed,       }, /* test_validator */
  { "Olive",      DT_STRLIST|D_STRLIST_SEP_COLON, IP "olive",               0, validator_warn,          },
  { "Papaya",     DT_STRLIST|D_STRLIST_SEP_COLON, IP "papaya",              0, validator_fail,          },
  { "Quince",     DT_STRLIST|D_STRLIST_SEP_COLON, 0,                        0, NULL,                    }, /* test_inherit */
  { "Raspberry",  DT_STRLIST|D_STRLIST_SEP_COLON, 0,                        0, NULL,                    }, /* test_plus_equals */
  { "Strawberry", DT_STRLIST|D_STRLIST_SEP_COLON, 0,                        0, NULL,                    }, /* test_minus_equals */
  { "Wolfberry",  DT_STRLIST|D_STRLIST_SEP_COLON, IP "utf-8",               0, charset_strlist_validator, }, /* test_charset_validator */
  { "Xigua",      DT_STRLIST|D_STRLIST_SEP_COLON|D_ON_STARTUP, IP "apple",  0, NULL,                    }, /* startup */
  { NULL },
};
// clang-format on

static void strlist_flags(unsigned int flags, struct Buffer *buf)
{
  if (!buf)
    return;

  switch (flags & D_STRLIST_SEP_MASK)
  {
    case D_STRLIST_SEP_SPACE:
      buf_addstr(buf, "SPACE");
      break;
    case D_STRLIST_SEP_COMMA:
      buf_addstr(buf, "COMMA");
      break;
    case D_STRLIST_SEP_COLON:
      buf_addstr(buf, "COLON");
      break;
    default:
      buf_addstr(buf, "UNKNOWN");
      return;
  }

  if (flags & D_STRLIST_ALLOW_DUPES)
    buf_addstr(buf, " | D_STRLIST_ALLOW_DUPES");
  if (flags & D_STRLIST_ALLOW_EMPTY)
    buf_addstr(buf, " | D_STRLIST_ALLOW_EMPTY");
  if (flags & D_STRLIST_CASE_SENSITIVE)
    buf_addstr(buf, " | D_STRLIST_CASE_SENSITIVE");
}

static void strlist_dump(const struct StrList *list, struct Buffer *buf)
{
  if (!list || !buf)
    return;

  buf_printf(buf, "[%zu] ", list->count);

  for (GSList *np = list->head; np != NULL; np = np->next)
  {
    if (np->data)
      buf_add_printf(buf, "'%s'", (char*)np->data);
    else
      buf_addstr(buf, "NULL");
    if (np->next)
      buf_addstr(buf, ",");
  }
  TEST_MSG("%s", buf_string(buf));
  buf_reset(buf);
}

static bool test_strlist_parse(struct Buffer *err)
{
  buf_reset(err);

  static const char *init[] = {
    NULL,
    "",
    "apple",
    "apple:banana",
    "apple:banana:cherry",
    "apple\\:banana:cherry",
    ":apple",
    "banana:",
    ":",
    "::",
    "apple:banana:apple",
    "apple::banana",
  };

  uint32_t flags = D_STRLIST_SEP_COLON | D_STRLIST_ALLOW_EMPTY;
  strlist_flags(flags, err);
  TEST_MSG("Flags: %s", buf_string(err));
  TEST_MSG("");
  buf_reset(err);

  struct StrList *list = NULL;
  for (size_t i = 0; i < mutt_array_size(init); i++)
  {
    TEST_CASE_(">>%s<<", init[i] ? init[i] : "NULL");
    list = strlist_parse(init[i], flags);
    strlist_dump(list, err);
    strlist_free(&list);
  }

  return true;
}

static bool test_strlist_add_string(struct Buffer *err)
{
  {
    strlist_add_string(NULL, NULL);
  }

  {
    struct StrList *list = strlist_parse(NULL, D_STRLIST_ALLOW_EMPTY);
    strlist_dump(list, err);

    strlist_add_string(list, "");
    strlist_dump(list, err);

    strlist_free(&list);
  }

  {
    struct StrList *list = strlist_parse(NULL, D_STRLIST_ALLOW_EMPTY);
    strlist_dump(list, err);

    strlist_add_string(list, NULL);
    strlist_dump(list, err);

    strlist_add_string(list, "apple");
    strlist_dump(list, err);
    strlist_add_string(list, "banana");
    strlist_dump(list, err);
    strlist_add_string(list, "apple");
    strlist_dump(list, err);

    strlist_free(&list);
  }

  {
    struct StrList *list = strlist_parse("apple", 0);
    strlist_add_string(NULL, "apple");
    strlist_add_string(list, NULL);
    strlist_free(&list);
  }

  return true;
}

static bool test_strlist_remove_string(struct Buffer *err)
{
  {
    buf_reset(err);

    uint32_t flags = D_STRLIST_SEP_COLON | D_STRLIST_ALLOW_EMPTY;
    struct StrList *list = strlist_parse("apple:banana::cherry", flags);
    strlist_dump(list, err);

    strlist_remove_string(list, NULL);
    strlist_dump(list, err);

    strlist_remove_string(list, "apple");
    strlist_dump(list, err);

    strlist_remove_string(list, "damson");
    strlist_dump(list, err);

    strlist_free(&list);
  }

  {
    struct StrList *list = strlist_parse("apple:banana::cherry", D_STRLIST_SEP_COLON);
    TEST_CHECK(strlist_remove_string(NULL, "apple") == NULL);
    TEST_CHECK(strlist_remove_string(list, NULL) == list);
    strlist_free(&list);
  }

  return true;
}

static bool test_strlist_is_member(struct Buffer *err)
{
  {
    buf_reset(err);

    TEST_CHECK(strlist_is_member(NULL, "apple") == false);

    uint32_t flags = D_STRLIST_SEP_COLON | D_STRLIST_ALLOW_EMPTY;
    struct StrList *list = strlist_parse("apple:banana::cherry", flags);
    strlist_dump(list, err);

    static const char *values[] = { "apple", "", "damson", NULL };

    for (size_t i = 0; i < mutt_array_size(values); i++)
    {
      TEST_MSG("member '%s' : %s", values[i], strlist_is_member(list, values[i]) ? "yes" : "no");
    }

    strlist_free(&list);
  }

  {
    struct StrList *list = strlist_parse("apple", 0);
    TEST_CHECK(strlist_is_member(list, NULL) == false);
    strlist_free(&list);
  }
  return true;
}

static bool test_strlist_equal(struct Buffer *err)
{
  struct StrList *list1 = strlist_parse("apple:banana::cherry",
                                    D_STRLIST_SEP_COLON | D_STRLIST_ALLOW_EMPTY);
  strlist_dump(list1, err);

  struct StrList *list2 = strlist_parse("apple,banana,,cherry",
                                    D_STRLIST_SEP_COMMA | D_STRLIST_ALLOW_EMPTY);
  strlist_dump(list2, err);

  struct StrList *list3 = strlist_parse("apple,banana,,cherry,damson",
                                    D_STRLIST_SEP_COMMA | D_STRLIST_ALLOW_EMPTY);
  strlist_dump(list2, err);

  bool result = true;

  if (!TEST_CHECK(strlist_equal(NULL, NULL) == true))
    result = false;

  if (!TEST_CHECK(strlist_equal(list1, NULL) == false))
    result = false;

  if (!TEST_CHECK(strlist_equal(NULL, list2) == false))
    result = false;

  if (!TEST_CHECK(strlist_equal(list1, list2) == true))
    result = false;

  if (!TEST_CHECK(strlist_equal(list1, list3) == false))
    result = false;

  strlist_free(&list1);
  strlist_free(&list2);
  strlist_free(&list3);

  return result;
}

static bool test_initial_values(struct ConfigSubset *sub, struct Buffer *err)
{
  log_line(__func__);

  static const char *values[] = { "apple", "banana", "cherry", NULL };
  struct ConfigSet *cs = sub->cs;

  const struct StrList *VarApple = cs_subset_slist(sub, "Apple");
  strlist_flags(VarApple->flags, err);
  TEST_MSG("Apple, %ld items, %s flags", VarApple->count, buf_string(err));
  buf_reset(err);
  if (VarApple->count != 1)
  {
    TEST_MSG("Apple should have 1 item");
    return false;
  }

  size_t i = 0;
  for (GSList *np = VarApple->head; np != NULL; np = np->next)
  {
    if (!mutt_str_equal(values[i], np->data))
      return false;
    i++;
  }

  const struct StrList *VarBanana = cs_subset_slist(sub, "Banana");
  strlist_flags(VarBanana->flags, err);
  TEST_MSG("Banana, %ld items, %s flags", VarBanana->count, buf_string(err));
  buf_reset(err);
  if (VarBanana->count != 2)
  {
    TEST_MSG("Banana should have 2 items");
    return false;
  }

  i = 0;
  for (GSList *np = VarBanana->head; np != NULL; np = np->next)
  {
    if (!mutt_str_equal(values[i], np->data))
      return false;
    i++;
  }

  const struct StrList *VarCherry = cs_subset_slist(sub, "Cherry");
  strlist_flags(VarCherry->flags, err);
  TEST_MSG("Cherry, %ld items, %s flags", VarCherry->count, buf_string(err));
  buf_reset(err);
  if (VarCherry->count != 3)
  {
    TEST_MSG("Cherry should have 3 items");
    return false;
  }

  i = 0;
  for (GSList *np = VarCherry->head; np != NULL; np = np->next)
  {
    if (!mutt_str_equal(values[i], np->data))
      return false;
    i++;
  }

  const char *name = "Cherry";
  const char *value = "raspberry";
  int rc = cs_str_initial_set(cs, name, value, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  value = "strawberry";
  rc = cs_str_initial_set(cs, name, value, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  name = "Elderberry";
  buf_reset(err);
  rc = cs_str_initial_get(cs, name, err);
  if (!TEST_CHECK(rc == (CSR_SUCCESS | CSR_SUC_EMPTY)))
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  return true;
}

static bool test_string_set(struct ConfigSubset *sub, struct Buffer *err)
{
  log_line(__func__);

  struct ConfigSet *cs = sub->cs;
  int rc;

  buf_reset(err);
  char *name = "Damson";
  rc = cs_str_string_set(cs, name, "pig:quail:rhino", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  buf_reset(err);
  name = "Damson";
  rc = cs_str_string_set(cs, name, "", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  buf_reset(err);
  name = "Damson";
  rc = cs_str_string_set(cs, name, NULL, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  buf_reset(err);
  name = "Elderberry";
  rc = cs_str_string_set(cs, name, "pig:quail:rhino", err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  name = "Ilama";
  rc = cs_str_string_set(cs, name, "apple", err);
  TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS);

  rc = cs_str_string_set(cs, name, "banana", err);
  TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS);

  return true;
}

static bool test_string_get(struct ConfigSubset *sub, struct Buffer *err)
{
  log_line(__func__);

  struct ConfigSet *cs = sub->cs;

  struct Buffer *initial = buf_pool_get();

  buf_reset(err);
  char *name = "Fig";

  int rc = cs_str_initial_get(cs, name, initial);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  if (!mutt_str_equal(buf_string(initial), buf_string(err)))
  {
    TEST_MSG("Differ: %s '%s' '%s'", name, buf_string(initial), buf_string(err));
    return false;
  }
  TEST_MSG("Match: %s '%s' '%s'", name, buf_string(initial), buf_string(err));

  buf_reset(err);
  buf_reset(initial);
  name = "Guava";

  rc = cs_str_initial_get(cs, name, initial);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  if (!mutt_str_equal(buf_string(initial), buf_string(err)))
  {
    TEST_MSG("Differ: %s '%s' '%s'", name, buf_string(initial), buf_string(err));
    return false;
  }
  TEST_MSG("Match: %s '%s' '%s'", name, buf_string(initial), buf_string(err));

  buf_reset(err);
  buf_reset(initial);
  name = "Hawthorn";

  rc = cs_str_initial_get(cs, name, initial);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  rc = cs_str_string_get(cs, name, err);
  if (CSR_RESULT(rc) != CSR_SUCCESS)
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  if (!mutt_str_equal(buf_string(initial), buf_string(err)))
  {
    TEST_MSG("Differ: %s '%s' '%s'", name, buf_string(initial), buf_string(err));
    return false;
  }
  TEST_MSG("Match: %s '%s' '%s'", name, buf_string(initial), buf_string(err));

  buf_pool_release(&initial);
  return true;
}

static bool test_native_set(struct ConfigSubset *sub, struct Buffer *err)
{
  log_line(__func__);

  struct ConfigSet *cs = sub->cs;
  const char *init = "apple:banana::cherry";
  const char *name = "Ilama";
  struct StrList *list = strlist_parse(init, D_STRLIST_SEP_COLON);

  buf_reset(err);
  int rc = cs_str_native_set(cs, name, (intptr_t) list, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  buf_reset(err);
  rc = cs_str_string_get(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  if (!TEST_CHECK_STR_EQ(buf_string(err), init))
    return false;

  buf_reset(err);
  rc = cs_str_native_set(cs, name, (intptr_t) NULL, err);
  if (!TEST_CHECK(rc == (CSR_SUCCESS | CSR_SUC_EMPTY)))
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  strlist_free(&list);
  list = strlist_parse("apple", D_STRLIST_SEP_COLON);

  name = "Xigua";
  rc = cs_str_native_set(cs, name, (intptr_t) list, err);
  TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS);

  strlist_free(&list);
  list = strlist_parse("banana", D_STRLIST_SEP_COLON);

  rc = cs_str_native_set(cs, name, (intptr_t) list, err);
  TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS);

  strlist_free(&list);
  return true;
}

static bool test_native_get(struct ConfigSubset *sub, struct Buffer *err)
{
  log_line(__func__);

  struct ConfigSet *cs = sub->cs;
  const char *name = "Jackfruit";

  buf_reset(err);
  intptr_t value = cs_str_native_get(cs, name, err);
  struct StrList *sl = (struct StrList *) value;

  const struct StrList *VarJackfruit = cs_subset_slist(sub, "Jackfruit");
  if (!TEST_CHECK(VarJackfruit == sl))
  {
    TEST_MSG("Get failed: %s", buf_string(err));
    return false;
  }

  return true;
}

static bool test_plus_equals(struct ConfigSubset *sub, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Raspberry";
  struct ConfigSet *cs = sub->cs;

  static char *PlusTests[][3] = {
    // clang-format off
    // Initial,        Plus,     Result
    { "",              "",       ""                   }, // Add nothing to various lists
    { "one",           "",       "one"                },
    { "one:two",       "",       "one:two"            },
    { "one:two:three", "",       "one:two:three"      },

    { "",              "nine",   "nine"               }, // Add an item to various lists
    { "one",           "nine",   "one:nine"           },
    { "one:two",       "nine",   "one:two:nine"       },
    { "one:two:three", "nine",   "one:two:three:nine" },

    { "one:two:three", "one",   "one:two:three"       }, // Add a duplicate to various positions
    { "one:two:three", "two",   "one:two:three"       },
    { "one:two:three", "three", "one:two:three"       },
    // clang-format on
  };

  int rc;
  for (size_t i = 0; i < mutt_array_size(PlusTests); i++)
  {
    buf_reset(err);
    rc = cs_str_string_set(cs, name, PlusTests[i][0], err);
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("Set failed: %s", buf_string(err));
      return false;
    }

    rc = cs_str_string_plus_equals(cs, name, PlusTests[i][1], err);
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("PlusEquals failed: %s", buf_string(err));
      return false;
    }

    rc = cs_str_string_get(cs, name, err);
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("Get failed: %s", buf_string(err));
      return false;
    }

    if (!TEST_CHECK_STR_EQ(buf_string(err), PlusTests[i][2]))
      return false;
  }

  // Test a failing validator
  VarsOther[8].validator = validator_fail; // "Raspberry"
  buf_reset(err);
  rc = cs_str_string_plus_equals(cs, name, "nine", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  name = "Xigua";
  rc = cs_str_string_plus_equals(cs, name, "apple", err);
  TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS);

  return true;
}

static bool test_minus_equals(struct ConfigSubset *sub, struct Buffer *err)
{
  log_line(__func__);
  const char *name = "Strawberry";
  struct ConfigSet *cs = sub->cs;

  static char *MinusTests[][3] = {
    // clang-format off
    // Initial,        Plus,    Result
    { "",              "",      ""              }, // Remove nothing from various lists
    { "one",           "",      "one"           },
    { "one:two",       "",      "one:two"       },
    { "one:two:three", "",      "one:two:three" },

    { "",              "",      ""              }, // Remove an item from various lists
    { "one",           "one",   ""              },
    { "one:two",       "one",   "two"           },
    { "one:two",       "two",   "one"           },
    { "one:two:three", "one",   "two:three"     },
    { "one:two:three", "two",   "one:three"     },
    { "one:two:three", "three", "one:two"       },

    { "",              "nine",  ""              }, // Remove a non-existent item from various lists
    { "one",           "nine",  "one"           },
    { "one:two",       "nine",  "one:two"       },
    { "one:two:three", "nine",  "one:two:three" },
    // clang-format on
  };

  int rc;
  for (size_t i = 0; i < mutt_array_size(MinusTests); i++)
  {
    buf_reset(err);
    rc = cs_str_string_set(cs, name, MinusTests[i][0], err);
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("Set failed: %s", buf_string(err));
      return false;
    }

    rc = cs_str_string_minus_equals(cs, name, MinusTests[i][1], err);
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("MinusEquals failed: %s", buf_string(err));
      return false;
    }

    rc = cs_str_string_get(cs, name, err);
    if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    {
      TEST_MSG("Get failed: %s", buf_string(err));
      return false;
    }

    if (!TEST_CHECK_STR_EQ(buf_string(err), MinusTests[i][2]))
      return false;
  }

  // Test a failing validator
  VarsOther[9].validator = validator_fail; // "Strawberry"
  buf_reset(err);
  rc = cs_str_string_minus_equals(cs, name, "two", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  name = "Xigua";
  rc = cs_str_string_minus_equals(cs, name, "apple", err);
  TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS);

  return true;
}

static bool test_reset(struct ConfigSubset *sub, struct Buffer *err)
{
  log_line(__func__);

  struct ConfigSet *cs = sub->cs;
  const char *name = "Lemon";

  buf_reset(err);

  const struct StrList *VarLemon = cs_subset_slist(sub, "Lemon");
  char *item = VarLemon->head->data;
  TEST_MSG("Initial: %s = '%s'", name, item);
  int rc = cs_str_string_set(cs, name, "apple", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;
  VarLemon = cs_subset_slist(sub, "Lemon");
  item = VarLemon->head->data;
  TEST_MSG("Set: %s = '%s'", name, item);

  rc = cs_str_reset(cs, name, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  VarLemon = cs_subset_slist(sub, "Lemon");
  item = VarLemon->head->data;
  if (!TEST_CHECK_STR_EQ(item, "lemon"))
  {
    TEST_MSG("Value of %s wasn't changed", name);
    return false;
  }

  TEST_MSG("Reset: %s = '%s'", name, item);

  name = "Mango";
  buf_reset(err);

  const struct StrList *VarMango = cs_subset_slist(sub, "Mango");
  item = VarMango->head->data;
  TEST_MSG("Initial: %s = '%s'", name, item);
  dont_fail = true;
  rc = cs_str_string_set(cs, name, "banana", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
    return false;
  VarMango = cs_subset_slist(sub, "Mango");
  item = VarMango->head->data;
  TEST_MSG("Set: %s = '%s'", name, item);
  dont_fail = false;

  rc = cs_str_reset(cs, name, err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    return false;
  }

  VarMango = cs_subset_slist(sub, "Mango");
  item = VarMango->head->data;
  if (!TEST_CHECK_STR_EQ(item, "banana"))
  {
    TEST_MSG("Value of %s changed", name);
    return false;
  }

  item = VarMango->head->data;
  TEST_MSG("Reset: %s = '%s'", name, item);

  name = "Xigua";
  rc = cs_str_reset(cs, name, err);
  TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS);

  StartupComplete = false;
  rc = cs_str_string_set(cs, name, "banana", err);
  TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS);
  StartupComplete = true;

  rc = cs_str_reset(cs, name, err);
  TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS);

  log_line(__func__);
  return true;
}

static bool test_validator(struct ConfigSubset *sub, struct Buffer *err)
{
  log_line(__func__);

  struct ConfigSet *cs = sub->cs;
  char *item = NULL;
  struct StrList *list = strlist_parse("apple", D_STRLIST_SEP_COMMA);
  bool result = false;

  const char *name = "Nectarine";
  buf_reset(err);
  int rc = cs_str_string_set(cs, name, "banana", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    goto tv_out;
  }
  const struct StrList *VarNectarine = cs_subset_slist(sub, "Nectarine");
  item = VarNectarine->head->data;
  TEST_MSG("Address: %s = %s", name, item);

  buf_reset(err);
  rc = cs_str_native_set(cs, name, IP list, err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    goto tv_out;
  }
  VarNectarine = cs_subset_slist(sub, "Nectarine");
  item = VarNectarine->head->data;
  TEST_MSG("Native: %s = %s", name, item);

  name = "Olive";
  buf_reset(err);
  rc = cs_str_string_set(cs, name, "cherry", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    goto tv_out;
  }
  const struct StrList *VarOlive = cs_subset_slist(sub, "Olive");
  item = VarOlive->head->data;
  TEST_MSG("Address: %s = %s", name, item);

  buf_reset(err);
  rc = cs_str_native_set(cs, name, IP list, err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    goto tv_out;
  }
  VarOlive = cs_subset_slist(sub, "Olive");
  item = VarOlive->head->data;
  TEST_MSG("Native: %s = %s", name, item);

  name = "Papaya";
  buf_reset(err);
  rc = cs_str_string_set(cs, name, "damson", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    goto tv_out;
  }
  const struct StrList *VarPapaya = cs_subset_slist(sub, "Papaya");
  item = VarPapaya->head->data;
  TEST_MSG("Address: %s = %s", name, item);

  buf_reset(err);
  rc = cs_str_native_set(cs, name, IP list, err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("Expected error: %s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    goto tv_out;
  }
  VarPapaya = cs_subset_slist(sub, "Papaya");
  item = VarPapaya->head->data;
  TEST_MSG("Native: %s = %s", name, item);

  result = true;
tv_out:
  strlist_free(&list);
  log_line(__func__);
  return result;
}

static bool test_charset_validator(struct ConfigSubset *sub, struct Buffer *err)
{
  log_line(__func__);

  struct ConfigSet *cs = sub->cs;
  char *item = NULL;
  struct StrList *list = strlist_parse("utf-8", D_STRLIST_SEP_COLON);
  bool result = false;

  const char *name = "Wolfberry";
  buf_reset(err);
  int rc = cs_str_string_set(cs, name, "", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    goto tv_out;
  }

  buf_reset(err);
  rc = cs_str_string_set(cs, name, "utf-8", err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    goto tv_out;
  }
  const struct StrList *VarWolfberry = cs_subset_slist(sub, "Wolfberry");
  item = VarWolfberry->head->data;
  TEST_MSG("Address: %s = %s", name, item);

  buf_reset(err);
  rc = cs_str_native_set(cs, name, IP list, err);
  if (TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("%s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    goto tv_out;
  }
  VarWolfberry = cs_subset_slist(sub, "Wolfberry");
  item = VarWolfberry->head->data;
  TEST_MSG("Native: %s = %s", name, item);

  // When one of the charsets is invalid, it fails
  buf_reset(err);
  rc = cs_str_string_set(cs, name, "us-ascii:utf-3", err);
  if (TEST_CHECK(CSR_RESULT(rc) != CSR_SUCCESS))
  {
    TEST_MSG("%s", buf_string(err));
  }
  else
  {
    TEST_MSG("%s", buf_string(err));
    goto tv_out;
  }

  result = true;
tv_out:
  strlist_free(&list);
  log_line(__func__);
  return result;
}

static void dump_native(struct ConfigSet *cs, const char *parent, const char *child)
{
  intptr_t pval = cs_str_native_get(cs, parent, NULL);
  intptr_t cval = cs_str_native_get(cs, child, NULL);

  struct StrList *pl = (struct StrList *) pval;
  struct StrList *cl = (struct StrList *) cval;

  char *pstr = pl ? pl->head->data : NULL;
  char *cstr = cl ? cl->head->data : NULL;

  TEST_MSG("%15s = %s", parent, NONULL(pstr));
  TEST_MSG("%15s = %s", child, NONULL(cstr));
}

static bool test_inherit(struct ConfigSet *cs, struct Buffer *err)
{
  log_line(__func__);
  bool result = false;

  const char *account = "fruit";
  const char *parent = "Quince";
  char child[128];
  snprintf(child, sizeof(child), "%s:%s", account, parent);

  struct ConfigSubset *sub = cs_subset_new(NULL, NULL, SpaceMutt->notify);
  sub->cs = cs;
  struct Account *a = account_new(account, sub);

  struct HashElem *he = cs_subset_create_inheritance(a->sub, parent);
  if (!he)
  {
    TEST_MSG("Error: %s", buf_string(err));
    goto ti_out;
  }

  // set parent
  buf_reset(err);
  int rc = cs_str_string_set(cs, parent, "apple", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Error: %s", buf_string(err));
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // set child
  buf_reset(err);
  rc = cs_str_string_set(cs, child, "banana", err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Error: %s", buf_string(err));
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // reset child
  buf_reset(err);
  rc = cs_str_reset(cs, child, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Error: %s", buf_string(err));
    goto ti_out;
  }
  dump_native(cs, parent, child);

  // reset parent
  buf_reset(err);
  rc = cs_str_reset(cs, parent, err);
  if (!TEST_CHECK(CSR_RESULT(rc) == CSR_SUCCESS))
  {
    TEST_MSG("Error: %s", buf_string(err));
    goto ti_out;
  }
  dump_native(cs, parent, child);

  log_line(__func__);
  result = true;
ti_out:
  account_free(&a);
  cs_subset_free(&sub);
  return result;
}

bool strlist_test_separator(struct ConfigDef vars[], struct Buffer *err)
{
  log_line(__func__);

  struct ConfigSubset *sub = SpaceMutt->sub;
  struct ConfigSet *cs = sub->cs;

  buf_reset(err);

  StartupComplete = false;
  cs_register_type(cs, &CstStrList);
  if (!TEST_CHECK(cs_register_variables(cs, vars)))
    return false;
  StartupComplete = true;

  notify_observer_add(SpaceMutt->notify, NT_CONFIG, log_observer, 0);

  set_list(cs);

  if (!test_initial_values(sub, err))
    return false;
  if (!test_string_set(sub, err))
    return false;
  if (!test_string_get(sub, err))
    return false;

  cs_str_delete(cs, "Apple", NULL);
  cs_str_delete(cs, "Banana", NULL);
  cs_str_delete(cs, "Cherry", NULL);
  cs_str_delete(cs, "Damson", NULL);
  cs_str_delete(cs, "Elderberry", NULL);
  cs_str_delete(cs, "Fig", NULL);
  cs_str_delete(cs, "Guava", NULL);
  cs_str_delete(cs, "Hawthorn", NULL);
  cs_str_delete(cs, "Ilama", NULL);
  return true;
}

void test_config_strlist(void)
{
  log_line(__func__);

  struct Buffer *err = buf_pool_get();
  TEST_CHECK(test_strlist_parse(err));
  TEST_CHECK(test_strlist_add_string(err));
  TEST_CHECK(test_strlist_remove_string(err));
  TEST_CHECK(test_strlist_is_member(err));
  TEST_CHECK(test_strlist_equal(err));

  TEST_CHECK(strlist_test_separator(VarsColon, err));
  TEST_CHECK(strlist_test_separator(VarsComma, err));
  TEST_CHECK(strlist_test_separator(VarsSpace, err));

  struct ConfigSubset *sub = SpaceMutt->sub;
  struct ConfigSet *cs = sub->cs;

  StartupComplete = false;
  dont_fail = true;
  if (!TEST_CHECK(cs_register_variables(cs, VarsOther)))
    return;
  dont_fail = false;
  StartupComplete = true;

  notify_observer_add(SpaceMutt->notify, NT_CONFIG, log_observer, 0);

  TEST_CHECK(test_native_set(sub, err));
  TEST_CHECK(test_native_get(sub, err));
  TEST_CHECK(test_plus_equals(sub, err));
  TEST_CHECK(test_minus_equals(sub, err));
  TEST_CHECK(test_reset(sub, err));
  TEST_CHECK(test_validator(sub, err));
  TEST_CHECK(test_charset_validator(sub, err));
  TEST_CHECK(test_inherit(cs, err));
  buf_pool_release(&err);

  log_line(__func__);
}
