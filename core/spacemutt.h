/**
 * @file
 * Container for Accounts, Notifications
 *
 * @authors
 * Copyright (C) 2019-2023 Richard Russon <rich@flatcap.org>
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

#ifndef MUTT_CORE_SPACEMUTT_H
#define MUTT_CORE_SPACEMUTT_H

#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>
#include "account.h"
#include "mailbox.h"
#ifdef __APPLE__
#include <xlocale.h>
#endif

struct ConfigSet;

/**
 * struct SpaceMutt - Container for Accounts, Notifications
 */
struct SpaceMutt
{
  struct Notify *notify;         ///< Notifications handler
  struct Notify *notify_resize;  ///< Window resize notifications handler
  struct Notify *notify_timeout; ///< Timeout notifications handler
  struct ConfigSubset *sub;      ///< Inherited config items
  AccountList *accounts;         ///< List of all Accounts
  locale_t time_c_locale;        ///< Current locale but LC_TIME=C
  mode_t user_default_umask;     ///< User's default file writing permissions (inferred from umask)
};

extern struct SpaceMutt *SpaceMutt;

/**
 * enum NotifyGlobal - Events not associated with an object
 *
 * Observers of #NT_GLOBAL will not be passed any Event data.
 */
enum NotifyGlobal
{
  NT_GLOBAL_STARTUP = 1, ///< SpaceMutt is initialised
  NT_GLOBAL_SHUTDOWN,    ///< SpaceMutt is about to close
  NT_GLOBAL_COMMAND,     ///< A SpaceMutt command
};

bool            spacemutt_account_add   (struct SpaceMutt *n, struct Account *a);
bool            spacemutt_account_remove(struct SpaceMutt *n, const struct Account *a);
void            spacemutt_free          (struct SpaceMutt **ptr);
struct SpaceMutt *spacemutt_new           (struct ConfigSet *cs);

void   spacemutt_mailboxlist_free   (MailboxList *ml);
size_t spacemutt_mailboxlist_get_all(MailboxList **head, struct SpaceMutt *n, enum MailboxType type);

// Similar to mutt_file_fopen, but with the proper permissions inferred from
#define mutt_file_fopen_masked(PATH, MODE) mutt_file_fopen_masked_full(PATH, MODE, __FILE__, __LINE__, __func__)

#endif /* MUTT_CORE_SPACEMUTT_H */
