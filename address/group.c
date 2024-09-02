/**
 * @file
 * Handling for email address groups
 *
 * @authors
 * Copyright (C) 2017-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2018 Bo Yu <tsu.yubo@gmail.com>
 * Copyright (C) 2018-2019 Pietro Cerutti <gahr@gahr.ch>
 * Copyright (C) 2019 Federico Kircheis <federico.kircheis@gmail.com>
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
 * @page addr_group Address groups
 *
 * Handling for email address groups
 */

#include "config.h"
#include <stdbool.h>
#include <stdlib.h>
#include "group.h"
#include "address.h"
#include "mutt/gqueue.h"


/**
 * Groups - Hash Table: "group-name" -> Group
 *
 * A set of all the Address Groups.
 */
static struct HashTable *Groups = NULL;

/**
 * group_free - Free an Address Group
 * @param ptr Group to free
 */
static void group_free(struct Group **ptr)
{
  if (!ptr || !*ptr)
    return;

  struct Group *g = *ptr;

  mutt_addrlist_free_full(g->al);
  mutt_regexlist_free(g->rs);
  FREE(&g->name);

  FREE(ptr);
}

/**
 * group_new - Create a new Address Group
 * @param  pat Pattern
 * @retval ptr New Address Group
 *
 * @note The pattern will be copied
 */
static struct Group *group_new(const char *pat)
{
  struct Group *g = mutt_mem_calloc(1, sizeof(struct Group));

  g->name = mutt_str_dup(pat);
  g->rs = NULL;
  g->al = mutt_addrlist_new();

  return g;
}

/**
 * group_hash_free - Free our hash table data - Implements ::hash_hdata_free_t - @ingroup hash_hdata_free_api
 */
static void group_hash_free(int type, void *obj, intptr_t data)
{
  struct Group *g = obj;
  group_free(&g);
}

/**
 * mutt_grouplist_init - Initialize the GroupList singleton
 *
 * This is called once from init.c when initializing the global structures.
 */
void mutt_grouplist_init(void)
{
  Groups = mutt_hash_new(1031, MUTT_HASH_NO_FLAGS);

  mutt_hash_set_destructor(Groups, group_hash_free, 0);
}

/**
 * mutt_grouplist_cleanup - Free GroupList singleton resource
 *
 * This is called once from init.c when deinitializing the global resources.
 */
void mutt_grouplist_cleanup(void)
{
  mutt_hash_free(&Groups);
}

/**
 * mutt_pattern_group - Match a pattern to a Group
 * @param pat Pattern to match
 * @retval ptr Matching Group, or new Group (if no match)
 */
struct Group *mutt_pattern_group(const char *pat)
{
  if (!pat)
    return NULL;

  struct Group *g = mutt_hash_find(Groups, pat);
  if (!g)
  {
    log_debug2("Creating group %s", pat);
    g = group_new(pat);
    mutt_hash_insert(Groups, g->name, g);
  }

  return g;
}

/**
 * group_remove - Remove a Group from the Hash Table
 * @param g Group to remove
 */
static void group_remove(struct Group *g)
{
  if (!g)
    return;
  mutt_hash_delete(Groups, g->name, g);
}

/**
 * mutt_grouplist_clear - Clear GroupList
 * @param gl GroupList to free
 */
void mutt_grouplist_clear(GroupList *gl)
{
  if (!gl)
    return;

  g_slist_free_full(gl, (GDestroyNotify)group_remove);
}

/**
 * empty_group - Is a Group empty?
 * @param g Group to test
 * @retval true The Group is empty
 */
static bool empty_group(struct Group *g)
{
  if (!g)
    return true;
  return g_queue_is_empty(g->al) && !g->rs;
}

/**
 * mutt_grouplist_add - Add a Group to a GroupList
 * @param gl    GroupList to add to
 * @param group Group to add
 */
void mutt_grouplist_add(GroupList **gl, struct Group *group)
{
  if (!gl || !group)
    return;

  if (g_slist_find(*gl, group))
    return;

  *gl = g_slist_append(*gl, group);
}

/**
 * mutt_grouplist_free - Free a GroupList
 * @param gl GroupList to free
 */
void mutt_grouplist_free(GroupList *gl)
{
  if (!gl)
    return;

  g_slist_free(gl);
}

/**
 * group_add_addrlist - Add an Address List to a Group
 * @param g  Group to add to
 * @param al Address List
 */
static void group_add_addrlist(struct Group *g, const AddressList *al)
{
  if (!g || !al)
    return;

  AddressList *al_new = mutt_addrlist_new();
  mutt_addrlist_copy(al_new, al, false);
  mutt_addrlist_remove_xrefs(g->al, al_new);
  g_queue_copy_tail(g->al, al_new);
  g_queue_free(al_new);
}

/**
 * group_add_regex - Add a Regex to a Group
 * @param g     Group to add to
 * @param s     Regex string to add
 * @param flags Flags, e.g. REG_ICASE
 * @param err   Buffer for error message
 * @retval  0 Success
 * @retval -1 Error
 */
static int group_add_regex(struct Group *g, const char *s, uint16_t flags, struct Buffer *err)
{
  return mutt_regexlist_add(&g->rs, s, flags, err);
}

/**
 * group_remove_regex - Remove a Regex from a Group
 * @param g Group to modify
 * @param s Regex string to match
 * @retval  0 Success
 * @retval -1 Error
 */
static int group_remove_regex(struct Group *g, const char *s)
{
  return mutt_regexlist_remove(&g->rs, s);
}

/**
 * mutt_grouplist_add_addrlist - Add Address list to a GroupList
 * @param gl GroupList to add to
 * @param al Address list to add
 */
void mutt_grouplist_add_addrlist(GroupList *gl, AddressList *al)
{
  if (!gl || !al)
    return;

  for (GSList *np = gl; np != NULL; np = np->next)
  {
    struct Group *g = np->data;
    group_add_addrlist(g, al);
  }
}

/**
 * mutt_grouplist_remove_addrlist - Remove an AddressList from a GroupList
 * @param gl GroupList to remove from
 * @param al AddressList to remove
 * @retval  0 Success
 * @retval -1 Error
 */
int mutt_grouplist_remove_addrlist(GroupList *gl, AddressList *al)
{
  if (!al)
    return -1;

  for (GSList *gnp = gl; gnp != NULL; gnp = gnp->next)
  {
    struct Group *grp = gnp->data;

    for (GList *np = al->head; np != NULL; np = np->next)
    {
      struct Address *a = np->data;
      mutt_addrlist_remove(grp->al, buf_string(a->mailbox));
    }
    if (empty_group(grp))
    {
      group_remove(grp);
    }
  }

  return 0;
}

/**
 * mutt_grouplist_add_regex - Add matching Addresses to a GroupList
 * @param gl    GroupList to add to
 * @param s     Address to match
 * @param flags Flags, e.g. REG_ICASE
 * @param err   Buffer for error message
 * @retval  0 Success
 * @retval -1 Error
 */
int mutt_grouplist_add_regex(GroupList *gl, const char *s,
                             uint16_t flags, struct Buffer *err)
{
  if (!s)
    return -1;

  int rc = 0;

  for (GSList *np = gl; np != NULL; np = np->next)
  {
    struct Group *grp = np->data;
    rc = group_add_regex(grp, s, flags, err);
    if (rc)
      return rc;
  }
  return rc;
}

/**
 * mutt_grouplist_remove_regex - Remove matching addresses from a GroupList
 * @param gl GroupList to remove from
 * @param s  Address to match
 * @retval  0 Success
 * @retval -1 Error
 */
int mutt_grouplist_remove_regex(GroupList *gl, const char *s)
{
  if (!s)
    return -1;

  int rc = 0;
  for (GSList *np = gl; np != NULL; np = np->next)
  {
    struct Group *grp = np->data;
    rc = group_remove_regex(grp, s);
    if (empty_group(grp))
      group_remove(grp);
    if (rc)
      return rc;
  }
  return rc;
}

/**
 * mutt_group_match - Does a string match an entry in a Group?
 * @param g Group to match against
 * @param s String to match
 * @retval true There's a match
 */
bool mutt_group_match(struct Group *g, const char *s)
{
  if (!g || !s)
    return false;

  if (mutt_regexlist_match(g->rs, s))
    return true;
  for (GList *np = g->al->head; np != NULL; np = np->next)
  {
    struct Address *a = np->data;
    if (a->mailbox && mutt_istr_equal(s, buf_string(a->mailbox)))
      return true;
  }

  return false;
}
