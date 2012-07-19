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

// -------------------------------------------------------
// Game UI
// -------------------------------------------------------
typedef class game_gui : public gui_base {

  ITEM_REPOSITORY *p_repo;
  DIR_LIST        *p_dir;
  BERUSKY         *p_ber;

public:
  
  game_gui(ITEM_REPOSITORY *p_repo_, DIR_LIST *p_dir_);
  ~game_gui(void);

  // Run level in GUI
  bool level_run(LEVEL_EVENT_QUEUE *p_queue, char *p_level);
  bool level_run(LEVEL_EVENT_QUEUE *p_queue, int set, int level);  
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
  void menu_main(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_new_game(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_password(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_password_check(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_settings(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_level_hint(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_help(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_help_keys(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_help_rules(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_help_credits(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_in_game(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);  

  // Level screens
  void menu_level_run(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_level_run_new(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_level_end(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  void menu_level_end_custom(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);  
  void menu_levelset_end(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);
  
  // Helpers
  void menu_settings_fullscreen(void);
  void menu_level_run_path_draw(int level_set);
  
  // Callback for all
  bool callback(LEVEL_EVENT_QUEUE *p_queue, int frame);

  void menu_dialog_error(char *p_text,...);

  void run_editor(void);

} GAME_GUI;

#endif
