/**
 * @file
 * Store attributes associated with a MIME part
 *
 * @authors
 * Copyright (C) 2017-2019 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2018-2020 Pietro Cerutti <gahr@gahr.ch>
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
 * @page email_parameter Attributes for a MIME part
 *
 * Store attributes associated with a MIME part
 */

#include "config.h"
#include <stdbool.h>
#include <stddef.h>
#include "mutt/lib.h"
#include "parameter.h"

/**
 * mutt_param_new - Create a new Parameter
 * @retval ptr Newly allocated Parameter
 */
struct Parameter *mutt_param_new(void)
{
  return mutt_mem_calloc(1, sizeof(struct Parameter));
}

/**
 * mutt_param_free - Free a Parameter
 * @param[out] p Parameter to free
 */
void mutt_param_free(struct Parameter **p)
{
  if (!p || !*p)
    return;
  FREE(&(*p)->attribute);
  FREE(&(*p)->value);
  FREE(p);
}

/**
 * mutt_paramlist_clear - Clear a ParameterList
 * @param pl ParameterList to free
 */
void mutt_paramlist_clear(ParameterList *pl)
{
  if (!pl)
    return;

  struct Parameter *p = NULL;
  while ((p = g_queue_pop_tail(pl)) != NULL)
  {
    mutt_param_free(&p);
  }
}

/**
 * mutt_paramlist_free_full - Clear and Free a ParameterList
 * @param pl ParameterList to free
 */
void mutt_paramlist_free_full(ParameterList *pl)
{
  if (!pl)
    return;

  mutt_paramlist_clear(pl);
  g_queue_free(pl);
}

/**
 * mutt_param_get - Find a matching Parameter
 * @param pl ParameterList
 * @param s  String to match
 * @retval ptr Matching Parameter
 * @retval NULL No match
 */
char *mutt_param_get(const ParameterList *pl, const char *s)
{
  if (!pl)
    return NULL;


  for (GList *np = pl->head; np != NULL; np = np->next)
  {
    struct Parameter *p = np->data;
    if (mutt_istr_equal(s, p->attribute))
      return p->value;
  }

  return NULL;
}

/**
 * mutt_param_set - Set a Parameter
 * @param[in]  pl        ParameterList
 * @param[in]  attribute Attribute to match
 * @param[in]  value     Value to set
 *
 * @note If value is NULL, the Parameter will be deleted
 *
 * @note If a matching Parameter isn't found a new one will be allocated.
 *       The new Parameter will be inserted at the front of the list.
 */
void mutt_param_set(ParameterList *pl, const char *attribute, const char *value)
{
  if (!pl)
    return;

  if (!value)
  {
    mutt_param_delete(pl, attribute);
    return;
  }

  for (GList *np = pl->head; np != NULL; np = np->next)
  {
    struct Parameter *p = np->data;
    if (mutt_istr_equal(attribute, p->attribute))
    {
      mutt_str_replace(&p->value, value);
      return;
    }
  }

  struct Parameter *p = mutt_param_new();
  p->attribute = mutt_str_dup(attribute);
  p->value = mutt_str_dup(value);
  g_queue_push_head(pl, p);
}

/**
 * mutt_param_delete - Delete a matching Parameter
 * @param[in] pl        ParameterList
 * @param[in] attribute Attribute to match
 */
void mutt_param_delete(ParameterList *pl, const char *attribute)
{
  if (!pl)
    return;

  for (GList *np = pl->head; np != NULL; np = np->next)
  {
    struct Parameter *p = np->data;
    if (mutt_istr_equal(attribute, p->attribute))
    {
      g_queue_remove(pl, p);
      mutt_param_free(&p);
      return;
    }
  }
}

/**
 * mutt_paramlist_cmp_strict - Strictly compare two ParameterLists
 * @param pl1 First parameter
 * @param pl2 Second parameter
 * @retval true Parameters are strictly identical
 */
bool mutt_paramlist_cmp_strict(const ParameterList *pl1, const ParameterList *pl2)
{
  if (!pl1 && !pl2)
    return false;

  if ((pl1 == NULL) ^ (pl2 == NULL))
    return true;

  if (pl1->length != pl2->length)
  {
    return false;
  }

  GList *iter1 = pl1->head;
  GList *iter2 = pl2->head;

  while (iter1 != NULL && iter2 != NULL)
  {
    struct Parameter *p1 = iter1->data;
    struct Parameter *p2 = iter2->data;
    if (!mutt_str_equal(p1->attribute, p2->attribute) ||
        !mutt_str_equal(p1->value, p2->value))
    {
      return false;
    }
    iter1 = iter1->next;
    iter2 = iter2->next;
  }

  return true;
}
