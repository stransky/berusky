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

#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "gui.h"

typedef enum {

  HORIZONTAL,
  VERTICAL

} DIRECTION;

#define NO_SELECTION (-1)

typedef class editor_selection {

  // The current level
  LEVEL_EDITOR *p_level;

public:

  // current selection
  int item;
  int variant;
  int rotation;

public:

  editor_selection(LEVEL_EDITOR *p_level_)
  : p_level(p_level_)
  {
    item = 0;
    variant = 0;
    rotation = 0;
  }

} EDITOR_SELECTION;

typedef enum {
  
  ITEM_SET,
  ITEM_MODIFY

} CHANGE_TYPE;

typedef class item_panel {

  static ITEM_REPOSITORY *p_repo;

  // A slave panel
  // Will be updated by selection of this panel
  class item_panel *p_slave;

  // Location and appearance of the panel
  tpos start_x, dx;
  tpos start_y, dy;

  // Number of visible items
  int item_num;
  int item_highlighted;
  int item_selected;

  int panel_handle;

public:
  
  // Items in the panel
  int item_first;
  int item_increment;

  // Variants in the panel
  int variant_first;
  int variant_increment;

public:

  /* Draws item(s) in panel
  */
  void publish(void);

  /* Draws controls for a panel
  */
  void publish_controls(INPUT *p_input);


  /* Items in panel
  */

  void items_first_set(int item_first_, bool redraw = TRUE)
  {
    item_first = item_first_;
    if(redraw)
      publish();
  }
  void items_increment_set(int variant_increment_, bool redraw = TRUE)
  {
    variant_increment = variant_increment_;
    if(redraw)
      publish();
  }


  void variant_first_set(int variant_first_, bool redraw = TRUE)
  {
    variant_first = variant_first_;
    if(redraw)
      publish();
  }
  void variant_increment_set(int variant_increment_, bool redraw = TRUE)
  {
    variant_increment = variant_increment_;
    if(redraw)
      publish();
  }


  void items_set(int item_first_, int item_increment_, bool redraw = TRUE)
  {
    item_first = item_first_;
    item_increment = item_increment_;
    if(redraw)
      publish();
  }
  void variant_set(int variant_first_, int variant_increment_, bool redraw = TRUE)
  {
    variant_first = variant_first_;
    variant_increment = variant_increment_;
    if(redraw)
      publish();
  }

  int  item_return(tpos x, tpos y, EDITOR_SELECTION *p_sel = NULL);
  int  item_return(int selected_item, EDITOR_SELECTION *p_sel = NULL, int correction = 0);

  void item_highlight(tpos x, tpos y, bool redraw = TRUE)
  {
    item_highlighted = item_return(x,y);
    if(redraw)
      publish();
  }
  void item_unhighlight(bool redraw = TRUE)
  {
    item_highlighted = NO_SELECTION;
    if(redraw)
      publish();
  }
  
  void item_select(tpos x, tpos y, EDITOR_SELECTION *p_sel, bool redraw = TRUE)
  {
    item_selected = item_return(x,y,p_sel);
    if(p_slave) {
      p_slave->items_first_set(p_sel->item);
    }
    if(redraw)
      publish();
  }
  void item_unselect(bool redraw = TRUE)
  {
    item_selected = NO_SELECTION;
    if(redraw)
      publish();
  }

  bool panel_scroll_check(int direction);
  void panel_scroll(int direction, EDITOR_SELECTION *p_sel, bool redraw = TRUE);

  // return size of panel tool (number of items)
  int  items_get(void)
  {
    return(item_num);
  }

  /* Setting up
  */
  static void set_up(ITEM_REPOSITORY *p_repo_)
  {    
    p_repo = p_repo_;
  }
  
  void set_up(int item_num_, DIRECTION direction, tpos start_x_, tpos start_y_, 
              int panel_handle_, class item_panel *p_slave_ = NULL)
  {
    switch(direction) {
      case HORIZONTAL:
        dx = ITEM_SIZE_X;
        dy = 0;
        break;
      case VERTICAL:
        dx = 0;
        dy = ITEM_SIZE_Y;
        break;
    }

    item_num = item_num_;
  
    start_x = start_x_;
    start_y = start_y_;
  
    panel_handle = panel_handle_;
    p_slave = p_slave_;
      
    item_unhighlight(FALSE);
    item_unselect();
  }  

  item_panel()
  {
    memset(this,0,sizeof(*this));
  }

} ITEM_PANEL;

typedef class editor_layer_config {

public:

  LAYER_CONFIG lc;
  RECT         coord[ALL_LEVEL_LAYERS];

public:
  
  editor_layer_config(void);  

} EDITOR_LAYER_CONFIG;

#define CONSOLE_MAX_INPUT_LINE            200
#define CONSOLE_OUTPUT_LINES              2
#define CONSOLE_BOOLEAN_NO_INPUT          (-1)
#define CONSOLE_INPUT_LINE_HEIGHT         22
#define CONSOLE_INPUT_LINE_SHIFT          5

typedef enum {
  
  INPUT_BOOLEAN,
  INPUT_STRING

} INPUT_TYPE;

typedef enum {

  INPUT_NONE,
  INPUT_READY  

} INPUT_STATE;

typedef class editor_console {

  static const char *boolean_yes;
  static const char *boolean_no;

  INPUT_TYPE   itype;
  INPUT_STATE  istate;

  tpos         x,y;
  tpos         w,h;
  tpos         height_diff;

  int          lines;
  char         output_lines[CONSOLE_OUTPUT_LINES][CONSOLE_MAX_INPUT_LINE];

  tpos         ix_title;
  tpos         iy_title;
  char         input_line_title[CONSOLE_MAX_INPUT_LINE];

  tpos         ix_input;
  tpos         iy_input;

  int          input_boolean;

  int          input_string_lenght;
  char         input_string[CONSOLE_MAX_INPUT_LINE];

  INPUT       *p_input;

public:

  editor_console(INPUT *p_input_, tpos sx, tpos sy, tpos dx, int lines_);

  // ----------------------------------------------
  // Output interface
  // ----------------------------------------------
  
  void print(const char *p_text,...);
  void output_clear(bool redraw = TRUE);
  void output_redraw(void);


  // ----------------------------------------------
  // Input line interface
  // ----------------------------------------------
  void  input_start(INPUT_TYPE type, char *p_text = NULL);
  void  input_stop(bool redraw = TRUE);
  void  input_clear(bool redraw = TRUE);
  void  input_clear_last(bool redraw = TRUE);
  void  input_add_char(char c, bool redraw = TRUE);
  void  input_redraw(void);  
  
  char * input_string_get(void)
  {
    return(input_string);
  }
  int  input_string_get_max_lenght(void)
  {
    return(CONSOLE_MAX_INPUT_LINE);
  }
  void input_string_update(char *p_str = NULL)
  {
    if(p_str) {
      strncpy(input_string,p_str,CONSOLE_MAX_INPUT_LINE);
    }
    input_string[CONSOLE_MAX_INPUT_LINE-1] = '\0';
    input_string_lenght = strlen(input_string);
  }

  // ----------------------------------------------
  // Input line title interface
  // ----------------------------------------------
  char * input_title_get(void)
  {
    return(input_line_title);
  }
  int  input_title_get_max_lenght(void)
  {
    return(CONSOLE_MAX_INPUT_LINE);
  }
  void input_title_update(char *p_str, bool redraw = TRUE)
  {
    if(p_str) {
      strncpy(input_line_title,p_str,CONSOLE_MAX_INPUT_LINE);
    }
    input_line_title[CONSOLE_MAX_INPUT_LINE-1] = '\0';
  
    if(redraw) {
      input_redraw();
    }
  }
  bool  input_bool_get(void)
  {
    return(input_boolean);
  }

  editor_console()
  {
    memset(this,0,sizeof(*this));
  }

} EDITOR_CONSOLE;

class editor_gui;

typedef class editor_gui EDITOR_GUI;
typedef void (EDITOR_GUI::*EDITOR_CONSOLE_CALLBACK)(void);

typedef class editor_gui : public gui_base {

  DIR_LIST               *p_dir;

  ITEM_REPOSITORY        *p_repo;
  LEVEL_EDITOR            level;

  LEVEL_EVENT_QUEUE       queue;

public:
  
  EDITOR_CONSOLE          console;              // console interface
  EDITOR_CONSOLE_CALLBACK console_callback;     // console callback function
  int                     console_callback_id;  // ID of callback

public:

  #define NO_HANDLE       0
  #define HANDLE_1        (NO_HANDLE+1)
  #define HANDLE_2        (NO_HANDLE+2)
  #define HANDLE_3        (NO_HANDLE+3)
  #define HANDLE_4        (NO_HANDLE+4)

  void   input_start(EDITOR_CONSOLE_CALLBACK callback,  int callback_id, INPUT_TYPE type, char *p_text,...);
  void   console_input(MENU_STATE state, int data, int data1);
  void   input_stop(bool success);  


  bool   input_get_bool(void)
  {
    return(console.input_bool_get());
  }

  char * input_get_string(void)
  {
    return(console.input_string_get());
  }

  int    input_get_callback_handle(void)
  {
    return(console_callback_id);
  }

public:

  #define                 PANELS        4
  ITEM_PANEL              ipanel[PANELS];

public:
  
  // Panel interface
  void panel_item_select(int panel, tpos x, tpos y);
  void panel_item_highlight(int panel, tpos x, tpos y);
  void panel_reset(void);
  void panel_scroll(int panel, int direction);

private:

  EDITOR_SELECTION        sel;

public:
  
  // Selection interface
  void selection_draw(void);

  void selection_cursor_draw_status(char *p_text,...);
  void selection_cursor_update(void);

  void selection_rotate(int direction = 1);

private:

  void side_menu_create(void);
  void side_menu_update(void);

private:

  EDITOR_LAYER_CONFIG config;

public:

  // General level interface
  char level_name[MAX_FILENAME];
  bool level_edited;

  char *level_name_get(void);
  void level_name_set(char *p_name);

  void level_caption_update(void);
  
  void level_edited_set(void);
  bool level_edited_get(void);
  void level_edited_clear(void);

  void help(void);

  void level_new(bool force = FALSE);
  void level_new_callback(void);

  void level_load(char *p_file, int force = FALSE);
  void level_load_callback(void);

  void level_save(int force = TRUE);
  void level_save_as(char *p_file = NULL, int force = FALSE);
  void level_save_as_callback(void);

public:

  editor_gui(ITEM_REPOSITORY *p_repo_, DIR_LIST *p_dir_);
  ~editor_gui(void);
  
  bool editor_quit(bool force);
  void editor_quit_callback(void);

  // Test event
  void test(void);
  void test_gui(void);

  // Level screen interface
  void level_draw(void);
  void level_config(void);

  // Menu interface
  void menu_draw(void);

  // Level screen events
  void level_cursor_set(tpos x, tpos y);  

  void level_item_insert(void);
  void level_item_insert_rectangle(bool filled);
  
  void level_item_rotate(CHANGE_TYPE type, int rotation);
  void level_item_variate(CHANGE_TYPE type, int variant);
  
  void level_item_clear(void);
  void level_item_clear_rectangle(bool filled);
  
  void layer_menu_draw(void);
  void layer_status_draw(void);
  void layer_status_switch(int layer, LAYER_STATE state);  
  void layer_active_set(int layer);
  int  layer_active_get(void);
  int  layer_active_get(tpos x, tpos y);
  

  // Mouse handler
  void mouse_handler(LEVEL_EVENT_QUEUE *p_queue, LEVEL_EVENT ev);

  // general handler
  bool event_handler(void);

  // Add an event to main queue
  void event_add(LEVEL_EVENT ev);
  
  // Set a keyset for editor
  void keyset_set(EVENT_KEY_SET *p_keyset);
  
  // Run level
  void editor_run_level(void);
  
  // Shade level 
  void editor_shader(void);
  void editor_shader(tpos sx, tpos sy, tpos dx, tpos dy);

  void level_change_backgroud(void);

  void help_print_line(tpos x_pos, tpos &y_pos, char *p_key, char *p_name);

public:  
  // An Undo interface
  void undo_store(void);
  bool undo_restore(void);

} EDITOR_GUI;

#define SCREEN_HANDLE_IN      0
#define SCREEN_HANDLE_OUT     1
#define SCREEN_HANDLE_CLICK   2
#define SCREEN_HANDLE_ROTATE  3
#define SCREEN_HANDLE_VARIATE 4

#define PANEL_HANDLE_1        11
#define PANEL_HANDLE_2        12
#define PANEL_HANDLE_3        13
#define PANEL_HANDLE_4        14

#endif // __EDITOR_H__
