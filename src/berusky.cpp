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


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "berusky.h"
#include "berusky_gui.h"

/* berusky-config defininitions
*/
int berusky_config::game_fps;
int berusky_config::new_gfx;

int berusky_config::screen_depth;
int berusky_config::fullscreen;

int berusky_config::double_size;
int berusky_config::double_size_gui;

int berusky_config::game_resolution_x;
int berusky_config::game_resolution_y;

int berusky_config::level_resolution_x;
int berusky_config::level_resolution_y;

int berusky_config::level_screen_start_x;
int berusky_config::level_screen_start_y;

int berusky_config::editor_resolution_x;
int berusky_config::editor_resolution_y;

int berusky_config::editor_screen_start_x;
int berusky_config::editor_screen_start_y;

int berusky_config::cell_size_x;
int berusky_config::cell_size_y;

void berusky_config::original_size_set(void)
{
  new_gfx = false;

  game_resolution_x = 640;
  game_resolution_y = 480;

  level_resolution_x = 640;
  level_resolution_y = 420;

  level_screen_start_x = 0;
  level_screen_start_y = 40;

  editor_resolution_x = 1024;
  editor_resolution_y = 768;

  editor_screen_start_x = EDITOR_LAYER_STATUS_X;
  editor_screen_start_y = EDITOR_LAYER_STATUS_Y+EDITOR_LAYER_STATUS_DY;

  cell_size_x = 20;
  cell_size_y = 20;
}

void berusky_config::double_size_set(void)
{
  new_gfx = true;

  game_resolution_x = 1280;
  game_resolution_y = 900;

  level_resolution_x = 1280;
  //level_resolution_y = 840; TODO
  level_resolution_y = 480;

  level_screen_start_x = 0;
  level_screen_start_y = 40;

  editor_resolution_x = 1280;
  editor_resolution_y = 900;

  editor_screen_start_x = 0;
  editor_screen_start_y = 0;

  cell_size_x = 40;
  cell_size_y = 40;
}

void berusky_config::config_load(const char *p_ini_file)
{
  game_fps = 30;  

  fullscreen = get_fullscreen(p_ini_file);
  screen_depth = get_colors(p_ini_file, SCREEN_DEPTH_DEFAULT);
  double_size_gui = double_size = get_doublesize(p_ini_file);

  if(DOUBLE_SIZE) {
    double_size_set();
  }
  else {
    original_size_set();
  }
}

berusky::berusky(ITEM_REPOSITORY *p_repo_, DIR_LIST *p_dir_)
: p_repo(p_repo_), p_dir(p_dir_)
{
  state.game_state_set(LOADING);
}

berusky::~berusky(void)
{
  /* TODO: Remove/free all loaded items */
}

/*
*/
bool berusky::level_load(char *p_level)
{
  state.game_state_set(LOADING);
  state.level_set = 0;
  state.level_num = 0;
  state.level_custom = TRUE;

  return(store.level_load(p_dir, p_level));
}

bool berusky::levelset_load(int set)
{
  state.game_state_set(LOADING);
  state.level_set = set;
  state.level_num = 0;
  state.level_custom = FALSE;

  #define LEVELSET_MASK "s%d.dat"
  char file[MAX_FILENAME];
  sprintf(file,LEVELSET_MASK,set);

  return(store.levelset_load(p_dir, file));
}

bool berusky::levelset_is_custom(void)
{
  return(state.level_custom);
}

int  berusky::levelset_get_levelnum(void)
{
  return(store.levelset_get_levelnum());
}

char * berusky::levelset_get_passwd(int level)
{
  return(store.levelset_get_passwd(level));
}

char * berusky::levelset_get_difficulty(void)
{
  static char *p_name_table[5] = {NULL,NULL,NULL,NULL,NULL};  

  if(!p_name_table[0]) {
    p_name_table[0] = _("training");
    p_name_table[1] = _("easy");
    p_name_table[2] = _("intermediate");
    p_name_table[3] = _("advanced");
    p_name_table[4] = _("impossible");
  }

  assert(state.level_set < (int)(sizeof(p_name_table)));
  return(p_name_table[state.level_set]);
}

char * berusky::levelset_get_name(void)
{
  return(store.levelset_get_name(state.level_num));
}

bool berusky::levelset_search(char *p_passwd, int *p_set, int *p_level)
{
  int i;
  for(i = 0; i < LEVELSET_NUM; i++) {
    if(levelset_load(i) && store.levelset_search(p_passwd, p_level)) {
      *p_set = i;
      return(TRUE);
    }
  }
  return(FALSE);
}

void berusky::levelset_set_level( int level )
{
  state.level_num = level;
}

int  berusky::levelset_get_level(void)
{
  return(state.level_num);
}

int  berusky::levelset_get_set(void)
{
  return(state.level_set);
}

LEVEL_STATUS * berusky::level_status_get(void)
{
  return(store.status_get());
}

bool berusky::level_play(LEVEL_EVENT_QUEUE *p_queue, int level)
{
  state.game_state_set(PLAYING);
  state.level_num = level;
  return(store.play_start(p_repo, p_queue, level));
}

bool berusky::level_callback(LEVEL_EVENT_QUEUE *p_queue)
{
  if(state.game_state_get() != SUSPENDED)
    return(store.play_callback(p_queue));
  else
    return(FALSE);
}

LEVEL_STATUS * berusky::level_stop(LEVEL_EVENT_QUEUE *p_queue)
{
  state.game_state_set(ENDING);
  store.play_end(p_queue);

  return(level_status_get());
}

void berusky::level_restart(LEVEL_EVENT_QUEUE *p_queue)
{
  level_stop(p_queue);
  if(!level_play(p_queue, state.level_num)) {
    bprintf(_("Unable to restart level %d!"),state.level_num);
  }
}

void berusky::level_suspend(LEVEL_EVENT_QUEUE *p_queue)
{
  state.game_state_set(SUSPENDED);
}

void berusky::level_restore(LEVEL_EVENT_QUEUE *p_queue)
{
  state.game_state_set(PLAYING);
  store.redraw();
}

BERUSKY_STATE berusky::status_get(void)
{
  return(state);
}

void berusky::level_save(void)
{
  store.level_save();
}

bool berusky::level_load(void)
{
  return(store.level_load());
}
