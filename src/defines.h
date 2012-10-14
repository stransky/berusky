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

#ifndef __DEFINES_H__
#define __DEFINES_H__

/* Basic constants
*/
#define GAME_FPS                  (berusky_config::game_fps)  // game refresh

#define FULLSCREEN                (berusky_config::fullscreen)

#define SCREEN_DEPTH              (berusky_config::screen_depth)
#define SCREEN_DEPTH_DEFAULT      32

#define DOUBLE_SIZE               (berusky_config::double_size_gui)

#define GAME_RESOLUTION_X         (berusky_config::game_resolution_x)
#define GAME_RESOLUTION_Y         (berusky_config::game_resolution_y)

#define LEVEL_SCREEN_START_X      (berusky_config::level_screen_start_x)
#define LEVEL_SCREEN_START_Y      (berusky_config::level_screen_start_y)

#define LEVEL_RESOLUTION_X        (berusky_config::level_resolution_x)
#define LEVEL_RESOLUTION_Y        (berusky_config::level_resolution_y)

#define EDITOR_RESOLUTION_X       (berusky_config::editor_resolution_x)
#define EDITOR_RESOLUTION_Y       (berusky_config::editor_resolution_y)

#define SCREEN_TOP_PANNEL_X       0
#define SCREEN_TOP_PANNEL_Y       0
#define SCREEN_TOP_PANNEL_DX      (DOUBLE_SIZE ? GAME_RESOLUTION_X/2 : GAME_RESOLUTION_X)
#define SCREEN_TOP_PANNEL_DY      40

#define INSERT_FIRST              0
#define INSERT_APPEND            (-1)
#define M_ERROR                  (-1)
#define A_ERROR                  (-1)

#define SELECT_PLAYER_FIRST      (-1)
#define SELECT_PLAYER_NEXT       (-2)

#define LEVEL_CELLS_X             32
#define LEVEL_CELLS_Y             21

#define IN_LEVEL(x,y)             ((x) >= 0 && (x) < LEVEL_CELLS_X && \
                                   (y) >= 0 && (y) < LEVEL_CELLS_Y)

#define CELL_SIZE_X               (berusky_config::cell_size_x)
#define CELL_SIZE_Y               (berusky_config::cell_size_y)

#define ITEM_SIZE_X               (20*3)
#define ITEM_SIZE_Y               (20*3)

#define EDITOR_LAYER_STATUS_X     (ITEM_SIZE_X*2)
#define EDITOR_LAYER_STATUS_Y     (ITEM_SIZE_Y*2+5)
#define EDITOR_LAYER_STATUS_DX    (EDITOR_RESOLUTION_X-EDITOR_LAYER_STATUS_X)
#define EDITOR_LAYER_STATUS_DY    (30)

#define EDITOR_SCREEN_START_X     (berusky_config::editor_screen_start_x)
#define EDITOR_SCREEN_START_Y     (berusky_config::editor_screen_start_y)

#define EDIT_COORD_START_X        (DOUBLE_SIZE ? ITEM_SIZE_X*2 : EDITOR_SCREEN_START_X)
#define EDIT_COORD_START_Y        (EDITOR_SCREEN_START_Y+(DOUBLE_SIZE ? LEVEL_RESOLUTION_Y-140 : LEVEL_RESOLUTION_Y))
#define EDIT_COORD_DX             (DOUBLE_SIZE ? LEVEL_RESOLUTION_X/2-200 : LEVEL_RESOLUTION_X)
#define EDIT_COORD_DY             (20)

#define EDIT_ITEM_START_X         (EDIT_COORD_START_X)
#define EDIT_ITEM_START_Y         (EDIT_COORD_START_Y+EDIT_COORD_DY)
#define EDIT_ITEM_DX              (DOUBLE_SIZE ? LEVEL_RESOLUTION_X/2-200 : LEVEL_RESOLUTION_X)
#define EDIT_ITEM_DY              (20)

#define EDIT_ITEM_PICT_START_X    (EDIT_ITEM_START_X)
#define EDIT_ITEM_PICT_START_Y    (EDIT_ITEM_START_Y+EDIT_ITEM_DY+10)

#define EDIT_HELP_KEY_X           40
#define EDIT_HELP_KEY_DX          120
#define EDIT_HELP_START_Y         40
#define EDIT_HELP_DY              10

#define EDIT_HELP_MOUSE_X         460

#define CONSOLE_X                 (EDITOR_LAYER_STATUS_X)
#define CONSOLE_Y                 (EDIT_ITEM_PICT_START_Y+ITEM_SIZE_Y+5)
#define CONSOLE_DX                (DOUBLE_SIZE ? 700 : (EDITOR_RESOLUTION_X-CONSOLE_X-200))
#define CONSOLE_LINES             2

#define LEVELSET_NUM              5

#define LAYER_NONE                (-1)
#define LAYER_FLOOR               0
#define LAYER_ITEMS               1
#define LAYER_PLAYER              2
#define LAYER_GRID                3

#define LEVEL_LAYERS              3
#define ALL_LEVEL_LAYERS          4

#define SPRITE_CLEAR              0x1
#define SPRITE_NO_DRAW            0x2
#define SPRITE_STATIC             0x4
#define SPRITE_DYNAMIC            0x8
#define SPRITE_NO_ROTATE          0x10

#define GLOBAL_SPRITES            15
#define CLASSIC_SPRITES           93
#define CYBER_SPRITES             98
#define GAME_SPRITES              59
#define PLAYER_SPRITES            50
#define BACKGROUND_SPRITES        7
#define MENU_SPRITES              58
#define TEXT_LENGHT               70000
#define FONT_SPRITES              61
#define FONT_STEP                 100
#define CURSOR_SPRITES            2

#define FIRST_GLOBAL_LEVEL        0
#define FIRST_CLASSIC_LEVEL       100
#define FIRST_CYBER_LEVEL         200
#define FIRST_BACKGROUND          400
#define FIRST_OTHER               410
#define FIRST_CURSOR              590
#define FIRST_PLAYER              600
#define FIRST_MENU                700
#define FIRST_LOGO                800
#define FIRST_FONT                1000
#define FIRST_BOX_BRIGHT          1100
#define FIRST_BOX_DARK            1150
#define FIRST_BOX_PAPER           1200
#define FIRST_BOX_SNOW            1250
#define FIRST_FLOOR_DANGER        1300
#define FIRST_FLOOR_ELEVATORS     1350
#define FIRST_FLOOR_GRAY          1400
#define FIRST_LIGHT_BOX           1450
#define FIRST_TNT_BRIGHT          1500
#define FIRST_TNT_DARK            1550
#define FIRST_TNT_PAPER           1600
#define FIRST_TNT_SNOW            1650
#define FIRST_TNT_SWAMP           1700
#define FIRST_WALL_IRON_BLUE      1750
#define FIRST_WALL_IRON_BROWN     1800
#define FIRST_WALL_IRON_DARK      1850
#define FIRST_WALL_IRON_GRAY      1900
#define FIRST_WALL_MACHINE        1950
#define FIRST_WALL_REPRO          2000
#define FIRST_WALL_SNOW           2050
#define FIRST_WALL_SWAMP          2100
#define FIRST_WALL_WOOD           2150
#define SPRITE_BLACK              2200

#define ROT_SHIFT                 10000

#define MENU_SPRIT_ROCK           FIRST_MENU
#define MENU_SPRIT_END            (FIRST_MENU+1)
#define MENU_SPRIT_LOGO           (FIRST_MENU+2)
#define MENU_SPRIT_WALL           (FIRST_MENU+3)
#define MENU_SPRIT_BACK           (FIRST_MENU+4)
#define MENU_SPRIT_BACK1          (FIRST_MENU+5)
#define MENU_SPRIT_BACK2          (FIRST_MENU+6)
#define MENU_SPRIT_BACK3          (FIRST_MENU+7)
#define MENU_SPRIT_LOGO_SMALL_1   (FIRST_MENU+8)
#define MENU_SPRIT_LOGO_SMALL_2   (FIRST_MENU+9)
#define MENU_SPRIT_LOGO_SMALL_3   (FIRST_MENU+10)
#define MENU_SPRIT_LOGO_SMALL_NUM 3

#define MENU_SPRIT_ARROWS         (MENU_SPRIT_LOGO_SMALL_1+MENU_SPRIT_LOGO_SMALL_NUM)
#define MENU_ARROWS_NUM           4

#define MENU_SPRIT_ARROW_L        (MENU_SPRIT_ARROWS)
#define MENU_SPRIT_ARROW_LC       (MENU_SPRIT_ARROWS+1)
#define MENU_SPRIT_ARROW_R        (MENU_SPRIT_ARROWS+2)
#define MENU_SPRIT_ARROW_RC       (MENU_SPRIT_ARROWS+3)

#define MENU_CHECKBOX_CHECKED     (MENU_SPRIT_ARROWS+MENU_ARROWS_NUM)
#define MENU_CHECKBOX_UNCHECKED   (MENU_CHECKBOX_CHECKED+1)
#define MENU_CHECKBOX_NUM         2
#define MENU_SLIDEBAR             (MENU_CHECKBOX_CHECKED+2)
#define MENU_SLIDER               (MENU_CHECKBOX_CHECKED+3)

#define MENU_SPRIT_START          (FIRST_MENU+50)
#define ANIM_PLAYER_SPRITES       10

#define FIRST_KEY                 (FIRST_OTHER+4)
#define X_KEYS_POSITION           (550)
#define Y_KEYS_POSITION           0

#define PLAYER_MARK               (FIRST_CURSOR)
#define EDIT_ZEME                 (FIRST_OTHER+10)
#define UKAZATEL_3                (FIRST_OTHER+15)

#define EDIT_ARROW_LEFT           (FIRST_OTHER+11)
#define EDIT_ARROW_RIGHT          (FIRST_OTHER+13)
#define EDIT_ARROW_UP             (ROT_SHIFT+FIRST_OTHER+11)
#define EDIT_ARROW_DOWN           (ROT_SHIFT+FIRST_OTHER+13)

#define EDITOR_MARK_BLACK         (FIRST_OTHER+100)
#define EDITOR_MARK_RED           (FIRST_OTHER+101)
#define EDITOR_MARK_YELLOW        (FIRST_OTHER+102)

#define EDIT_ARROW_DX             20
#define EDIT_ARROW_DY             20

#define ANAK_LOGO                 (FIRST_LOGO)

#define SPRITE_MATOCK             (FIRST_GLOBAL_LEVEL+6)

#define SUB_ITEMS                 8

#define NO_PLAYER                 0xffff
#define NO_ITEM                   0xffff
#define NO_FLOOR                  0xffff
#define NO_ROTATION               (-1)

#define GAME_ITEMS                72

#define COLOR_KEY_BLACK_FULL      RGB(0,0,0)
#define COLOR_KEY_BLACK           RGB(3,3,3)
#define COLOR_KEY_GAME            RGB(165,106,106)
#define COLOR_KEY_MENU            RGB(255,255,255)

#define ANIM_EXIT                 6
#define REV_EXIT                  8

#define ROTATION_MIN              0
#define ROTATION_MAX              3

#define BOOL_UNDEFINED            (-1)
#define ERROR                     (-1)

#define DEFAULT_LEVEL_NAME        "a.lv3"

/* Hlavni herni prvky */
#define P_GROUND                  0
#define P_PLAYER_1                1
#define P_PLAYER_2                2
#define P_PLAYER_3                3
#define P_PLAYER_4                4
#define P_PLAYER_5                5
#define P_BOX                     6
#define P_TNT                     7
#define P_WALL                    8
#define P_EXIT                    9
#define P_STONE                   10
#define P_KEY                     11
#define P_MATTOCK                 12
#define P_KEY1                    13
#define P_KEY2                    14
#define P_KEY3                    15
#define P_KEY4                    16
#define P_KEY5                    17

#define P_DOOR1_H_O               18
#define P_DOOR2_H_O               19
#define P_DOOR3_H_O               20
#define P_DOOR4_H_O               21
#define P_DOOR5_H_O               22
//--- mam
#define P_DOOR1_H_Z               23
#define P_DOOR2_H_Z               24
#define P_DOOR3_H_Z               25
#define P_DOOR4_H_Z               26
#define P_DOOR5_H_Z               27
//--- mam
#define P_DOOR1_V_O               28
#define P_DOOR2_V_O               29
#define P_DOOR3_V_O               30
#define P_DOOR4_V_O               31
#define P_DOOR5_V_O               32
//--- mam
#define P_DOOR1_V_Z               33
#define P_DOOR2_V_Z               34
#define P_DOOR3_V_Z               35
#define P_DOOR4_V_Z               36
#define P_DOOR5_V_Z               37
//--- mam
#define P_ID_DOOR1_H_O            38
#define P_ID_DOOR2_H_O            39
#define P_ID_DOOR3_H_O            40
#define P_ID_DOOR4_H_O            41
#define P_ID_DOOR5_H_O            42
//--- mam
#define P_ID_DOOR1_H_Z            43
#define P_ID_DOOR2_H_Z            44
#define P_ID_DOOR3_H_Z            45
#define P_ID_DOOR4_H_Z            46
#define P_ID_DOOR5_H_Z            47
//--- mam
#define P_ID_DOOR1_V_O            48
#define P_ID_DOOR2_V_O            49
#define P_ID_DOOR3_V_O            50
#define P_ID_DOOR4_V_O            51
#define P_ID_DOOR5_V_O            52
//--- mam
#define P_ID_DOOR1_V_Z            53
#define P_ID_DOOR2_V_Z            54
#define P_ID_DOOR3_V_Z            55
#define P_ID_DOOR4_V_Z            56
#define P_ID_DOOR5_V_Z            57
//--- mam
#define P_DV_H_O                  58
#define P_DV_H_Z                  59
#define P_DV_V_O                  60
#define P_DV_V_Z                  61
//--- mam to zadarmo !
#define P_DV_H                    62
#define P_DV_V                    63

#define PP_LEFT_JAMB_O            64
#define PP_RIGHT_JAMB_O           65
#define PP_TOP_JAMB_O             66
#define PP_BOTTOM_JAMB_O          67

#define PP_LEFT_JAMB_Z            68
#define PP_RIGHT_JAMB_Z           69
#define PP_TOP_JAMB_Z             70
#define PP_BOTTOM_JAMB_Z          71

#define DOOR_VARIATION_CLASSIC    0
#define DOOR_VARIATION_CYBER      1

#define FLOOR_CLASSIC_CORNER      0
#define FLOOR_CLASSIC_CORNER_1    1
#define FLOOR_CLASSIC_UP          3
#define FLOOR_CLASSIC_LEFT        2

#define SPRITE_ID_GROUND          1
#define SPRITE_ID_FLOOR           2
#define SPRITE_ID_GRID            3
#define SPRITE_ID_PLAYER          4

// -------------------------------------------------------
// Animation definition - predefined
// -------------------------------------------------------

#define ANIM_PLAYER_1               0
#define ANIM_PLAYER_2               1
#define ANIM_PLAYER_3               2
#define ANIM_PLAYER_4               3
#define ANIM_PLAYER_5               4

#define ANIM_MOVE_FRAMES            9

#define ANIM_MOVE_UP                5
#define ANUM_MOVE_DOWN              7
#define ANIM_MOVE_LEFT              6
#define ANIM_MOVE_RIGHT             8

#define ANIM_STONE_1                9
#define ANIM_STONE_2                10

#define ANIM_EXIT_1                 11

#define ANIM_BLAST                  14

#define ANIM_PLAYER_1_FAST          15
#define ANIM_PLAYER_2_FAST          16
#define ANIM_PLAYER_3_FAST          17
#define ANIM_PLAYER_4_FAST          18
#define ANIM_PLAYER_5_FAST          19

#define ANIM_MOVE_UP_FAST           20
#define ANIM_MOVE_DOWN_FAST         22
#define ANIM_MOVE_LEFT_FAST         21
#define ANIM_MOVE_RIGHT_FAST        23

#define ANIM_MOVE_FRAMES_FAST       4

#define FAST_ANIMATION_SHIFT        15

// -------------------------------------------------------
// Animation flags
// -------------------------------------------------------

#define  ANIM_USED                  0x001 // animation change sprite position

#define  ANIM_MOVE                  0x002 // animation change sprite position
#define  ANIM_SPRITE                0x004 // animation change sprite

#define  ANIM_INSTANT               0x008 // run this animation imediately after create
#define  ANIM_LOOP                  0x010 // loop animation
#define  ANIM_ADD_INVERSE           0x020 // add inverted sprites to animation

#define  ANIM_REMOVE                0x100 // remove this animation from anim queue
                                          // in next anim process
#define  ANIM_GENERATE              0x200 // it's generated animation                                          

#define  ANIM_TRIGGER_MOVE          0x1000
#define  ANIM_TRIGGER_ERASE         0x2000
#define  ANIM_TRIGGER_INSERT        0x4000

#define  ANIM_TRIGGER               (ANIM_TRIGGER_MOVE|\
                                     ANIM_TRIGGER_ERASE|\
                                     ANIM_TRIGGER_INSERT)

// -------------------------------------------------------
// Animation queue
// -------------------------------------------------------
#define ANIM_NONE                   0
#define ANIM_QUEUE_FIRST            1
#define ANIM_QUEUE_LENGHT           100

// Basic types
typedef int item_handle;

// Handle for stored animation
typedef int anim_template_handle;
typedef int anim_handle;

// -------------------------------------------------------
// Ini file & configuration
// -------------------------------------------------------
#define INI_FILE_NAME      "berusky.ini"

#ifdef LINUX
#define INI_USER_DIRECTORY "~/.berusky"
#define INI_USER_LEVELS    "~/.berusky/User"
#define INI_USER_PROFILES  "~/.berusky/Profiles"
#define INI_FILE_GLOBAL    "/var/games/berusky/"INI_FILE_NAME
#define INI_FILE_USER      "~/.berusky/"INI_FILE_NAME
#define INI_FILE_LOCAL     "./"INI_FILE_NAME
#elif WINDOWS
#define INI_USER_DIRECTORY "./"
#define INI_USER_LEVELS    "./"
#define INI_FILE_GLOBAL    INI_FILE_NAME
#define INI_FILE_USER      INI_FILE_NAME
#define INI_FILE_LOCAL     INI_FILE_NAME
#endif

#define INI_FILE          (config_file())

#define LEVEL_HINTS_FILE  "hints.dat"

#define SET_END_FILE_TEXT "end%d.dat"
#define SET_END_FILE_BACK "end%d.spr"

#define BACKGROUND_NAME   "background%d.spr"

#define GAME_TITLE        _("Berusky")

#define ID_FILE           "file_id.diz"

#endif // __DEFINES_H__
