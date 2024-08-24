/**
 * @file
 * Store attributes associated with a MIME part
 *
 * @authors
 * Copyright (C) 2017-2019 Richard Russon <rich@flatcap.org>
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

#ifndef MUTT_EMAIL_PARAMETER_H
#define MUTT_EMAIL_PARAMETER_H

#include <stdbool.h>
#include <glib.h>
#include "mutt/lib.h"

/**
 * struct Parameter - Attribute associated with a MIME part
 */
struct Parameter
{
  char *attribute;                ///< Parameter name
  char *value;                    ///< Parameter value
};
typedef GQueue ParameterList;

bool              mutt_paramlist_cmp_strict(const ParameterList *pl1, const ParameterList *pl2);
void              mutt_paramlist_clear     (ParameterList *pl);
void              mutt_paramlist_free_full (ParameterList *pl);
void              mutt_param_delete    (ParameterList *pl, const char *attribute);
char *            mutt_param_get       (const ParameterList *pl, const char *s);
void              mutt_param_free      (struct Parameter **pl);
struct Parameter *mutt_param_new       (void);
void              mutt_param_set       (ParameterList *pl, const char *attribute, const char *value);

#endif /* MUTT_EMAIL_PARAMETER_H */
