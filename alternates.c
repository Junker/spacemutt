/**
 * @file
 * Alternate address handling
 *
 * @authors
 * Copyright (C) 2021-2023 Richard Russon <rich@flatcap.org>
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
 * @page neo_alternates Alternate address handling
 *
 * Alternate address handling
 */

#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "mutt/lib.h"
#include "address/lib.h"
#include "email/lib.h"
#include "core/lib.h"
#include "alternates.h"
#include "parse/lib.h"
#include "commands.h"
#include "mview.h"

static RegexList *Alternates = NULL; ///< List of regexes to match the user's alternate email addresses
static RegexList *UnAlternates = NULL; ///< List of regexes to exclude false matches in Alternates
static struct Notify *AlternatesNotify = NULL; ///< Notifications: #NotifyAlternates

/**
 * alternates_cleanup - Free the alternates lists
 */
void alternates_cleanup(void)
{
  notify_free(&AlternatesNotify);

  mutt_regexlist_free_full(g_steal_pointer(&Alternates));
  mutt_regexlist_free_full(g_steal_pointer(&UnAlternates));
}

/**
 * alternates_init - Set up the alternates lists
 */
void alternates_init(void)
{
  if (AlternatesNotify)
    return;

  AlternatesNotify = notify_new();
  notify_set_parent(AlternatesNotify, SpaceMutt->notify);
}

/**
 * mutt_alternates_reset - Clear the recipient valid flag of all emails
 * @param mv Mailbox view
 */
void mutt_alternates_reset(struct MailboxView *mv)
{
  if (!mv || !mv->mailbox)
    return;

  struct Mailbox *m = mv->mailbox;

  for (int i = 0; i < m->msg_count; i++)
  {
    struct Email *e = m->emails[i];
    if (!e)
      break;
    e->recip_valid = false;
  }
}

/**
 * parse_alternates - Parse the 'alternates' command - Implements Command::parse() - @ingroup command_parse
 */
enum CommandResult parse_alternates(struct Buffer *buf, struct Buffer *s,
                                    intptr_t data, struct Buffer *err)
{
  GroupList *gl = NULL;

  do
  {
    parse_extract_token(buf, s, TOKEN_NO_FLAGS);

    if (parse_grouplist(&gl, buf, s, err) == -1)
    {
      mutt_grouplist_free(gl);
      return MUTT_CMD_ERROR;
    }

    mutt_regexlist_remove(&UnAlternates, buf->data);

    if ((mutt_regexlist_add(&Alternates, buf->data, REG_ICASE, err) != 0) ||
        (mutt_grouplist_add_regex(gl, buf->data, REG_ICASE, err) != 0))
    {
      mutt_grouplist_free(gl);
      return MUTT_CMD_ERROR;
    }
  } while (MoreArgs(s));

  mutt_grouplist_free(gl);

  log_notify("NT_ALTERN_ADD: %s", buf->data);
  notify_send(AlternatesNotify, NT_ALTERN, NT_ALTERN_ADD, NULL);

  return MUTT_CMD_SUCCESS;

}

/**
 * parse_unalternates - Parse the 'unalternates' command - Implements Command::parse() - @ingroup command_parse
 */
enum CommandResult parse_unalternates(struct Buffer *buf, struct Buffer *s,
                                      intptr_t data, struct Buffer *err)
{
  do
  {
    parse_extract_token(buf, s, TOKEN_NO_FLAGS);
    mutt_regexlist_remove(&Alternates, buf->data);

    if (!mutt_str_equal(buf->data, "*") &&
        (mutt_regexlist_add(&UnAlternates, buf->data, REG_ICASE, err) != 0))
    {
      return MUTT_CMD_ERROR;
    }

  } while (MoreArgs(s));

  log_notify("NT_ALTERN_DELETE: %s", buf->data);
  notify_send(AlternatesNotify, NT_ALTERN, NT_ALTERN_DELETE, NULL);

  return MUTT_CMD_SUCCESS;
}

/**
 * mutt_alternates_match - Compare an Address to the Un/Alternates lists
 * @param addr Address to check
 * @retval true Address matches
 */
bool mutt_alternates_match(const char *addr)
{
  if (!addr)
    return false;

  if (mutt_regexlist_match(Alternates, addr))
  {
    log_debug5("yes, %s matched by alternates", addr);
    if (mutt_regexlist_match(UnAlternates, addr))
      log_debug5("but, %s matched by unalternates", addr);
    else
      return true;
  }

  return false;
}
