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

#ifndef __GUI_H__
#define __GUI_H__

#include "stack.h"

typedef enum _MENU_STATE {

  MENU_ENTER,
  MENU_RETURN,
  MENU_TIMER,
  MENU_KEY_INPUT,
  MENU_LEAVE

} MENU_STATE;

class gui_base;

typedef class gui_base GUI_BASE;
typedef void (GUI_BASE::*GUI_BASE_FUNC)(MENU_STATE state, size_ptr data, size_ptr data1);

typedef class menu_fuction {

public:
  
  GUI_BASE      *p_class;
  GUI_BASE_FUNC  p_func;
  size_ptr       p1;
  size_ptr       p2;

public:
  
  menu_fuction(void) : p_class(0), p_func(0), p1(0), p2(0) {};
  menu_fuction(GUI_BASE *p_class_, GUI_BASE_FUNC p_func_, size_ptr param_1 = 0, size_ptr param_2 = 0) :
  p_class(p_class_), p_func(p_func_), p1(param_1), p2(param_2) {};

  void set(GUI_BASE *p_class_, GUI_BASE_FUNC p_func_, size_ptr param_1 = 0, size_ptr param_2 = 0)
  {
    p_class = p_class_;
    p_func = p_func_;
    p1 = param_1;
    p2 = param_2;
  }

  void clear(void)
  {
    p_class = 0;
    p_func  = 0;
    p2 = p1 = 0;
  }

  bool valid(void)
  {
    return(p_class != NULL && p_func != NULL);
  }

  void run(MENU_STATE state)
  {
    if(valid()) {
      (p_class->*p_func)(state,p1,p2);
    }
  }

  void run_and_clear(MENU_STATE state)
  {
    if(valid()) {
      (p_class->*p_func)(state,p1,p2);
      clear();
    }
  }

} MENU_FUCTION;

typedef struct checkbox_config {

  bool        state;
  spr_handle  sprites[2];  
  tpos        x,y;
  LEVEL_EVENT event[MEVENTS];
  int         event_num;

  spr_handle sprite_get(void)
  {
    return(sprites[state ? 1 : 0]);
  }

  void check_switch(void)
  {
    state = !state;
  }

  void draw(void)
  {
    p_grf->draw(sprite_get(), x, y);
  }

  void draw_all(void)
  {
    p_grf->draw(sprite_get(), x, y);
    p_grf->redraw_add(sprite_get(), x, y);
    p_grf->flip();  
  }

  void event_set(LEVEL_EVENT ev)
  {
    event[0] = ev;
    event_num = 1;
  }

  void event_set(LEVEL_EVENT ev, LEVEL_EVENT ev1)
  {
    event[0] = ev;
    event[1] = ev1;
    event_num = 2;
  }

  void event_set(LEVEL_EVENT ev, LEVEL_EVENT ev1, LEVEL_EVENT ev2)
  {
    event[0] = ev;
    event[1] = ev1;
    event[2] = ev2;
    event_num = 3;
  }

  checkbox_config(void)
  {
    memset(this,0,sizeof(*this));
    sprites[0] = MENU_CHECKBOX_UNCHECKED;
    sprites[1] = MENU_CHECKBOX_CHECKED;
  }

  checkbox_config(bool state_, tpos x_, tpos y_)
  {
    state = state_;
    sprites[0] = MENU_CHECKBOX_UNCHECKED;
    sprites[1] = MENU_CHECKBOX_CHECKED;  
    x = x_;
    y = y_;
    event_num = 0;
  }

  checkbox_config(bool state_, tpos x_, tpos y_, LEVEL_EVENT ev)
  {
    state = state_;
    sprites[0] = MENU_CHECKBOX_UNCHECKED;
    sprites[1] = MENU_CHECKBOX_CHECKED;
    x = x_;
    y = y_;
    event[0] = ev;
    event_num = 1;
  }

} CHECKBOX_CONFIG;

typedef struct highlight_event {
  
  bool               active;
  int                event_num;
  LEVEL_EVENT        event[MEVENTS];

  highlight_event(void)
  {
    active = FALSE;
  }

  highlight_event(LEVEL_EVENT ev)
  {
    active = TRUE;
    event[0] = ev;
    event_num = 1;
  }

  highlight_event(LEVEL_EVENT ev, LEVEL_EVENT ev1)
  {
    active = TRUE;
    event[0] = ev;
    event[1] = ev1;
    event_num = 2;
  }

  highlight_event(LEVEL_EVENT ev, LEVEL_EVENT ev1, LEVEL_EVENT ev2)
  {    
    active = TRUE;
    event[0] = ev;
    event[1] = ev1;
    event[2] = ev2;
    event_num = 3;
  }

  void clear(void)
  {
    active = FALSE;
  }

} HIGHLIGHT_EVENT;

typedef class gui_base {

public:
  
  INPUT         input;

public:

  /* the current menu function */
  MENU_FUCTION  menu_current;

  /* Key input function */
  MENU_FUCTION  menu_key_input;

  /* Function called every frame */
  MENU_FUCTION  menu_timer;

  /* Backtrace */
  stack<MENU_FUCTION> menu_back_stack;

private:
  
  /* Menu sprites */
  spr_handle menu_spr_active;
  spr_handle menu_spr_inactive;
  tpos       menu_text_diff_x;
  tpos       menu_text_diff_y;
  tpos       menu_spr_diff_dx;
  tpos       menu_spr_diff_dy;

  /* Last used coordinates */
  tpos last_x;
  tpos last_y;
  tpos last_dx;
  tpos last_dy;

  /* Last used rect */
  RECT r;

  /* Array of active checkboxes */
  #define CHECKBOX_NUM 10
  CHECKBOX_CONFIG checkbox[CHECKBOX_NUM];

  /* Events launched when another menu from the group is selected */
  #define HIGHLIGHT_GROUP_NUM   10
  HIGHLIGHT_EVENT highlight_group[HIGHLIGHT_GROUP_NUM];
  
  #define HIGHLIGHT_GROUP_NONE  0
  int         highlight_group_next;

public:

  gui_base(void);
  ~gui_base(void);

  // Game UI - clean up management
  void menu_enter(GUI_BASE *p_class, GUI_BASE_FUNC p_func, size_ptr param_1, size_ptr param_2);
  void menu_leave(void);
  
  // Game UI - "back" management
  void back_push(void);
  void back_pop(void);

  // Menu support functions
  void menu_services(LEVEL_EVENT_QUEUE *p_read_queue, LEVEL_EVENT_QUEUE *p_write_queue, LEVEL_EVENT ev);
  void menu_dummy(MENU_STATE state, size_ptr data = 0, size_ptr data1 = 0);

  void menu_item_set_pos(tpos x, tpos y);
  void menu_item_set_add(tpos dx, tpos dy);
  void menu_item_set_diff(tpos dx, tpos dy);

  void menu_item_start(void);

  // keep the menu high-lighted, unless another one with this group
  // is selected
  void menu_item_highlight(int group)
  {
    highlight_group_next = group;
  }

  void menu_item_draw_sprite_set(spr_handle active, spr_handle inactive, 
                                 int menu_text_diff_x, int menu_text_diff_y);
  void menu_item_draw_sprite(char *p_text, MENU_TYPE spr_align, bool save_back,                             
                             LEVEL_EVENT click1,
                             LEVEL_EVENT click2 = LEVEL_EVENT(EV_NONE),
                             LEVEL_EVENT click3 = LEVEL_EVENT(EV_NONE));
  void menu_item_draw_sprite(tpos x, tpos y, char *p_text, MENU_TYPE spr_align, bool save_back,
                             LEVEL_EVENT click1, 
                             LEVEL_EVENT click2 = LEVEL_EVENT(EV_NONE),
                             LEVEL_EVENT click3 = LEVEL_EVENT(EV_NONE));
  void menu_item_draw_text(char *p_text, MENU_TYPE spr_align, bool save_back,
                           LEVEL_EVENT click1,
                           LEVEL_EVENT click2 = LEVEL_EVENT(EV_NONE), 
                           LEVEL_EVENT click3 = LEVEL_EVENT(EV_NONE));

  void menu_item_draw(char *p_text,
                      MENU_TYPE spr_align, bool save_back, LEVEL_EVENT click1,
                      LEVEL_EVENT click2 = LEVEL_EVENT(EV_NONE), 
                      LEVEL_EVENT click3 = LEVEL_EVENT(EV_NONE));
  void menu_item_draw(tpos x, tpos y, char *p_text,
                      MENU_TYPE spr_align, bool save_back, LEVEL_EVENT click1,
                      LEVEL_EVENT click2 = LEVEL_EVENT(EV_NONE), 
                      LEVEL_EVENT click3 = LEVEL_EVENT(EV_NONE));

  void menu_item_draw_checkbox(char *p_text, MENU_TYPE spr_align,
                               bool checked, int checkbox_id,
                               LEVEL_EVENT click1,
                               LEVEL_EVENT click2 = LEVEL_EVENT(EV_NONE),
                               LEVEL_EVENT click3 = LEVEL_EVENT(EV_NONE));
                      
  RECT * menu_get_last_rect(RECT *p_rect)
  {    
    return(*p_rect = r, p_rect);
  }
     
  void window_set_title(char *p_text,...);
  
} GUI_BASE;

#endif //__GUI_H__
