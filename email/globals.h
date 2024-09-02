/**
 * @file
 * Email Global Variables
 *
 * @authors
 * Copyright (C) 2023 Richard Russon <rich@flatcap.org>
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

#ifndef MUTT_EMAIL_GLOBALS_H
#define MUTT_EMAIL_GLOBALS_H

#include <stdbool.h>
#include "mutt/lib.h"

extern struct HashTable  *AutoSubscribeCache;
extern GSList *Ignore;
extern RegexList   *MailLists;
extern GSList *MailToAllow;
extern RegexList   *NoSpamList;
extern struct ReplaceList SpamList;
extern RegexList   *SubscribedLists;
extern GSList *UnIgnore;
extern RegexList   *UnMailLists;
extern RegexList   *UnSubscribedLists;

#endif /* MUTT_EMAIL_GLOBALS_H */
