/**
 * @file
 * Attachment Content-ID header functions
 *
 * @authors
 * Copyright (C) 2022 David Purton <dcpurton@marshwiggle.net>
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

/**
 * @page attach_cid Attachment Content-ID header functions
 *
 * Attachment Content-ID header functions
 */

#include "config.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "email/lib.h"
#include "core/lib.h"
#include "cid.h"
#include "attach.h"
#include "mailcap.h"
#include "mutt_attach.h"

/**
 * cid_map_free - Free a CidMap
 * @param[out] ptr CidMap to free
 */
void cid_map_free(struct CidMap **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct CidMap *cid_map = *ptr;

  FREE(&cid_map->cid);
  FREE(&cid_map->fname);

  FREE(ptr);
}

/**
 * cid_map_new - Initialise a new CidMap
 * @param  cid      Content-ID to replace including "cid:" prefix
 * @param  filename Path to file to replace Content-ID with
 * @retval ptr      Newly allocated CidMap
 */
struct CidMap *cid_map_new(const char *cid, const char *filename)
{
  if (!cid || !filename)
    return NULL;

  struct CidMap *cid_map = g_new0(struct CidMap, 1);

  cid_map->cid = mutt_str_dup(cid);
  cid_map->fname = mutt_str_dup(filename);

  return cid_map;
}

/**
 * cid_map_list_clear - Empty a CidMapList
 * @param cid_map_list List of Content-ID to filename mappings
 */
void cid_map_list_clear(CidMapList **cid_map_list)
{
  if (!*cid_map_list)
    return;

  for (GSList *np = *cid_map_list; np != NULL; np = np->next)
  {
    struct CidMap *cid_map = np->data;
    cid_map_free(&cid_map);
  }
  g_slist_free(g_steal_pointer(cid_map_list));
}

/**
 * cid_save_attachment - Save attachment if it has a Content-ID
 * @param[in]  b            Body to check and save
 * @param[out] cid_map_list List of Content-ID to filename mappings
 *
 * If body has a Content-ID, it is saved to disk and a new Content-ID to filename
 * mapping is added to cid_map_list.
 */
static void cid_save_attachment(struct Body *b, CidMapList **cid_map_list)
{
  if (!b || !cid_map_list)
    return;

  char *id = b->content_id;
  if (!id)
    return;

  struct Buffer *tmpfile = buf_pool_get();
  struct Buffer *cid = buf_pool_get();
  bool has_tempfile = false;
  FILE *fp = NULL;

  log_debug2("attachment found with \"Content-ID: %s\"", id);
  /* get filename */
  char *fname = mutt_str_dup(b->filename);
  if (b->aptr)
    fp = b->aptr->fp;
  mutt_file_sanitize_filename(fname, fp ? true : false);
  mailcap_expand_filename("%s", fname, tmpfile);
  FREE(&fname);

  /* save attachment */
  if (mutt_save_attachment(fp, b, buf_string(tmpfile), 0, NULL) == -1)
    goto bail;
  has_tempfile = true;
  log_debug2("attachment with \"Content-ID: %s\" saved to file \"%s\"",
             id, buf_string(tmpfile));

  /* add Content-ID to filename mapping to list */
  buf_printf(cid, "cid:%s", id);
  struct CidMap *cid_map = cid_map_new(buf_string(cid), buf_string(tmpfile));
  *cid_map_list = g_slist_append(*cid_map_list, cid_map);

bail:

  if ((fp && !buf_is_empty(tmpfile)) || has_tempfile)
    mutt_add_temp_attachment(buf_string(tmpfile));
  buf_pool_release(&tmpfile);
  buf_pool_release(&cid);
}

/**
 * cid_save_attachments - Save all attachments in a "multipart/related" group with a Content-ID
 * @param[in]  body         First body in "multipart/related" group
 * @param[out] cid_map_list List of Content-ID to filename mappings
 */
void cid_save_attachments(struct Body *body, CidMapList **cid_map_list)
{
  if (!body || !*cid_map_list)
    return;

  for (struct Body *b = body; b; b = b->next)
  {
    if (b->parts)
      cid_save_attachments(b->parts, cid_map_list);
    else
      cid_save_attachment(b, cid_map_list);
  }
}

/**
 * cid_to_filename - Replace Content-IDs with filenames
 * @param filename     Path to file to replace Content-IDs with filenames
 * @param cid_map_list List of Content-ID to filename mappings
 */
void cid_to_filename(struct Buffer *filename, const CidMapList *cid_map_list)
{
  if (!filename || !cid_map_list)
    return;

  FILE *fp_out = NULL;
  char *pbuf = NULL;
  char *searchbuf = NULL;
  char *buf = NULL;
  char *cid = NULL;
  size_t blen = 0;

  struct Buffer *tmpfile = buf_pool_get();
  struct Buffer *tmpbuf = buf_pool_get();

  FILE *fp_in = mutt_file_fopen(buf_string(filename), "r");
  if (!fp_in)
    goto bail;

  /* ensure tmpfile has the same file extension as filename otherwise an
   * HTML file may be opened as plain text by the viewer */
  const char *suffix = buf_rfind(filename, ".");
  if (suffix && *(suffix++))
    buf_mktemp_pfx_sfx(tmpfile, "neomutt", suffix);
  else
    buf_mktemp(tmpfile);
  fp_out = mutt_file_fopen(buf_string(tmpfile), "w+");
  if (!fp_out)
    goto bail;

  /* Read in lines from filename into buf */
  while ((buf = mutt_file_read_line(buf, &blen, fp_in, NULL, MUTT_RL_NO_FLAGS)) != NULL)
  {
    if (mutt_str_len(buf) == 0)
    {
      fputs(buf, fp_out);
      continue;
    }

    /* copy buf to searchbuf because we need to edit multiple times */
    searchbuf = mutt_str_dup(buf);
    buf_reset(tmpbuf);

    /* loop through Content-ID to filename mappings and do search and replace */
    for (GSList *np = cid_map_list; np != NULL; np = np->next)
    {
     struct CidMap *cid_map = np->data;
     pbuf = searchbuf;
     while ((cid = strstr(pbuf, cid_map->cid)) != NULL)
      {
        buf_addstr_n(tmpbuf, pbuf, cid - pbuf);
        buf_addstr(tmpbuf, cid_map->fname);
        pbuf = cid + mutt_str_len(cid_map->cid);
        log_debug2("replaced \"%s\" with \"%s\" in file \"%s\"",
                   cid_map->cid, cid_map->fname, buf_string(filename));
      }
      buf_addstr(tmpbuf, pbuf);
      FREE(&searchbuf);
      searchbuf = buf_strdup(tmpbuf);
      buf_reset(tmpbuf);
    }

    /* write edited line to output file */
    fputs(searchbuf, fp_out);
    fputs("\n", fp_out);
    FREE(&searchbuf);
  }

  mutt_file_set_mtime(buf_string(filename), buf_string(tmpfile));

  /* add filename to TempAtachmentsList so it doesn't get left lying around */
  mutt_add_temp_attachment(buf_string(filename));
  /* update filename to point to new file */
  buf_copy(filename, tmpfile);

bail:
  FREE(&buf);
  mutt_file_fclose(&fp_in);
  mutt_file_fclose(&fp_out);
  buf_pool_release(&tmpfile);
  buf_pool_release(&tmpbuf);
}
