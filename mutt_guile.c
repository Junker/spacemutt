/**
 * @file
 * Integrated Guile scripting
 * Adapted from mutt_lua
 *
 * @authors
 * Copyright (C) 2024 Dmitrii Kosenkov
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

#include "config.h"
#include <libguile.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "mutt/lib.h"
#include "config/lib.h"
#include "core/lib.h"
#include "parse/lib.h"
#include "muttlib.h"

static char* symbol_to_string(SCM symbol) {
    if (scm_is_symbol(symbol)) {
        SCM string_part = scm_symbol_to_string(symbol);
        return scm_to_latin1_string(string_part);
    }
    return NULL;
}

static SCM dt_quad_to_symbol(unsigned char val)
{
  switch(val)
  {
    case MUTT_NO:
      return scm_from_latin1_symbol("no");
    case MUTT_YES:
      return scm_from_latin1_symbol("yes");
    case MUTT_ASKNO:
      return scm_from_latin1_symbol("ask-no");
    case MUTT_ASKYES:
      return scm_from_latin1_symbol("ask-yes");
  }
}

static char symbol_to_dt_quad(SCM val)
{
  char *name = symbol_to_string(val);
  if (strcmp(name, "no") == 0)
  {
      return MUTT_NO;
  }
  else if (strcmp(name, "yes") == 0)
  {
      return MUTT_YES;
  }
  else if (strcmp(name, "ask-no") == 0)
  {
      return MUTT_ASKNO;
  }
  else if (strcmp(name, "ask-yes") == 0)
  {
      return MUTT_ASKYES;
  }

  return -1;
}

/**
 * guile_mutt_call - Call a NeoMutt command by name
 */
static SCM guile_mutt_call(SCM scm_command, SCM scm_args)
{
  mutt_debug(LL_DEBUG2, " * guile_mutt_call()\n");
  struct Buffer *err = buf_pool_get();
  struct Buffer *token = buf_pool_get();
  struct Buffer *buf = buf_pool_get();
  const struct Command *cmd = NULL;
  SCM rc = SCM_BOOL_F;

  cmd = command_get(scm_to_locale_string(scm_command));
  if (!cmd)
  {
    scm_misc_error(__func__, "Error command ~S not found.", scm_list_1(scm_command));
    return SCM_BOOL_F;
  }

  for (int i = 0; i < scm_to_int(scm_length(scm_args)); i++)
  {
    buf_addstr(buf, scm_to_locale_string(scm_list_ref(scm_args, scm_from_int(i))));
    buf_addch(buf, ' ');
  }

  if (cmd->parse(token, buf, cmd->data, err))
  {
    scm_misc_error(__func__, "SpaceMutt error: ~S", scm_from_locale_string(buf_string(err)));
  }
  else
  {
    rc = SCM_BOOL_T;
  }

  buf_pool_release(&buf);
  buf_pool_release(&token);
  buf_pool_release(&err);
  return rc;
}


/**
 * guile_mutt_set - Set a NeoMutt variable
 */
static SCM guile_mutt_set(SCM scm_param, SCM scm_value)
{
  const char *param = scm_to_locale_string(scm_param);
  mutt_debug(LL_DEBUG2, " * mutt_set(%s)\n", param);

  struct Buffer *err = buf_pool_get();
  struct HashElem *he = cs_subset_lookup(NeoMutt->sub, param);
  if (!he)
  {
    // In case it is a my_var, we have to create it
    if (mutt_str_startswith(param, "my_"))
    {
      struct ConfigDef my_cdef = { 0 };
      my_cdef.name = param;
      my_cdef.type = DT_MYVAR;
      he = cs_create_variable(NeoMutt->sub->cs, &my_cdef, err);
      if (!he)
        return SCM_BOOL_F;
    }
    else
    {
      scm_misc_error(__func__, "SpaceMutt parameter not found ~S", scm_list_1(scm_param));
      return SCM_BOOL_F;
    }
  }

  struct ConfigDef *cdef = he->data;

  SCM rc;

  switch (DTYPE(cdef->type))
  {
    case DT_ADDRESS:
    case DT_ENUM:
    case DT_MBTABLE:
    case DT_MYVAR:
    case DT_PATH:
    case DT_REGEX:
    case DT_SLIST:
    case DT_SORT:
    case DT_STRING:
    {
      const char *value = scm_to_locale_string(scm_value);
      struct Buffer *value_buf = buf_pool_get();
      buf_strcpy_n(value_buf, value, strlen(value));
      if (DTYPE(he->type) == DT_PATH)
        buf_expand_path(value_buf);

      int rv = cs_subset_he_string_set(NeoMutt->sub, he, buf_string(value_buf), err);
      buf_pool_release(&value_buf);
      if (CSR_RESULT(rv) != CSR_SUCCESS)
        rc = SCM_UNDEFINED;
      break;
    }
    case DT_NUMBER:
    case DT_QUAD:
    {
      const char value = symbol_to_dt_quad(scm_value);
      if (value == -1)
      {
        scm_misc_error(__func__, "Wrong value ~S for param ~S quadoption type.", scm_list_2(scm_value, scm_param));
        rc = SCM_UNDEFINED;
        break;
      }
      int rv = cs_subset_he_native_set(NeoMutt->sub, he, value, err);
      if (CSR_RESULT(rv) != CSR_SUCCESS)
        rc = SCM_UNDEFINED;
      break;
    }
    case DT_BOOL:
    {
      const bool value = scm_to_bool(scm_value);
      int rv = cs_subset_he_native_set(NeoMutt->sub, he, value, err);
      if (CSR_RESULT(rv) != CSR_SUCCESS)
        rc = SCM_UNDEFINED;
      break;
    }
    default:
      scm_misc_error(__func__, "Unsupported SpaceMutt parameter type ~D for ~S", scm_list_2(scm_from_uchar(DTYPE(cdef->type)), scm_param));
      rc = SCM_UNDEFINED;
      break;
  }

  buf_pool_release(&err);
  return rc;
}



/**
 * guile_mutt_get - Get a NeoMutt variable
 */
static SCM guile_mutt_get(SCM scm_param)
{
  char *param = scm_to_locale_string(scm_param);
  mutt_debug(LL_DEBUG2, " * mutt_get(%s)\n", param);

  struct HashElem *he = cs_subset_lookup(NeoMutt->sub, param);
  if (!he)
  {
    mutt_debug(LL_DEBUG2, " * error\n");
    scm_misc_error(__func__, "SpaceMutt parameter not found ~S", scm_list_1(scm_param));
    return SCM_UNDEFINED;
  }

  struct ConfigDef *cdef = he->data;

  switch (DTYPE(cdef->type))
  {
    case DT_ADDRESS:
    case DT_ENUM:
    case DT_MBTABLE:
    case DT_MYVAR:
    case DT_REGEX:
    case DT_SLIST:
    case DT_SORT:
    case DT_STRING:
    {
      struct Buffer *value = buf_pool_get();
      int rc = cs_subset_he_string_get(NeoMutt->sub, he, value);
      if (CSR_RESULT(rc) != CSR_SUCCESS)
      {
        buf_pool_release(&value);
        return SCM_UNDEFINED;
      }

      struct Buffer *escaped = buf_pool_get();
      escape_string(escaped, buf_string(value));
      SCM scm_str = scm_from_locale_string(buf_string(escaped));
      buf_pool_release(&value);
      buf_pool_release(&escaped);
      return scm_str;
    }
    case DT_QUAD:
      return dt_quad_to_symbol((unsigned char) cdef->var);
    case DT_NUMBER:
      return scm_from_char((signed char) cdef->var);
    case DT_BOOL:
      return scm_from_bool((bool) cdef->var);
    default:
      scm_misc_error(__func__, "SpaceMutt parameter type ~D unknown for ~S", scm_list_2(scm_from_uchar(DTYPE(cdef->type)), scm_param));
      return SCM_UNDEFINED;
  }
}
/**
 * guile_mutt_message - Display a message in Neomutt
 */
static SCM guile_mutt_message(SCM scm_msg)
{
  mutt_debug(LL_DEBUG2, " * mutt_message()\n");
  const char *msg = scm_to_locale_string(scm_msg);
  if (msg)
    mutt_message("%s", msg);
  return 0;
}



static void *functions_register()
{
  scm_c_define_gsubr("mutt-get", 1, 0, 0, guile_mutt_get);
  scm_c_define_gsubr("mutt-set!", 2, 0, 0, guile_mutt_set);
  scm_c_define_gsubr("mutt-message", 1, 0, 0, guile_mutt_message);
  scm_c_define_gsubr("mutt-call", 2, 0, 0, guile_mutt_call);
  return NULL;
}



bool mutt_guile_load_config(const char *path)
{
  scm_c_primitive_load(path);
  return true; //TODO
}

void mutt_guile_run_hook(char *name, SCM args)
{
  /* scm_c_run_hook(scm_c_public_ref("guile", name), args); */
  scm_c_run_hook(scm_variable_ref(scm_c_lookup(name)), args);

}
/**
 * mutt_guile_init - Setup feature commands
 */
void mutt_guile_init(void)
{
  scm_init_guile();
  functions_register();
}
