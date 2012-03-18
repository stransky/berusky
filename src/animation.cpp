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

// -------------------------------------------------------
// Animation repository
// -------------------------------------------------------

// -------------------------------------------------------
// Num of anim. templates
// -------------------------------------------------------
#define ANIM_NUM                    100

animation_repository::animation_repository(void)
  : p_tpl(NULL), tpl_num(0), dynamic_last(0)
{
  repository_create(ANIM_NUM);
}

animation_repository::~animation_repository(void)
{  
  if(p_tpl) {    
    for(int i = 0; i < tpl_num; i++)
      template_free(i);
  
    if(p_tpl) {
      free(p_tpl);
      p_tpl = NULL;    
    }
  }
}

void animation_repository::repository_create(int tpl_num_)
{
  tpl_num = tpl_num_;
  p_tpl = (ANIMATION_TEMPLATE *)mmalloc(sizeof(ANIMATION_TEMPLATE)*tpl_num);  
}

int  animation_repository::repository_load(const char *p_file, const char *p_dir)
{
  DATA_PARSER data_file(get_parser_table(), get_parser_table_items());
    
  if(!data_file.open(p_file, p_dir))
    return(FALSE);
    
  dynamic_first = dynamic_last = data_file.get_max_index()+1;
  assert(dynamic_last < tpl_num);
  
  /*
    Load animations from disc
  */
  
  #define RECORDS 9
  DATA_RECORD rec[RECORDS];
  int rec_num, rec_line = 0;  
  while((rec_num = data_file.records_get(rec, RECORDS))) {
  
    bprintf("%d: animation template = %d, flag = %d, frame_num = %d, dx = %d, dy = %d, sprite_first = %d, sprite_num = %d, sprite_step = %d, frame_correction = %d",
           rec_line, rec[0].num, rec[1].num, rec[2].num, rec[3].num,
           rec[4].num, rec[5].num, rec[6].num, rec[7].num, rec[8].num);
  
    assert(rec_num == RECORDS);
    assert(rec[0].num < dynamic_last);
  
    ANIMATION_TEMPLATE *p_tmp = p_tpl+rec[0].num;
  
    p_tmp->flag = rec[1].num;
    
    p_tmp->frame_num = rec[2].num;
  
    p_tmp->dx = rec[3].num;
    p_tmp->dy = rec[4].num;
  
    p_tmp->sprite_first = rec[5].num;
    p_tmp->sprite_num = rec[6].num;
    p_tmp->sprite_step = rec[7].num;
  
    p_tmp->frame_correction = rec[8].num;
  
    rec_line++;
  }

  data_file.close();

  return(TRUE);
}

void animation_repository::repository_modify(void)
{ 
  int i,j;
    
  for(i = 0; i < dynamic_last; i++) {
    ANIMATION_TEMPLATE *p_tmp = p_tpl+i;
    if(p_tmp->flag&ANIM_SPRITE && p_tmp->flag&ANIM_ADD_INVERSE) {
      int frame_num = p_tmp->frame_num*2-2;
        
      p_tmp->p_sprite_table = (spr_handle *)mmalloc(sizeof(p_tmp->p_sprite_table[0])*frame_num);
      p_tmp->p_delay_table = (int *)mmalloc(sizeof(p_tmp->p_delay_table[0])*frame_num);
    
      for(j = 0; j < p_tmp->frame_num; j++) {
        p_tmp->p_sprite_table[j] = p_tmp->sprite_first + j*p_tmp->sprite_step;
        p_tmp->p_delay_table[j] = p_tmp->frame_correction;
      }

      for(j = p_tmp->frame_num; j < frame_num; j++) {
        p_tmp->p_sprite_table[j] = p_tmp->p_sprite_table[frame_num - j];
        p_tmp->p_delay_table[j] = p_tmp->frame_correction;
      }
    
      p_tmp->frame_num = frame_num;
      p_tmp->sprite_step = 0;
      p_tmp->sprite_first = 0;
      p_tmp->frame_correction = 0;
    }
  }    
}

anim_template_handle animation_repository::template_create(int flag, int frame_num)
{
  anim_template_handle new_template = template_get();
  
  assert(new_template);

  ANIMATION_TEMPLATE *p_tmp = p_tpl+new_template;

  p_tmp->flag = flag;
  p_tmp->frame_num = frame_num;
  
  return(new_template);
}

void animation_repository::template_shift(anim_template_handle handle, tpos dx, tpos dy)
{  
  ANIMATION_TEMPLATE *p_tmp = p_tpl+handle;
  
  p_tmp->dx = dx;
  p_tmp->dy = dy;
}

void animation_repository::template_sprites_static(anim_template_handle handle, 
                                                  spr_handle sprite_first, 
                                                  spr_handle sprite_num, 
                                                  spr_handle sprite_step)
{
  ANIMATION_TEMPLATE *p_tmp = p_tpl+handle;
  
  p_tmp->sprite_num = sprite_num;
  p_tmp->sprite_first = sprite_first;
  p_tmp->sprite_step = sprite_step;

  p_tmp->p_sprite_table = NULL;
  p_tmp->p_delay_table = NULL;
}

void animation_repository::template_sprites_dynamic(anim_template_handle handle,
                                                    int           frame_num,
                                                    spr_handle   *p_sprite_table,
                                                    int          *p_delay_table)
{
  ANIMATION_TEMPLATE *p_tmp = p_tpl+handle;

  p_tmp->frame_num = frame_num;
  p_tmp->p_sprite_table = (spr_handle *)mmemcpy(p_sprite_table, sizeof(p_tmp->p_sprite_table[0])*frame_num);
  p_tmp->p_delay_table = (int *)mmemcpy(p_delay_table, sizeof(p_tmp->p_delay_table[0])*frame_num);

  p_tmp->sprite_step = 0;
  p_tmp->sprite_first = 0;
}

anim_template_handle animation_repository::template_copy(anim_template_handle src)
{ 
  anim_template_handle new_template = template_get();
  
  assert(new_template);

  ANIMATION_TEMPLATE *p_tmp = p_tpl+new_template;

  *p_tmp = p_tpl[src];

  if(p_tpl[src].p_sprite_table)
    p_tmp->p_sprite_table = (spr_handle *)mmemcpy(p_tpl[src].p_sprite_table, sizeof(p_tmp->p_sprite_table[0])*p_tmp->frame_num);
  if(p_tpl[src].p_delay_table)
    p_tmp->p_delay_table = (int *)mmemcpy(p_tpl[src].p_delay_table, sizeof(p_tmp->p_delay_table[0])*p_tmp->frame_num);

  return(new_template);
}

void animation_repository::template_free(anim_template_handle handle)
{
  if(p_tpl[handle].flag&ANIM_USED) {
    ffree(p_tpl[handle].p_sprite_table);
    ffree(p_tpl[handle].p_delay_table);
    p_tpl[handle].flag &= ~ANIM_USED;
  }
}

#define DOOR_FRAMES 10

anim_template_handle animation_repository::template_generate(ANIM_TYPE type)
{
  anim_template_handle temp = template_create(ANIM_SPRITE|ANIM_LOOP, DOOR_FRAMES);

  spr_handle spr_array[DOOR_FRAMES];
  int time_array[DOOR_FRAMES];
  int i;

  switch(type) {
    case ANIM_DOOR_ID_H:
      spr_array[0] = FIRST_CYBER_LEVEL+39;
      spr_array[1] = FIRST_CYBER_LEVEL+97;
      break;
    case ANIM_DOOR_ID_V:
      spr_array[0] = FIRST_CYBER_LEVEL+30;
      spr_array[1] = FIRST_CYBER_LEVEL+60;
      break;
    case ANIM_DOOR_DV_H:
      spr_array[0] = FIRST_CYBER_LEVEL+79;
      spr_array[1] = FIRST_CYBER_LEVEL+76;
      break;
    case ANIM_DOOR_DV_V:
      spr_array[0] = FIRST_CYBER_LEVEL+73;
      spr_array[1] = FIRST_CYBER_LEVEL+70;
      break;
  }

  for(i = 3; i < DOOR_FRAMES; i += 2) {
    spr_array[i-1] = spr_array[i-3];
    spr_array[i] = spr_array[i-2];
  }
  for(i = 0; i < DOOR_FRAMES; i++) {
    time_array[i] = random()%30+1;
  }

  template_sprites_dynamic(temp,DOOR_FRAMES,spr_array,time_array);

  return(temp);
}

// -------------------------------------------------------
// Run-time animation
// -------------------------------------------------------
ANIMATION_REPOSITORY * animation::p_repository = NULL;

animation::animation(void)
{
  memset(this,0,sizeof(*this));
}

animation::~animation(void)
{    
}

void animation::init(anim_template_handle handle, tpos x_, tpos y_, tpos layer_, int rotation_)
{
  memset(this,0,sizeof(*this));

  thandle = handle;
  rotation = rotation_;
  
  frame_num = p_repository->get_frame_num(handle);
  flag = p_repository->get_flag(handle);

  x = x_;
  y = y_;

  layer = layer_;
}

int animation::post_event_add(LEVEL_EVENT *p_event)
{
  if(post_events_num < MAX_POST_EVENTS) {
    post_events[post_events_num++] = *p_event;
    return(true);
  } else {
    return(false);
  }
}

void animation::start(void)
{
  frame_current = 0;
  frame_correction = p_repository->get_frame_correction(thandle);
  position_in_animation = 0;
}

void animation::stop(LEVEL_EVENT_QUEUE *p_queue, int *p_events, bool remove = TRUE)
{
  // add events associated with this animation
  *p_events += end(p_queue);

  // remove from animation queue (current running animations)
  next_set(NULL);

  // remove from animation stack
  if(remove)
    set_used(false);  
}

int animation::end(LEVEL_EVENT_QUEUE *p_queue)
{    
  for(int i = 0; i < post_events_num; i++) {   
    p_queue->add(post_events[i]);
  }
  
  p_queue->commit();

  return(post_events_num);
}

void animation::process_sprite_animation(LEVEL_GAME *p_level)
{
  ANIMATION_TEMPLATE *p_template = p_repository->get(thandle);
  spr_handle spr = 0;

  if(!p_template->p_sprite_table) {
    spr = p_template->sprite_first + p_template->sprite_step * position_in_animation;
    assert(p_template->sprite_step * position_in_animation < p_template->sprite_num);
  } else {
    spr = p_template->p_sprite_table[position_in_animation];
  }

  if(rotation == NO_ROTATION) {
    p_level->cell_graphics_set(x, y, layer, spr, SPRITE_DYNAMIC);
  } else {
    p_level->cell_graphics_set_and_rotate(x, y, layer, spr, rotation, SPRITE_DYNAMIC);
  }
}

void animation::process_move_animation(LEVEL_GAME *p_level)
{ 
  ANIMATION_TEMPLATE *p_template = p_repository->get(thandle);  
  p_level->cell_graphics_add_diff(x, y, layer, p_template->dx, p_template->dy);
}

bool animation::process(LEVEL_GAME *p_level, LEVEL_EVENT_QUEUE *p_queue, int *p_events)
{ 

  if(flag&ANIM_REMOVE) {
    stop(p_queue, p_events, TRUE);
    return(true);
  }

  if(frame_current >= frame_num) {
    if(flag&(ANIM_LOOP))
      start();
    else {
      stop(p_queue, p_events, TRUE);
      return(true);
    }
  }

  if(frame_correction) {
    frame_correction--;
  } else {
    frame_correction = p_repository->get_frame_correction(thandle,position_in_animation);
  
    if(flag&ANIM_SPRITE) {
      process_sprite_animation(p_level);
    }

    if(flag&ANIM_MOVE) {
      process_move_animation(p_level);
    }    
  
    position_in_animation++;
    frame_current++;
  }  

  return(false);
}

// -------------------------------------------------------
// Animation engine
// -------------------------------------------------------
animation_engine::animation_engine(LEVEL_GAME *p_level_, DIR_LIST *p_dir)
  : animation_last(ANIM_QUEUE_FIRST), p_running(NULL), p_level(p_level_)
{
  #define ANIM_FILE "anim.dat"
  repository.repository_load(ANIM_FILE, p_dir->gamedata_get());
  repository.repository_modify();
  animation::repository_set(&repository);
}

animation_engine::~animation_engine(void)
{

}

anim_handle animation_engine::animation_find_next_free(void)
{
  int act;

  for(act = next_index(animation_last); act != animation_last; act = next_index(act)) {
      if(!(animation_stack[act].is_used())) {
        animation_last = act;
        return(act);
      }
  }
  return(A_ERROR);
}

anim_handle animation_engine::animation_new(anim_template_handle handle, 
                              tpos x, tpos y, tpos layer, int rotation)
{ 
  anim_handle anim = animation_find_next_free();  
  if(anim == A_ERROR)
    return(A_ERROR);

  animation_stack[anim].init(handle, x, y, layer, rotation);
  animation_stack[anim].set_used();
  
  return(anim);
}

int animation_engine::animation_process(LEVEL_EVENT_QUEUE *p_queue)
{
  ANIMATION *p_tmp = p_running;
  ANIMATION *p_running_new = NULL;
  int ret = 0;

  while(p_tmp) {
    ANIMATION *p_next = p_tmp->next_get();
  
    if(!p_tmp->process(p_level,p_queue,&ret)) {
      // add animation to new running queue
      p_tmp->next_set(p_running_new);
      p_running_new = p_tmp;
    }
  
    p_tmp = p_next;
  }

  p_running = p_running_new;

  return(ret);
}
