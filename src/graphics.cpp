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
  
  - backround - all static items + floor + background image
*/

#include "berusky.h"

bool screen::draw(void)
{ 
  if(!ch_dynamic.changed() && !ch_dynamic.changed())
    return(FALSE);
    
  tpos min_x = MIN(ch_static.get_min_x(), ch_dynamic.get_min_x()),
       min_y = MIN(ch_static.get_min_y(), ch_dynamic.get_min_y()),
       max_x = MAX(ch_static.get_max_x(), ch_dynamic.get_max_x()),
       max_y = MAX(ch_static.get_max_y(), ch_dynamic.get_max_y());

  tpos x, y, layer;
  
  assert(min_x >= 0 && max_x < LEVEL_CELLS_X);
  assert(min_y >= 0 && max_y < LEVEL_CELLS_Y);
  
  // reset background && static items if they were affected
  if(ch_static.changed()) {
    for(layer = -1; layer <= LAYER_ITEMS; layer++) {
      for(y = ch_static.get_min_y(); y <= ch_static.get_max_y(); y++) {
        for(x = ch_static.get_min_x(); x <= ch_static.get_max_x(); x++) {
          if(ch_static.is_set(x,y)) {
          
            if(layer == -1) {
              back_reset(x, y);
            }
            else {
              window_offset_reset();
              grid_draw(x, y, layer, SPRITE_STATIC, back_static);
            }

            if(layer == LAYER_ITEMS) {
              window_offset_set();
              back_draw(x, y);
            }
          }
        }
      }
    }
  }

  window_offset_set();

  // draw dynamic items
  for(layer = LAYER_FLOOR; layer <= LAYER_PLAYER; layer++) {
    for(y = ch_dynamic.get_min_y(); y <= ch_dynamic.get_max_y(); y++) {
      for(x = ch_dynamic.get_min_x(); x <= ch_dynamic.get_max_x(); x++) {
        if(ch_dynamic.is_set(x,y)) {
          if(layer == LAYER_FLOOR && !ch_static.is_set(x,y))
            back_draw(x, y);
          grid_draw(x, y, layer, SPRITE_DYNAMIC);
        }
      }
    }
  }

  //-- udelat neco -> flag pro nepouzitou bunku? -> aby se to nemuselo kreslit
  for(x = min_x; x <= max_x; x++) {
    for(y = min_y; y <= max_y; y++) {
      if(ch_dynamic.is_set(x,y) || ch_static.is_set(x,y)) {
        p_grf->redraw_add(x*cell_x + start_x, y*cell_y + start_y, cell_x, cell_y);
        ch_static.clear(x,y);
        ch_dynamic.clear(x,y);
      }
    }
  }

  ch_static.clear();
  ch_dynamic.clear();

  // Draw all extra-sprites
  sprite_draw_all();

  return(FALSE);
}

void screen::flip(void)
{
  p_grf->flip();
}

screen_editor::screen_editor(tpos cell_x_, tpos cell_y_)
 : screen(cell_x_,cell_y_)
{
  if(DOUBLE_SIZE) {
    selection_area_active = TRUE;
    selection_min_x = 3;
    selection_min_y = 4;
    selection_max_x = 22;
    selection_max_y = 16;
  }
  else {
    selection_area_active = FALSE;
  }
}

bool screen_editor::coord_in_level_shadowed(tpos level_x, tpos level_y)
{ 
  if(!INSIDE(0, level_x, LEVEL_CELLS_X) || !INSIDE(0, level_y, LEVEL_CELLS_Y))
    return(FALSE);

  if(selection_area_active) {
    grid_to_screen(&level_x, &level_y);
    return(!(INSIDE_ABS(selection_min_x,level_x,selection_max_x) &&
             INSIDE_ABS(selection_min_y,level_y,selection_max_y)));
  }
  else {
    return(FALSE);
  }  
}      

void screen_editor::set_layers(LAYER_CONFIG *p_lconfig)
{
  lconfig = *p_lconfig;
}

// Draw all extra-sprites
void screen_editor::selection_draw(bool clear)
{   
  bool active = clear ? FALSE : sel.selection;

  if(!sel.rectangle) {
    if(!active) {
      sprite_mask(sel.x,sel.y);
    } else {
      sprite_draw(PLAYER_MARK, sel.x, sel.y);
    }
  }
  else {
    tpos x, y;
  
    tpos sx, sy; // start x/y
    tpos ex, ey; // end x/y
    
    //EDITOR_MARK
    spr_handle mark = EDITOR_MARK_RED;
  
    if(sel.rect_x < sel.x) {
      sx = sel.rect_x;
      ex = sel.x;
    }
    else {
      ex = sel.rect_x;
      sx = sel.x;
    }
  
    if(sel.rect_y < sel.y) {
      sy = sel.rect_y;
      ey = sel.y;
    }
    else {
      ey = sel.rect_y;
      sy = sel.y;
    }
 
    if(sel.filled) {
      if(!active) {
        // Top & Bottom lines
        for(x = sx; x <= ex; x++) {
          for(y = sy; y <= ey; y++) {
            sprite_mask(x, y);
          }
        }  
      } 
      else {
        // Top & Bottom lines
        for(x = sx; x <= ex; x++) {
          for(y = sy; y <= ey; y++) {
            sprite_draw(mark, x, y);
          }
        }
      }
    }
    else {
      if(!active) {
        // Top & Bottom lines
        for(x = sx; x <= ex; x++) {
          sprite_mask(x, sy);
          sprite_mask(x, ey);
        }  
        // left & right lines
        for(y = sy; y <= ey; y++) {
          sprite_mask(sx, y);
          sprite_mask(ex, y);
        }  
      } 
      else {
        // Top & Bottom lines
        for(x = sx; x <= ex; x++) {
          sprite_draw(mark, x, sy);
          sprite_draw(mark, x, ey);
        }
        // left & right lines
        for(y = sy; y <= ey; y++) {
          sprite_draw(mark, sx, y);
          sprite_draw(mark, ex, y);
        }        
      }  
    }
  }
}

void screen_editor::inactive_area_draw(tpos x, tpos y)
{
  tpos sx = x*cell_x + start_x;
  tpos sy = y*cell_y + start_y;
  if(IS_ON_SCREEN(sx, sy, sx+cell_x, sy+cell_y)) {
    p_grf->draw(SPRITE_BLACK, sx, sy);
  }
}

bool screen_editor::draw(void)
{
  if(!ch_static.changed() && !ch_dynamic.changed())
    return(FALSE);
    
  tpos min_x = MIN(ch_static.get_min_x(), ch_dynamic.get_min_x()),
       min_y = MIN(ch_static.get_min_y(), ch_dynamic.get_min_y()),
       max_x = MAX(ch_static.get_max_x(), ch_dynamic.get_max_x()),
       max_y = MAX(ch_static.get_max_y(), ch_dynamic.get_max_y());

  tpos x, y, dx, dy;
  
  assert(min_x >= 0 && max_x < LEVEL_CELLS_X);
  assert(min_y >= 0 && max_y < LEVEL_CELLS_Y);

  bool outside_change = FALSE;

  if(clear_screen) {
    p_grf->fill(0,0,EDITOR_RESOLUTION_X,EDITOR_RESOLUTION_Y,0);
    clear_screen = FALSE;
  }
  
  if(ch_static.changed() || ch_dynamic.changed()) {
  
    dx = (max_x - min_x)+1;
    dy = (max_y - min_y)+1;
  
    for(x = min_x; x <= max_x; x++) {
      for(y = min_y; y <= max_y; y++) {
        if(ch_dynamic.is_set(x,y) || ch_static.is_set(x,y)) {
          // Draw background
          back_draw_editor(x,y,lconfig.get(LAYER_GRID));
        
          // Draw floor
          if(lconfig.get(LAYER_FLOOR)) {
            grid_draw(x, y, LAYER_FLOOR);
          }
        
          // Draw items
          if(lconfig.get(LAYER_ITEMS)) {
            grid_draw(x, y, LAYER_ITEMS);
          }
        
          // Draw players
          if(lconfig.get(LAYER_PLAYER)) {
            grid_draw(x, y, LAYER_PLAYER);
          }
        
          if(coord_in_level_shadowed(x, y)) {
            inactive_area_draw(x, y);
            outside_change = TRUE;
          }
        
          ch_static.clear(x,y);
          ch_dynamic.clear(x,y);
        }
      }
    }
  
    RECT tmp = {min_x*cell_x+start_x,min_y*cell_y+start_y,dx*cell_x,dy*cell_y};
    p_grf->adjust_to_screen(&tmp);
    if(!outside_change)
      p_grf->redraw_add(&tmp);
  }

  ch_static.clear();
  ch_dynamic.clear();

  // Draw all extra-sprites
  sprite_draw_all();

  // Draw cursors
  selection_draw();

  return(outside_change);
}

void screen_editor::back_draw_editor(tpos x, tpos y, bool grid)
{ 
  tpos sx = x*cell_x + start_x;
  tpos sy = y*cell_y + start_y;

  if(IS_ON_SCREEN(sx, sy, sx+cell_x, sy+cell_y)) {
    if(grid) {
      p_grf->draw(EDIT_ZEME, sx, sy);
    } else {
      p_grf->draw(back_original, x*CELL_SIZE_X, y*CELL_SIZE_Y, 
                  CELL_SIZE_X, CELL_SIZE_Y, 0, sx, sy);
    }
  }
}
