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

/*
  Berusky
*/

#ifndef __BERUSKY_H
#define __BERUSKY_H

using namespace std;

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/* System include files
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <locale.h>

#include "portability.h"

#ifdef LINUX

#include <syslog.h>
#include <libintl.h>
#include <SDL/SDL.h>

#define _(string) gettext (string)

#elif WINDOWS

#include "SDL.h"
#define _(string) (string)

#endif

typedef struct berusky_config {

  static int game_fps;

  static int screen_depth;
  static int fullscreen;

  static int double_size;
  static int double_size_gui;
  static int new_gfx;

  static int game_resolution_x;
  static int game_resolution_y;

  static int level_resolution_x;
  static int level_resolution_y;

  static int level_screen_start_x;
  static int level_screen_start_y;

  static int editor_resolution_x;
  static int editor_resolution_y;

  static int editor_screen_start_x;
  static int editor_screen_start_y;

  static int cell_size_x;
  static int cell_size_y;

public:

  static void original_size_set(void);
  static void double_size_set(void);

  static void config_load(const char *p_ini_file);

} BERUSKY_CONFIG;

/* Basic header files
*/
#include "defines.h"

#include "types.h"

#include "ini.h"

#include "utils.h"

#include "2d_graph.h"
#include "graphics.h"

#include "events.h"

#include "llist.h"
#include "input.h"

//#include "timer.h"

#include "data_parser.h"
#include "data_parser_tables.h"

#include "level.h"
#include "level_game.h"
#include "animation.h"
#include "game_logic.h"
#include "level_changer.h"
#include "level_active.h"
#include "profile.h"

typedef enum _GAME_STATE {

  LOADING,
  PLAYING,
  SUSPENDED,
  ENDING

} GAME_STATE;

#define GAME_GRAPHICS 0x1
#define MENU_GRAPHICS 0x2

typedef class berusky_state {

public:
  
  GAME_STATE state;

  int        load_flag;

  int        level_set;
  int        level_num;
  bool       level_custom;

public:
  
  berusky_state(void)
  {
    memset(this,0,sizeof(*this));
  }
  
  void game_state_set(GAME_STATE st)
  {
    state = st;
  }

  GAME_STATE game_state_get(void)
  {
    return(state);
  }

} BERUSKY_STATE;

typedef class berusky_sound {

public:

  int sound_on;
  int sound_volume;
  int music_on;
  int music_volume;

public:

  berusky_sound(void)
  {
    memset(this,0,sizeof(*this));
  }

} BERUSKY_SOUND;

typedef class berusky {
  
  LEVEL_STORE       store;

  ITEM_REPOSITORY  *p_repo;
  DIR_LIST         *p_dir;

  BERUSKY_STATE     state;

public:

  berusky(ITEM_REPOSITORY *p_repo_, DIR_LIST *p_dir_);
  ~berusky(void);

  bool   level_load(char *p_level);
  bool   levelset_load(int set);
  bool   levelset_is_custom(void);

  void   levelset_set_level(int level);
  int    levelset_get_level(void);
  int    levelset_get_set(void);
  int    levelset_get_levelnum(void);
  char * levelset_get_difficulty(void);
  char * levelset_get_passwd(int level);
  char * levelset_get_name(void);
  bool   levelset_search(char *p_passwd, int *p_set, int *p_level);
  
  LEVEL_STATUS * level_status_get(void);

  bool   level_play(LEVEL_EVENT_QUEUE *p_queue, int level);
  bool   level_callback(LEVEL_EVENT_QUEUE *p_queue);
  LEVEL_STATUS * level_stop(LEVEL_EVENT_QUEUE *p_queue);

  void   level_restart(LEVEL_EVENT_QUEUE *p_queue);

  void   level_suspend(LEVEL_EVENT_QUEUE *p_queue);
  void   level_restore(LEVEL_EVENT_QUEUE *p_queue);

  void   level_save(void);
  bool   level_load(void);

  BERUSKY_STATE status_get(void);  

} BERUSKY;

#endif
