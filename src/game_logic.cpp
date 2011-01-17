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

/*
 ****************************************************************
  Level events - for game level
  
  UI_ - events from user interface
  GL_ - events from game logic
  AN_ - events for animation engine (usually created by game logic or level changer)
  MN_ - events for user interface / menus (usually obtained as user input)
 ****************************************************************
typedef enum {

  // move/fast move player in level
  // format: [UI_PLAYER_MOVE, x, y, diff_x, diff_y]
  //         [UI_PLAYER_MOVE_FAST, x, y, diff_x, diff_y]
  UI_PLAYER_MOVE,
  UI_PLAYER_MOVE_FAST,

  // make other player active
  // format: [UI_PLAYER_SWITCH,  -1] - switch to another active player
  // format: [UI_PLAYER_SWITCH, num] - switch to player num
  UI_PLAYER_SWITCH,

  // ------------------------------------------------------------------------

  // change / create new object in level, erase old (if exist)
  // format: [GL_ITEM_CHANGE, x, y, new_item]
  GL_ITEM_CHANGE,

  // Erase item from level, remove all sprites and so on
  // format: [GL_ITEM_CLEAR, x, y]
  GL_ITEM_CLEAR,

  // add mattock to player
  // format: [GL_PLAYER_ADD_MATTOCK, player_number]
  GL_PLAYER_ADD_MATTOCK,

  // take mattock away from player
  // format: [GL_PLAYER_DROP_MATTOCK, player_number]
  GL_PLAYER_DROP_MATTOCK,

  // add key to player
  // format: [GL_PLAYER_ADD_GENERAL_KEY, player_number]
  GL_PLAYER_ADD_GENERAL_KEY,

  // add color key to player
  // format: [GL_PLAYER_ADD_COLOR_KEY, player_number]
  GL_PLAYER_ADD_COLOR_KEY,

  // take mattock away from player
  // format: [GL_PLAYER_DROP_COLOR_KEY, player_number]
  GL_PLAYER_DROP_COLOR_KEY,

  // ------------------------------------------------------------------------

  // Run predefined animation of item (if exist)
  // format: [AN_RUN, x, y, num_of_events_after_animation][events after animation 1...][events after animation 2...]
  AN_RUN,

  // ------------------------------------------------------------------------
  
  MN_GAME_END,
  MN_GAME_SAVE,
  MN_GAME_LOAD,
  MN_GAME_HELP

} EVENT_TYPE;
*/

#define push(event) { *p_stack = event; p_stack++; }

int game_logic::player_move(LEVEL_EVENT *p_stack, int  player, tpos px, tpos py,
                            tpos dx, tpos dy,
                            int extra_events = 0)
{
  LEVEL_EVENT *p_start = p_stack;
  anim_template_handle anim;
  bool rt;
  int  flag;

  tpos nx = px + dx;
  tpos ny = py + dy;

  // Queue 
  push(LEVEL_EVENT(GI_BLOCK_KEYS, KEY_GROUP_BLOCK_MOVE, FALSE));
  push(LEVEL_EVENT(GL_CELL_SET_ROTATION, nx, ny, LAYER_PLAYER, t2r(dx,dy), TRUE));
  push(LEVEL_EVENT(GL_CELL_SET_DIFF, nx, ny, LAYER_PLAYER, 0, 0));
  push(LEVEL_EVENT(GL_CELL_SET_DIFF, px, py, LAYER_PLAYER, 0, 0));
  push(LEVEL_EVENT(PL_PLAYER_SET_POS, nx, ny, player));
  push(LEVEL_EVENT(GL_CELL_MOVE, px, py, LAYER_PLAYER, nx, ny, TRUE));
  push(LEVEL_EVENT(AN_RUN, px, py, LAYER_PLAYER, t2a(dx,dy), 6 + extra_events,
                   NO_ROTATION));
  // Queue 
  rt = p_level->repo_get_animation(px, py, LAYER_PLAYER, &flag, &anim);
  assert(rt && flag&ANIM_TRIGGER_MOVE);
  push(LEVEL_EVENT(AN_RUN, px, py, LAYER_PLAYER, anim, 0, t2r(dx,dy)));
  
  // Statistics update
  p_status->steps_add();
  
  return(p_stack - p_start);
}

int game_logic::item_move(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer,
                          tpos dx, tpos dy, 
                          int extra_events = 0)
{
  LEVEL_EVENT *p_start = p_stack;

  tpos nx = px + dx;
  tpos ny = py + dy;

  push(LEVEL_EVENT(GL_CELL_SET_DIFF, nx, ny, layer, 0, 0));
  push(LEVEL_EVENT(GL_CELL_MOVE, px, py, layer, nx, ny, TRUE));
  push(LEVEL_EVENT(AN_RUN, px, py, layer, t2a(dx,dy), 2 + extra_events));
  
  return(p_stack - p_start);
}

int game_logic::item_blast(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer,
                           int extra_events = 0)
{
  LEVEL_EVENT *p_start = p_stack;
  
  // remove item from level
  // run the blast animations
  push(LEVEL_EVENT(AN_RUN, px, py, layer, ANIM_BLAST, extra_events));
  push(LEVEL_EVENT(GL_CELL_CLEAR_ALL, px, py, layer, TRUE));

  return(p_stack - p_start);
}

int game_logic::item_erase(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer,
                           bool graphics_change,
                           int extra_events = 0)
{
  LEVEL_EVENT *p_start = p_stack;
  int flag, anim;
  
  if(p_level->repo_get_animation(px, py, layer, &flag, &anim) &&
     flag&ANIM_TRIGGER_ERASE) {
    push(LEVEL_EVENT(GL_CELL_CLEAR_ALL, px, py, layer, graphics_change));
    push(LEVEL_EVENT(AN_RUN, px, py, layer, anim, 1 + extra_events));
  }
  else {
    push(LEVEL_EVENT(GL_CELL_CLEAR_ALL, px, py, layer, graphics_change));
  }

  return(p_stack - p_start);
}

int game_logic::item_set(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer,
                         item_handle itm,
                         bool graphics_change,
                         bool rewrite_subitems = FALSE)
{
  LEVEL_EVENT *p_start = p_stack;
    
  //format: [GL_CELL_SET_ITEM, x, y, layer, new_item, rewrite_sub_items, change_sprite]
  push(LEVEL_EVENT(GL_CELL_SET_ITEM, px, py, layer, itm, rewrite_subitems, graphics_change));
  push(LEVEL_EVENT(AN_STOP_AND_CLEAR, px, py, layer));

  return(p_stack - p_start);
}

int game_logic::item_variation_set(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer,
                                   int  variation,
                                   bool graphics_change,
                                   bool rewrite_subitems = FALSE)
{
  LEVEL_EVENT *p_start = p_stack;

  // format: [GL_CELL_SET_VARIATION, x, y, layer, variation, rewrite_subitems, change_sprite]
  push(LEVEL_EVENT(GL_CELL_SET_VARIATION, px, py, layer, variation, rewrite_subitems, graphics_change));
  push(LEVEL_EVENT(AN_STOP_AND_CLEAR, px, py, layer));

  return(p_stack - p_start);
}

int game_logic::anim_generate(LEVEL_EVENT *p_stack, ANIM_TYPE type, tpos px, tpos py,
                              int extra_events = 0)
{
  LEVEL_EVENT *p_start = p_stack;

  push(LEVEL_EVENT(AN_GENERATE_AND_RUN, px, py, LAYER_ITEMS, type, extra_events,
                   NO_ROTATION));

  return(p_stack - p_start);
}

int game_logic::exit_animate(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer,
                             int extra_events = 0)
{
  LEVEL_EVENT *p_start = p_stack;
  
  // run the exit animation
  push(LEVEL_EVENT(AN_RUN, px, py, layer, ANIM_EXIT_1, extra_events));

  return(p_stack - p_start);
}

#define CHECK_MOVEMENT(nx,ny) (nx >= 0 && nx < LEVEL_CELLS_X && ny >= 0 && ny < LEVEL_CELLS_Y)

void game_logic::player_move_check(LEVEL_EVENT_QUEUE *p_queue, LEVEL_EVENT *p_in)
{
  int  player = p_level->player_get();
  bool player_moved = FALSE;
  
  tpos px,py,dx,dy;
  tpos nx,ny,nnx,nny;

  //-- je blbost aby hrac byl jak v poli tak v tom levelu -> nejak to sjednotit

  dx = p_in->param_int_get(PARAM_POSITION_X);
  dy = p_in->param_int_get(PARAM_POSITION_Y);

  p_level->player_get_pos(player,&px,&py);
  
  nx = px + dx;
  ny = py + dy;

  nnx = nx + dx;
  nny = ny + dy;

  item_handle itm_start = p_level->cell_get_item(px, py, LAYER_ITEMS);
  item_handle itm = -1;
  item_handle itm_next = -1;
  item_handle itm_player = -1;
  item_handle itm_next_player = -1;

  if(CHECK_MOVEMENT(nx,ny)) {
    itm = p_level->cell_get_item(nx, ny, LAYER_ITEMS);  
    itm_player = p_level->cell_get_item(nx, ny, LAYER_PLAYER);
  }
  if(CHECK_MOVEMENT(nnx,nny)) {
    itm_next = p_level->cell_get_item(nnx, nny, LAYER_ITEMS);  
    itm_next_player = p_level->cell_get_item(nnx, nny, LAYER_PLAYER);
  }

  #define     TMP_EVENTS    100
  LEVEL_EVENT events[TMP_EVENTS];
  int         event_num = 0;

  // Check player's starting point
  switch(itm_start) {
    // Opened ID color doors w/o key
    case P_ID_DOOR1_H_O:
    case P_ID_DOOR2_H_O:
    case P_ID_DOOR3_H_O:
    case P_ID_DOOR4_H_O:
    case P_ID_DOOR5_H_O:
      if(dx) {
        if(p_level->cell_get_variation(px, py, LAYER_ITEMS) == DOOR_VARIATION_CYBER) {
          event_num += anim_generate(events+event_num, ANIM_DOOR_ID_H, px, py);
        }
        event_num += item_set(events+event_num,px,py,LAYER_ITEMS,
                              P_ID_DOOR1_H_Z+itm_start-P_ID_DOOR1_H_O,TRUE,TRUE);
      }
      break;
        
    // Opened ID color doors w/o key
    case P_ID_DOOR1_V_O:
    case P_ID_DOOR2_V_O:
    case P_ID_DOOR3_V_O:
    case P_ID_DOOR4_V_O:
    case P_ID_DOOR5_V_O:
      if(dy) {
        if(p_level->cell_get_variation(px, py, LAYER_ITEMS) == DOOR_VARIATION_CYBER) {
          event_num += anim_generate(events+event_num, ANIM_DOOR_ID_V, px, py);
        }
        event_num += item_set(events+event_num,px,py,LAYER_ITEMS,
                              P_ID_DOOR1_V_Z+itm_start-P_ID_DOOR1_V_O,TRUE,TRUE);
      }
      break;
      
    // One-pass doors
    case P_DV_H_O:
      if(dx) {
        if(p_level->cell_get_variation(px, py, LAYER_ITEMS) == DOOR_VARIATION_CYBER) {
          event_num += anim_generate(events+event_num, ANIM_DOOR_DV_H, px, py);
        }
        event_num += item_set(events+event_num,px,py,LAYER_ITEMS,
                              P_DV_H_Z,TRUE,TRUE);
      }
      break;
        
    case P_DV_V_O:
      if(dy) {
        if(p_level->cell_get_variation(px, py, LAYER_ITEMS) == DOOR_VARIATION_CYBER) {
          event_num += anim_generate(events+event_num, ANIM_DOOR_DV_V, px, py);
        }
        event_num += item_set(events+event_num,px,py,LAYER_ITEMS,
                              P_DV_V_Z,TRUE,TRUE);
      }
      break;
  }

  // Check player target point
  switch(itm) {
    // Just move the player
    case NO_ITEM:
      if(!CHECK_MOVEMENT(nx,ny))
        break;
      
      if(itm_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        player_moved = TRUE;
      }
      break;
    
    case P_BOX: // move player and box
      if(!CHECK_MOVEMENT(nnx,nny))
        break;
      
      if(itm_next == NO_ITEM && itm_next_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        event_num += item_move(events+event_num,nx,ny,LAYER_ITEMS,dx,dy);
        player_moved = TRUE;
      }
      break;
    
    case P_TNT: // move player and box or explode both boxes
      if(!CHECK_MOVEMENT(nnx,nny))
        break;
      
      // move player and tnt
      if(itm_next == NO_ITEM && itm_next_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        event_num += item_move(events+event_num,nx,ny,LAYER_ITEMS,dx,dy);
        player_moved = TRUE;
      }
      else if(itm_next == P_BOX) { // blow tnt & box   
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        event_num += item_blast(events+event_num,nnx,nny,LAYER_ITEMS);
        event_num += item_erase(events+event_num,nx,ny,LAYER_ITEMS,TRUE);
        player_moved = TRUE;
      }
      break;
  
    case P_EXIT:
      if(p_level->state_keys_enough()) {
        p_status->bug_in_exit();
        p_queue->add(LEVEL_EVENT(GC_STOP_LEVEL, FALSE, TRUE));
      }
      break;
  
    case P_STONE:
      if(p_level->player_mattock_drop(player)) {
        event_num += player_move(events+event_num, player, px, py, dx, dy, event_num);
        event_num += item_erase(events+event_num, nx, ny, LAYER_ITEMS, TRUE, event_num);
        player_moved = TRUE;
      }
      break;
    
    case P_KEY:
      if(p_level->state_add_key()) {

        /* Status update */
        p_status->keys_add();
      
        /* Manage player movement */
        event_num += item_erase(events+event_num, nx, ny, LAYER_ITEMS, TRUE);
        event_num += player_move(events+event_num, player, px, py, dx, dy, event_num);
        player_moved = TRUE;
      
        /* Anim exit(s) if player has the propper amount of keys */
        if(p_level->state_keys_enough()) {
          tpos px, py;
          
          bool ret = exit_find(TRUE, &px, &py);
          assert(ret);
          
          do {
            int variant = p_level->cell_get_variation(px, py, LAYER_ITEMS);
            if(variant == ANIM_EXIT) {
              /* Create an exit animation */
              event_num += exit_animate(events+event_num, px, py, LAYER_ITEMS, 0);
            } else {
              /* Change the graphics only */
              variant = (variant == REV_EXIT) ? variant-1 : variant+1;
              event_num += item_variation_set(events+event_num, px, py, LAYER_ITEMS, variant, TRUE);
            }
          } while(exit_find(FALSE, &px, &py));        
        }      
      }
      break;
  
    case P_MATTOCK:
      if(p_level->player_mattock_add(player)) {
        event_num += item_erase(events+event_num, nx, ny, LAYER_ITEMS, TRUE);
        event_num += player_move(events+event_num, player, px, py, dx, dy, event_num);
        player_moved = TRUE;
      }
      break;
  
    // Color keys
    case P_KEY1:
    case P_KEY2:
    case P_KEY3:
    case P_KEY4:
    case P_KEY5:
      if(player == (itm - P_KEY1) && p_level->player_key_add(player)) {
        event_num += item_erase(events+event_num, nx, ny, LAYER_ITEMS, TRUE);
        event_num += player_move(events+event_num, player, px, py, dx, dy, event_num);
        player_moved = TRUE;
      }
      break;
    
    // Already opened color doors
    // Horizontal variation
    case P_DOOR1_H_O:
    case P_DOOR2_H_O:
    case P_DOOR3_H_O:
    case P_DOOR4_H_O:
    case P_DOOR5_H_O:
      if(dx && itm_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy, event_num);
        player_moved = TRUE;
      }
      break;
    
    // Already opened color doors
    // Vertical variation
    case P_DOOR1_V_O:
    case P_DOOR2_V_O:
    case P_DOOR3_V_O:
    case P_DOOR4_V_O:
    case P_DOOR5_V_O:
      if(dy && itm_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy, event_num);
        player_moved = TRUE;
      }
      break;

    // Closed color doors
    // Horizontal variation    
    case P_DOOR1_H_Z:
    case P_DOOR2_H_Z:
    case P_DOOR3_H_Z:
    case P_DOOR4_H_Z:
    case P_DOOR5_H_Z:    
      if(player == (itm - P_DOOR1_H_Z) && dx && p_level->player_key_drop(player)) {
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);        
        if(p_level->cell_get_variation(nx, ny, LAYER_ITEMS) == DOOR_VARIATION_CLASSIC) {
          event_num += item_variation_set(events+event_num, nx, ny, LAYER_FLOOR, FLOOR_CLASSIC_UP, TRUE);
          event_num += item_set(events+event_num, nx, ny, LAYER_FLOOR, P_GROUND, FALSE);
          event_num += item_erase(events+event_num, nx, ny, LAYER_ITEMS, TRUE);
          event_num += item_erase(events+event_num, nx, ny, LAYER_FLOOR, FALSE);
          event_num += item_set(events+event_num,nx,ny,LAYER_ITEMS,P_DOOR1_H_O+(itm-P_DOOR1_H_Z),TRUE,TRUE);
        }
        if(p_level->cell_get_variation(nx, ny, LAYER_ITEMS) == DOOR_VARIATION_CYBER) {
          event_num += item_variation_set(events+event_num, nx, ny, LAYER_ITEMS, 0, TRUE, TRUE);
          event_num += item_set(events+event_num, nx, ny, LAYER_ITEMS, P_DV_H, FALSE, FALSE);
        }
        player_moved = TRUE;
      }
      break;

    // Closed color doors
    // Vertical variation
    case P_DOOR1_V_Z:
    case P_DOOR2_V_Z:
    case P_DOOR3_V_Z:
    case P_DOOR4_V_Z:
    case P_DOOR5_V_Z:
      if(player == (itm - P_DOOR1_V_Z) && dy && p_level->player_key_drop(player)) {
        event_num += player_move(events+event_num,player,px,py,dx,dy, event_num);        
        if(p_level->cell_get_variation(nx, ny, LAYER_ITEMS) == DOOR_VARIATION_CLASSIC) {
          event_num += item_variation_set(events+event_num, nx, ny, LAYER_FLOOR, FLOOR_CLASSIC_LEFT, TRUE);
          event_num += item_set(events+event_num, nx, ny, LAYER_FLOOR, P_GROUND, FALSE);
          event_num += item_erase(events+event_num, nx, ny, LAYER_ITEMS, TRUE);
          event_num += item_erase(events+event_num, nx, ny, LAYER_FLOOR, FALSE);
          event_num += item_set(events+event_num,nx,ny,LAYER_ITEMS,P_DOOR1_V_O+(itm-P_DOOR1_V_Z),TRUE,TRUE);
        }
        if(p_level->cell_get_variation(nx, ny, LAYER_ITEMS) == DOOR_VARIATION_CYBER) {
          event_num += item_variation_set(events+event_num, nx, ny, LAYER_ITEMS, 0, TRUE, TRUE);
          event_num += item_set(events+event_num, nx, ny, LAYER_ITEMS, P_DV_V, FALSE, FALSE);
        }
        player_moved = TRUE;
      }
      break;
  
    // Closed color doors w/o key
    case P_ID_DOOR1_H_Z:
    case P_ID_DOOR2_H_Z:
    case P_ID_DOOR3_H_Z:
    case P_ID_DOOR4_H_Z:
    case P_ID_DOOR5_H_Z:
      if(player == (itm-P_ID_DOOR1_H_Z) && dx) {      
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        event_num += item_set(events+event_num,nx,ny,LAYER_ITEMS,P_ID_DOOR1_H_O+(itm-P_ID_DOOR1_H_Z),TRUE,TRUE);
        player_moved = TRUE;
      }    
      break; 
    
    // Opened color doors w/o key
    case P_ID_DOOR1_H_O:
    case P_ID_DOOR2_H_O:
    case P_ID_DOOR3_H_O:
    case P_ID_DOOR4_H_O:
    case P_ID_DOOR5_H_O:
      if(player == (itm - P_ID_DOOR1_H_O) && dx && itm_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        player_moved = TRUE;
      }
      break;
    
    case P_ID_DOOR1_V_Z:
    case P_ID_DOOR2_V_Z:
    case P_ID_DOOR3_V_Z:
    case P_ID_DOOR4_V_Z:
    case P_ID_DOOR5_V_Z:
      if(player == (itm - P_ID_DOOR1_V_Z) && dy && itm_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        event_num += item_set(events+event_num,nx,ny,LAYER_ITEMS,P_ID_DOOR1_V_O+(itm-P_ID_DOOR1_V_Z),TRUE,TRUE);
        player_moved = TRUE;
      }
      break;
    
    case P_ID_DOOR1_V_O:
    case P_ID_DOOR2_V_O:
    case P_ID_DOOR3_V_O:
    case P_ID_DOOR4_V_O:
    case P_ID_DOOR5_V_O:
      if(player == (itm-P_ID_DOOR1_V_O) && dy && itm_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        player_moved = TRUE;
      }
      break;
      
    // One-pass doors
    case P_DV_H_O:
      if(dx && itm_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        player_moved = TRUE;    
      }
      break;
        
    case P_DV_V_O:
      if(dy && itm_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        player_moved = TRUE;    
      }
      break;
      
    case P_DV_H_Z:
    case P_DV_V_Z:    
      // closed -> no way
      break;
    
    // Some strange door?
    case P_DV_H:
      if(dx && itm_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        player_moved = TRUE;
      }
      break;
      
    case P_DV_V:
      if(dy && itm_player == NO_ITEM) {
        event_num += player_move(events+event_num,player,px,py,dx,dy,event_num);
        player_moved = TRUE;
      }
      break;
      
    // Wall or something similiar
    default:
      break;
  }

  // Add events to queue...
  if(event_num) {
    p_queue->add(events+event_num-1, event_num, DIRECTION_STACK);
  }

  // Enable next movement of player...
  if(!player_moved) {
    p_queue->add(LEVEL_EVENT(GI_BLOCK_KEYS, KEY_GROUP_BLOCK_MOVE, FALSE));
  }
}

/*
  {LEVEL_EVENT(PLAYER_SWITCH, -1, 0), K_TAB, 0,0,0},

  {LEVEL_EVENT(PLAYER_SWITCH,  0, 0), K_1, 0,0,0},
  {LEVEL_EVENT(PLAYER_SWITCH,  1, 0), K_2, 0,0,0},
  {LEVEL_EVENT(PLAYER_SWITCH,  2, 0), K_3, 0,0,0},
  {LEVEL_EVENT(PLAYER_SWITCH,  3, 0), K_4, 0,0,0},
  {LEVEL_EVENT(PLAYER_SWITCH,  4, 0), K_5, 0,0,0},
*/
void game_logic::player_switch(LEVEL_EVENT *p_in)
{
  p_level->player_switch(p_in->param_int_get(PARAM_0));
}

// Process event and return events for level-changer
void game_logic::events_process(LEVEL_EVENT_QUEUE *p_queue)
{
  while(!p_queue->empty()) {
    LEVEL_EVENT ev = p_queue->get();
    if(ev.action_get() == GL_PLAYER_MOVE) {
      player_move_check(p_queue, &ev);
    } else if(ev.action_get() == GL_PLAYER_SWITCH) {
      player_switch(&ev);
    } else {
      p_queue->add(ev);
    }
  }

  p_queue->commit();
}

bool game_logic::exit_find(bool start, tpos *p_px, tpos *p_py)
{
  static tpos x, y;

  if(start) {
    x = 0;
    y = 0;
  }

  for(; y < LEVEL_CELLS_Y; y++) {
    for(; x < LEVEL_CELLS_X; x++) {      
      item_handle itm = p_level->cell_get_item(x, y, LAYER_ITEMS);
      if(itm == P_EXIT) {
        *p_px = x++;
        *p_py = y;
        return(TRUE);
      }
    }
    x = 0;
  }
  return(FALSE);
}
