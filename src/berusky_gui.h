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

#ifndef __BERUSKY_GUI_H
#define __BERUSKY_GUI_H

#include "gui.h"

#define FIRST_PIPE            (FIRST_CLASSIC_LEVEL+13)

#define LEVEL_DONE            (FIRST_CLASSIC_LEVEL+43)
#define LEVEL_NEXT            (FIRST_CLASSIC_LEVEL+45)
#define LEVEL_CLOSED          (FIRST_CLASSIC_LEVEL+42)

#define ITEM_SIZE             (DOUBLE_SIZE ? 40 : 20)
#define TEXT_SHIFT_VERTICAL   (DOUBLE_SIZE ? 10 : 0)
#define TEXT_SHIFT_HORIZONTAL (DOUBLE_SIZE ? 50 : 20)

#define MENU_PATH_GROUP       1

// -------------------------------------------------------
// Game UI
// -------------------------------------------------------
typedef class game_gui : public gui_base {

  ITEM_REPOSITORY   *p_repo;
  DIR_LIST          *p_dir;
  BERUSKY           *p_ber;

  BERUSKY_SOUND     sound;
  BERUSKY_PROFILE   profile;

public:
  
  game_gui(ITEM_REPOSITORY *p_repo_, DIR_LIST *p_dir_);
  ~game_gui(void);

  void player_profile_load(void);
  void player_profile_save(void);

  // Run level in GUI
  void level_set_select(int level_set);
  void level_select(int level, tpos spr_x, tpos spr_y);

  bool level_run(LEVEL_EVENT_QUEUE *p_queue, char *p_level);
  bool level_run(LEVEL_EVENT_QUEUE *p_queue);
  void level_stop(LEVEL_EVENT_QUEUE *p_queue, int cheat, int menu);
  void level_suspend(LEVEL_EVENT_QUEUE *p_queue);
  void level_restore(LEVEL_EVENT_QUEUE *p_queue);
  void level_restart(LEVEL_EVENT_QUEUE *p_queue);
  void level_save(LEVEL_EVENT_QUEUE *p_queue);
  void level_load(LEVEL_EVENT_QUEUE *p_queue);

  char * level_hint_load(int set, int level);
  char * level_end_text_load(int set);
  char * credit_text_load(void);

  // Run menu (create menu background) in GUI
  void menu_double_size_question(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_double_size_question_switch(void);
  void menu_double_size_set(bool double_size);
  void menu_main(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_new_game(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_profiles(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_profile_create(size_ptr data);
  void menu_profile_select(size_ptr data, size_ptr data1);
  void menu_settings(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_level_hint(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_help(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_help_keys(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_help_rules(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_help_credits(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_in_game(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);  

  // Level screens
  void menu_level_run_new(MENU_STATE state, size_ptr level_set = 0, size_ptr unused = 0);  
  void menu_level_end(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_level_end_custom(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);  
  void menu_levelset_end(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  
  // Helpers
  void menu_settings_fullscreen(void);
  void menu_settings_doublesize(void);
  void menu_level_run_path_draw(int level_set, int level_act, int level_num, int level_last);
  int  menu_level_run_path_draw_line(const char *p_path, int level_act, int level_num, int level_last, int level_set, int sx, int sy);
  void menu_level_draw_level(int lev, int level_act, int level_num, int level_last, int level_set, int x, int y);
  void menu_level_draw_pipe(int pip, int x, int y);
  void menu_level_name_print(void);

  // Callback for all
  bool callback(LEVEL_EVENT_QUEUE *p_queue, int frame);

  void menu_dialog_error(char *p_text,...);

  void run_editor(void);
  
} GAME_GUI;

#endif
