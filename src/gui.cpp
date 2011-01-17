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
#include "gui.h"

gui_base::gui_base(void)
: last_x(0), last_y(0), last_dx(0), last_dy(0)
{

}

gui_base::~gui_base(void)
{

}

// -------------------------------------------------------
// Game UI - clean up management
// -------------------------------------------------------

void gui_base::menu_enter(GUI_BASE *p_class, GUI_BASE_FUNC p_func, int param_1, int param_2)
{
  // If there is a menu function, call it
  menu_leave();

  // Set the new menu function
  menu_current.set(p_class, p_func, param_1, param_2);
}

void gui_base::menu_leave(void)
{
  menu_current.run_and_clear(MENU_LEAVE);
}

// -------------------------------------------------------
// Game UI - "back" management
// -------------------------------------------------------

void gui_base::back_push(void)
{  
  menu_back_stack.push(menu_current);
}

void gui_base::back_pop(void)
{
  if(!(menu_back_stack.is_empty())) {
    MENU_FUCTION fnc = menu_back_stack.pop();
    fnc.run(MENU_RETURN);
  }
}

// -------------------------------------------------------
// Game UI - menu items
// -------------------------------------------------------

#define MENU_TEXT_DIFF_X 10
#define MENU_TEXT_DIFF_Y 5


void gui_base::menu_item_set_pos(tpos x, tpos y)
{
  last_x = x;
  last_y = y;
}

void gui_base::menu_item_set_diff(tpos dx, tpos dy)
{
  last_dx = dx;
  last_dy = dy;
}

void gui_base::menu_item_set_add(tpos dx, tpos dy)
{
  last_x += dx;
  last_y += dy;
}

void gui_base::menu_item_start(void)
{
  input.mevent_clear();
}

void gui_base::menu_item_draw(char *p_text, ALIGNMENT spr_align, bool save_back, LEVEL_EVENT click1, LEVEL_EVENT click2, LEVEL_EVENT click3)
{

  switch(spr_align)
  {
    case LEFT:
      {      
        p_grf->draw(MENU_SPRIT_ARROW_L, last_x, last_y);
        p_font->alignment_set(LEFT);
        p_font->select(FONT_DEFAULT);
        p_font->print(&r, last_x + 17 + MENU_TEXT_DIFF_X, last_y + MENU_TEXT_DIFF_Y, p_text);
      
        LEVEL_EVENT s_spr = LEVEL_EVENT(GI_SPRITE_DRAW, MENU_SPRIT_ARROW_LC, last_x, last_y);
        LEVEL_EVENT u_spr = LEVEL_EVENT(GI_SPRITE_DRAW, MENU_SPRIT_ARROW_L,  last_x, last_y);
        LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, ET(FONT_SELECTED), ET(last_x + 17 + MENU_TEXT_DIFF_X), ET(last_y + MENU_TEXT_DIFF_Y), ET(LEFT), ET(p_text));
        LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, ET(FONT_DEFAULT), ET(last_x + 17 + MENU_TEXT_DIFF_X), ET(last_y + MENU_TEXT_DIFF_Y), ET(LEFT), ET(p_text));

        RECT r_arrow = {last_x, last_y, 17, 34};
      
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_spr, s_text));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_spr, u_text));
        if(save_back)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(GI_MENU_BACK_PUSH), click1, click2));
        else
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));

        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_spr, s_text));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_spr, u_text));
        if(save_back)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(GI_MENU_BACK_PUSH), click1, click2));
        else
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));
        
        last_x += last_dx;
        last_y += last_dy;
      }
      break;
    
    case RIGHT:
      {
        p_grf->draw(MENU_SPRIT_ARROW_R, last_x, last_y);
        p_font->alignment_set(RIGHT);
        p_font->select(FONT_DEFAULT);
        p_font->print(&r, last_x - MENU_TEXT_DIFF_X, last_y + MENU_TEXT_DIFF_Y, p_text);
      
        LEVEL_EVENT s_spr = LEVEL_EVENT(GI_SPRITE_DRAW, MENU_SPRIT_ARROW_RC, last_x, last_y);
        LEVEL_EVENT u_spr = LEVEL_EVENT(GI_SPRITE_DRAW, MENU_SPRIT_ARROW_R,  last_x, last_y);
        LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, ET(FONT_SELECTED), ET(last_x - MENU_TEXT_DIFF_X), ET(last_y + MENU_TEXT_DIFF_Y), ET(RIGHT), ET(p_text));
        LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, ET(FONT_DEFAULT),  ET(last_x - MENU_TEXT_DIFF_X), ET(last_y + MENU_TEXT_DIFF_Y), ET(RIGHT), ET(p_text));
      
        RECT r_arrow = {last_x, last_y, 17, 34};
      
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_spr, s_text));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_spr, u_text));
        if(save_back)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(GI_MENU_BACK_PUSH), click1, click2));
        else
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));

        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_spr, s_text));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_spr, u_text));
        if(save_back)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(GI_MENU_BACK_PUSH), click1, click2));
        else
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));
      
        last_x += last_dx;
        last_y += last_dy;
      }          
      break;
    
    case CENTER:
      {
      
      
      
      }      
      break;
    
    case LEFT_NO_ARROW:
      {
        p_font->alignment_set(LEFT);
        p_font->select(FONT_DEFAULT);
        p_font->print(&r, last_x, last_y, p_text);
      
        LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, ET(FONT_SELECTED), ET(last_x), ET(last_y), ET(LEFT), ET(p_text));
        LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, ET(FONT_DEFAULT),  ET(last_x), ET(last_y), ET(LEFT), ET(p_text));
      
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_text));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_text));
        if(save_back)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(GI_MENU_BACK_PUSH), click1, click2));
        else
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));
        
        last_x += last_dx;
        last_y += last_dy;
      }
      break;
    
    case RIGHT_NO_ARROW:
      {
        p_font->alignment_set(RIGHT);
        p_font->select(FONT_DEFAULT);
        p_font->print(&r, last_x, last_y, p_text);        
      
        LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, ET(FONT_SELECTED), ET(last_x), ET(last_y), ET(RIGHT), ET(p_text));
        LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, ET(FONT_DEFAULT),  ET(last_x), ET(last_y), ET(RIGHT), ET(p_text));
        
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_text));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_text));
        if(save_back)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(GI_MENU_BACK_PUSH), click1, click2));
        else
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));
      
        last_x += last_dx;
        last_y += last_dy;
      }
      break;
    
    case CENTER_NO_ARROW:
      {
      
      
      
      }
      break;
  }
}

void gui_base::menu_item_draw(tpos x, tpos y, char *p_text, ALIGNMENT spr_align, bool save_back, LEVEL_EVENT click1, LEVEL_EVENT click2, LEVEL_EVENT click3)
{
  menu_item_set_pos(x,y);
  menu_item_draw(p_text,spr_align,save_back,click1,click2,click3);
}

void gui_base::menu_services(LEVEL_EVENT ev)
{
  switch(ev.action_get()) {
    case GI_SPRITE_DRAW:
      {
        // format: [GI_SPRITE_DRAW, sprite_handle, x, y]
        p_grf->draw((spr_handle)ev.param_int_get(PARAM_0),ev.param_int_get(PARAM_1),ev.param_int_get(PARAM_2));
        p_grf->redraw_add((spr_handle)ev.param_int_get(PARAM_0),ev.param_int_get(PARAM_1),ev.param_int_get(PARAM_2));
        p_grf->flip();
      }
      break;
    case GI_STRING_DRAW:
      {
        // format: [GI_STRING_DRAW, font, x, y, x_aling, string]
        RECT r;
        
        p_font->select(ev.param_int_get(PARAM_0));
        p_font->alignment_set((ALIGNMENT)ev.param_int_get(PARAM_3));
        p_font->print(&r,(tpos)ev.param_int_get(PARAM_1),(tpos)ev.param_int_get(PARAM_2),(char *)ev.param_point_get(PARAM_4));
      
        p_grf->redraw_add(&r);
        p_grf->flip();
      }
      break;
    case GI_KEY_DOWN:
      {
        if(menu_key_input.valid()) {
          (this->*menu_key_input.p_func)(MENU_KEY_INPUT, ev.param_int_get(PARAM_0), 0);
        }
      }
      break;
    default:
      break;
  }
}

void gui_base::menu_dummy(MENU_STATE state, int data, int data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&gui_base::menu_dummy, data, data1);
      break;
    
    case MENU_LEAVE:
      break;      
    
    default:
      break;
  }
}

void gui_base::window_set_title(char *p_text,...)
{
  static char title[200];
  va_list     arguments;

  va_start(arguments,p_text);
  vsnprintf(title,200,p_text,arguments);
  va_end(arguments);
  
  SDL_WM_SetCaption(title, NULL);
}
