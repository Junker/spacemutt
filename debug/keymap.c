/**
 * @file
 * Dump keybindings
 *
 * @authors
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
 * @page debug_keymap Dump keybindings
 *
 * Dump keybindings
 */

#include "config.h"
#include <stdio.h>
#include "mutt/lib.h"
#include "config/lib.h"
#include "gui/lib.h"
#include "key/lib.h"
#include "menu/lib.h"

/**
 * log_bind - Dumps all the binds maps of a menu into a buffer
 * @param menu   Menu to dump
 * @param keystr Bound string
 * @param map    Keybinding
 */
void log_bind(enum MenuType menu, const char *keystr, struct Keymap *map)
{
  if (map->op == OP_NULL)
  {
    log_debug1("    bind %s noop", keystr);
    return;
  }

  const char *fn_name = NULL;
  /* The pager and editor menus don't use the generic map,
   * however for other menus try generic first. */
  if ((menu != MENU_PAGER) && (menu != MENU_EDITOR) && (menu != MENU_GENERIC))
  {
    fn_name = mutt_get_func(OpGeneric, map->op);
  }

  /* if it's one of the menus above or generic doesn't find the function,
   * try with its own menu. */
  if (!fn_name)
  {
    const struct MenuFuncOp *funcs = km_get_table(menu);
    if (!funcs)
      return;

    fn_name = mutt_get_func(funcs, map->op);
  }

  log_debug1("    bind %-8s <%s>", keystr, fn_name);
  log_debug1("        op = %d (%s)", map->op, opcodes_get_name(map->op));
  log_debug1("        eq = %d", map->eq);

  struct Buffer *keys = buf_pool_get();
  for (int i = 0; i < map->len; i++)
  {
    buf_add_printf(keys, "%d ", map->keys[i]);
  }
  log_debug1("        keys: %s", buf_string(keys));
  buf_pool_release(&keys);
}

/**
 * log_macro - Dumps all the macros maps of a menu into a buffer
 * @param keystr Bound string
 * @param map    Keybinding
 */
void log_macro(const char *keystr, struct Keymap *map)
{
  struct Buffer *esc_macro = buf_pool_get();
  escape_string(esc_macro, map->macro);

  log_debug1("    macro %-8s \"%s\"", keystr, buf_string(esc_macro));
  if (map->desc)
    log_debug1("        %s", map->desc);

  buf_pool_release(&esc_macro);

  log_debug1("        op = %d", map->op);
  log_debug1("        eq = %d", map->eq);
  struct Buffer *keys = buf_pool_get();
  for (int i = 0; i < map->len; i++)
  {
    buf_add_printf(keys, "%d ", map->keys[i]);
  }
  log_debug1("        keys: %s", buf_string(keys));
  buf_pool_release(&keys);
}

/**
 * log_menu - Dump a Menu's keybindings to the log
 * @param menu Menu to dump
 * @param kml  Map of keybindings
 */
void log_menu(enum MenuType menu, struct KeymapList *kml)
{
  struct Keymap *map = NULL;

  if (STAILQ_EMPTY(kml))
  {
    log_debug1("    [NONE]");
    return;
  }

  STAILQ_FOREACH(map, kml, entries)
  {
    char key_binding[128] = { 0 };
    km_expand_key(key_binding, sizeof(key_binding), map);

    struct Buffer *esc_key = buf_pool_get();
    escape_string(esc_key, key_binding);

    if (map->op == OP_MACRO)
      log_macro(buf_string(esc_key), map);
    else
      log_bind(menu, buf_string(esc_key), map);

    buf_pool_release(&esc_key);
  }
}

/**
 * dump_keybindings - Dump all the keybindings to the log
 */
void dump_keybindings(void)
{
  log_debug1("Keybindings:");
  for (int i = 1; i < MENU_MAX; i++)
  {
    log_debug1("Menu: %s", mutt_map_get_name(i, MenuNames));
    log_menu(i, &Keymaps[i]);
  }
}
