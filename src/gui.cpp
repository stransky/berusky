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
: last_x(0), last_y(0), last_dx(0), last_dy(0), highlight_group_next(0)
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

/*
  MENU_TEXT_DIFF_X, MENU_TEXT_DIFF_Y
*/
void gui_base::menu_item_draw_sprite_set(spr_handle active, spr_handle inactive, 
                                         int menu_text_diff_x_, int menu_text_diff_y_)
{
  menu_spr_active = active;
  menu_spr_inactive = inactive;
  menu_text_diff_x = menu_text_diff_x_;
  menu_text_diff_y = menu_text_diff_y_;
  menu_spr_diff_dx = p_grf->sprite_get_width(active);
  menu_spr_diff_dy = p_grf->sprite_get_height(active);
}

void gui_base::menu_item_draw_sprite(char *p_text, MENU_TYPE spr_align, bool save_back,
                                     LEVEL_EVENT click1, LEVEL_EVENT click2, LEVEL_EVENT click3)
{
  switch(spr_align)
  {
    case MENU_LEFT_SPRITE:
      {
        p_grf->draw(menu_spr_inactive, last_x, last_y);
        p_font->alignment_set(MENU_LEFT);
        p_font->select(FONT_DEFAULT);
        p_font->print(&r, last_x + menu_spr_diff_dx + menu_text_diff_x, 
                          last_y + menu_text_diff_y, p_text);
      
        RECT r_arrow = {last_x, last_y, menu_spr_diff_dx, menu_spr_diff_dy};
      
        LEVEL_EVENT s_spr = LEVEL_EVENT(GI_SPRITE_DRAW, menu_spr_active, last_x, last_y);
        LEVEL_EVENT u_spr = LEVEL_EVENT(GI_SPRITE_DRAW, menu_spr_inactive,  last_x, last_y);
        LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, ET(FONT_SELECTED), 
                             ET_INT(last_x + menu_spr_diff_dx + menu_text_diff_x), 
                             ET_INT(last_y + menu_text_diff_y), 
                             ET_INT(MENU_LEFT), ET_INT(p_text));
        LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, ET_INT(FONT_DEFAULT), 
                             ET_INT(last_x + menu_spr_diff_dx + menu_text_diff_x), 
                             ET_INT(last_y + menu_text_diff_y), 
                             ET_INT(MENU_LEFT), ET_INT(p_text));
        LEVEL_EVENT u_text_highlight = LEVEL_EVENT(GI_HIGHLIGHT_EVENT, highlight_group_next);
    
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_spr, s_text));
        if(!highlight_group_next)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_spr, u_text));
        else
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, u_text_highlight, u_spr, u_text));
        if(save_back)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(GI_MENU_BACK_PUSH)));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));
  
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_spr, s_text));
        if(!highlight_group_next)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_spr, u_text));
        else
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, u_text_highlight, u_spr, u_text));
        if(save_back)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(GI_MENU_BACK_PUSH)));
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));
        
        last_x += last_dx;
        last_y += last_dy;      
      }
      break;
    case MENU_RIGHT_SPRITE:
      {
        p_grf->draw(menu_spr_inactive, last_x, last_y);
        p_font->alignment_set(MENU_RIGHT);
        p_font->select(FONT_DEFAULT);
        p_font->print(&r, last_x - menu_text_diff_x, last_y + menu_text_diff_y, p_text);

        RECT r_arrow = {last_x, last_y, menu_spr_diff_dx, menu_spr_diff_dy};

        LEVEL_EVENT s_spr = LEVEL_EVENT(GI_SPRITE_DRAW, menu_spr_active, last_x, last_y);
        LEVEL_EVENT u_spr = LEVEL_EVENT(GI_SPRITE_DRAW, menu_spr_inactive,  last_x, last_y);
        LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, 
                                        ET_INT(FONT_SELECTED), 
                                        ET_INT(last_x - menu_text_diff_x), 
                                        ET_INT(last_y + menu_text_diff_y), 
                                        ET_INT(MENU_RIGHT), ET_INT(p_text));
        LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, 
                                        ET_INT(FONT_DEFAULT),  
                                        ET_INT(last_x - menu_text_diff_x), 
                                        ET_INT(last_y + menu_text_diff_y), 
                                        ET_INT(MENU_RIGHT), ET_INT(p_text));
        LEVEL_EVENT u_text_highlight = LEVEL_EVENT(GI_HIGHLIGHT_EVENT, highlight_group_next);

        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_spr, s_text));
        if(!highlight_group_next)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_spr, u_text));
        else
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, u_text_highlight, u_text));
        if(save_back)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(GI_MENU_BACK_PUSH)));        
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r_arrow, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));

        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_spr, s_text));
        if(!highlight_group_next)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_spr, u_text));
        else
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, u_text_highlight, u_text));
        if(save_back)
          input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(GI_MENU_BACK_PUSH)));        
        input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));

        last_x += last_dx;
        last_y += last_dy;      
      }
      break;
    case MENU_CENTER_SPRITE:
      {
        // not implemented
        assert(0);
      }
      break;
    default:
      break;
  }
}

void gui_base::menu_item_draw_sprite(tpos x, tpos y, char *p_text, MENU_TYPE spr_align, bool save_back,
                                     LEVEL_EVENT click1, LEVEL_EVENT click2, LEVEL_EVENT click3)
{
  menu_item_set_pos(x,y);
  menu_item_draw_sprite(p_text, spr_align, save_back, click1, click2, click3);
}

void gui_base::menu_item_draw_text(char *p_text, MENU_TYPE spr_align, bool save_back,
                                   LEVEL_EVENT click1, LEVEL_EVENT click2, LEVEL_EVENT click3)
{
  MENU_TYPE align;

  switch(spr_align) {
    case MENU_LEFT_NO_ARROW:
      {
        align = MENU_LEFT;
      }
      break;
    
    case MENU_RIGHT_NO_ARROW:
      {
        align = MENU_RIGHT;
      }
      break;
    case MENU_CENTER_NO_ARROW:
      {
        align = MENU_CENTER;
      }
      break;
    default:
      // WTF??
      assert(0);
      align = MENU_LEFT;
      break;
  }

  p_font->alignment_set(align);
  p_font->select(FONT_DEFAULT);
  p_font->print(&r, last_x, last_y, p_text);

  LEVEL_EVENT s_text = LEVEL_EVENT(GI_STRING_DRAW, ET_INT(FONT_SELECTED), 
                                  ET_INT(last_x), 
                                  ET_INT(last_y), 
                                  ET_INT(align),
                                  ET_INT(p_text));
  LEVEL_EVENT u_text = LEVEL_EVENT(GI_STRING_DRAW, ET_INT(FONT_DEFAULT),
                                  ET_INT(last_x), 
                                  ET_INT(last_y), 
                                  ET_INT(align),
                                  ET_INT(p_text));
  LEVEL_EVENT u_text_highlight = LEVEL_EVENT(GI_HIGHLIGHT_EVENT, highlight_group_next);
  u_text_highlight.depends_add(1);

  // event - release the saved highlighted event (if any)
  input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN,  s_text));
  if(!highlight_group_next)
    input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_OUT, u_text));
  else
    input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, u_text_highlight, u_text));

  if(save_back)
    input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, LEVEL_EVENT(GI_MENU_BACK_PUSH)));
  
  input.mevent_add(MOUSE_EVENT(MOUSE_STATE(r, MASK_BUTTON_LEFT), MEVENT_ACTIVATE_ONCE|MEVENT_MOUSE_IN|MEVENT_MOUSE_BUTTONS, click1, click2, click3));
  
  last_x += last_dx;
  last_y += last_dy;
  
  highlight_group_next = HIGHLIGHT_GROUP_NONE;
}

void gui_base::menu_item_draw(char *p_text, MENU_TYPE spr_align, bool save_back, LEVEL_EVENT click1, LEVEL_EVENT click2, LEVEL_EVENT click3)
{
  switch(spr_align)
  {
    case MENU_LEFT:
      {      
        menu_item_draw_sprite_set(MENU_SPRIT_ARROW_LC, MENU_SPRIT_ARROW_L, 
                                  MENU_TEXT_DIFF_X, MENU_TEXT_DIFF_Y);
        menu_item_draw_sprite(p_text, MENU_LEFT_SPRITE, save_back,
                              click1, click2, click3);
      }
      break;
    case MENU_RIGHT:
      {
        menu_item_draw_sprite_set(MENU_SPRIT_ARROW_RC, MENU_SPRIT_ARROW_R,
                                  MENU_TEXT_DIFF_X, MENU_TEXT_DIFF_Y);
        menu_item_draw_sprite(p_text, MENU_RIGHT_SPRITE, save_back,
                              click1, click2, click3);
      }          
      break;
    
    case MENU_CENTER:
      {
        menu_item_draw_sprite_set(MENU_SPRIT_ARROW_RC, MENU_SPRIT_ARROW_L,
                                  MENU_TEXT_DIFF_X, MENU_TEXT_DIFF_Y);
        menu_item_draw_sprite(p_text, MENU_CENTER_SPRITE, save_back,
                              click1, click2, click3);
      }      
      break;
    
    case MENU_LEFT_NO_ARROW:
    case MENU_RIGHT_NO_ARROW:
    case MENU_CENTER_NO_ARROW:
      {
        menu_item_draw_text(p_text, spr_align, save_back, click1, click2, click3);
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

void gui_base::menu_services(LEVEL_EVENT_QUEUE *p_read_queue, LEVEL_EVENT_QUEUE *p_write_queue, LEVEL_EVENT ev)
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
    case GI_HIGHLIGHT_EVENT:
      {
        // format: [GI_HIGHLIGHT_EVENT, group]

        // launch the event if there's any active
        int group = ev.param_int_get(PARAM_0);
        if(highlight_group[group].active) {          
          p_write_queue->add(highlight_group[group].event, highlight_group[group].event_num);          
          p_write_queue->commit();
          highlight_group[group].active = FALSE;
        }      
        
        int highlight_events = ev.depends_get();
        // store the next one to the slot
        assert(highlight_events > 0);
        for(int i = 0; i < highlight_events; i++) {
          assert(!p_read_queue->empty());
          highlight_group[group].event[i] = p_read_queue->get();
        }
        highlight_group[group].event_num = highlight_events;
        highlight_group[group].active = TRUE;
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
