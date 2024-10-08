/**
 * @file
 * Driver based email tags
 *
 * @authors
 * Copyright (C) 2017 Mehdi Abaakouk <sileht@sileht.net>
 * Copyright (C) 2017-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2021-2022 Pietro Cerutti <gahr@gahr.ch>
 * Copyright (C) 2024 Dennis Schön <mail@dennis-schoen.de>
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
 * @page email_tags Email tags
 *
 * Driver based email tags
 */

#include "config.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <glib.h>
#include "mutt/lib.h"
#include "config/lib.h"
#include "core/lib.h"
#include "tags.h"
#include "mutt/gslist.h"


struct HashTable *TagTransforms = NULL; ///< Hash Table: "inbox" -> "i" - Alternative tag names
struct HashTable *TagFormats = NULL; ///< Hash Table: "inbox" -> "GI" - Tag format strings

/**
 * tag_free - Free a Tag
 * @param ptr Tag to free
 */
void tag_free(struct Tag **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct Tag *tag = *ptr;
  FREE(&tag->name);
  FREE(&tag->transformed);

  FREE(ptr);
}

/**
 * tag_new - Create a new Tag
 * @retval ptr New Tag
 */
struct Tag *tag_new(void)
{
  return g_new0(struct Tag, 1);
}

/**
 * driver_tags_getter - Get transformed tags separated by space
 * @param[in]  tl               List of tags
 * @param[in]  show_hidden      Show hidden tags
 * @param[in]  show_transformed Show transformed tags
 * @param[in]  filter           Match tags to this string
 * @param[out] tags             String list of tags
 */
void driver_tags_getter(TagList *tl, bool show_hidden, bool show_transformed,
                        const char *filter, struct Buffer *tags)
{
  if (!tl)
    return;

  for (GSList *np = tl; np != NULL; np = np->next)
  {
    struct Tag *tag = np->data;
    if (filter && !mutt_str_equal(tag->name, filter))
      continue;
    if (show_hidden || !tag->hidden)
    {
      if (show_transformed && tag->transformed)
        buf_join_str(tags, tag->transformed, ' ');
      else
        buf_join_str(tags, tag->name, ' ');
    }
  }
}

/**
 * driver_tags_add - Add a tag to header
 * @param[in] tl      List of tags
 * @param[in] new_tag String representing the new tag
 *
 * Add a tag to the header tags
 *
 * @note The ownership of the string is passed to the TagList structure
 */
void driver_tags_add(TagList **tl, char *new_tag)
{
  char *new_tag_transformed = mutt_hash_find(TagTransforms, new_tag);

  struct Tag *tag = tag_new();
  tag->name = new_tag;
  tag->hidden = false;
  tag->transformed = mutt_str_dup(new_tag_transformed);

  /* filter out hidden tags */
  const struct StrList *c_hidden_tags = cs_subset_slist(SpaceMutt->sub, "hidden_tags");
  if (c_hidden_tags)
    if (g_slist_find_str(c_hidden_tags->head, new_tag, false))
      tag->hidden = true;

  *tl = g_slist_append(*tl, tag);
}

/**
 * driver_tags_free - Free tags from a header
 * @param[in] tl List of tags
 *
 * Free the whole tags structure
 */
void driver_tags_free(TagList *tl)
{
  if (!tl)
    return;

  for (GSList *np = tl; np != NULL; np = np->next)
  {
    struct Tag *tag = np->data;
    tag_free(&tag);
  }
  g_slist_free(tl);
}

/**
 * driver_tags_get_transformed - Get transformed tags separated by space
 * @param[in]  tl List of tags
 * @param[out] tags String list of tags
 */
void driver_tags_get_transformed(TagList *tl, struct Buffer *tags)
{
  driver_tags_getter(tl, false, true, NULL, tags);
}

/**
 * driver_tags_get - Get tags all tags separated by space
 * @param[in]  tl   List of tags
 * @param[out] tags String list of tags
 *
 * @note Hidden tags are not returned. Use driver_tags_get_with_hidden() for that.
 */
void driver_tags_get(TagList *tl, struct Buffer *tags)
{
  driver_tags_getter(tl, false, false, NULL, tags);
}

/**
 * driver_tags_get_with_hidden - Get all tags, also hidden ones, separated by space
 * @param[in]  tl List of tags
 * @param[out] tags String list of tags
 */
void driver_tags_get_with_hidden(TagList *tl, struct Buffer *tags)
{
  driver_tags_getter(tl, true, false, NULL, tags);
}

/**
 * driver_tags_get_transformed_for - Get transformed tags for a tag name separated by space
 * @param[in] tl   List of tags
 * @param[in] name Tag to transform
 * @param[out] tags String list of tags
 *
 * @note Will also return hidden tags.
 */
void driver_tags_get_transformed_for(TagList *tl, const char *name, struct Buffer *tags)
{
  driver_tags_getter(tl, true, true, name, tags);
}

/**
 * driver_tags_replace - Replace all tags
 * @param[in] tl    List of tags
 * @param[in] tags string of all tags separated by space
 * @retval false No changes are made
 * @retval true  Tags are updated
 *
 * Free current tags structures and replace it by new tags
 */
bool driver_tags_replace(TagList **tl, const char *tags)
{
  if (!tl || !*tl)
    return false;

  driver_tags_free(g_steal_pointer(tl));

  if (tags)
  {
    char **tagsarr = g_strsplit(tags, " ", -1);
    for (gchar **tag = tagsarr; *tag != NULL; tag++) {
      driver_tags_add(tl, *tag);
    }
    g_strfreev(tagsarr);
  }
  return true;
}

/**
 * tags_deleter - Free our hash table data - Implements ::hash_hdata_free_t - @ingroup hash_hdata_free_api
 */
static void tags_deleter(int type, void *obj, intptr_t data)
{
  FREE(&obj);
}

/**
 * driver_tags_init - Initialize structures used for tags
 */
void driver_tags_init(void)
{
  TagTransforms = mutt_hash_new(64, MUTT_HASH_STRCASECMP | MUTT_HASH_STRDUP_KEYS);
  mutt_hash_set_destructor(TagTransforms, tags_deleter, 0);

  TagFormats = mutt_hash_new(64, MUTT_HASH_STRDUP_KEYS);
  mutt_hash_set_destructor(TagFormats, tags_deleter, 0);
}

/**
 * driver_tags_cleanup - Deinitialize structures used for tags
 */
void driver_tags_cleanup(void)
{
  mutt_hash_free(&TagFormats);
  mutt_hash_free(&TagTransforms);
}
