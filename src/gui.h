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
typedef void (GUI_BASE::*GUI_BASE_FUNC)(MENU_STATE state, int data, int data1);

typedef class menu_fuction {

public:
  
  GUI_BASE      *p_class;
  GUI_BASE_FUNC  p_func;
  int            p1;
  int            p2;

public:
  
  menu_fuction(void) : p_class(0), p_func(0), p1(0), p2(0) {};
  menu_fuction(GUI_BASE *p_class_, GUI_BASE_FUNC p_func_, int param_1 = 0, int param_2 = 0) :  
  p_class(p_class_), p_func(p_func_), p1(param_1), p2(param_2) {};

  void set(GUI_BASE *p_class_, GUI_BASE_FUNC p_func_, int param_1 = 0, int param_2 = 0)
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
  
  /* Last used coordinates */
  tpos last_x;
  tpos last_y;
  tpos last_dx;
  tpos last_dy;

  /* Last used rect */
  RECT r;

public:

  gui_base(void);
  ~gui_base(void);

  // Game UI - clean up management
  void menu_enter(GUI_BASE *p_class, GUI_BASE_FUNC p_func, int param_1, int param_2);
  void menu_leave(void);
  
  // Game UI - "back" management
  void back_push(void);
  void back_pop(void);

  // Menu support functions
  void menu_services(LEVEL_EVENT ev);
  void menu_dummy(MENU_STATE state, int data = 0, int data1 = 0);

  void menu_item_set_pos(tpos x, tpos y);
  void menu_item_set_add(tpos dx, tpos dy);
  void menu_item_set_diff(tpos dx, tpos dy);

  void menu_item_start(void);
  void menu_item_draw(char *p_text, 
                      ALIGNMENT spr_align, bool save_back, LEVEL_EVENT click1,
                      LEVEL_EVENT click2 = LEVEL_EVENT(EV_NONE), 
                      LEVEL_EVENT click3 = LEVEL_EVENT(EV_NONE));
  void menu_item_draw(tpos x, tpos y, char *p_text,                      
                      ALIGNMENT spr_align, bool save_back, LEVEL_EVENT click1,
                      LEVEL_EVENT click2 = LEVEL_EVENT(EV_NONE), 
                      LEVEL_EVENT click3 = LEVEL_EVENT(EV_NONE));
                      
  RECT * menu_get_last_rect(RECT *p_rect)
  {    
    return(*p_rect = r, p_rect);
  }
     
  void window_set_title(char *p_text,...);
  
} GUI_BASE;

#endif //__GUI_H__
