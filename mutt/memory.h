/**
 * @file
 * Memory management wrappers
 *
 * @authors
 * Copyright (C) 2017 Richard Russon <rich@flatcap.org>
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

#ifndef MUTT_MUTT_MEMORY_H
#define MUTT_MUTT_MEMORY_H

#include <stddef.h>

#undef ROUND_UP
#define ROUND_UP(NUM, STEP) ((((NUM) + (STEP) -1) / (STEP)) * (STEP))
#define mutt_array_size(x) (sizeof(x) / sizeof((x)[0]))

#define FREE(x) ((void *)x != NULL) ? g_free(g_steal_pointer((void**)x)) : NULL

#endif /* MUTT_MUTT_MEMORY_H */
