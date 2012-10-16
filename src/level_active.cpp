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

/* *******************************************************
    Level Store
   *******************************************************
*/
void level_store::alloc(int num)
{
  if(levelnum <= num) {
    free();
    p_list = new LEVEL_STORE_ITEM [num];
    assert(p_list);
    if(!p_list) {
      abort();
    }
    levelnum = num;
  }
}

void level_store::free(void)
{
  if(p_list) {
    delete [] p_list;
    p_list = NULL;
  }
  levelnum = 0;
}

bool level_store::level_load(DIR_LIST *p_dir_, char *p_level)
{
  alloc(1);

  p_dir = p_dir_;

  strncpy(p_list[0].levelname, p_level, sizeof(p_list[0].levelname));
  p_list[0].password[0] = '\0';

  // It's an user level
  status.user_set(TRUE);

  return(true);
}

char * level_store::is_valid_line(char *p_line)
{
  while(*p_line && isspace(*p_line))
    p_line++;
  return(*p_line == '\0' || *p_line == '#' ? NULL : p_line);
}

bool level_store::levelset_load(DIR_LIST *p_dir_, char *p_script)
{
  p_dir = p_dir_;

  FHANDLE f = file_open(p_dir->gamedata_get(),p_script,"r");
  char *p_start = NULL;
  
  char tmp[2000];
  int  i,max;
  
  for(i = 0, max = 0; fgets(tmp,sizeof(tmp),f); i++) {    
    if(is_valid_line(tmp))
        max++;  
  }
  
  max /= 2;
  
  alloc(max);
  
  fseek(f,0,SEEK_SET);
  
  for(i = 0; i < max; i++) {
  
    while((fgets(tmp,sizeof(tmp),f)) && !(p_start = is_valid_line(tmp)));
    if(!p_start) {
      i--;
      break;
    }
    strncpy(p_list[i].levelname,p_start,sizeof(p_list[i].levelname));
    fgets_correction(p_list[i].levelname);
  
    while((fgets(tmp,sizeof(tmp),f)) && !(p_start = is_valid_line(tmp)));
    if(!p_start) // We don't have any password for this level -> error
      return(false);
    strncpy(p_list[i].password,p_start,sizeof(p_list[i].password));
    fgets_correction(p_list[i].password);
  }

  levelnum = i;

  bprintf("Levelnum %d",levelnum);

  // It's level from game
  status.user_set(FALSE);

  return(true);
}

bool level_store::levelset_search(char *p_passwd, int *p_level)
{
  if(levelnum) {
    int i;
    for(i = 0; i < levelnum; i++) {
#ifdef LINUX
      if(!strncasecmp(p_list[i].password, p_passwd, MAX_PASSWORD)) {
#elif WINDOWS
      if(!_strnicmp(p_list[i].password, p_passwd, MAX_PASSWORD)) {
#endif
        *p_level = i;
        return(TRUE);
      }
    }
  }
  return(FALSE);
}

char * level_store::levelset_get_passwd(int level)
{
  if(levelnum) {
    return(p_list[level].password);
  }
  return(NULL);
}

char * level_store::levelset_get_name(int level)
{
  if(levelnum) {
    return(p_list[level].levelname);
  }
  return(NULL);
}

char * level_store::levelset_get_name_custom(void)
{
  return(levelset_get_name(0));
}

bool level_store::levelset_is_level(int level)
{
  return(level < levelnum);
}

int  level_store::levelset_get_levelnum(void)
{
  return(levelnum);
}

bool level_store::play_start(ITEM_REPOSITORY *p_repo, LEVEL_EVENT_QUEUE *p_queue, int level)
{
  assert(!p_active);

  if(level < levelnum) {
    status.game_start();
    p_repos = p_repo;
    p_active = new LEVEL_ACTIVE(p_queue, p_repos, &status, p_dir, p_list[level].levelname);
    return(p_active->loaded());
  } else {  
    return(false);
  }
}

bool level_store::play_callback(LEVEL_EVENT_QUEUE *p_queue)
{
  if(p_active && p_active->loaded()) {
    p_active->play_callback(p_queue);
    return(true);
  } else {
    return(false);
  }
}

bool level_store::play_end(LEVEL_EVENT_QUEUE *p_queue)
{
  if(p_active) {
    status.game_stop();
    delete p_active;
    p_active = NULL;
    return(true);
  } else {
    return(false);
  }
}

LEVEL_STATUS * level_store::status_get(void)
{
  return(&status);
}

void level_store::redraw(void)
{
  if(p_active)
    p_active->redraw();
}

void level_store::level_save(void)
{
  p_active->level_save();
}

bool level_store::level_load(void)
{
  return(p_active->level_load());
}

/***************************************************************
  Active level
 ***************************************************************
*/

/* Scan level and add events (animations and so on...) */
void level_active::add_level_events(LEVEL_EVENT_QUEUE *p_queue)
{  
  tpos x,y,l;

  for(x = 0; x < LEVEL_CELLS_X; x++) {
    for(y = 0; y < LEVEL_CELLS_Y; y++) {
      for(l = 0; l < LEVEL_LAYERS; l++) {
        if(level.cell_is_full(x,y,l)) {
          if(level.repo_get_flag(x,y,l)&ANIM_TRIGGER_INSERT) {
            // static items do not have fast animations
            p_queue->add(LEVEL_EVENT(AN_GENERATE_AND_RUN, x, y, l, level.repo_get_animation(x,y,l,FALSE), 0, NO_ROTATION));
          }
        }
      }
    }
  }

  p_queue->commit();
}

/* Load specified level into memory and initialize it */
level_active::level_active(LEVEL_EVENT_QUEUE *p_queue,
                           ITEM_REPOSITORY *p_repo_,
                           LEVEL_STATUS *p_status_,
                           DIR_LIST *p_dir,
                           char *p_name):

scr(CELL_SIZE_X,CELL_SIZE_Y),
level(&scr, p_repo_, p_status_),
logic(&level,p_status_),
changer(&level,p_dir)

{
  p_repo = p_repo_;
  p_status = p_status_;

  // Clear saved level
  p_saved = NULL;

  // Set coordinates of level
  level.window_offset_set(LEVEL_SCREEN_START_X, LEVEL_SCREEN_START_Y);
  
  // Load user levels from user-levels, cwd and w/o path
  if(p_status->user_get()) {
    bprintf(_("User defined level %s..."),p_name); 
  
    const char *p_paths[] = { p_dir->levels_user_get(), p_dir->cwd_get(), NULL };
      
    load = level.level_load(p_name, p_paths, sizeof(p_paths)/sizeof(p_paths[0]));
  }
  else {
    bprintf(_("Game level %s..."),p_name);
  
    const char *p_paths[] = { p_dir->levels_get() };
  
    load = level.level_load(p_name, p_paths, sizeof(p_paths)/sizeof(p_paths[0]));    
  }
    
  if(!load) {
    bprintf(_("Unable to load level %s"),p_name);
  } else {
    level.level_populate();
    add_level_events(p_queue);
  }
}

/* Free level from memory */
level_active::~level_active(void)
{
  if(p_saved) {    
    delete p_saved;
    p_saved = NULL;
  }
}

void level_active::level_save(void)
{
  if(!p_saved) {
    p_saved = new LEVEL_GAME(&scr, p_repo, p_status);
  }
  *p_saved = level;
}

bool level_active::level_load(void)
{
  if(p_saved) {
    level = *p_saved;
    level.level_populate();
    return(TRUE);
  } else {
    return(FALSE);
  }
}

/* play callback - should be called  */
bool level_active::play_callback(LEVEL_EVENT_QUEUE *p_queue)
{
  // Get and process all events in level
  // Process events from user

#if EVENTS_DEBUG
  p_queue->dump_read("Read - before logic");
#endif

  logic.events_process(p_queue);

#if EVENTS_DEBUG
  p_queue->dump_read("Read - before changer");
#endif

  changer.events_process(p_queue);

#if EVENTS_DEBUG
  p_queue->dump_read("Read - after changer");
#endif
  
  // Redraw level/screen  
  level.level_draw();
    
  return(TRUE);
}
