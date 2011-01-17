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
#include <sys/wait.h>
#include <error.h>
#include <errno.h>

#include "berusky.h"
#include "berusky_gui.h"
#include "main.h"
#include "editor.h"

ITEM_REPOSITORY *item_panel::p_repo = NULL;

// draws item panel, writes records to item_array and 
// returns number of written items

/* Rect is
  start_x;
  start_y;

  dx = dx*item_num;
  dy = dy*item_num;
*/

bool item_panel::panel_scroll_check(int direction)
{
  int tmp_item    = item_first    + item_increment * direction;
  int tmp_variant = variant_first + variant_increment * direction;

  if(tmp_item < 0 || tmp_variant < 0)
    return(FALSE);

  int i;
  for(i = 0; i < item_num; i++) {
    int item =    tmp_item + item_increment * i;
    int variant = tmp_variant + variant_increment * i;
  
    if(!p_repo->item_valid(item) || !p_repo->variant_valid(item,variant))
      break;
  }
  
  return(i == item_num);
}

void item_panel::panel_scroll(int direction, EDITOR_SELECTION *p_sel, bool redraw)
{
  if(panel_scroll_check(direction)) {
    item_first    += item_increment * direction;
    variant_first += variant_increment * direction;
    if(item_selected != NO_SELECTION) {
      item_return(item_selected,p_sel,direction);
    }
    if(redraw)
      publish();
  }
}

int  item_panel::item_return(tpos x, tpos y, EDITOR_SELECTION *p_sel)
{
  tpos sx = start_x;
  tpos sy = start_y;

  /* Panel is horizontal/vertical? */
  if(dx) { 
    sx += EDIT_ARROW_DX;
  } else {
    sy += EDIT_ARROW_DY;
  }  

  int i;
  for(i = 0; i < item_num; i++) {
  
    tpos tx = sx + dx*i;
    tpos ty = sy + dy*i;
  
    int item =    item_first + item_increment*i;
    int variant = variant_first + variant_increment*i;
  
    if(!p_repo->item_valid(item) || !p_repo->variant_valid(item,variant))
      return(NO_SELECTION);
    
    if(INSIDE(tx, x, ITEM_SIZE_X) && INSIDE(ty, y, ITEM_SIZE_Y)) {
      if(p_sel) {
        p_sel->item = item;
        p_sel->variant = variant;      
      }
      return(i);
    }
  }
  
  return(NO_SELECTION);
}

int  item_panel::item_return(int selected_item, EDITOR_SELECTION *p_sel, int correction)
{
  int i;

  selected_item += correction;
  if(selected_item < 0)
    selected_item = 0;
  if(selected_item >= item_num)
    selected_item = item_num-1;

  for(i = 0; i < item_num; i++) {
  
    int item =    item_first + item_increment*i;
    int variant = variant_first + variant_increment*i;
  
    if(!p_repo->item_valid(item) || !p_repo->variant_valid(item,variant)) {
      return(i > 0 && i == selected_item ? i-1 : NO_SELECTION);
    }
    if(i == selected_item) {
      if(p_sel) {
        p_sel->item = item;
        p_sel->variant = variant;      
      }
      return(i);
    }
  }

  return(NO_SELECTION);
}

void item_panel::publish(void)
{
  RECT r;

  tpos sx = start_x;
  tpos sy = start_y;

  if(dx) {
    // Panel is horizontal
    sx += EDIT_ARROW_DX;
  
    r.x = sx;
    r.y = sy;
    r.w = dx*item_num;
    r.h = ITEM_SIZE_Y;  
  } else {
    // Panel is vertical
    sy += EDIT_ARROW_DY;
  
    r.x = sx;
    r.y = sy;
    r.w = ITEM_SIZE_X;
    r.h = dy*item_num;
  }

  p_grf->fill(&r,0);

  int i;
  for(i = 0; i < item_num; i++) {
  
    tpos x = sx + dx*i;
    tpos y = sy + dy*i;
  
    int item =    item_first + item_increment*i;
    int variant = variant_first + variant_increment*i;
  
    if(!p_repo->item_valid(item) || !p_repo->variant_valid(item,variant))
      break;

    #define COLOR_HIGHLIGHTED_R   0
    #define COLOR_HIGHLIGHTED_G   0
    #define COLOR_HIGHLIGHTED_B   200
    
    #define COLOR_SELECTED_R      175
    #define COLOR_SELECTED_G      0
    #define COLOR_SELECTED_B      0
    
    if(i == item_highlighted) {
      p_grf->fill(x,y,ITEM_SIZE_X,ITEM_SIZE_Y,p_grf->color_map(COLOR_HIGHLIGHTED_R,
                  COLOR_HIGHLIGHTED_G, COLOR_HIGHLIGHTED_B));
    }
    if(i == item_selected) {
      p_grf->fill(x,y,ITEM_SIZE_X,ITEM_SIZE_Y, p_grf->color_map(COLOR_SELECTED_R,
                  COLOR_SELECTED_G, COLOR_SELECTED_B));
    }
    
    p_repo->draw(x+CELL_SIZE_X,y+CELL_SIZE_X,item,variant,0);
  }

  p_grf->redraw_add(&r);
}

// draws controls for a panel
void item_panel::publish_controls(INPUT *p_input)
{
  RECT r;

  tpos sx = start_x;
  tpos sy = start_y;

  if(dx) {
    // Panel is horizontal
    sx += EDIT_ARROW_DX;
  
    r.x = sx;
    r.y = sy;
    r.w = dx*item_num;
    r.h = ITEM_SIZE_Y;
  } else {
    // Panel is vertical
    sy += EDIT_ARROW_DY;
  
    r.x = sx;
    r.y = sy;
    r.w = ITEM_SIZE_X;
    r.h = dy*item_num;
  }

  /* Scrolling (by mouse wheel) */
  p_input->mevent_add(MOUSE_EVENT(MOUSE_STATE(r,MASK_WHEEL_UP),
                      MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS,
                      LEVEL_EVENT(ED_LEVEL_IPANEL_SCROLL, panel_handle,-1)));
  p_input->mevent_add(MOUSE_EVENT(MOUSE_STATE(r,MASK_WHEEL_DOWN),
                      MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS,
                      LEVEL_EVENT(ED_LEVEL_IPANEL_SCROLL, panel_handle, 1)));

  /* Highlight of items in panel */
  p_input->mevent_add(MOUSE_EVENT(MOUSE_STATE(r),
                      MEVENT_MOUSE_EXTERNAL|MEVENT_MOUSE_IN, 
                      panel_handle));
  p_input->mevent_add(MOUSE_EVENT(MOUSE_STATE(r),
                      MEVENT_MOUSE_EXTERNAL|MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT,
                      panel_handle));
  p_input->mevent_add(MOUSE_EVENT(MOUSE_STATE(r,MASK_BUTTON_LEFT),
                      MEVENT_MOUSE_EXTERNAL|MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, 
                      panel_handle));

  if(dx) { // Panel is horizontal
    {
      p_grf->draw(EDIT_ARROW_LEFT,start_x,start_y+CELL_SIZE_Y);
      p_grf->redraw_add(EDIT_ARROW_LEFT,start_x,start_y+CELL_SIZE_Y);
      RECT t = {start_x, start_y+CELL_SIZE_Y, EDIT_ARROW_DX, EDIT_ARROW_DY};
      p_input->mevent_add(MOUSE_EVENT(MOUSE_STATE(t,MASK_BUTTON_LEFT),
                          MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(ED_LEVEL_IPANEL_SCROLL,
                          panel_handle,-1)));
    }
    {
      p_grf->draw(EDIT_ARROW_RIGHT,r.x+r.w,r.y+CELL_SIZE_Y);
      p_grf->redraw_add(EDIT_ARROW_RIGHT,r.x+r.w,r.y+CELL_SIZE_Y);
      RECT t = {r.x+r.w,r.y+CELL_SIZE_Y, EDIT_ARROW_DX, EDIT_ARROW_DY};
      p_input->mevent_add(MOUSE_EVENT(MOUSE_STATE(t,MASK_BUTTON_LEFT),
                          MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(ED_LEVEL_IPANEL_SCROLL,
                          panel_handle,1)));
    }
  } else { // Panel is vertical
    {
      p_grf->draw(EDIT_ARROW_UP,start_x+CELL_SIZE_X,start_y);
      p_grf->redraw_add(EDIT_ARROW_UP,start_x+CELL_SIZE_X,start_y);
      RECT t = {start_x+CELL_SIZE_X, start_y, EDIT_ARROW_DX, EDIT_ARROW_DY};
      p_input->mevent_add(MOUSE_EVENT(MOUSE_STATE(t,MASK_BUTTON_LEFT),
                          MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(ED_LEVEL_IPANEL_SCROLL,
                          panel_handle,-1)));
    }
    {
      p_grf->draw(EDIT_ARROW_DOWN,r.x+CELL_SIZE_X,r.y+r.h);
      p_grf->redraw_add(EDIT_ARROW_DOWN,r.x+CELL_SIZE_X,r.y+r.h);
      RECT t = {r.x+CELL_SIZE_X, r.y+r.h, EDIT_ARROW_DX, EDIT_ARROW_DY};
      p_input->mevent_add(MOUSE_EVENT(MOUSE_STATE(t,MASK_BUTTON_LEFT),
                          MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(ED_LEVEL_IPANEL_SCROLL,
                          panel_handle,1)));
    }
  }  
}

editor_layer_config::editor_layer_config(void)
{
  
}

editor_gui::editor_gui(ITEM_REPOSITORY *p_repo_, DIR_LIST *p_dir_):
  gui_base(), 
  p_dir(p_dir_),
  p_repo(p_repo_),
  level(p_repo_), 
  console(&input,CONSOLE_X,CONSOLE_Y,CONSOLE_DX,CONSOLE_LINES),
  sel(&level)
{
  input.mevent_clear();
  input.events_wait(TRUE);

  // Clear whole screen
  p_grf->clear();
  
  // TODO -- predelat vsechno draw -> na konfigy (reset)
  level_config();  
  panel_reset();
  selection_draw();
  layer_menu_draw();
  layer_status_draw();
  level_draw();
  layer_active_set(ALL_LEVEL_LAYERS);
  side_menu_create();
  side_menu_update();

  level.back_max_set(background_num(p_dir));
}

editor_gui::~editor_gui(void)
{

}

// Panel interface
void editor_gui::panel_item_select(int panel, tpos x, tpos y)
{
  panel -= PANEL_HANDLE_1;
  assert(panel >= 0 && panel < PANELS);

  int i;
  for(i = 0; i < PANELS; i++) {
    if(i == panel)
      ipanel[i].item_select(x,y,&sel);
    else
      ipanel[i].item_unselect();    
  }
  // Draw the current selection
  selection_draw();

  // Set active layer
  layer_active_set(p_repo->item_get_layer(sel.item));
}

void editor_gui::panel_item_highlight(int panel, tpos x, tpos y)
{
  panel -= PANEL_HANDLE_1;
  assert(panel >= 0 && panel < PANELS);
  ipanel[panel].item_highlight(x,y);
}

void editor_gui::panel_reset(void)
{
  #define ITEMS_X_START   0
  #define ITEMS_Y_START   0
  #define ITEMS_X_DIFF    0
  #define ITEMS_Y_DIFF    35
  #define ITEMS_DX        (EDITOR_RESOLUTION_X-ITEMS_X_START)
  #define ITEMS_DY        (EDITOR_RESOLUTION_Y-ITEMS_Y_START)

  //-- dodelat tu listu
  #define ITEMS_IN_PANEL  12

  item_panel::set_up(p_repo);

  ipanel[0].set_up(ITEMS_IN_PANEL, VERTICAL, 0, 0, PANEL_HANDLE_1,ipanel+1);
  ipanel[0].item_increment = 1;
  ipanel[1].set_up(ITEMS_IN_PANEL, VERTICAL, ITEM_SIZE_X, 0, PANEL_HANDLE_2);
  ipanel[1].variant_increment = 1;

  ipanel[2].set_up(ITEMS_IN_PANEL, HORIZONTAL, 2*ITEM_SIZE_X, 0, PANEL_HANDLE_3, ipanel+3);
  ipanel[2].item_increment = 1;
  ipanel[3].set_up(ITEMS_IN_PANEL, HORIZONTAL, 2*ITEM_SIZE_X, ITEM_SIZE_Y, PANEL_HANDLE_4);
  ipanel[3].variant_increment = 1;

  int i;
  for(i = 0; i < PANELS; i++) {
    ipanel[i].publish();
    ipanel[i].publish_controls(&input);
  }
}

void editor_gui::panel_scroll(int panel, int direction)
{
  panel -= PANEL_HANDLE_1;
  assert(panel >= 0 && panel < PANELS);
  ipanel[panel].panel_scroll(direction,&sel); 
  selection_draw();
}

/*
#define EDIT_ITEM_START_X        (EDIT_COORD_START_X)
#define EDIT_ITEM_START_Y        (EDIT_COORD_START_Y+EDIT_COORD_DY)
#define EDIT_ITEM_DX             (LEVEL_RESOLUTION_X)
#define EDIT_ITEM_DY             (20)
*/
/*
#define EDIT_ITEM_PICT_START_X   (EDIT_ITEM_START_X)
#define EDIT_ITEM_PICT_START_Y   (EDIT_ITEM_START_Y+EDIT_ITEM_DY+10)
*/
void editor_gui::selection_draw(void)
{   
  p_font->select(FONT_DEFAULT);
  p_font->alignment_set(LEFT);
  p_grf->fill(EDIT_ITEM_START_X,EDIT_ITEM_START_Y,EDIT_ITEM_DX,EDIT_ITEM_DY,0);
  p_font->print(NULL,EDIT_ITEM_START_X,EDIT_ITEM_START_Y,_("I:%d V:%d R:%d L:%d"),
                sel.item, sel.variant, sel.rotation, 
                p_repo->item_get_layer(sel.item));
  p_grf->redraw_add(EDIT_ITEM_START_X,EDIT_ITEM_START_Y,EDIT_ITEM_DX,EDIT_ITEM_DY);

  {
    #define COLOR_BACK_R      100
    #define COLOR_BACK_G      100
    #define COLOR_BACK_B      100
  
    RECT r = {EDIT_ITEM_PICT_START_X,
              EDIT_ITEM_PICT_START_Y,
              ITEM_SIZE_X,
              ITEM_SIZE_Y};
  
    p_grf->fill(&r,p_grf->color_map(COLOR_BACK_R, COLOR_BACK_G, COLOR_BACK_B));
    p_repo->draw(EDIT_ITEM_PICT_START_X+CELL_SIZE_X,EDIT_ITEM_PICT_START_Y+CELL_SIZE_Y,
                 sel.item, sel.variant, sel.rotation);
    p_grf->redraw_add(&r);
  }
  {
    RECT r = {EDIT_ITEM_PICT_START_X+ITEM_SIZE_X+10,
              EDIT_ITEM_PICT_START_Y,
              LEVEL_RESOLUTION_X,
              ITEM_SIZE_Y};
    RECT dr;
  
    p_grf->fill(&r,0);
    p_font->print(&dr,r.x,r.y,_("Item: %s\nVariation: %d\nRotation: %s"),
            p_repo->item_get_name(sel.item),
            sel.variant,
            p_repo->item_get_rotation(sel.rotation));
    r.h = dr.h;
    p_grf->redraw_add(&r);
  }  
}

/* Draw the status line
*/
void editor_gui::selection_cursor_draw_status(char *p_text,...)
{
  char      text[200];
  va_list   arguments;  

  va_start(arguments,p_text);
  vsnprintf(text,200,p_text,arguments);
  va_end(arguments);

  p_font->select(FONT_DEFAULT);
  p_font->alignment_set(LEFT);
  p_grf->fill(EDIT_COORD_START_X,EDIT_COORD_START_Y,EDIT_COORD_DX,EDIT_COORD_DY,0);
  p_font->print(NULL,EDIT_COORD_START_X,EDIT_COORD_START_Y,text);
  p_grf->redraw_add(EDIT_COORD_START_X,EDIT_COORD_START_Y,EDIT_COORD_DX,EDIT_COORD_DY); 
}

/* draw screen cursor */
void editor_gui::selection_cursor_update(void)
{
  tpos x, y;
  tpos sx,sy;
  tpos dx,dy;

  if(level.selection_get()) {
    level.selection_get(&x,&y);
    if(level.selection_rectangle_get()) {
      level.selection_rectangle_get(&sx,&sy,&dx,&dy);
      selection_cursor_draw_status(_("selection %d,%d - %d,%d"),sx,sy,dx,dy);
    }
    else {
      selection_cursor_draw_status(_("level cursor %d x %d"),x,y);
    }
  }
  else {
    selection_cursor_draw_status(_("No selection"));
  }
}

/* Side menu - general
*/

#define SIDE_MENU_X       (EDITOR_SCREEN_START_X+GAME_RESOLUTION_X+10)
#define SIDE_MENU_Y       (EDITOR_SCREEN_START_Y+100)
#define SIDE_MENU_DX      (EDITOR_RESOLUTION_X-SIDE_MENU_X)
#define SIDE_MENU_DY      (EDITOR_RESOLUTION_Y-SIDE_MENU_Y)
#define SIDE_MENU_X_DIFF   0
#define SIDE_MENU_Y_DIFF   35


static char *side_menu[] = 
{ 
  _("(1) floor"),
  _("(2) items"),
  _("(3) players"),
  _("(4) all")
};

void editor_gui::side_menu_create(void)
{
  p_font->print(NULL,SIDE_MENU_X, SIDE_MENU_Y, "Select layer:");

  menu_item_set_pos(SIDE_MENU_X, SIDE_MENU_Y+SIDE_MENU_Y_DIFF);
  menu_item_set_diff(SIDE_MENU_X_DIFF, SIDE_MENU_Y_DIFF);

  menu_item_draw(side_menu[0], LEFT_NO_ARROW, TRUE, LEVEL_EVENT(ED_LEVEL_SELECT_LAYER,LAYER_FLOOR));
  menu_item_draw(side_menu[1], LEFT_NO_ARROW, TRUE, LEVEL_EVENT(ED_LEVEL_SELECT_LAYER,LAYER_ITEMS));
  menu_item_draw(side_menu[2], LEFT_NO_ARROW, TRUE, LEVEL_EVENT(ED_LEVEL_SELECT_LAYER,LAYER_PLAYER));
  menu_item_draw(side_menu[3], LEFT_NO_ARROW, TRUE, LEVEL_EVENT(ED_LEVEL_SELECT_LAYER,ALL_LEVEL_LAYERS));

  p_grf->redraw_add(SIDE_MENU_X,SIDE_MENU_Y,SIDE_MENU_DX,SIDE_MENU_DY);
}

#define SIDE_STATUS_X     (EDITOR_SCREEN_START_X+GAME_RESOLUTION_X+10)
#define SIDE_STATUS_Y     (EDITOR_SCREEN_START_Y)
#define SIDE_STATUS_DX    (EDITOR_RESOLUTION_X-SIDE_STATUS_X)
#define SIDE_STATUS_DY    100

void editor_gui::side_menu_update(void)
{
  p_font->select(FONT_DEFAULT);
  p_font->alignment_set(LEFT);
  p_grf->fill(SIDE_STATUS_X,SIDE_STATUS_Y,SIDE_STATUS_DX,SIDE_STATUS_DY,0);

  char *p_act = NULL;

  /* Side menu - layers
  */
  int active_layer = layer_active_get();
  switch(active_layer) {
    case LAYER_FLOOR:
      p_act = _("floor");
      break;
    case LAYER_ITEMS:
      p_act = _("items");
      break;
    case LAYER_PLAYER:
      p_act = _("players");
      break;
    case ALL_LEVEL_LAYERS:
      p_act = _("all layers");
      break;
    default:
      break;
  }
  if(p_act)
    p_font->print(NULL,SIDE_STATUS_X,SIDE_STATUS_Y,_("active layer:\n\n%s"),p_act);
  p_grf->redraw_add(SIDE_STATUS_X,SIDE_STATUS_Y,SIDE_STATUS_DX,SIDE_STATUS_DY);
}

/*
#define EDITOR_LAYER_STATUS_X     (ITEM_SIZE_X*2)
#define EDITOR_LAYER_STATUS_Y     (ITEM_SIZE_Y*2)
#define EDITOR_LAYER_STATUS_DX    (LEVEL_RESOLUTION_Y)
#define EDITOR_LAYER_STATUS_DY    (30)
*/
void editor_gui::layer_menu_draw(void)
{
  RECT r = {EDITOR_LAYER_STATUS_X, 
            EDITOR_LAYER_STATUS_Y, 
            EDITOR_LAYER_STATUS_DX, 
            EDITOR_LAYER_STATUS_DY};  

  p_grf->fill(&r,0);

  static char *layer_names[ALL_LEVEL_LAYERS] = 
  {   
    _("(1)Grid:"),
    _("(2)Floor:"),
    _("(3)Items:"),
    _("(4)Players:")
  };

  static int layer_handle[] = 
  {
    LAYER_GRID,
    LAYER_FLOOR,
    LAYER_ITEMS,
    LAYER_PLAYER
  };

  #define LAYER_DIFF 174  

  menu_item_set_pos(r.x, r.y);
  menu_item_set_diff(LAYER_DIFF,0);

  int i;
  for(i = 0; i < ALL_LEVEL_LAYERS; i++) {
    menu_item_draw(layer_names[i], LEFT_NO_ARROW, FALSE, LEVEL_EVENT(ED_LEVEL_LAYER,layer_handle[i],LAYER_CHANGE));
    menu_get_last_rect(config.coord+i);
  }

  p_grf->redraw_add(&r); 
}

void editor_gui::layer_status_draw(void)
{
  p_font->select(FONT_DEFAULT);
  p_font->alignment_set(LEFT);
  
  static char * layers_status[2] = 
  {
    _("off"),
    _("on")
  };

  static int layer_handle[] = 
  {
    LAYER_GRID,
    LAYER_FLOOR,
    LAYER_ITEMS,
    LAYER_PLAYER
  };

  #define LAYER_STATUS_DIFF   LAYER_DIFF
  #define LAYER_STATUS_OFFSET 120

  RECT r;
  int  i;
  for(i = 0; i < ALL_LEVEL_LAYERS; i++) {
    p_grf->fill(config.coord[i].x+config.coord[i].w,
                config.coord[i].y,50,config.coord[i].h,0);
    p_font->print(&r,config.coord[i].x+config.coord[i].w,
                  config.coord[i].y, 
                  layers_status[config.lc.get(layer_handle[i]) ? 1 : 0]);
    r.w += 20;
    p_grf->redraw_add(&r);
  }  
}

void editor_gui::layer_status_switch(int layer, LAYER_STATE state)
{
  if(state == LAYER_CHANGE) {
    config.lc.negate(layer);
  } else {
    config.lc.set(layer,state);
  }
  level.set_layers(&config.lc);

  layer_status_draw();
  level_draw();
}

void editor_gui::layer_active_set(int layer)
{
  config.lc.set_active(layer);  
  side_menu_update();
}

int editor_gui::layer_active_get(void)
{
  return(config.lc.get_active());
}

int editor_gui::layer_active_get(tpos x, tpos y)
{
  int active_layer;

  if((active_layer = layer_active_get()) == ALL_LEVEL_LAYERS) {
    int i;
    for(i = LEVEL_LAYERS-1; i >= 0; i--) {
      if(level.cell_is_full(x,y,i)) {
        return(i);
      }
    }
    return(LAYER_NONE);
  }
  else {
    return(active_layer);  
  }
}

void editor_gui::level_config(void)
{
  // Set coordinates of level
  level.window_offset(EDITOR_SCREEN_START_X,EDITOR_SCREEN_START_Y);

  // in rect
  RECT r = {EDITOR_SCREEN_START_X,EDITOR_SCREEN_START_Y,LEVEL_RESOLUTION_X,LEVEL_RESOLUTION_Y};

  // Cursor in level
  input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r),
                   MEVENT_MOUSE_EXTERNAL|MEVENT_MOUSE_IN,
                   SCREEN_HANDLE_IN));
  input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r),
                   MEVENT_MOUSE_EXTERNAL|MEVENT_MOUSE_OUT,
                   SCREEN_HANDLE_OUT));

  // Set/Remove items in level
  input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r,MASK_BUTTON_LEFT|MASK_BUTTON_RIGHT),
                   MEVENT_MOUSE_EXTERNAL|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS,
                   SCREEN_HANDLE_CLICK));

  // Modifications in level - rotation
  input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r,MASK_WHEEL_UP|MASK_WHEEL_DOWN,K_R),
                   MEVENT_MOUSE_EXTERNAL|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS|MEVENT_KEY,
                   SCREEN_HANDLE_ROTATE));

  // Modifications in level - variants
  input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r,MASK_WHEEL_UP|MASK_WHEEL_DOWN,K_V),
                   MEVENT_MOUSE_EXTERNAL|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS|MEVENT_KEY,
                   SCREEN_HANDLE_VARIATE));                   
}

void editor_gui::level_draw(void)
{
  level.level_populate();

  p_grf->redraw_add(EDITOR_SCREEN_START_X,EDITOR_SCREEN_START_Y,
                    LEVEL_RESOLUTION_X,LEVEL_RESOLUTION_Y);  
}

void editor_gui::event_add(LEVEL_EVENT ev)
{
  queue.add(ev);
  queue.commit();
}

void editor_gui::level_new(bool force)
{
  if(!force) {
    input_start(&editor_gui::level_new_callback,NO_HANDLE,INPUT_BOOLEAN, _("erase all data?"));
  } else {
    undo_store();
    console.print("new level");
    level.level_new();
    strcpy(level_name,DEFAULT_LEVEL_NAME);
    level_edited_set();
    level_edited_clear();
  }
}

void editor_gui::level_new_callback(void)
{
  if(input_get_bool()) {
    level_new(TRUE);
  }
}

void editor_gui::level_load(char *p_file, bool force)
{
  static char file[MAX_FILENAME] = "";

  // Level is edited and not saved - ask for action
  if(level_edited_get() && !force) {
    strncpy(file,p_file,MAX_FILENAME);
    input_start(&editor_gui::level_load_callback, HANDLE_1, INPUT_BOOLEAN, _("level is modified. erase all data?"));
    return;
  }

  // restore saved filename, if it's saved or ask user for level-name
  if(!p_file) {
    p_file = file;
    if(!p_file[0]) {
      input_start(&editor_gui::level_load_callback, HANDLE_2, INPUT_STRING, _("Level to load:"));
      return;
    }
  }

  // Save undo
  undo_store();  

  // Load given level
  const char *p_paths[] = { p_dir->levels_user_get(), p_dir->cwd_get(), NULL };

  bool loaded = level.level_load(p_file,p_paths,sizeof(p_paths)/sizeof(p_paths[0]));
  if(loaded) {
    strncpy(level_name,p_file,MAX_FILENAME);
    level_edited_clear();
    level_caption_update();
  } else {
    console.print(_("Unable to load level %s"),p_file);
  }

  // Clear internal name
  file[0] = '\0';
}

void editor_gui::level_load_callback(void)
{  
  switch(input_get_callback_handle()) {
    case HANDLE_1:      
      if(input_get_bool())
        level_load(NULL, TRUE);
      break;
    case HANDLE_2:
      level_load(input_get_string(), TRUE);
      break;
    default:
      assert(0);
      break;
  }
}

void editor_gui::level_save_as(char *p_file, bool force)
{
  static char file[MAX_FILENAME] = "";
  static int  force_saved = BOOL_UNDEFINED;

  if(force == BOOL_UNDEFINED)
    force = force_saved;
  else
    force_saved = force;

  if(!p_file && file[0] == '\0') {
    input_start(&editor_gui::level_save_as_callback, HANDLE_1, INPUT_STRING, _("save to:"));
    return;
  } else if(file[0] != '\0') {
    p_file = file;  
  } else {
    strncpy(file, p_file, MAX_FILENAME);
  }

  if(!force && level.level_exists(p_file)) {
    input_start(&editor_gui::level_save_as_callback, HANDLE_2, INPUT_BOOLEAN, _("file %s exists! overwrite?:"));
    return;
  }  

  if(level.level_save(p_file)) {
    level_edited_clear();
  }
  else {
    console.print(_("Unable to save level %s"),p_file);
  }

  console.print(_("Saved as %s"),p_file);
  file[0] = '\0';
  force_saved = BOOL_UNDEFINED;
}

void editor_gui::level_save_as_callback(void)
{
  switch(input_get_callback_handle()) {
    case HANDLE_1:
      level_save_as(input_get_string(), BOOL_UNDEFINED);
      break;
    case HANDLE_2:
      if(input_get_bool())
        level_save_as(NULL, TRUE);
      break;    
    default:
      assert(0);
      break;
  }  
}

void editor_gui::level_save(bool force)
{
  level_save_as(level_name,force);
}

void editor_gui::level_cursor_set(tpos x, tpos y)
{
  bool state;
  

  if((state = level.coord_in_level(x,y))) {
    level.coord_to_cell(&x,&y);
    level.selection_set(x,y);  
  }

  level.selection_set(state);
  selection_cursor_update();
}

void editor_gui::level_item_insert(void)
{
  if(level.selection_get()) {
    undo_store();
  
    tpos layer = p_repo->item_get_layer(sel.item);
  
    bool rect = level.selection_rectangle_get();  
    if(rect) {
      bool fill = level.selection_filled_get();
      tpos sx,sy,dx,dy;
    
      level.selection_rectangle_get(&sx,&sy,&dx,&dy);
      level.cell_set(sx, sy, dx, dy, layer, sel.item, sel.variant, sel.rotation, TRUE, fill);
    
      level.selection_rectangle_set(FALSE);
      console.print(_("Inserted item %d at %dx%d - %dx%d layer %d"),sel.item,sx,sy,dx,dy,layer);
    }
    else {
      tpos x, y;
    
      level.selection_get(&x, &y);
      level.cell_set(x, y, layer, sel.item, sel.variant, sel.rotation, TRUE);
    
      console.print(_("Inserted item %d at %dx%d layer %d"),sel.item,x,y,layer);
    }
  
    level_edited_set();
  }
}

void editor_gui::level_item_insert_rectangle(bool filled)
{
  if(level.selection_get()) {
    tpos x,y;
    level.selection_get(&x,&y);
    level.selection_rectangle_set(TRUE);
    level.selection_filled_set(filled);
    level.selection_rectangle_start_set(x,y);
  }
}

void editor_gui::level_item_rotate(CHANGE_TYPE type, int rotation)
{
  tpos x, y;

  if(level.selection_get()) {

    level.selection_get(&x,&y);
    int layer = layer_active_get(x,y);

    if(layer != LAYER_NONE) {
      if(level.cell_can_rotate(x,y,layer)) {
        undo_store();
        level_edited_set();
      
        if(type == ITEM_MODIFY) {
          level.cell_modify_rotation(x,y,layer,rotation,TRUE);
          console.print(_("[Modify] Rotating item at %dx%d layer %d"),x,y,layer);
        } 
        else if(type == ITEM_SET) {
          level.cell_set_rotation(x,y,layer,rotation,TRUE);
          console.print(_("[Set] Rotating item at %dx%d layer %d"),x,y,layer);
        }
      } else {
        console.print(_("Can't rotate item at %dx%d layer %d"),x,y,layer);
      }
    }
  }
}

void editor_gui::level_item_variate(CHANGE_TYPE type, int variant)
{  
  tpos x, y;

  if(level.selection_get()) {
  
    level.selection_get(&x,&y);
    int layer = layer_active_get(x,y);
  
    if(layer != LAYER_NONE) {
      undo_store();
      level_edited_set();
      
      if(type == ITEM_MODIFY) {
        level.cell_modify_variation(x,y, layer, variant, TRUE, TRUE);
        console.print(_("[Modify] Variating item at %dx%d layer %d"),x,y,layer);
      } 
      else if(type == ITEM_SET) {
        level.cell_set_variation(x,y, layer, variant, TRUE, TRUE);
        console.print(_("[Set] Variating item at %dx%d layer %d"),x,y,layer);
      }
    }
  }
}

void editor_gui::level_item_clear(void)
{
  tpos x, y;

  if(level.selection_get()) {
    undo_store();
    level_edited_set();

    level.selection_get(&x,&y);
    int layer = layer_active_get(x,y);
  
    bool rect = level.selection_rectangle_get();
    if(rect) {
      bool fill = level.selection_filled_get();
      tpos sx,sy,dx,dy;
    
      level.selection_rectangle_get(&sx,&sy,&dx,&dy);
      level.cell_clear(sx, sy, dx, dy, layer, TRUE, fill);
    
      level.selection_rectangle_set(FALSE);
      console.print(_("Erased items from %dx%d - %dx%d layer %d"),sx,sy,dx,dy,layer);
    }
    else {
      if(layer != LAYER_NONE) {      
        level.cell_clear(x,y, layer, TRUE);      
        console.print(_("Erased item at %dx%d layer %d"),x,y,layer);
      }
    }
  }
}

void editor_gui::level_item_clear_rectangle(bool filled)
{
  if(level.selection_get()) {
    tpos x,y;
    level.selection_get(&x,&y);
    level.selection_rectangle_set(TRUE);
    level.selection_filled_set(filled);
    level.selection_rectangle_start_set(x,y);
  }
}

void editor_gui::selection_rotate(int direction)
{
  if(p_repo->item_can_rotate(sel.item)) {
    ROTATE_ITEM(sel.rotation, direction);
    selection_draw();
  } else {
    console.print(_("Can't rotate item %d"),sel.item);
  }
}

// format: [GI_MOUSE_EVENT, handle, x, y, button]
void editor_gui::mouse_handler(LEVEL_EVENT_QUEUE *p_queue, LEVEL_EVENT ev)
{
  int button, direction;

  switch(ev.param_int_get(PARAM_0)) {
    case SCREEN_HANDLE_IN:
      p_queue->add(LEVEL_EVENT(ED_LEVEL_SET_CURSOR,ev.param_get(PARAM_1),ev.param_get(PARAM_2),ET(TRUE)));
      p_queue->commit();  
      break;
    case SCREEN_HANDLE_OUT:
      p_queue->add(LEVEL_EVENT(ED_LEVEL_SET_CURSOR,-1,-1,TRUE));
      p_queue->commit();
      break;
    case SCREEN_HANDLE_CLICK:
      p_queue->add(LEVEL_EVENT(ED_LEVEL_SET_CURSOR,ev.param_get(PARAM_1),ev.param_get(PARAM_2)));
      button = ev.param_int_get(PARAM_4);
      if(button == BUTTON_LEFT)
        p_queue->add(LEVEL_EVENT(ED_LEVEL_DRAW_CURSOR_INSERT_ITEM));
      else if(button == BUTTON_MIDDLE)
        ;        
      else if(button == BUTTON_RIGHT)
        p_queue->add(LEVEL_EVENT(ED_LEVEL_DRAW_CURSOR_CLEAR_ITEM));        
      p_queue->commit();      
      break;
    case SCREEN_HANDLE_ROTATE:
      direction = (ev.param_int_get(PARAM_4) == WHEEL_UP) ? -1 : 1;  
      p_queue->add(LEVEL_EVENT(ED_LEVEL_SET_CURSOR,ev.param_get(PARAM_1),ev.param_get(PARAM_2)));
      p_queue->add(LEVEL_EVENT(ED_LEVEL_DRAW_CURSOR_ROTATE_ITEM, ITEM_MODIFY, direction));
      p_queue->commit();
      break;
    case SCREEN_HANDLE_VARIATE:
      direction = (ev.param_int_get(PARAM_4) == WHEEL_UP) ? -1 : 1;    
      p_queue->add(LEVEL_EVENT(ED_LEVEL_SET_CURSOR,ev.param_get(PARAM_1),ev.param_get(PARAM_2)));
      p_queue->add(LEVEL_EVENT(ED_LEVEL_DRAW_CURSOR_VARIATE_ITEM,ITEM_MODIFY,direction));
      p_queue->commit();
      break;
    case PANEL_HANDLE_1:
    case PANEL_HANDLE_2:
    case PANEL_HANDLE_3:
    case PANEL_HANDLE_4:
      if(ev.param_int_get(PARAM_3)) {
        panel_item_select(ev.param_int_get(PARAM_0),ev.param_int_get(PARAM_1),ev.param_int_get(PARAM_2));
      } else {
        panel_item_highlight(ev.param_int_get(PARAM_0),ev.param_int_get(PARAM_1),ev.param_int_get(PARAM_2));
      }
      break;
    default:
      break;
  }
}

bool editor_gui::editor_quit(bool force)
{
  if(level_edited_get() && !force) {
    input_start(&editor_gui::editor_quit_callback, NO_HANDLE, INPUT_BOOLEAN, _("level is not saved. exit anyway?"));
    return(FALSE);
  } else {
    return(TRUE);
  }
}

void editor_gui::editor_quit_callback(void)
{
  if(input_get_bool()) {
    queue.add(LEVEL_EVENT(ED_QUIT,TRUE));
  }
}

void editor_gui::editor_run_level(void)
{
  #define TMP_LEVEL "berusky-editor-level.lv3"

  console.print("Run level...");  

  char filename[MAX_FILENAME];
  return_path(p_dir->tmp_get(), TMP_LEVEL, filename, MAX_FILENAME);

  if(level.level_save(filename)) {
    bprintf("Saved as %s",filename);
    int pid = fork();
    if(!pid) {
      char level_name[MAX_FILENAME];
      return_path(p_dir->tmp_get(), TMP_LEVEL, level_name, MAX_FILENAME);
      bprintf("%s -u %s",p_dir->game_binary_get(),level_name);
      int ret = execlp(p_dir->game_binary_get(),p_dir->game_binary_get(),"-u",level_name,NULL);
      if(ret == -1) {
        bprintf("Error: %s",strerror(errno));
      }      
    } 
    else {
      int status;
      bprintf("Waiting for %d",pid);
      waitpid(pid,&status,0);
      bprintf("Pid %d done",pid);
    }
  }
}


#define FLOOR_TYPES 5

#define CHANGE_FLOOR(x,y,new_variant)                                         \
{                                                                             \
  int variant = level.cell_get_variation(x,y,LAYER_FLOOR);                    \
  int base = variant / FLOOR_TYPES;                                           \
  level.cell_set_variation(x, y, LAYER_FLOOR, base*FLOOR_TYPES + new_variant, TRUE, TRUE);\
}

#define IS_WALL(x,y)  (                                                           \
                       level.cell_get_item(x,y,LAYER_ITEMS) == P_WALL ||          \
                       level.cell_get_item(x,y,LAYER_ITEMS) == P_EXIT ||          \
                       (level.cell_get_item(x,y,LAYER_ITEMS) >= PP_LEFT_JAMB_O && \
                        level.cell_get_item(x,y,LAYER_ITEMS) <= PP_BOTTOM_JAMB_Z) \
                      )


void editor_gui::editor_shader(tpos sx, tpos sy, tpos dx, tpos dy)
{
  int x,y;

  undo_store();
  console.print(_("Shading..."));

  for(x = sx; x < sx+dx; x++) {
    for(y = sy; y < sy+dy; y++) {    
      if(IN_LEVEL(x-1,y)   && IS_WALL(x-1,y) &&         
         IN_LEVEL(x,  y-1) && IS_WALL(x,  y-1)) {
         // ####
         // ##@@         
         CHANGE_FLOOR(x,y,0);
      }
      else if(IN_LEVEL(x,  y-1) && IS_WALL(x,  y-1)) {
         //   ##        
         //   @@         
         CHANGE_FLOOR(x,y,3);
      }
      else if(IN_LEVEL(x-1, y ) && IS_WALL(x-1,y)) {
         //
         // ##@@         
         CHANGE_FLOOR(x,y,2);
      }
      else if(IN_LEVEL(x-1, y-1) && IS_WALL(x-1,y-1)) {
         // ##
         //   @@
         CHANGE_FLOOR(x,y,1);
      }
      else  {
         // 
         //   @@
         CHANGE_FLOOR(x,y,4);
      }
    }  
  }

  level_edited_set();
}

void editor_gui::editor_shader(void)
{
  editor_shader(0, 0, LEVEL_CELLS_X, LEVEL_CELLS_Y);
}

void editor_gui::level_change_backgroud(void)
{
  int max = level.back_max_get();
  int back = level.level_background_get()+1;

  if(back >= max)
    back = 0;
    
  level.level_background_set(back);
  
  console.print("background %d (from %d)",back, max);
}

void editor_gui::test_gui(void)
{
  bprintf("Test-gui!!(%s)",input_get_string());
}

void editor_gui::test(void)
{
  console.print("Test's gift");
}

bool editor_gui::event_handler(void)
{
  /* load events from system from SDL event handler */
  input.events_loop(&queue);

  /* Save all new events here */
  static LEVEL_EVENT_QUEUE tmp_queue;

  do {
    while(!queue.empty()) {
      LEVEL_EVENT ev = queue.get();
        
      switch(ev.action_get()) {

        case EV_TEST:
          test();
          break;
      
        case ED_LEVEL_NEW:
          level_new(ev.param_int_get(PARAM_0));
          break;
      
        case ED_LEVEL_LOAD:
          level_load((char *)ev.param_point_get(PARAM_0));
          break;
        case ED_LEVEL_SAVE:
          level_save();
          break;
        case ED_LEVEL_SAVE_AS:
          level_save_as((char *)ev.param_point_get(PARAM_0));
          break;
      
        case ED_LEVEL_SET_CURSOR:
          level_cursor_set(ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1));
          break;

        case ED_LEVEL_RECTANGLE_SELECTION:
          level_item_insert_rectangle(ev.param_int_get(PARAM_0));
          break;
        
        case ED_LEVEL_DRAW_CURSOR_INSERT_ITEM:
          level_item_insert();
          break;
        
        case ED_LEVEL_DRAW_CURSOR_ROTATE_ITEM:
          level_item_rotate((CHANGE_TYPE)ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1));
          break;
  
        case ED_LEVEL_DRAW_CURSOR_VARIATE_ITEM:
          level_item_variate((CHANGE_TYPE)ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1));
          break;
  
        case ED_LEVEL_DRAW_CURSOR_CLEAR_ITEM:
          level_item_clear();
          break;          

        case ED_LEVEL_IPANEL_SCROLL:
          panel_scroll(ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1));
          break;
  
        case ED_LEVEL_LAYER:        
          layer_status_switch(ev.param_int_get(PARAM_0),(LAYER_STATE)(ev.param_int_get(PARAM_1)));
          break;
        
        case ED_LEVEL_SELECT_LAYER:
          layer_active_set(ev.param_int_get(PARAM_0));
          break;
        
        case ED_LEVEL_CHANGE_BACKGROUND:
          level_change_backgroud();
          break;
        
        case ED_UNDO:
          undo_restore();
          break;
        case ED_REDO:
          break;        
        
        case ED_LEVEL_RUN:
          editor_run_level();
          break;

        case ED_LEVEL_SHADER:
          editor_shader();
          break;
        
        case ED_ROTATE_SELECTION:
          selection_rotate();
          break;
                
        case GC_MENU_QUIT:
        case ED_QUIT:
          if(editor_quit(ev.param_int_get(PARAM_0))) {
            return(FALSE);
          }
        
        case GI_MENU_BACK_POP:
          back_pop();
          break;
        case GI_MENU_BACK_PUSH:
          back_push();
          break;
        
        case GI_SPRITE_DRAW:
        case GI_STRING_DRAW:
        case GI_KEY_DOWN:
          menu_services(ev);
          break;
        
        case GI_MOUSE_EVENT:
          mouse_handler(&tmp_queue,ev);
          break;          
        
        default:
          assert(ev.valid());
          tmp_queue.add(ev);
          break;
      }
    }
  
    /* Commit all writes */
    queue.add(&tmp_queue);
    queue.commit();
  
    tmp_queue.clear();
  
  } while(!queue.empty());

  // Draw all changes    
  level.draw();
  level.flip();

  return(TRUE);
}

void editor_gui::keyset_set(EVENT_KEY_SET *p_keyset)
{
  input.keyset_set(p_keyset);
}

void editor_gui::level_caption_update(void)
{
  window_set_title(_("Level %s %s"),level_name, level_edited_get() ? _("(unsaved)") : _("(saved)"));
}

void editor_gui::level_edited_set(void)
{
  if(!level_edited) {
    level_edited = TRUE;
    level_caption_update();
  }
}

bool editor_gui::level_edited_get(void)
{
  return(level_edited);
}

void editor_gui::level_edited_clear(void)
{
  if(level_edited) {
    level_edited = FALSE;
    level_caption_update();
  }
}

// An Undo interface
void editor_gui::undo_store(void)
{
  level.core_push();
}

bool editor_gui::undo_restore(void)
{
  bool ret = level.core_pop();
  if(ret) {
    level_edited_set();
    console.print(_("undo"));
  } else {
    console.print(_("no more undo"));
  }
  return(ret);
}

/* Start input from console
*/
void editor_gui::input_start(EDITOR_CONSOLE_CALLBACK callback, int callback_id, INPUT_TYPE type,
                             char *p_text,...)
{
  console_callback    = callback;
  console_callback_id = callback_id;

  char *p_tmp      = console.input_title_get();
  int   max_lenght = console.input_title_get_max_lenght();

  va_list  arguments;
  va_start(arguments,p_text);
  vsnprintf(p_tmp,max_lenght,p_text,arguments);
  va_end(arguments);

  menu_key_input.set((GUI_BASE *)this,(GUI_BASE_FUNC)&editor_gui::console_input);
  console.input_start(type, NULL);
}

/* An editor interface to console
*/
void editor_gui::console_input(MENU_STATE state, int data, int data1)
{ 
  switch(data) {
    case K_ENTER:      
    case K_ESC:
      input_stop(data == K_ENTER);
      break;
    case K_BKSP:
    case K_DEL:
      console.input_clear_last();
      break;
    default:
      {
        char c;
        if(key_to_ascii(data,&c)) {
          console.input_add_char(c);
        }
      }
      break;
  }
}

/* Stop input from console
*/
void editor_gui::input_stop(bool success)
{
  menu_key_input.clear();
  console.input_stop();
  (this->*console_callback)();
}

// ----------------------------------------------------------------------
//  An editor console code
// ----------------------------------------------------------------------

const char * editor_console::boolean_yes = _("y");
const char * editor_console::boolean_no = _("n");

editor_console::editor_console(INPUT *p_input_, tpos sx, tpos sy, tpos dx, int lines_)
{
  istate = INPUT_NONE;
  
  p_input = p_input_;

  lines = lines_;

  height_diff = p_font->height_get();

  x = sx;
  y = sy;
  w = dx;
  h = lines * height_diff;

  ix_title = ix_input = sx;
  iy_title = iy_input = sy + height_diff * lines + CONSOLE_INPUT_LINE_SHIFT;

  output_clear();
  input_clear();
}

void editor_console::input_start(INPUT_TYPE type, char *p_text)
{
  input_clear(FALSE);

  istate = INPUT_READY;
  itype = type;
  if(p_text) {
    strncpy(p_text, input_line_title, CONSOLE_MAX_INPUT_LINE);
  }

  if(itype == INPUT_BOOLEAN) {
    sprintf(input_line_title+strlen(input_line_title)," (%s/%s) ",
            boolean_yes,boolean_no);
  } else {
    sprintf(input_line_title+strlen(input_line_title)," ");
  }

  p_input->block(TRUE);
  p_input->key_repeat(TRUE);

  input_redraw();
}

void editor_console::input_stop(bool redraw)
{  
  p_input->block(FALSE);
  p_input->key_repeat(FALSE);  

  istate = INPUT_NONE;
  input_redraw();
}

void editor_console::input_clear(bool redraw)
{
  istate = INPUT_NONE;

  input_boolean = CONSOLE_BOOLEAN_NO_INPUT;
  input_string_lenght = 0;
  input_string[0] = '\0';

  if(redraw)
    input_redraw();
}

void editor_console::input_clear_last(bool redraw)
{
  if(istate == INPUT_READY) {
    if(itype == INPUT_BOOLEAN) {
      input_boolean = CONSOLE_BOOLEAN_NO_INPUT;
    } else if(itype == INPUT_STRING) {
      input_string_lenght--;
      if(input_string_lenght < 0)
        input_string_lenght = 0;
      input_string[input_string_lenght] = '\0';
    }
    if(redraw)
      input_redraw();
  }
}

void editor_console::input_add_char(char c, bool redraw)
{
  if(istate == INPUT_READY) {
    switch(itype) {
      case INPUT_BOOLEAN:
        if(c == boolean_yes[0]) {
          input_boolean = TRUE;
        }
        else if(c == boolean_no[0]) {
          input_boolean = FALSE;
        }
        break;
      case INPUT_STRING:
        if(input_string_lenght+1 < CONSOLE_MAX_INPUT_LINE) {
          input_string[input_string_lenght] = c;
          input_string[input_string_lenght+1] = '\0';
          input_string_lenght++;
        }
        break;
      default:
        break;
    }
    if(redraw)
      input_redraw();
  }
}

void editor_console::input_redraw(void)
{ 
  if(istate == INPUT_READY) {  
    p_grf->fill(ix_title,iy_title,w,CONSOLE_INPUT_LINE_HEIGHT,COLOR_MAP(0,0,255));

    p_font->select(FONT_DEFAULT);
    p_font->alignment_set(LEFT);
  
    RECT r;
    p_font->print(&r,ix_title,iy_title,input_line_title);
  
    ix_input = ix_title + r.w;
    iy_input = iy_title;
      
    if(itype == INPUT_BOOLEAN) {
      const char *p_tmp;
    
      if(input_boolean == CONSOLE_BOOLEAN_NO_INPUT)
        p_tmp = "_";
      else if(input_boolean)
        p_tmp = boolean_yes;
      else
        p_tmp = boolean_no;
      
      p_font->print(NULL,ix_input,iy_input,p_tmp);
    } 
    else if(itype == INPUT_STRING) {
      p_font->print(NULL,ix_input,iy_input,"%s_",input_string);
    }  
  }
  else {
    p_grf->fill(ix_title,iy_title,w,CONSOLE_INPUT_LINE_HEIGHT,COLOR_MAP(0,0,0));
  }
  p_grf->redraw_add(ix_title,iy_title,w,CONSOLE_INPUT_LINE_HEIGHT);
}

void editor_console::print(const char *p_text,...)
{
  strncpy(output_lines[0],output_lines[1],CONSOLE_MAX_INPUT_LINE);
  
  va_list   arguments;
  va_start(arguments,p_text);
  vsnprintf(output_lines[1]+2,CONSOLE_MAX_INPUT_LINE-2,p_text,arguments);
  va_end(arguments);  
  output_lines[1][0] = '*';
  output_lines[1][1] = ' ';

  output_redraw();
}

void editor_console::output_clear(bool redraw)
{
  int i;
  for(i = 0; i < lines; i++)
    output_lines[i][0] = '\0';
  
  if(redraw)
    output_redraw();
}

void editor_console::output_redraw(void)
{
  p_font->select(FONT_DEFAULT);
  p_font->alignment_set(LEFT);

  p_grf->fill(x,y,w,h,0);
  int i;
  for(i = 0; i < lines; i++)
    p_font->print(NULL,x,y+i*height_diff,output_lines[i]);
  
  p_grf->redraw_add(x,y,w,h);
}
