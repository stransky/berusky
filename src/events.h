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

#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "portability.h"

// uncomment for event debugging
// #define EVENTS_DEBUG 1

#ifdef LINUX
#include <sys/time.h>
#endif

/****************************************************************
  Level events - for game level
  
  UI_ - events from user interface
  GL_ - events from game logic
  AN_ - events for animation engine (usually created by game logic or level changer)
  MN_ - events for user interface / menus (usually obtained as user input)
  GC_ - events for game control (start/stop playing, load/save level and so on)
  
  ED_ - events from level editor
  
 ****************************************************************
 
 Common parameters:
 
 change_sprite = true -> change sprites on screen
 change_sprite = false -> don't touch sprites on screen  
 
  
*/
typedef enum {

  // Zero-event
  EV_NONE = 0,

  // Test-event
  EV_TEST = 1,

  // ------------------------------------------------------------------------
  // GUI events

  // user interface - block selected event (player move and so on)
  // format: [GI_BLOCK_KEYS, key_group, blocked]
  // key_group = key group to block
  // blocked = true/false
  GI_BLOCK_KEYS,

  // user interface - enable all groups
  // format: [GI_UNBLOCK_ALL]
  GI_UNBLOCK_ALL,

  // pressed key
  // format: [GI_KEY_DOWN, asci code of key]
  GI_KEY_DOWN,

  // Mouse was moved
  // format: [GI_MOUSE_MOVE, x, y, button]
  GI_MOUSE_MOVE,

  // a raw mouse event (it's generated by an input interface when
  // MEVENT_MOUSE_EXTERNAL flag was set)
  // format: [GI_MOUSE_EVENT, handle, x, y, state, button]
  GI_MOUSE_EVENT,
  
  // Pick up one message from backtrace stack
  // and run it
  GI_MENU_BACK_POP,
  
  // Push one message to backtrace stack
  // format: [GC_MENU_BACK_PUSH, function, param_1, param_2]
  GI_MENU_BACK_PUSH,
  
  // Draw sprite
  // format: [GC_SPRITE_DRAW, sprite_handle, x, y]
  GI_SPRITE_DRAW,

  // Print string
  // format: [GC_STRING_DRAW, font, x, y, x_aling, string]
  GI_STRING_DRAW,
  
  // Switch and redraw the checkbox
  // format: [GI_CHECKBOX_SWITCH, checkbox_id]
  GI_CHECKBOX_SWITCH,
  
  // Launch an event for the previous menu and set the next event
  // format: [GI_HIGHLIGHT_EVENT, group]
  GI_HIGHLIGHT_EVENT,
  
  // ------------------------------------------------------------------------
  // Game events
  
  // move/fast move player in level
  // format: [GL_PLAYER_MOVE, x, y, diff_x, diff_y]
  //         [GL_PLAYER_MOVE_FAST, x, y, diff_x, diff_y]
  GL_PLAYER_MOVE,  
  GL_PLAYER_MOVE_FAST,
  
  // make other player active
  // format: [GL_PLAYER_SWITCH,  -1] - switch to another active player
  // format: [GL_PLAYER_SWITCH, num] - switch to player num
  GL_PLAYER_SWITCH,

  // ------------------------------------------------------------------------

  // change / create new object in level, erase old (if exist)
  // format: [GL_CELL_SET_ITEM, x, y, layer, new_item, rewrite_sub_items, change_sprite]
  GL_CELL_SET_ITEM,

  // change / set variation of item
  // format: [GL_CELL_SET_VARIATION, x, y, layer, variation, rewrite_subitems, change_sprite]
  GL_CELL_SET_VARIATION,

  // change / set rotation of items
  // format: [GL_CELL_SET_ITEM, x, y, layer, rotation, change_sprite]
  GL_CELL_SET_ROTATION,

  // change diff for cell
  // format: [GL_SET_DIFF, x, y, layer, diff_x, diff_y]
  GL_CELL_SET_DIFF,

  // erase cell in level, remove all sprites and so on
  // format: [GL_CELL_CLEAR_ALL, x, y, layer, change_sprite]
  GL_CELL_CLEAR_ALL,

  // Move cell in level
  // format: [GL_ITEM_MOVE, x, y, layer, nx, ny, change_sprite]
  GL_CELL_MOVE,

  // Set player position
  // format: [PL_PLAYER_SET_POS, x, y, player_num]
  PL_PLAYER_SET_POS,

  // ------------------------------------------------------------------------

  // Run defined animation to selected item
  // format: [AN_RUN, x, y, layer, animation, chained_animations, chained_events][next...][....]
  AN_RUN,
  
  // Run defined animation to selected item
  // format: [AN_GENERATE_AND_RUN, x, y, layer, animation_type, chained_animations, chained_events][next...][....]
  AN_GENERATE_AND_RUN,

  // erase animation from level (if exist)
  // format: [AN_STOP_AND_CLEAR, x, y, layer]
  AN_STOP_AND_CLEAR,

  // ------------------------------------------------------------------------
  // Game control events
  
  // play level
  // format: [GC_RUN_LEVEL_SELECT, level_number, x, y]
  GC_RUN_LEVEL_SELECT,
    
  // play level
  // format: [GC_RUN_LEVEL_SET]
  GC_RUN_LEVEL_SET,
  
  // play level
  // format: [GC_RUN_LEVEL_LINE, level_name]
  GC_RUN_LEVEL_LINE,

  // run editor
  // format: [GC_RUN_EDITOR]
  GC_RUN_EDITOR,
  
  // quit playing level
  // format: [GC_STOP_LEVEL, cheat, menu]
  GC_STOP_LEVEL,

  // play level with welcome screen
  // format: [GC_MENU_RUN_LEVEL, level_set]  
  GC_MENU_RUN_LEVEL,
  
  // end screen for a level
  // format: [GC_MENU_END_LEVEL]
  GC_MENU_END_LEVEL,
  
  // end screen for a custom level
  // format: [GC_MENU_END_LEVEL_CUSTOM]
  GC_MENU_END_LEVEL_CUSTOM,

  // end screen for whole episode
  // format: [GC_MENU_END_LEVEL_SET, level_set]
  GC_MENU_END_LEVEL_SET,
  
  // create a new profile with given name
  // format: [GC_MENU_PROFILE_CREATE, profile_name]
  GC_MENU_PROFILE_CREATE,

  // select from loaded profiles
  // format: [GC_MENU_PROFILE_SELECT, profile_number]
  GC_MENU_PROFILE_SELECT,
  
  // suspend/restore playing level
  // format: [GC_SUSPEND_LEVEL/GC_RESTORE_LEVEL]
  GC_SUSPEND_LEVEL,
  GC_RESTORE_LEVEL,
  
  // Restart current level
  // format: [GC_RESTART_LEVEL]
  GC_RESTART_LEVEL,

  // load/save level
  // format: [GC_SAVE_LEVEL/GC_LOAD_LEVEL]
  GC_SAVE_LEVEL,
  GC_LOAD_LEVEL,

  // messages in menu
  // format: [message ID, page]
  GC_MENU_START,
  GC_MENU_NEW_GAME,
  GC_MENU_PROFILES,
  GC_MENU_SETTINGS,
  GC_MENU_SETTINGS_FULSCREEN_SWITCH,
  GC_MENU_SETTINGS_DOUBLESIZE_SWITCH,
  GC_MENU_SETTINGS_SOUND_SWITCH,
  GC_MENU_SETTINGS_MUSIC_SWITCH,
  GC_MENU_LEVEL_HINT,
  GC_MENU_HELP,
  GC_MENU_HELP_KEYS,
  GC_MENU_HELP_RULES,
  GC_MENU_HELP_CREDIT,
  GC_MENU_IN_GAME,
  GC_MENU_QUIT,

  // ------------------------------------------------------------------------
  // Editor events
  
  // clear level
  // format: [ED_LEVEL_NEW]
  ED_LEVEL_NEW,
  
  // load whole level
  // format: [ED_LEVEL_LOAD, p_garg]
  ED_LEVEL_LOAD,
  ED_LEVEL_SAVE,
  ED_LEVEL_SAVE_AS,

  // Select layer
  // format: [ED_LEVEL_SELECT_LAYER, layer]
  ED_LEVEL_SELECT_LAYER,

  // set cursor in level area
  // format: [ED_LEVEL_SET_CURSOR,x,y,redraw]
  ED_LEVEL_SET_CURSOR,

  // selection by rectangle
  // format: [ED_LEVEL_RECTANGLE_SELECTION, fill]
  // fill - true/false
  ED_LEVEL_RECTANGLE_SELECTION,

  // insert item to level
  // format: [ED_LEVEL_DRAW_CURSOR_INSERT_ITEM]
  ED_LEVEL_DRAW_CURSOR_INSERT_ITEM,

  // rotate item in level
  // format: [ED_LEVEL_DRAW_CURSOR_ROTATE_ITEM, type, rotation]  
  ED_LEVEL_DRAW_CURSOR_ROTATE_ITEM,

  // variate item in level
  // format: [ED_LEVEL_DRAW_CURSOR_VARIATE_ITEM, type, variation]
  ED_LEVEL_DRAW_CURSOR_VARIATE_ITEM,
  
  // clear item from level
  // format: [ED_LEVEL_DRAW_CURSOR_CLEAR_ITEM]
  ED_LEVEL_DRAW_CURSOR_CLEAR_ITEM,

  // Change background
  // format: [ED_LEVEL_CHANGE_BACKGROUND]
  ED_LEVEL_CHANGE_BACKGROUND,

  ED_HELP,
  ED_QUIT,

  // rotate selected item
  // format: [ED_ROTATE_SELECTION]
  ED_ROTATE_SELECTION,

  // variate selected item
  // format: [ED_VARIATE_SELECTION]
  ED_VARIATE_SELECTION,

  // Shade level
  // format: [ED_LEVEL_SHADER]
  ED_LEVEL_SHADER,

  // Run level
  // format: [ED_LEVEL_RUN]
  ED_LEVEL_RUN,

  // level layer on/off  
  // format: [ED_LEVEL_LAYER, layer]
  ED_LEVEL_LAYER,

  // scroll panel with items
  // format: [ED_LEVEL_IPANEL_SCROLL, panel_handle, direction]
  ED_LEVEL_IPANEL_SCROLL,
  
  // scroll a panel selected by mouse cursor
  // format: [ED_LEVEL_MOUSE_PANEL_SCROLL, direction]
  ED_LEVEL_MOUSE_PANEL_SCROLL,
  
  // highlight an item in panel
  // format: [ED_LEVEL_IPANEL_SELECT, panel_handle, direction]
  ED_LEVEL_IPANEL_SELECT,
  
  // select an item in panel
  // format: [ED_LEVEL_IPANEL_SELECT_LOCK, panel_handle, direction]
  ED_LEVEL_IPANEL_SELECT_LOCK,
  
  // editor undo
  // format: [ED_UNDO]
  ED_UNDO,
    
  // editor redo
  // format: [ED_REDO]
  ED_REDO,
  
  // Last event, used for debugging
  EV_LAST,

} EVENT_TYPE;

#define EVENT_LAST        0
#define EVENT_FIRST       1

#define EV_PLAYER         0

#define PARAM_0           0
#define PARAM_1           1
#define PARAM_2           2
#define PARAM_3           3
#define PARAM_4           4
#define PARAM_5           5


#define PARAM_POSITION_X  PARAM_0
#define PARAM_POSITION_Y  PARAM_1
#define PARAM_LAYER       PARAM_2
#define PARAM_ANIMATION   PARAM_3
#define PARAM_NEXT_EVENTS PARAM_4
#define PARAM_PLAYER_NUM  PARAM_5
#define PARAM_ROTATION    PARAM_5

#define PARAM_GROUP       PARAM_0
#define PARAM_BLOCK       PARAM_1

#define PARAMS            6

typedef void * EVENT_PARAM_TYPE;

#define EVENT_TYPE_NONE   ((EVENT_PARAM_TYPE)(NULL))
#define ET(p)             ((EVENT_PARAM_TYPE)(p))
#define ET_INT(p)         ((EVENT_PARAM_TYPE)(INT_TO_POINTER(p)))

/* Everything
*/
typedef class level_event {

  static const char * translation_table[];

private:
  
  EVENT_TYPE        action;
  int               depend_events;
  EVENT_PARAM_TYPE  params[PARAMS];
  
public:

  void params_set(EVENT_PARAM_TYPE p0, 
                  EVENT_PARAM_TYPE p1 = EVENT_TYPE_NONE,
                  EVENT_PARAM_TYPE p2 = EVENT_TYPE_NONE,
                  EVENT_PARAM_TYPE p3 = EVENT_TYPE_NONE, 
                  EVENT_PARAM_TYPE p4 = EVENT_TYPE_NONE,
                  EVENT_PARAM_TYPE p5 = EVENT_TYPE_NONE)
  {
    params[0] = p0;
    params[1] = p1;
    params[2] = p2;
    params[3] = p3;
    params[4] = p4;
    params[5] = p5;
  }

  int param_int_get(int index)
  {
    assert(index < PARAMS);
    int *p_tmp = (int *)(params+index);
    return((int)(*p_tmp));
  }

  void * param_point_get(int index)
  {
    assert(index < PARAMS);
    return((void *)(params[index]));
  }

  EVENT_PARAM_TYPE param_get(int index)
  {
    assert(index < PARAMS);
    return(params[index]);
  }

  static const char * translate_event(EVENT_TYPE action);

  void dump(void)
  {
    bprintf("event = %s, params = [%d,%d,%d,%d,%d,%d]",
            translate_event(action),
            param_int_get(0),
            param_int_get(1),
            param_int_get(2),
            param_int_get(3),
            param_int_get(4),
            param_int_get(5));
  }

  void depends_add(int num)
  {
    depend_events += num;
  }
  int depends_get(void)
  {
    return(depend_events);
  }

  void action_set(EVENT_TYPE act)
  {
    action = act;
  }

  EVENT_TYPE action_get(void)
  {
    return(action);
  }

  bool valid(void)
  {
    return(action != EV_NONE);
  }

  bool used(void)
  {
    return(valid());
  }

  void clear(void)
  {
    action = EV_NONE;
    depend_events = 0;
    memset(params,0,sizeof(params));
  }

  const char * translate(void)
  {
    return(translation_table[action]);
  }

  void translate_test(void);

public:
  
  level_event(void)  
  {
    clear();
  }

  level_event(EVENT_TYPE  ev,
              EVENT_PARAM_TYPE p0 = EVENT_TYPE_NONE,
              EVENT_PARAM_TYPE p1 = EVENT_TYPE_NONE,
              EVENT_PARAM_TYPE p2 = EVENT_TYPE_NONE,
              EVENT_PARAM_TYPE p3 = EVENT_TYPE_NONE, 
              EVENT_PARAM_TYPE p4 = EVENT_TYPE_NONE,
              EVENT_PARAM_TYPE p5 = EVENT_TYPE_NONE)
  : action(ev), depend_events(0)
  {
    params_set(p0,p1,p2,p3,p4,p5);
  }

  level_event(EVENT_TYPE  ev,
              int p0,
              int p1 = 0,
              int p2 = 0,
              int p3 = 0, 
              int p4 = 0,
              int p5 = 0)
  : action(ev), depend_events(0)
  {
    params_set(ET(INT_TO_POINTER(p0)),
               ET(INT_TO_POINTER(p1)),
               ET(INT_TO_POINTER(p2)),
               ET(INT_TO_POINTER(p3)),
               ET(INT_TO_POINTER(p4)),
               ET(INT_TO_POINTER(p5)));
  }

} LEVEL_EVENT;

#define EXTEND_STEP 1000

typedef class event_stream {
 
  int          max_events;   // the lenght of queue
  LEVEL_EVENT *p_queue;      // read queue

  int          read_first;
  int          read_next;

  int          write_first;
  int          write_next;

public:


  // ----------------------------------------------------
  // basic operations
  // ----------------------------------------------------
  int index_next(int index)
  {    
    return ++index < max_events ? index : index = 0;
  }
  int index_prev(int index)
  {    
    return --index < 0 ? index = max_events -1 : index;
  }

  bool in_range(int first, int last, int index)
  {
    if(first < last) { // normal order
      return(index >= first && index <= last);
    } else {           // flipped order
      return(index >= first || index <= last);
    }    
  }

  void dump(const char *p_desc, int first, int next)
  {
    int akt;
  
    if(!empty(first,next)) {
      bprintf(p_desc);
      for(akt = first; akt != next; akt = index_next(akt)) {
        p_queue[akt].dump();
      }
    }
  }

  void dump_read(const char *p_desc)
  {
    dump(p_desc,read_first,read_next);
  }
  void dump_write(const char *p_desc)
  {
    dump(p_desc,write_first,write_next);
  }

  bool empty(int first, int next)
  {
    return(first == next);
  }
  bool full(int first, int next)
  {
    return(first == index_next(next));
  }

  bool empty(void)
  {
    return(empty(read_first, read_next));
  }
  bool full(void)
  {
    return(full(read_first, write_next));
  }

  // ----------------------------------------------------
  // Read stream
  // ----------------------------------------------------

  LEVEL_EVENT get(void)
  {
    assert(!empty());
  
    int tmp_index = read_first;
    read_first = index_next(read_first);
    return p_queue[tmp_index];
  }

  // ----------------------------------------------------
  // Write stream
  // ----------------------------------------------------

  void add(LEVEL_EVENT event, bool safe = TRUE)
  {
    if(!full()) {
      if(event.valid()) {        
        p_queue[write_next] = event;
        write_next = index_next(write_next);
      }
    }
    else if(safe)
      assert(0);
  }

  #define DIRECTION_ARRAY   1
  #define DIRECTION_STACK (-1)

  int add(LEVEL_EVENT *p_event, int num = 1, int direction = 1,
          bool safe = TRUE)
  {
    assert(p_event);
   
    int i;

    for(i = 0; i < num; i++, p_event += direction) {
      if(full()) {
        if(safe)
          assert(0);
        else
          return(num);
      } else if(p_event->valid()) {
        p_queue[write_next] = *p_event;
        write_next = index_next(write_next);
      }
    }
  
    return(num);
  }

  void add(class event_stream *p_stream, bool safe = TRUE)
  {  
    if(!p_stream || p_stream->empty())
      return;
        
    while(!p_stream->empty()) {
      if(!full()) {
        LEVEL_EVENT ev = p_stream->get();
        if(ev.valid())
          add(ev);
      }
      else if(safe)
        assert(0);
    }
  }

  // move all items from write stream to read stream
  // empty the write stream
  // 
  void commit(void)
  {
    read_next = write_first = write_next;
  }

  // ----------------------------------------------------
  // Initialization & creation
  // ----------------------------------------------------
  void clear(void)
  {
    read_first = read_next = write_first = write_next = 0;
  }

  void extend(int step)
  {
    if(!p_queue) {
      max_events = step;
      p_queue = new LEVEL_EVENT[step];
    } else {
    
      commit();
    
      int min = read_first;
      int max = read_next;
    
      LEVEL_EVENT *p_tmp = new LEVEL_EVENT[max_events+step];
    
      if(!empty(read_first, read_next)) {
        int akt,i;        
      
        for(i = 0, akt = min; akt != max; i++, akt = index_next(akt)) {
          p_tmp[i++] = p_queue[akt];
        }
      
        read_first = 0;
        write_first = write_next = read_next = max;
      } else {      
        clear();
      }
    
      delete [] p_queue;
      p_queue = p_tmp;

      max_events += step;
    }
  }

  event_stream(void) : max_events(0), p_queue(NULL)
  {    
    // Init and test the created event queue
    testcase();
  }

  ~event_stream(void)
  {
    max_events = 0;
    if(p_queue)
      delete [] p_queue;
    clear();
  }

  void testcase(void);

} EVENT_STREAM;

typedef EVENT_STREAM LEVEL_EVENT_QUEUE;

#endif // __EVENTS_H__
