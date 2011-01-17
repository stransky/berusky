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


#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__


/* Game logic module - confirms changes in level
*/
typedef class game_logic {

  LEVEL_GAME   *p_level;
  LEVEL_STATUS *p_status;

  int  player_move(LEVEL_EVENT *p_buffer, int  player, tpos px, tpos py, tpos dx, tpos dy, int extra_events);
  int  item_move(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer, tpos dx, tpos dy, int extra_events);
  int  item_blast(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer, int extra_events);
  int  item_erase(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer, bool graphics_change, int extra_events);
  int  item_set(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer, item_handle itm, bool graphics_change, bool rewrite_subitems);  
  int  item_variation_set(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer, int  variation, bool graphics_change, bool rewrite_subitems);
  int  anim_generate(LEVEL_EVENT *p_stack, ANIM_TYPE type, tpos px, tpos py, int extra_events);
  int  exit_animate(LEVEL_EVENT *p_stack, tpos px, tpos py, tpos layer, int extra_events);
  bool exit_find(bool start, tpos *p_px, tpos *p_py);

public:

  game_logic(LEVEL_GAME *p_game_level, LEVEL_STATUS *p_level_status) 
  : p_level(p_game_level), p_status(p_level_status) {};
  
  void player_move_check(LEVEL_EVENT_QUEUE *p_queue, LEVEL_EVENT *p_in);
  void player_switch(LEVEL_EVENT *p_in);
  
  // Process event and return events for level-changer  
  void events_process(LEVEL_EVENT_QUEUE *p_queue);  

} GAME_LOGIC;

#endif // __GAME_LOGIC_H__
