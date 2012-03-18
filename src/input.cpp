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

/* Global input interface
*/
#include "berusky.h" 
#include "berusky_gui.h"
#include "editor.h"

/* KB definitions
*/
EVENT_KEY game_key_array[] = 
{/*
  {LEVEL_EVENT(GL_PLAYER_MOVE, 0,-1),LEVEL_EVENT(EV_NONE), K_UP,   0,0,0,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},
  {LEVEL_EVENT(GL_PLAYER_MOVE, 0, 1),LEVEL_EVENT(EV_NONE), K_DOWN, 0,0,0,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},
  {LEVEL_EVENT(GL_PLAYER_MOVE,-1, 0),LEVEL_EVENT(EV_NONE), K_LEFT, 0,0,0,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},
  {LEVEL_EVENT(GL_PLAYER_MOVE, 1, 0),LEVEL_EVENT(EV_NONE), K_RIGHT,0,0,0,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},

  {LEVEL_EVENT(GL_PLAYER_MOVE_FAST, 0,-1),LEVEL_EVENT(EV_NONE), K_UP,   0,0,1,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},
  {LEVEL_EVENT(GL_PLAYER_MOVE_FAST, 0, 1),LEVEL_EVENT(EV_NONE), K_DOWN, 0,0,1,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},
  {LEVEL_EVENT(GL_PLAYER_MOVE_FAST,-1, 0),LEVEL_EVENT(EV_NONE), K_LEFT, 0,0,1,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},
  {LEVEL_EVENT(GL_PLAYER_MOVE_FAST, 1, 0),LEVEL_EVENT(EV_NONE), K_RIGHT,0,0,1,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},
*/
  {LEVEL_EVENT(GL_PLAYER_MOVE_FAST, 0,-1),LEVEL_EVENT(EV_NONE), K_UP,   0,0,0,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},
  {LEVEL_EVENT(GL_PLAYER_MOVE_FAST, 0, 1),LEVEL_EVENT(EV_NONE), K_DOWN, 0,0,0,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},
  {LEVEL_EVENT(GL_PLAYER_MOVE_FAST,-1, 0),LEVEL_EVENT(EV_NONE), K_LEFT, 0,0,0,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},
  {LEVEL_EVENT(GL_PLAYER_MOVE_FAST, 1, 0),LEVEL_EVENT(EV_NONE), K_RIGHT,0,0,0,KEY_GROUP_BLOCK, KEY_GROUP_BLOCK_MOVE},

  {LEVEL_EVENT(GL_PLAYER_SWITCH, SELECT_PLAYER_NEXT, 0),LEVEL_EVENT(EV_NONE), K_TAB, 0,0,0,KEY_CLEAR_AFTER_PRESS},

  {LEVEL_EVENT(GL_PLAYER_SWITCH,  0, 0),LEVEL_EVENT(EV_NONE), K_1, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GL_PLAYER_SWITCH,  1, 0),LEVEL_EVENT(EV_NONE), K_2, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GL_PLAYER_SWITCH,  2, 0),LEVEL_EVENT(EV_NONE), K_3, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GL_PLAYER_SWITCH,  3, 0),LEVEL_EVENT(EV_NONE), K_4, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GL_PLAYER_SWITCH,  4, 0),LEVEL_EVENT(EV_NONE), K_5, 0,0,0,KEY_CLEAR_AFTER_PRESS},

  {LEVEL_EVENT(GC_SUSPEND_LEVEL),LEVEL_EVENT(GC_MENU_IN_GAME),   K_ESC, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_SUSPEND_LEVEL),LEVEL_EVENT(GC_MENU_HELP,TRUE), K_F1, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_SUSPEND_LEVEL),LEVEL_EVENT(GC_MENU_LEVEL_HINT,TRUE), K_F1, 0,1,0,KEY_CLEAR_AFTER_PRESS},

  {LEVEL_EVENT(GC_SUSPEND_LEVEL),LEVEL_EVENT(EV_NONE),K_S, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_RESTART_LEVEL),LEVEL_EVENT(EV_NONE),K_R, 0,1,0,KEY_CLEAR_AFTER_PRESS},

  {LEVEL_EVENT(GC_SAVE_LEVEL),LEVEL_EVENT(GC_RESTORE_LEVEL),K_F2, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_LOAD_LEVEL),LEVEL_EVENT(GC_RESTORE_LEVEL),K_F3, 0,0,0,KEY_CLEAR_AFTER_PRESS},

  {LEVEL_EVENT(GC_MENU_QUIT),LEVEL_EVENT(EV_NONE), K_X, 1,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_MENU_QUIT),LEVEL_EVENT(EV_NONE), K_X, 0,1,0,KEY_CLEAR_AFTER_PRESS},

  {LEVEL_EVENT(GC_STOP_LEVEL, TRUE, TRUE),LEVEL_EVENT(EV_NONE), K_E, 0,1,0,KEY_CLEAR_AFTER_PRESS}
};

EVENT_KEY_SET game_keys = { game_key_array , sizeof(game_key_array) / sizeof(game_key_array[0]) };

/* KB definitions
*/
EVENT_KEY suspend_key_array[] = 
{
  {LEVEL_EVENT(GC_RESTORE_LEVEL),LEVEL_EVENT(EV_NONE), K_ESC, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_RESTORE_LEVEL),LEVEL_EVENT(EV_NONE), K_R, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_RESTART_LEVEL),LEVEL_EVENT(EV_NONE), K_R, 0,1,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_MENU_QUIT),    LEVEL_EVENT(EV_NONE), K_X, 1,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_MENU_QUIT),    LEVEL_EVENT(EV_NONE), K_X, 0,1,0,KEY_CLEAR_AFTER_PRESS}
};

EVENT_KEY_SET suspend_keys = { suspend_key_array, sizeof(suspend_key_array) / sizeof(suspend_key_array[0]) };

/* KB definitions
*/
EVENT_KEY menu_key_array[] = 
{
  {LEVEL_EVENT(GC_MENU_START), LEVEL_EVENT(EV_NONE),K_ESC,   0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_MENU_START), LEVEL_EVENT(EV_NONE),K_ENTER, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_MENU_START), LEVEL_EVENT(EV_NONE),K_SPACE, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_MENU_QUIT),  LEVEL_EVENT(EV_NONE),K_X,     1,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(GC_MENU_QUIT),  LEVEL_EVENT(EV_NONE),K_X,     0,1,0,KEY_CLEAR_AFTER_PRESS}
};

EVENT_KEY_SET menu_keys = { menu_key_array, sizeof(menu_key_array) / sizeof(menu_key_array[0]) };

/* KB definitions
*/
EVENT_KEY editor_key_array[] = 
{
  {LEVEL_EVENT(GC_MENU_QUIT),                               LEVEL_EVENT(EV_NONE),K_ESC, 0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_LAYER,LAYER_GRID,LAYER_CHANGE),     LEVEL_EVENT(EV_NONE),K_1,   0,1,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_LAYER,LAYER_FLOOR,LAYER_CHANGE),    LEVEL_EVENT(EV_NONE),K_2,   0,1,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_LAYER,LAYER_ITEMS,LAYER_CHANGE),    LEVEL_EVENT(EV_NONE),K_3,   0,1,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_LAYER,LAYER_PLAYER,LAYER_CHANGE),   LEVEL_EVENT(EV_NONE),K_4,   0,1,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_HELP),                                    LEVEL_EVENT(EV_NONE),K_F1,  0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_NEW),                               LEVEL_EVENT(EV_NONE),K_N,   0,1,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_SAVE),                              LEVEL_EVENT(EV_NONE),K_F2,  0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_SAVE_AS),                           LEVEL_EVENT(EV_NONE),K_F2,  0,1,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_LOAD),                              LEVEL_EVENT(EV_NONE),K_F3,  0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_UNDO),                                    LEVEL_EVENT(EV_NONE),K_U,   0,1,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_REDO),                                    LEVEL_EVENT(EV_NONE),K_R,   0,1,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_RUN),                               LEVEL_EVENT(EV_NONE),K_F9,  0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_SHADER),                            LEVEL_EVENT(EV_NONE),K_S,   0,1,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_ROTATE_SELECTION),                        LEVEL_EVENT(EV_NONE),K_R,   0,0,1,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_RECTANGLE_SELECTION,FALSE),         LEVEL_EVENT(EV_NONE),K_D,   0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_RECTANGLE_SELECTION,TRUE),          LEVEL_EVENT(EV_NONE),K_F,   0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_SELECT_LAYER,LAYER_FLOOR),          LEVEL_EVENT(EV_NONE),K_1,   0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_SELECT_LAYER,LAYER_ITEMS),          LEVEL_EVENT(EV_NONE),K_2,   0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_SELECT_LAYER,LAYER_PLAYER),         LEVEL_EVENT(EV_NONE),K_3,   0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_SELECT_LAYER,ALL_LEVEL_LAYERS),     LEVEL_EVENT(EV_NONE),K_4,   0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_CHANGE_BACKGROUND),                 LEVEL_EVENT(EV_NONE),K_B,   0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_MOUSE_PANEL_SCROLL,-ITEMS_IN_PANEL),LEVEL_EVENT(EV_NONE),K_PGUP,0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_MOUSE_PANEL_SCROLL,ITEMS_IN_PANEL), LEVEL_EVENT(EV_NONE),K_PGDN,0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_MOUSE_PANEL_SCROLL,ITEMS_START),    LEVEL_EVENT(EV_NONE),K_HOME,0,0,0,KEY_CLEAR_AFTER_PRESS},
  {LEVEL_EVENT(ED_LEVEL_MOUSE_PANEL_SCROLL,ITEMS_END),      LEVEL_EVENT(EV_NONE),K_END, 0,0,0,KEY_CLEAR_AFTER_PRESS},

  //{LEVEL_EVENT(EV_TEST),                                  LEVEL_EVENT(EV_NONE),K_A, 0,0,0,KEY_CLEAR_AFTER_PRESS},
};

EVENT_KEY_SET editor_keys = { editor_key_array, sizeof(editor_key_array) / sizeof(editor_key_array[0]) };

bool key_to_ascii(int key, char *p_char)
{
  if(key < 127) {
    *p_char = key;
    return(TRUE);
  }
  else {
    return(FALSE);
  }
}

// -------------------------------------------------------------------------
// Keyboard interface
// -------------------------------------------------------------------------
bool input::key_status(int sdl_key)
{
  Uint8 *keystate = SDL_GetKeyState(NULL);
  return(keystate[sdl_key]);
}

void input::keyset_set(EVENT_KEY_SET *p_keyset)
{
  p_set = p_keyset;

  if(p_set) {
    EVENT_KEY *p_key = p_set->p_keys;
    int        keynum = p_set->keynum;
    int        i;
  
    for(i = 0; i < keynum; i++, p_key++) {
      p_key->flag = p_key->flag&~KEY_PRESSED;      
    }
  }
}

// group_mask == 0 -> mask all groups
void input::key_block(int group_mask, bool block)
{ 
  if(!group_mask)
    group_mask = ~group_mask;
  group = block ? group | group_mask : group & (~group_mask);
}

void input::key_add(LEVEL_EVENT_QUEUE *p_queue)
{
  // check evets-list
  if(!p_set)
    return;
 
  EVENT_KEY *p_key = p_set->p_keys;
  int        keynum = p_set->keynum;
  int        i;
  
  for(i = 0; i < keynum; i++, p_key++) {  
    // check group - is already present?
    if(p_key->flag&KEY_PRESSED && !(p_key->flag&KEY_GROUP_BLOCK && p_key->group&group)) {
      // okay, add an event      
      p_queue->add(LEVEL_EVENT(p_key->e1));
      if(p_key->e2.valid())
        p_queue->add(LEVEL_EVENT(p_key->e2));
      
      if(p_key->flag&KEY_GROUP_BLOCK)
        group |= p_key->group;

      if(p_key->flag&KEY_CLEAR_AFTER_PRESS)
        p_key->flag &= ~KEY_PRESSED;
    }
  }
  p_queue->commit();
}


void input::events_game(LEVEL_EVENT_QUEUE *p_queue)
{
  while(!p_queue->empty()) {
    
    LEVEL_EVENT ev = p_queue->get();
  
    if(ev.action_get() == GI_BLOCK_KEYS) {
      // format: [UI_BLOCK_KEYS, key_group, blocked]
      key_block(ev.param_int_get(PARAM_GROUP), ev.param_int_get(PARAM_BLOCK));
    } else if(ev.action_get() == GI_UNBLOCK_ALL) {
      key_block(0, FALSE);
    } else {
      p_queue->add(ev);
    }
  }

  p_queue->commit();
}

void input::key_input(KEYTYPE key, KEYMOD mod, bool pressed)
{
  // check input queue  
  if(pressed) {
    input_queue.add(LEVEL_EVENT(GI_KEY_DOWN, (int)key));
    input_queue.commit();
  }  

  // check evets-list
  if(!p_set || flag&INPUT_BLOCK_SETS)
    return;
 
  EVENT_KEY *p_key = p_set->p_keys;
  int        keynum = p_set->keynum;
  int        i;

  bool shift = mod&K_SHIFT_MASK;
  bool ctrl = mod&K_CTRL_MASK;
  
 // Update key flag
  for(i = 0; i < keynum; i++, p_key++) {
    if(p_key->key == key && ctrl == p_key->ctrl && shift == p_key->shift) {
      p_key->flag = pressed ? p_key->flag|KEY_PRESSED : p_key->flag&~KEY_PRESSED;
    }
  }
}

void input::events_loop(LEVEL_EVENT_QUEUE *p_queue)
{
  // Queue where are stored events captured from KB/Mouse/...
  SDL_Event event;
  int ret;

  // Clear input queue
  input_queue.clear();

  // Loop until there are no SDL events left on the queue    
  if(flag&INPUT_EVENT_LOOP_WAIT) {
    ret = SDL_WaitEvent(&event);    
  }
  else {
    ret = SDL_PollEvent(&event);
  }
    
  while(ret) {
    switch (event.type) {
      case SDL_KEYDOWN:
        key_input(event.key.keysym.sym, event.key.keysym.mod, TRUE);
        break;
      case SDL_KEYUP:
        key_input(event.key.keysym.sym, event.key.keysym.mod, FALSE);
        break;
      case SDL_MOUSEMOTION:
        {
          bool pressed = FALSE;
          int  i;
          for(i = 0; i < MOUSE_BUTTONS; i++) {        
            bool state = event.motion.state&SDL_BUTTON(i);
            if(state) {
              mouse_input(event.motion.x, event.motion.y, 1, i);
              pressed = TRUE;
            }
          }
          if(!pressed) {
            mouse_input(event.motion.x, event.motion.y, 0, 0);
          }
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        mouse_input(event.button.x, event.button.y, 1, event.button.button);
        break;
      case SDL_MOUSEBUTTONUP:
        mouse_input(event.button.x, event.button.y, 0, event.button.button);
        break;
      case SDL_ACTIVEEVENT:
        if(event.active.state&SDL_APPACTIVE) {
          if(event.active.gain) {
            bprintf("App activated\n");
          } else {
            bprintf("App iconified\n");
          }
        }
        break;
      case SDL_QUIT:        
        p_queue->add(LEVEL_EVENT(GC_MENU_QUIT));
        p_queue->commit();
        return;
      default:
        break;
    }
  
    ret = SDL_PollEvent(&event);
  }  

  // Add all input events
  p_queue->add(&input_queue);
  p_queue->commit();

  // Add events from key-sets
  events_game(p_queue);
  key_add(p_queue);
}

// -------------------------------------------------------------------------
// Mouse interface
// -------------------------------------------------------------------------

void input::mouse_input(tpos mx, tpos my, int state, int button)
{
  // bprintf("Mouse state [%d,%d], state %d, button %d", mx, my, state, button);

  /* Save recent mouse state */
  mstate.rect.x = mx;
  mstate.rect.y = my;

  if(flag&INPUT_BLOCK_SETS)
    return;

  if(state != NO_BUTTON && button < MOUSE_BUTTONS) {
    mstate.button[button] = state;
  }

  /* Process all events */
  if(mevents_num) {  
    Uint8 *p_keystate = SDL_GetKeyState(NULL);
  
    int i;
    for(i = 0; i < mevents_num; i++) {
      bool cond_button = TRUE;
      bool cond_key = TRUE;
      bool cond_area = FALSE;
      bool active;
    
      MOUSE_EVENT *p_ev = mevents+i;    

      if(p_ev->flag&MEVENT_MOUSE_BUTTONS) {
        int j;
        for(j = 0; j < MOUSE_BUTTONS; j++) {
          /* button is required and pressed - process it */
          if(mstate.button[j] && mstate.button[j] == p_ev->mstate.button[j]) {
            break;
          }          
        }
        cond_button = (j != MOUSE_BUTTONS);
      }
    
      if(p_ev->flag&MEVENT_MOUSE_IN) {
        cond_area = in_rect(p_ev->mstate.rect,mx,my);
      } else if(p_ev->flag&MEVENT_MOUSE_OUT) {
        cond_area = !in_rect(p_ev->mstate.rect,mx,my);
      }
    
      if(p_ev->flag&MEVENT_KEY) {
        cond_key = p_ev->mstate.key && p_keystate[p_ev->mstate.key];
      }
          
      if(cond_button && cond_key && cond_area) {
        if(p_ev->flag&MEVENT_ACTIVATE_ONCE) {
          if(p_ev->last_state) {
            active = FALSE;
          } else {
            active = p_ev->last_state = TRUE;
          }           
        }
        else
          active = TRUE;
      } else {
        active = p_ev->last_state = FALSE;
      }
      
      if(active) {
        if(p_ev->flag&MEVENT_MOUSE_EXTERNAL) {          
          if(p_ev->flag&MEVENT_MOUSE_BUTTONS)
            input_queue.add(LEVEL_EVENT(GI_MOUSE_EVENT, p_ev->event[0].param_int_get(PARAM_0), mx, my, state, button));
          else
            input_queue.add(LEVEL_EVENT(GI_MOUSE_EVENT, p_ev->event[0].param_int_get(PARAM_0), mx, my, 0, 0));
        }
        else {
          input_queue.add(p_ev->event,p_ev->event_num);
        }
      }
    }
    
    input_queue.commit();
  }

  mstate.button[button] = FALSE;
}

void input::mevent_state_clear(int first)
{
  for(int i = 0; i < mevents_num; i++)
    mevents[i].state_clear();
}

void input::mevent_clear(void)
{  
  mevents_num = 0;
}

void input::mevent_add(MOUSE_EVENT *p_event, int num)
{
  int mevents_start = mevents_num;

  assert(mevents_num + num < MAX_MOUSE_EVENTS);

  // Add events
  memcpy(mevents+mevents_num,p_event,sizeof(p_event[0])*num);
  mevents_num += num;

  // Initialize all
  mevent_state_clear(mevents_start);  
}

void input::block(bool state)
{
  flag = state ? flag|INPUT_BLOCK_SETS : flag&~INPUT_BLOCK_SETS;
}

void input::key_repeat(bool state)
{
  if(state) {
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
  }
  else {
    SDL_EnableKeyRepeat(0,0);
  }
}

void input::events_wait(bool state)
{
  flag = state ? flag|INPUT_EVENT_LOOP_WAIT : flag&~INPUT_EVENT_LOOP_WAIT;
}
