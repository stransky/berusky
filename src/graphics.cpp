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

void screen::draw(void)
{ 
  if(!ch_dynamic.changed() && !ch_dynamic.changed())
    return;
    
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
}

void screen::flip(void)
{
  p_grf->flip();
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

void screen_editor::draw(void)
{
  if(!ch_static.changed() && !ch_dynamic.changed())
    return;
    
  tpos min_x = MIN(ch_static.get_min_x(), ch_dynamic.get_min_x()),
       min_y = MIN(ch_static.get_min_y(), ch_dynamic.get_min_y()),
       max_x = MAX(ch_static.get_max_x(), ch_dynamic.get_max_x()),
       max_y = MAX(ch_static.get_max_y(), ch_dynamic.get_max_y());

  tpos x, y, dx, dy;
  
  assert(min_x >= 0 && max_x < LEVEL_CELLS_X);
  assert(min_y >= 0 && max_y < LEVEL_CELLS_Y);
    
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
          
          ch_static.clear(x,y);
          ch_dynamic.clear(x,y);
        }
      }
    }
    p_grf->redraw_add(min_x*cell_x+start_x,min_y*cell_y+start_y,dx*cell_x,dy*cell_y);
  }

  ch_static.clear();
  ch_dynamic.clear();

  // Draw all extra-sprites
  sprite_draw_all();

  // Draw cursors
  selection_draw();
}
