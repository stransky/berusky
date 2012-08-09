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

void berusky_profile::load(const char *p_dir, const char *p_file)
{
  char tmp[MAX_FILENAME];
  return_path(p_dir, p_file, tmp, MAX_FILENAME);

  ini_read_string_file(tmp, PROFILE_NAME, profile_name, sizeof(profile_name), "Default");
  level_set[0].level_last = ini_read_int_file(tmp, PROFILE_LAST_TRAINING, 0);
  level_set[1].level_last = ini_read_int_file(tmp, PROFILE_LAST_EASY, 0);
  level_set[2].level_last = ini_read_int_file(tmp, PROFILE_LAST_INTERMEDIATE, 0);
  level_set[3].level_last = ini_read_int_file(tmp, PROFILE_LAST_ADVANCED, 0);
  level_set[4].level_last = ini_read_int_file(tmp, PROFILE_LAST_IMPOSSIBLE, 0);
}

void berusky_profile::create(const char *p_name)
{
  memset(this,0,sizeof(*this));
  strcpy(profile_name, p_name);
}

// Scan the directory for all profile files
bool profiles_load(const char *p_dir, BERUSKY_PROFILE **p_profiles, int *p_num)
{
  #define PROFILE_FILE_MASK "*.ini"
  DIRECTORY_ENTRY *p_profile_names;

  *p_profiles = NULL;
  *p_num = 0;

  int files = file_list_get(p_dir, PROFILE_FILE_MASK, &p_profile_names);
  if(!files)
    return(FALSE);
  
  BERUSKY_PROFILE *p_list = new BERUSKY_PROFILE[files];
  for(int i = 0; i < files; i++) {
    p_list[i].load(p_dir, p_profile_names[i].name);
  }
  ffree(p_profile_names);
  
  *p_profiles = p_list;
  *p_num = files;

  return(TRUE);
}
