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

#ifndef __LEVEL_ACTIVE_H__
#define __LEVEL_ACTIVE_H__

/***************************************************************
  Active level
 ***************************************************************
*/
typedef class level_active {

  SCREEN            scr;

  LEVEL_GAME        level;    // active level
  LEVEL_GAME       *p_saved;  // saved level

  GAME_LOGIC        logic;
  LEVEL_CHANGER     changer;

  ITEM_REPOSITORY  *p_repo;
  LEVEL_STATUS     *p_status;  

  bool              load;

  void add_level_events(LEVEL_EVENT_QUEUE *p_queue);

public:
  
  level_active(LEVEL_EVENT_QUEUE *p_queue,ITEM_REPOSITORY *p_repo_,
               LEVEL_STATUS *p_status_,
               DIR_LIST *p_dir, char *p_name);
  ~level_active(void);
  
  bool play_callback(LEVEL_EVENT_QUEUE *p_queue);

  bool loaded(void)
  { 
    return(load);
  }

  void redraw(void)
  { 
    level.level_redraw();
  }

  void level_save(void);
  bool level_load(void);

} LEVEL_ACTIVE;

/********************************************************
   Level Store
  *******************************************************
*/
#define MAX_LEVELNAME 50
#define MAX_PASSWORD  10

typedef struct level_store_item {

  char levelname[MAX_LEVELNAME];
  char password[MAX_PASSWORD];

} LEVEL_STORE_ITEM;

typedef class level_store {

  // Level in progress
  LEVEL_ACTIVE     *p_active;

  LEVEL_STORE_ITEM *p_list;
  int               levelnum;

  DIR_LIST         *p_dir;
  ITEM_REPOSITORY  *p_repos;

  LEVEL_STATUS      status;

public:

  char * is_valid_line(char *p_line);

  void   alloc(int num);
  void   free(void);
    
  bool   level_load(DIR_LIST *p_dir_, char *p_level);
  bool   levelset_load(DIR_LIST *p_dir_, char *p_script);
  bool   levelset_is_level(int level);
  int    levelset_get_levelnum(void);
  char * levelset_get_difficulty(int set);
  char * levelset_get_passwd(int level);
  char * levelset_get_name(int level);
  char * levelset_get_name_custom(void);
  bool   levelset_search(char *p_passwd, int *p_level);

  bool   play_start(ITEM_REPOSITORY *p_repo, LEVEL_EVENT_QUEUE *p_queue, int level);
  bool   play_callback(LEVEL_EVENT_QUEUE *p_queue);
  bool   play_end(LEVEL_EVENT_QUEUE *p_queue);

  void   level_save(void);
  bool   level_load(void);

  void   redraw(void);
  
  LEVEL_STATUS * status_get(void);

  level_store()      { memset(this,0,sizeof(*this)); }
  ~level_store(void) { free(); }

} LEVEL_STORE;

#endif // __LEVEL_ACTIVE_H__
