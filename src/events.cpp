/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * Berusky (C) AnakreoN
 * Martin Stransky <stransky@anakreon.cz> 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "berusky.h"

const char * level_event::translation_table[] =
{
  "EV_NONE",
  "EV_TEST",
  "GI_BLOCK_KEYS",
  "GI_UNBLOCK_ALL",
  "GI_KEY_DOWN",
  "GI_MOUSE_MOVE",
  "GI_MOUSE_EVENT",
  "GI_MENU_BACK_POP",
  "GI_MENU_BACK_PUSH",
  "GI_SPRITE_DRAW",
  "GI_STRING_DRAW",
  "GI_CHECKBOX_SWITCH",
  "GL_PLAYER_MOVE",
  "GL_PLAYER_MOVE_FAST",
  "GL_PLAYER_SWITCH",
  "GL_CELL_SET_ITEM",
  "GL_CELL_SET_VARIATION",
  "GL_CELL_SET_ROTATION",
  "GL_CELL_SET_DIFF",
  "GL_CELL_CLEAR_ALL",
  "GL_CELL_MOVE",
  "PL_PLAYER_SET_POS",
  "AN_RUN",
  "AN_GENERATE_AND_RUN",
  "AN_STOP_AND_CLEAR",
  "GC_RUN_LEVEL_SET",
  "GC_RUN_LEVEL_LINE",
  "GC_RUN_EDITOR",
  "GC_STOP_LEVEL",
  "GC_MENU_RUN_LEVEL",
  "GC_MENU_END_LEVEL",
  "GC_MENU_END_LEVEL_CUSTOM",
  "GC_MENU_END_LEVEL_SET",
  "GC_MENU_PROFILE_CREATE",
  "GC_MENU_PROFILE_SELECT",
  "GC_SUSPEND_LEVEL",
  "GC_RESTORE_LEVEL",
  "GC_RESTART_LEVEL",
  "GC_SAVE_LEVEL",
  "GC_LOAD_LEVEL",
  "GC_MENU_START",
  "GC_MENU_NEW_GAME",
  "GC_MENU_PROFILES",
  "GC_MENU_SETTINGS",
  "GC_MENU_SETTINGS_FULSCREEN_SWITCH",
  "GC_MENU_SETTINGS_DOUBLESIZE_SWITCH",
  "GC_MENU_SETTINGS_SOUND_SWITCH",
  "GC_MENU_SETTINGS_MUSIC_SWITCH",
  "GC_MENU_LEVEL_HINT",
  "GC_MENU_HELP",
  "GC_MENU_HELP_KEYS",
  "GC_MENU_HELP_RULES",
  "GC_MENU_HELP_CREDIT",
  "GC_MENU_IN_GAME",
  "GC_MENU_QUIT",
  "ED_LEVEL_NEW",
  "ED_LEVEL_LOAD",
  "ED_LEVEL_SAVE",
  "ED_LEVEL_SAVE_AS",
  "ED_LEVEL_SELECT_LAYER",
  "ED_LEVEL_SET_CURSOR",
  "ED_LEVEL_RECTANGLE_SELECTION",
  "ED_LEVEL_DRAW_CURSOR_INSERT_ITEM",
  "ED_LEVEL_DRAW_CURSOR_ROTATE_ITEM",
  "ED_LEVEL_DRAW_CURSOR_VARIATE_ITEM",
  "ED_LEVEL_DRAW_CURSOR_CLEAR_ITEM",
  "ED_LEVEL_CHANGE_BACKGROUND",
  "ED_HELP",
  "ED_QUIT",
  "ED_ROTATE_SELECTION",
  "ED_VARIATE_SELECTION",
  "ED_LEVEL_SHADER",
  "ED_LEVEL_RUN",
  "ED_LEVEL_LAYER",
  "ED_LEVEL_IPANEL_SCROLL",
  "ED_LEVEL_MOUSE_PANEL_SCROLL",
  "ED_LEVEL_IPANEL_SELECT",
  "ED_LEVEL_IPANEL_SELECT_LOCK",
  "ED_UNDO",
  "ED_REDO",
  "EV_LAST",
};

const char * level_event::translate_event(EVENT_TYPE action)
{
  return(translation_table[action]);
}

void level_event::translate_test(void)
{
  assert(!strcmp(translate_event(AN_RUN), "AN_RUN"));
  assert(!strcmp(translate_event(GC_MENU_SETTINGS), "GC_MENU_SETTINGS"));
  assert(!strcmp(translate_event(GC_MENU_QUIT), "GC_MENU_QUIT"));
  assert(!strcmp(translate_event(ED_LEVEL_RUN), "ED_LEVEL_RUN"));
  assert(!strcmp(translate_event(ED_QUIT), "ED_QUIT"));
  assert(!strcmp(translate_event(EV_LAST), "EV_LAST"));

  int size = (sizeof(level_event::translation_table)/sizeof(level_event::translation_table[0]));
  assert(size == EV_LAST+1);  
}

void event_stream::testcase(void)
{
  clear();
  extend(EXTEND_STEP);

  LEVEL_EVENT ev(EV_TEST);
  ev.translate_test();

  assert(empty());
  assert(!full());
  add(ev);
  assert(read_first == 0);
  assert(read_next == 0);
  assert(write_first == 0);
  assert(write_next == 1);
  commit();
  assert(!empty());
  assert(!full());
  assert(read_first == 0);
  assert(read_next == 1);
  assert(write_first == 1);
  assert(write_next == 1);
  ev = get();
  assert(empty());
  assert(!full());
  assert(read_first == 1);
  assert(read_next == 1);
  assert(write_first == 1);
  assert(write_next == 1);
  
  clear();
}
