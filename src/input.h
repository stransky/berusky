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

/* Interface for keyboard/mouse/everything
*/

#ifndef __INPUT_H__
#define __INPUT_H__

/* Key definition - GTK */
/*
#include <gdk/gdkkeysyms.h>

  #define  KEYNUM    0xff
  
  #define  K_ESC     GDK_Escape

  #define  K_F1      GDK_F1
  #define  K_F2      GDK_F1
  #define  K_F3      GDK_F3
  #define  K_F4      GDK_F4
  #define  K_F5      GDK_F5
  #define  K_F6      GDK_F6
  #define  K_F7      GDK_F7
  #define  K_F8      GDK_F8
  #define  K_F9      GDK_F9
  #define  K_F10     GDK_F10
  #define  K_F11     GDK_F11
  #define  K_F12     GDK_F12
  
  #define  K_1       GDK_1
  #define  K_2       GDK_2
  #define  K_3       GDK_3
  #define  K_4       GDK_4
  #define  K_5       GDK_5
  #define  K_6       GDK_6
  #define  K_7       GDK_7
  #define  K_8       GDK_8
  #define  K_9       GDK_9
  #define  K_0       GDK_0
    
  #define  K_A       GDK_a
  #define  K_B       GDK_b
  #define  K_C       GDK_c
  #define  K_D       GDK_d
  #define  K_E       GDK_e
  #define  K_F       GDK_f
  #define  K_G       GDK_g
  #define  K_H       GDK_h
  #define  K_I       GDK_i
  #define  K_J       GDK_j
  #define  K_K       GDK_k
  #define  K_L       GDK_l
  #define  K_M       GDK_m
  #define  K_N       GDK_n
  #define  K_O       GDK_o
  #define  K_P       GDK_p
  #define  K_Q       GDK_q
  #define  K_R       GDK_r
  #define  K_S       GDK_s
  #define  K_T       GDK_t
  #define  K_U       GDK_u
  #define  K_V       GDK_v
  #define  K_W       GDK_w
  #define  K_X       GDK_x
  #define  K_Y       GDK_y
  #define  K_Z       GDK_z

  #define  K_MINUS   GDK_minus
  #define  K_PLUS    GDK_plus

  #define  K_BKSP    GDK_BackSpace
  #define  K_TAB     GDK_Tab
  #define  K_ENTER   GDK_Return
  
  #define  K_BRACKET_L SDLK_LEFTBRACKET
  #define  K_BRACKET_R SDLK_RIGHTBRACKET
  
  #define  K_SEMICOL   SDLK_SEMICOLON
  #define  K_QUOTE     SDLK_QUOTE
  #define  K_TILDA     
  #define  K_BACKSLASH SDLK_BACKSLASH
  #define  K_COMMA     SDLK_COMMA
  #define  K_PERIOD    SDLK_PERIOD
  #define  K_SLASH     SDLK_SLASH
  #define  K_SPACE     SDLK_SPACE
  
  #define  K_UP        GDK_Up
  #define  K_LEFT      GDK_Left
  #define  K_RIGHT     GDK_Right
  #define  K_DOWN      GDK_Down

  #define  K_HOME      SDLK_HOME
  #define  K_PGUP      SDLK_PAGEUP
  #define  K_END       SDLK_END
  #define  K_PGDN      SDLK_PAGEDOWN
  #define  K_INSERT    SDLK_INSERT

  #define  K_DEL       GDK_Delete
  
  #define  K_SHIFT_MASK GDK_SHIFT_MASK
  #define  K_CTRL_MASK  GDK_CONTROL_MASK


  typedef guint KEYTYPE;
  typedef guint KEYMOD;

#endif
*/

/* Key definition - SDL */

#define  KEYNUM    0xff

#define  K_ESC     SDLK_ESCAPE

#define  K_F1      SDLK_F1
#define  K_F2      SDLK_F2
#define  K_F3      SDLK_F3
#define  K_F4      SDLK_F4
#define  K_F5      SDLK_F5
#define  K_F6      SDLK_F6
#define  K_F7      SDLK_F7
#define  K_F8      SDLK_F8
#define  K_F9      SDLK_F9
#define  K_F10     SDLK_F10
#define  K_F11     SDLK_F11
#define  K_F12     SDLK_F12

#define  K_1       SDLK_1
#define  K_2       SDLK_2
#define  K_3       SDLK_3
#define  K_4       SDLK_4
#define  K_5       SDLK_5
#define  K_6       SDLK_6
#define  K_7       SDLK_7
#define  K_8       SDLK_8
#define  K_9       SDLK_9
#define  K_0       SDLK_0

#define  KP_0      SDLK_KP0
#define  KP_1      SDLK_KP1
#define  KP_2      SDLK_KP2
#define  KP_3      SDLK_KP3
#define  KP_4      SDLK_KP4
#define  KP_5      SDLK_KP5
#define  KP_6      SDLK_KP6
#define  KP_7      SDLK_KP7
#define  KP_8      SDLK_KP8
#define  KP_9      SDLK_KP9

#define  K_A       SDLK_a
#define  K_B       SDLK_b
#define  K_C       SDLK_c
#define  K_D       SDLK_d
#define  K_E       SDLK_e
#define  K_F       SDLK_f
#define  K_G       SDLK_g
#define  K_H       SDLK_h
#define  K_I       SDLK_i
#define  K_J       SDLK_j
#define  K_K       SDLK_k
#define  K_L       SDLK_l
#define  K_M       SDLK_m
#define  K_N       SDLK_n
#define  K_O       SDLK_o
#define  K_P       SDLK_p
#define  K_Q       SDLK_q
#define  K_R       SDLK_r
#define  K_S       SDLK_s
#define  K_T       SDLK_t
#define  K_U       SDLK_u
#define  K_V       SDLK_v
#define  K_W       SDLK_w
#define  K_X       SDLK_x
#define  K_Y       SDLK_y
#define  K_Z       SDLK_z

#define  K_MINUS   SDLK_MINUS
#define  K_PLUS    SDLK_PLUS
#define  K_BKSP    SDLK_BACKSPACE
#define  K_TAB     SDLK_TAB

#define  K_BRACKET_L  SDLK_LEFTBRACKET
#define  K_BRACKET_R  SDLK_RIGHTBRACKET
#define  K_ENTER      SDLK_RETURN

#define  K_SEMICOL    SDLK_SEMICOLON
#define  K_QUOTE      SDLK_QUOTE
#define  K_TILDA     
#define  K_BACKSLASH  SDLK_BACKSLASH
#define  K_COMMA      SDLK_COMMA
#define  K_PERIOD     SDLK_PERIOD
#define  K_SLASH      SDLK_SLASH
#define  K_SPACE      SDLK_SPACE

#define  K_UP         SDLK_UP
#define  K_LEFT       SDLK_LEFT
#define  K_RIGHT      SDLK_RIGHT
#define  K_DOWN       SDLK_DOWN

#define  K_HOME       SDLK_HOME
#define  K_PGUP       SDLK_PAGEUP
#define  K_END        SDLK_END
#define  K_PGDN       SDLK_PAGEDOWN
#define  K_INSERT     SDLK_INSERT
#define  K_DEL        SDLK_DELETE

#define  K_SHIFT_MASK KMOD_SHIFT
#define  K_CTRL_MASK  KMOD_CTRL

typedef  SDLKey       KEYTYPE;
typedef  SDLMod       KEYMOD;
 

#define  KEY_PRESSED                  0x1     // key is pressed
#define  KEY_CLEAR_AFTER_PRESS        0x2     // clear key after press
#define  KEY_GROUP_BLOCK              0x8     // Key is member of group

#define  KEY_GROUP_BLOCK_MOVE         0x100

inline bool in_rect(RECT &dst, tpos x, tpos y)
{
  return(x >= dst.x && y >= dst.y && x < dst.x+dst.w && y < dst.y+dst.h);
}

inline bool in_rect(RECT &dst, RECT &src)
{
  return(src.x >= dst.x && src.y >= dst.y && 
         src.x < dst.x+dst.w && src.y < dst.y+dst.h);
}

// ------------------------------------------------------------
// Game input info
// ------------------------------------------------------------

typedef struct event_key {

  LEVEL_EVENT e1;
  LEVEL_EVENT e2;

  KEYTYPE     key;      // active key

  bool        alt;
  bool        ctrl;
  bool        shift;

  int         flag;
  int         group;    // group flag

} EVENT_KEY;

typedef struct event_key_set {

  EVENT_KEY *p_keys;
  int        keynum;

} EVENT_KEY_SET;

// -------------------------------------------------------
// Mouse UI
// -------------------------------------------------------

#define NO_BUTTON             0
#define MOUSE_BUTTONS         6

#define BUTTON_LEFT           1
#define BUTTON_MIDDLE         2
#define BUTTON_RIGHT          3
#define WHEEL_UP              4
#define WHEEL_DOWN            5

#define MASK_BUTTON_LEFT      0x02
#define MASK_BUTTON_MIDDLE    0x04
#define MASK_BUTTON_RIGHT     0x08
#define MASK_WHEEL_UP         0x10
#define MASK_WHEEL_DOWN       0x20

typedef enum { 

  BUTTON_NONE = 0,
  BUTTON_DOWN,
  BUTTON_UP,

} MOUSE_BUTTON_STATE;


typedef class mouse_state {

public:

  RECT                rect;
  MOUSE_BUTTON_STATE  button[MOUSE_BUTTONS];
  int                 key;

public:
  
  bool in_rect(RECT &dst)
  {
    return(::in_rect(dst, rect.x, rect.y));
  }

public:

  mouse_state(void)
  {
    memset(this,0,sizeof(*this));
  }

  mouse_state(RECT r)
  {
    rect = r;
    memset(button,0,sizeof(button[0])*MOUSE_BUTTONS);
  }

  mouse_state(RECT r, int buttons, int key_ = 0)
  {
    rect = r;
  
    int i;
    for(i = 0; i < MOUSE_BUTTONS; i++)
      button[i] = (0x1&(buttons >> i)) ? BUTTON_DOWN : BUTTON_NONE;
    
    key = key_;
  }

  mouse_state(RECT r, int buttons, MOUSE_BUTTON_STATE state)
  {
    rect = r;
    int i;
    for(i = 0; i < MOUSE_BUTTONS; i++)
      button[i] = (0x1&(buttons >> i)) ? state : BUTTON_NONE;
  }

} MOUSE_STATE;

// Activators of events
#define MEVENT_MOUSE_BUTTONS          0x01
#define MEVENT_MOUSE_IN               0x02
#define MEVENT_MOUSE_OUT              0x04
#define MEVENT_KEY                    0x08

// Modificators of events
#define MEVENT_ACTIVATE_ONCE          0x10

// Run external evet instead of the given one(s)
#define MEVENT_MOUSE_EXTERNAL         0x20

#define MEVENTS                       3

typedef struct mouse_event : public llist_item 
{
  
  MOUSE_STATE        mstate;
  int                flag;
  int                event_num;
  LEVEL_EVENT        event[MEVENTS];
  bool               last_state;

  mouse_event(void)
  {
  }

  mouse_event(MOUSE_STATE state, int flg, int handle)
  {
    mstate = state;
    flag = flg;    
    last_state = FALSE;
    event_num = 0;
    event[0].params_set(ET(INT_TO_POINTER(handle)));
  }

  mouse_event(MOUSE_STATE state, int flg, LEVEL_EVENT ev)
  {
    mstate = state;
    flag = flg;
    event[0] = ev;
    event_num = 1;
    last_state = FALSE;
  }

  mouse_event(MOUSE_STATE state, int flg, LEVEL_EVENT ev, LEVEL_EVENT ev1)
  {
    mstate = state;
    flag = flg;
    event[0] = ev;
    event[1] = ev1;
    event_num = 2;
    last_state = FALSE;
  }

  mouse_event(MOUSE_STATE state, int flg, LEVEL_EVENT ev, LEVEL_EVENT ev1, LEVEL_EVENT ev2)
  {    
    mstate = state;
    flag = flg;
    event[0] = ev;
    event[1] = ev1;
    event[2] = ev2;
    event_num = 3;
    last_state = FALSE;
  }

  void state_clear(void)
  {
    last_state = FALSE;
  }

} MOUSE_EVENT;

// ------------------------------------------------------------
// Basic game input class
// ------------------------------------------------------------
#define INPUT_BLOCK_SETS       0x1   // Block all key-sets (is captured by console)                                       
#define INPUT_EVENT_LOOP_WAIT  0x2   // wait for events

typedef class input {  

  LEVEL_EVENT_QUEUE input_queue;     // All input events come there

  EVENT_KEY_SET    *p_set;           // Active event-set for keys
  int               group;           // global group flag

  MOUSE_STATE       mstate;          // Last mouse state
/*
  MOUSE_EVENT       mevents[MAX_MOUSE_EVENTS]; // Active mouse events
  int               mevents_num;
*/
  LLIST_HEAD        mevents;

  int               flag;            // current input-interface flags

private:

  void key_block(int group_mask, bool block);

  void events_game(LEVEL_EVENT_QUEUE *p_queue);
  
public:
  
  input(void) : p_set(NULL), group(0), flag(0) {};

  bool key_status(int sdl_key);
  void key_repeat(bool state);

  // Block/Unblock all input
  void block(bool state);
  
  // General interface for all events  
  void events_loop(LEVEL_EVENT_QUEUE *p_queue);
  void events_wait(bool state);
  
  // Keyboard interface
  void keyset_set(EVENT_KEY_SET *p_keyset);
  void key_input(KEYTYPE key, KEYMOD modification, bool pressed);
  void key_add(LEVEL_EVENT_QUEUE *p_queue);

  // Mouse interface
  void mouse_input(tpos mx, tpos my, MOUSE_BUTTON_STATE state, int button);

  void mevent_state_clear(MOUSE_EVENT *p_first = NULL);
  void mevent_clear(void);
  
  MOUSE_EVENT * mevent_add(MOUSE_EVENT event);
  MOUSE_EVENT * mevent_add(MOUSE_EVENT *p_event, int num);
  void          mevent_remove(MOUSE_EVENT *p_first, int num);
  
  LLIST_HEAD  * mevents_get(void)
  {
    return(&mevents);
  }
  
  MOUSE_STATE * mouse_state_get(void)
  {
    return(&mstate);
  }

} INPUT;

extern EVENT_KEY_SET game_keys;
extern EVENT_KEY_SET suspend_keys;
extern EVENT_KEY_SET menu_keys;
extern EVENT_KEY_SET editor_keys;

bool key_to_ascii(int key, char *p_char);

#endif // __INPUT_H__
