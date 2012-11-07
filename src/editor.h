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

#define ITEMS_X_START   0
#define ITEMS_Y_START   0
#define ITEMS_X_DIFF    0
#define ITEMS_Y_DIFF    35
#define ITEMS_DX        (EDITOR_RESOLUTION_X-ITEMS_X_START)
#define ITEMS_DY        (EDITOR_RESOLUTION_Y-ITEMS_Y_START)

//-- finish the pannel
#define ITEMS_IN_PANEL  7
#define ITEMS_START     0xffff
#define ITEMS_END       0xfffe

typedef class editor_panel {

  // Attached panel with items
  class editor_panel *p_attached_panel;

  // Location and appearance of the panel
  tpos start_x, dx;
  tpos start_y, dy;

  // Number of visible cells
  int panel_slot_num;
  int panel_slot_highlighted;
  int panel_slot_selected;

  int panel_handle;

protected:

  static ITEM_REPOSITORY *p_repo;

public:

  void attached_panel_set(class editor_panel *p_panel)
  {
    p_attached_panel = p_panel;
  }

  class editor_panel * attached_panel_get(void)
  {
    return(p_attached_panel);
  }

  /* Return size of panel tool (number of slots which can be displayed)
  */
  int panel_size_get(void)
  {
    return(panel_slot_num);
  }

  /* Return index of slot at x,y coordinates
  */
  bool slot_return(tpos x, tpos y, int &slot);

  /* Return index of slot at x,y coordinates 
     or NO_SELECTION
  */
  int slot_return(tpos x, tpos y)
  {
    int slot;
    return(slot_return(x, y, slot) ? slot : NO_SELECTION);
  }

  /* Draw one slot of the panel
  */
  void slot_draw(int slot, int item, int variant);

  /* Draw panel controls
  */
  void controls_draw(bool draw);
  
  /* Panel highlight routines
  */
  void slot_highlight(int slot, bool redraw)
  {
    panel_slot_highlighted = slot;
  
    assert(panel_slot_highlighted >= NO_SELECTION && 
           panel_slot_highlighted < panel_slot_num);
  
    if(redraw)
      panel_draw(redraw);
  }
  void slot_highlight(tpos x, tpos y, bool redraw)
  {
    slot_highlight(slot_return(x,y),redraw);
  }
  void slot_unhighlight(bool redraw)
  {
    slot_highlight(NO_SELECTION,redraw);
  }

  /* Panel selection routines
  */
  void slot_select(int slot, EDITOR_SELECTION *p_sel, bool propagate, bool redraw)
  {
    // Set selected slot
    panel_slot_selected = slot;
  
    assert(panel_slot_selected >= NO_SELECTION && 
           panel_slot_selected < panel_slot_num);
  
    if(redraw) {
      panel_draw(redraw);
    }
  
    // Propagate to editor selection (item/variant)
    if(propagate) {
      item_select(p_sel);
    }  
  }
  void slot_select(tpos x, tpos y, EDITOR_SELECTION *p_sel, bool propagate, bool redraw)
  {
    slot_select(slot_return(x,y),p_sel,propagate,redraw);
  }
  void slot_unselect(bool propagate, bool redraw)
  {
    slot_select(NO_SELECTION,NULL,propagate, redraw);
  }

  void slot_selection_fix(int last_valid_slot, bool redraw)
  {
    if(panel_slot_selected != NO_SELECTION) {
      if(panel_slot_selected > last_valid_slot)
        panel_slot_selected = last_valid_slot;
    }
    if(panel_slot_highlighted != NO_SELECTION) {
      if(panel_slot_highlighted > last_valid_slot)
        panel_slot_highlighted = last_valid_slot;
    }
    if(redraw) {
      panel_draw(redraw);
    }
  }

  int selected_slot_get(void)
  {
    return(panel_slot_selected);
  }  

  /* Is the panel slot valid?
     It's a relative slot positition, from 0 to panel_item_num
  */
  virtual bool slot_valid(int slot) = 0;

  /* Return first item on the panel
  */
  virtual int item_firts_get(void) = 0;
  
  /* Return last valid item (0 is the first one)
  */
  virtual int items_num_get(void) = 0;
  
  /* Select an editor item. It's based on recent selected slot
  */
  virtual void item_select(EDITOR_SELECTION *p_sel) = 0;

  /* Set an item for first slot
  */
  virtual void panel_set(int first, EDITOR_SELECTION *p_sel, bool propagate, bool redraw) = 0;

  /* Draws controls and panel item(s)
  */
  virtual void panel_draw(bool draw) = 0;

  /* Scroll the pannel with given direction
     and preserve selected item
  */
  void panel_scroll(int direction, EDITOR_SELECTION *p_sel, bool redraw);

  /* Setting up
  */
  static void set_up(ITEM_REPOSITORY *p_repo_)
  {    
    p_repo = p_repo_;
  }
  
  void register_controls_events(INPUT *p_input);

  // Get pannel size, without control buttons
  RECT boundary_get(void);

  void set_up(int panel_size, DIRECTION direction, 
              tpos sx, tpos sy, int handle)
  {
    switch(direction) {
      case HORIZONTAL:
        dx = EDITOR_ITEM_SIZE_X;
        dy = 0;
        break;
      case VERTICAL:
        dx = 0;
        dy = EDITOR_ITEM_SIZE_Y;
        break;
    }

    panel_slot_num = panel_size;
  
    start_x = sx;
    start_y = sy;
  
    panel_handle = handle;

    panel_slot_highlighted = NO_SELECTION;
    panel_slot_selected = NO_SELECTION;
  }  

  editor_panel(int panel_item_num, DIRECTION direction, 
              tpos start_x, tpos start_y,
              int handle)
  {
    memset(this,0,sizeof(*this));
    set_up(panel_item_num, direction, start_x, start_y, handle);
  }

} EDITOR_PANEL;

/* Derived panel for items
*/
typedef class item_panel : public editor_panel {

  // Items in the panel
  int panel_item_first;

public:
  
  /* Items in panel
  */  
  void panel_set(int first, EDITOR_SELECTION *p_sel, bool propagate, bool redraw)
  {
    panel_item_first = first;
  
    if(items_num_get() <= panel_size_get()) {  
      slot_selection_fix(items_num_get()-1, redraw);
    }
    else {
      slot_selection_fix(panel_size_get(), redraw);
    }
  
    if(propagate)
      item_select(p_sel);
  }

  /* Return first item on the panel
  */
  int item_firts_get(void)
  {
    return(panel_item_first);
  }
  
  /* Return last valid item (0 is the first one)
  */
  int items_num_get(void)
  {
    return(p_repo->item_num_get());
  }

  /* Select the select item by variant panel
  */
  void item_select(EDITOR_SELECTION *p_sel);

  bool slot_valid(int slot)
  {
    if(slot >= 0 && slot < panel_size_get()) {
      return(item_firts_get() + slot < items_num_get());
    }
    return(FALSE);
  }

public:
  
  void panel_draw(bool draw);

public:

  item_panel(int panel_item_num, 
             DIRECTION direction,
             tpos start_x, tpos start_y, 
             int handle)
  : editor_panel(panel_item_num, direction, start_x, start_y, handle),    
    panel_item_first(0)
  {  
  }

} ITEM_PANEL;

/* Derived panel for variants
*/
typedef class variant_panel : public editor_panel {

  // Item which variants are published
  int panel_item;

  // Variants in the panel
  int panel_variant_first;

public:
  
  void panel_item_set(int item, bool redraw)
  {
    panel_item = item;
    if(redraw)
      panel_draw(redraw);
  }

  /* Items in panel
  */  
  void panel_set(int first, EDITOR_SELECTION *p_sel, bool propagate, bool redraw)
  {
    panel_variant_first = first;
  
    if(variant_last_get() < panel_size_get()) {  
      slot_selection_fix(items_num_get()-1, redraw);
    }
    else {
      slot_selection_fix(panel_size_get(), redraw);
    }  
  
    if(propagate)
      item_select(p_sel);
  }

  /* Return first variant on the panel
  */
  int variant_firts_get(void)
  {
    return(panel_variant_first);
  }
  
  /* Return last valid item (0 is the first one)
  */
  int variant_last_get(void)
  {
    return(p_repo->variants_get(panel_item));
  }

  /* Return first item on the panel
  */
  int item_firts_get(void)
  {
    return(variant_firts_get());
  }
  
  /* Return last valid item (0 is the first one)
  */
  int items_num_get(void)
  {
    return(variant_last_get());
  }

  void item_select(EDITOR_SELECTION *p_sel)
  {
    int slot = selected_slot_get();
    if(p_sel && slot != NO_SELECTION) {
      p_sel->item = panel_item;
      p_sel->variant = panel_variant_first+slot;
    }
  }

  bool slot_valid(int slot)
  {
    if(slot >= 0 && slot < panel_size_get()) {
      return(variant_firts_get() + slot < variant_last_get());
    }
    return(FALSE);
  }

public:
  
  void panel_draw(bool draw);

public:

  variant_panel(int panel_size, DIRECTION direction,
                tpos start_x, tpos start_y, int handle)
  : editor_panel(panel_size, direction, start_x, start_y, handle),    
    panel_item(0),
    panel_variant_first(0)
  {  
  }

} VARIANT_PANEL;

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
  INPUT_STRING,
  INPUT_WAIT,

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
  void output_redraw(bool draw);


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
  bool input_bool_get(void)
  {
    return(input_boolean != CONSOLE_BOOLEAN_NO_INPUT && input_boolean);
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
  bool                    draw_level; // Draw the level?

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

  void  input_start(EDITOR_CONSOLE_CALLBACK callback,  int callback_id, INPUT_TYPE type, char *p_text,...);
  void  console_input(MENU_STATE state, int data, int data1);  
  void  console_wait(MENU_STATE state, int data, int data1);
  void  input_stop(bool success);  

  void  console_draw(bool draw)
  {
    console.output_redraw(draw);
  }

  bool  input_get_bool(void)
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

  #define                 PANELS        2
  EDITOR_PANEL           *ipanel[PANELS];

public:
  
  // Panel interface
  void panel_item_select(int panel, tpos x, tpos y);
  void panel_item_highlight(int panel, tpos x, tpos y);
  void panel_draw(bool draw);
  void panel_scroll(int panel, int direction);
  void panel_scroll_mouse(int direction);

private:

  // Recent selected item (in editor)
  EDITOR_SELECTION        selected_editor_item;

private:

  void selection_cursor_draw_status(bool draw, char *p_text,...);

public:
  
  // Selection interface
  void selection_draw(bool draw);
  void selection_cursor_draw(bool draw);

  void selection_rotate(int direction = 1);

private:

  void side_menu_create(void);
  void side_menu_draw(bool draw);

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
  void help_quit(void);

  void level_new(bool force = FALSE);
  void level_new_callback(void);

  void level_load(char *p_file, int force = FALSE);
  void level_load_callback(void);

  void level_save(int force = TRUE);
  void level_save_as(char *p_file = NULL, int force = FALSE);
  void level_save_as_callback(void);

  void level_move(tpos dx, tpos dy);

  void draw(void);

public:

  editor_gui(ITEM_REPOSITORY *p_repo_, DIR_LIST *p_dir_);
  ~editor_gui(void);
  
  void editor_reset(void);

  bool editor_quit(bool force);
  void editor_quit_callback(void);

  // Test event
  void test(void);
  void test_gui(void);

  // Level screen interface  
  bool level_draw(void);
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
  
  void layer_menu_create(void);
  void layer_menu_draw(bool draw);
  
  void layer_status_draw(bool draw);
  void layer_status_switch(int layer, LAYER_STATE state);  
  void layer_active_set(int layer, bool draw);
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
