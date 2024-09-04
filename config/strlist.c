/**
 * @file
 * Type representing a list of strings
 *
 * @authors
 * Copyright (C) 2019-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2020 Jakub Jindra <jakub.jindra@socialbakers.com>
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

/**
 * @page config_slist Type: List of strings
 *
 * Config type representing a list of strings.
 *
 * - Backed by `struct StrList`
 * - Empty string list is stored as `NULL`
 * - Validator is passed `struct StrList`, which may be `NULL`
 * - Data is freed when `ConfigSet` is freed
 * - Implementation: #CstStrList
 */

#include "config.h"
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include "mutt/lib.h"
#include "set.h"
#include "types.h"

/**
 * strlist_destroy - Destroy a StrList object - Implements ConfigSetType::destroy() - @ingroup cfg_type_destroy
 */
static void strlist_destroy(const struct ConfigSet *cs, void *var, const struct ConfigDef *cdef)
{
  if (!cs || !var || !cdef)
    return; /* LCOV_EXCL_LINE */

  struct StrList **l = (struct StrList **) var;
  if (!*l)
    return;

  strlist_free(l);
}

/**
 * strlist_string_set - Set a StrList by string - Implements ConfigSetType::string_set() - @ingroup cfg_type_string_set
 */
static int strlist_string_set(const struct ConfigSet *cs, void *var, struct ConfigDef *cdef,
                              const char *value, struct Buffer *err)
{
  if (!cs || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  /* Store empty string list as NULL */
  if (value && (value[0] == '\0'))
    value = NULL;

  struct StrList *list = NULL;

  int rc = CSR_SUCCESS;

  if (var)
  {
    list = strlist_parse(value, cdef->type);

    if (strlist_equal(list, *(struct StrList **) var))
    {
      strlist_free(&list);
      return CSR_SUCCESS | CSR_SUC_NO_CHANGE;
    }

    if (startup_only(cdef, err))
    {
      strlist_free(&list);
      return CSR_ERR_INVALID | CSR_INV_VALIDATOR;
    }

    if (cdef->validator)
    {
      rc = cdef->validator(cs, cdef, (intptr_t) list, err);

      if (CSR_RESULT(rc) != CSR_SUCCESS)
      {
        strlist_free(&list);
        return rc | CSR_INV_VALIDATOR;
      }
    }

    strlist_destroy(cs, var, cdef);

    *(struct StrList **) var = list;

    if (!list)
      rc |= CSR_SUC_EMPTY;
  }
  else
  {
    if (cdef->type & D_INTERNAL_INITIAL_SET)
      FREE(&cdef->initial);

    cdef->type |= D_INTERNAL_INITIAL_SET;
    cdef->initial = (intptr_t) mutt_str_dup(value);
  }

  return rc;
}

/**
 * strlist_string_get - Get a StrList as a string - Implements ConfigSetType::string_get() - @ingroup cfg_type_string_get
 */
static int strlist_string_get(const struct ConfigSet *cs, void *var,
                              const struct ConfigDef *cdef, struct Buffer *result)
{
  if (!cs || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  if (var)
  {
    struct StrList *list = *(struct StrList **) var;
    if (!list)
      return CSR_SUCCESS | CSR_SUC_EMPTY; /* empty string */

    strlist_to_buffer(list, result);
  }
  else
  {
    buf_addstr(result, (char *) cdef->initial);
  }

  int rc = CSR_SUCCESS;
  if (buf_is_empty(result))
    rc |= CSR_SUC_EMPTY;

  return rc;
}

/**
 * strlist_native_set - Set a StrList config item by StrList - Implements ConfigSetType::native_set() - @ingroup cfg_type_native_set
 */
static int strlist_native_set(const struct ConfigSet *cs, void *var,
                              const struct ConfigDef *cdef, intptr_t value, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  int rc;

  if (strlist_equal((struct StrList *) value, *(struct StrList **) var))
    return CSR_SUCCESS | CSR_SUC_NO_CHANGE;

  if (startup_only(cdef, err))
    return CSR_ERR_INVALID | CSR_INV_VALIDATOR;

  if (cdef->validator)
  {
    rc = cdef->validator(cs, cdef, value, err);

    if (CSR_RESULT(rc) != CSR_SUCCESS)
      return rc | CSR_INV_VALIDATOR;
  }

  strlist_free(var);

  struct StrList *list = strlist_dup((struct StrList *) value);

  rc = CSR_SUCCESS;
  if (!list)
    rc |= CSR_SUC_EMPTY;

  *(struct StrList **) var = list;
  return rc;
}

/**
 * strlist_native_get - Get a StrList from a StrList config item - Implements ConfigSetType::native_get() - @ingroup cfg_type_native_get
 */
static intptr_t strlist_native_get(const struct ConfigSet *cs, void *var,
                                   const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return INT_MIN; /* LCOV_EXCL_LINE */

  struct StrList *list = *(struct StrList **) var;

  return (intptr_t) list;
}

/**
 * strlist_string_plus_equals - Add to a StrList by string - Implements ConfigSetType::string_plus_equals() - @ingroup cfg_type_string_plus_equals
 */
static int strlist_string_plus_equals(const struct ConfigSet *cs, void *var,
                                      const struct ConfigDef *cdef,
                                      const char *value, struct Buffer *err)
{
  if (!cs || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  int rc = CSR_SUCCESS;

  /* Store empty strings as NULL */
  if (value && (value[0] == '\0'))
    value = NULL;

  if (!value)
    return rc | CSR_SUC_NO_CHANGE;

  if (startup_only(cdef, err))
    return CSR_ERR_INVALID | CSR_INV_VALIDATOR;

  struct StrList *orig = *(struct StrList **) var;
  if (strlist_is_member(orig, value))
    return rc | CSR_SUC_NO_CHANGE;

  struct StrList *copy = strlist_dup(orig);
  if (!copy)
    copy = strlist_new(cdef->type & D_STRLIST_SEP_MASK);

  strlist_add_string(copy, value);

  if (cdef->validator)
  {
    rc = cdef->validator(cs, cdef, (intptr_t) copy, err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      strlist_free(&copy);
      return rc | CSR_INV_VALIDATOR;
    }
  }

  strlist_free(&orig);
  *(struct StrList **) var = copy;

  return rc;
}

/**
 * strlist_string_minus_equals - Remove from a StrList by string - Implements ConfigSetType::string_minus_equals() - @ingroup cfg_type_string_minus_equals
 */
static int strlist_string_minus_equals(const struct ConfigSet *cs, void *var,
                                       const struct ConfigDef *cdef,
                                       const char *value, struct Buffer *err)
{
  if (!cs || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  int rc = CSR_SUCCESS;

  /* Store empty strings as NULL */
  if (value && (value[0] == '\0'))
    value = NULL;

  if (!value)
    return rc | CSR_SUC_NO_CHANGE;

  if (startup_only(cdef, err))
    return CSR_ERR_INVALID | CSR_INV_VALIDATOR;

  struct StrList *orig = *(struct StrList **) var;
  if (!strlist_is_member(orig, value))
    return rc | CSR_SUC_NO_CHANGE;

  struct StrList *copy = strlist_dup(orig);
  strlist_remove_string(copy, value);

  if (cdef->validator)
  {
    rc = cdef->validator(cs, cdef, (intptr_t) copy, err);
    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      strlist_free(&copy);
      return rc | CSR_INV_VALIDATOR;
    }
  }

  strlist_free(&orig);
  *(struct StrList **) var = copy;

  return rc;
}

/**
 * strlist_reset - Reset a StrList to its initial value - Implements ConfigSetType::reset() - @ingroup cfg_type_reset
 */
static int strlist_reset(const struct ConfigSet *cs, void *var,
                         const struct ConfigDef *cdef, struct Buffer *err)
{
  if (!cs || !var || !cdef)
    return CSR_ERR_CODE; /* LCOV_EXCL_LINE */

  struct StrList *list = NULL;
  const char *initial = (const char *) cdef->initial;

  if (initial)
    list = strlist_parse(initial, cdef->type);

  if (strlist_equal(list, *(struct StrList **) var))
  {
    strlist_free(&list);
    return CSR_SUCCESS | CSR_SUC_NO_CHANGE;
  }

  if (startup_only(cdef, err))
  {
    strlist_free(&list);
    return CSR_ERR_INVALID | CSR_INV_VALIDATOR;
  }

  int rc = CSR_SUCCESS;

  if (cdef->validator)
  {
    rc = cdef->validator(cs, cdef, (intptr_t) list, err);

    if (CSR_RESULT(rc) != CSR_SUCCESS)
    {
      strlist_destroy(cs, &list, cdef);
      return rc | CSR_INV_VALIDATOR;
    }
  }

  if (!list)
    rc |= CSR_SUC_EMPTY;

  strlist_destroy(cs, var, cdef);

  *(struct StrList **) var = list;
  return rc;
}

/**
 * CstStrList - Config type representing a list of strings
 */
const struct ConfigSetType CstStrList = {
  DT_STRLIST,
  "strlist",
  strlist_string_set,
  strlist_string_get,
  strlist_native_set,
  strlist_native_get,
  strlist_string_plus_equals,
  strlist_string_minus_equals,
  strlist_reset,
  strlist_destroy,
};
