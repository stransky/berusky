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

berusky_profile::berusky_profile(void)
{  
  create("Default");
}

void berusky_profile::selected_level_set(int level)
{
  if(level > level_set[level_set_selected].level_last)
    level = level_set[level_set_selected].level_last;
  
  level_set[level_set_selected].level_selected = level;
}

int  berusky_profile::selected_level_get(void)
{
  return(level_set[level_set_selected].level_selected);
}

void berusky_profile::last_level_set(int level)
{
  level_set[level_set_selected].level_last = level;
}

int  berusky_profile::last_level_get(void)
{
  return(level_set[level_set_selected].level_last);
}

void berusky_profile::level_set_set(int level_set_num)
{
  assert(level_set_num >= 0 && level_set_num < LEVEL_SET_NUM);
  level_set_selected = level_set_num;
  selected_level_set(level_set[level_set_num].level_selected);
}

int  berusky_profile::level_set_get(void)
{
  return(level_set_selected);
}

void berusky_profile::selected_level_finished(void)
{
  int selected_level = selected_level_get();

  if(selected_level == last_level_get()) {
    selected_level += 1;
    last_level_set(selected_level);
    selected_level_set(selected_level);
  }
}

void berusky_profile::load(const char *p_dir, const char *p_file)
{
  char tmp[MAX_FILENAME];
  return_path(p_dir, p_file, tmp, MAX_FILENAME);

  ini_read_string_file(tmp, PROFILE_NAME, 
                       profile_name, sizeof(profile_name), "Default");

  level_set[0].level_selected = level_set[0].level_last = ini_read_int_file(tmp, PROFILE_LAST_TRAINING, 0);
  level_set[1].level_selected = level_set[1].level_last = ini_read_int_file(tmp, PROFILE_LAST_EASY, 0);
  level_set[2].level_selected = level_set[2].level_last = ini_read_int_file(tmp, PROFILE_LAST_INTERMEDIATE, 0);
  level_set[3].level_selected = level_set[3].level_last = ini_read_int_file(tmp, PROFILE_LAST_ADVANCED, 0);
  level_set[4].level_selected = level_set[4].level_last = ini_read_int_file(tmp, PROFILE_LAST_IMPOSSIBLE, 0);
}

void berusky_profile::save(void)
{ 
  char buffer[1024];
  snprintf(buffer, 1024,
           "name = %s\nl0 = %d\nl1 = %d\nl2 = %d\nl3 = %d\nl4 = %d\n",
           profile_name,
           level_set[0].level_last,
           level_set[1].level_last,
           level_set[2].level_last,
           level_set[3].level_last,
           level_set[4].level_last);
  file_save(INI_USER_PROFILES, profile_name, (void *)buffer, 1024, "w");
}

void berusky_profile::create(const char *p_name)
{
  memset(this,0,sizeof(*this));
  strcpy(profile_name, p_name);
}

// Scan the directory for all profile files
bool profiles_load(BERUSKY_PROFILE **p_profiles, int *p_num)
{
  #define PROFILE_FILE_MASK "*.ini"
  DIRECTORY_ENTRY *p_profile_names;

  *p_profiles = NULL;
  *p_num = 0;

  int files = file_list_get(INI_USER_PROFILES, PROFILE_FILE_MASK, &p_profile_names);
  if(!files)
    return(FALSE);
  
  BERUSKY_PROFILE *p_list = new BERUSKY_PROFILE[files];
  for(int i = 0; i < files; i++) {
    p_list[i].load(INI_USER_PROFILES, p_profile_names[i].name);
  }
  ffree(p_profile_names);
  
  *p_profiles = p_list;
  *p_num = files;

  return(TRUE);
}

// Try to load last used profile (defined by p_name, if it fails load the first one, 
// if it fails return an error
bool profile_load_last(char *p_name, BERUSKY_PROFILE &profile)
{
  BERUSKY_PROFILE *p_profiles = NULL;
  int profile_num = 0;

  if(!profiles_load(&p_profiles, &profile_num))
    return(FALSE);   
  
  int i;
  for(i = 0; i < profile_num; i++) {
    if(!strncmp(p_profiles[i].profile_name, p_name, MAX_FILENAME)) {
      profile = p_profiles[i];
      break;
    }
  }

  if(i == profile_num) {
    // select the first one
    profile = p_profiles[0];
  }

  delete [] p_profiles;
  return(TRUE);
}
