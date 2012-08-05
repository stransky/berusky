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

void gui_base::menu_enter(GUI_BASE *p_class, GUI_BASE_FUNC p_func, size_ptr param_1, size_ptr param_2)
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
  // Clear all input events for menu
  input.mevent_clear();
}

void gui_base::menu_item_draw_sprite(char *p_text, MENU_TYPE spr_align, bool save_back,
                                     spr_handle active, spr_handle inactive,
                                     LEVEL_EVENT click1, LEVEL_EVENT click2, LEVEL_EVENT click3)
{
  switch(spr_align)
  {
    case MENU_LEFT_SPRITE:
      {
        p_grf->draw(MENU_SPRIT_ARROW_L, last_x, last_y);
        p_font->alignment_set(MENU_LEFT);
        p_font->select(FONT_DEFAULT);
        p_font->print(&r, last_x + 17 + MENU_TEXT_DIFF_X, last_y + MENU_TEXT_DIFF_Y, p_text);
      
        RECT r_arrow = {last_x, last_y, 17, 34};
      
        LEVEL_EVENT s_spr = LEVEL_EVENT(GI_SPRITE_DRAW, active, last_x, last_y);
        LEVEL_EVENT u_spr = LEVEL_EVENT(GI_SPRITE_DRAW, inactive,  last_x, last_y);
        LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, ET(FONT_SELECTED), 
                             ET_INT(last_x + 17 + MENU_TEXT_DIFF_X), 
                             ET_INT(last_y + MENU_TEXT_DIFF_Y), 
                             ET_INT(MENU_LEFT), ET_INT(p_text));
        LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, ET_INT(FONT_DEFAULT), 
                             ET_INT(last_x + 17 + MENU_TEXT_DIFF_X), 
                             ET_INT(last_y + MENU_TEXT_DIFF_Y), 
                             ET_INT(MENU_LEFT), ET_INT(p_text));
    
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
    case MENU_RIGHT_SPRITE:
      {
        p_grf->draw(MENU_SPRIT_ARROW_R, last_x, last_y);
        p_font->alignment_set(MENU_RIGHT);
        p_font->select(FONT_DEFAULT);
        p_font->print(&r, last_x - MENU_TEXT_DIFF_X, last_y + MENU_TEXT_DIFF_Y, p_text);
      
        LEVEL_EVENT s_spr = LEVEL_EVENT(GI_SPRITE_DRAW, active, last_x, last_y);
        LEVEL_EVENT u_spr = LEVEL_EVENT(GI_SPRITE_DRAW, inactive,  last_x, last_y);
        LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, 
                                        ET_INT(FONT_SELECTED), 
                                        ET_INT(last_x - MENU_TEXT_DIFF_X), 
                                        ET_INT(last_y + MENU_TEXT_DIFF_Y), 
                                        ET_INT(MENU_RIGHT), ET_INT(p_text));
        LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, 
                                        ET_INT(FONT_DEFAULT),  
                                        ET_INT(last_x - MENU_TEXT_DIFF_X), 
                                        ET_INT(last_y + MENU_TEXT_DIFF_Y), 
                                        ET_INT(MENU_RIGHT), ET_INT(p_text));
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
    case MENU_CENTER_SPRITE:
      {
      }
      break;
    default:
      break;
  }
}
    
void gui_base::menu_item_draw_text(char *p_text, MENU_TYPE spr_align, bool save_back,
                                   LEVEL_EVENT click1, LEVEL_EVENT click2, LEVEL_EVENT click3)
{
  switch(spr_align) {
    case MENU_LEFT_NO_ARROW:
      {
        p_font->alignment_set(MENU_LEFT);
        p_font->select(FONT_DEFAULT);
        p_font->print(&r, last_x, last_y, p_text);
      
        LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, ET_INT(FONT_SELECTED), 
                                        ET_INT(last_x), 
                                        ET_INT(last_y), 
                                        ET_INT(MENU_LEFT),
                                        ET_INT(p_text));
        LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, ET_INT(FONT_DEFAULT),  
                                        ET_INT(last_x), 
                                        ET_INT(last_y), 
                                        ET_INT(MENU_LEFT),
                                        ET_INT(p_text));
      
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
    
    case MENU_RIGHT_NO_ARROW:
      {
        p_font->alignment_set(MENU_RIGHT);
        p_font->select(FONT_DEFAULT);
        p_font->print(&r, last_x, last_y, p_text);        
      
        LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, ET_INT(FONT_SELECTED), 
                                        ET_INT(last_x), 
                                        ET_INT(last_y), 
                                        ET_INT(MENU_RIGHT), 
                                        ET_INT(p_text));
        LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, ET_INT(FONT_DEFAULT),  
                                        ET_INT(last_x), 
                                        ET_INT(last_y), 
                                        ET_INT(MENU_RIGHT), 
                                        ET_INT(p_text));
        
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
    default:
      break;
  }
}

void gui_base::menu_item_draw(char *p_text, MENU_TYPE spr_align, bool save_back, LEVEL_EVENT click1, LEVEL_EVENT click2, LEVEL_EVENT click3)
{
  switch(spr_align)
  {
    case MENU_LEFT:
      {      
        menu_item_draw_sprite(p_text, MENU_LEFT_SPRITE, save_back, 
                              MENU_SPRIT_ARROW_LC, MENU_SPRIT_ARROW_L, 
                              click1, click2, click3);
      }
      break;
    case MENU_RIGHT:
      {
        menu_item_draw_sprite(p_text, MENU_RIGHT_SPRITE, save_back,
                              MENU_SPRIT_ARROW_RC, MENU_SPRIT_ARROW_R,
                              click1, click2, click3);
      }          
      break;
    
    case MENU_CENTER:
      {
        // not implemented
        assert(0);
      }      
      break;
    
    case MENU_LEFT_NO_ARROW:
    case MENU_RIGHT_NO_ARROW:
      {
        menu_item_draw_text(p_text, spr_align, save_back, click1, click2, click3);
      }
      break;
    case MENU_CENTER_NO_ARROW:
      {
        // not implemented
        assert(0);
      }
      break;
    default:
      break;
  }
}

void gui_base::menu_item_draw(tpos x, tpos y, char *p_text, MENU_TYPE spr_align, 
                              bool save_back, LEVEL_EVENT click1, LEVEL_EVENT click2, LEVEL_EVENT click3)
{
  menu_item_set_pos(x,y);
  menu_item_draw(p_text,spr_align,save_back,click1,click2,click3);
}

#define MENU_TEXT_CHECKBOX_DIFF_X (20)
#define MENU_TEXT_CHECKBOX_DIFF_Y (-1)

void gui_base::menu_item_draw_checkbox(char *p_text, MENU_TYPE spr_align, bool checked, int checkbox_id,
                                       LEVEL_EVENT click1, LEVEL_EVENT click2, LEVEL_EVENT click3)
{

  switch(spr_align)
  {
    case MENU_LEFT:
      {
        checkbox[checkbox_id] = CHECKBOX_CONFIG(checked, last_x, last_y);
        checkbox[checkbox_id].draw();
      
        p_font->alignment_set(MENU_LEFT);
        p_font->select(FONT_DEFAULT);
        p_font->print(&r,
                      last_x + MENU_TEXT_CHECKBOX_DIFF_X + MENU_TEXT_DIFF_X,
                      last_y + MENU_TEXT_CHECKBOX_DIFF_Y,
                      p_text);
      
        LEVEL_EVENT checkbox_draw = LEVEL_EVENT(GI_CHECKBOX_SWITCH, checkbox_id);
        LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, ET_INT(FONT_SELECTED), 
                                        ET_INT(last_x + MENU_TEXT_CHECKBOX_DIFF_X + MENU_TEXT_DIFF_X),
                                        ET_INT(last_y + MENU_TEXT_CHECKBOX_DIFF_Y),
                                        ET_INT(MENU_LEFT), ET_INT(p_text));
        LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, ET_INT(FONT_DEFAULT), 
                                        ET_INT(last_x + MENU_TEXT_CHECKBOX_DIFF_X + MENU_TEXT_DIFF_X), 
                                        ET_INT(last_y + MENU_TEXT_CHECKBOX_DIFF_Y),
                                        ET_INT(MENU_LEFT), ET_INT(p_text));

        // 20x20 is a size of the check-box
        RECT r_box = {last_x, last_y, 20, 20};

        // highlight the text when mouse is over
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_box), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_text));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_box), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_text));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_text));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_text));
      
        // if user clicked, activate the check-box and wait for mouse button release
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_box, MASK_BUTTON_LEFT, BUTTON_UP),
                        MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, checkbox_draw));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT, BUTTON_UP),
                        MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, checkbox_draw));
      
        // if user clicked, activate the check-box and wait for mouse button release
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_box, MASK_BUTTON_LEFT, BUTTON_UP),
                        MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT, BUTTON_UP),
                        MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));
              
        last_x += last_dx;
        last_y += last_dy;
      }
      break;
    
    default:
      break;
   }
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
        p_font->alignment_set((MENU_TYPE)ev.param_int_get(PARAM_3));
        p_font->print(&r,(tpos)ev.param_int_get(PARAM_1),(tpos)ev.param_int_get(PARAM_2),(char *)ev.param_point_get(PARAM_4));
      
        p_grf->redraw_add(&r);
        p_grf->flip();
      }
      break;    
    case GI_CHECKBOX_SWITCH:
      {
        // format: [GC_CHECKBOX_DRAW, checkbox_id]
        int id = ev.param_int_get(PARAM_0);
        assert(id < CHECKBOX_NUM);
      
        checkbox[id].check_switch();
        checkbox[id].draw_all();      
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

void gui_base::menu_dummy(MENU_STATE state, size_ptr data, size_ptr data1)
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
