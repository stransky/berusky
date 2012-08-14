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

#ifndef __PROFILE_H__
#define __PROFILE_H__

#define LEVEL_SET_NUM             5

#define PROFILE_NAME              "name"
#define PROFILE_LAST_TRAINING     "l0"
#define PROFILE_LAST_EASY         "l1"
#define PROFILE_LAST_INTERMEDIATE "l2"
#define PROFILE_LAST_ADVANCED     "l3"
#define PROFILE_LAST_IMPOSSIBLE   "l4"

typedef struct profile_level_set {

  int level_selected; // selected level
  int level_last;     // last finished level

} PROFILE_LEVEL_SET;

// Player profile
typedef struct berusky_profile {

  char              filename[MAX_FILENAME];  
  char              profile_name[MAX_FILENAME];
  PROFILE_LEVEL_SET level_set[LEVEL_SET_NUM];

public:
  
  void create(const char *p_name);
  void load(const char *p_dir, const char *p_file); 

public:
  
  berusky_profile(void);

} BERUSKY_PROFILE;

// BERUSKY_PROFILE *p_profiles array has to be released by delete []
bool profiles_load(BERUSKY_PROFILE **p_profiles, int *p_num);

// Try to load last used profile (defined by p_name, if it fails load the first one,
// if it fails return an error
bool profile_load_last(char *p_name, BERUSKY_PROFILE &profile);

#endif // __PROFILE_H__
