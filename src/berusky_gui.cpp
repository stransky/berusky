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
}

game_gui::~game_gui(void)
{
  if(p_ber)
    delete p_ber;
   
  // TODO -> dat to tam kde se to SDL inicializuje
  SDL_Quit();
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
                  
        #define LOGO_START 0
      
        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);

        #define MENU_X_START 250
        #define MENU_Y_START 190
        #define MENU_X_DIFF  0
        #define MENU_Y_DIFF  35

        static char *new_game = _("new game");
        static char *password = _("password");
        static char *settings = _("settings");
        static char *help = _("help");
        static char *editor = _("editor");
        static char *quit = _("quit");
              
        p_font->select(FONT_DEFAULT);

        menu_item_set_pos(MENU_X_START, MENU_Y_START);
        menu_item_set_diff(MENU_X_DIFF, MENU_Y_DIFF);

        menu_item_start();
        
        menu_item_draw(new_game, LEFT, TRUE, LEVEL_EVENT(GC_MENU_NEW_GAME));
        menu_item_draw(password, LEFT, TRUE, LEVEL_EVENT(GC_MENU_PASSWORD));
        menu_item_draw(settings, LEFT, TRUE, LEVEL_EVENT(GC_MENU_SETTINGS));
        menu_item_draw(help, LEFT, TRUE, LEVEL_EVENT(GC_MENU_HELP,FALSE));
        menu_item_draw(editor, LEFT, TRUE, LEVEL_EVENT(GC_RUN_EDITOR));      
        menu_item_draw(quit, LEFT, TRUE, LEVEL_EVENT(GC_MENU_QUIT));
      
        p_font->alignment_set(CENTER);
        p_font->start_set(0, GAME_RESOLUTION_Y - 60);
        p_font->print(_("berusky (C) Anakreon 1997-2007\n"));
        p_font->print(_("distributed under GPL\n"));
        
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

        #define LOGO_START 0
      
        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);
      
        #define MENU_X_START 250
        #define MENU_Y_START 240
        #define MENU_X_DIFF  0
        #define MENU_Y_DIFF  35
      
        p_font->select(FONT_DEFAULT);
        p_font->alignment_set(CENTER);
        p_font->start_set(0, MENU_Y_START - 50);
        p_font->print(_("select difficulty of the new game:"));
      
        static char *training      = _("training");
        static char *easy          = _("easy");
        static char *intermediate  = _("intermediate");
        static char *advanced      = _("advanced");
        static char *impossible    = _("impossible");
        static char *back          = _("back");
      
        menu_item_set_pos(MENU_X_START, MENU_Y_START);
        menu_item_set_diff(MENU_X_DIFF, MENU_Y_DIFF);
        
        // udelat na to ulozeni tady flag -> prejdi a uloz!! (Zprava -> menu_back_save)
        menu_item_start();
        menu_item_draw(training, LEFT, TRUE, LEVEL_EVENT(GC_MENU_RUN_LEVEL, 0, 0));
        menu_item_draw(easy, LEFT, TRUE, LEVEL_EVENT(GC_MENU_RUN_LEVEL, 1, 0));
        menu_item_draw(intermediate, LEFT, TRUE, LEVEL_EVENT(GC_MENU_RUN_LEVEL, 2, 0));
        menu_item_draw(advanced, LEFT, TRUE, LEVEL_EVENT(GC_MENU_RUN_LEVEL, 3, 0));
        menu_item_draw(impossible, LEFT, TRUE, LEVEL_EVENT(GC_MENU_RUN_LEVEL, 4, 0));
        menu_item_draw(back, LEFT, FALSE, LEVEL_EVENT(GI_MENU_BACK_POP));
              
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

void game_gui::menu_password(MENU_STATE state, size_ptr data, size_ptr data1)
{
  #define MAX_PASSWORD 10
  static char password[MAX_PASSWORD] = "";

  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_password, data, data1);
    
        p_grf->fill(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y, 0);
        
        tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);        

        #define LOGO_START 0
      
        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);
      
        #define INSERT_START 200
        p_font->select(FONT_DEFAULT);
        p_font->alignment_set(CENTER);
        p_font->start_set(0, INSERT_START);
        p_font->print(_("please insert a password to level:"));
        password[0] = '\0';
        p_font->print(NULL, 0, INSERT_START+50, "_");

        #define MENU_X_START 270
        #define MENU_Y_START 400
        #define MENU_X_DIFF  90
        #define MENU_Y_DIFF  35

        static char *play = _("play");
        static char *back = _("back");
              
        menu_item_start();
        menu_item_draw(MENU_X_START + MENU_X_DIFF, MENU_Y_START, play, RIGHT, TRUE, LEVEL_EVENT(GC_MENU_PASSWORD_CHECK, password));
        menu_item_draw(MENU_X_START, MENU_Y_START+MENU_Y_DIFF, back, LEFT, FALSE, LEVEL_EVENT(GI_MENU_BACK_POP));
              
        p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
        p_grf->flip();
        
        menu_key_input.set((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_password);
      }
      break;
    case MENU_KEY_INPUT:
      {
        char ascii_char;
        bool character = key_to_ascii(data,&ascii_char);
        int  len = strlen(password);
      
        if(character && ascii_char >= ' ') {
          if(len < MAX_PASSWORD-2) {
            password[len] = ascii_char;
            password[len+1] = '\0';
          }        
        } else {
          switch(data) {
            case K_BKSP:
            case K_DEL:
              if(len > 0) {
                password[--len] = '\0';
              }
              break;
            case K_ENTER:
              break;
            default:
              break;
          }
        }
      
        p_font->select(FONT_DEFAULT);
        p_font->alignment_set(CENTER);        
        p_grf->fill(0,INSERT_START+50,GAME_RESOLUTION_X,20,0);
        p_font->print(NULL, 0, INSERT_START+50, "%s_", password);
        p_grf->redraw_add(0,INSERT_START+50,GAME_RESOLUTION_X,20);
        p_grf->flip();
      }
      break;
    case MENU_LEAVE:    
      input.mevent_clear();
      menu_key_input.clear();
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

void game_gui::menu_password_check(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_password_check, data, data1);
              
        int set, level;
      
        if(p_ber->levelset_search((char *)data, &set, &level)) {
          menu_level_run(MENU_ENTER,set,level);
        } else {
          p_grf->fill(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y, 0);
          
          tpos width = p_grf->sprite_get_width(MENU_SPRIT_LOGO);
  
          #define LOGO_START 0
        
          p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);
        
          #define INSERT_START 200
          p_font->select(FONT_DEFAULT);
          p_font->alignment_set(CENTER);
          p_font->start_set(0, INSERT_START);
          p_font->print(_("Cha cha cha!!!\n"));
          p_font->print(_("Would you like to cheat?\n"));
          p_font->print(_("Try me again!"));
        
  
          #define MENU_X_START 270
          #define MENU_Y_START 400
          #define MENU_X_DIFF  90
          #define MENU_Y_DIFF  35
  
          static char *back = _("back");
          
          menu_item_start();
          menu_item_draw(MENU_X_START, MENU_Y_START+MENU_Y_DIFF, back, LEFT, FALSE, LEVEL_EVENT(GI_MENU_BACK_POP));
                
          p_grf->redraw_add(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
          p_grf->flip();                  
        }
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

        #define LOGO_START 0
      
        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);

        #define MENU_X_START 240
        #define MENU_Y_START 240
        #define MENU_X_DIFF  0
        #define MENU_Y_DIFF  35
      
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
          menu_item_draw(hint, LEFT, TRUE, LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
        }
        
        menu_item_draw(keys, LEFT, TRUE, LEVEL_EVENT(GC_MENU_HELP_KEYS));
        menu_item_draw(rules, LEFT, TRUE, LEVEL_EVENT(GC_MENU_HELP_RULES,0));
        menu_item_draw(credits, LEFT, TRUE, LEVEL_EVENT(GC_MENU_HELP_CREDIT));
        menu_item_draw(back, LEFT, FALSE, LEVEL_EVENT(from_game ? GC_RESTORE_LEVEL : GI_MENU_BACK_POP));

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

        #define LOGO_START 0
      
        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);

        #define MENU_X_START 100
        #define MENU_Y_START 180
        #define MENU_X_DIFF  0
        #define MENU_Y_DIFF  35
      
        bool from_game = (bool)data;
      
        static char *fulscreen = _("fulscreen");
/*
        static char *sound = _("sound");
        static char *music = _("music");
*/
        menu_item_set_pos(MENU_X_START, MENU_Y_START);
        menu_item_set_diff(MENU_X_DIFF, MENU_Y_DIFF);

        menu_item_start();
        menu_item_draw_checkbox(fulscreen, LEFT, p_grf->fullscreen_get(), 0,
                                LEVEL_EVENT(GC_MENU_SETTINGS_FULSCREEN_SWITCH));
/*
        menu_item_draw_checkbox(sound, LEFT, p_ber->sound.sound_on, 1,
                                LEVEL_EVENT(GC_MENU_SETTINGS_SOUND_SWITCH));
        menu_item_draw_checkbox(music, LEFT, p_ber->sound.music_on, 2,
                                LEVEL_EVENT(GC_MENU_SETTINGS_MUSIC_SWITCH));
*/
        static char *back = _("back");
        
        #define MENU_X_START_BACK 270
        #define MENU_Y_START_BACK 400
        
        menu_item_set_pos(MENU_X_START_BACK, MENU_Y_START_BACK);
        menu_item_draw(back, LEFT, FALSE, LEVEL_EVENT(from_game ? GC_RESTORE_LEVEL : GI_MENU_BACK_POP));

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

/*
*/

#undef LOGO_START
#undef MENU_X_START
#undef MENU_Y_START
#undef MENU_X_DIFF
#undef MENU_Y_DIFF

void game_gui::menu_help_rules(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {      
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_help_rules, data, data1);
      
        p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
        p_grf->draw(MENU_SPRIT_BACK,0,0);
        p_font->alignment_set(LEFT);
        p_font->select(FONT_DEFAULT);
              
        int j, i, x, y;
        int variants;
        int page = data;

        int page_prev = page-1,
            page_next = page+1;
          
        switch(page) {
          case 0:  
            p_font->print(NULL, 80, 95, _("Basic rules and game elements"));
            
            p_font->start_set(20, 140);
            p_font->print(_("In order to leave each level it is\n\
necessary to own five keys and also\n\
to have a free way to the exit.\n\n\
You will be meeting miscellaneous game\n\
elements while completing individual\n\
missions, we will try to explain their\n\
meaning now.\n"));

            p_font->print(NULL, 20, 310, _("box - it is possible to push it."));
            x = 20;
            y = 340;
            variants = p_repo->variants_get(P_BOX);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_BOX, i, 0);
              x += 30;
            }
            
            p_font->print(NULL, 20, 380, _("explosive - can destroy the boxes."));
            x = 20;
            y = 410;
            variants = p_repo->variants_get(P_TNT);
            for (i = 0; i < variants; i++) {                
              p_repo->draw(x, y, P_TNT, i, 0);
              x += 30;
            }
          
            page_prev = page;
          
            break;
          
          case 1:
            p_font->print(NULL, 160, 95, _("Active game elements"));

            p_font->print(NULL, 20, 140, _("key - you need five of them."));
            p_grf->draw(p_repo->sprite_get(P_KEY, 0, 0), 20, 165);
            
            p_font->print(NULL, 20, 200, _("exit - a gate to next level."));
            x = 20;
            y = 230;
            variants = p_repo->variants_get(P_EXIT);
            for (i = 0; i < variants - 1; i += 2) {
              p_repo->draw(x, y, P_EXIT, i, 0);
              x += 30;
            }
            p_grf->draw(p_repo->sprite_get(P_EXIT, i - 1, 0), x, y);
          
            p_font->print(NULL, 20, 270, _("stone - can be broken by a pickax."));
            x = 20;
            y = 300;
            variants = p_repo->variants_get(P_STONE);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_STONE, i, 0);
              x += 30;
            }
          
            p_font->print(NULL, 20, 340, _("pickax - a tool for stone crushing."));
            x = 20;
            y = 370;
            variants = p_repo->variants_get(P_MATTOCK);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_MATTOCK, i, 0);
              x += 30;
            }
                              
            break;
      
          case 2:
            p_font->print(NULL, 160, 95,  _("Active game elements"));
          
            p_font->start_set(20, 140);
            p_font->print(_("color key - used to unlock color door,\n\
only a bug with identical color can\n\
pick them up"));
          
            x = 20;
            y = 210;
            for (j = 0; j < 5; j++) {
              variants = p_repo->variants_get(P_KEY1 + j);
              for (i = 0; i < variants; i++) {
                p_repo->draw(x, y, P_KEY1 + j, i, 0);
                x += 30;
              }
            }
          
            p_font->start_set(20, 250);
            p_font->print(_("color door - can be opened by the\nrespective color key only"));
          
            x = 40;
            y = 300;
            variants = p_repo->variants_get(P_DOOR1_V_Z);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_DOOR1_V_Z, i, 0);
              x += 70;
            }
          
            p_font->start_set(20, 340);
            p_font->print(_("color gate-way - only a bug with\n\
identical color is allowed to go\n\
through. Boxes cannot be pushed\n\
through."));
          
            x = 40;
            y = 430;
            variants = p_repo->variants_get(P_ID_DOOR1_V_Z);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_ID_DOOR1_V_Z, i, 0);
              x += 70;
            }
                    
            break;
      
          case 3:
            p_font->print(NULL, 160, 95,  _("Active game elements"));

            p_font->start_set(20, 140);
            p_font->print(_("one-pass door - can be used only once,\n\
then it is closed off and there's no\n\
way to open it\n"));

            x = 40;
            y = 220;
            variants = p_repo->variants_get(P_DV_V_O);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_DV_V_O, i, 0);        
              x += 70;
            }
            x = 40;
            y = 260;
            variants = p_repo->variants_get(P_DV_V_Z);
            for (i = 0; i < variants; i++) {
              p_repo->draw(x, y, P_DV_V_Z, i, 0);        
              x += 70;
            }

            p_font->start_set(20, 300);
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
      
        #define MENU_X_START    350
        #define MENU_X_START_L  MENU_X_START
        #define MENU_X_START_R  MENU_X_START_L + 250
        #define MENU_X_START_B  MENU_X_START_L + 50
        #define MENU_Y_START    410
        #define MENU_X_DIFF     0
        #define MENU_Y_DIFF     30
      
        menu_item_start();
        menu_item_draw(MENU_X_START_L, MENU_Y_START+0*MENU_Y_DIFF, prev, LEFT, FALSE, LEVEL_EVENT(GC_MENU_HELP_RULES,page_prev));
        menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, next, RIGHT, FALSE, LEVEL_EVENT(GC_MENU_HELP_RULES,page_next));
        menu_item_draw(MENU_X_START_B, MENU_Y_START+1*MENU_Y_DIFF, back, LEFT, FALSE, LEVEL_EVENT(GI_MENU_BACK_POP));
        
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

void game_gui::menu_help_keys(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_help_keys, data, data1);
      
        p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
        p_grf->draw(MENU_SPRIT_BACK,0,0);
        p_font->select(FONT_DEFAULT);
        
        p_font->alignment_set(CENTER);
        p_font->print(NULL, 220, 100, _("Game Controls"));
      
        p_font->alignment_set(LEFT);
        p_font->print(NULL, 20,  140, _("Up to five bugs are available,\n\
which can be controlled by these keys:"));
      
        #define Y_DIFF  25
        #define Y_START 200
        int start_y;
      
        p_font->print(NULL, 20,  start_y = Y_START, _("arrows"));
        p_font->print(NULL, 20,  start_y += Y_DIFF, _("SHIFT+arrows"));
        
        p_font->print(NULL, 20,  start_y += Y_DIFF, _("Tab"));
        p_font->print(NULL, 20,  start_y += Y_DIFF, _("N"));
        p_font->print(NULL, 20,  start_y += Y_DIFF, _("D"));
        p_font->print(NULL, 20,  start_y += Y_DIFF, _("CTRL+X"));
        p_font->print(NULL, 20,  start_y += Y_DIFF, _("F1"));
        p_font->print(NULL, 20,  start_y += Y_DIFF, _("F2"));
        p_font->print(NULL, 20,  start_y += Y_DIFF, _("F3"));
      
        p_font->print(NULL, 135,  start_y = Y_START, _(". . . . . move the bug"));        
        p_font->print(NULL, 205,  start_y += Y_DIFF, _(". . quick bug movement"));
        p_font->print(NULL, 86,  start_y += Y_DIFF, _(". . . . . . . switch among the bugs"));
        p_font->print(NULL, 60,  start_y += Y_DIFF, _(". . . . . . . . change the music"));
        p_font->print(NULL, 60,  start_y += Y_DIFF, _(". . . . . . . . demo"));
        p_font->print(NULL, 110, start_y += Y_DIFF, _(". . . . . . quit quickly"));
        p_font->print(NULL, 60,  start_y += Y_DIFF, _(". . . . . . . . Help"));
        p_font->print(NULL, 60,  start_y += Y_DIFF, _(". . . . . . . . Save level"));
        p_font->print(NULL, 60,  start_y += Y_DIFF, _(". . . . . . . . Load level"));

        #define MENU_X_START 270
        #define MENU_Y_START 400
        #define MENU_X_DIFF  90
        #define MENU_Y_DIFF  35

        static char *back = _("back");
      
        menu_item_start();
        menu_item_draw(MENU_X_START, MENU_Y_START+MENU_Y_DIFF, back, LEFT, FALSE, LEVEL_EVENT(GI_MENU_BACK_POP));
        
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
#define SCROLL_START_Y  180
#define SCROLL_LINES    12

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
        #define LOGO_START 0      
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
        
          p_font->alignment_set(CENTER);
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
          
            #define MENU_X_START_L (320 - 17)
            #define MENU_Y_START    440
                        
            static char *back_string = _("back");

            menu_item_start();
            menu_item_draw(MENU_X_START_L, MENU_Y_START, back_string, LEFT, FALSE, LEVEL_EVENT(GC_MENU_START));
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
        p_grf->draw(MENU_SPRIT_BACK,0,0);

        p_font->select(FONT_DEFAULT);
        p_font->alignment_set(LEFT);

        char *p_hint = NULL;
        bool from_game = data;
      
        if(!p_ber->levelset_is_custom()) {
          int set = p_ber->levelset_get_set();
          int level = p_ber->levelset_get_level();
        
          // ... Load & print hint for level ...
          p_hint = level_hint_load(set, level);
        }
        
        if(p_hint) {
          p_font->print(NULL, 20, 100, _("Level hint:"));
        } else {
          p_hint = _("\nSorry dude, no hint available for this\nlevel.");
        }
        p_font->print(NULL, 20, 120, p_hint);
      
        #define MENU_X_START 270
        #define MENU_Y_START 400
        #define MENU_X_DIFF  90
        #define MENU_Y_DIFF  35

        static char *back = _("back");
      
        menu_item_start();
        menu_item_draw(MENU_X_START, MENU_Y_START+MENU_Y_DIFF, back, LEFT, FALSE, LEVEL_EVENT(from_game ? GC_RESTORE_LEVEL : GI_MENU_BACK_POP));
        
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

void game_gui::menu_level_run(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {            
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_level_run, data, data1);

        int set = data;
        int level = data1;
      
        bool ret = p_ber->levelset_load(set);
        assert(ret);
        p_ber->levelset_set_level(level);
      
        p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
        p_font->alignment_set(CENTER);
        p_font->select(FONT_DEFAULT);

        #define TEXT_START 250
      
        switch(set) {
          case 0:
            p_grf->draw(MENU_SPRIT_START+0,0,50);
            p_font->print(NULL, 0, 20,_("difficulty training"));
            p_font->print(NULL, 0, TEXT_START+35, _("Level: %d"),level);
            p_font->print(NULL, 0, TEXT_START+35+25, _("Password: %s"),p_ber->levelset_get_passwd(level));
            break;
          case 1:
            p_grf->draw(MENU_SPRIT_START+1,0,25);
            p_font->print(NULL, 0, 20, _("difficulty easy"));
            p_font->print(NULL, 0, TEXT_START+35, _("Level: %d"),level);
            p_font->print(NULL, 0, TEXT_START+35+25, _("Password: %s"),p_ber->levelset_get_passwd(level));
            break;
          case 2:
            p_grf->draw(MENU_SPRIT_START+2,0,50);
            p_font->print(NULL, 0, 20, _("difficulty intermediate"));
            p_font->print(NULL, 0, TEXT_START+35, _("Level: %d"),level);
            p_font->print(NULL, 0, TEXT_START+35+25, _("Password: %s"),p_ber->levelset_get_passwd(level));
            break;
          case 3:
            p_grf->draw(MENU_SPRIT_START+3,0,50);
            p_font->print(NULL, 0, 20, _("difficulty advanced"));
            p_font->print(NULL, 0, TEXT_START+35, _("Level: %d"),level);
            p_font->print(NULL, 0, TEXT_START+35+25, _("Password: %s"),p_ber->levelset_get_passwd(level));
            break;
          case 4:
            p_grf->draw(MENU_SPRIT_START+3,0,50);
            p_font->print(NULL, 0, 20, _("difficulty impossible"));
            p_font->print(NULL, 0, TEXT_START+35, _("Level: %d"),level);
            p_font->print(NULL, 0, TEXT_START+35+25, _("Password: %s"),p_ber->levelset_get_passwd(level));
            break;
          default:
            assert(0);
            break;
        }
      
        #define MENU_X_START_L (320 - 17 - 60)
        #define MENU_X_START_R (320 + 60)
        #define MENU_Y_START    360
        #define MENU_X_DIFF     0
        #define MENU_Y_DIFF     35
        
        static char *play_string = _("play level");
        static char *level_hint  = _("level hint");
        static char *back_string = _("back");
      
        menu_item_start();
        menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, play_string, RIGHT, FALSE, LEVEL_EVENT(GC_RUN_LEVEL_SET, set, level));
        menu_item_draw(MENU_X_START_R, MENU_Y_START+1*MENU_Y_DIFF, level_hint, RIGHT, TRUE, LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
        menu_item_draw(MENU_X_START_L, MENU_Y_START+2*MENU_Y_DIFF, back_string, LEFT, FALSE, LEVEL_EVENT(GI_MENU_BACK_POP));

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
bool game_gui::level_run(LEVEL_EVENT_QUEUE *p_queue, int set, int level)
{
  assert(p_ber);

  menu_leave();

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
        if(level+1 < p_ber->levelset_get_levelnum()) {
          p_queue->add(LEVEL_EVENT(GC_MENU_END_LEVEL, set, level));
        } else {        
          p_queue->add(LEVEL_EVENT(GC_MENU_END_LEVEL_SET, set));
        }    
      }
    } else {
      if(p_ber->levelset_is_custom()) {
        /* if we run it from command line, stop it */
        p_queue->add(LEVEL_EVENT(GC_MENU_QUIT));
      } else {
        /* if we run it from set, return to menu */
        p_queue->add(LEVEL_EVENT(GC_MENU_END_LEVEL, set, level));
      }
    }
  }
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
        p_font->alignment_set(CENTER);
        p_font->select(FONT_DEFAULT);

        LEVEL_STATUS *p_status = p_ber->level_status_get();
        char tmp[100];

        #define SMALL_LOGO_START 80
        p_grf->draw(MENU_SPRIT_LOGO_SMALL_2,p_grf->sprite_get_width_center(MENU_SPRIT_LOGO_SMALL_2),SMALL_LOGO_START);
      
        if(p_status->resolved()) {
          p_font->print(NULL,0,SMALL_LOGO_START+100,_("your bugs have survived!"));
          p_font->print(NULL,0,SMALL_LOGO_START+130,_("difficulty %s"), p_ber->levelset_get_difficulty());
          p_font->print(NULL,0,SMALL_LOGO_START+180,_("it takes %d steps"), p_status->steps_get());
          p_font->print(NULL,0,SMALL_LOGO_START+210,_("and %s."), p_status->time_get(tmp,100));
        } else {
          p_font->print(NULL,0,SMALL_LOGO_START+100,_("your bugs have given it up!"));
          p_font->print(NULL,0,SMALL_LOGO_START+130,_("difficulty %s"), p_ber->levelset_get_difficulty());
          p_font->print(NULL,0,SMALL_LOGO_START+180,_("they made %d steps"), p_status->steps_get());
          p_font->print(NULL,0,SMALL_LOGO_START+210,_("and spent %s"), p_status->time_get(tmp,100));
        }

        p_grf->draw(MENU_SPRIT_LOGO_SMALL_3,p_grf->sprite_get_width_center(MENU_SPRIT_LOGO_SMALL_3),SMALL_LOGO_START+240);
        
        #define MENU_X_START_L (320 - 17 - 120)
        #define MENU_X_START_R (320 + 120)
        #define MENU_Y_START    400
        #define MENU_X_DIFF     0
        #define MENU_Y_DIFF     30
        
        static char *play_string = _("play next level");
        static char *back_string = _("back to menu");
      
        menu_item_start();
      
        if(p_status->resolved()) {
          int set = data;
          int level = data1;
          menu_item_draw(MENU_X_START_R, MENU_Y_START+0*MENU_Y_DIFF, play_string, RIGHT, FALSE, LEVEL_EVENT(GC_MENU_RUN_LEVEL, set, level+1));
        }
        
        menu_item_draw(MENU_X_START_L, MENU_Y_START+1*MENU_Y_DIFF, back_string, LEFT, FALSE, LEVEL_EVENT(GI_MENU_BACK_POP));

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

void game_gui::menu_level_end_custom(MENU_STATE state, size_ptr data, size_ptr data1)
{
  switch(state) {
    case MENU_RETURN:
    case MENU_ENTER:
      {
        menu_enter((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_level_end_custom, data, data1);

        /* Some statistics here */
        p_grf->fill(0,0,GAME_RESOLUTION_X,GAME_RESOLUTION_Y,0);
        p_font->alignment_set(CENTER);
        p_font->select(FONT_DEFAULT);
      
        #define SMALL_LOGO_START 80      
        p_grf->draw(MENU_SPRIT_LOGO_SMALL_2,p_grf->sprite_get_width_center(MENU_SPRIT_LOGO_SMALL_2),SMALL_LOGO_START);
              
        p_font->print(NULL,0,SMALL_LOGO_START+100,_("your bugs have survived!"));
        p_font->print(NULL,0,SMALL_LOGO_START+130,_("custom level %s."), p_ber->levelset_get_name());
      
        LEVEL_STATUS *p_status = p_ber->level_status_get();
        p_font->print(NULL,0,SMALL_LOGO_START+180,_("it takes %d steps"), p_status->steps_get());
        char tmp[100];
        p_font->print(NULL,0,SMALL_LOGO_START+210,_("and %s."), p_status->time_get(tmp,100));

        p_grf->draw(MENU_SPRIT_LOGO_SMALL_3,p_grf->sprite_get_width_center(MENU_SPRIT_LOGO_SMALL_3),SMALL_LOGO_START+240);      

        #define MENU_X_START   (320 - 35)
        #define MENU_Y_START    400
        #define MENU_X_DIFF     0
        #define MENU_Y_DIFF     30
                
        static char *back_string = _("quit");
                    
        menu_item_start();
        menu_item_draw(MENU_X_START, MENU_Y_START+0*MENU_Y_DIFF, back_string, LEFT, FALSE, LEVEL_EVENT(GC_MENU_QUIT));

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

#undef MENU_X_START_L
#undef MENU_Y_START
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
      
        sprite::color_key_set(COLOR_KEY_GAME);
        p_grf->sprite_delete(MENU_SPRIT_END, 1);
      
        char line[1000];
        snprintf(line, 1000, SET_END_FILE_BACK, set+1);
        p_grf->sprite_insert(line, MENU_SPRIT_END, 1, NULL);
      
        input.keyset_set(&menu_keys);
        
        menu_timer.set((GUI_BASE *)this,(GUI_BASE_FUNC)&game_gui::menu_levelset_end, data, data1);
        frame = 0;
        position = 0;
      }
      break;
    
    case MENU_TIMER:
      {
        if((frame & 0x3) == 0x0) {
          p_font->alignment_set(CENTER);
          p_font->select(FONT_DEFAULT);
                
          p_grf->draw(MENU_SPRIT_END,0,0);        
        
          if(position >= p_font->height_get_new_line(p_text)) {
            position -= p_font->height_get_new_line(p_text);
            p_text = strchr(p_text+1,'\n');
          }
        
          if(p_text) {
            #define SCROLL_START_X  0
            #define SCROLL_START_Y  0
            #define SCROLL_LINES    20
          
            p_font->print(NULL, SCROLL_START_X, SCROLL_START_Y-position, SCROLL_LINES, p_text);
          } else {
            menu_timer.clear();
          
            #define MENU_X_START_L (320 - 17)
            #define MENU_Y_START    440
                        
            static char *back_string = _("back");
          
            menu_item_start();
            menu_item_draw(MENU_X_START_L, MENU_Y_START, back_string, LEFT, FALSE, LEVEL_EVENT(GC_MENU_START));
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

        #define LOGO_START 0
      
        p_grf->draw(MENU_SPRIT_LOGO,(GAME_RESOLUTION_X-width)/2,LOGO_START);

        #define MENU_X_START 200
        #define MENU_Y_START 180
        #define MENU_X_DIFF  0
        #define MENU_Y_DIFF  35

        static char *back    = _("return to game (ESC)");
        static char *rest    = _("restart level (CTRL+R)");
      
        static char *save    = _("save game (F2)");
        static char *load    = _("load game (F3)");
      
        static char *hint    = _("level hint (CTRL+F1)");
        static char *help    = _("help (F1)");
        
        static char *menu    = _("back to main menu");
        static char *quit    = _("quit (CTRL+X)");
      
        menu_item_set_pos(MENU_X_START, MENU_Y_START);
        menu_item_set_diff(MENU_X_DIFF, MENU_Y_DIFF);

        menu_item_start();
        
        menu_item_draw(back, LEFT, FALSE, LEVEL_EVENT(GC_RESTORE_LEVEL));
        menu_item_draw(rest, LEFT, FALSE, LEVEL_EVENT(GC_RESTART_LEVEL));
        
        menu_item_draw(save, LEFT, FALSE, LEVEL_EVENT(GC_SAVE_LEVEL));
        menu_item_draw(load, LEFT, FALSE, LEVEL_EVENT(GC_LOAD_LEVEL));
        
        menu_item_draw(hint, LEFT,  TRUE, LEVEL_EVENT(GC_MENU_LEVEL_HINT, FALSE));
        menu_item_draw(help, LEFT,  TRUE, LEVEL_EVENT(GC_MENU_HELP, FALSE));
        
        menu_item_draw(menu, LEFT, FALSE, LEVEL_EVENT(GC_STOP_LEVEL, FALSE, FALSE), LEVEL_EVENT(GC_MENU_START));
        menu_item_draw(quit, LEFT, FALSE, LEVEL_EVENT(GC_STOP_LEVEL, FALSE, FALSE), LEVEL_EVENT(GC_MENU_QUIT));

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
    p_font->alignment_set(LEFT);
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
    p_font->alignment_set(LEFT);
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

  while(!p_queue->empty()) {
    LEVEL_EVENT ev = p_queue->get();
      
    switch(ev.action_get()) {
      /* Start of the game
       * This is the firts event after start
       */
      case GC_MENU_START:
        menu_main(MENU_ENTER);
        break;      
      case GC_MENU_NEW_GAME:
        menu_new_game(MENU_ENTER);
        break;      
      case GC_MENU_PASSWORD:
        menu_password(MENU_ENTER);
        break;
      case GC_MENU_PASSWORD_CHECK:
        menu_password_check(MENU_ENTER, ev.param_int_get(PARAM_0));
        break;
      case GC_MENU_SETTINGS:
        menu_settings(MENU_ENTER, ev.param_int_get(PARAM_0));
        break;
      case GC_MENU_SETTINGS_FULSCREEN_SWITCH:
        p_grf->fullscreen_toggle();
        break;
      case GC_MENU_SETTINGS_SOUND_SWITCH:
        p_ber->sound.sound_on = !p_ber->sound.sound_on;
        break;
      case GC_MENU_SETTINGS_MUSIC_SWITCH:
        p_ber->sound.music_on = !p_ber->sound.music_on;
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
      
      case GC_RUN_LEVEL_LINE:
        level_run(&tmp_queue, (char *)ev.param_point_get(PARAM_0));
        break;
      case GC_RUN_LEVEL_SET:
        level_run(&tmp_queue, ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1));
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
        menu_level_run(MENU_ENTER, ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1));
        break;
      case GC_MENU_END_LEVEL:
        menu_level_end(MENU_ENTER, ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1));
        break;
      case GC_MENU_END_LEVEL_CUSTOM:
        menu_level_end_custom(MENU_ENTER, ev.param_int_get(PARAM_0), ev.param_int_get(PARAM_1));
        break;
      case GC_MENU_END_LEVEL_SET:
        menu_levelset_end(MENU_ENTER, ev.param_int_get(PARAM_0));
        break;
      
      case GC_MENU_IN_GAME:
        menu_in_game(MENU_ENTER);
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
      case GI_KEY_DOWN:
        menu_services(ev);
        break;
            
      default:
        assert(ev.valid());
        tmp_queue.add(ev);
        tmp_queue.commit();
        break;
    }
  }

  /* Commit all writes */
  p_queue->add(&tmp_queue);
  p_queue->commit();  
  tmp_queue.clear();

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
