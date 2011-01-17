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

/**************************************************************** 
  Player - structure for storing information about player
 ****************************************************************
*/
void player::set_graphics(int num)
{
  switch(num) {
    case 0:
      ls[0] = FIRST_OTHER + 48;
      ls[1] = FIRST_OTHER + 49;
      ls[2] = FIRST_OTHER + 52;
      ls[3] = FIRST_OTHER + 53;
      rs[0] = FIRST_OTHER + 50;
      rs[1] = FIRST_OTHER + 51;
      rs[2] = FIRST_OTHER + 54;
      rs[3] = FIRST_OTHER + 55;
      break;
    
    case 1:
      ls[0] = FIRST_OTHER + 16;
      ls[1] = FIRST_OTHER + 17;
      ls[2] = FIRST_OTHER + 44;
      ls[3] = FIRST_OTHER + 45;
      rs[0] = FIRST_OTHER + 24;
      rs[1] = FIRST_OTHER + 25;
      rs[2] = FIRST_OTHER + 46;
      rs[3] = FIRST_OTHER + 47;
      break;
      
    case 2:
      ls[0] = FIRST_OTHER + 22;
      ls[1] = FIRST_OTHER + 23;
      ls[2] = FIRST_OTHER + 20;
      ls[3] = FIRST_OTHER + 21;
      rs[0] = FIRST_OTHER + 18;
      rs[1] = FIRST_OTHER + 19;
      rs[2] = FIRST_OTHER + 26;
      rs[3] = FIRST_OTHER + 27;
      break;
      
    case 3:
      ls[0] = FIRST_OTHER + 36;
      ls[1] = FIRST_OTHER + 37;
      ls[2] = FIRST_OTHER + 40;
      ls[3] = FIRST_OTHER + 41;
      rs[0] = FIRST_OTHER + 38;
      rs[1] = FIRST_OTHER + 39;
      rs[2] = FIRST_OTHER + 42;
      rs[3] = FIRST_OTHER + 43;
      break;
      
    case 4:
      ls[0] = FIRST_OTHER + 28;
      ls[1] = FIRST_OTHER + 29;
      ls[2] = FIRST_OTHER + 32;
      ls[3] = FIRST_OTHER + 33;
      rs[0] = FIRST_OTHER + 30;
      rs[1] = FIRST_OTHER + 31;
      rs[2] = FIRST_OTHER + 34;
      rs[3] = FIRST_OTHER + 35;
      break;
  }
}

/***********************************************************************
   Generic level interface - for game & editor
  **********************************************************************
*/
level_generic::level_generic(ITEM_REPOSITORY *p_rep)
: p_repo(p_rep), p_scr(NULL)
{
  memset(cells,0,sizeof(cells));

  int i;
  for(i = 0; i < MAX_PLAYERS; i++)
    players[i].set_graphics(i);
}

level_generic::~level_generic(void)
{

}

bool level_generic::level_background_set(int back)
{
  background = back;
  bool loaded = p_scr->back_load(back);
  p_scr->set_redraw();
  return(loaded);
}

bool level_generic::level_new(void)
{
  level_clear();
  level_background_set(background);
  return (TRUE);
}

bool level_generic::level_load(const char * p_file)
{   
  LEVEL_DISK dl;
 
  if(!file_load(NULL,p_file,(char *)&dl,(t_off)sizeof(dl)))
    return(FALSE);

  if(strcmp(dl.signum, H_LEVEL))
    return(FALSE);  
  
  level_import(&dl);

  return(level_background_set(background));
}

bool level_generic::level_load(const char *p_file, const char **p_dirs, int dirs)
{
  bprintf(_("level_generic::level_load %s..."),p_file);

  int i;
  for(i = 0; i < dirs; i++) {
    char file[MAX_FILENAME];
  
    return_path(p_dirs[i], p_file, file, MAX_FILENAME);
  
    if(level_exists(file)) {
      bprintf(_("Checking %s...OK"),file);      
      return(level_load(file));
    }
    else {
      bprintf(_("Checking %s...not found"),file);      
    }
  }

  return(FALSE);
}

bool level_generic::level_save(const char * p_file)
{
  LEVEL_DISK dl;
  return(file_save(NULL, p_file, level_export(&dl), (t_off)sizeof(dl)));
}

bool level_generic::level_exists(const char *p_file)
{ 
  return(file_exists(NULL, p_file));
}

void level_generic::level_import(LEVEL_DISK *p_lev)
{
  item_handle item;
  int x,y, player;

  background = p_lev->back;
  music = p_lev->music;

  for(x = 0; x < LEVEL_CELLS_X; x++) {
     for(y = 0; y < LEVEL_CELLS_Y; y++) {
     
        // Import correction for floor
        item = p_lev->floor[y][x][0];
        if(item)
          item = NO_ITEM;
     
        if(p_repo->item_valid(item))
          cell_set(x, y, LAYER_FLOOR, item, p_lev->floor[y][x][1], p_lev->floor[y][x][2], true);
        else
          cell_clear(x, y, LAYER_FLOOR, true);

        // Import correction for items
        item = p_lev->level[y][x][0];
        if(!item)
          item = NO_ITEM;
      
        if(p_repo->item_valid(item))
          cell_set(x, y, LAYER_ITEMS, item, p_lev->level[y][x][1], p_lev->level[y][x][2], true);
        else
          cell_clear(x, y, LAYER_ITEMS, true);
        
        // Import correction for players
        player = p_lev->players[y][x];
        if(player == NO_PLAYER)
          item = NO_ITEM;
        else {          
          item = P_PLAYER_1 + player;
        }
        
        if(p_repo->item_valid(item)) {
          // add player to graphics grid
          cell_set(x, y, LAYER_PLAYER, item, 0, p_lev->rot[player], true);
        
          // add player to level
          player_set_item(player,item);
          player_set_pos(player,x,y);
          player_set_rot(player,p_lev->rot[player]);
          player_set_active(player);
        
          bprintf("Adding player %d at [%d, %d]",player,x,y);
        }
        else
          cell_clear(x, y, LAYER_PLAYER, true);
    }
  }
}

void level_generic::level_clear(void)
{  
  int x,y;

  background = 0;
  music = 0;

  for(x = 0; x < LEVEL_CELLS_X; x++) {
     for(y = 0; y < LEVEL_CELLS_Y; y++) {
        cell_clear(x, y, LAYER_FLOOR, true);
        cell_clear(x, y, LAYER_ITEMS, true);
        cell_clear(x, y, LAYER_PLAYER, true);
    }
  }  
}

LEVEL_DISK * level_generic::level_export(LEVEL_DISK *p_lev)
{
  int i,x,y;

  p_lev->back = background;
  p_lev->music = music;

  strcpy(p_lev->signum, H_LEVEL);

  for(i = 0; i < MAX_PLAYERS; i++) {
    p_lev->rot[i] = player_get_rot(i);
  }  

  for(x = 0; x < LEVEL_CELLS_X; x++) {
    for(y = 0; y < LEVEL_CELLS_Y; y++) {
    
      if(cell_is_full(x, y, LAYER_FLOOR)) {
        p_lev->floor[y][x][0] = P_GROUND;
        p_lev->floor[y][x][1] = cell_get_variation(x,y,LAYER_FLOOR);
        p_lev->floor[y][x][2] = cell_get_rotation(x,y,LAYER_FLOOR);
      } else {
        p_lev->floor[y][x][0] = NO_ITEM;
      }
    
      if(cell_is_full(x, y, LAYER_ITEMS)) {
        p_lev->level[y][x][0] = cell_get_item(x,y,LAYER_ITEMS);
        p_lev->level[y][x][1] = cell_get_variation(x,y,LAYER_ITEMS);
        p_lev->level[y][x][2] = cell_get_rotation(x,y,LAYER_ITEMS);
      } else {
        p_lev->level[y][x][0] = P_GROUND;
      }
    
      if(cell_is_full(x, y, LAYER_PLAYER)) {
        p_lev->players[y][x] = cell_get_item(x,y,LAYER_ITEMS) - P_PLAYER_1;        
      } else {
        p_lev->players[y][x] = NO_PLAYER;
      }
    
    }
  }

  return(p_lev);
}

/***********************************************************************
   Derived level interface for game
  **********************************************************************
*/
level_game::level_game(SCREEN *p_scrn, ITEM_REPOSITORY *p_rep, LEVEL_STATUS *p_status_)
: level_generic(p_rep)
{
  p_scr = p_scrn;

  global_keys = 0;
  player_active = 0;

  p_status = p_status_;  
}

level_game::~level_game(void)
{

}

bool level_game::level_populate(void)
{  
  int x,y,i;

  /* Select the first player in level */
  player_switch();

  /* Clear whole screen */
  p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
  p_grf->redraw_add(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y);

  /* Redraw all items */
  for(x = 0; x < LEVEL_CELLS_X; x++) {
      for(y = 0; y < LEVEL_CELLS_Y; y++) {
         for(i = 0; i < LEVEL_LAYERS; i++)
           cell_graphics_reset(x,y,i);
    }
  }

  /* Draw the top panel */
  top_panel_draw(FALSE);
  
  /* Draw low panel */
  low_panel_draw_matt(FALSE);
  low_panel_draw_steps(FALSE);

  /* Draw items to screen */
  level_redraw();

  return(true);
}

bool level_game::player_switch(int num)
{   
  int act = -1;
  int last_act = -1;

  bprintf("Switch to player %d",num);
  bprintf("Active %d",player_active);

  /* select first player in level */
  if(num == SELECT_PLAYER_FIRST) {
  
    for(act = 0; act < MAX_PLAYERS; act++) {
      if(players[act].flag&PLAYER_ACTIVE) {
        last_act = player_active;
        player_active = act;
        break;
      }
    }
  
    if(act >= MAX_PLAYERS)
      act = -1;
        
  /* select next player in level */
  } else if(num == SELECT_PLAYER_NEXT) {
  
    num = player_active;
  
    for(act = num+1; act != num; act++) {
      if(act >= MAX_PLAYERS)
        act = 0;
      if(players[act].flag&PLAYER_ACTIVE) {
        last_act = player_active;
        player_active = act;
        break;
      }    
    }
  
    if(act == num)
      act = -1;
  
  } else if(players[num].flag&PLAYER_ACTIVE) {
    last_act = player_active;
    act = player_active = num;  
  }

  if(act != -1) {
  
    top_panel_player_draw(last_act, TRUE);
    player_mark(last_act, FALSE);
  
    top_panel_player_draw(act,TRUE);
    player_mark(act,TRUE);
  
    low_panel_draw_matt(TRUE);
  
    return(TRUE);
  } else {
    return(FALSE);
  }
}

#define PANEL_X_OFFSET     (40 + (46 / 2))
#define PANEL_Y_OFFSET      2

#define PANEL_X_SIZE        29
#define PANEL_X_SIZE_2      (2*PANEL_X_SIZE+10)
#define PANEL_Y_SIZE        40

#define PANEL_DIFF          100

#define PLAYER_MASK         FIRST_OTHER+56


/*
  x_centre = player_num * PANEL_JUMP + PANEL_XOFFSET
  y_centre = PANEL_YOFFSET
*/

void level_game::top_panel_player_draw(int num, bool redraw)
{
  PLAYER *p_player = players+num;
  int     x, s1, s2;

  x = PANEL_X_OFFSET + (num * PANEL_DIFF);

  s1 = p_player->a_ls;
  s2 = p_player->a_rs;

  // Draw selected player
  if(num == player_active) {
    s1--;
    s2--;
  }

  tpos sx = x - PANEL_X_SIZE;
  tpos sy = 0;
  tpos dx = PANEL_X_SIZE_2;
  tpos dy = PANEL_Y_SIZE;

  p_grf->fill(sx, sy, dx, dy, 0);

  p_grf->draw(p_player->ls[s1], sx, 0);
  p_grf->draw(p_player->rs[s2], x, 0);

  if(!(p_player->flag&PLAYER_ACTIVE))
    p_grf->draw(PLAYER_MASK, x - 23, 2);
  
  if(redraw)
    p_grf->redraw_add(sx, sy, dx, dy);
}

void level_game::top_panel_keys_draw(bool redraw)
{
  p_grf->fill(X_KEYS_POSITION, Y_KEYS_POSITION, 60, 40, 0);

  if(global_keys > 0 && global_keys <= 5)
    p_grf->draw(FIRST_KEY + global_keys, X_KEYS_POSITION, Y_KEYS_POSITION);

  if(redraw)
    p_grf->redraw_add(X_KEYS_POSITION, Y_KEYS_POSITION, 60, 40);
}

void level_game::top_panel_draw(bool redraw)
{
  int i;

  /* Clear whole panel? */
  if(redraw) {
    p_grf->fill(SCREEN_TOP_PANNEL_X,SCREEN_TOP_PANNEL_Y,SCREEN_TOP_PANNEL_DX,SCREEN_TOP_PANNEL_DY,0);
    p_grf->redraw_add(SCREEN_TOP_PANNEL_X,SCREEN_TOP_PANNEL_Y,SCREEN_TOP_PANNEL_DX,SCREEN_TOP_PANNEL_DY);
  }

  for(i = 0; i < MAX_PLAYERS; i++)
    top_panel_player_draw(i,FALSE);

  top_panel_keys_draw(FALSE);
}

#define LOW_PANEL_MAT_X     0
#define LOW_PANEL_MAT_Y     460
#define LOW_PANEL_MAT_DX    (GAME_RESOLUTION_X / 2)
#define LOW_PANEL_MAT_DY    20

void level_game::low_panel_draw_matt(bool redraw)
{
  PLAYER *p_player = players + player_active;
  int     i;

  p_grf->fill(LOW_PANEL_MAT_X,LOW_PANEL_MAT_Y,LOW_PANEL_MAT_DX,LOW_PANEL_MAT_DY,0);

  for(i = 0; i < p_player->matt; i++) {
    p_grf->draw(SPRITE_MATOCK, LOW_PANEL_MAT_X+i*CELL_SIZE_X, LOW_PANEL_MAT_Y);
  }

  if(redraw)
    p_grf->redraw_add(LOW_PANEL_MAT_X,LOW_PANEL_MAT_Y,LOW_PANEL_MAT_DX,LOW_PANEL_MAT_DY);
}

#define LOW_PANEL_STP_X      (GAME_RESOLUTION_X / 2)
#define LOW_PANEL_STP_X_TEXT (GAME_RESOLUTION_X - 20)
#define LOW_PANEL_STP_Y       460
#define LOW_PANEL_STP_DX     (GAME_RESOLUTION_X / 2)
#define LOW_PANEL_STP_DY      20

void level_game::low_panel_draw_steps(bool redraw)
{
  p_grf->fill(LOW_PANEL_STP_X,LOW_PANEL_STP_Y,LOW_PANEL_STP_DX,LOW_PANEL_STP_DY,0);

  p_font->alignment_set(RIGHT);
  p_font->select(FONT_DEFAULT);

  p_font->print(NULL,LOW_PANEL_STP_X_TEXT,LOW_PANEL_STP_Y,"steps: %d",p_status->steps_get());

  if(redraw)
    p_grf->redraw_add(LOW_PANEL_STP_X,LOW_PANEL_STP_Y,LOW_PANEL_STP_DX,LOW_PANEL_STP_DY);
}

/***********************************************************************
   Derived level interface for editor
  **********************************************************************
*/
void level_editor::level_populate(void)
{  
  int x,y,l;  

  /* Redraw all items */
  for(l = 0; l < LEVEL_LAYERS; l++) {    
    for(x = 0; x < LEVEL_CELLS_X; x++) {
      for(y = 0; y < LEVEL_CELLS_Y; y++) {
        cell_graphics_reset(x,y,l);
      }
    }    
  }

  /* Draw items to screen */
  p_scr->set_redraw();
  p_scr->draw();
  p_scr->flip();
}

void level_editor::import_from_core(LEVEL_CORE *p_core)
{
  int x,y,l,i;
  
  if(background != p_core->background) {
    background = p_core->background;
    level_background_set(background);
  }

  music = p_core->music;

  for(l = 0; l < LEVEL_LAYERS; l++) {
    for(x = 0; x < LEVEL_CELLS_X; x++) {
      for(y = 0; y < LEVEL_CELLS_Y; y++) {
        cells[x][y][l] = p_core->cells[x][y][l];
        cell_graphics_reset(x,y,l);
      }
    }
  }

  for(i = 0; i < MAX_PLAYERS; i++)
    players[i] = p_core->players[i];
}

void level_editor::export_to_core(LEVEL_CORE *p_core)
{
  int x,y,l,i;

  p_core->background = background;
  p_core->music = music;

  for(l = 0; l < LEVEL_LAYERS; l++) {
    for(x = 0; x < LEVEL_CELLS_X; x++) {
      for(y = 0; y < LEVEL_CELLS_Y; y++) {
        p_core->cells[x][y][l] = cells[x][y][l];
      }
    }
  }

  for(i = 0; i < MAX_PLAYERS; i++)
    p_core->players[i] = players[i];
}

// stores level and returns a handle to it
void level_editor::core_push(void)
{ 
  LEVEL_CORE *p_tmp = core.top();
  LEVEL_CORE  act;

  export_to_core(&act);

  if((p_tmp && *p_tmp != act) || !p_tmp) {
    core.push(act);
  }
}

// pop core from stack
bool level_editor::core_pop(void)
{
  if(!core.is_empty()) {
    LEVEL_CORE *p_tmp = core.pop_pointer();
    assert(p_tmp);
    import_from_core(p_tmp);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

void level_editor::tmp_core_store(void)
{
  export_to_core(&core_tmp);
  core_tmp.used_set();
}

bool level_editor::tmp_core_restore(void)
{
  if(core_tmp.used_get()) {
    import_from_core(&core_tmp);
    return(TRUE);
  }
  else {
    return(FALSE);
  }
}
