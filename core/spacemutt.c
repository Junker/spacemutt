/**
 * @file
 * Container for Accounts, Notifications
 *
 * @authors
 * Copyright (C) 2019-2023 Richard Russon <rich@flatcap.org>
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

/**
 * @page core_neomutt NeoMutt object
 *
 * Container for Accounts, Notifications
 */

#include "config.h"
#include <errno.h>
#include <locale.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "mutt/lib.h"
#include "config/lib.h"
#include "spacemutt.h"
#include "account.h"
#include "mailbox.h"

struct SpaceMutt *SpaceMutt = NULL; ///< Global NeoMutt object

/**
 * spacemutt_new - Create the main SpaceMutt object
 * @param cs Config Set
 * @retval ptr New NeoMutt
 */
struct SpaceMutt *spacemutt_new(struct ConfigSet *cs)
{
  if (!cs)
    return NULL;

  struct SpaceMutt *n = g_new0(struct SpaceMutt, 1);

  n->accounts = g_queue_new();
  n->notify = notify_new();
  n->sub = cs_subset_new(NULL, NULL, n->notify);
  n->sub->cs = cs;
  n->sub->scope = SET_SCOPE_NEOMUTT;

  n->time_c_locale = duplocale(LC_GLOBAL_LOCALE);
  if (n->time_c_locale)
    n->time_c_locale = newlocale(LC_TIME_MASK, "C", n->time_c_locale);

  if (!n->time_c_locale)
  {
    log_fault("%s", strerror(errno)); // LCOV_EXCL_LINE
    mutt_exit(1);                      // LCOV_EXCL_LINE
  }

  n->notify_timeout = notify_new();
  notify_set_parent(n->notify_timeout, n->notify);

  n->notify_resize = notify_new();
  notify_set_parent(n->notify_resize, n->notify);

  return n;
}

/**
 * spacemutt_free - Free a NeoMutt
 * @param[out] ptr NeoMutt to free
 */
void spacemutt_free(struct SpaceMutt **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct SpaceMutt *n = *ptr;

  spacemutt_account_remove(n, NULL);
  if (n->accounts)
    g_queue_free(n->accounts);
  cs_subset_free(&n->sub);
  notify_free(&n->notify_resize);
  notify_free(&n->notify_timeout);
  notify_free(&n->notify);
  if (n->time_c_locale)
    freelocale(n->time_c_locale);

  FREE(ptr);
}

/**
 * spacemutt_account_add - Add an Account to the global list
 * @param n NeoMutt
 * @param a Account to add
 * @retval true Account was added
 */
bool spacemutt_account_add(struct SpaceMutt *n, struct Account *a)
{
  if (!n || !a)
    return false;

  g_queue_push_tail(n->accounts, a);
  notify_set_parent(a->notify, n->notify);

  log_notify("NT_ACCOUNT_ADD: %s %p",
             mailbox_get_type_name(a->type), (void *) a);
  struct EventAccount ev_a = { a };
  notify_send(n->notify, NT_ACCOUNT, NT_ACCOUNT_ADD, &ev_a);
  return true;
}

/**
 * spacemutt_account_remove - Remove an Account from the global list
 * @param n NeoMutt
 * @param a Account to remove
 * @retval true Account was removed
 *
 * @note If a is NULL, all the Accounts will be removed
 */
bool spacemutt_account_remove(struct SpaceMutt *n, const struct Account *a)
{
  if (!n || !n->accounts || g_queue_is_empty(n->accounts))
    return false;

  if (!a)
  {
    log_notify("NT_ACCOUNT_DELETE_ALL");
    struct EventAccount ev_a = { NULL };
    notify_send(n->notify, NT_ACCOUNT, NT_ACCOUNT_DELETE_ALL, &ev_a);
  }

  bool result = false;
  for (GList *np = n->accounts->head; np != NULL; np = np->next)
  {
    struct Account *acc = np->data;
    if (a && (acc != a))
      continue;

    g_queue_remove(n->accounts, acc);
    account_free(&acc);
    result = true;
    if (a)
      break;
  }
  return result;
}

/**
 * spacemutt_mailboxlist_free - Free a Mailbox List
 * @param ml Mailbox List to free
 *
 * @note The Mailboxes aren't freed
 */
void spacemutt_mailboxlist_free(MailboxList *ml)
{
  if (!ml)
    return;

  g_slist_free(ml);
}

/**
 * spacemutt_mailboxlist_get_all - Get a List of all Mailboxes
 * @param head List to store the Mailboxes
 * @param n    NeoMutt
 * @param type Type of Account to match, see #MailboxType
 * @retval num Number of Mailboxes in the List
 *
 * @note If type is #MUTT_MAILBOX_ANY then all Mailbox types will be matched
 */
size_t spacemutt_mailboxlist_get_all(MailboxList **head, struct SpaceMutt *n,
                                   enum MailboxType type)
{
  if (!n)
    return 0;

  size_t count = 0;

  for (GList *np = n->accounts->head; np != NULL; np = np->next)
  {
    struct Account *a = np->data;
    if ((type > MUTT_UNKNOWN) && (a->type != type))
      continue;

    *head = g_slist_concat(*head, g_slist_copy(a->mailboxes));
    count += g_slist_length(a->mailboxes);
  }

  return count;
}

/**
 * mutt_file_fopen_masked_full - Wrapper around mutt_file_fopen_full()
 * @param path  Filename
 * @param mode  Mode e.g. "r" readonly; "w" read-write
 * @param file  Source file
 * @param line  Source line number
 * @param func  Source function
 * @retval ptr  FILE handle
 * @retval NULL Error, see errno
 *
 * Apply the user's umask, then call mutt_file_fopen_full().
 */
FILE *mutt_file_fopen_masked_full(const char *path, const char *mode,
                                  const char *file, int line, const char *func)
{
  // Set the user's umask (saved on startup)
  mode_t old_umask = umask(SpaceMutt->user_default_umask);
  log_debug3("umask set to %03o", SpaceMutt->user_default_umask);

  // The permissions will be limited by the umask
  FILE *fp = mutt_file_fopen_full(path, mode, 0666, file, line, func);

  umask(old_umask); // Immediately restore the umask
  log_debug3("umask set to %03o", old_umask);

  return fp;
}
