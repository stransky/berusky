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

#include <errno.h>

#include "portability.h"

#ifdef LINUX
#include <sys/wait.h>
#endif

#ifdef WINDOWS
#include <stdio.h>
#include <process.h>
#endif

#include "berusky.h"
#include "berusky_gui.h"
#include "main.h"


spr_handle menu_background_get(void)
{  
  spr_handle handles[] = {MENU_SPRIT_BACK2, MENU_SPRIT_BACK3};
  int num = sizeof(handles)/sizeof(handles[0]);
  static int init = TRUE;

  if(init) {
    srand(SDL_GetTicks());
    init = FALSE;
  }

  return(handles[rand()/(RAND_MAX/num)]);
}

// -------------------------------------------------------
// Game UI
// -------------------------------------------------------

game_gui::game_gui(ITEM_REPOSITORY *p_repo_, DIR_LIST *p_dir_)
: gui_base(), p_repo(p_repo_), p_dir(p_dir_)
{
  // Create game main class
  p_ber = new BERUSKY(p_repo_,p_dir);

  // Set window title
  window_set_title(GAME_TITLE);

  // Load last used profile
  player_profile_load();
}

game_gui::~game_gui(void)
{
  player_profile_save();

  if(p_ber)
    delete p_ber;
   
  // TODO -> dat to tam kde se to SDL inicializuje
  SDL_Quit();
}

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF

/* Double size start question after start
*/
void game_gui::menu_double_size_question(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_settings, data, data1);
    
        p_grf->fill(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y, 0);
              
        p_font->select(FONT_DEFAULT);
        p_font->alignment_set(MENU_CENTER);
        p_font->start_set(0, 150);
        p_font->print(NULL, _("Start in high resolution mode?"));
      
        #define MENU_X_START_BACK (GAME_RESOLUTION_X/2)
        #define MENU_Y_START_BACK (200)
        #define MENU_X_DIFF       0
        #define MENU_Y_DIFF       35
        
        static char *yes = _("yes");
        static char *no = _("no");
                
        menu_item_set_pos(MENU_X_START_BACK, MENU_Y_START_BACK);
        menu_item_set_diff(MENU_X_DIFF, MENU_Y_DIFF);
        menu_item_start();
      
        menu_item_draw(yes, MENU_LEFT, FALSE,
                       LEVEL_EVENT(GC_MENU_DOUBLESIZE_SET, TRUE),
                       LEVEL_EVENT(GC_GAME_DATA_LOAD),
                       LEVEL_EVENT(GC_MENU_START));
        menu_item_draw(no, MENU_RIGHT, FALSE, 
                       LEVEL_EVENT(GC_MENU_DOUBLESIZE_SET, FALSE),
                       LEVEL_EVENT(GC_GAME_DATA_LOAD),
                       LEVEL_EVENT(GC_MENU_START));                       

        #define MENU_X_START (GAME_RESOLUTION_X/2 - 120)
        #define MENU_Y_START (400)
      
        static char *dont_ask = _("Don't ask again");
      
        menu_item_set_pos(MENU_X_START, MENU_Y_START);
        menu_item_draw_checkbox(dont_ask, MENU_LEFT, !berusky_config::double_size_question, 0, 0,
                                LEVEL_EVENT(GC_MENU_DOUBLESIZE_SWITCH));

        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();              
      }
      break;
    
    case MENU_LEAVE:
      input.mevent_clear();
      break;
    default:
      break;
  }
}

void game_gui::menu_double_size_question_switch(void)
{
  berusky_config::double_size_question = !berusky_config::double_size_question;
  set_doublesize_question(INI_FILE, berusky_config::double_size_question);
}

void game_gui::menu_double_size_set(bool double_size)
{
  if(berusky_config::double_size != double_size) {
    berusky_config::double_size = double_size;
    berusky_config::game_screen_set();
  }
}

#define LAST_PLAYER_PROFILE "last_profile"

void game_gui::player_profile_load(void)
{
  char last_profile[MAX_FILENAME];
  ini_read_string_file(INI_FILE, LAST_PLAYER_PROFILE,
                       last_profile, sizeof(last_profile), "none");

  // Load last used profile
  BERUSKY_PROFILE prf;
  if(profile_load_last(last_profile, prf)) {
    profile = prf;
  }
}

void game_gui::player_profile_save(void)
{
  profile.save();
  ini_write_string(INI_FILE, LAST_PLAYER_PROFILE, profile.profile_name);
}

void game_gui::level_set_select(int level_set)
{
  bool ret = p_ber->levelset_load(level_set);
  if(!ret) {
    berror("Unable to load levelset %d\n",level_set);
    return;
  }
  profile.level_set_set(level_set);
}

void game_gui::level_select(int level, tpos spr_x, tpos spr_y)
{  
  // Selec the level
  p_ber->levelset_set_level(level);
  profile.selected_level_set(level);

  // Draw the selected level
  p_grf->draw(LEVEL_DONE, profile.level_spr_x, profile.level_spr_y);
  p_grf->redraw_add(profile.level_spr_x, profile.level_spr_y, 
                    p_grf->sprite_get_width(LEVEL_DONE),
                    p_grf->sprite_get_height(LEVEL_DONE));
  p_grf->draw(LEVEL_NEXT, spr_x, spr_y);
  p_grf->redraw_add(spr_x, spr_y,
                    p_grf->sprite_get_width(LEVEL_NEXT),
                    p_grf->sprite_get_height(LEVEL_NEXT));
  profile.level_spr_x = spr_x;
  profile.level_spr_y = spr_y;
  p_grf->flip();
}

/*
  MENU_ENTER = 0,
  MENU_LEAVE = 1
*/
#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF

/*
  - zrusit 
*/
void game_gui::menu_main(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)(&game_gui::menu_main), data, data1);

        p_grf->fill(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y, 0);

        tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);
        tpos height = p_grf->sprite_get_height(MENU_SPRIT_LOGO);
      
        #define LOGO_START (DOUBLE_SIZE ? 60 : 0)
      
        if(DOUBLE_SIZE) {
          p_grf->draw(menu_background_get(),0,0);
        }
      
        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2, LOGO_START);

        static char *new_game = _("play");
        static char *profiles = _("change profile");
        static char *settings = _("settings");
        static char *help = _("help");
        static char *editor = _("editor");
        static char *quit = _("quit");

        p_font->select(FONT_DEFAULT);

        menu_item_set_pos(GAME_RESOLUTION_X/2 - 70, GAME_RESOLUTION_Y/2 - 50);

        #define MENU_X_DIFF  0
        #define MENU_Y_DIFF  (DOUBLE_SIZE ? 45 : 35)
        menu_item_set_diff(MENU_X_DIFF, MENU_Y_DIFF);

        menu_item_start();

        menu_item_draw(new_game, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_NEW_GAME));
        menu_item_draw(profiles, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_PROFILES));
        menu_item_draw(settings, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_SETTINGS));
        menu_item_draw(help, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_HELP,FALSE));
        menu_item_draw(editor, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_RUN_EDITOR));      
        menu_item_draw(quit, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_QUIT));
      
        p_font->alignment_set(MENU_CENTER);
        p_font->start_set(0, GAME_RESOLUTION_Y - 60);
        p_font->print(NULL,_("berusky version %s (C) Anakreon 1997-2012\n"), VERSION);
        p_font->print(_("distributed under GPLv2\n"));
        
        #define PROFILE_Y_DIFF  (DOUBLE_SIZE ? 70 : -10)
        p_font->alignment_set(MENU_CENTER);
        p_font->start_set(0, LOGO_START+height+PROFILE_Y_DIFF);
        p_font->print(NULL, _("Selected profile: %s"), profile.profile_name);
        
        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();
      }
      break;
    case MENU_LEAVE:
      input.mevent_clear();
      break;
    default:
      break;
  }
}

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF

void game_gui::menu_new_game(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_new_game, data, data1);
    
        p_grf->fill(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y, 0);
        
        tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);

        #define LOGO_START (DOUBLE_SIZE ? 60 : 0)
      
        if(DOUBLE_SIZE) {
          p_grf->draw(menu_background_get(),0,0);
        }
      
        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);
      
        #define MENU_X_START (GAME_RESOLUTION_X/2 - 70)
        #define MENU_Y_START (GAME_RESOLUTION_Y/2 - 30)

        p_font->select(FONT_DEFAULT);
        p_font->alignment_set(MENU_CENTER);
        p_font->start_set(0, MENU_Y_START - 50);
        p_font->print(_("Choose your level map:"));
      
        static char *training      = _("training");
        static char *easy          = _("easy");
        static char *intermediate  = _("intermediate");
        static char *advanced      = _("advanced");
        static char *impossible    = _("impossible");
        static char *user_levels   = _("user levels");
        static char *back          = _("back");
      
        menu_item_set_pos(MENU_X_START, MENU_Y_START);
        
        #define MENU_X_DIFF  0
        #define MENU_Y_DIFF  (DOUBLE_SIZE ? 45 : 35)
        menu_item_set_diff(MENU_X_DIFF, MENU_Y_DIFF);
        
        menu_item_start();
        menu_item_draw(training, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_RUN_LEVEL, 0));
        menu_item_draw(easy, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_RUN_LEVEL, 1));
        menu_item_draw(intermediate, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_RUN_LEVEL, 2));
        menu_item_draw(advanced, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_RUN_LEVEL, 3));
        menu_item_draw(impossible, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_RUN_LEVEL, 4));
        menu_item_draw(user_levels, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_RUN_LEVEL, 5));
        menu_item_draw(back, MENU_LEFT, FALSE, LEVEL_EVENT(GI_MENU_BACK_POP));
              
        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();              
      }
      break;    
    case MENU_LEAVE:    
      input.mevent_clear();
      break;
    default:
      break;
  }
}

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF
#undef PROFILE_Y_DIFF

void game_gui::menu_profiles(MENU_STATE state, size_ptr data, size_ptr data1)
{
  #define MAX_PROFILE_NAME MAX_FILENAME
  static char profile_name[MAX_PROFILE_NAME] = "";
  static BERUSKY_PROFILE *p_profiles = NULL;
  static int profile_num = 0;

  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_profiles, data, data1);

        p_grf->fill(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y, 0);
      
        #define LOGO_START (DOUBLE_SIZE ? 60 : 0)
      
        if(DOUBLE_SIZE) {
          tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);
          p_grf->draw(menu_background_get(),0,0);
          p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);
        }
      
        p_font->select(FONT_DEFAULT);
        p_font->alignment_set(MENU_CENTER);
        #define PROFILE_NAME_START (DOUBLE_SIZE ? 250 : 0)
        p_font->start_set(0, PROFILE_NAME_START);
        p_font->print(NULL, _("Current profile is: %s"), profile.profile_name);

        // Create a new profile
        #define INSERT_START          (DOUBLE_SIZE ? 300 : 30)
        #define INSERT_START_CONSOLE  (INSERT_START + (DOUBLE_SIZE ? 50 : 30))
        p_font->start_set(0, INSERT_START);
        p_font->print(_("Create a new player profile:\n"));
        p_font->print(NULL, 0, INSERT_START_CONSOLE, "_");
        profile_name[0] = '\0';

        #define MENU_X_START (GAME_RESOLUTION_X/2 - 50)
        #define MENU_Y_START (DOUBLE_SIZE ? (INSERT_START+100) : (INSERT_START+70))
        #define MENU_Y_DIFF  35

        static char *create = _("create");

        menu_item_start();
        menu_item_draw(MENU_X_START, MENU_Y_START, create, MENU_LEFT, FALSE, 
                       LEVEL_EVENT(GC_MENU_PROFILE_CREATE, profile_name), 
                       LEVEL_EVENT(GI_MENU_BACK_POP));

        #define PROFILE_LIST_START    (DOUBLE_SIZE ? 500 : 150)
        #define PROFILE_MENU_Y_START  (DOUBLE_SIZE ? (PROFILE_LIST_START+50) : (PROFILE_LIST_START+35))
        #define PROFILE_MAX           (DOUBLE_SIZE ? 15 : 10)
        #define PROFILE_Y_DIFF        (DOUBLE_SIZE ? 25 : 25)

        p_font->alignment_set(MENU_CENTER);
        p_font->start_set(0, PROFILE_LIST_START);
        p_font->print(_("Choose saved profile:\n"));

        // Load profiles from disc
        if(p_profiles) {
          delete [] p_profiles;
          p_profiles = NULL;
          profile_num = 0;
        }
        profiles_load(&p_profiles, &profile_num);

        if(profile_num > PROFILE_MAX)
          profile_num = PROFILE_MAX;

        for(int i = 0; i < profile_num; i++) {
          menu_item_draw_text(MENU_X_START, PROFILE_MENU_Y_START+PROFILE_Y_DIFF*i, p_profiles[i].profile_name,
                              MENU_CENTER, FALSE,
                              LEVEL_EVENT(GC_MENU_PROFILE_SELECT, INT_TO_POINTER(i), p_profiles),
                              LEVEL_EVENT(GI_MENU_BACK_POP));
        }

        #define MENU_BACK_Y_START (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 90 : 40))
        static char *back = _("back");
        menu_item_draw(MENU_X_START, MENU_BACK_Y_START, back, MENU_LEFT, FALSE, LEVEL_EVENT(GI_MENU_BACK_POP));
      
        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();

        menu_key_input.set((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_profiles);
      }
      break;
    case MENU_KEY_INPUT:
      {
        char ascii_char;
        bool character = key_to_ascii(data,&ascii_char);
        int  len = strlen(profile_name);
      
        if(character && ascii_char >= ' ') {
          if(len < MAX_PASSWORD-2) {
            profile_name[len] = ascii_char;
            profile_name[len+1] = '\0';
          }        
        } else {
          switch(data) {
            case K_BKSP:
            case K_DEL:
              if(len > 0) {
                profile_name[--len] = '\0';
              }
              break;
            case K_ENTER:
              break;
            default:
              break;
          }
        }

        p_font->select(FONT_DEFAULT);
        p_font->alignment_set(MENU_CENTER);        
        p_grf->fill(0,INSERT_START_CONSOLE,GAME_RESOLUTION_X,20,0);
        p_font->print(NULL, 0, INSERT_START_CONSOLE, "%s_", profile_name);
        p_grf->redraw_add(0,INSERT_START_CONSOLE,GAME_RESOLUTION_X,20);
        p_grf->flip();
      }
      break;
    case MENU_LEAVE:    
      input.mevent_clear();
      menu_key_input.clear();
      if(p_profiles) {
        delete [] p_profiles;
        p_profiles = NULL;
        profile_num = 0;
      }
      break;
    default:
      break;
  }
}

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF
#undef INSERT_START
#undef PROFILE_Y_DIFF

void game_gui::menu_profile_create(size_ptr data)
{
  // Create a new profile
  profile.create((char *)data);
}

void game_gui::menu_profile_select(size_ptr data, size_ptr data1)
{
  // Set the current profile
  profile = ((BERUSKY_PROFILE *)data1)[data];
}

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF

/*
  data = bool from_game
*/
void game_gui::menu_help(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_help, data, data1);
    
        p_grf->fill(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y, 0);
        
        tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);

        #define LOGO_START (DOUBLE_SIZE ? 60 : 0)
      
        if(DOUBLE_SIZE) {
          p_grf->draw(menu_background_get(),0,0);
        }
        
        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);

        #define MENU_X_START (GAME_RESOLUTION_X/2 - 80)
        #define MENU_Y_START (GAME_RESOLUTION_Y/2)
        #define MENU_X_DIFF  0
        #define MENU_Y_DIFF  (DOUBLE_SIZE ? 45 : 35)

        bool from_game = (bool)data;

        static char *hint    = _("level hint");
        static char *keys    = _("game controls");
        static char *rules   = _("game rulez");
        static char *credits = _("authors");
        static char *back    = _("back");

        menu_item_set_pos(MENU_X_START, MENU_Y_START);
        menu_item_set_diff(MENU_X_DIFF, MENU_Y_DIFF);

        menu_item_start();

        if(from_game) {
          menu_item_draw(hint, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
        }

        menu_item_draw(keys, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_HELP_KEYS));
        menu_item_draw(rules, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_HELP_RULES,0));
        menu_item_draw(credits, MENU_LEFT, MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_HELP_CREDIT));
        menu_item_draw(back, MENU_LEFT, FALSE, LEVEL_EVENT(from_game ? GC_RESTORE_LEVEL : GI_MENU_BACK_POP));

        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();              
      }
      break;
    
    case MENU_LEAVE:
      input.mevent_clear();
      break;
    default:
      break;
  }
}

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF
#undef MENU_X_START_BACK
#undef MENU_Y_START_BACK

/*
  data = bool from_game
*/
void game_gui::menu_settings(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_settings, data, data1);
    
        p_grf->fill(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y, 0);
        
        tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);

        #define LOGO_START (DOUBLE_SIZE ? 60 : 0)
      
        if(DOUBLE_SIZE) {
          p_grf->draw(menu_background_get(),0,0);
        }
        
        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);
      
        #define MENU_X_START (GAME_RESOLUTION_X/2 - 220)
        #define MENU_Y_START (GAME_RESOLUTION_Y/2 - 60)

        #define MENU_X_DIFF  0
        #define MENU_Y_DIFF  35
      
        bool from_game = (bool)data;
      
        static char *fulscreen = _("fulscreen");
        static char *double_size = _("High resolution mode");
        static char *double_size_question = _("Ask on start up");
/*      
        static char *sound = _("sound");
        static char *music = _("music");
*/
        menu_item_set_pos(MENU_X_START, MENU_Y_START);
        menu_item_set_diff(MENU_X_DIFF, MENU_Y_DIFF);

        menu_item_start();
        menu_item_draw_checkbox(fulscreen, MENU_LEFT, p_grf->fullscreen_get(), 0, 0,
                                LEVEL_EVENT(GC_MENU_SETTINGS_FULSCREEN_SWITCH));
        menu_item_draw_checkbox(double_size, MENU_LEFT, berusky_config::double_size, 0, 0,
                                LEVEL_EVENT(GC_MENU_SETTINGS_DOUBLESIZE_SWITCH));
                                
        menu_item_set_pos(MENU_X_START+35, MENU_Y_START+2*MENU_Y_DIFF);
        menu_item_draw_checkbox(double_size_question, MENU_LEFT,
                                berusky_config::double_size_question, 0, 0,
                                LEVEL_EVENT(GC_MENU_DOUBLESIZE_SWITCH));
/*
        menu_item_draw_checkbox(sound, MENU_LEFT, p_ber->sound.sound_on, 1,
                                LEVEL_EVENT(GC_MENU_SETTINGS_SOUND_SWITCH));
        menu_item_draw_checkbox(music, MENU_LEFT, p_ber->sound.music_on, 2,
                                LEVEL_EVENT(GC_MENU_SETTINGS_MUSIC_SWITCH));
*/
        static char *back = _("back");
                
        #define MENU_X_START_BACK (GAME_RESOLUTION_X/2 - 50)
        #define MENU_Y_START_BACK (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 120 : 80))
        
        menu_item_set_pos(MENU_X_START_BACK, MENU_Y_START_BACK);
        menu_item_draw(back, MENU_LEFT, FALSE, LEVEL_EVENT(from_game ? GC_RESTORE_LEVEL : GI_MENU_BACK_POP));

        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();              
      }
      break;
    
    case MENU_LEAVE:
      input.mevent_clear();
      break;
    default:
      break;
  }
}

void game_gui::menu_settings_fullscreen(void)
{
  p_grf->fullscreen_toggle();
  set_fullscreen(INI_FILE, p_grf->fullscreen_get());
}

void game_gui::menu_settings_doublesize(void)
{
  berusky_config::double_size = !berusky_config::double_size;
  set_doublesize(INI_FILE, berusky_config::double_size);

  p_font->select(FONT_DEFAULT);
  p_font->alignment_set(MENU_CENTER);
  p_font->start_set(0, GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 200 : 150));
  RECT r;
  p_font->print(&r,_("The game must be restarted\nfor the change to take effect."));
  p_grf->redraw_add(&r);
  p_grf->flip();
}

/*
*/
#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF
#undef X_START
#undef Y_START

void game_gui::menu_help_rules(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {      
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_help_rules, data, data1);
      
        p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
      
        if(DOUBLE_SIZE) {
          #define LOGO_START 60
          tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);

          p_grf->draw(menu_background_get(),0,0);
          p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);
        }
        else {
          p_grf->draw(MENU_SPRIT_BACK,0,0);
        }      
      
        p_font->alignment_set(MENU_LEFT);
        p_font->select(FONT_DEFAULT);
      
        #define X_START ((DOUBLE_SIZE) ? 320 : 0)
        #define Y_START ((DOUBLE_SIZE) ? 150 : 0)

        int j, i, x, y;
        int variants;
        int page = data;

        int page_prev = page-1,
            page_next = page+1;
          
        switch(page) {
          case 0:  
            x = 20+X_START;
            y = 95+Y_START;
          
            p_font->alignment_set(MENU_CENTER);
            p_font->print(NULL, x+60, y, _("Basic rules and game elements"));
            
            p_font->alignment_set(MENU_LEFT);
            y += 45; // 140
            p_font->start_set(x, y);
            p_font->print(_("In order to leave each level it is\n\
necessary to own five keys and also\n\
to have a free way to the exit.\n\n\
You will be meeting miscellaneous game\n\
elements while completing individual\n\
missions, we will try to explain their\n\
meaning now.\n"));

            y += 170; // 310
            p_font->print(NULL, x, y, _("box - it is possible to push it."));
            y += 30; // 340
            if(DOUBLE_SIZE) y += 10;
            variants = p_repo->variants_get(P_BOX);
            if(variants > 13) {
              variants = (DOUBLE_SIZE) ? 13 : 8;
            }
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_BOX, i, 0);
              x += DOUBLE_SIZE ? 50 : 30;
            }

            if(DOUBLE_SIZE) {
              x = 20+X_START;
              y += 30;
              if(DOUBLE_SIZE) y += 40;
              p_font->print(NULL, x, y, _("Light box - bugs can push two of them."));
              y += 30;
              if(DOUBLE_SIZE) y += 10;
              variants = p_repo->variants_get(P_BOX_LIGHT);
              if(variants > 13) {
                variants = (DOUBLE_SIZE) ? 13 : 8;
              }
              for (i = 0; i < variants; i++) {
                p_repo->draw(x, y, P_BOX_LIGHT, i, 0);
                x += DOUBLE_SIZE ? 50 : 30;
              }
            }
          
            x = 20+X_START;
            y += 30;
            if(DOUBLE_SIZE) y += 40;
            p_font->print(NULL, x, y, _("explosive - can destroy the boxes."));
            y += 30;
            if(DOUBLE_SIZE) y += 10;
            variants = p_repo->variants_get(P_TNT);
            if(variants > 13) {
              variants = (DOUBLE_SIZE) ? 13 : 5;
            }
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_TNT, i, 0);
              x += DOUBLE_SIZE ? 50 : 30;
            }
          
            page_prev = page;
          
            break;
          
          case 1:
            x = 20+X_START;
            y = 95+Y_START;
            p_font->alignment_set(MENU_CENTER);
            p_font->print(NULL, 0, y, _("Active game elements"));

            // A line for key
            p_font->alignment_set(MENU_LEFT);
            y += 40;
            if(DOUBLE_SIZE) y += 40;
            p_font->print(NULL, x, y, _("key - you need five of them."));
            y += 30;
            if(DOUBLE_SIZE) y += 10;
            p_grf->draw(p_repo->sprite_get(P_KEY, 0, 0), x, y);

            // A line for exits
            y += 40;
            if(DOUBLE_SIZE) y += 40;
            p_font->print(NULL, x, y, _("exit - a gate to next level."));            
            y += 30;
            if(DOUBLE_SIZE) y += 10;
            variants = p_repo->variants_get(P_EXIT);
            for (i = 0; i < variants - 1; i += 2) {
              p_repo->draw(x, y, P_EXIT, i, 0);
              x += DOUBLE_SIZE ? 50 : 30;
            }
            p_grf->draw(p_repo->sprite_get(P_EXIT, i - 1, 0), x, y);
          
            // A line for stones
            x = 20+X_START;
            y += 40;
            if(DOUBLE_SIZE) y += 40;
            p_font->print(NULL, x, y, _("stone - can be broken by a pickax."));
            y += 30;
            if(DOUBLE_SIZE) y += 10;
            variants = p_repo->variants_get(P_STONE);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_STONE, i, 0);
              x += DOUBLE_SIZE ? 50 : 30;
            }
          
            // A line for pickax
            x = 20+X_START;
            y += 40;
            if(DOUBLE_SIZE) y += 40;
            p_font->print(NULL, x, y, _("pickax - a tool for stone crushing."));
            y += 30;
            if(DOUBLE_SIZE) y += 10;
            variants = p_repo->variants_get(P_MATTOCK);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_MATTOCK, i, 0);
              x += DOUBLE_SIZE ? 50 : 30;
            }
            break;
      
          case 2:
            x = 20+X_START;
            y = 95+Y_START;
            p_font->alignment_set(MENU_CENTER);
            p_font->print(NULL, x, y,  _("Active game elements"));
          
            // Color keys
            y += 55; // 140
            p_font->alignment_set(MENU_LEFT);
            p_font->start_set(x, y);
            p_font->print(_("color key - used to unlock color door,\n\
only a bug with identical color can\n\
pick them up"));
          
            y += 70; // 210;
            for (j = 0; j < 5; j++) {
              variants = p_repo->variants_get(P_KEY1 + j);
              for (i = 0; i < variants; i++) {
                p_repo->draw(x, y, P_KEY1 + j, i, 0);
                x += DOUBLE_SIZE ? 50 : 30;
              }
            }
          
            // Color doors
            x = 20+X_START;
            y += 40; // 250
            if(DOUBLE_SIZE) y += 40;
            p_font->start_set(x, y);
            p_font->print(_("color door - can be opened by the\nrespective color key only"));
          
            x = 40+X_START;
            if(DOUBLE_SIZE) x += 20;
            y += 50; // 300
            if(DOUBLE_SIZE) y += 10;
            variants = p_repo->variants_get(P_DOOR1_V_Z);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_DOOR1_V_Z, i, 0);              
              x += DOUBLE_SIZE ? 140 : 70;
            }
          
            x = 20+X_START;
            y += 40; // 340
            if(DOUBLE_SIZE) y += 40;
            p_font->start_set(x, y);
            p_font->print(_("color gate-way - only a bug with\n\
identical color is allowed to go\n\
through. Boxes cannot be pushed\n\
through."));
          
            x = 40+X_START;
            if(DOUBLE_SIZE) x += 20;
            y += 90; // 430;
            if(DOUBLE_SIZE) y += 10;
            variants = p_repo->variants_get(P_ID_DOOR1_V_Z);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_ID_DOOR1_V_Z, i, 0);
              x += DOUBLE_SIZE ? 140 : 70;
            }
                    
            break;
      
          case 3:
            x = 20+X_START;
            y = 95+Y_START;
            p_font->alignment_set(MENU_CENTER);
            p_font->print(NULL, x, y,  _("Active game elements"));

            // One-pass doors
            p_font->alignment_set(MENU_LEFT);
            y += 45; // 140
            p_font->start_set(x, y);
            p_font->print(_("one-pass door - can be used only once,\n\
then it is closed off and there's no\n\
way to open it\n"));
            
            x = 40+X_START;
            if(DOUBLE_SIZE) x += 20;
            y += 80; // 220
            if(DOUBLE_SIZE) y += 10;
            variants = p_repo->variants_get(P_DV_V_O);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_DV_V_O, i, 0);        
              x += DOUBLE_SIZE ? 140 : 70;
            }
            x = 40+X_START;
            if(DOUBLE_SIZE) x += 20;
            y += 40; // 260;
            if(DOUBLE_SIZE) y += 20;
            variants = p_repo->variants_get(P_DV_V_Z);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_DV_V_Z, i, 0);        
              x += DOUBLE_SIZE ? 140 : 70;
            }

            // The rest
            x = 20+X_START;
            y += 40; // 300
            if(DOUBLE_SIZE) y += 40;
            p_font->start_set(x, y);
            p_font->print(_("Other elements not listed here are just\n\
walls, which have no interesting\n\
properties. They cannot be push away nor\n\
it is possible to break them anywise.\n"));
          
            page_next = page;

            break;
          
          default:
            assert(0);
            break;
        }

        static char *prev = _("previous");
        static char *next = _("next");
        static char *back = _("back");
      
        #define MENU_X_START    (350 + (X_START/2))
        #define MENU_X_START_L  MENU_X_START
        #define MENU_X_START_R  MENU_X_START_L + 250
        #define MENU_X_START_B  MENU_X_START_L + 50
        #define MENU_Y_START    ((DOUBLE_SIZE) ? GAME_RESOLUTION_Y - 90 : 410)
        #define MENU_X_DIFF     0
        #define MENU_Y_DIFF     30
      
        menu_item_start();
        menu_item_draw(MENU_X_START_L, MENU_Y_START+0*MENU_Y_DIFF, prev, MENU_LEFT, FALSE, LEVEL_EVENT(GC_MENU_HELP_RULES,page_prev));
        menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, next, MENU_RIGHT, FALSE, LEVEL_EVENT(GC_MENU_HELP_RULES,page_next));
        menu_item_draw(MENU_X_START_B, MENU_Y_START+1*MENU_Y_DIFF, back, MENU_LEFT, FALSE, LEVEL_EVENT(GI_MENU_BACK_POP));
        
        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();                
      }
      break;
    
    case MENU_LEAVE:
      input.mevent_clear();
      break;
    
    default:
      break;
  }
}

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF
#undef X_START
#undef Y_START

void game_gui::menu_help_keys(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_help_keys, data, data1);
      
        p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
      
        if(DOUBLE_SIZE) {      
          #define LOGO_START 60
          tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);
        
          p_grf->draw(menu_background_get(),0,0);
          p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);
        }
        else {
          p_grf->draw(MENU_SPRIT_BACK,0,0);
        }
        p_font->select(FONT_DEFAULT);
        
        #define X_START ((DOUBLE_SIZE) ? 320 : 0)
        #define Y_START ((DOUBLE_SIZE) ? 300 : 100)
        #define Y_DIFF  25
            
        p_font->alignment_set(MENU_CENTER);
        p_font->print(NULL, 220, Y_START, _("Game Controls"));
      
        p_font->alignment_set(MENU_LEFT);
        p_font->print(NULL, X_START+20,  Y_START+40, _("Up to five bugs are available,\n\
which can be controlled by these keys:"));      
      
        int start_y = Y_START+100;
        p_font->print(NULL, X_START+20,  start_y, _("arrows"));
        p_font->print(NULL, X_START+20,  start_y += Y_DIFF, _("SHIFT+arrows"));
        
        p_font->print(NULL, X_START+20,  start_y += Y_DIFF, _("Tab"));
        p_font->print(NULL, X_START+20,  start_y += Y_DIFF, _("N"));
        p_font->print(NULL, X_START+20,  start_y += Y_DIFF, _("D"));
        p_font->print(NULL, X_START+20,  start_y += Y_DIFF, _("CTRL+X"));
        p_font->print(NULL, X_START+20,  start_y += Y_DIFF, _("F1"));
        p_font->print(NULL, X_START+20,  start_y += Y_DIFF, _("F2"));
        p_font->print(NULL, X_START+20,  start_y += Y_DIFF, _("F3"));
      
        start_y = Y_START+100;
        p_font->print(NULL, X_START+135,  start_y, _(". . . . . move the bug"));
        p_font->print(NULL, X_START+205,  start_y += Y_DIFF, _(". . quick bug movement"));
        p_font->print(NULL, X_START+86,  start_y += Y_DIFF, _(". . . . . . . switch among the bugs"));
        p_font->print(NULL, X_START+60,  start_y += Y_DIFF, _(". . . . . . . . change the music"));
        p_font->print(NULL, X_START+60,  start_y += Y_DIFF, _(". . . . . . . . demo"));
        p_font->print(NULL, X_START+110, start_y += Y_DIFF, _(". . . . . . quit quickly"));
        p_font->print(NULL, X_START+60,  start_y += Y_DIFF, _(". . . . . . . . Help"));
        p_font->print(NULL, X_START+60,  start_y += Y_DIFF, _(". . . . . . . . Save level"));
        p_font->print(NULL, X_START+60,  start_y += Y_DIFF, _(". . . . . . . . Load level"));

        #define MENU_X_START (GAME_RESOLUTION_X/2-50)
        #define MENU_Y_START ((DOUBLE_SIZE) ? GAME_RESOLUTION_Y - 90 : 400)
        #define MENU_X_DIFF  90
        #define MENU_Y_DIFF  35

        static char *back = _("back");
      
        menu_item_start();
        menu_item_draw(MENU_X_START, MENU_Y_START+MENU_Y_DIFF, back, MENU_LEFT, FALSE, LEVEL_EVENT(GI_MENU_BACK_POP));
        
        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();        
      }
      break;
      
    case MENU_LEAVE:
      input.mevent_clear();
      break;      
    
    default:
      break;
  }
}

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF
#undef MENU_X_START_L

#define SCROLL_START_X  0
#define SCROLL_START_Y  (DOUBLE_SIZE ? 180+60 : 180)
#define SCROLL_LINES    (DOUBLE_SIZE ? 24 : 12)

void game_gui::menu_help_credits(MENU_STATE state, size_ptr data, size_ptr data1)
{
  static char *p_text = NULL;
  static int frame = 0;
  static int position;

  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_help_credits, data, data1);          
      
        p_text = credit_text_load();
        assert(p_text);        
      
        sprite::color_key_set(COLOR_KEY_GAME);

        p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
        tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);
      
        #define LOGO_START (DOUBLE_SIZE ? 60 : 0)
        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);
        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();
      
        input.keyset_set(&menu_keys);
        
        menu_timer.set((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_help_credits, data, data1);
        frame = 0;
        position = 0;
      }
      break;

    case MENU_TIMER:
      {
        if((frame & 0x3) == 0x0) {
        
          p_font->alignment_set(MENU_CENTER);
          p_font->select(FONT_DEFAULT);
        
          if(position >= p_font->height_get_new_line(p_text)) {
            position -= p_font->height_get_new_line(p_text);
            p_text = strchr(p_text+1,'\n');
          }
        
          if(p_text) {
            p_grf->fill(0,SCROLL_START_Y,GAME_RESOLUTION_X,GAME_RESOLUTION_Y-SCROLL_START_Y,0);
            p_font->print(NULL, SCROLL_START_X, SCROLL_START_Y-position, SCROLL_LINES, p_text);
          } else {
            menu_timer.clear();
          
            #define MENU_X_START_L (GAME_RESOLUTION_X/2 - 17)
            #define MENU_Y_START   (DOUBLE_SIZE ? GAME_RESOLUTION_Y - 90 : 440)
                        
            static char *back_string = _("back");

            menu_item_start();
            menu_item_draw(MENU_X_START_L, MENU_Y_START, back_string, MENU_LEFT, FALSE, LEVEL_EVENT(GC_MENU_START));
          }
        
          p_grf->redraw_add(0,SCROLL_START_Y,GAME_RESOLUTION_X,GAME_RESOLUTION_Y-SCROLL_START_Y);
          p_grf->flip();
                    
          position += 2;
        }
        frame++;
      }    
      break;
    
    case MENU_LEAVE:
      p_text = NULL;
      menu_timer.clear();
      input.keyset_set(NULL);
      input.mevent_clear();
      break;
    
    default:
      break;
  }
}

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF

/*
  data  = set
  data1 = level
*/
void game_gui::menu_level_hint(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_level_hint, data, data1);
      
        p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
      
        if(DOUBLE_SIZE) {
          p_grf->draw(MENU_SPRIT_BACK1,0,0);
        }
        else {
          p_grf->draw(MENU_SPRIT_BACK,0,0);
        }

        p_font->select(FONT_DEFAULT);
        p_font->alignment_set(MENU_LEFT);

        char *p_hint = NULL;
        bool from_game = data;
      
        if(!p_ber->levelset_is_custom()) {
          int set = profile.level_set_get();
          int level = profile.selected_level_get();
        
          // ... Load & print hint for level ...
          p_hint = level_hint_load(set, level);
        }
      
        tpos start_x;
        tpos start_y;
      
        if(DOUBLE_SIZE) {
          start_x = GAME_RESOLUTION_X/2-60;
          start_y = GAME_RESOLUTION_Y/2;
        }
        else {
          start_x = 0;
          start_y = 0;
        }

        if(p_hint) {
          p_font->print(NULL, start_x+20, start_y+100, _("Level hint:"));
        } else {
          p_hint = _("\nSorry dude, no hint available for this\nlevel.");
        }
        p_font->print(NULL, start_x+20, start_y+120, p_hint);

        #define MENU_X_START (start_x+270)
        #define MENU_Y_START ((DOUBLE_SIZE) ? GAME_RESOLUTION_Y - 90 : 400)
        #define MENU_X_DIFF  90
        #define MENU_Y_DIFF  35

        static char *back = _("back");

        menu_item_start();
        menu_item_draw(MENU_X_START, MENU_Y_START+MENU_Y_DIFF, 
                       back, MENU_LEFT, FALSE, LEVEL_EVENT(from_game ? GC_RESTORE_LEVEL : GI_MENU_BACK_POP));

        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();        
      }
      break;
      
    case MENU_LEAVE:
      input.mevent_clear();
      break;      
    
    default:
      break;
  }
}

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF
#undef MENU_X_START_L
#undef MENU_X_START_R

#define LEVEL  'V' // Draw level - V+two digits of level number, so the first one will be "V01"
#define PIPE   'P' // Draw pipe

#define MLEFT  'L' // cursor movement
#define MRIGHT 'R' 
#define MUP    'U'
#define MDOWN  'D'

typedef enum { // index to the translation table

  LEFT_I   = 0,
  RIGHT_I  = 1,
  UP_I     = 2,
  DOWN_I   = 3,

} DIRECTION_INDEX;

/*
  LEVEL_PATH "LPUPLPD"
*/
/*
  Pipe list:

  0  -
  1  \
  2  |
  3  _|
  4  /
  5  L
*/

static int translate_direction(DIRECTION_INDEX last, DIRECTION_INDEX next)
{
  static int table_init = TRUE;
  static int pipe_table[4][4];

  #define WRONG_PIPE (-1)

  if(table_init) {
    for(int i = 0; i < 16; i++)
      ((int *)pipe_table)[i] = WRONG_PIPE;
  
    pipe_table[LEFT_I][UP_I] = 5;
    pipe_table[LEFT_I][DOWN_I] = 4;
    pipe_table[LEFT_I][LEFT_I] = 0;
  
    pipe_table[RIGHT_I][UP_I] = 3;
    pipe_table[RIGHT_I][DOWN_I] = 1;
    pipe_table[RIGHT_I][RIGHT_I] = 0;
  
    pipe_table[UP_I][UP_I] = 2;
    pipe_table[UP_I][LEFT_I] = 1;
    pipe_table[UP_I][RIGHT_I] = 4;
  
    pipe_table[DOWN_I][DOWN_I] = 2;
    pipe_table[DOWN_I][LEFT_I] = 3;
    pipe_table[DOWN_I][RIGHT_I] = 5;
  
    table_init = FALSE;
  }

  int pipe = pipe_table[last][next];
  assert(pipe != WRONG_PIPE);
  return(pipe_table[last][next]);
}

void game_gui::menu_level_draw_level(int lev,
                                     int level_act, int level_num, int level_last, int level_set,
                                     int x, int y)
{
  assert(lev < level_num);

  char *p_level_name = DOUBLE_SIZE ? p_ber->levelset_get_passwd(lev) : (char*)"";

  if(lev > level_last) {
    // Draw as inactive sprite
    int spr = LEVEL_CLOSED;
    p_grf->draw(spr,(x)*ITEM_SIZE,(y)*ITEM_SIZE);
    p_font->select(FONT_DEFAULT);
    p_font->print(NULL, (x)*ITEM_SIZE+TEXT_SHIFT_HORIZONTAL,
                        (y)*ITEM_SIZE+TEXT_SHIFT_VERTICAL,
                        p_level_name);
  }
  else {
    tpos spr_x = (x)*ITEM_SIZE;
    tpos spr_y = (y)*ITEM_SIZE;
    int spr;
  
    if(lev == level_act) {
      profile.level_spr_x = spr_x;
      profile.level_spr_y = spr_y;
      spr = LEVEL_NEXT;
    }
    else {
      spr = LEVEL_DONE;
    }
  
    p_grf->draw(spr, spr_x, spr_y);
    menu_item_draw_sprite_set(spr, spr,
                              TEXT_SHIFT_HORIZONTAL-ITEM_SIZE, TEXT_SHIFT_VERTICAL);
    menu_item_draw_sprite(spr_x,
                          spr_y,
                          p_level_name,
                          MENU_LEFT, MENU_DONT_DRAW_SPRITE,
                          LEVEL_EVENT(GC_RUN_LEVEL_SELECT, lev, spr_x, spr_y));
  }
}

void game_gui::menu_level_draw_pipe(int pip, int x, int y)
{
  if(x >= 0 &&
     y >= 0 &&
     (x+1)*ITEM_SIZE <= GAME_RESOLUTION_X &&
     (y+1)*ITEM_SIZE <= GAME_RESOLUTION_Y)
  {    
    p_grf->draw(FIRST_PIPE+pip, (x)*ITEM_SIZE, (y)*ITEM_SIZE);
  }
}

/*
  The level path is encoded from those paths + two digits of level number
*/
int game_gui::menu_level_run_path_draw_line(const char *p_path,
                                            int level_act, int level_num, int level_last, int level_set,
                                            int sx, int sy)
{
  #define translate_coords(direction, x, y, index)            \
  {                                                           \
    switch(direction) {                                       \
      case MLEFT:                                             \
        x--;                                                  \
        index = LEFT_I;                                       \
        break;                                                \
      case MRIGHT:                                            \
        x++;                                                  \
        index = RIGHT_I;                                      \
        break;                                                \
      case MUP:                                               \
        y--;                                                  \
        index = UP_I;                                         \
        break;                                                \
      case MDOWN:                                             \
        y++;                                                  \
        index = DOWN_I;                                       \
        break;                                                \
    }                                                         \
  }

  DIRECTION_INDEX index_last = LEFT_I;
  tpos lx = sx,
       ly = sy;
  int  levels = 0;


  while(p_path[0]) {
    switch(p_path[0]) {
      case LEVEL:
        {
          char tmp[3];
          tmp[0] = p_path[1];
          tmp[1] = p_path[2];
          tmp[2] = '\0';          
          int level = atoi(tmp);
          menu_level_draw_level(level,level_act,level_num,level_last,level_set,sx,sy);
          p_path += 3;
          levels++;
        }
        break;
      case PIPE:
        {
          assert(lx != sx || ly != sy);
          
          DIRECTION_INDEX index_next = LEFT_I;
          tpos fx, fy;
        
          translate_coords(p_path[1], fx, fy, index_next);
          int pipe_num = translate_direction(index_last, index_next);
        
          menu_level_draw_pipe(pipe_num, sx, sy);
          p_path++;
        }
        break;
      
      // It's a coordinate - translate it
      default:
        lx = sx;
        ly = sy;
        translate_coords(p_path[0], sx, sy, index_last);
        p_path++;
        break;
    }
  }

  return(levels);
}

// Draw level path
void game_gui::menu_level_run_path_draw(int level_set, int level_act, int level_num, int level_last)
{
  p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
  p_font->alignment_set(MENU_LEFT);
  p_font->select(FONT_DEFAULT);
  
  #define IMAGE_START         (DOUBLE_SIZE ? 100 : 50)
  #define TEXT_START          (DOUBLE_SIZE ? 400 : 250)
  
  p_grf->draw(DOUBLE_SIZE ? menu_background_get() : MENU_SPRIT_WALL, 0, 0);

  // Levels are drawn as menu
  menu_item_start();

  static char *play_string = _("play level");
  static char *level_hint  = _("level hint");
  static char *select_string = _("select last");
  static char *back_string = _("back");

  switch(level_set) {
    case 0:
      {
        // Draw top of the pipe
        menu_level_draw_pipe(2,14,0);
        menu_level_draw_pipe(2,14,1);
        menu_level_draw_level(0,level_act,level_num,level_last,level_set,14,2);
      
        menu_level_draw_pipe(3,14,3);
        menu_level_draw_pipe(4,13,3);
        menu_level_draw_level(1,level_act,level_num,level_last,level_set,13,4);
      
        menu_level_draw_pipe(3,13,5);
        menu_level_draw_pipe(0,12,5);
        menu_level_draw_pipe(4,11,5);
        menu_level_draw_level(2,level_act,level_num,level_last,level_set,11,6);
      
        menu_level_draw_pipe(2,11,7);
        menu_level_draw_level(3,level_act,level_num,level_last,level_set,11,8);
      
        menu_level_draw_pipe(2,11,9);
        menu_level_draw_level(4,level_act,level_num,level_last,level_set,11,10);
      
        menu_level_draw_pipe(3,11,11);
        menu_level_draw_pipe(0,10,11);
        menu_level_draw_pipe(4,9,11);
        menu_level_draw_level(5,level_act,level_num,level_last,level_set,9,12);

        menu_level_draw_pipe(3,9,13);
        menu_level_draw_pipe(0,8,13);
        menu_level_draw_pipe(0,7,13);
        menu_level_draw_pipe(0,6,13);
        menu_level_draw_pipe(4,5,13);
        menu_level_draw_level(6,level_act,level_num,level_last,level_set,5,14);
        menu_level_draw_pipe(0,4,14);
        menu_level_draw_pipe(0,3,14);
        menu_level_draw_pipe(0,2,14);
        menu_level_draw_pipe(0,1,14);
        menu_level_draw_pipe(0,0,14);

        menu_level_draw_pipe(2,5,15);
        menu_level_draw_pipe(2,5,16);
        menu_level_draw_level(7,level_act,level_num,level_last,level_set,5,17);
        
        menu_level_draw_pipe(2,5,18);
        menu_level_draw_pipe(5,5,19);
        menu_level_draw_pipe(0,6,19);
        menu_level_draw_pipe(0,7,19);
        menu_level_draw_level(8,level_act,level_num,level_last,level_set,8,19);

        menu_level_draw_pipe(0,10,6);
        menu_level_draw_pipe(0,9,6);
        menu_level_draw_pipe(0,8,6);
        menu_level_draw_pipe(5,7,6);
        menu_level_draw_pipe(2,7,5);
        menu_level_draw_pipe(2,7,4);
        menu_level_draw_level(9,level_act,level_num,level_last,level_set,7,3);
        menu_level_draw_pipe(2,7,2);
        menu_level_draw_pipe(2,7,1);
        menu_level_draw_pipe(2,7,0);
        
        if(!DOUBLE_SIZE) {
          #define MENU_X_START_L (GAME_RESOLUTION_X/2 + 60 - 17 - 60)
          #define MENU_X_START_R (GAME_RESOLUTION_X/2 + 60 + 60)
          #define MENU_Y_START   (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 180 : 140))
          #define MENU_X_DIFF     0
          #define MENU_Y_DIFF     30
        
          menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, play_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SET));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+1*MENU_Y_DIFF, level_hint,
                         MENU_RIGHT, MENU_SAVE_BACK, 
                         LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+2*MENU_Y_DIFF, select_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SELECT, level_last, profile.level_spr_x ,profile.level_spr_y));
          menu_item_draw(MENU_X_START_L, MENU_Y_START+3*MENU_Y_DIFF, back_string,
                         MENU_LEFT, FALSE, 
                         LEVEL_EVENT(GI_MENU_BACK_POP));
        
          #undef MENU_X_START_L
          #undef MENU_X_START_R
          #undef MENU_Y_START
          #undef MENU_X_DIFF
          #undef MENU_Y_DIFF
        }      
      }
      break;
    // Easy
    case 1:
      { 
        int lev = 0;
      
        lev += menu_level_run_path_draw_line("V01LPUPUV02UPLPUPV03UPRPRV13UPUV04UPUV05UPRPRV06UPUPU",
                                             level_act, level_num, level_last, level_set, 15, 12);
        lev += menu_level_run_path_draw_line("LPLPUPUV07LPUPUPV08UPUPUPUPU",
                                             level_act, level_num, level_last, level_set, 13, 8);
        lev += menu_level_run_path_draw_line("UPUV09UPUV10UPLPUPU",
                                             level_act, level_num, level_last, level_set, 6, 5);
        lev += menu_level_run_path_draw_line("UPRPRPRPRV00UPRV11UPUV12UPRPUV13UPUV14UPUV15UPRPUPU",
                                             level_act, level_num, level_last, level_set, 15, 12);
        lev += menu_level_run_path_draw_line("DPRV16DPDV17",
                                             level_act, level_num, level_last, level_set, 15, 12);
        lev += menu_level_run_path_draw_line("LPLPDPDV18DPDV19DPDPD",
                                             level_act, level_num, level_last, level_set, 23, 17);
        lev += menu_level_run_path_draw_line("DPDV20DPRPDPRPR",
                                             level_act, level_num, level_last, level_set, 12, -1);
        lev += menu_level_run_path_draw_line("LPLPLPLPUV21UPLPLPLPLPDV22DPDV23DPDV24LPDV25DPDV26",
                                             level_act, level_num, level_last, level_set, 16, 15);
        lev += menu_level_run_path_draw_line("DPDV27DPLPDV28DPDV29DPDPLPDPDV30DPDPDV31LPLPLPLPLPLPU",
                                             level_act, level_num, level_last, level_set, 27, -1);
        lev += menu_level_run_path_draw_line("DPLPDV32DPLPDPDPV33DPRPRPRPDPV34DPDV35",
                                             level_act, level_num, level_last, level_set, 25, 12);
        lev += menu_level_run_path_draw_line("LPLPLPLPDV36LPLPL",
                                             level_act, level_num, level_last, level_set, 6, 3);
        lev += menu_level_run_path_draw_line("DPDPV37DPDV38DPDV39DPDV40LPLPL",
                                             level_act, level_num, level_last, level_set, 2, 4);
        lev += menu_level_run_path_draw_line("DPDV41DPRPRPRPRPDPRPR",
                                             level_act, level_num, level_last, level_set, 2, 12);
        lev += menu_level_run_path_draw_line("LPDPDPV43DPDV44DPDV49DPDPDPD",
                                             level_act, level_num, level_last, level_set, 2, 14);
        lev += menu_level_run_path_draw_line("LPLPDPDPLPLPLPLPLPUV45UPUV46UPUV47LPUPUV48",
                                             level_act, level_num, level_last, level_set, 14, 10);
        //assert(lev == 50);
        
        if(!DOUBLE_SIZE) {
          #define MENU_X_START_L (GAME_RESOLUTION_X/2 - 17 - 60 - 10)
          #define MENU_X_START_R (GAME_RESOLUTION_X/2 + 60 - 10)
          #define MENU_Y_START   (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 180 : 130))
          #define MENU_X_DIFF     0
          #define MENU_Y_DIFF     30
        
          menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, play_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SET));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+1*MENU_Y_DIFF, level_hint,
                         MENU_RIGHT, MENU_SAVE_BACK, 
                         LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+2*MENU_Y_DIFF, select_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SELECT, level_last, profile.level_spr_x ,profile.level_spr_y));
          menu_item_draw(MENU_X_START_L, MENU_Y_START+3*MENU_Y_DIFF, back_string,
                         MENU_LEFT, FALSE, 
                         LEVEL_EVENT(GI_MENU_BACK_POP));
        
          #undef MENU_X_START_L
          #undef MENU_X_START_R
          #undef MENU_Y_START
          #undef MENU_X_DIFF
          #undef MENU_Y_DIFF
        }
      }
      break;
    // Intermediate
    case 2:
      {      
        // 35 levels
        #undef  LEVEL_LINE
        #define LEVEL_LINE "DPDV00DPDV01DPDV02DPLPDV03DPDV04DPDV05DPLPDV06DPDV07DPDV08DPLPLPLPLPUV09UP" \
                           "UV10UPUV11UPLPUV12UPLPUV13UPUV14UPUV15LPUPUPUV16UPLPLPDPLPDPLPLPDV17DPDV18" \
                           "DPDV19DPDV20DPLPDPDV21DPDV22LPDPDPDPLPDPDPLPLPLPUPUPUV23UPUV24UPUV25UPLPUV26UPUV" \
                           "27UPUV28UPUV29UPRPUPV30UPUV31UPLPLPDPDPLPLPLPDPV32DPDV33DPLPDPDV34DPLP"
        int lev = 0;
      
        lev += menu_level_run_path_draw_line(LEVEL_LINE, level_act, level_num, level_last, level_set, 27, -1);
        assert(lev == 35);
      
        if(!DOUBLE_SIZE) {
          #define MENU_X_START_L (GAME_RESOLUTION_X/2 +20 - 17 - 60)
          #define MENU_X_START_R (GAME_RESOLUTION_X/2 +20 + 60)
          #define MENU_Y_START   (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 180 : 130))
          #define MENU_X_DIFF     0
          #define MENU_Y_DIFF     30
        
          menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, play_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SET));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+1*MENU_Y_DIFF, level_hint,
                         MENU_RIGHT, MENU_SAVE_BACK, 
                         LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+2*MENU_Y_DIFF, select_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SELECT, level_last, profile.level_spr_x ,profile.level_spr_y));
          menu_item_draw(MENU_X_START_L, MENU_Y_START+3*MENU_Y_DIFF, back_string,
                         MENU_LEFT, FALSE, 
                         LEVEL_EVENT(GI_MENU_BACK_POP));
        
          #undef MENU_X_START_L
          #undef MENU_X_START_R
          #undef MENU_Y_START
          #undef MENU_X_DIFF
          #undef MENU_Y_DIFF
        }      
      }
      break;
    case 3:
      {
        // Draw center of the net
        #define ADV_START_X 15
        #define ADV_START_Y 10
      
        menu_level_draw_level(0,level_act,level_num,level_last,level_set,ADV_START_X, ADV_START_Y);
      
        menu_level_draw_pipe(2, ADV_START_X, ADV_START_Y-1);
        menu_level_draw_level(1,level_act,level_num,level_last,level_set,ADV_START_X, ADV_START_Y-2);
      
        menu_level_draw_pipe(2, ADV_START_X, ADV_START_Y-3);
        menu_level_draw_pipe(2, ADV_START_X, ADV_START_Y-4);
        menu_level_draw_level(3,level_act,level_num,level_last,level_set,ADV_START_X, ADV_START_Y-5);
      
        menu_level_draw_pipe(0, ADV_START_X-1, ADV_START_Y-5);
        menu_level_draw_pipe(5, ADV_START_X-2, ADV_START_Y-5);
        menu_level_draw_pipe(2, ADV_START_X-2, ADV_START_Y-6);
        menu_level_draw_level(3,level_act,level_num,level_last,level_set,ADV_START_X-2, ADV_START_Y-7);
      
        menu_level_draw_pipe(2, ADV_START_X-2, ADV_START_Y-8);
        menu_level_draw_pipe(2, ADV_START_X-2, ADV_START_Y-9);
        menu_level_draw_pipe(2, ADV_START_X-2, ADV_START_Y-10);

        menu_level_draw_pipe(2, ADV_START_X, ADV_START_Y+1);
        menu_level_draw_pipe(2, ADV_START_X, ADV_START_Y+2);
        menu_level_draw_pipe(5, ADV_START_X, ADV_START_Y+3);
        menu_level_draw_pipe(0, ADV_START_X+1, ADV_START_Y+3);
        menu_level_draw_level(4,level_act,level_num,level_last,level_set,ADV_START_X+2, ADV_START_Y+3);

        menu_level_draw_pipe(4, ADV_START_X+2, ADV_START_Y+2);
        menu_level_draw_pipe(0, ADV_START_X+3, ADV_START_Y+2);
        menu_level_draw_pipe(0, ADV_START_X+4, ADV_START_Y+2);        
        menu_level_draw_level(5,level_act,level_num,level_last,level_set,ADV_START_X+5, ADV_START_Y+2);

        menu_level_draw_pipe(2, ADV_START_X+5, ADV_START_Y+1);
        menu_level_draw_pipe(2, ADV_START_X+5, ADV_START_Y);
        menu_level_draw_level(6,level_act,level_num,level_last,level_set,ADV_START_X+5, ADV_START_Y-1);

        menu_level_draw_pipe(4, ADV_START_X+5, ADV_START_Y-2);
        menu_level_draw_pipe(0, ADV_START_X+6, ADV_START_Y-2);
        menu_level_draw_pipe(0, ADV_START_X+7, ADV_START_Y-2);
        menu_level_draw_pipe(0, ADV_START_X+8, ADV_START_Y-2);
        menu_level_draw_level(7,level_act,level_num,level_last,level_set,ADV_START_X+9, ADV_START_Y-2);

        menu_level_draw_pipe(2, ADV_START_X+9, ADV_START_Y-3);
        menu_level_draw_pipe(2, ADV_START_X+9, ADV_START_Y-4);
        menu_level_draw_pipe(2, ADV_START_X+9, ADV_START_Y-5);
        menu_level_draw_level(8,level_act,level_num,level_last,level_set,ADV_START_X+9, ADV_START_Y-6);

        menu_level_draw_pipe(2, ADV_START_X+9, ADV_START_Y-1);
        menu_level_draw_pipe(2, ADV_START_X+9, ADV_START_Y);
        menu_level_draw_level(9,level_act,level_num,level_last,level_set,ADV_START_X+9, ADV_START_Y+1);
        
        menu_level_draw_pipe(5, ADV_START_X+2, ADV_START_Y+4);
        menu_level_draw_pipe(0, ADV_START_X+3, ADV_START_Y+4);
        menu_level_draw_level(10,level_act,level_num,level_last,level_set,ADV_START_X+4, ADV_START_Y+4);
        
        menu_level_run_path_draw_line("DPRPRPDPV11DPD",
                                      level_act, level_num, level_last, level_set, ADV_START_X+4, ADV_START_Y+4);
        menu_level_draw_level(12,level_act,level_num,level_last,level_set,ADV_START_X+6, ADV_START_Y+8);

        menu_level_draw_pipe(0, ADV_START_X-1, ADV_START_Y);
        menu_level_draw_pipe(0, ADV_START_X-2, ADV_START_Y);
        menu_level_draw_pipe(0, ADV_START_X-3, ADV_START_Y);
        menu_level_draw_pipe(0, ADV_START_X-4, ADV_START_Y);
        menu_level_draw_pipe(5, ADV_START_X-5, ADV_START_Y);
        menu_level_draw_level(13,level_act,level_num,level_last,level_set,ADV_START_X-5, ADV_START_Y-1);
        menu_level_draw_pipe(0, ADV_START_X-1, ADV_START_Y-2);
        menu_level_draw_pipe(0, ADV_START_X-2, ADV_START_Y-2);
        menu_level_draw_pipe(0, ADV_START_X-3, ADV_START_Y-2);
        menu_level_draw_pipe(0, ADV_START_X-4, ADV_START_Y-2);
        menu_level_draw_pipe(4, ADV_START_X-5, ADV_START_Y-2);

        menu_level_draw_pipe(0, ADV_START_X-6, ADV_START_Y-1);
        menu_level_draw_pipe(0, ADV_START_X-7, ADV_START_Y-1);
        menu_level_draw_pipe(0, ADV_START_X-8, ADV_START_Y-1);
        menu_level_draw_pipe(0, ADV_START_X-9, ADV_START_Y-1);
        menu_level_draw_pipe(5, ADV_START_X-10, ADV_START_Y-1);
        menu_level_draw_level(14,level_act,level_num,level_last,level_set,ADV_START_X-10, ADV_START_Y-2);

        menu_level_draw_pipe(2, ADV_START_X-10, ADV_START_Y-3);
        menu_level_draw_pipe(2, ADV_START_X-10, ADV_START_Y-4);
        menu_level_draw_level(15,level_act,level_num,level_last,level_set,ADV_START_X-10, ADV_START_Y-5);

        menu_level_draw_pipe(2, ADV_START_X+6, ADV_START_Y+9);
        menu_level_draw_pipe(2, ADV_START_X+6, ADV_START_Y+10);
        menu_level_draw_level(16,level_act,level_num,level_last,level_set,ADV_START_X+6, ADV_START_Y+11);
        
        menu_level_draw_pipe(2, ADV_START_X-10, ADV_START_Y-6);
        menu_level_draw_pipe(2, ADV_START_X-10, ADV_START_Y-7);
        menu_level_draw_level(17,level_act,level_num,level_last,level_set,ADV_START_X-10, ADV_START_Y-8);
        
        menu_level_draw_pipe(4, ADV_START_X-11, ADV_START_Y-2);
        menu_level_draw_pipe(2, ADV_START_X-11, ADV_START_Y-1);
        menu_level_draw_pipe(2, ADV_START_X-11, ADV_START_Y);
        menu_level_draw_pipe(2, ADV_START_X-11, ADV_START_Y+1);
        menu_level_draw_level(18,level_act,level_num,level_last,level_set,ADV_START_X-11, ADV_START_Y+2);
        
        menu_level_draw_pipe(3, ADV_START_X-11, ADV_START_Y+3);
        menu_level_draw_pipe(0, ADV_START_X-12, ADV_START_Y+3);        
        menu_level_draw_pipe(4, ADV_START_X-13, ADV_START_Y+3);
        menu_level_draw_pipe(2, ADV_START_X-13, ADV_START_Y+4);
        menu_level_draw_pipe(2, ADV_START_X-13, ADV_START_Y+5);
        menu_level_draw_level(19,level_act,level_num,level_last,level_set,ADV_START_X-13, ADV_START_Y+6);
        
        menu_level_draw_pipe(0, ADV_START_X-14, ADV_START_Y+6);
        menu_level_draw_pipe(0, ADV_START_X-15, ADV_START_Y+6);

        if(!DOUBLE_SIZE) {
          #define MENU_X_START_L (GAME_RESOLUTION_X/2 - 17 - 60 - 60)
          #define MENU_X_START_R (GAME_RESOLUTION_X/2 + 60 - 60)
          #define MENU_Y_START   (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 180 : 130))
          #define MENU_X_DIFF     0
          #define MENU_Y_DIFF     30
        
          menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, play_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SET));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+1*MENU_Y_DIFF, level_hint,
                         MENU_RIGHT, MENU_SAVE_BACK, 
                         LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+2*MENU_Y_DIFF, select_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SELECT, level_last, profile.level_spr_x ,profile.level_spr_y));
          menu_item_draw(MENU_X_START_L, MENU_Y_START+3*MENU_Y_DIFF, back_string,
                         MENU_LEFT, FALSE, 
                         LEVEL_EVENT(GI_MENU_BACK_POP));
        
          #undef MENU_X_START_L
          #undef MENU_X_START_R
          #undef MENU_Y_START
          #undef MENU_X_DIFF
          #undef MENU_Y_DIFF
        }
      }
      break;
    case 4:
      {
        // Draw top of the pipe      
        #define IMP_START_X 15
        #define IMP_START_Y 6
      
        menu_level_draw_pipe(5,IMP_START_X+1,IMP_START_Y);
        menu_level_draw_level(0,level_act,level_num,level_last,level_set,IMP_START_X+2, IMP_START_Y);
      
        menu_level_draw_pipe(2,IMP_START_X+2,IMP_START_Y+1);
        menu_level_draw_level(1,level_act,level_num,level_last,level_set,IMP_START_X+2, IMP_START_Y+2);
      
        menu_level_draw_pipe(5,IMP_START_X+2,IMP_START_Y+3);
        menu_level_draw_pipe(0,IMP_START_X+3,IMP_START_Y+3);
        menu_level_draw_pipe(1,IMP_START_X+4,IMP_START_Y+3);
        menu_level_draw_level(2,level_act,level_num,level_last,level_set,IMP_START_X+4, IMP_START_Y+4);
      
        menu_level_draw_pipe(2,IMP_START_X+4,IMP_START_Y+5);
        menu_level_draw_pipe(3,IMP_START_X+4,IMP_START_Y+6);
        menu_level_draw_pipe(0,IMP_START_X+3,IMP_START_Y+6);
        menu_level_draw_pipe(0,IMP_START_X+3,IMP_START_Y+6);
        menu_level_draw_pipe(0,IMP_START_X+2,IMP_START_Y+6);
        menu_level_draw_pipe(0,IMP_START_X+1,IMP_START_Y+6);
        menu_level_draw_pipe(4,IMP_START_X+0,IMP_START_Y+6);
        menu_level_draw_pipe(3,IMP_START_X+0,IMP_START_Y+7);
        menu_level_draw_pipe(0,IMP_START_X-1,IMP_START_Y+7);
        menu_level_draw_pipe(5,IMP_START_X-2,IMP_START_Y+7);
        menu_level_draw_pipe(2,IMP_START_X-2,IMP_START_Y+6);
        menu_level_draw_level(3,level_act,level_num,level_last,level_set,IMP_START_X-2, IMP_START_Y+5);

        menu_level_draw_pipe(1,IMP_START_X-2,IMP_START_Y+4);
        menu_level_draw_pipe(0,IMP_START_X-3,IMP_START_Y+4);
        menu_level_draw_pipe(5,IMP_START_X-4,IMP_START_Y+4);
        menu_level_draw_level(4,level_act,level_num,level_last,level_set,IMP_START_X-4, IMP_START_Y+3);

        menu_level_draw_pipe(4,IMP_START_X-4,IMP_START_Y+2);
        menu_level_draw_pipe(0,IMP_START_X-3,IMP_START_Y+2);
        menu_level_draw_pipe(0,IMP_START_X-2,IMP_START_Y+2);
        menu_level_draw_pipe(3,IMP_START_X-1,IMP_START_Y+2);
        menu_level_draw_pipe(2,IMP_START_X-1,IMP_START_Y+1);
        menu_level_draw_pipe(2,IMP_START_X-1,IMP_START_Y);
        menu_level_draw_pipe(2,IMP_START_X-1,IMP_START_Y-1);
        menu_level_draw_pipe(4,IMP_START_X-1,IMP_START_Y-2);        
        menu_level_draw_pipe(0,IMP_START_X  ,IMP_START_Y-2);
        menu_level_draw_pipe(1,IMP_START_X+1,IMP_START_Y-2);
        menu_level_draw_pipe(2,IMP_START_X+1,IMP_START_Y-1);
        
        if(!DOUBLE_SIZE) {
          #define MENU_X_START_L (GAME_RESOLUTION_X/2 - 17 - 60)
          #define MENU_X_START_R (GAME_RESOLUTION_X/2 + 60)
          #define MENU_Y_START   (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 180 : 130))
          #define MENU_X_DIFF     0
          #define MENU_Y_DIFF     30
        
          menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, play_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SET));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+1*MENU_Y_DIFF, level_hint,
                         MENU_RIGHT, MENU_SAVE_BACK, 
                         LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+2*MENU_Y_DIFF, select_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SELECT, level_last, profile.level_spr_x ,profile.level_spr_y));
          menu_item_draw(MENU_X_START_L, MENU_Y_START+3*MENU_Y_DIFF, back_string,
                         MENU_LEFT, FALSE, 
                         LEVEL_EVENT(GI_MENU_BACK_POP));
        
          #undef MENU_X_START_L
          #undef MENU_X_START_R
          #undef MENU_Y_START
          #undef MENU_X_DIFF
          #undef MENU_Y_DIFF
        }      
      }
      break;
    case 5:
      {
        // 32 levels
/*
        #define LEVEL_LINE "DPDV00DPDV01DPDV02DPLPDV03DPDV04DPDV05DPLPDV06DPDV07DPDV08DPLPLPLPLPUV09UP" \
                           "UV10UPUV11UPLPUV12UPLPUV13UPUV14UPUV15LPUPUPUV16UPLPLPDPLPDPLPLPDV17DPDV18" \
                           "DPDV19DPDV20DPLPDPDV21DPDV22LPDPDPDPLPDPDPLPLPLPUPUPUV23UPUV24UPUV25UPLPUV26UPUV" \
                           "27UPUV28UPUV29UPRPUPV30UPUV31"
*/
        #undef  LEVEL_LINE
        #define LEVEL_LINE "DPDV00DPDV01DPDV02DPLPDV03DPDV04DPDV05DPRPDV06DPDV07DPDV08DPDPDPDV09LPLP"
        int lev = 0;
      
        lev += menu_level_run_path_draw_line(LEVEL_LINE, level_act, level_num, level_last, level_set, 2, -1);
      
        #undef  LEVEL_LINE
        #define LEVEL_LINE "DPDV10"
        lev += menu_level_run_path_draw_line(LEVEL_LINE, level_act, level_num, level_last, level_set, 11, -1);
        //assert(lev == 32);
      
        if(!DOUBLE_SIZE) {
          #define MENU_X_START_L (GAME_RESOLUTION_X/2 +20 - 17 - 60)
          #define MENU_X_START_R (GAME_RESOLUTION_X/2 +20 + 60)
          #define MENU_Y_START   (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 180 : 130))
          #define MENU_X_DIFF     0
          #define MENU_Y_DIFF     30
        
          menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, play_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SET));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+1*MENU_Y_DIFF, level_hint,
                         MENU_RIGHT, MENU_SAVE_BACK, 
                         LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
          menu_item_draw(MENU_X_START_R, MENU_Y_START+2*MENU_Y_DIFF, select_string,
                         MENU_RIGHT, FALSE, 
                         LEVEL_EVENT(GC_RUN_LEVEL_SELECT, level_last, profile.level_spr_x ,profile.level_spr_y));
          menu_item_draw(MENU_X_START_L, MENU_Y_START+3*MENU_Y_DIFF, back_string,
                         MENU_LEFT, FALSE, 
                         LEVEL_EVENT(GI_MENU_BACK_POP));
        
          #undef MENU_X_START_L
          #undef MENU_X_START_R
          #undef MENU_Y_START
          #undef MENU_X_DIFF
          #undef MENU_Y_DIFF
        }
      }
      break;
    default:
      assert(0);
      break;
  }

  #undef MENU_X_START_L
  #undef MENU_X_START_R
  #undef MENU_Y_START
  #undef MENU_X_DIFF
  #undef MENU_Y_DIFF

  if(DOUBLE_SIZE) {
    #define MENU_X_START_L (GAME_RESOLUTION_X/2 - 17 - 60)
    #define MENU_X_START_R (GAME_RESOLUTION_X/2 + 60)
    #define MENU_Y_START   (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 180 : 180))
    #define MENU_X_DIFF     0
    #define MENU_Y_DIFF     35
  
    menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, play_string,
                   MENU_RIGHT, FALSE, 
                   LEVEL_EVENT(GC_RUN_LEVEL_SET));
    menu_item_draw(MENU_X_START_R, MENU_Y_START+1*MENU_Y_DIFF, level_hint,
                   MENU_RIGHT, MENU_SAVE_BACK, 
                   LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
    menu_item_draw(MENU_X_START_R, MENU_Y_START+2*MENU_Y_DIFF, select_string,
                   MENU_RIGHT, FALSE, 
                   LEVEL_EVENT(GC_RUN_LEVEL_SELECT, level_last, profile.level_spr_x ,profile.level_spr_y));
    menu_item_draw(MENU_X_START_L, MENU_Y_START+3*MENU_Y_DIFF, back_string,
                   MENU_LEFT, FALSE, 
                   LEVEL_EVENT(GI_MENU_BACK_POP));
  }

  p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
  p_grf->flip();
}

// New level set - based on profiles
void game_gui::menu_level_run_new(MENU_STATE state, size_ptr level_set, size_ptr unused)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {            
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_level_run_new, level_set, unused);
      
        level_set_select(level_set);
        menu_level_run_path_draw(level_set,
                                 profile.selected_level_get(),
                                 p_ber->levelset_get_levelnum(),
                                 profile.last_level_get());
        menu_level_name_print();
      }
      break;
    
    case MENU_LEAVE:
      input.mevent_clear();
      break;      
    
    default:
      break;
  }
}

#undef MENU_X_START
#undef MENU_Y_START

/* Print selected level name
*/
void game_gui::menu_level_name_print(void)
{
  int  level = profile.selected_level_get();
  int  level_set = profile.level_set_get();

  p_font->select(FONT_DEFAULT);
  p_font->alignment_set(MENU_LEFT);

  int menu_x_start = 0;
  int menu_y_start = 0;

  if(DOUBLE_SIZE) {
    p_font->alignment_set(MENU_CENTER);
    menu_x_start = 20;
    menu_y_start = (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 220 : 150));
  }
  else {
    switch(level_set) {
      case 0:
        menu_x_start = (GAME_RESOLUTION_X/2 - 70);
        menu_y_start = (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 220 : 170));
        break;
      case 1:
        menu_x_start = (GAME_RESOLUTION_X/2 - 130);
        menu_y_start = (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 220 : 155));
        break;
      case 2:
        menu_x_start = 10;
        menu_y_start = (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 220 : 25));
        break;
      case 3:
        menu_x_start = (GAME_RESOLUTION_X/2 - 17 - 180);
        menu_y_start = (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 220 : 160));
        break;
      case 4:
        p_font->alignment_set(MENU_CENTER);
        menu_x_start = 20;
        menu_y_start = (GAME_RESOLUTION_Y - (DOUBLE_SIZE ? 220 : 160));
        break;
    }
  }

  RECT r;
  p_font->try_run_set(TRUE);
  p_font->print(&r, menu_x_start, menu_y_start, _("Level: %d - %s"),level+1,
                p_ber->levelset_get_passwd(level));
  p_font->try_run_set(FALSE);

  // Adjust the stored rectange
  #define NAME_MARGIN (DOUBLE_SIZE ? 20 : 10)
  r.x -= NAME_MARGIN;
  r.w += NAME_MARGIN*2;

  static SURFACE *p_background = NULL;
  static tpos     background_x;
  static tpos     background_y;

  // We have stored background here - restore it first
  if(p_background) {
    p_grf->draw(p_background, background_x, background_y);
    delete p_background;
    p_background = NULL;
  }

  assert(!p_background);

  // Store the background
  p_background = new SURFACE(p_grf->screen_surface_get(), r.x, r.y, r.w, r.h);
  background_x = r.x;
  background_y = r.y;

  // And print the level name
  p_font->print(NULL, menu_x_start, menu_y_start, _("Level: %d - %s"),level+1,
                p_ber->levelset_get_passwd(level));

  p_grf->redraw_add(&r);
  p_grf->flip();
}

/* Load a hint for given level */

#define MARK_START  "~%d_%d"
#define MARK_END    "~"

char * game_gui::level_hint_load(int set, int level)
{  
  FHANDLE f = file_open(p_dir->gamedata_get(), LEVEL_HINTS_FILE, "r");

  char start_mark[1000];
  snprintf(start_mark,1000,MARK_START,set+1,level+1);

  // Search a mark in the hint file
  char line[1000];
  char *p_tmp = NULL;
  while(fgets(line,1000,f)) {
    if((p_tmp = strstr(line,start_mark)) && p_tmp == line) {
      break;
    }
  }

  // Did we find it?
  if(!p_tmp || p_tmp != line) {
    return(NULL);
  }    

  // Load the hind
  #define HINT_BUFFER_LENGHT 40000
  static char hint_buffer[HINT_BUFFER_LENGHT];

  p_tmp = hint_buffer;
  while(1) {
    p_tmp = fgets(p_tmp, HINT_BUFFER_LENGHT - (p_tmp - hint_buffer), f);
    if(!p_tmp)
      break;
    if(p_tmp[0] == MARK_END[0]) {
      *p_tmp = '\0';
      break;
    }
    p_tmp = p_tmp + strlen(p_tmp);
  }

  fclose(f);
  return(hint_buffer);
}

/* Load end text for given episode */
char * game_gui::level_end_text_load(int set)
{
  char file[MAX_FILENAME];
  snprintf(file,MAX_FILENAME,SET_END_FILE_TEXT,set);

  #define END_BUFFER_LENGHT 40000
  static char end_buffer[END_BUFFER_LENGHT];
  file_load_text(p_dir->gamedata_get(),file,end_buffer,END_BUFFER_LENGHT);

  return(end_buffer);
}

/* Load text for credits */
char * game_gui::credit_text_load(void)
{
  #define CREDITS_FILE "credits.dat"

  // Load the whole file    
  #define END_BUFFER_LENGHT 40000
  static char cr_buffer[END_BUFFER_LENGHT];
  file_load_text(p_dir->gamedata_get(), CREDITS_FILE, cr_buffer, END_BUFFER_LENGHT);

  return(cr_buffer);
}

/* Load an external level from command line */
bool game_gui::level_run(LEVEL_EVENT_QUEUE *p_queue, char *p_level)
{
  assert(p_ber);
    
  /* Quit if it fails */
  if(p_ber->level_load(p_level) && p_ber->level_play(p_queue, 0)) {
    menu_leave();
    input.keyset_set(&game_keys);
    return(TRUE);
  } else {
    menu_dialog_error(_("Unable to load level %s"),p_level);
    p_queue->add(LEVEL_EVENT(GC_MENU_QUIT));
    return(FALSE);
  }
}

/* Load specified level from level set */
bool game_gui::level_run(LEVEL_EVENT_QUEUE *p_queue)
{
  assert(p_ber);

  menu_leave();

  int set = profile.level_set_get();
  int level = profile.selected_level_get();

  /* Run a menu if it fails */
  if(p_ber->levelset_load(set) && p_ber->level_play(p_queue, level)) {
    input.keyset_set(&game_keys);
    return(TRUE);
  } else {
    menu_dialog_error(_("Unable to load level %d from set %d"),level,set);
    p_queue->add(LEVEL_EVENT(GC_MENU_START));
    return(FALSE);
  }
}

//void level_stop(LEVEL_EVENT_QUEUE *p_queue, bool resolved, bool levelset, int set, int level);
void game_gui::level_stop(LEVEL_EVENT_QUEUE *p_queue, int cheat, int menu)
{
  assert(p_ber);

  input.keyset_set(NULL);

  LEVEL_STATUS *p_status = p_ber->level_stop(p_queue);

  if(menu) {
    int level = p_ber->levelset_get_level();
    int set = p_ber->levelset_get_set();
  
    if(cheat)
      p_status->bug_in_exit();
  
    if(p_status->resolved()) {
      if(p_ber->levelset_is_custom()) {
        p_queue->add(LEVEL_EVENT(GC_MENU_END_LEVEL_CUSTOM));
      } else {
        /* Mark the level as finished */
        profile.selected_level_finished();

        if(level+1 < p_ber->levelset_get_levelnum()) {
          /* There are more levels to finish */
          p_queue->add(LEVEL_EVENT(GC_MENU_END_LEVEL));
        } else {
          /* The whole level-set has been finished */
          p_queue->add(LEVEL_EVENT(GC_MENU_END_LEVEL_SET, set));
        }    
      }
    } else {
      if(p_ber->levelset_is_custom()) {
        /* if we run it from command line, stop it */
        p_queue->add(LEVEL_EVENT(GC_MENU_QUIT));
      } else {
        /* if we run it from set, return to menu */
        p_queue->add(LEVEL_EVENT(GC_MENU_END_LEVEL));
      }
    }
  }
#if EVENTS_DEBUG
  p_queue->dump_write("Write - game_gui::level_stop() - end");
#endif 
}

#undef MENU_X_START_L
#undef MENU_X_START_R
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF

void game_gui::menu_level_end(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_level_end, data, data1);

        /* Some statistics here */
        p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
        p_font->alignment_set(MENU_CENTER);
        p_font->select(FONT_DEFAULT);

        LEVEL_STATUS *p_status = p_ber->level_status_get();
        char tmp[100];

        if(DOUBLE_SIZE) {
          p_grf->draw(menu_background_get(),0,0);
        }

        if(DOUBLE_SIZE) {
          #define LOGO_START (DOUBLE_SIZE ? 60 : 0)
          tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);
          p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2, LOGO_START);
        }
        else {
          #define SMALL_LOGO_START 80
          p_grf->draw(MENU_SPRIT_LOGO_SMALL_2,p_grf->sprite_get_width_center(MENU_SPRIT_LOGO_SMALL_2),SMALL_LOGO_START);
        }
      
        #define END_TEXT_START (DOUBLE_SIZE ? (GAME_RESOLUTION_Y/2-100) : 80)
      
        if(p_status->resolved()) {
          p_font->print(NULL,0,END_TEXT_START+100,_("your bugs have survived!"));
          p_font->print(NULL,0,END_TEXT_START+130,_("difficulty %s"), p_ber->levelset_get_difficulty());
          p_font->print(NULL,0,END_TEXT_START+180,_("they made %d steps"), p_status->steps_get());
          p_font->print(NULL,0,END_TEXT_START+210,_("and %s."), p_status->time_get(tmp,100));
        } else {
          p_font->print(NULL,0,END_TEXT_START+100,_("your bugs have given it up!"));
          p_font->print(NULL,0,END_TEXT_START+130,_("difficulty %s"), p_ber->levelset_get_difficulty());
          p_font->print(NULL,0,END_TEXT_START+180,_("they made %d steps"), p_status->steps_get());
          p_font->print(NULL,0,END_TEXT_START+210,_("and spent %s"), p_status->time_get(tmp,100));
        }

        p_grf->draw(MENU_SPRIT_LOGO_SMALL_3,
                    p_grf->sprite_get_width_center(MENU_SPRIT_LOGO_SMALL_3),
                    GAME_RESOLUTION_Y-130);
        
        #define MENU_X_START_L (GAME_RESOLUTION_X/2 - 17 - 120)
        #define MENU_X_START_R (GAME_RESOLUTION_X/2 + 120)
        #define MENU_Y_START   (DOUBLE_SIZE ? (GAME_RESOLUTION_Y-80) : 400)
        #define MENU_X_DIFF     0
        #define MENU_Y_DIFF     30
        
        static char *play_string = _("play next level");
        static char *back_string = _("back to menu");
      
        menu_item_start();
      
        if(p_status->resolved()) {
          // Run next level directly
          menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, play_string, MENU_RIGHT, FALSE, LEVEL_EVENT(GC_RUN_LEVEL_SET));
        }
        
        menu_item_draw(MENU_X_START_L, MENU_Y_START+1*MENU_Y_DIFF, back_string, MENU_LEFT, FALSE, LEVEL_EVENT(GC_MENU_RUN_LEVEL));

        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();        
      }
      break;
    
    case MENU_LEAVE:
      input.mevent_clear();
      break;
    
    default:
      break;
  }
}

#undef MENU_X_START
#undef MENU_X_START_L
#undef MENU_X_START_R
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF

void game_gui::menu_level_end_custom(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_level_end_custom, data, data1);

        /* Some statistics here */
        p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
        p_font->alignment_set(MENU_CENTER);
        p_font->select(FONT_DEFAULT);

        if(DOUBLE_SIZE) {
          p_grf->draw(menu_background_get(),0,0);
        }

        if(DOUBLE_SIZE) {
          #define LOGO_START (DOUBLE_SIZE ? 60 : 0)
          tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);
          p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2, LOGO_START);
        }
        else {
          #define SMALL_LOGO_START 80
          p_grf->draw(MENU_SPRIT_LOGO_SMALL_2,p_grf->sprite_get_width_center(MENU_SPRIT_LOGO_SMALL_2),SMALL_LOGO_START);
        }

        #define END_TEXT_START (DOUBLE_SIZE ? (GAME_RESOLUTION_Y/2-100) : 80)

        p_font->print(NULL,0,END_TEXT_START+100,_("your bugs have survived!"));
        p_font->print(NULL,0,END_TEXT_START+130,_("custom level %s."), p_ber->levelset_get_name());

        LEVEL_STATUS *p_status = p_ber->level_status_get();
        p_font->print(NULL,0,END_TEXT_START+180,_("it took %d steps"), p_status->steps_get());
        char tmp[100];
        p_font->print(NULL,0,END_TEXT_START+210,_("and %s."), p_status->time_get(tmp,100));      

        p_grf->draw(MENU_SPRIT_LOGO_SMALL_3,
                    p_grf->sprite_get_width_center(MENU_SPRIT_LOGO_SMALL_3),
                    GAME_RESOLUTION_Y-130);

        #define MENU_X_START   (GAME_RESOLUTION_X/2 - 35)
        #define MENU_Y_START   (DOUBLE_SIZE ? (GAME_RESOLUTION_Y-80) : 400)
        #define MENU_X_DIFF     0
        #define MENU_Y_DIFF     30
                
        static char *back_string = _("quit");
                    
        menu_item_start();
        menu_item_draw(MENU_X_START, MENU_Y_START+0*MENU_Y_DIFF, back_string, MENU_LEFT, FALSE, LEVEL_EVENT(GC_MENU_QUIT));

        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();        
      }
      break;
    
    case MENU_LEAVE:
      input.mevent_clear();
      break;
    
    default:
      break;
  }
}

#undef MENU_X_START
#undef MENU_X_START_L
#undef MENU_X_START_R
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF

#undef SCROLL_START_X
#undef SCROLL_START_Y
#undef SCROLL_LINES

/*
  int set = data;
*/
void game_gui::menu_levelset_end(MENU_STATE state, size_ptr data, size_ptr data1)
{
  static char *p_text = NULL;
  static int frame = 0;
  static int position;

  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_levelset_end, data, data1);
        
        int set = data;
      
        p_text = level_end_text_load(set);
        assert(p_text);        
      
        sprite::color_key_set(COLOR_KEY_BLACK);
        p_grf->sprite_delete(MENU_SPRIT_END, 1);
      
        char line[1000];
        snprintf(line, 1000, SET_END_FILE_BACK, set+1);
        p_grf->sprite_insert(line, MENU_SPRIT_END);
      
        input.keyset_set(&menu_keys);
        
        menu_timer.set((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_levelset_end, data, data1);
        frame = 0;
        position = 0;
      }
      break;
    
    case MENU_TIMER:
      {
        if((frame & 0x3) == 0x0) {
          p_font->alignment_set(MENU_CENTER);
          p_font->select(FONT_DEFAULT);
        
          tpos width = p_grf->sprite_get_width(MENU_SPRIT_END);
        
          p_grf->fill(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y, 0);
          p_grf->draw(MENU_SPRIT_END,GAME_RESOLUTION_X/2-width/2,DOUBLE_SIZE ? 60 : 0);

          if(position >= p_font->height_get_new_line(p_text)) {
            position -= p_font->height_get_new_line(p_text);
            p_text = strchr(p_text+1,'\n');
          }

          if(p_text) {
            #define SCROLL_START_X  0
            #define SCROLL_START_Y  0
            #define SCROLL_LINES    (DOUBLE_SIZE ? 40 : 20)

            p_font->print(NULL, SCROLL_START_X, SCROLL_START_Y-position, SCROLL_LINES, p_text);
          } else {
            menu_timer.clear();

            #define MENU_X_START_L (GAME_RESOLUTION_X/2 - 17)
            #define MENU_Y_START   (DOUBLE_SIZE ? (GAME_RESOLUTION_Y - 90) : 440)

            static char *back_string = _("back");

            menu_item_start();
            menu_item_draw(MENU_X_START_L, MENU_Y_START, back_string, MENU_LEFT, FALSE, LEVEL_EVENT(GC_MENU_START));
          }

          p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
          p_grf->flip();

          /* pokud jsem na konci -> odregistrovat timer, pridat <- back menu
          */
          position += 2;
        }
        frame++;
      }    
      break;

    case MENU_LEAVE:
      p_text = NULL;
      menu_timer.clear();
      input.keyset_set(NULL);
      input.mevent_clear();
      break;
    
    default:
      break;
  }
}

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF

void game_gui::menu_in_game(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_in_game, data, data1);
    
        p_grf->fill(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y, 0);
        
        tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);

        #define LOGO_START (DOUBLE_SIZE ? 60 : 0)

        if(DOUBLE_SIZE) {
          p_grf->draw(menu_background_get(),0,0);
        }

        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);

        #define MENU_X_START (GAME_RESOLUTION_X/2 - 120)
        #define MENU_Y_START (GAME_RESOLUTION_Y/2 - 60)
        #define MENU_X_DIFF  0
        #define MENU_Y_DIFF  (DOUBLE_SIZE ? 45 : 35)

        static char *back    = _("return to game (ESC)");
        static char *rest    = _("restart level (CTRL+R)");
      
        static char *save    = _("save game (F2)");
        static char *load    = _("load game (F3)");
      
        static char *hint    = _("level hint (CTRL+F1)");
        static char *help    = _("help (F1)");
        
        static char *menu    = _("back to menu");
        static char *quit    = _("quit (CTRL+X)");
      
        menu_item_set_pos(MENU_X_START, MENU_Y_START);
        menu_item_set_diff(MENU_X_DIFF, MENU_Y_DIFF);

        menu_item_start();
        
        menu_item_draw(back, MENU_LEFT, FALSE, LEVEL_EVENT(GC_RESTORE_LEVEL));
        menu_item_draw(rest, MENU_LEFT, FALSE, LEVEL_EVENT(GC_RESTART_LEVEL));
        
        menu_item_draw(save, MENU_LEFT, FALSE, LEVEL_EVENT(GC_SAVE_LEVEL));
        menu_item_draw(load, MENU_LEFT, FALSE, LEVEL_EVENT(GC_LOAD_LEVEL));
        
        menu_item_draw(hint, MENU_LEFT,  MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
        menu_item_draw(help, MENU_LEFT,  MENU_SAVE_BACK, LEVEL_EVENT(GC_MENU_HELP, FALSE));
        
        menu_item_draw(menu, MENU_LEFT, FALSE, LEVEL_EVENT(GC_STOP_LEVEL, FALSE, FALSE),
                                               LEVEL_EVENT(GC_MENU_RUN_LEVEL, profile.level_set_get()));
        menu_item_draw(quit, MENU_LEFT, FALSE, LEVEL_EVENT(GC_STOP_LEVEL, FALSE, FALSE),
                                               LEVEL_EVENT(GC_MENU_QUIT));

        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();
      }
      break;
    
    case MENU_LEAVE:
      input.mevent_clear();
      break;
    default:
      break;
  }
}

void game_gui::level_suspend(LEVEL_EVENT_QUEUE *p_queue)
{
  input.keyset_set(&suspend_keys);
  p_ber->level_suspend(p_queue);
}

void game_gui::level_restore(LEVEL_EVENT_QUEUE *p_queue)
{
  menu_leave();

  input.keyset_set(&game_keys);
  p_ber->level_restore(p_queue);
}

void game_gui::level_restart(LEVEL_EVENT_QUEUE *p_queue)
{
  menu_leave();

  input.keyset_set(&game_keys);
  p_ber->level_restart(p_queue);
}

void game_gui::level_save(LEVEL_EVENT_QUEUE *p_queue)
{
  if(p_ber) {
    p_ber->level_save();
  
    p_font->select(FONT_DEFAULT);
    p_font->alignment_set(MENU_LEFT);
    p_grf->fill(0,460,GAME_RESOLUTION_X,20,0);
    p_font->print(NULL,0,460,_("Level saved..."));
    p_grf->redraw_add(0,460,GAME_RESOLUTION_X,20);
    p_grf->flip();    
    delay(500);
  }
}

void game_gui::level_load(LEVEL_EVENT_QUEUE *p_queue)
{
  if(p_ber) {
    p_ber->level_load();
  
    p_font->select(FONT_DEFAULT);
    p_font->alignment_set(MENU_LEFT);
    p_grf->fill(0,460,GAME_RESOLUTION_X,20,0);
    p_font->print(NULL,0,460,_("Level loaded..."));
    p_grf->redraw_add(0,460,GAME_RESOLUTION_X,20);
    p_grf->flip();
    delay(500);
  }
}

void game_gui::run_editor(void)
{ 
#ifdef LINUX
  int pid = fork();
  if(!pid) {
    bprintf("%s -e",p_dir->game_binary_get());
    int ret = execlp(p_dir->game_binary_get(),p_dir->game_binary_get(),"-e",NULL);
    if(ret == -1) {
      bprintf("Error: %s",strerror(errno));
    }
  }
  else {
    int status;
    bprintf("Waiting for %d",pid);
    waitpid(pid,&status,0);
    bprintf("Pid %d done",pid);
  }
#elif WINDOWS
  bprintf("%s -e",p_dir->game_binary_get());  
  int ret = _spawnl( _P_WAIT, p_dir->game_binary_get(),p_dir->game_binary_get(),"-e",NULL);
  if(ret == -1) {
    bprintf("Error: %s",strerror(errno));
  }
#endif
}

bool game_gui::callback(LEVEL_EVENT_QUEUE *p_queue, int frame)
{  
  /* If there's a timer function, call it! */
  if(menu_timer.valid()) {
    (this->*menu_timer.p_func)(MENU_TIMER, menu_timer.p1, menu_timer.p2);
  }

  /* load events from system by SDL event handler */
  input.events_loop(p_queue);
  
  /* Scan the queue and do the propper action */
  static LEVEL_EVENT_QUEUE tmp_queue;

#if EVENTS_DEBUG
  p_queue->dump_read("Read - game_gui::callback() loop start");
#endif

  while(!p_queue->empty()) {
    LEVEL_EVENT ev = p_queue->get();
      
    switch(ev.action_get()) {
      /* Start of the game
       * This is the firts event after start
       */
      case GC_MENU_DOUBLESIZE_QUESTION:
        menu_double_size_question(MENU_ENTER);
        break;
      case GC_MENU_DOUBLESIZE_SWITCH:
        menu_double_size_question_switch();
        break;
      case GC_MENU_DOUBLESIZE_SET:
        menu_double_size_set(ev.param_int_get(PARAM_0));
        break;
      case GC_MENU_START:
        menu_main(MENU_ENTER);
        break;      
      case GC_MENU_NEW_GAME:
        menu_new_game(MENU_ENTER);
        break;      
      case GC_MENU_PROFILES:
        menu_profiles(MENU_ENTER);
        break;
      case GC_MENU_PROFILE_CREATE:
        menu_profile_create(ev.param_int_get(PARAM_0));
        break;
      case GC_MENU_PROFILE_SELECT:        
        menu_profile_select(ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1));
        break;
      case GC_MENU_SETTINGS:
        menu_settings(MENU_ENTER, ev.param_int_get(PARAM_0));
        break;
      case GC_MENU_SETTINGS_FULSCREEN_SWITCH:
        menu_settings_fullscreen();
        break;
      case GC_MENU_SETTINGS_DOUBLESIZE_SWITCH:
        menu_settings_doublesize();
        break;
      case GC_MENU_SETTINGS_SOUND_SWITCH:
        //p_ber->sound.sound_on = !p_ber->sound.sound_on;
        break;
      case GC_MENU_SETTINGS_MUSIC_SWITCH:
        //p_ber->sound.music_on = !p_ber->sound.music_on;
        break;
      case GC_MENU_HELP:
        menu_help(MENU_ENTER, ev.param_int_get(PARAM_0));
        break;
      case GC_MENU_QUIT:        
        return(FALSE);
        break;
      case GC_MENU_HELP_KEYS:
        menu_help_keys(MENU_ENTER, ev.param_int_get(PARAM_0));
        break;
      case GC_MENU_HELP_RULES:
        menu_help_rules(MENU_ENTER, ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1));
        break;
      case GC_MENU_HELP_CREDIT:
        menu_help_credits(MENU_ENTER, ev.param_int_get(PARAM_0));
        break;
      case GC_MENU_LEVEL_HINT:
        menu_level_hint(MENU_ENTER, ev.param_int_get(PARAM_0));
        break;
      
      case GC_RUN_LEVEL_SELECT:
        level_select(ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1), ev.param_int_get(PARAM_2));
        menu_level_name_print();
        break;
      case GC_RUN_LEVEL_LINE:
        level_run(&tmp_queue, (char *)ev.param_point_get(PARAM_0));
        break;
      case GC_RUN_LEVEL_SET:
        level_run(&tmp_queue);
        break;
      case GC_STOP_LEVEL:
        level_stop(&tmp_queue, ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1));
        break;
      
      case GC_SUSPEND_LEVEL:
        level_suspend(&tmp_queue);
        break;
      case GC_RESTORE_LEVEL:
        level_restore(&tmp_queue);
        break;
      
      case GC_RESTART_LEVEL:
        level_restart(&tmp_queue);
        break;
      
      case GC_SAVE_LEVEL:
        level_save(&tmp_queue);
        break;
      case GC_LOAD_LEVEL:        
        level_load(&tmp_queue);
        break;
     
      case GC_MENU_RUN_LEVEL:
        menu_level_run_new(MENU_ENTER, ev.param_int_get(PARAM_0));
        break;
      case GC_MENU_END_LEVEL:
        menu_level_end(MENU_ENTER);
        break;
      case GC_MENU_END_LEVEL_CUSTOM:
        menu_level_end_custom(MENU_ENTER);
        break;
      case GC_MENU_END_LEVEL_SET:
        menu_levelset_end(MENU_ENTER);
        break;
      
      case GC_MENU_IN_GAME:
        menu_in_game(MENU_ENTER);
        break;

      case GC_GAME_DATA_LOAD:
        p_ber->game_data_load();
        break;
      
      case GC_RUN_EDITOR:
        run_editor();
        break;

      case GI_MENU_BACK_POP:
        back_pop();
        break;
      case GI_MENU_BACK_PUSH:
        back_push();
        break;
      
      case GI_SPRITE_DRAW:
      case GI_STRING_DRAW:
      case GI_CHECKBOX_SWITCH:
      case GI_HIGHLIGHT_EVENT:
      case GI_KEY_DOWN:
        menu_services(p_queue, &tmp_queue, ev);
        break;
            
      default:
        // Copy only valid events
        if(ev.valid()) {
          tmp_queue.add(ev);
          tmp_queue.commit();
        }
        break;
    }
  }

  /* Commit all writes */
  tmp_queue.commit();
  p_queue->add(&tmp_queue);
  p_queue->commit();  
  tmp_queue.clear();

#if EVENTS_DEBUG
  p_queue->dump_read("Read - game_gui::callback() end");
#endif

  /* Call the game (if exists) */
  if(p_ber) {
    p_ber->level_callback(p_queue);
  }

  return(TRUE);
}

void game_gui::menu_dialog_error(char *p_text,...)
{
/*
  #define MAX_TEXT_LEN 2000

  char      text[MAX_TEXT_LEN];
  va_list   arguments;

  va_start(arguments,p_text);
  vsnprintf(text,MAX_TEXT_LEN,p_text,arguments);
  va_end(arguments);

  GtkWidget *dialog = gtk_message_dialog_new(main_window_get(),
                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                GTK_MESSAGE_ERROR,
                                GTK_BUTTONS_CLOSE,
                                text);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
*/  
}
