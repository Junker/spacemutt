/**
 * @file
 * Manage regular expressions
 *
 * @authors
 * Copyright (C) 2017-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2020 Pietro Cerutti <gahr@gahr.ch>
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

#ifndef MUTT_MUTT_REGEX3_H
#define MUTT_MUTT_REGEX3_H

#include "config.h"
#include <regex.h>
#include <stdbool.h>
#include <stdint.h>
#include <glib.h>

struct Buffer;

/* This is a non-standard option supported by Solaris 2.5.x
 * which allows patterns of the form \<...\> */
#ifndef REG_WORDS
#define REG_WORDS 0
#endif

/**
 * REG_COMP - Compile a regular expression
 * @param preg   regex_t struct to fill
 * @param regex  Regular expression string
 * @param cflags Flags
 * @retval   0 Success
 * @retval num Failure, e.g. REG_BADPAT
 */
#define REG_COMP(preg, regex, cflags) regcomp(preg, regex, REG_WORDS | REG_EXTENDED | (cflags))

/**
 * mutt_regmatch_start - Return the start of a match
 * @param match Match
 * @retval num Start of the match
 */
static inline regoff_t mutt_regmatch_start(const regmatch_t *match)
{
  return match->rm_so;
}

/**
 * mutt_regmatch_end - Return the end of a match
 * @param match Match
 * @retval num End of the match
 */
static inline regoff_t mutt_regmatch_end(const regmatch_t *match)
{
  return match->rm_eo;
}

/**
 * mutt_regmatch_len - Return the length of a match
 * @param match Match
 * @retval num Length of the match
 */
static inline size_t mutt_regmatch_len(const regmatch_t *match)
{
  return match->rm_eo - match->rm_so;
}

/**
 * struct Regex - Cached regular expression
 */
struct Regex
{
  char *pattern;  ///< printable version
  regex_t *regex; ///< compiled expression
  bool pat_not;   ///< do not match
};
typedef GSList RegexList;

/**
 * struct Replace - List of regular expressions
 */
struct Replace
{
  struct Regex *regex;           ///< Regex containing a regular expression
  size_t nmatch;                 ///< Match the 'nth' occurrence (0 means the whole expression)
  char *templ;                   ///< Template to match
};
typedef GSList ReplaceList;

struct Regex *mutt_regex_compile(const char *str, uint16_t flags);
struct Regex *mutt_regex_new(const char *str, uint32_t flags, struct Buffer *err);
void          mutt_regex_free(struct Regex **ptr);

int               mutt_regexlist_add   (RegexList **rl, const char *str, uint16_t flags, struct Buffer *err);
void              mutt_regexlist_free_full (RegexList *rl);
bool              mutt_regexlist_match (RegexList *rl, const char *str);
int               mutt_regexlist_remove(RegexList **rl, const char *str);

int             mutt_replacelist_add   (ReplaceList **rl, const char *pat, const char *templ, struct Buffer *err);
char *          mutt_replacelist_apply (ReplaceList *rl, const char *str);
void            mutt_replacelist_free_full (ReplaceList *rl);
bool            mutt_replacelist_match (ReplaceList *rl, char *buf, size_t buflen, const char *str);
struct Replace *mutt_replacelist_new   (void);
int             mutt_replacelist_remove(ReplaceList **rl, const char *pat);

bool mutt_regex_match  (const struct Regex *regex, const char *str);
bool mutt_regex_capture(const struct Regex *regex, const char *str, size_t num, regmatch_t matches[]);

#endif /* MUTT_MUTT_REGEX3_H */
