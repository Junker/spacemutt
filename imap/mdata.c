/**
 * @file
 * Imap-specific Mailbox data
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
 * @page imap_mdata Imap-specific Mailbox data
 *
 * Imap-specific Mailbox data
 */

#include "config.h"
#include <stdbool.h>
#include <glib.h>
#include "private.h"
#include "core/lib.h"
#include "mdata.h"
#include "hcache/lib.h"
#include "adata.h"

/**
 * imap_mdata_free - Free the private Mailbox data - Implements Mailbox::mdata_free() - @ingroup mailbox_mdata_free
 */
void imap_mdata_free(void **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct ImapMboxData *mdata = *ptr;

  imap_mdata_cache_reset(mdata);
  g_slist_free_full(g_steal_pointer(&mdata->flags), g_free);
  FREE(&mdata->name);
  FREE(&mdata->real_name);
  FREE(&mdata->munge_name);

  FREE(ptr);
}

/**
 * imap_mdata_get - Get the Mailbox data for this mailbox
 * @param m Mailbox
 * @retval ptr ImapMboxData
 */
struct ImapMboxData *imap_mdata_get(struct Mailbox *m)
{
  if (!m || (m->type != MUTT_IMAP) || !m->mdata)
    return NULL;
  return m->mdata;
}

/**
 * imap_mdata_new - Allocate and initialise a new ImapMboxData structure
 * @param adata Imap Account data
 * @param name  Name for Mailbox
 * @retval ptr New ImapMboxData
 */
struct ImapMboxData *imap_mdata_new(struct ImapAccountData *adata, const char *name)
{
  char buf[1024] = { 0 };
  struct ImapMboxData *mdata = g_new0(struct ImapMboxData, 1);

  mdata->real_name = mutt_str_dup(name);

  imap_fix_path(adata->delim, name, buf, sizeof(buf));
  if (buf[0] == '\0')
    mutt_str_copy(buf, "INBOX", sizeof(buf));
  mdata->name = mutt_str_dup(buf);

  imap_munge_mbox_name(adata->unicode, buf, sizeof(buf), mdata->name);
  mdata->munge_name = mutt_str_dup(buf);

  mdata->reopen &= IMAP_REOPEN_ALLOW;

#ifdef USE_HCACHE
  imap_hcache_open(adata, mdata, false);
  if (mdata->hcache)
  {
    if (hcache_fetch_raw_obj(mdata->hcache, "UIDVALIDITY", 11, &mdata->uidvalidity))
    {
      hcache_fetch_raw_obj(mdata->hcache, "UIDNEXT", 7, &mdata->uid_next);
      hcache_fetch_raw_obj(mdata->hcache, "MODSEQ", 6, &mdata->modseq);
      log_debug3("hcache uidvalidity %u, uidnext %u, modseq %llu",
                 mdata->uidvalidity, mdata->uid_next, mdata->modseq);
    }
    imap_hcache_close(mdata);
  }
#endif

  return mdata;
}
