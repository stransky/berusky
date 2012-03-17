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

#ifndef __ANIMATION_H__
#define __ANIMATION_H__


// -------------------------------------------------------
// Template for animations
// -------------------------------------------------------
typedef struct animation_template {
 
  int           flag;
  
  int           frame_num;        //
  int           frame_correction; // base frame correction for this animation
  int           speed_up;         // can be this animation boosted?

  // linear animations -> same values for every frame
  tpos          dx,dy;
    
  spr_handle    sprite_first;
  spr_handle    sprite_num;
  spr_handle    sprite_step;

  // tables -> lenght is defined by frame_num
  spr_handle   *p_sprite_table;   // sprite array, there is an entry for every frame
  int          *p_delay_table;    // time corrections, there is an entry for every frame

} ANIMATION_TEMPLATE;

// -------------------------------------------------------
// defines for template_generate()
// -------------------------------------------------------
typedef enum _ANIM_TYPE {

  ANIM_DOOR_ID_H,
  ANIM_DOOR_ID_V,
  ANIM_DOOR_DV_H,
  ANIM_DOOR_DV_V

} ANIM_TYPE;

// -------------------------------------------------------
// Storage for anim. templates
// -------------------------------------------------------
typedef class animation_repository {

  ANIMATION_TEMPLATE *p_tpl;
  int                 tpl_num;

  int                 dynamic_first; // first dynamic item
  int                 dynamic_last; // last dynamic item

private:
  
  anim_template_handle template_get(void)
  {
    int i;
    for(i = dynamic_last+1; i < tpl_num; i++) {
      if(!(p_tpl[i].flag&ANIM_USED)) {
        p_tpl[i].flag |= ANIM_USED;
        dynamic_last = i;
        return i;
      }
    }
    return(FALSE);
  }

  int template_extend(void)
  {
    return(FALSE);
  }

  int template_reduce(void)
  {
    return(FALSE);
  }

public:

  animation_repository(void);
  ~animation_repository(void);
  
  void repository_create(int tpl_num_);
  int  repository_load(const char *p_file, const char *p_dir);
  void repository_modify(void);

  anim_template_handle template_create(int flag, int frame_num);
  void                 template_shift(anim_template_handle handle, tpos dx, tpos dy);
  void                 template_sprites_static(anim_template_handle handle,
                                               spr_handle sprite_first,
                                               spr_handle sprite_num,
                                               spr_handle sprite_step);
  void                 template_sprites_dynamic(anim_template_handle handle,
                                                int         frame_num,
                                                spr_handle *p_sprite_table,
                                                int        *p_frame_table);
  anim_template_handle template_copy(anim_template_handle src);
  void template_free(anim_template_handle handle);

  anim_template_handle template_generate(ANIM_TYPE type);

  ANIMATION_TEMPLATE * get(anim_template_handle handle)
  {
    assert(handle >= 0 && handle < tpl_num);
    return(p_tpl + handle);
  }

  int get_frame_correction(anim_template_handle handle, int position = 0)
  {
    assert(handle >= 0 && handle < tpl_num);
    if(p_tpl[handle].p_delay_table) {
      assert(position < p_tpl[handle].frame_num);
      return(p_tpl[handle].p_delay_table[position]);
    } else {
      return(p_tpl[handle].frame_correction);    
    }    
  }

  int get_frame_num(anim_template_handle handle)
  {
    assert(handle >= 0 && handle < tpl_num);
    return(p_tpl[handle].frame_num);
  }

  int get_flag(anim_template_handle handle)
  {
    assert(handle >= 0 && handle < tpl_num);
    return(p_tpl[handle].flag);
  }

} ANIMATION_REPOSITORY;

// -------------------------------------------------------
// Run-time animation
// -------------------------------------------------------

#define MAX_POST_EVENTS  10

typedef class animation {

  static ANIMATION_REPOSITORY * p_repository;

  class animation    *p_next;
  
  int                 flag;
  
  int                 frame_num;
  int                 frame_current;
  int                 frame_correction;

  int                 position_in_animation;
  
  tpos                x,y,layer;
  int                 rotation;
  
  anim_template_handle thandle;
  
  int                 post_events_num;
  LEVEL_EVENT         post_events[MAX_POST_EVENTS];
  
public:

  animation(void);
  ~animation(void);

  static void repository_set(ANIMATION_REPOSITORY *p_repository_)
  {
    p_repository = p_repository_;
  }

  void set_used(bool used = true)
  {
    flag = used ? flag | ANIM_USED : flag & ~ANIM_USED;
  }

  void remove(void)
  {
    flag |= ANIM_REMOVE;
  }

  bool is_used(void)
  {
    return(flag&ANIM_USED);
  }

  void init(anim_template_handle thandle_, tpos x_, tpos y_, tpos layer_, int rotation_);
  int  post_event_add(LEVEL_EVENT *p_event);
  void start(void);
  void stop(LEVEL_EVENT_QUEUE *p_queue, int *p_events, bool remove);
  int  end(LEVEL_EVENT_QUEUE *p_queue);
  void process_sprite_animation(LEVEL_GAME *p_level);
  void process_move_animation(LEVEL_GAME *p_level);

  // it returns status of animation
  //      true - end of animation
  //      false - animation si running
  bool process(LEVEL_GAME *p_level, LEVEL_EVENT_QUEUE *p_queue, int *p_events);

  void next_set(class animation *p_next_)
  { 
    p_next = p_next_;
  }

  class animation * next_get(void)
  {  
    return(p_next);
  }

} ANIMATION;

// -------------------------------------------------------
// Animation engine
// -------------------------------------------------------

typedef class animation_engine {

  // round-robin animation repository
  ANIMATION            animation_stack[ANIM_QUEUE_LENGHT];
  int                  animation_last;

  ANIMATION_REPOSITORY repository;  // repository for animations

  ANIMATION           *p_running;   // list of animations  

  LEVEL_GAME          *p_level;

public:

  // -------------------------------------------------------
  // Load / Destroy functions
  // -------------------------------------------------------
  animation_engine(LEVEL_GAME *p_level_, DIR_LIST *p_dir);
  ~animation_engine(void);

  // ------------------------------------------------------
  // Animation template interface
  // ------------------------------------------------------
  int next_index(int index)
  {    
    return ++index < ANIM_QUEUE_LENGHT ? index : ANIM_QUEUE_FIRST;
  }

  int prev_index(int index)
  {    
    return --index < ANIM_QUEUE_FIRST ? ANIM_QUEUE_LENGHT-1 : index;
  }

  anim_handle animation_find_next_free(void);
  anim_handle animation_new(anim_template_handle handle, tpos x, tpos y, 
                            tpos layer, int rotation);
    
  // -------------------------------------------------------
  // Animation interface
  // -------------------------------------------------------
  void add_to_running(anim_handle anim)
  {
    animation_stack[anim].next_set(p_running);
    p_running = animation_stack+anim;
  }

  int add_evets(anim_handle anim, LEVEL_EVENT *p_event)
  {  
    return(animation_stack[anim].post_event_add(p_event));
  }

  anim_handle animation_new_and_run(anim_template_handle handle,
                        tpos x, tpos y, tpos layer, int rotation)
  {
    anim_handle anim = animation_new(handle, x, y, layer, rotation);
    add_to_running(anim);
    return(anim);
  }

  anim_handle animation_generate_and_run(ANIM_TYPE type,
                        tpos x, tpos y, tpos layer, int rotation)
  {    
    return(animation_new_and_run(repository.template_generate(type),x,y,layer,rotation));
  }

  void animation_stop_and_delete(anim_handle anim)
  {
    assert(anim >= ANIM_QUEUE_FIRST && anim < ANIM_QUEUE_LENGHT);
    animation_stack[anim].remove();
  }

  // -------------------------------------------------------
  // process animation (more than one frame change) and changes
  // it returns: true  - there are new events for level changer
  //             flase - no new events
  // -------------------------------------------------------
  int animation_process(LEVEL_EVENT_QUEUE *p_queue);

} ANIMATION_ENGINE;

// Translation to animation
inline int t2a(int dx, int dy)
{
  assert((dx+dy) && (!dx || !dy));
  return dx ? (7 + dx) : (6 + dy);  
}

// Translation to rotation
inline int t2r(int dx, int dy)
{
  assert((dx+dy) && (!dx || !dy));
  return dx ? (2 - dx) : (1 + dy);
}

#endif //__ANIMATION_H__
