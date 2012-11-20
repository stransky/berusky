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

#ifndef __LEVEL_GAME_H__
#define __LEVEL_GAME_H__

#include <time.h>
#include "stack.h"

#define ROTATE_ITEM(rotation, direction)                 \
{                                                        \
  (rotation) += (direction);                             \
  if((rotation) < ROTATION_MIN) {                        \
    (rotation) = ROTATION_MAX;                           \
  } else if((rotation) > ROTATION_MAX) {                 \
    (rotation) = ROTATION_MIN;                           \
  }                                                      \
}

/**************************************************************** 
  Player - structure for storing information about player
 ****************************************************************
*/

#define MAX_PLAYERS          5
#define PLAYER_ACTIVE        0x1

#define PLAYER_MAX_MATTLOCKS 9
#define PLAYER_MAX_KEYS      1

#define PLAYER_ITEM          1
#define PLAYER_HAND          3

#define PLAYER_CURSOR        0

typedef class player {

public:

  int         flag; // player flags
  
  int         num;  // player number
  tpos        x,y;  // actual position
  int         rot;  // player rotation
  
  int         matt; // num of mattocks
  int         keys; // num of keys
  
  item_handle item;

  int         a_ls;
  int         a_rs;

  spr_handle  ls[4];  //left sprites
  spr_handle  rs[4];  //right sprites

public:

  player(void)
  {
    memset(this,0,sizeof(*this));
  
    a_ls = PLAYER_HAND;
    a_rs = PLAYER_HAND;
  }

  void set_graphics(int num);

  void set_item(item_handle itm)
  {
    item = item;
  }

  item_handle get_item(void)
  {
    return(item);
  }

  void set_pos(tpos x_, tpos y_)
  {
    x = x_;
    y = y_;
  }
  
  void get_pos(tpos *p_x, tpos *p_y)
  {
    *p_x = x;
    *p_y = y;
  }

  tpos get_pos_x(void)
  {
    return(x);    
  }
  
  tpos get_pos_y(void)
  {
    return(y);    
  }
 
  void set_rot(int rot_)
  {
    rot = rot_;
  }
  
  int get_rot(void)
  {
    return(rot);
  }
  
  // key mattlock
  bool mattock_add(void)
  {
    if(matt < PLAYER_MAX_MATTLOCKS) {
      matt++;
      a_ls = PLAYER_ITEM;
      return(TRUE);
    } else {
      return(FALSE);
    }    
  }
  
  bool mattock_drop(void)
  {
    if(matt > 0) {
      matt--;
      if(!matt)
        a_ls = PLAYER_HAND;
      return(TRUE);
    } else {
      return(FALSE);
    }
  }
  
  // key managment
  bool key_add(void)
  {
    if(keys < PLAYER_MAX_KEYS) {
      keys++;
      a_rs = PLAYER_ITEM;
      return(TRUE);
    } else {
      return(FALSE);
    }      
  }
  
  bool key_drop(void)
  {
    if(keys > 0) {
      keys--;
      if(!keys)
        a_rs = PLAYER_HAND;
      return(TRUE);
    } else {
      return(FALSE);
    }
  } 
  
  void set_active(void)
  {
    flag |= PLAYER_ACTIVE;
  }

} PLAYER;

/***********************************************************************
   Level structures on disc
  **********************************************************************
*/

#define POS_ITEM        0
#define POS_VARIATION   1
#define POS_ROTATION    2
#define POS_ANIMATION   3

typedef unsigned short int word;

#define H_LEVEL  "Level 3 (C) Anakreon 1999"

#ifdef LINUX
typedef struct __attribute__ ((__packed__)) {
#elif WINDOWS
#pragma pack(push,1)
typedef struct {
#endif

   char signum[30];               // -> "Berusky (C) Anakreon 1998"
   char back;                     // -> background number
   char music;                    // -> track number
   char rot[5];                   // -> players rotation
   char rezerved[100];            // -> reserved
   word floor[LEVEL_CELLS_Y][LEVEL_CELLS_X][10];  //-> floor cells
   word level[LEVEL_CELLS_Y][LEVEL_CELLS_X][10];  //-> level items
   word players[LEVEL_CELLS_Y][LEVEL_CELLS_X];    //-> players

} LEVEL_DISK;

#ifdef WINDOWS
#pragma pack(pop)
#endif

typedef struct level_cell {

  item_handle   item;       // -> 
  item_handle   variation;  // -> 
  item_handle   rotation;   // ->   
  anim_handle   animation;

public:
  
  bool used(void)
  {
    return(item != NO_ITEM);
  }

} LEVEL_CELL;

/***********************************************************************
   Generic level interface - for game & editor
  **********************************************************************
*/
typedef class level_generic {

protected:
  
  char	           background;               // -> num of background
  char	           music;                    // -> num of music  

  LEVEL_CELL       cells[LEVEL_CELLS_X][LEVEL_CELLS_Y][LEVEL_LAYERS];
  
protected:
  
  ITEM_REPOSITORY *p_repo;     // repository with items
  SCREEN          *p_scr;      // screen class for drawing

protected:

  int              player_active;
  PLAYER           players[MAX_PLAYERS];

public:

  level_generic(ITEM_REPOSITORY *p_rep);
  ~level_generic(void);

  /***********************************************************************
     Look of the level
    **********************************************************************
  */

  // Is the cell in area?
  bool cell_in_level(tpos x, tpos y, tpos layer = 0)
  {
    return(x >= 0 && x < LEVEL_CELLS_X && 
           y >= 0 && y < LEVEL_CELLS_Y && 
           layer >= 0 && layer < LEVEL_LAYERS);
  }

  // Is the global coordination in area?
  bool coord_in_level(tpos x, tpos y)
  { 
    return(p_scr->coord_in_area(x,y));
  }

  // Translate global coordination to cell coordination
  void coord_to_cell(tpos *p_x, tpos *p_y)
  {
    p_scr->coord_to_grid(p_x, p_y);
  }

  // Returns a global coordination and size of given cell
  void cell_to_coord(tpos *p_x, tpos *p_y, tpos *p_dx, tpos *p_dy)
  {
    p_scr->coord_to_screen(p_x, p_y, p_dx, p_dy);
  }

  // Set offset of level window
  void window_offset_set(tpos start_x, tpos start_y)
  {
    p_scr->window_offset_set(start_x, start_y);
  }

  // Set offset of level window
  void window_offset_get(tpos &start_x, tpos &start_y)
  {
    p_scr->window_offset_get(start_x, start_y);
  }

 /***********************************************************************
     Drawing
    **********************************************************************
  */

  // Draw all changes in level to screen
  void draw(void)
  {
    p_scr->draw();
  }

  // and make them visible
  void flip(void)
  {
    p_scr->flip();
  }

  /***********************************************************************
     Graphics interface
    **********************************************************************
  */
  void cell_graphics_set(tpos x, tpos y, tpos layer, spr_handle spr, int flag = 0)
  {     
    p_scr->grid_set(x, y, layer, spr, flag);
  }

  void cell_graphics_set_and_rotate(tpos x, tpos y, tpos layer, spr_handle spr, 
                                    int rotation, int flag = 0)
  {    
    p_scr->grid_set(x, y, layer, spr + rotation * ROT_SHIFT, flag);
  }
  
  void cell_graphics_set_diff(tpos x, tpos y, tpos layer, tpos x_diff, tpos y_diff)
  {     
    p_scr->grid_diff(x, y, layer, x_diff, y_diff);
  }
  
  void cell_graphics_add_diff(tpos x, tpos y, tpos layer, tpos x_diff, tpos y_diff)
  {
    p_scr->grid_diff_add(x, y, layer, x_diff, y_diff);
  }
  
  void cell_graphics_reset(tpos x, tpos y, tpos layer)
  {
    level_cell *p_cell = cells[x][y]+layer;
    if(p_repo->item_valid(p_cell->item)) {
     p_scr->grid_set(x, y, layer,
                  p_repo->sprite_get(p_cell->item, p_cell->variation, p_cell->rotation),
                  p_repo->item_flag_get(p_cell->item, p_cell->variation));
    } else {
     p_scr->grid_set(x, y, layer, NO_ITEM, SPRITE_CLEAR);
    }
  }

  void sprite_add(int handle, spr_handle spr, tpos x, tpos y, spr_handle dst = 0)
  {
    p_scr->sprite_add(handle,spr,x,y,dst);
  }

  void sprite_mask(tpos x, tpos y)
  {    
    p_scr->sprite_mask(x,y);
  }

  /***********************************************************************
    level interface
    **********************************************************************
  */  

  void level_clear(void);

  bool level_new(void);
  bool level_load(const char *p_file);
  bool level_load(const char *p_file, const char **p_dirs, int dirs);
  bool level_save(const char *p_file);
  bool level_exists(const char *p_file);

  void level_import(LEVEL_DISK *p_lev);
  LEVEL_DISK * level_export(LEVEL_DISK *p_lev);

  bool level_background_set(int back);

  int  level_background_get(void)
  {
    return(background);
  }

  /***********************************************************************
     interface to repository (templates for items in level)
    **********************************************************************
  */
  bool repo_is_valid(item_handle item)
  {     
   return(p_repo->item_valid(item));
  }

  int repo_get_flag(tpos x, tpos y, tpos layer)
  {
    LEVEL_CELL *p_cell = cells[x][y]+layer;
    return(p_repo->item_valid(p_cell->item) ? 
      p_repo->item_flag_get(p_cell->item, p_cell->variation) : 0);
  }

  anim_template_handle repo_get_animation(tpos x, tpos y, tpos layer, 
                                          bool fast_movement)
  {
    LEVEL_CELL *p_cell = cells[x][y]+layer;
    anim_template_handle anim = ANIM_NONE;
    if(p_repo->item_valid(p_cell->item)) {
      anim = p_repo->animation_get(p_cell->item, p_cell->variation);
      if(fast_movement && p_repo->item_player(p_cell->item))
        anim += FAST_ANIMATION_SHIFT;
    }
    return(anim);
  }

  bool repo_get_animation(tpos x, tpos y, tpos layer, bool fast_movement,
                          int *p_flag,
                          anim_template_handle *p_anim_template)
  {
    assert(p_flag && p_anim_template);
  
    *p_anim_template = repo_get_animation(x,y,layer,fast_movement);
    *p_flag = repo_get_flag(x,y,layer);
  
    return((*p_flag)&ANIM_TRIGGER);
  }

  /***********************************************************************
     interface to level grid
    **********************************************************************
  */
  bool cell_is_full(tpos x, tpos y, tpos layer)
  {     
   return(p_repo->item_valid(cells[x][y][layer].item));
  }
  
  bool cell_is_free(tpos x, tpos y, tpos layer)
  {     
   return(!(p_repo->item_valid(cells[x][y][layer].item)));
  }

  void cell_set(tpos x, tpos y, tpos layer, item_handle item_,
               item_handle variation_, item_handle rotation_,
               bool graphics_change)
  {
    level_cell *p_cell = cells[x][y]+layer;
  
    p_cell->item = item_;
    p_cell->variation = variation_;
    p_cell->rotation = rotation_;
  
    cell_subitems_reset(x, y, layer, graphics_change);
  
    if(graphics_change) {
      cell_graphics_reset(x,y,layer);
    }
  }

  void cell_set(tpos sx, tpos sy, tpos dx, tpos dy, tpos layer, 
                item_handle item, item_handle variation, item_handle rotation,
                bool graphics_change, bool filled)
  {
    tpos x,y;
  
    if(filled) {
      for(x = sx; x < sx+dx; x++) {
        for(y = sy; y < sy+dy; y++) {
          cell_set(x,y,layer,item,variation,rotation,graphics_change);
        }
      }
    }
    else {
      for(y = sy; y < sy+dy; y++) {
        cell_set(sx,       y,layer,item,variation,rotation,graphics_change);
        cell_set(sx+(dx-1),y,layer,item,variation,rotation,graphics_change);
      }
      for(x = sx; x < sx+dx; x++) {
        cell_set(x,sy       ,layer,item,variation,rotation,graphics_change);
        cell_set(x,sy+(dy-1),layer,item,variation,rotation,graphics_change);
      }    
    }
  }

  void cell_set_item(tpos x, tpos y, tpos layer, 
                     item_handle item_,
                     bool rewrite_sub, 
                     bool graphics_change)
  {
    level_cell *p_cell = cells[x][y]+layer;
  
    p_cell->item = item_;
  
    if(rewrite_sub) {
      cell_subitems_reset(x, y, layer, graphics_change);
    }
  
    if(graphics_change) {
      cell_graphics_reset(x,y,layer);
    }
  }

  item_handle cell_get_item(tpos x, tpos y, tpos layer)
  {
    return(cells[x][y][layer].item);
  }

  void cell_set_variation(tpos x, tpos y, tpos layer,
                          item_handle variation_,
                          bool rewrite_sub,
                          bool graphics_change)
  {
    level_cell *p_cell = cells[x][y]+layer;
    
    p_cell->variation = variation_;
  
    if(rewrite_sub) {
      cell_subitems_reset(x, y, layer, graphics_change);
    }  
    if(graphics_change) {
      cell_graphics_reset(x,y,layer);
    }
  }

  void cell_modify_variation(tpos x, tpos y, tpos layer,
                             item_handle direction,
                             bool rewrite_sub,
                             bool graphics_change)
  {
    level_cell *p_cell = cells[x][y]+layer;
    
    p_cell->variation += direction;
  
    int variation_max = p_repo->variants_get(p_cell->item)-1;
    if(p_cell->variation < 0)
      p_cell->variation = variation_max;
    else if(p_cell->variation > variation_max)
      p_cell->variation = 0;
  
    if(rewrite_sub) {
      cell_subitems_reset(x, y, layer, graphics_change);
    }
    if(graphics_change) {
      cell_graphics_reset(x,y,layer);
    }
  }

  int cell_get_variation(tpos x, tpos y, tpos layer)
  {
    return(cells[x][y][layer].variation);
  }  

  bool cell_can_rotate(tpos x, tpos y, tpos layer)
  {
    level_cell *p_cell = cells[x][y]+layer;
    return(p_cell->used() && p_repo->item_can_rotate(p_cell->item));
  }

  void cell_set_rotation(tpos x, tpos y, tpos layer, item_handle rotation_, bool graphics_change)
  {
    level_cell *p_cell = cells[x][y]+layer;
  
    p_cell->rotation = rotation_;
  
    if(graphics_change) {
      cell_graphics_reset(x,y,layer);
    }
  }

  int cell_get_rotation(tpos x, tpos y, tpos layer)
  {
    return(cells[x][y][layer].rotation);
  }

  void cell_modify_rotation(tpos x, tpos y, tpos layer, int direction, bool graphics_change)
  {
    level_cell *p_cell = cells[x][y]+layer;
      
    ROTATE_ITEM(p_cell->rotation, direction);
  
    if(graphics_change) {
      cell_graphics_reset(x,y,layer);
    }
  }  

  void cell_set_animation(tpos x, tpos y, tpos layer, anim_handle anim)
  {
    LEVEL_CELL *p_cell = cells[x][y]+layer;
    p_cell->animation = anim;
  }

  anim_handle cell_get_animation(tpos x, tpos y, tpos layer)
  {
    LEVEL_CELL *p_cell = cells[x][y]+layer;
    return(p_cell->animation);
  }
 
  void cell_subitems_reset(tpos x, tpos y, tpos layer, bool graphics_change)
  {
    level_cell *p_cell = cells[x][y]+layer;

    if(p_repo->item_valid(p_cell->item)) {
      int i,sitems = p_repo->subitems_num_get(p_cell->item, p_cell->variation);
      for(i = 0; i < sitems; i++) {
        tpos dx,dy;

        p_repo->subitems_position_get(p_cell->item, p_cell->variation, i, &dx, &dy);

        item_handle sub_item = p_repo->subitems_item_get(p_cell->item, p_cell->variation, i);
        int sub_variation = p_repo->subitems_variation_get(p_cell->item, p_cell->variation, i);

        cell_set_item(x+dx, y+dy, layer, sub_item, FALSE, FALSE);
        cell_set_variation(x+dx, y+dy, layer, sub_variation, FALSE, graphics_change);
      }
    }
  }
    
  void cell_clear(tpos x, tpos y, tpos layer, bool graphics_change)
  {
    cells[x][y][layer].item = NO_ITEM;
    cells[x][y][layer].variation = 0;
    cells[x][y][layer].rotation = 0;
  
    p_scr->grid_diff(x, y, layer, 0, 0);
  
    if(graphics_change) {
      cell_graphics_reset(x,y,layer);
    }
  }

  void cell_clear(tpos sx, tpos sy, tpos dx, tpos dy, tpos layer, bool graphics_change, bool filled)
  {
    tpos x,y;
  
    if(filled) {
      for(x = sx; x < sx+dx; x++) {
        for(y = sy; y < sy+dy; y++) {
          cell_clear(x,y,layer,graphics_change);
        }
      }
    }
    else {
      for(y = sy; y < sy+dy; y++) {
        cell_clear(sx,       y,layer,graphics_change);
        cell_clear(sx+(dx-1),y,layer,graphics_change);
      }
      for(x = sx; x < sx+dx; x++) {
        cell_clear(x,sy       ,layer,graphics_change);
        cell_clear(x,sy+(dy-1),layer,graphics_change);
      }        
    }
  }

  void cell_move(tpos x, tpos y, tpos layer, 
                tpos nx, tpos ny, bool graphics_change)
  {    
    cells[nx][ny][layer] = cells[x][y][layer];
       
    if(graphics_change) {
      cell_graphics_reset(nx,ny,layer);
    }
  
    cell_clear(x, y, layer, graphics_change);
  }

  /***********************************************************************
     player interface
    **********************************************************************
  */

  void player_set_item(int num, item_handle item)
  {
    players[num].set_item(item);
  }

  item_handle player_get_item(int num)
  {
    return(players[num].get_item());
  }

  void player_set_pos(int num, tpos x, tpos y)
  {
    players[num].set_pos(x,y);
  }
  
  void player_get_pos(int num, tpos *p_x, tpos *p_y)
  {
    players[num].get_pos(p_x,p_y);
  }
  
  void player_set_rot(int num, int rot)
  {
    players[num].set_rot(rot);
  }
  
  int player_get_rot(int num)
  {
    return(players[num].get_rot());
  }

  void player_set_active(int num)
  {
    players[num].flag |= PLAYER_ACTIVE;
  }

} LEVEL_GENERIC;


/**************************************************************** 
  Game level - main structure
 ****************************************************************
*/
typedef class level_status {

  bool   user;        /* user level */
  bool   success;     /* have bugs reached exit with all keys? */
  int    keys;        /* colected keys */
  int    steps;       /* all players steps */
  time_t tm_start;    /* time of play (in sec) */
  time_t tm_elapsed;  /* time of play (in sec) */

public:
  
  void game_start(void)
  {
    success = FALSE;  
    keys = 0;
    steps = 0;
    tm_start = time(NULL);    
  }

  void game_stop(void)
  {
    tm_elapsed = time(NULL) - tm_start;
  }

  char * time_get(char *p_string, int num)
  {
    time_t hr = tm_elapsed / 3600;    
    time_t min = (tm_elapsed % 3600) / 60;
    time_t sec = tm_elapsed - hr*3600 - min*60;
  
    snprintf(p_string,num,"%.2d:%.2d:%.2d",(int)hr,(int)min,(int)sec);
    return(p_string);
  }

  void steps_add(void)
  {
    steps++;
  }

  int  steps_get(void)
  {
    return(steps);
  }
  
  void keys_add(void)
  {
    keys++;
  }

  int  keys_get(void)
  {
    return(keys);
  }
  
  void bug_in_exit(void)
  {
    success = TRUE;
  }

  bool resolved(void)
  {
    return(success);
  }

  void user_set(bool user_)
  {
    user = user_;
  }

  bool user_get(void)
  {
    return(user);
  }

} LEVEL_STATUS;

/***********************************************************************
   Derived level interface for game
  **********************************************************************
*/
typedef class level_game : public level_generic {
  
  LEVEL_STATUS    *p_status;

  // game level state
  #define MAX_GAME_KEYS  5
  int              global_keys;
  
public:
  
  level_game(SCREEN *p_scrn, ITEM_REPOSITORY *p_rep, LEVEL_STATUS *p_status_);
  ~level_game(void);
  
  /***********************************************************************
    top & low panel interface
    **********************************************************************
  */  
  void top_panel_player_draw(int num, bool redraw);
  void top_panel_keys_draw(bool redraw);
  void top_panel_draw(bool redraw);
  void low_panel_draw_matt(bool redraw);
  void low_panel_draw_steps(bool redraw);

  bool low_panel_steps_changed(void)
  {
    static int last_steps = 0;
    int act_steps = p_status->steps_get();

    if(last_steps != act_steps) {
      last_steps = act_steps;
      return(TRUE);
    } else {
      return(FALSE);
    }
  }

  /***********************************************************************
    draw interface
    **********************************************************************
  */
  bool level_populate(void);

  void level_redraw(void)
  {
    top_panel_draw(TRUE);
    low_panel_draw_matt(TRUE);
    low_panel_draw_steps(TRUE);
  
    p_scr->set_redraw();
  }
  
  void level_draw(void)
  {
    p_scr->draw();
  
    if(low_panel_steps_changed())
      low_panel_draw_steps(TRUE);
    
    p_scr->flip();
  }

  /***********************************************************************
     global keys
    **********************************************************************
  */

  bool state_keys_enough(void)
  {
   assert(global_keys <= MAX_GAME_KEYS);
   return(global_keys == MAX_GAME_KEYS);
  }
  
  bool state_add_key(void)
  {
    if(global_keys < MAX_GAME_KEYS) {
      global_keys++;
      top_panel_keys_draw(TRUE);
      return(TRUE);
    } else {
      return(FALSE);
    }
  }

  int player_get(void)
  {
    return(player_active);
  }
  
  // key mattlock
  bool player_mattock_add(int num)
  {
    bool ret;
  
    if((ret = players[num].mattock_add())) {
      top_panel_player_draw(num,TRUE);
      low_panel_draw_matt(TRUE);
    }
  
    return(ret);
  }
  
  bool player_mattock_drop(int num)
  {
    bool ret;
  
    if((ret = players[num].mattock_drop())) {
      top_panel_player_draw(num,TRUE);
      low_panel_draw_matt(TRUE);
    }
  
    return(ret);
  }
  
  // key managment
  bool player_key_add(int num)
  {
    bool ret;
  
    if((ret = players[num].key_add())) {
      top_panel_player_draw(num,TRUE);
    }
    
    return(ret);
  }
  
  bool player_key_drop(int num)
  {
    bool ret;
  
    if((ret = players[num].key_drop())) {
      top_panel_player_draw(num,TRUE);
    }
  
    return(ret);
  } 
  
  void player_mark(int num, bool active)
  {    
    if(active)
      sprite_add(PLAYER_CURSOR, PLAYER_MARK, players[num].get_pos_x(), players[num].get_pos_y());
    else
      sprite_mask(players[num].get_pos_x(), players[num].get_pos_y());
  } 

  bool player_switch(int num = SELECT_PLAYER_FIRST);
 
} LEVEL_GAME;


/***********************************************************************
   Derived level interface for editor
  **********************************************************************
*/
#define CORE_ACTIVE       0x1

typedef class level_core {

public:

  int              flag;
  
  char	           background;               // -> num of background
  char	           music;                    // -> num of music

  LEVEL_CELL       cells[LEVEL_CELLS_X][LEVEL_CELLS_Y][LEVEL_LAYERS];
  
  PLAYER           players[MAX_PLAYERS];

public:

  void  used_set(void)
  {
    flag |= CORE_ACTIVE;
  }
  void  used_clear(void)
  {
    flag &= ~CORE_ACTIVE;
  }
  bool  used_get(void)
  {
    return(flag&CORE_ACTIVE);
  }

  bool operator==(class level_core &c)
  {  
    return(memcmp(this,&c,sizeof(*this)) == 0);
  }
  bool operator!=(class level_core &c)
  {  
    return(memcmp(this,&c,sizeof(*this)) != 0);
  }

} LEVEL_CORE;

typedef class level_editor : public level_generic {

public:

  void selection_rectangle_set(bool rectangle)
  {
    SCREEN_EDITOR *p_tmp = static_cast<SCREEN_EDITOR *>(p_scr);
    p_tmp->selection_rectangle_set(rectangle);
  }
  bool selection_rectangle_get(void)
  {
    SCREEN_EDITOR *p_tmp = static_cast<SCREEN_EDITOR *>(p_scr);
    return(p_tmp->selection_rectangle_get());
  }

  void selection_filled_set(bool filled)
  {
    SCREEN_EDITOR *p_tmp = static_cast<SCREEN_EDITOR *>(p_scr);
    return(p_tmp->selection_filled_set(filled));  
  }
  bool selection_filled_get(void)
  {
    SCREEN_EDITOR *p_tmp = static_cast<SCREEN_EDITOR *>(p_scr);
    return(p_tmp->selection_filled_get());
  }

  void selection_set(bool selection)
  {
    SCREEN_EDITOR *p_tmp = static_cast<SCREEN_EDITOR *>(p_scr);
    p_tmp->selection_set(selection);
  }
  bool selection_get(void)
  {
    SCREEN_EDITOR *p_tmp = static_cast<SCREEN_EDITOR *>(p_scr);
    return(p_tmp->selection_get());
  }

  void selection_set(tpos x, tpos y)
  {
    SCREEN_EDITOR *p_tmp = static_cast<SCREEN_EDITOR *>(p_scr);
    p_tmp->selection_set(x, y);
  }
  void selection_get(tpos *p_x, tpos *p_y)
  {
    SCREEN_EDITOR *p_tmp = static_cast<SCREEN_EDITOR *>(p_scr);
    p_tmp->selection_get(p_x, p_y);
  }

  void selection_rectangle_start_set(tpos x, tpos y)
  {
    SCREEN_EDITOR *p_tmp = static_cast<SCREEN_EDITOR *>(p_scr);
    p_tmp->selection_rectangle_start_set(x, y);
  }
  void selection_rectangle_get(tpos *p_sx, tpos *p_sy, tpos *p_dx, tpos *p_dy)
  {
    SCREEN_EDITOR *p_tmp = static_cast<SCREEN_EDITOR *>(p_scr);
    p_tmp->selection_rectangle_get(p_sx, p_sy, p_dx, p_dy);
  }

public:

  void set_layers(LAYER_CONFIG *p_lconfig)
  {
    ((SCREEN_EDITOR *)p_scr)->set_layers(p_lconfig);
  }

  void level_populate(void);

  void level_move(tpos dx, tpos dy);

  // -------------------------------------------------------------------
  // undo/restore interface
  // -------------------------------------------------------------------
private:
  
  circle_stack<LEVEL_CORE> core;      // undo/redo stack

  void import_from_core(LEVEL_CORE *p_core);
  void export_to_core(LEVEL_CORE *p_core);

public:
  // stores level and returns a handle to it
  void core_push(void);

  // pop core from stack
  bool core_pop(void);


private:
  
  // tmp core, for editing
  LEVEL_CORE  core_tmp;  

public:

  void tmp_core_store(void);
  bool tmp_core_restore(void);
  
private:

  int  back_max;

public:

  void back_max_set(int max_back_)
  {
    back_max = max_back_;
  }

  int  back_max_get(void)
  {
    return(back_max);
  }

public:
  
  bool level_draw(void);

public:
  
  level_editor(ITEM_REPOSITORY *p_rep)
  : level_generic(p_rep)
  {
    p_scr = new SCREEN_EDITOR(CELL_SIZE_X,CELL_SIZE_Y);
    level_new();
  }

  ~level_editor(void)
  {
    delete(p_scr);
  }

} LEVEL_EDITOR;


#endif // __LEVEL_GAME_H__
