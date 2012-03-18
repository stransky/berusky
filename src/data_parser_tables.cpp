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

POSITION_TABLE_ITEM parser_table[] = 
{
  {"P_GROUND",P_GROUND},
  {"P_PLAYER_1",P_PLAYER_1},
  {"P_PLAYER_2",P_PLAYER_2},
  {"P_PLAYER_3",P_PLAYER_3},
  {"P_PLAYER_4",P_PLAYER_4},
  {"P_PLAYER_5",P_PLAYER_5},
  {"P_BOX",P_BOX},
  {"P_TNT",P_TNT},
  {"P_WALL",P_WALL},
  {"P_EXIT",P_EXIT},
  {"P_STONE",P_STONE},
  {"P_KEY",P_KEY},
  {"P_MATTOCK",P_MATTOCK},
  {"P_KEY1",P_KEY1},
  {"P_KEY2",P_KEY2},
  {"P_KEY3",P_KEY3},
  {"P_KEY4",P_KEY4},
  {"P_KEY5",P_KEY5},

  {"P_DOOR1_H_O",P_DOOR1_H_O},
  {"P_DOOR2_H_O",P_DOOR2_H_O},
  {"P_DOOR3_H_O",P_DOOR3_H_O},
  {"P_DOOR4_H_O",P_DOOR4_H_O},
  {"P_DOOR5_H_O",P_DOOR5_H_O},

  {"P_DOOR1_H_Z",P_DOOR1_H_Z},
  {"P_DOOR2_H_Z",P_DOOR2_H_Z},
  {"P_DOOR3_H_Z",P_DOOR3_H_Z},
  {"P_DOOR4_H_Z",P_DOOR4_H_Z},
  {"P_DOOR5_H_Z",P_DOOR5_H_Z},

  {"P_DOOR1_V_O",P_DOOR1_V_O},
  {"P_DOOR2_V_O",P_DOOR2_V_O},
  {"P_DOOR3_V_O",P_DOOR3_V_O},
  {"P_DOOR4_V_O",P_DOOR4_V_O},
  {"P_DOOR5_V_O",P_DOOR5_V_O},

  {"P_DOOR1_V_Z",P_DOOR1_V_Z},
  {"P_DOOR2_V_Z",P_DOOR2_V_Z},
  {"P_DOOR3_V_Z",P_DOOR3_V_Z},
  {"P_DOOR4_V_Z",P_DOOR4_V_Z},
  {"P_DOOR5_V_Z",P_DOOR5_V_Z},

  {"P_ID_DOOR1_H_O",P_ID_DOOR1_H_O},
  {"P_ID_DOOR2_H_O",P_ID_DOOR2_H_O},
  {"P_ID_DOOR3_H_O",P_ID_DOOR3_H_O},
  {"P_ID_DOOR4_H_O",P_ID_DOOR4_H_O},
  {"P_ID_DOOR5_H_O",P_ID_DOOR5_H_O},

  {"P_ID_DOOR1_H_Z",P_ID_DOOR1_H_Z},
  {"P_ID_DOOR2_H_Z",P_ID_DOOR2_H_Z},
  {"P_ID_DOOR3_H_Z",P_ID_DOOR3_H_Z},
  {"P_ID_DOOR4_H_Z",P_ID_DOOR4_H_Z},
  {"P_ID_DOOR5_H_Z",P_ID_DOOR5_H_Z},

  {"P_ID_DOOR1_V_O",P_ID_DOOR1_V_O},
  {"P_ID_DOOR2_V_O",P_ID_DOOR2_V_O},
  {"P_ID_DOOR3_V_O",P_ID_DOOR3_V_O},
  {"P_ID_DOOR4_V_O",P_ID_DOOR4_V_O},
  {"P_ID_DOOR5_V_O",P_ID_DOOR5_V_O},

  {"P_ID_DOOR1_V_Z",P_ID_DOOR1_V_Z},
  {"P_ID_DOOR2_V_Z",P_ID_DOOR2_V_Z},
  {"P_ID_DOOR3_V_Z",P_ID_DOOR3_V_Z},
  {"P_ID_DOOR4_V_Z",P_ID_DOOR4_V_Z},
  {"P_ID_DOOR5_V_Z",P_ID_DOOR5_V_Z},

  {"P_DV_H_O",P_DV_H_O},
  {"P_DV_H_Z",P_DV_H_Z},
  {"P_DV_V_O",P_DV_V_O},
  {"P_DV_V_Z",P_DV_V_Z},

  {"P_DV_H",P_DV_H},
  {"P_DV_V",P_DV_V},

  {"PP_LEFT_JAMB_O", PP_LEFT_JAMB_O},
  {"PP_RIGHT_JAMB_O", PP_RIGHT_JAMB_O},
  {"PP_TOP_JAMB_O", PP_TOP_JAMB_O},
  {"PP_BOTTOM_JAMB_O", PP_BOTTOM_JAMB_O},

  {"PP_LEFT_JAMB_Z", PP_LEFT_JAMB_Z},
  {"PP_RIGHT_JAMB_Z", PP_RIGHT_JAMB_Z},
  {"PP_TOP_JAMB_Z", PP_TOP_JAMB_Z},
  {"PP_BOTTOM_JAMB_Z", PP_BOTTOM_JAMB_Z},

  {"ANIM_TRIGGER_MOVE", ANIM_TRIGGER_MOVE},
  {"ANIM_TRIGGER_ERASE", ANIM_TRIGGER_ERASE},
  {"ANIM_TRIGGER_INSERT", ANIM_TRIGGER_INSERT},

  {"FIRST_GLOBAL_LEVEL", FIRST_GLOBAL_LEVEL},
  {"FIRST_CLASSIC_LEVEL", FIRST_CLASSIC_LEVEL},
  {"FIRST_CYBER_LEVEL", FIRST_CYBER_LEVEL},
  {"FIRST_BACKGROUND", FIRST_BACKGROUND},
  {"FIRST_OTHER", FIRST_OTHER},
  {"FIRST_PLAYER", FIRST_PLAYER},
  {"FIRST_MENU", FIRST_MENU},

  {"SPRITE_STATIC", SPRITE_STATIC},
  {"SPRITE_DYNAMIC", SPRITE_DYNAMIC},
  {"SPRITE_NO_ROTATE", SPRITE_NO_ROTATE},

  {"ANIM_PLAYER_1_FAST", ANIM_PLAYER_1_FAST},
  {"ANIM_PLAYER_2_FAST", ANIM_PLAYER_2_FAST},
  {"ANIM_PLAYER_3_FAST", ANIM_PLAYER_3_FAST},
  {"ANIM_PLAYER_4_FAST", ANIM_PLAYER_4_FAST},
  {"ANIM_PLAYER_5_FAST", ANIM_PLAYER_5_FAST},

  {"ANIM_PLAYER_1", ANIM_PLAYER_1},
  {"ANIM_PLAYER_2", ANIM_PLAYER_2},
  {"ANIM_PLAYER_3", ANIM_PLAYER_3},
  {"ANIM_PLAYER_4", ANIM_PLAYER_4},
  {"ANIM_PLAYER_5", ANIM_PLAYER_5},

  {"ANIM_PLAYER_FRAMES_FAST",ANIM_MOVE_FRAMES_FAST},
  {"ANIM_PLAYER_FRAMES",ANIM_MOVE_FRAMES},

  {"ANIM_MOVE_UP_FAST",   ANIM_MOVE_UP_FAST},
  {"ANIM_MOVE_DOWN_FAST", ANIM_MOVE_DOWN_FAST},
  {"ANIM_MOVE_LEFT_FAST", ANIM_MOVE_LEFT_FAST},
  {"ANIM_MOVE_RIGHT_FAST",ANIM_MOVE_RIGHT_FAST},

  {"ANIM_MOVE_UP",   ANIM_MOVE_UP},
  {"ANIM_MOVE_DOWN", ANUM_MOVE_DOWN},
  {"ANIM_MOVE_LEFT", ANIM_MOVE_LEFT},
  {"ANIM_MOVE_RIGHT",ANIM_MOVE_RIGHT},

  {"ANIM_STONE_1",  ANIM_STONE_1},
  {"ANIM_STONE_1_FRAME_FIRST", FIRST_CLASSIC_LEVEL+1},

  {"ANIM_STONE_2",  ANIM_STONE_2},
  {"ANIM_STONE_2_FRAME_FIRST", FIRST_CYBER_LEVEL+47},

  {"ANIM_STONE_FRAMES", 3},

  {"ANIM_EXIT_1", ANIM_EXIT_1},
  {"ANIM_EXIT_1_FRAMES",4},
  {"ANIM_EXIT_1_FRAME_FIRST",FIRST_CLASSIC_LEVEL+42},

  {"ANIM_BLAST",  ANIM_BLAST},
  {"ANIM_BLAST_FRAMES", 7},
  {"ANIM_BLAST_FRAME_FIRST", FIRST_GLOBAL_LEVEL+7},

  {"ANIM_MOVE",         ANIM_MOVE},
  {"ANIM_SPRITE",       ANIM_SPRITE},
  {"ANIM_INSTANT",      ANIM_INSTANT},
  {"ANIM_LOOP",         ANIM_LOOP},
  {"ANIM_ADD_INVERSE",  ANIM_ADD_INVERSE},
  {"ANIM_GENERATE",     ANIM_GENERATE},

  {"ANIM_DOOR_ID_H",    ANIM_DOOR_ID_H},
  {"ANIM_DOOR_ID_V",    ANIM_DOOR_ID_V},
  {"ANIM_DOOR_DV_H",    ANIM_DOOR_DV_H},
  {"ANIM_DOOR_DV_V",    ANIM_DOOR_DV_V}

};

POSITION_TABLE_ITEM * get_parser_table(void)
{
  return(parser_table);
}

int get_parser_table_items(void)
{
  return(sizeof(parser_table) / sizeof(parser_table[0]));
}
