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


void level_changer::events_process(LEVEL_EVENT_QUEUE *p_queue)
{  
  LEVEL_EVENT ev;  

  do {

    while(!p_queue->empty()) {
      ev = p_queue->get();
    
      switch(ev.action_get()) {
        // Set player position
        // format: [PL_PLAYER_SET_POS, x, y, player_num]
        case PL_PLAYER_SET_POS:
          p_level->player_set_pos(ev.param_int_get(PARAM_2), ev.param_int_get(PARAM_0),
                                  ev.param_int_get(PARAM_1));
          break;      
      
        // change / create new object in level, erase old (if exist)        
        // format: [GL_CELL_SET_ITEM, x, y, layer, new_item, rewrite_sub_items, change_sprite]
        case GL_CELL_SET_ITEM:
          p_level->cell_set_item(ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1),
                                 ev.param_int_get(PARAM_2), ev.param_int_get(PARAM_3),
                                 ev.param_int_get(PARAM_4), ev.param_int_get(PARAM_5));
          break;
        
        // change / set variation
        // format: [GL_CELL_SET_VARIATION, x, y, layer, variation, rewrite_subitems, change_sprite]
        case GL_CELL_SET_VARIATION:
          p_level->cell_set_variation(ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1),
                                      ev.param_int_get(PARAM_2), ev.param_int_get(PARAM_3),
                                      ev.param_int_get(PARAM_4), ev.param_int_get(PARAM_5));
          break;
        
        // change / set rotation of items
        // format: [GL_CELL_SET_ITEM, x, y, layer, rotation, change_sprite]
        case GL_CELL_SET_ROTATION:  
          p_level->cell_set_rotation(ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1),
                                     ev.param_int_get(PARAM_2), ev.param_int_get(PARAM_3),
                                     ev.param_int_get(PARAM_4));
          break;
  
        // change diff for cell
        // format: [GL_SET_DIFF, x, y, layer, diff_x, diff_y]      
        case GL_CELL_SET_DIFF:
          p_level->cell_graphics_set_diff(ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1),
                                          ev.param_int_get(PARAM_2), ev.param_int_get(PARAM_3),
                                          ev.param_int_get(PARAM_3));
          break;
        
        // erase cell in level, remove all sprites and so on
        // format: [GL_CELL_CLEAR_ALL, x, y, layer, change_sprite]
        // GL_CELL_CLEAR_ALL = 50
        case GL_CELL_CLEAR_ALL:
          {
            p_level->cell_clear(ev.param_int_get(PARAM_0),ev.param_int_get(PARAM_1),
                                ev.param_int_get(PARAM_2),ev.param_int_get(PARAM_3));        
          }
          // ********* Continue to AN_STOP_AND_CLEAR stage *********
        
        // erase animation from level (if exist)
        // format: [AN_STOP_AND_CLEAR, x, y, layer]
        case AN_STOP_AND_CLEAR:
          {
            anim_handle anim = p_level->cell_get_animation(ev.param_int_get(PARAM_POSITION_X),
                               ev.param_int_get(PARAM_POSITION_Y),ev.param_int_get(PARAM_LAYER));
            if(anim != ANIM_NONE) {
              ae.animation_stop_and_delete(anim);
              p_level->cell_set_animation(ev.param_int_get(PARAM_POSITION_X),ev.param_int_get(PARAM_POSITION_Y),
                                          ev.param_int_get(PARAM_LAYER),ANIM_NONE);
            }
          }
          break;
        
        // Move cell in level
        // format: [GL_CELL_MOVE, x, y, layer, nx, ny, change_sprite]
        // GL_CELL_MOVE = 60      
        case GL_CELL_MOVE:
          p_level->cell_move(ev.param_int_get(PARAM_0), // x
                             ev.param_int_get(PARAM_1), // y
                             ev.param_int_get(PARAM_2), // layer
                             ev.param_int_get(PARAM_3), // nx
                             ev.param_int_get(PARAM_4), // ny                           
                             ev.param_int_get(PARAM_5));// sprite change
          break;
  
        // Run defined animation to selected item        
        case AN_RUN:
        case AN_GENERATE_AND_RUN:
          {
            // create new animation
            // format: [AN_RUN, x, y, layer, animation, chained_events][next...][....]
            anim_handle anim = 0;
          
            if(ev.action_get() == AN_RUN) {
              anim = ae.animation_new_and_run(ev.param_int_get(PARAM_ANIMATION),
                                              ev.param_int_get(PARAM_POSITION_X),
                                              ev.param_int_get(PARAM_POSITION_Y),
                                              ev.param_int_get(PARAM_LAYER),
                                              ev.param_int_get(PARAM_ROTATION));
            } else {
              // generate and run new animation
              // format: [AN_GENERATE_AND_RUN, x, y, layer, animation, chained_events][next...][....]
              anim = ae.animation_generate_and_run((ANIM_TYPE)ev.param_int_get(PARAM_ANIMATION),
                                                  ev.param_int_get(PARAM_POSITION_X),
                                                  ev.param_int_get(PARAM_POSITION_Y),
                                                  ev.param_int_get(PARAM_LAYER),
                                                  ev.param_int_get(PARAM_ROTATION));
            
              p_level->cell_set_animation(ev.param_int_get(PARAM_POSITION_X),
                                          ev.param_int_get(PARAM_POSITION_Y),
                                          ev.param_int_get(PARAM_LAYER),
                                          anim);
            }
            assert(anim != A_ERROR);

            // fetch events from p_queue
            int events;
            for(events = ev.param_int_get(PARAM_NEXT_EVENTS); events > 0; events--) {
              assert(!p_queue->empty());
              LEVEL_EVENT tmp = p_queue->get();
              int ret = ae.add_evets(anim, &tmp);
              assert(ret);
            }
          
            p_level->cell_set_animation(ev.param_int_get(PARAM_POSITION_X),ev.param_int_get(PARAM_POSITION_Y),
                                       ev.param_int_get(PARAM_LAYER),anim);
          }
          break;
        
        default:
          p_queue->add(ev);
          break;
      }    
    }
  
    // commit all changes to queue
    p_queue->commit();
  
    // run animation engine
  } while(ae.animation_process(p_queue));
}
