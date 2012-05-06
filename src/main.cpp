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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "berusky.h"
#include "berusky_gui.h"
#include "main.h"
#include "editor.h"

/* Time loop - grabbed from SDL documentation */
#define  TICK_INTERVAL    (1000 / GAME_FPS)

static Uint32 next_time;

Uint32 time_left(void)
{
  Uint32 now = SDL_GetTicks();
  if(next_time <= now)
    return 0;
  else
    return next_time - now;
}

void banner(void)
{
  printf(_("Berusky v.%s (C) Anakreon 2006, http://www.anakreon.cz/\n"),BERUSKY_VERSION);
  printf(_("This is free software; see the source for copying conditions.\n"));
  printf(_("There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"));
  printf(_("Built %s, %s\n\n"),__DATE__,__TIME__);
}

void help(bool error)
{
  if(error)
    printf(_("Bad command line argument(s)!\n\n"));
  
  printf(_("Using: berusky [-e [level.lv3]] [-u level.lv3]\n\n"));
  printf(_("  -e [level.lv3]       -run level editor\n"));
  printf(_("  -u  level.lv3        -run level\n"));
/* TODO - not implemented yet
  printf(_("  -d  level.dm         -run demo.dm\n"));
*/  
  exit(0);
}

void end(DIR_LIST *p_dir)
{
  char in_buffer[4000];
  if(file_load_text(p_dir->gamedata_get(),ID_FILE, in_buffer, 4000, 0, FALSE)) {
    puts(in_buffer);
  }
}

/*
 * Let begin - run game
 */
void run_game(GAME_MODE gmode, char *p_garg, DIR_LIST *p_dir)
{
  /* Create game objects */
  bool fullscreen = get_fullscreen(INI_FILE);
  int  colors = get_colors(INI_FILE, SCREEN_DEPTH);

  graphics_start(GAME_RESOLUTION_X, GAME_RESOLUTION_Y, colors, fullscreen);

  #define START_LEN 400
  next_time = SDL_GetTicks() + START_LEN;

  /* Load&draw logos */
  graphics_logos_load(p_dir);
  start_logo_draw(gmode);
  start_logo_progress(4);

  /* Load game data */
  graphics_game_load(p_dir);
  start_logo_progress();
  graphics_menu_load(p_dir);
  start_logo_progress();

  /* Load item repositores */
  ITEM_REPOSITORY repo;
  repository_load(&repo, p_dir);
  start_logo_progress();

  /* Load GUI */
  GAME_GUI *p_gui = new GAME_GUI(&repo, p_dir);
  assert(p_gui);

  /* Create main event queue */
  LEVEL_EVENT_QUEUE  main_queue;  

  /* Add events to start game */
  switch(gmode) {
    /* Run menu */
    case MENU:
      main_queue.add(LEVEL_EVENT(GC_MENU_START));
      break;
    
    /* Run user level */
    case USER_LEVEL:
      main_queue.add(LEVEL_EVENT(GC_RUN_LEVEL_LINE, p_garg));
      break;
    
    /* Run demo */
    case USER_DEMO:
      //main_queue.add(LEVEL_EVENT(GC_RUN_LEVEL_LINE, p_garg));
      break;
    
    /* WTF? */
    default:
      assert(0);
      break;
  }

  start_logo_progress();
  if(gmode == MENU)
    SDL_Delay(time_left());
  
  /* Time loop - grabbed from SDL documentation */
  int global_frame = 0;

  /* Run game loop */
  next_time = SDL_GetTicks() + TICK_INTERVAL;
  while(1) {
      global_frame++;
  
      if(p_gui) {
        bool status = p_gui->callback(&main_queue, global_frame);
        if(!status)
          break;
      }
        
      SDL_Delay(time_left());
      next_time += TICK_INTERVAL;
  }  

  /* Clean up */
  graphics_menu_free();
  graphics_game_free();
  graphics_logos_free();

  /* Clean up */  
  delete p_gui;

  /* Graphics stop */  
  graphics_stop();

  /* Game over */
  exit(0);
}

/*
 * Let's begin - run editor
 */
void run_editor(GAME_MODE gmode, char *p_garg, DIR_LIST *p_dir)
{
  /* Create game objects */
  bool fullscreen = get_fullscreen(INI_FILE);
  int  colors = get_colors(INI_FILE, SCREEN_DEPTH);

  graphics_start(EDITOR_RESOLUTION_X, EDITOR_RESOLUTION_Y, colors, fullscreen);

  /* Load&draw logos */
  graphics_logos_load(p_dir);
  start_logo_draw(EDITOR);

  /* Load Graphics */
  graphics_game_load(p_dir);
  graphics_menu_load(p_dir);

  /* Load item repositores */
  ITEM_REPOSITORY repo;
  repository_load(&repo, p_dir);

  /* Load GUI and so on */
  EDITOR_GUI *p_editor_gui = new EDITOR_GUI(&repo, p_dir);

  /* Run editor */  
  p_editor_gui->event_add(p_garg ? LEVEL_EVENT(ED_LEVEL_LOAD,p_garg) : LEVEL_EVENT(ED_LEVEL_NEW,TRUE));    
  p_editor_gui->keyset_set(&editor_keys);
  
  /* Run editor loop */
  while(1) {
    if(!p_editor_gui->event_handler())
      break;
  }

  /* Clean up */
  graphics_menu_free();
  graphics_game_free();
  graphics_logos_free();
  
  /* Clean up */
  delete p_editor_gui;

  /* Graphics stop */  
  graphics_stop();

  /* Game over */
  exit(0);
}

/*
 * Manage a configuration file
 */
const char * config_file(bool configure)
{
  static const char *files[] = { INI_FILE_LOCAL, INI_FILE_USER, INI_FILE_GLOBAL };
  static const char *ini_file = NULL;

  if(configure) {
    int i;
    for(i = 0; i < (int)(sizeof(files)/sizeof(files[0])); i++) {
      bprintfnl(_("Checking config file %s..."),files[i]);
      if(file_exists(NULL,files[i])) {
        bprintf("ok");
        ini_file = files[i];
        bprintf(_("Selected config file %s"),ini_file);
        return(ini_file);
      }
      else {
        bprintf("fails");
      }
    }
    berror(_("Can't find any configuration file!"));
  }
  bprintf(" ");
  return(ini_file);
}

/* It tries to create the user directory (~./berusky)
 * and copy berusky.ini file there
 *
 */
void user_directory_create(void)
{
  // Check ~./berusky
  dir_create(INI_USER_DIRECTORY);
  dir_create(INI_USER_LEVELS);

  // Check ~./berusky/berusky.ini
  bprintfnl(_("Checking %s/%s..."),INI_USER_DIRECTORY,INI_FILE_NAME);
  if(!file_exists(INI_USER_DIRECTORY,INI_FILE_NAME)) {
    bprintfnl(_("missing, try to copy it from %s..."),INI_FILE_GLOBAL);
    bool ret = file_copy(INI_FILE_GLOBAL, NULL, INI_FILE_NAME, INI_USER_DIRECTORY,FALSE);
    if(ret) {
      bprintf(_("ok"));
    } else {
      print_errno(TRUE);
      bprintf(_("failed"));
    }
  } else {
    bprintf(_("ok"));
  }
  bprintf(" ");
}

/*
 * Let's begin
 */
int main(int argc, char *argv[])
{
  GAME_MODE gmode = MENU;
  char     *p_garg = NULL;

  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
  srand(clock());
  
  banner();
  
  /* Run help */
  if((argc > 3) || (argc > 1 && (argv[1][0] != '-' || argv[1][1] == '?' || argv[1][1] == 'h')))
  {
    help(argc > 3);
  } 
  /* Run editor */
  else if(argc > 1 && (argv[1][0] != '-' || argv[1][1] == 'e'))
  {
    gmode = EDITOR;
    p_garg = argc > 2 ? argv[2] : NULL;
  }
  /* Run user level from command line */
  else if(argc > 2 && (argv[1][0] != '-' || argv[1][1] == 'u'))
  {
    gmode = USER_LEVEL;
    p_garg = argv[2];
  }
  /* Run demo */
  else if(argc > 2 && (argv[1][0] != '-' || argv[1][1] == 'd'))
  {
    gmode = USER_DEMO;
    p_garg = argv[2];
  }
  /* Run normal game from menu */
  else if(argc == 1)
  {
    gmode = MENU;
  } 
  else {
    help(TRUE);
  }

  user_directory_create();
  config_file(TRUE);
  log_open_ini(INI_FILE);

  DIR_LIST dir;
  dir.load(INI_FILE);

  if(gmode == EDITOR) {
    run_editor(gmode, p_garg, &dir);
  } else {
    run_game(gmode, p_garg, &dir);
  }  
  return 0;
}

void start_logo_draw(GAME_MODE mode)
{
  switch(mode) {  
    case USER_DEMO:
    case USER_LEVEL:
    case MENU:
      {
        tpos sx, sy;
        sx = p_grf->sprite_get_width_center(ANAK_LOGO);
        sy = p_grf->sprite_get_height_center(ANAK_LOGO);
        p_grf->draw(ANAK_LOGO,sx,sy);
        p_grf->redraw_add(ANAK_LOGO,sx,sy);
        p_grf->flip();
        break;
      }
    case EDITOR:
      {
        tpos sx, sy;
        sx = p_grf->sprite_get_width_center(ANAK_LOGO);
        sy = p_grf->sprite_get_height_center(ANAK_LOGO);
        p_grf->draw(ANAK_LOGO,sx,sy);
        p_grf->redraw_add(ANAK_LOGO,sx,sy);
        p_grf->flip();
        break;
      }
    default:
      break;
  }
}

void start_logo_progress(int steps)
{
  static int steps_max;
  static int steps_act;

  #define PROGRESS_HEIGHT 20

  if(steps == PROGRESS_MOVE) {
    steps_act++;
    if(steps_act > steps_max)
      steps_act = steps_max;

    tpos dx = p_grf->sprite_get_width(),
         dy = p_grf->sprite_get_height();
    tpos ax = (dx/steps_max)*steps_act;

    p_grf->fill(0,dy-PROGRESS_HEIGHT,ax,PROGRESS_HEIGHT,COLOR_MAP(212,214,211));
    p_grf->redraw_add(0,dy-PROGRESS_HEIGHT,ax,PROGRESS_HEIGHT);

    p_grf->flip();
  } else {
    steps_max = steps;
    steps_act = 0;
  }
}

bool graphics_logos_load(DIR_LIST *p_dir)
{
  int i = 0;

  p_grf->graphics_dir_set(p_dir->graphics_get());

  sprite::color_key_set(COLOR_KEY_GAME);
  i  += p_grf->sprite_insert("logo.spr", FIRST_LOGO, 1, NULL);

  return(i);
}

void graphics_logos_free(void)
{
  p_grf->sprite_delete(FIRST_LOGO, 1);
}

bool graphics_game_load(DIR_LIST *p_dir)
{
  int i;

  p_grf->graphics_dir_set(p_dir->graphics_get());

  bprintf(_("Graphics dir '%s'"),p_dir->graphics_get());
  bprintf(_("Loading game graphics..."));

  sprite::color_key_set(COLOR_KEY_GAME);

  i  = p_grf->sprite_insert("global1.spr", FIRST_GLOBAL_LEVEL, GLOBAL_SPRITES, NULL);
  i += p_grf->sprite_insert("global2.spr", FIRST_GLOBAL_LEVEL + ROT_SHIFT, GLOBAL_SPRITES,NULL);
  i += p_grf->sprite_insert("global3.spr", FIRST_GLOBAL_LEVEL + 2 * ROT_SHIFT, GLOBAL_SPRITES,NULL);
  i += p_grf->sprite_insert("global4.spr", FIRST_GLOBAL_LEVEL + 3 * ROT_SHIFT, GLOBAL_SPRITES,NULL);

  i += p_grf->sprite_insert("klasik1.spr", FIRST_CLASSIC_LEVEL, CLASSIC_SPRITES,NULL);
  i += p_grf->sprite_insert("klasik2.spr", FIRST_CLASSIC_LEVEL + ROT_SHIFT, CLASSIC_SPRITES,NULL);
  i += p_grf->sprite_insert("klasik3.spr", FIRST_CLASSIC_LEVEL + 2 * ROT_SHIFT, CLASSIC_SPRITES,NULL);
  i += p_grf->sprite_insert("klasik4.spr", FIRST_CLASSIC_LEVEL + 3 * ROT_SHIFT, CLASSIC_SPRITES,NULL);

  i += p_grf->sprite_insert("kyber1.spr", FIRST_CYBER_LEVEL, CYBER_SPRITES,NULL);
  i += p_grf->sprite_insert("kyber2.spr", FIRST_CYBER_LEVEL + ROT_SHIFT,CYBER_SPRITES,NULL);
  i += p_grf->sprite_insert("kyber3.spr", FIRST_CYBER_LEVEL + 2 * ROT_SHIFT, CYBER_SPRITES,NULL);
  i += p_grf->sprite_insert("kyber4.spr", FIRST_CYBER_LEVEL + 3 * ROT_SHIFT, CYBER_SPRITES,NULL);
  
  i += p_grf->sprite_insert("herni1.spr",  FIRST_OTHER, GAME_SPRITES,NULL);
  i += p_grf->sprite_insert("herni2.spr",  FIRST_OTHER + ROT_SHIFT, GAME_SPRITES,NULL);
  
  i += p_grf->sprite_insert("hraci1.spr", FIRST_PLAYER, PLAYER_SPRITES,NULL);
  i += p_grf->sprite_insert("hraci2.spr", FIRST_PLAYER + ROT_SHIFT, PLAYER_SPRITES,NULL);
  i += p_grf->sprite_insert("hraci3.spr", FIRST_PLAYER + 2 * ROT_SHIFT, PLAYER_SPRITES,NULL);
  i += p_grf->sprite_insert("hraci4.spr", FIRST_PLAYER + 3 * ROT_SHIFT, PLAYER_SPRITES,NULL);

  if(!i) {
    berror(_("Unable to load data, exiting..."));    
  }
  bprintf(_("%d sprites loaded..."), i);

  return(i > 0);
}

void graphics_game_free(void)
{
  p_grf->sprite_delete(FIRST_GLOBAL_LEVEL, GLOBAL_SPRITES);
  p_grf->sprite_delete(FIRST_GLOBAL_LEVEL + ROT_SHIFT, GLOBAL_SPRITES);
  p_grf->sprite_delete(FIRST_GLOBAL_LEVEL + 2 * ROT_SHIFT, GLOBAL_SPRITES);
  p_grf->sprite_delete(FIRST_GLOBAL_LEVEL + 3 * ROT_SHIFT, GLOBAL_SPRITES);

  p_grf->sprite_delete(FIRST_CLASSIC_LEVEL, CLASSIC_SPRITES);
  p_grf->sprite_delete(FIRST_CLASSIC_LEVEL + ROT_SHIFT, CLASSIC_SPRITES);
  p_grf->sprite_delete(FIRST_CLASSIC_LEVEL + 2 * ROT_SHIFT, CLASSIC_SPRITES);
  p_grf->sprite_delete(FIRST_CLASSIC_LEVEL + 3 * ROT_SHIFT, CLASSIC_SPRITES);

  p_grf->sprite_delete(FIRST_CYBER_LEVEL, CYBER_SPRITES);
  p_grf->sprite_delete(FIRST_CYBER_LEVEL + ROT_SHIFT,CYBER_SPRITES);
  p_grf->sprite_delete(FIRST_CYBER_LEVEL + 2 * ROT_SHIFT, CYBER_SPRITES);
  p_grf->sprite_delete(FIRST_CYBER_LEVEL + 3 * ROT_SHIFT, CYBER_SPRITES);
  
  p_grf->sprite_delete(FIRST_OTHER, GAME_SPRITES);
  
  p_grf->sprite_delete(FIRST_PLAYER, PLAYER_SPRITES);
  p_grf->sprite_delete(FIRST_PLAYER + ROT_SHIFT, PLAYER_SPRITES);
  p_grf->sprite_delete(FIRST_PLAYER + 2 * ROT_SHIFT, PLAYER_SPRITES);
  p_grf->sprite_delete(FIRST_PLAYER + 3 * ROT_SHIFT, PLAYER_SPRITES);  
}

bool graphics_menu_load(DIR_LIST *p_dir)
{
  int i = 0;
  
  p_grf->graphics_dir_set(p_dir->graphics_get());

  bprintf(_("Graphics dir '%s'"),p_dir->graphics_get());
  bprintf(_("Loading menu graphics..."));

  sprite::color_key_set(COLOR_KEY_GAME);
  i   = p_grf->sprite_insert("menu1.spr", MENU_SPRIT_ROCK, 1, NULL);
  i  += p_grf->sprite_insert("menu2.spr", MENU_SPRIT_LOGO, 1, NULL);
  i  += p_grf->sprite_insert("menu3.spr", MENU_SPRIT_BACK, 1, NULL);

  sprite::color_key_set(COLOR_KEY_MENU);
  i  += p_grf->sprite_insert("menu4.spr", MENU_SPRIT_ARROWS,
                                          MENU_ARROWS_NUM, NULL);
  i  += p_grf->sprite_insert("controls.spr", MENU_CHECKBOX_CHECKED,
                                             MENU_CHECKBOX_NUM, NULL);
  i  += p_grf->sprite_insert("slidebar.spr", MENU_SLIDEBAR, 1, NULL);
  i  += p_grf->sprite_insert("slider.spr", MENU_SLIDER, 1, NULL);

  sprite::color_key_set(COLOR_KEY_GAME);
  i  += p_grf->sprite_insert("menu5.spr", MENU_SPRIT_LOGO_SMALL_1, MENU_SPRIT_LOGO_SMALL_NUM, NULL);

  i  += p_grf->sprite_insert("back1.spr", MENU_SPRIT_START,   1, NULL);
  i  += p_grf->sprite_insert("back2.spr", MENU_SPRIT_START+1, 1, NULL);
  i  += p_grf->sprite_insert("back3.spr", MENU_SPRIT_START+2, 1, NULL);
  i  += p_grf->sprite_insert("back4.spr", MENU_SPRIT_START+3, 1, NULL);

  i  += p_grf->sprite_insert("mask1.spr",  EDITOR_MARK_BLACK, 1, NULL);
  i  += p_grf->sprite_insert("mask2.spr",  EDITOR_MARK_RED, 1, NULL);
  i  += p_grf->sprite_insert("mask3.spr",  EDITOR_MARK_YELLOW, 1, NULL);  
  
  sprite::color_key_set(COLOR_KEY_MENU);
  
  int j;
  for(j = 0; j < FONT_NUM; j++) {
    if(!p_font->load(j, FIRST_FONT + j*FONT_STEP, FONT_SPRITES))
      bprintf(_("Unable to load font %d!"),j);
  }

  if(!i) {
    berror(_("Unable to load data, exiting..."));    
  }
  bprintf(_("%d sprites loaded..."), i);

  return((bool)i);
}

void graphics_menu_free(void)
{
  p_grf->sprite_delete(MENU_SPRIT_ROCK);
  p_grf->sprite_delete(MENU_SPRIT_LOGO);
  p_grf->sprite_delete(MENU_SPRIT_BACK);

  int i;
  for(i = 0; i < FONT_NUM; i++) {
    p_font->free(i);
  }
}

bool repository_load(ITEM_REPOSITORY *p_repo, DIR_LIST *p_dir)
{
  bprintf(_("Data dir '%s'"),p_dir->gamedata_get());
  bprintf(_("Loading items repository...."));

  bool loaded = p_repo->load("items.dat", "items_animation.dat", p_dir->gamedata_get());
  if(!loaded) {
    berror(_("Failed, exiting..."));    
  }

  return(TRUE);
}

int  background_num(DIR_LIST *p_dir)
{
  bprintf(_("Graphics dir '%s'"),p_dir->graphics_get());
  bprintf(_("Checking backgrounds..."));

  int j;

  for(j = 0; j < 100; j++) {
    char file[MAX_FILENAME];
    sprintf(file, BACKGROUND_NAME, j+1);
    if(!file_exists(p_dir->graphics_get(),file))
      break;
  }  

  bprintf(_("%d backgrounds..."), j);

  return(j);
}
