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
  Graphics interface
*/

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#define NO_SPRITE                 (-1)
#define IS_ON_SCREEN(sx,sy,ex,ey) ((sx) >= 0 && (sy) >= 0 && \
                                   (ex) <= GAME_RESOLUTION_X && (ey) <= GAME_RESOLUTION_Y)

/*
  Screen class - is composed from cells
*/
typedef class screen_sprite {

  int         flag;        // sprite flags
  
  spr_handle  spr;         // sprite handle

public:

  tpos        x_diff;      // sprite correction
  tpos        y_diff;

public:

  screen_sprite(void)
  {
    memset(this,0,sizeof(*this));
  }  

  void clear(void)
  {    
    flag = SPRITE_CLEAR;
  }

  void set_spr(spr_handle spr_)
  {
    spr = spr_;
    flag &= ~SPRITE_CLEAR;
  }

  void set_diff(tpos x_diff_, tpos y_diff_)
  {
    x_diff = x_diff_;
    y_diff = y_diff_;
  }

  void add_diff(tpos x_diff_, tpos y_diff_)
  {
    x_diff += x_diff_;
    y_diff += y_diff_;
  }

  void set_flag(int flag_)
  {
    flag = flag_;
  }

  int get_flag(void)
  {
    return(flag);
  }

  spr_handle get(void)
  {
    return(spr);
  }

  void set_add(tpos x_diff_, tpos y_diff_)
  {
    x_diff += x_diff_;
    y_diff += y_diff_;
  }

  void draw(tpos x, tpos y, spr_handle dst = 0)
  {       
    assert(spr != NO_SPRITE);
    p_grf->draw(spr, x+x_diff, y+y_diff, dst);
  }

  bool match(int flag_ = 0)
  {
    return((flag&flag_) == flag_);
  }

} SCREEN_SPRITE;

// ***************************************************************
// Change routines
// ***************************************************************
typedef class change {

  int  changes[LEVEL_CELLS_X][LEVEL_CELLS_Y];
  int  min_x, max_x;
  int  min_y, max_y;

public:

  bool changed(void)
  {
    return(max_x != 0 || max_y != 0 || min_x != LEVEL_CELLS_X ||
           min_y != LEVEL_CELLS_Y);
  }

  void set(tpos x, tpos y, tpos dx = 0, tpos dy = 0)
  {
    if(!dx && !dy) {
      if(x < min_x) min_x = x;
      if(x > max_x) max_x = x;

      if(y < min_y) min_y = y;
      if(y > max_y) max_y = y;
        
      changes[x][y] = true;
    } else {
    
      if(x < min_x)      min_x = x;
      if(x+dx-1 > max_x) max_x = x+dx-1;

      if(y < min_y)      min_y = y;
      if(y+dy-1 > max_y) max_y = y+dy-1;
        
      int tx;
      for(tx = x; tx < x+dx; tx++)
        memset(changes[tx]+y,1,sizeof(changes[0][0])*dy);
    }
  }

  void set_all(void)
  {
    set(0,0,LEVEL_CELLS_X,LEVEL_CELLS_Y);
  }

  void clear(tpos x, tpos y)
  {
    changes[x][y] = false;
  }

  void clear(void)
  {
    min_x = LEVEL_CELLS_X;
    min_y = LEVEL_CELLS_Y;
    max_x = max_y = 0;
  }

  void clear_all(void)
  {
    memset(this,0,sizeof(*this));
    clear();
  }

  bool is_set(tpos x, tpos y)
  {
    return(changes[x][y]);
  }

  int get_min_x(void)
  {
    return(min_x);
  }

  int get_min_y(void)
  {
    return(min_y);
  }

  int get_max_x(void)
  {
    return(max_x);
  }

  int get_max_y(void)
  {
    return(max_y);
  }

  change(void)
  {
    clear();
  }

} CHANGE;

typedef struct EXTRA_SPRITE {

  spr_handle spr;
  spr_handle dst;
  tpos       x,y;

} EXTRA_SPRITE;

typedef class screen {

protected:

  // coordinates of 0,0 cell on screen
  tpos                screen_start_x;
  tpos                screen_start_y;

  // coordinates of 0,0 cell on screen - recent one
  tpos                start_x;
  tpos                start_y;

  // cell size
  tpos                cell_x;
  tpos                cell_y;

  SCREEN_SPRITE       grid[LEVEL_CELLS_X][LEVEL_CELLS_Y][LEVEL_LAYERS];

  CHANGE              ch_static;
  CHANGE              ch_dynamic;

  spr_handle          back_original; // handle to orignal background  
  spr_handle          back_static;   // handle to background with static items

  #define             MAX_EXTRA_SPRITES 10   // Extra sprites -> cursors & so on
  EXTRA_SPRITE        esprite[MAX_EXTRA_SPRITES];
  int                 esprite_num;

  bool                clear_screen;

public:

  screen(tpos cell_x_, tpos cell_y_)
  {
    memset(grid,0,sizeof(grid));
  
    back_original = 0;
    back_static = 0;

    cell_x = cell_x_;
    cell_y = cell_y_;
  
    esprite_num = 0;
    clear_screen = FALSE;
  }

  virtual ~screen(void)
  {
  
  }
  
  void window_offset_set(tpos start_x_, tpos start_y_)
  {
    screen_start_x = start_x = start_x_;
    screen_start_y = start_y = start_y_;
  }

  void window_offset_get(tpos &start_x_, tpos &start_y_)
  {
    start_x_ = screen_start_x;
    start_y_ = screen_start_y;
  }
  
  virtual bool coord_in_area(tpos x, tpos y)
  { 
    x -= screen_start_x;
    y -= screen_start_y;
  
    return(INSIDE(0, x, LEVEL_RESOLUTION_X) && INSIDE(0, y, LEVEL_RESOLUTION_Y));
  }      

  void coord_to_grid(tpos *p_x, tpos *p_y)
  {
    *p_x = (*p_x - screen_start_x) / cell_x;
    *p_y = (*p_y - screen_start_y) / cell_y;
  }

  void coord_to_screen(tpos *p_x, tpos *p_y, tpos *p_dx, tpos *p_dy)
  {
    *p_x = *p_x * cell_x + screen_start_x;
    *p_y = *p_y * cell_y + screen_start_y;
  
    *p_dx = cell_x;
    *p_dy = cell_y;
  }

  bool coord_in_level(tpos level_x, tpos level_y)
  { 
    return(INSIDE(0, level_x, LEVEL_CELLS_X) && INSIDE(0, level_y, LEVEL_CELLS_Y));
  }      

  void grid_to_screen(tpos *p_x, tpos *p_y)
  {
    *p_x += (screen_start_x / cell_x);
    *p_y += (screen_start_y / cell_y);
  }

protected:
  
  void window_offset_set(void)
  {
    start_x = screen_start_x;
    start_y = screen_start_y;
  }

  void window_offset_reset(void)
  {
    start_x = start_y = 0;
  }

public:

  // ***************************************************************
  // Background routines
  // ***************************************************************
  void back_free(void)
  {  
    if(back_original) {
      p_grf->sprite_delete(back_original);
      back_original = 0;
    }
    if(back_static) {
      p_grf->sprite_delete(back_static);  
      back_static = 0;
    }
  }

  bool back_load(int back_num)
  {
    char file[MAX_FILENAME];

    back_free();

    sprintf(file, BACKGROUND_NAME, back_num + 1);
  
    back_original = FIRST_BACKGROUND; // handle to orignal background  
    back_static = FIRST_BACKGROUND + 1;// handle to background with static items

    bool loaded = p_grf->sprite_insert(file, back_original);
    if(loaded) {
      p_grf->sprite_copy(back_static, back_original, true);
      return(TRUE);
    }
    else {
      return(FALSE);
    }
  }

protected:

  void back_reset(tpos x, tpos y, tpos dx = 1, tpos dy = 1)
  {    
    p_grf->draw(back_original, x*cell_x, y*cell_y, dx*cell_x,
                dy*cell_y, back_static, x*cell_x, y*cell_y);
  }

  void back_draw(tpos x, tpos y, tpos dx = 1, tpos dy = 1, spr_handle dst = 0)
  {
    tpos sx = x*cell_x + start_x;
    tpos sy = y*cell_y + start_y;
  
    if(IS_ON_SCREEN(sx, sy, sx+dx*cell_x, sy+dy*cell_y)) {
      p_grf->draw(back_static, x*cell_x, y*cell_y,
                  dx*cell_x, dy*cell_y, dst, sx, sy);
    }
  }

  SCREEN_SPRITE * grid_get(tpos x, tpos y, tpos layer)
  {  
    return(grid[x][y]+layer);
  }

  void set_change(tpos x, tpos y, int flag)
  {
    if(flag&(SPRITE_STATIC|SPRITE_CLEAR))
      ch_static.set(x,y);
    if(flag&(SPRITE_DYNAMIC|SPRITE_CLEAR))
      ch_dynamic.set(x,y);    
  }

  void set_changes(tpos x, tpos y, tpos layer)
  {
    SCREEN_SPRITE *p_grid = grid_get(x, y, layer);
    tpos nx, ny;
    
    assert(abs(p_grid->x_diff) <= CELL_SIZE_X);
    assert(abs(p_grid->y_diff) <= CELL_SIZE_Y);
    
    if(p_grid->x_diff || p_grid->y_diff) {
      nx = x; ny = y;
      if(p_grid->x_diff)
        nx = (p_grid->x_diff > 0) ? x + 1 : x - 1;
      if(p_grid->y_diff)
        ny = (p_grid->y_diff > 0) ? y + 1 : y - 1;
          
      set_change(nx, ny, p_grid->get_flag());
    }
    
    set_change(x, y, p_grid->get_flag());
  }

public:

  void grid_set(tpos x, tpos y, tpos layer, spr_handle spr, int flag = 0)
  {
    SCREEN_SPRITE *p_grid = grid_get(x, y, layer);
  
    p_grid->set_spr(spr);
  
    if(flag)
      p_grid->set_flag(flag);
    else
      flag = p_grid->get_flag();
  
    set_changes(x, y, layer);
  }

  void grid_diff(tpos x, tpos y, tpos layer, tpos x_diff, tpos y_diff)
  {
    SCREEN_SPRITE *p_grid = grid_get(x, y, layer);
        
    p_grid->set_diff(x_diff, y_diff);
  
    set_changes(x, y, layer);
  }

  void grid_diff_add(tpos x, tpos y, tpos layer, tpos x_diff, tpos y_diff)
  {
    SCREEN_SPRITE *p_grid = grid_get(x, y, layer);
        
    p_grid->add_diff(x_diff, y_diff);
      
    set_changes(x, y, layer);
  }

  void grid_clear(tpos x, tpos y, tpos layer)
  {
    SCREEN_SPRITE *p_grid = grid_get(x, y, layer);
  
    set_changes(x, y, layer);
    
    p_grid->clear();
  }

  spr_handle grid_sprite_get(tpos x, tpos y, tpos layer)
  {
    return(grid[x][y][layer].get());
  }

protected:

  void grid_draw(tpos x, tpos y, int layer, int flag = 0, spr_handle dst = 0)
  {
    SCREEN_SPRITE *p_spr = grid[x][y]+layer;
    if(!p_spr->match(SPRITE_CLEAR) && p_spr->match(flag)) {
      tpos sx = x*cell_x + start_x;
      tpos sy = y*cell_y + start_y;
      if(IS_ON_SCREEN(sx, sy, sx+cell_x, sy+cell_y)) {
        p_spr->draw(sx, sy, dst);
      }
    }
  }

public:
  
  void sprite_add(int handle, spr_handle spr, tpos x, tpos y, spr_handle dst = 0)
  {  
    assert(handle < MAX_EXTRA_SPRITES);
    if(esprite_num <= handle)
      esprite_num = handle+1;
    
    esprite[handle].spr = spr;
    esprite[handle].dst = dst;
    esprite[handle].x = x;
    esprite[handle].y = y;
  }

  void sprite_mask(tpos x, tpos y)
  {      
    set_change(x,y,SPRITE_STATIC|SPRITE_DYNAMIC);
  }

  void sprite_draw(spr_handle spr, tpos x, tpos y, spr_handle dst = 0)
  {
    tpos sx = x*cell_x + start_x;
    tpos sy = y*cell_y + start_y;
    if(IS_ON_SCREEN(sx, sy, sx+cell_x, sy+cell_y)) {
      p_grf->draw(spr, sx, sy, dst);
      p_grf->redraw_add(spr, sx, sy);
    }
  }

protected:

  // Draw all extra-sprites
  void sprite_draw_all(void)
  {  
    if(esprite_num) {
      int i;
      for(i = 0; i < esprite_num; i++) {
        if(esprite[i].spr) {
          sprite_draw(esprite[i].spr, esprite[i].x, esprite[i].y, esprite[i].dst);
          esprite[i].spr = 0;
        }
      }
      esprite_num = 0;
    }
  }

public:

  void set_redraw(void)
  {
    ch_static.set_all();
    ch_dynamic.set_all();
    clear_screen = TRUE;
  }  

  virtual bool draw(void);
  void flip(void);

} SCREEN;

// ***************************************************************
// Screen class derived for editor
// ***************************************************************

typedef enum {

  LAYER_OFF = 0,
  LAYER_ON = 1,
  LAYER_CHANGE = 2

} LAYER_STATE;

/*
#define LAYER_FLOOR               0
#define LAYER_ITEMS               1
#define LAYER_PLAYER              2
#define ALL_LEVEL_LAYERS          4
*/

typedef struct layer_config {

  bool layer[ALL_LEVEL_LAYERS];
  int  selected;

  layer_config(void)
  {
    layer[LAYER_FLOOR] = TRUE;
    layer[LAYER_ITEMS] = TRUE;
    layer[LAYER_PLAYER] = TRUE;
    layer[LAYER_GRID] = FALSE;
  
    selected = ALL_LEVEL_LAYERS;
  }

  bool get(int lr)
  {
    assert(lr < ALL_LEVEL_LAYERS);
    return(layer[lr]);
  }

  void set(int lr, bool state)
  {
    assert(lr < ALL_LEVEL_LAYERS);
    layer[lr] = state;
  }

  void negate(int lr)
  {
    assert(lr < ALL_LEVEL_LAYERS);
    layer[lr] = !layer[lr];
  }

  int get_active(void)
  {  
    return(selected);
  }

  void set_active(int sel)
  {  
    selected = sel;
  }

} LAYER_CONFIG;

typedef struct screen_selection {

  // state of selection
  bool  selection;
  bool  rectangle;
  bool  filled;

  // start of rect selection
  tpos  rect_x;
  tpos  rect_y;

  // end of rect selection / cursor position
  tpos  x;
  tpos  y;  

  screen_selection(void)
  {
    memset(this,0,sizeof(*this));
  }

} SCREEN_SELECTION;

typedef class screen_editor : public screen {

  SCREEN_SELECTION   sel;

  // Active area of the level
  // It's in absolute coordinates (it does not depend on the level position)
  bool                selection_area_active;
  tpos                selection_min_x;
  tpos                selection_min_y;
  tpos                selection_max_x;
  tpos                selection_max_y;

private:
  
  void selection_draw(bool clear = FALSE);
  void inactive_area_draw(tpos x, tpos y);

public:

  virtual bool coord_in_area(tpos x, tpos y)
  { 
    if(selection_area_active &&
       (!INSIDE_ABS(selection_min_x*CELL_SIZE_X,x,selection_max_x*CELL_SIZE_X) ||
        !INSIDE_ABS(selection_min_y*CELL_SIZE_Y,y,selection_max_y*CELL_SIZE_Y)))
    {
      return(FALSE);
    }
    
    return(screen::coord_in_area(x, y));
  }

  void selection_set(bool selection)
  {
    if(sel.selection != selection) {
      sel.selection = selection;
      selection_draw();
    }
  }
  bool selection_get(void)
  {
    return(sel.selection);
  }

  void selection_rectangle_set(bool rectangle)
  {    
    if(sel.selection && sel.rectangle && !rectangle) {
      selection_draw(TRUE);
      sel.rectangle = rectangle;
      selection_draw();
    }
    else {
      sel.rectangle = rectangle;
    }
    
  }
  bool selection_rectangle_get(void)
  {
    return(sel.rectangle);
  }

  void selection_filled_set(bool filled)
  {
    if(sel.selection && sel.filled && !filled) {
      selection_draw(TRUE);
      sel.filled = filled;
      selection_draw();
    }
    else {
      sel.filled = filled;
    }  
  }
  bool selection_filled_get(void)
  {
    return(sel.filled);
  }

  void selection_set(tpos x, tpos y)
  {
    if(sel.x != x || sel.y != y) {
    
      if(sel.selection)
        selection_draw(TRUE);
    
      sel.x = x;
      sel.y = y;
    }
  }
  void selection_get(tpos *p_x, tpos *p_y)
  {
    if(p_x)
      *p_x = sel.x;
    if(p_y)
      *p_y = sel.y;
  }

  void selection_rectangle_start_set(tpos x, tpos y)
  {
    if(sel.rect_x != x || sel.rect_y != y) {
    
      if(sel.selection)
        selection_draw(TRUE);
    
      sel.rect_x = x;
      sel.rect_y = y;
    }
  }
  void selection_rectangle_get(tpos *p_sx, tpos *p_sy, tpos *p_dx, tpos *p_dy)
  {
    if(sel.rect_x < sel.x) {
      *p_sx = sel.rect_x;
      *p_dx = (sel.x - sel.rect_x)+1;
    }
    else {
      *p_sx = sel.x;
      *p_dx = (sel.rect_x - sel.x)+1;
    }
  
    if(sel.rect_y < sel.y) {
      *p_sy = sel.rect_y;
      *p_dy = (sel.y - sel.rect_y)+1;
    }
    else {
      *p_sy = sel.y;
      *p_dy = (sel.rect_y - sel.y)+1;
    }  
  }

private:

  LAYER_CONFIG       lconfig;

public:
  
  bool coord_in_level_shadowed(tpos level_x, tpos level_y);

  void set_layers(LAYER_CONFIG *p_lconfig);

  void back_draw_editor(tpos x, tpos y, bool grid);

  screen_editor(tpos cell_x_, tpos cell_y_);

  virtual ~screen_editor(void)
  {  
  }

  // returns true if level rendering was outside
  // of restricted area, so we need to redraw controls  
  virtual bool draw(void);

} SCREEN_EDITOR;

#endif //__GRAPHICS_H__
