/**
 * @file
 * Test code hub
 *
 * @authors
 * Copyright (C) 2018-2022 Pietro Cerutti <gahr@gahr.ch>
 * Copyright (C) 2019 Simon Symeonidis <lethaljellybean@gmail.com>
 * Copyright (C) 2019-2023 Richard Russon <rich@flatcap.org>
 * Copyright (C) 2021 Austin Ray <austin@austinray.io>
 * Copyright (C) 2021 Eric Blake <eblake@redhat.com>
 * Copyright (C) 2022 Michal Siedlaczek <michal@siedlaczek.me>
 * Copyright (C) 2023 Anna Figueiredo Gomes <navi@vlhl.dev>
 * Copyright (C) 2023 Rayford Shireman
 * Copyright (C) 2023-2024 Dennis Schön <mail@dennis-schoen.de>
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

void test_init(void);
void test_fini(void);

#include "config.h"
#define TEST_INIT test_init()
#define TEST_FINI test_fini()
#include "acutest.h"

/******************************************************************************
 * Add your test cases to this list.
 *****************************************************************************/
#define SPACEMUTT_TEST_LIST                                                      \
  /* account */                                                                  \
  SPACEMUTT_TEST_ITEM(test_account_free)                                         \
  SPACEMUTT_TEST_ITEM(test_account_mailbox_add)                                  \
  SPACEMUTT_TEST_ITEM(test_account_mailbox_remove)                               \
  SPACEMUTT_TEST_ITEM(test_account_new)                                          \
                                                                                 \
  /* address */                                                                  \
  SPACEMUTT_TEST_ITEM(test_address_config)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_addr_cat)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_addr_cmp)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_addr_copy)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_addr_create)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_addr_for_display)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_addr_free)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_addr_new)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_addr_to_intl)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_addr_to_local)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_addr_valid_msgid)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_addr_write)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_append)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_clear)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_copy)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_count_recips)                           \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_dedupe)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_equal)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_parse)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_parse2)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_prepend)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_qualify)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_remove)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_remove_xrefs)                           \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_search)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_to_intl)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_to_local)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_write)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_write_list)                             \
  SPACEMUTT_TEST_ITEM(test_mutt_addrlist_write_wrap)                             \
                                                                                 \
  /* array */                                                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_array_api)                                       \
                                                                                 \
  /* attach */                                                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_actx_add_attach)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_actx_add_body)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_actx_add_fp)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_actx_entries_free)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_actx_free)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_actx_new)                                        \
                                                                                 \
  /* base64 */                                                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_b64_buffer_decode)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_b64_buffer_encode)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_b64_decode)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_b64_encode)                                      \
                                                                                 \
  /* body */                                                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_body_cmp_strict)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_body_free)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_body_new)                                        \
                                                                                 \
  /* buffer */                                                                   \
  SPACEMUTT_TEST_ITEM(test_buf_addch)                                            \
  SPACEMUTT_TEST_ITEM(test_buf_addstr)                                           \
  SPACEMUTT_TEST_ITEM(test_buf_addstr_n)                                         \
  SPACEMUTT_TEST_ITEM(test_buf_add_printf)                                       \
  SPACEMUTT_TEST_ITEM(test_buf_alloc)                                            \
  SPACEMUTT_TEST_ITEM(test_buf_at)                                               \
  SPACEMUTT_TEST_ITEM(test_buf_coll)                                             \
  SPACEMUTT_TEST_ITEM(test_buf_concatn_path)                                     \
  SPACEMUTT_TEST_ITEM(test_buf_concat_path)                                      \
  SPACEMUTT_TEST_ITEM(test_buf_copy)                                             \
  SPACEMUTT_TEST_ITEM(test_buf_dealloc)                                          \
  SPACEMUTT_TEST_ITEM(test_buf_dequote_comment)                                  \
  SPACEMUTT_TEST_ITEM(test_buf_find_char)                                        \
  SPACEMUTT_TEST_ITEM(test_buf_find_string)                                      \
  SPACEMUTT_TEST_ITEM(test_buf_fix_dptr)                                         \
  SPACEMUTT_TEST_ITEM(test_buf_free)                                             \
  SPACEMUTT_TEST_ITEM(test_buf_init)                                             \
  SPACEMUTT_TEST_ITEM(test_buf_inline_replace)                                   \
  SPACEMUTT_TEST_ITEM(test_buf_insert)                                           \
  SPACEMUTT_TEST_ITEM(test_buf_istr_equal)                                       \
  SPACEMUTT_TEST_ITEM(test_buf_is_empty)                                         \
  SPACEMUTT_TEST_ITEM(test_buf_join_str)                                         \
  SPACEMUTT_TEST_ITEM(test_buf_len)                                              \
  SPACEMUTT_TEST_ITEM(test_buf_lower)                                            \
  SPACEMUTT_TEST_ITEM(test_buf_new)                                              \
  SPACEMUTT_TEST_ITEM(test_buf_pool_cleanup)                                     \
  SPACEMUTT_TEST_ITEM(test_buf_pool_get)                                         \
  SPACEMUTT_TEST_ITEM(test_buf_pool_release)                                     \
  SPACEMUTT_TEST_ITEM(test_buf_printf)                                           \
  SPACEMUTT_TEST_ITEM(test_buf_reset)                                            \
  SPACEMUTT_TEST_ITEM(test_buf_rfind)                                            \
  SPACEMUTT_TEST_ITEM(test_buf_seek)                                             \
  SPACEMUTT_TEST_ITEM(test_buf_startswith)                                       \
  SPACEMUTT_TEST_ITEM(test_buf_strcpy)                                           \
  SPACEMUTT_TEST_ITEM(test_buf_strcpy_n)                                         \
  SPACEMUTT_TEST_ITEM(test_buf_strdup)                                           \
  SPACEMUTT_TEST_ITEM(test_buf_str_equal)                                        \
  SPACEMUTT_TEST_ITEM(test_buf_substrcpy)                                        \
                                                                                 \
  /* charset */                                                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_canonical_charset)                            \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_charset_lookup)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_check)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_check_charset)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_choose)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_chscmp)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_convert_nonmime_string)                       \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_convert_string)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_fgetconv)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_fgetconv_close)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_fgetconv_open)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_fgetconvs)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_get_default_charset)                          \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_get_langinfo_charset)                         \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_iconv)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_iconv_lookup)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_iconv_open)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_lookup_add)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_lookup_remove)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_ch_set_charset)                                  \
                                                                                 \
  /* color */                                                                    \
  SPACEMUTT_TEST_ITEM(test_ansi_color)                                           \
  SPACEMUTT_TEST_ITEM(test_ansi_color_parse_single)                              \
  SPACEMUTT_TEST_ITEM(test_attr_colors)                                          \
  SPACEMUTT_TEST_ITEM(test_color_dump)                                           \
  SPACEMUTT_TEST_ITEM(test_color_notify)                                         \
  SPACEMUTT_TEST_ITEM(test_curses_colors)                                        \
  SPACEMUTT_TEST_ITEM(test_merged_colors)                                        \
  SPACEMUTT_TEST_ITEM(test_parse_attr_spec)                                      \
  SPACEMUTT_TEST_ITEM(test_parse_color_colornnn)                                 \
  SPACEMUTT_TEST_ITEM(test_parse_color_name)                                     \
  SPACEMUTT_TEST_ITEM(test_parse_color_namedcolor)                               \
  SPACEMUTT_TEST_ITEM(test_parse_color_pair)                                     \
  SPACEMUTT_TEST_ITEM(test_parse_color_prefix)                                   \
  SPACEMUTT_TEST_ITEM(test_parse_color_rrggbb)                                   \
  SPACEMUTT_TEST_ITEM(test_quoted_colors)                                        \
  SPACEMUTT_TEST_ITEM(test_simple_colors)                                        \
                                                                                 \
  /* config */                                                                   \
  SPACEMUTT_TEST_ITEM(test_config_account)                                       \
  SPACEMUTT_TEST_ITEM(test_config_bool)                                          \
  SPACEMUTT_TEST_ITEM(test_config_dump)                                          \
  SPACEMUTT_TEST_ITEM(test_config_enum)                                          \
  SPACEMUTT_TEST_ITEM(test_config_helpers)                                       \
  SPACEMUTT_TEST_ITEM(test_config_initial)                                       \
  SPACEMUTT_TEST_ITEM(test_config_long)                                          \
  SPACEMUTT_TEST_ITEM(test_config_mbtable)                                       \
  SPACEMUTT_TEST_ITEM(test_config_myvar)                                         \
  SPACEMUTT_TEST_ITEM(test_config_number)                                        \
  SPACEMUTT_TEST_ITEM(test_config_path)                                          \
  SPACEMUTT_TEST_ITEM(test_config_quad)                                          \
  SPACEMUTT_TEST_ITEM(test_config_regex)                                         \
  SPACEMUTT_TEST_ITEM(test_config_set)                                           \
  SPACEMUTT_TEST_ITEM(test_config_slist)                                         \
  SPACEMUTT_TEST_ITEM(test_config_sort)                                          \
  SPACEMUTT_TEST_ITEM(test_config_string)                                        \
  SPACEMUTT_TEST_ITEM(test_config_subset)                                        \
  SPACEMUTT_TEST_ITEM(test_config_synonym)                                       \
  SPACEMUTT_TEST_ITEM(test_config_variable)                                      \
                                                                                 \
  /* convert */                                                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_convert_file_to)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_convert_file_from_to)                            \
  SPACEMUTT_TEST_ITEM(test_mutt_update_content_info)                             \
  SPACEMUTT_TEST_ITEM(test_mutt_get_content_info)                                \
                                                                                 \
  /* core */                                                                     \
  SPACEMUTT_TEST_ITEM(test_buf_mktemp_full)                                      \
  SPACEMUTT_TEST_ITEM(test_config_cache)                                         \
  SPACEMUTT_TEST_ITEM(test_dispatcher_get_retval_name)                           \
  SPACEMUTT_TEST_ITEM(test_mutt_file_mkstemp_full)                               \
                                                                                 \
  /* date */                                                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_date_add_timeout)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_date_check_month)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_date_now)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_date_now_ms)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_date_gmtime)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_date_local_tz)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_date_localtime)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_date_localtime_format)                           \
  SPACEMUTT_TEST_ITEM(test_mutt_date_localtime_format_locale)                    \
  SPACEMUTT_TEST_ITEM(test_mutt_date_make_date)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_date_make_imap)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_date_make_time)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_date_make_tls)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_date_normalize_time)                             \
  SPACEMUTT_TEST_ITEM(test_mutt_date_parse_date)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_date_parse_imap)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_date_sleep_ms)                                   \
                                                                                 \
  /* editor */                                                                   \
  SPACEMUTT_TEST_ITEM(test_editor_backspace)                                     \
  SPACEMUTT_TEST_ITEM(test_editor_backward_char)                                 \
  SPACEMUTT_TEST_ITEM(test_editor_backward_word)                                 \
  SPACEMUTT_TEST_ITEM(test_editor_bol)                                           \
  SPACEMUTT_TEST_ITEM(test_editor_buffer_is_empty)                               \
  SPACEMUTT_TEST_ITEM(test_editor_case_word)                                     \
  SPACEMUTT_TEST_ITEM(test_editor_delete_char)                                   \
  SPACEMUTT_TEST_ITEM(test_editor_eol)                                           \
  SPACEMUTT_TEST_ITEM(test_editor_forward_char)                                  \
  SPACEMUTT_TEST_ITEM(test_editor_forward_word)                                  \
  SPACEMUTT_TEST_ITEM(test_editor_kill_eol)                                      \
  SPACEMUTT_TEST_ITEM(test_editor_kill_eow)                                      \
  SPACEMUTT_TEST_ITEM(test_editor_kill_line)                                     \
  SPACEMUTT_TEST_ITEM(test_editor_kill_whole_line)                               \
  SPACEMUTT_TEST_ITEM(test_editor_kill_word)                                     \
  SPACEMUTT_TEST_ITEM(test_editor_state)                                         \
  SPACEMUTT_TEST_ITEM(test_editor_transpose_chars)                               \
                                                                                 \
  /* email */                                                                    \
  SPACEMUTT_TEST_ITEM(test_email_cmp_strict)                                     \
  SPACEMUTT_TEST_ITEM(test_email_free)                                           \
  SPACEMUTT_TEST_ITEM(test_email_new)                                            \
  SPACEMUTT_TEST_ITEM(test_email_size)                                           \
  SPACEMUTT_TEST_ITEM(test_mutt_autocrypthdr_free)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_autocrypthdr_new)                                \
  SPACEMUTT_TEST_ITEM(test_email_header_find)                                    \
  SPACEMUTT_TEST_ITEM(test_email_header_add)                                     \
  SPACEMUTT_TEST_ITEM(test_email_header_update)                                  \
  SPACEMUTT_TEST_ITEM(test_email_header_set)                                     \
  SPACEMUTT_TEST_ITEM(test_email_header_free)                                    \
                                                                                 \
  /* envelope */                                                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_env_cmp_strict)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_env_free)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_env_merge)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_env_new)                                         \
  SPACEMUTT_TEST_ITEM(test_mutt_env_to_intl)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_env_to_local)                                    \
                                                                                 \
  /* envlist */                                                                  \
  SPACEMUTT_TEST_ITEM(test_envlist_free)                                         \
  SPACEMUTT_TEST_ITEM(test_envlist_init)                                         \
  SPACEMUTT_TEST_ITEM(test_envlist_set)                                          \
  SPACEMUTT_TEST_ITEM(test_envlist_unset)                                        \
                                                                                 \
  /* eqi */                                                                      \
  SPACEMUTT_TEST_ITEM(test_eqi)                                                  \
                                                                                 \
  /* expando */                                                                  \
  SPACEMUTT_TEST_ITEM(test_expando_colors_render)                                \
  SPACEMUTT_TEST_ITEM(test_expando_complex_if_else)                              \
  SPACEMUTT_TEST_ITEM(test_expando_conditional_date)                             \
  SPACEMUTT_TEST_ITEM(test_expando_conditional_date_render)                      \
  SPACEMUTT_TEST_ITEM(test_expando_conditional_date_render2)                     \
  SPACEMUTT_TEST_ITEM(test_expando_config)                                       \
  SPACEMUTT_TEST_ITEM(test_expando_date_render)                                  \
  SPACEMUTT_TEST_ITEM(test_expando_emoji_text)                                   \
  SPACEMUTT_TEST_ITEM(test_expando_empty)                                        \
  SPACEMUTT_TEST_ITEM(test_expando_empty_if_else)                                \
  SPACEMUTT_TEST_ITEM(test_expando_empty_if_else_render)                         \
  SPACEMUTT_TEST_ITEM(test_expando_expando)                                      \
  SPACEMUTT_TEST_ITEM(test_expando_filter)                                       \
  SPACEMUTT_TEST_ITEM(test_expando_formatted_expando)                            \
  SPACEMUTT_TEST_ITEM(test_expando_helpers)                                      \
  SPACEMUTT_TEST_ITEM(test_expando_if_else_false_render)                         \
  SPACEMUTT_TEST_ITEM(test_expando_if_else_true_render)                          \
  SPACEMUTT_TEST_ITEM(test_expando_justify)                                      \
  SPACEMUTT_TEST_ITEM(test_expando_nested_if_else)                               \
  SPACEMUTT_TEST_ITEM(test_expando_nested_if_else_render)                        \
  SPACEMUTT_TEST_ITEM(test_expando_new_if_else)                                  \
  SPACEMUTT_TEST_ITEM(test_expando_node)                                         \
  SPACEMUTT_TEST_ITEM(test_expando_node_condbool)                                \
  SPACEMUTT_TEST_ITEM(test_expando_node_conddate)                                \
  SPACEMUTT_TEST_ITEM(test_expando_node_container)                               \
  SPACEMUTT_TEST_ITEM(test_expando_node_expando)                                 \
  SPACEMUTT_TEST_ITEM(test_expando_node_expando_format)                          \
  SPACEMUTT_TEST_ITEM(test_expando_node_padding)                                 \
  SPACEMUTT_TEST_ITEM(test_expando_old_if_else)                                  \
  SPACEMUTT_TEST_ITEM(test_expando_padding)                                      \
  SPACEMUTT_TEST_ITEM(test_expando_padding_render)                               \
  SPACEMUTT_TEST_ITEM(test_expando_parser)                                       \
  SPACEMUTT_TEST_ITEM(test_expando_percent_sign_text)                            \
  SPACEMUTT_TEST_ITEM(test_expando_simple_expando)                               \
  SPACEMUTT_TEST_ITEM(test_expando_simple_expando_render)                        \
  SPACEMUTT_TEST_ITEM(test_expando_simple_text)                                  \
  SPACEMUTT_TEST_ITEM(test_expando_two_char_expando)                             \
  SPACEMUTT_TEST_ITEM(test_expando_two_char_expando_render)                      \
  SPACEMUTT_TEST_ITEM(test_expando_unicode_padding)                              \
  SPACEMUTT_TEST_ITEM(test_expando_validation)                                   \
                                                                                 \
  /* file */                                                                     \
  SPACEMUTT_TEST_ITEM(test_buf_file_expand_fmt_quote)                            \
  SPACEMUTT_TEST_ITEM(test_buf_quote_filename)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_file_check_empty)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_file_chmod_add)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_file_chmod_add_stat)                             \
  SPACEMUTT_TEST_ITEM(test_mutt_file_chmod_rm_stat)                              \
  SPACEMUTT_TEST_ITEM(test_mutt_file_copy_bytes)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_file_copy_stream)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_file_decrease_mtime)                             \
  SPACEMUTT_TEST_ITEM(test_mutt_file_expand_fmt)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_file_fclose)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_file_fopen)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_file_fsync_close)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_file_get_size)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_file_get_stat_timespec)                          \
  SPACEMUTT_TEST_ITEM(test_mutt_file_iter_line)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_file_lock)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_file_map_lines)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_file_mkdir)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_file_open)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_file_read_keyword)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_file_read_line)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_file_rename)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_file_resolve_symlink)                            \
  SPACEMUTT_TEST_ITEM(test_mutt_file_rmtree)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_file_safe_rename)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_file_sanitize_filename)                          \
  SPACEMUTT_TEST_ITEM(test_mutt_file_sanitize_regex)                             \
  SPACEMUTT_TEST_ITEM(test_mutt_file_set_mtime)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_file_stat_compare)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_file_stat_timespec_compare)                      \
  SPACEMUTT_TEST_ITEM(test_mutt_file_symlink)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_file_timespec_compare)                           \
  SPACEMUTT_TEST_ITEM(test_mutt_file_touch_atime)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_file_unlink)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_file_unlink_empty)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_file_unlock)                                     \
                                                                                 \
  /* filter */                                                                   \
  SPACEMUTT_TEST_ITEM(test_filter_create)                                        \
  SPACEMUTT_TEST_ITEM(test_filter_create_fd)                                     \
  SPACEMUTT_TEST_ITEM(test_filter_wait)                                          \
                                                                                 \
  /* from */                                                                     \
  SPACEMUTT_TEST_ITEM(test_is_from)                                              \
                                                                                 \
  /* group */                                                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_group_match)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_grouplist_add)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_grouplist_add_addrlist)                          \
  SPACEMUTT_TEST_ITEM(test_mutt_grouplist_add_regex)                             \
  SPACEMUTT_TEST_ITEM(test_mutt_grouplist_cleanup)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_grouplist_clear)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_grouplist_free)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_grouplist_init)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_grouplist_remove_addrlist)                       \
  SPACEMUTT_TEST_ITEM(test_mutt_grouplist_remove_regex)                          \
  SPACEMUTT_TEST_ITEM(test_mutt_pattern_group)                                   \
                                                                                 \
  /* gui */                                                                      \
  SPACEMUTT_TEST_ITEM(test_window_reflow)                                        \
  SPACEMUTT_TEST_ITEM(test_window_visible)                                       \
                                                                                 \
  /* hash */                                                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_delete)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_find)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_find_bucket)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_find_elem)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_free)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_insert)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_int_delete)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_int_find)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_int_insert)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_int_new)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_new)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_set_destructor)                             \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_typed_insert)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_hash_walk)                                       \
                                                                                 \
  /* history */                                                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_hist_add)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_hist_at_scratch)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_hist_cleanup)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_hist_init)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_hist_next)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_hist_prev)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_hist_read_file)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_hist_reset_state)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_hist_save_scratch)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_hist_search)                                     \
                                                                                 \
  /* idna */                                                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_idna_intl_to_local)                              \
  SPACEMUTT_TEST_ITEM(test_mutt_idna_local_to_intl)                              \
  SPACEMUTT_TEST_ITEM(test_mutt_idna_print_version)                              \
  SPACEMUTT_TEST_ITEM(test_mutt_idna_to_ascii_lz)                                \
                                                                                 \
  /* imap */                                                                     \
  SPACEMUTT_TEST_ITEM(test_imap_msg_set)                                         \
                                                                                 \
  /* logging */                                                                  \
  SPACEMUTT_TEST_ITEM(test_log_disp_file)                                        \
  SPACEMUTT_TEST_ITEM(test_log_disp_queue)                                       \
  SPACEMUTT_TEST_ITEM(test_log_disp_terminal)                                    \
  SPACEMUTT_TEST_ITEM(test_log_file_close)                                       \
  SPACEMUTT_TEST_ITEM(test_log_file_open)                                        \
  SPACEMUTT_TEST_ITEM(test_log_file_running)                                     \
  SPACEMUTT_TEST_ITEM(test_log_file_set_filename)                                \
  SPACEMUTT_TEST_ITEM(test_log_file_set_level)                                   \
  SPACEMUTT_TEST_ITEM(test_log_file_set_version)                                 \
  SPACEMUTT_TEST_ITEM(test_log_queue_add)                                        \
  SPACEMUTT_TEST_ITEM(test_log_queue_empty)                                      \
  SPACEMUTT_TEST_ITEM(test_log_queue_flush)                                      \
  SPACEMUTT_TEST_ITEM(test_log_queue_save)                                       \
  SPACEMUTT_TEST_ITEM(test_log_queue_set_max_size)                               \
                                                                                 \
  /* mailbox */                                                                  \
  SPACEMUTT_TEST_ITEM(test_mailbox_changed)                                      \
  SPACEMUTT_TEST_ITEM(test_mailbox_find)                                         \
  SPACEMUTT_TEST_ITEM(test_mailbox_find_name)                                    \
  SPACEMUTT_TEST_ITEM(test_mailbox_free)                                         \
  SPACEMUTT_TEST_ITEM(test_mailbox_new)                                          \
  SPACEMUTT_TEST_ITEM(test_mailbox_set_subset)                                   \
  SPACEMUTT_TEST_ITEM(test_mailbox_size_add)                                     \
  SPACEMUTT_TEST_ITEM(test_mailbox_size_sub)                                     \
  SPACEMUTT_TEST_ITEM(test_mailbox_update)                                       \
                                                                                 \
  /* mapping */                                                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_map_get_name)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_map_get_value)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_map_get_value_n)                                 \
                                                                                 \
  /* mbyte */                                                                    \
  SPACEMUTT_TEST_ITEM(test_buf_mb_wcstombs)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_mb_charlen)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_mb_filter_unprintable)                           \
  SPACEMUTT_TEST_ITEM(test_mutt_mb_get_initials)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_mb_is_display_corrupting_utf8)                   \
  SPACEMUTT_TEST_ITEM(test_mutt_mb_is_lower)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_mb_is_shell_char)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_mb_mbstowcs)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_mb_wcswidth)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_mb_wcwidth)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_mb_width)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_mb_width_ceiling)                                \
                                                                                 \
  /* memory */                                                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_mem_calloc)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_mem_free)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_mem_malloc)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_mem_realloc)                                     \
                                                                                 \
  /* neomutt */                                                                  \
  SPACEMUTT_TEST_ITEM(test_spacemutt_account_add)                                \
  SPACEMUTT_TEST_ITEM(test_spacemutt_account_remove)                             \
  SPACEMUTT_TEST_ITEM(test_spacemutt_free)                                       \
  SPACEMUTT_TEST_ITEM(test_spacemutt_mailboxlist_free)                           \
  SPACEMUTT_TEST_ITEM(test_spacemutt_mailboxlist_get_all)                        \
  SPACEMUTT_TEST_ITEM(test_spacemutt_new)                                        \
                                                                                 \
  /* notify */                                                                   \
  SPACEMUTT_TEST_ITEM(test_notify_free)                                          \
  SPACEMUTT_TEST_ITEM(test_notify_new)                                           \
  SPACEMUTT_TEST_ITEM(test_notify_observer_add)                                  \
  SPACEMUTT_TEST_ITEM(test_notify_observer_remove)                               \
  SPACEMUTT_TEST_ITEM(test_notify_send)                                          \
  SPACEMUTT_TEST_ITEM(test_notify_set_parent)                                    \
                                                                                 \
  /* parameter */                                                                \
  SPACEMUTT_TEST_ITEM(test_mutt_paramlist_cmp_strict)                            \
  SPACEMUTT_TEST_ITEM(test_mutt_paramlist_free_full)                             \
  SPACEMUTT_TEST_ITEM(test_mutt_param_delete)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_param_free)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_param_get)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_param_new)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_param_set)                                       \
                                                                                 \
  /* parse */                                                                    \
  SPACEMUTT_TEST_ITEM(test_command_set)                                          \
  SPACEMUTT_TEST_ITEM(test_mutt_auto_subscribe)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_check_encoding)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_check_mime_type)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_extract_message_id)                              \
  SPACEMUTT_TEST_ITEM(test_mutt_is_message_type)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_matches_ignore)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_parse_content_type)                              \
  SPACEMUTT_TEST_ITEM(test_mutt_parse_mailto)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_parse_multipart)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_parse_part)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_read_mime_header)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_rfc822_parse_line)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_rfc822_parse_message)                            \
  SPACEMUTT_TEST_ITEM(test_mutt_rfc822_read_header)                              \
  SPACEMUTT_TEST_ITEM(test_mutt_rfc822_read_line)                                \
  SPACEMUTT_TEST_ITEM(test_parse_extract_token)                                  \
  SPACEMUTT_TEST_ITEM(test_parse_rc)                                             \
  SPACEMUTT_TEST_ITEM(test_parse_set)                                            \
                                                                                 \
  /* path */                                                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_path_abbr_folder)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_path_basename)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_path_canon)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_path_dirname)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_path_escape)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_path_getcwd)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_path_realpath)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_path_tidy)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_path_tidy_dotdot)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_path_tidy_slash)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_path_tilde)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_path_to_absolute)                                \
                                                                                 \
  /* pattern */                                                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_pattern_comp)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_pattern_leak)                                    \
                                                                                 \
  /* prex */                                                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_prex_capture)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_prex_cleanup)                                    \
                                                                                 \
  /* random */                                                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_rand64)                                          \
  SPACEMUTT_TEST_ITEM(test_mutt_rand_base32)                                     \
                                                                                 \
  /* regex */                                                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_regex_capture)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_regex_compile)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_regex_free)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_regex_match)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_regex_new)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_regexlist_add)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_regexlist_free)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_regexlist_match)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_regexlist_remove)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_replacelist_add)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_replacelist_apply)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_replacelist_free)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_replacelist_match)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_replacelist_new)                                 \
  SPACEMUTT_TEST_ITEM(test_mutt_replacelist_remove)                              \
                                                                                 \
  /* rfc2047 */                                                                  \
  SPACEMUTT_TEST_ITEM(test_rfc2047_decode)                                       \
  SPACEMUTT_TEST_ITEM(test_rfc2047_decode_addrlist)                              \
  SPACEMUTT_TEST_ITEM(test_rfc2047_decode_envelope)                              \
  SPACEMUTT_TEST_ITEM(test_rfc2047_encode)                                       \
  SPACEMUTT_TEST_ITEM(test_rfc2047_encode_addrlist)                              \
  SPACEMUTT_TEST_ITEM(test_rfc2047_encode_envelope)                              \
                                                                                 \
  /* rfc2231 */                                                                  \
  SPACEMUTT_TEST_ITEM(test_rfc2231_decode_parameters)                            \
  SPACEMUTT_TEST_ITEM(test_rfc2231_encode_string)                                \
                                                                                 \
  /* signal */                                                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_sig_allow_interrupt)                             \
  SPACEMUTT_TEST_ITEM(test_mutt_sig_block)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_sig_block_system)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_sig_empty_handler)                               \
  SPACEMUTT_TEST_ITEM(test_mutt_sig_exit_handler)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_sig_init)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_sig_unblock)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_sig_unblock_system)                              \
                                                                                 \
  /* slist */                                                                    \
  SPACEMUTT_TEST_ITEM(test_slist_add_string)                                     \
  SPACEMUTT_TEST_ITEM(test_slist_dup)                                            \
  SPACEMUTT_TEST_ITEM(test_slist_equal)                                          \
  SPACEMUTT_TEST_ITEM(test_slist_free)                                           \
  SPACEMUTT_TEST_ITEM(test_slist_is_empty)                                       \
  SPACEMUTT_TEST_ITEM(test_slist_is_member)                                      \
  SPACEMUTT_TEST_ITEM(test_slist_parse)                                          \
  SPACEMUTT_TEST_ITEM(test_slist_remove_string)                                  \
  SPACEMUTT_TEST_ITEM(test_slist_to_buffer)                                      \
                                                                                 \
  /* sort */                                                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_qsort_r)                                         \
                                                                                 \
  /* string */                                                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_istr_equal)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_istr_find)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_istr_remall)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_istrn_cmp)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_istrn_equal)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_istrn_rfind)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_str_adjust)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_str_asprintf)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_str_atoi)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_str_atol)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_str_atos)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_str_atoui)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_str_atoul)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_str_atoull)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_str_coll)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_str_copy)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_str_dup)                                         \
  SPACEMUTT_TEST_ITEM(test_mutt_str_equal)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_str_find_word)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_str_getenv)                                      \
  SPACEMUTT_TEST_ITEM(test_mutt_str_hyphenate)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_str_inbox_cmp)                                   \
  SPACEMUTT_TEST_ITEM(test_mutt_str_is_ascii)                                    \
  SPACEMUTT_TEST_ITEM(test_mutt_str_is_email_wsp)                                \
  SPACEMUTT_TEST_ITEM(test_mutt_str_len)                                         \
  SPACEMUTT_TEST_ITEM(test_mutt_str_lower)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_str_lws_len)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_str_remove_trailing_ws)                          \
  SPACEMUTT_TEST_ITEM(test_mutt_str_replace)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_str_sep)                                         \
  SPACEMUTT_TEST_ITEM(test_mutt_str_skip_email_wsp)                              \
  SPACEMUTT_TEST_ITEM(test_mutt_str_skip_whitespace)                             \
  SPACEMUTT_TEST_ITEM(test_mutt_str_startswith)                                  \
  SPACEMUTT_TEST_ITEM(test_mutt_str_sysexit)                                     \
  SPACEMUTT_TEST_ITEM(test_mutt_str_upper)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_strn_copy)                                       \
  SPACEMUTT_TEST_ITEM(test_mutt_strn_equal)                                      \
                                                                                 \
  /* tags */                                                                     \
  SPACEMUTT_TEST_ITEM(test_driver_tags_free)                                     \
  SPACEMUTT_TEST_ITEM(test_driver_tags_get)                                      \
  SPACEMUTT_TEST_ITEM(test_driver_tags_get_transformed)                          \
  SPACEMUTT_TEST_ITEM(test_driver_tags_get_transformed_for)                      \
  SPACEMUTT_TEST_ITEM(test_driver_tags_get_with_hidden)                          \
  SPACEMUTT_TEST_ITEM(test_driver_tags_replace)                                  \
                                                                                 \
  /* thread */                                                                   \
  SPACEMUTT_TEST_ITEM(test_clean_references)                                     \
  SPACEMUTT_TEST_ITEM(test_find_virtual)                                         \
  SPACEMUTT_TEST_ITEM(test_insert_message)                                       \
  SPACEMUTT_TEST_ITEM(test_is_descendant)                                        \
  SPACEMUTT_TEST_ITEM(test_mutt_break_thread)                                    \
  SPACEMUTT_TEST_ITEM(test_unlink_message)                                       \
                                                                                 \
  /* url */                                                                      \
  SPACEMUTT_TEST_ITEM(test_url_check_scheme)                                     \
  SPACEMUTT_TEST_ITEM(test_url_free)                                             \
  SPACEMUTT_TEST_ITEM(test_url_parse)                                            \
  SPACEMUTT_TEST_ITEM(test_url_pct_decode)                                       \
  SPACEMUTT_TEST_ITEM(test_url_pct_encode)                                       \
  SPACEMUTT_TEST_ITEM(test_url_tobuffer)                                         \
  SPACEMUTT_TEST_ITEM(test_url_tostring)

/******************************************************************************
 * You probably don't need to touch what follows.
 *****************************************************************************/
// clang-format off
#define SPACEMUTT_TEST_ITEM(x) void x(void);
SPACEMUTT_TEST_LIST
#if defined(USE_LZ4) || defined(USE_ZLIB) || defined(USE_ZSTD)
  SPACEMUTT_TEST_ITEM(test_compress_common)
#endif
#ifdef USE_LZ4
  SPACEMUTT_TEST_ITEM(test_compress_lz4)
#endif
#ifdef USE_NOTMUCH
  SPACEMUTT_TEST_ITEM(test_nm_parse_type_from_query)
  SPACEMUTT_TEST_ITEM(test_nm_query_type_to_string)
  SPACEMUTT_TEST_ITEM(test_nm_string_to_query_type)
  SPACEMUTT_TEST_ITEM(test_nm_string_to_query_type_mapper)
  SPACEMUTT_TEST_ITEM(test_nm_windowed_query_from_query)
  SPACEMUTT_TEST_ITEM(test_nm_tag_string_to_tags)
#endif
#ifdef USE_ZLIB
  SPACEMUTT_TEST_ITEM(test_compress_zlib)
#endif
#ifdef USE_ZSTD
  SPACEMUTT_TEST_ITEM(test_compress_zstd)
#endif
#if defined(HAVE_BDB) || defined(HAVE_GDBM) || defined(HAVE_KC) || defined(HAVE_LMDB) || defined(HAVE_QDBM) || defined(HAVE_ROCKSDB) || defined(HAVE_TC) || defined(HAVE_TDB)
  SPACEMUTT_TEST_ITEM(test_store_store)
#endif
#ifdef HAVE_BDB
  SPACEMUTT_TEST_ITEM(test_store_bdb)
#endif
#ifdef HAVE_GDBM
  SPACEMUTT_TEST_ITEM(test_store_gdbm)
#endif
#ifdef HAVE_KC
  SPACEMUTT_TEST_ITEM(test_store_kc)
#endif
#ifdef HAVE_LMDB
  SPACEMUTT_TEST_ITEM(test_store_lmdb)
#endif
#ifdef HAVE_QDBM
  SPACEMUTT_TEST_ITEM(test_store_qdbm)
#endif
#ifdef HAVE_ROCKSDB
  SPACEMUTT_TEST_ITEM(test_store_rocksdb)
#endif
#ifdef HAVE_TDB
  SPACEMUTT_TEST_ITEM(test_store_tdb)
#endif
#ifdef HAVE_TC
  SPACEMUTT_TEST_ITEM(test_store_tc)
#endif
#undef SPACEMUTT_TEST_ITEM

TEST_LIST = {
#define SPACEMUTT_TEST_ITEM(x) { #x, x },
  SPACEMUTT_TEST_LIST
#if defined(USE_LZ4) || defined(USE_ZLIB) || defined(USE_ZSTD)
SPACEMUTT_TEST_ITEM(test_compress_common)
#endif
#ifdef USE_LZ4
  SPACEMUTT_TEST_ITEM(test_compress_lz4)
#endif
#ifdef USE_NOTMUCH
  SPACEMUTT_TEST_ITEM(test_nm_parse_type_from_query)
  SPACEMUTT_TEST_ITEM(test_nm_query_type_to_string)
  SPACEMUTT_TEST_ITEM(test_nm_string_to_query_type)
  SPACEMUTT_TEST_ITEM(test_nm_string_to_query_type_mapper)
  SPACEMUTT_TEST_ITEM(test_nm_windowed_query_from_query)
  SPACEMUTT_TEST_ITEM(test_nm_tag_string_to_tags)
#endif
#ifdef USE_ZLIB
  SPACEMUTT_TEST_ITEM(test_compress_zlib)
#endif
#ifdef USE_ZSTD
  SPACEMUTT_TEST_ITEM(test_compress_zstd)
#endif
#if defined(HAVE_BDB) || defined(HAVE_GDBM) || defined(HAVE_KC) || defined(HAVE_LMDB) || defined(HAVE_QDBM) || defined(HAVE_ROCKSDB) || defined(HAVE_TC) || defined(HAVE_TDB)
  SPACEMUTT_TEST_ITEM(test_store_store)
#endif
#ifdef HAVE_BDB
  SPACEMUTT_TEST_ITEM(test_store_bdb)
#endif
#ifdef HAVE_GDBM
  SPACEMUTT_TEST_ITEM(test_store_gdbm)
#endif
#ifdef HAVE_KC
  SPACEMUTT_TEST_ITEM(test_store_kc)
#endif
#ifdef HAVE_LMDB
  SPACEMUTT_TEST_ITEM(test_store_lmdb)
#endif
#ifdef HAVE_QDBM
  SPACEMUTT_TEST_ITEM(test_store_qdbm)
#endif
#ifdef HAVE_ROCKSDB
  SPACEMUTT_TEST_ITEM(test_store_rocksdb)
#endif
#ifdef HAVE_TDB
  SPACEMUTT_TEST_ITEM(test_store_tdb)
#endif
#ifdef HAVE_TC
  SPACEMUTT_TEST_ITEM(test_store_tc)
#endif
#undef SPACEMUTT_TEST_ITEM
  { 0 }
};
// clang-format on
