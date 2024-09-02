/**
 * @file
 * Email Global Variables
 *
 * @authors
 * Copyright (C) 2018-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2019 Pietro Cerutti <gahr@gahr.ch>
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
 * @page email_globals Global Variables
 *
 * These global variables are private to the email library.
 */

#include "config.h"
#include <stddef.h>
#include <glib.h>
#include "mutt/lib.h"

/* Global variables */
///< Hash Table: "mailto:" -> AutoSubscribeCache
struct HashTable *AutoSubscribeCache = NULL;
///< List of header patterns to ignore
GSList *Ignore = NULL;
///< List of regexes to match mailing lists
RegexList *MailLists = NULL;
///< List of permitted fields in a mailto: url
GSList *MailToAllow = NULL;
///< List of regexes to identify non-spam emails
RegexList *NoSpamList = NULL;
///< List of regexes and patterns to match spam emails
ReplaceList *SpamList = NULL;
///< List of regexes to match subscribed mailing lists
RegexList *SubscribedLists = NULL;
///< List of header patterns to unignore (see)
GSList *UnIgnore = NULL;
///< List of regexes to exclude false matches in MailLists
RegexList *UnMailLists = NULL;
///< List of regexes to exclude false matches in SubscribedLists
RegexList *UnSubscribedLists = NULL;
