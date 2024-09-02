/**
 * @file
 * A group of associated Mailboxes
 *
 * @authors
 * Copyright (C) 2018-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2020-2022 Pietro Cerutti <gahr@gahr.ch>
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
 * @page core_account Account object
 *
 * A group of associated Mailboxes
 */

#include "config.h"
#include <stddef.h>
#include "mutt/lib.h"
#include "config/lib.h"
#include "account.h"
#include "mailbox.h"
#include "neomutt.h"

/**
 * account_new - Create a new Account
 * @param name Name for the Account
 * @param sub  Parent Config Subset
 * @retval ptr New Account
 */
struct Account *account_new(const char *name, struct ConfigSubset *sub)
{
  if (!sub)
    return NULL;

  struct Account *a = mutt_mem_calloc(1, sizeof(struct Account));

  a->mailboxes = NULL;
  a->notify = notify_new();
  a->name = mutt_str_dup(name);
  a->sub = cs_subset_new(name, sub, a->notify);
  a->sub->cs = sub->cs;
  a->sub->scope = SET_SCOPE_ACCOUNT;

  return a;
}

/**
 * account_mailbox_add - Add a Mailbox to an Account
 * @param a Account
 * @param m Mailbox to add
 * @retval true Mailbox was added
 */
bool account_mailbox_add(struct Account *a, struct Mailbox *m)
{
  if (!a || !m)
    return false;

  if (a->type == MUTT_UNKNOWN)
    a->type = m->type;

  m->account = a;
  a->mailboxes = g_slist_append(a->mailboxes, m);
  mailbox_set_subset(m, a->sub);
  notify_set_parent(m->notify, a->notify);

  log_notify("NT_MAILBOX_ADD: %s %p",
             mailbox_get_type_name(m->type), (void *) m);
  struct EventMailbox ev_m = { m };
  notify_send(a->notify, NT_MAILBOX, NT_MAILBOX_ADD, &ev_m);
  return true;
}

/**
 * account_mailbox_remove - Remove a Mailbox from an Account
 * @param a Account
 * @param m Mailbox to remove
 * @retval true On success
 *
 * @note If m is NULL, all the mailboxes will be removed and FREE'd. Otherwise,
 * the specified mailbox is removed from the Account but not FREE'd.
 */
bool account_mailbox_remove(struct Account *a, struct Mailbox *m)
{
  if (!a || !a->mailboxes)
    return false;

  if (!m)
  {
    log_notify("NT_MAILBOX_DELETE_ALL");
    struct EventMailbox ev_m = { NULL };
    notify_send(a->notify, NT_MAILBOX, NT_MAILBOX_DELETE_ALL, &ev_m);
  }

  bool result = false;
  for (GSList *np = a->mailboxes; np != NULL;)
  {
    struct Mailbox *mn = np->data;
    np = np->next;
    if (m && (mn != m))
      continue;

    a->mailboxes = g_slist_remove(a->mailboxes, mn);
    if (m)
    {
      m->account = NULL;
      notify_set_parent(m->notify, NeoMutt->notify);
    }
    else
    {
      // we make it invisible here to force the deletion of the mailbox
      mn->visible = false;
      mailbox_free(&mn);
    }
    result = true;
    if (m)
      break;
  }

  return result;
}

/**
 * account_free - Free an Account
 * @param[out] ptr Account to free
 */
void account_free(struct Account **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct Account *a = *ptr;

  log_notify("NT_ACCOUNT_DELETE: %s %p",
             mailbox_get_type_name(a->type), (void *) a);
  struct EventAccount ev_a = { a };
  notify_send(a->notify, NT_ACCOUNT, NT_ACCOUNT_DELETE, &ev_a);

  account_mailbox_remove(a, NULL);

  if (a->adata && a->adata_free)
    a->adata_free(&a->adata);

  cs_subset_free(&a->sub);
  FREE(&a->name);
  notify_free(&a->notify);

  FREE(ptr);
}
