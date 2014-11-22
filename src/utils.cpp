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

/*
  Utility
*/
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "portability.h"

#ifdef LINUX
#include <dirent.h>
#include <fnmatch.h>
#endif
#ifdef WINDOWS
#include <direct.h>
#endif

#include "ini.h"
#include "utils.h"
#include "types.h"

#include "berusky.h"
#include "berusky_gui.h"
#include "main.h"
#include "editor.h"


FHANDLE log_file;

void log_close(void)
{
  if(log_file) {
    file_close(log_file);
    log_file = (void *)NULL;
  }
}

void log_open(const char *p_file)
{
  log_close();
 
  if(p_file) {
    log_file = file_open(NO_DIR,p_file,"a",FALSE);
    if(!log_file)
      berror("Unable to open log file '%s', logging is disabled.",p_file);
    bprintf("------------- start ----------------");
  }
}

void log_open_ini(const char *p_ini_file)
{
  char logfile[1000];
  #define INI_LOGFILE "logfile"
  #define INI_LOG     "log"

  if(ini_read_bool_file(p_ini_file, INI_LOG, FALSE)) {
    ini_read_string_file(p_ini_file, INI_LOGFILE, logfile, sizeof(logfile),"./b1logfile.txt");
    log_open(logfile);
  }
}

void log_flush(void)
{
  if(log_file)
    fflush(log_file);
}

void dir_list::update_path(char *p_dir)
{
  char buffer[MAX_FILENAME];
  return_path(p_dir, "", buffer, MAX_FILENAME);
  chdir(cwd);
  chdir(buffer);
  getcwd(p_dir,MAX_FILENAME);
}

void dir_list::load(const char *p_ini)
{
  #define INI_LEVEL       "level_data"
  #define INI_GAME        "game_data"
  #define INI_GRAPHICS    "graphics_data"
  #define INI_LEVEL_USER  "level_data_user"
  #define INI_BINARY      "game_binary"

  ini_read_string_file(p_ini, INI_LEVEL, levels, sizeof(levels), "./Lihen/Levels");
  ini_read_string_file(p_ini, INI_GAME, gamedata, sizeof(gamedata), "./Lihen/GameData");
  ini_read_string_file(p_ini, INI_GRAPHICS, graphics, sizeof(graphics), "./Lihen/Graphics");
  ini_read_string_file(p_ini, INI_BINARY, game_binary, sizeof(game_binary), "berusky");

#ifdef LINUX
  ini_read_string_file(p_ini, INI_LEVEL_USER, levels_user, sizeof(levels_user), "./Lihen/User");

  #define INI_TMP         "tmp_data"
  ini_read_string_file(p_ini, INI_TMP, tmp, sizeof(tmp), "/var/tmp");

  getcwd(cwd,MAX_FILENAME);
  update_path(levels);
  update_path(gamedata);
  update_path(graphics);  
  update_path(levels_user);
  update_path(tmp);
  chdir(cwd);
#endif

#ifdef WINDOWS
  strcpy(levels_user,DIRECTORY_GET(INI_USER_LEVELS));

  char path[PATH_MAX];
  strcpy(tmp,tmpfile_get(path));
  dir_create(tmp);
#endif
  
  bprintf("level_data: %s",levels);
  bprintf("game_data: %s",gamedata);
  bprintf("graphics_data: %s",graphics);
  bprintf("level_data_user: %s",levels_user);
  bprintf("tmp_data: %s",tmp);
  bprintf("current working dir: %s",cwd);
}

#define INI_FULLSCREEN "fullscreen"
bool get_fullscreen(const char *p_ini_file)
{  
  return(ini_read_int_file(p_ini_file, INI_FULLSCREEN, FALSE));
}

bool set_fullscreen(const char *p_ini_file, bool state)
{  
  char tmp[100];
  return(ini_write_string(p_ini_file, INI_FULLSCREEN, my_itoa(10, tmp, state ? 1 : 0)));
}

#define INI_DOUBLESIZE "disable_double_size"
bool get_doublesize(const char *p_ini_file)
{
  return(!ini_read_int_file(p_ini_file, INI_DOUBLESIZE, FALSE));
}

bool set_doublesize(const char *p_ini_file, bool state)
{
  char tmp[100];
  // it's reversed - disable_double_size = 0 is the default
  return(ini_write_string(p_ini_file, INI_DOUBLESIZE, my_itoa(10, tmp, !state ? 1 : 0)));
}

#define INI_DOUBLESIZE_QUESTION "startup_doublesize_question"
bool get_doublesize_question(const char *p_ini_file)
{
  return(ini_read_int_file(p_ini_file, INI_DOUBLESIZE_QUESTION, TRUE));
}

bool set_doublesize_question(const char *p_ini_file, bool state)
{
  char tmp[100];  
  return(ini_write_string(p_ini_file, INI_DOUBLESIZE_QUESTION, my_itoa(10, tmp, state ? 1 : 0)));
}

int  get_colors(const char *p_ini_file, int default_color_depth)
{ 
  #define INI_COLOR "color_depth"
  return(ini_read_int_file(p_ini_file, INI_COLOR, default_color_depth));
}

char * my_itoa(int base, char *buf, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;

/* If %d is specified and D is minus, put `-' in the head. */
  if (base == 'd' && d < 0) {
    *p++ = '-';
    buf++;
    ud = -d;
  }
  else if (base == 'x')
    divisor = 16;

  /* Divide UD by DIVISOR until UD == 0. */
  do {
    int remainder = ud % divisor;

    *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
  }
  while (ud /= divisor);

  /* Terminate BUF. */
  *p = 0;

  /* Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2) {
    char tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
    p1++;
    p2--;
  }
  return(buf);
}

/* Create a path */
char * return_path(const char *p_dir, const char *p_file, char *p_buffer, int max_lenght)
{
  if(p_dir) {
    if(p_dir[0] == '~') {
      dir_home_get(p_buffer,max_lenght);
      strncat(p_buffer,p_dir+1,max_lenght-strlen(p_buffer)-1);
    } else {      
      strncpy(p_buffer,p_dir,max_lenght-1);
    }
#ifdef LINUX
    strcat(p_buffer,"/");
#elif WINDOWS
    strcat(p_buffer,"\\");
#endif
    strncat(p_buffer,p_file,max_lenght-strlen(p_buffer));
  } else {
    if(p_file[0] == '~') {
      dir_home_get(p_buffer,max_lenght);
      strncat(p_buffer,p_file+1,max_lenght-strlen(p_buffer)-1);
    } else {
      strncpy(p_buffer,p_file,max_lenght-1);
    }
  }
  return(p_buffer);
}

/* Open a file */
FHANDLE file_open(const char * p_dir, const char * p_file, const char *p_mode, bool safe)
{
  FHANDLE tmp;

  char filename[MAX_FILENAME];

  tmp = fopen(return_path(p_dir, p_file, filename, MAX_FILENAME),p_mode);
  if(!tmp && safe) {
    char cwd[MAX_FILENAME];
    berror("Unable to open %s!\nError: %s\nCurrent dir: %s",
            filename,strerror(errno),getcwd(cwd,MAX_FILENAME));
  }

  return(tmp);
}

void file_close(FHANDLE f)
{
  if(f) {
    fclose(f);
  }
}

/* load file into memory */
int file_load(const char * p_dir, const char * p_file, char * p_mem, t_off max_lenght, t_off start_address, bool safe)
{
  return(file_load(file_open(p_dir, p_file, "rb", safe), p_mem, max_lenght, start_address));
}

int file_load_text(const char * p_dir, const char * p_file, char * p_mem, t_off max_lenght, t_off start_address, bool safe)
{  
  return(file_load_text(file_open(p_dir, p_file, "r", safe), p_mem, max_lenght, start_address));
}

/* load file into memory */
int file_load(FHANDLE f, char * p_mem, t_off max_lenght, t_off start_address)
{
  dword loaded = 0;

  if(f) {
    if(start_address)
      fseek(f, start_address, SEEK_SET);
    loaded = fread(p_mem, 1, max_lenght, f);
    fclose(f);
  }

  return (loaded);
}

/* load file into memory */
int file_load_text(FHANDLE f, char * p_mem, t_off max_lenght, t_off start_address)
{
  dword loaded = 0;

  if(f) {
    if(start_address)
      fseek(f, start_address, SEEK_SET);
    loaded = fread(p_mem, 1, max_lenght, f);
    p_mem[loaded] = '\0';
    fclose(f);
  }

  return (loaded);
}


/* load file into memory */
void * file_load(const char * p_dir, const char * p_file, t_off *p_lenght, t_off start_address, bool safe)
{  
  FHANDLE f = file_open(p_dir, p_file, "rb", safe);
  t_off to_load;

  if(!f)
    return(NULL);

  fseek(f, 0, SEEK_END);
  to_load = ftell(f) - start_address;

  if(start_address)
    fseek(f, start_address, SEEK_SET);

  void *p_mem = mmalloc(to_load);  
  *p_lenght = fread(p_mem, 1, to_load, f);
  fclose(f);
  
  return (p_mem);
}

/* save file from memory */
bool file_save(const char * p_dir, const char * p_file, void *p_buffer, t_off lenght, const char *p_mode)
{  
  FHANDLE f = file_open(p_dir, p_file, p_mode);
  t_off wrt = fwrite(p_buffer, 1, lenght, f);
  fclose(f);  
  return(wrt == lenght);
}

#define FILE_BUF_SIZE 4096
bool file_copy(FHANDLE f_in, FHANDLE f_out, int len)
{  
  char buffer[FILE_BUF_SIZE];
  int  readed;

  if(len) {
    int to_read;
    do {
      len -= FILE_BUF_SIZE;
      to_read = (len < 0) ? len+FILE_BUF_SIZE : FILE_BUF_SIZE;

      if((readed = fread(buffer,1,to_read,f_in))) {
        fwrite(buffer,1,readed,f_out);
      }

      if(readed != to_read) {
        berror("file_copy - missing data?");
      }
    } while(to_read == FILE_BUF_SIZE);
  }
  else {
    while((readed = fread(buffer,1,FILE_BUF_SIZE,f_in))) {
      fwrite(buffer,1,readed,f_out);
    }
  }

  return(TRUE);
}

bool file_copy(const char *p_src, const char *p_src_dir, const char *p_dest, const char *p_dest_dir, bool safe)
{
  FHANDLE src = file_open(p_src_dir,  p_src,  "rb", safe);
  if(!src)
    return(FALSE);

  FHANDLE dst = file_open(p_dest_dir, p_dest, "wb", safe);
  if(!dst) {    
    file_close(src);
    return(FALSE);
  }

  bool ret = file_copy(src, dst);
  
  file_close(src);
  file_close(dst);

  return(ret);
}

bool file_exists(const char * p_dir, const char * p_file)
{
  FHANDLE f = file_open(p_dir, p_file, "rb", FALSE);  
  if(!(f)) {
    return(FALSE);
  } 
  else {
    file_close(f);
    return(TRUE);
  }
}

int file_size_get(FHANDLE f)
{
  int size;
  int zal;

  zal = ftell(f);
  fseek(f, 0, SEEK_END);
  size = ftell(f) + 1;
  fseek(f, zal, SEEK_SET);

  return (size);
}

int file_size_get(const char * p_dir, const char * p_file)
{
  return(file_size_get(file_open(p_dir, p_file, "rb")));
}

void print_errno(bool new_line)
{
  if(new_line) {
    bprintf("\nError: %s",strerror(errno));
  } else {
    bprintf(strerror(errno));
  }  
}

char * dir_home_get(char *p_dir, int max)
{
  assert(p_dir);

  char *p_tmp = getenv("HOME");
  if(p_tmp) {
    strncpy(p_dir,p_tmp,max);
  } else {
    // a homeless user?
    assert(max >= 1);
    p_dir[0] = '\0';
  }
  return(p_dir);
}

bool dir_create(const char *p_dir)
{
  assert(p_dir);
  struct stat st;

  char tmp_dir[MAX_FILENAME];
#ifdef WINDOWS
  strcpy(tmp_dir, p_dir);
#else
  return_path(p_dir, "", tmp_dir, MAX_FILENAME);
#endif

  // Check the dir
  bprintfnl("Checking %s...",tmp_dir);
  if(stat(tmp_dir,&st) == -1 && errno == ENOENT) {
    bprintfnl("\nmissing, try to create it...");
    if(mkdirm(tmp_dir) != -1) {
      bprintf("ok");
      return(TRUE);
    } else {
      print_errno(TRUE);      
      return(FALSE);
    }
  } else {
    bprintf("ok");
    return(TRUE);
  }
}

#ifdef LINUX
static const char *p_file_mask;

static int filter(const struct dirent *file)
{
  return(!fnmatch(p_file_mask, file->d_name, 0));
}

int file_list_get(const char *p_dir, const char *p_mask, DIRECTORY_ENTRY **p_list)
{
  char tmp[MAX_FILENAME];
  struct dirent **namelist;
  int i;
  
  return_path(p_dir, "", tmp, MAX_FILENAME);
  
  p_file_mask = p_mask;
  int c = scandir(tmp, &namelist, &filter, alphasort);
  if (c < 0) {
    return 0;
  }

  *p_list = (DIRECTORY_ENTRY *)mmalloc(sizeof(DIRECTORY_ENTRY)*c);
  for(i = 0; i < c; i++) {    
    strcpy((*p_list)[i].name, namelist[i]->d_name);
    free(namelist[i]);
  } 

  free(namelist);
  return(c);
}
#endif

#ifdef WINDOWS
int file_list_get(const char *p_dir, const char *p_mask, DIRECTORY_ENTRY **p_list)
{	
	long ret, handle;
  char current_dir[MAX_FILENAME];
	struct _finddata_t	Data;
  int  size;

  _getcwd(current_dir,MAX_FILENAME);
  _chdir(p_dir);

  size = 0;
  handle = ret = _findfirst(p_mask, &Data);
	while(ret != -1)
	{
    size++;
		ret = _findnext(handle, &Data);
	}
	_findclose(handle); 

  // Sorry dude, no files
  if(size) {
    *p_list = (DIRECTORY_ENTRY *)mmalloc(sizeof(DIRECTORY_ENTRY)*size);
  
    size = 0;
    handle = ret = _findfirst(p_mask, &Data);	
    while(ret != -1)
    {
      strcpy((*p_list)[size++].name, Data.name);
      ret = _findnext(handle, &Data);
    }
    _findclose(handle); 
  }

  _chdir(current_dir);

  return size;
}
#endif

/* Loading routines
*/
bool graphics_logos_load(DIR_LIST *p_dir)
{
  int i = 0;

  p_grf->graphics_dir_set(p_dir->graphics_get());

  sprite::color_key_set(COLOR_KEY_GAME);
  i  += p_grf->sprite_insert("logo.spr", FIRST_LOGO);

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

  i  = p_grf->sprite_insert("global1.spr", FIRST_GLOBAL_LEVEL);
  i += p_grf->sprite_insert("global2.spr", FIRST_GLOBAL_LEVEL + ROT_SHIFT);
  i += p_grf->sprite_insert("global3.spr", FIRST_GLOBAL_LEVEL + 2 * ROT_SHIFT);
  i += p_grf->sprite_insert("global4.spr", FIRST_GLOBAL_LEVEL + 3 * ROT_SHIFT);

  i += p_grf->sprite_insert("klasik1.spr", FIRST_CLASSIC_LEVEL);
  i += p_grf->sprite_insert("klasik2.spr", FIRST_CLASSIC_LEVEL + ROT_SHIFT);
  i += p_grf->sprite_insert("klasik3.spr", FIRST_CLASSIC_LEVEL + 2 * ROT_SHIFT);
  i += p_grf->sprite_insert("klasik4.spr", FIRST_CLASSIC_LEVEL + 3 * ROT_SHIFT);

  i += p_grf->sprite_insert("kyber1.spr", FIRST_CYBER_LEVEL);
  i += p_grf->sprite_insert("kyber2.spr", FIRST_CYBER_LEVEL + ROT_SHIFT);
  i += p_grf->sprite_insert("kyber3.spr", FIRST_CYBER_LEVEL + 2 * ROT_SHIFT);
  i += p_grf->sprite_insert("kyber4.spr", FIRST_CYBER_LEVEL + 3 * ROT_SHIFT);
  
  i += p_grf->sprite_insert("herni1.spr",  FIRST_OTHER);
  i += p_grf->sprite_insert("herni2.spr",  FIRST_OTHER + ROT_SHIFT);
  
  i += p_grf->sprite_insert("game_cur.spr", FIRST_CURSOR);
  
  i += p_grf->sprite_insert("hraci1.spr", FIRST_PLAYER);
  i += p_grf->sprite_insert("hraci2.spr", FIRST_PLAYER + ROT_SHIFT);
  i += p_grf->sprite_insert("hraci3.spr", FIRST_PLAYER + 2 * ROT_SHIFT);
  i += p_grf->sprite_insert("hraci4.spr", FIRST_PLAYER + 3 * ROT_SHIFT);

  if(berusky_config::new_gfx) {
    i += p_grf->sprite_insert("box_bright1.spr", FIRST_BOX_BRIGHT);
    i += p_grf->sprite_insert("box_dark1.spr", FIRST_BOX_DARK);
    i += p_grf->sprite_insert("box_paper1.spr", FIRST_BOX_PAPER);
    i += p_grf->sprite_insert("box_snow1.spr", FIRST_BOX_SNOW);
    i += p_grf->sprite_insert("light_box1.spr", FIRST_LIGHT_BOX);
    i += p_grf->sprite_insert("tnt_bright1.spr", FIRST_TNT_BRIGHT);
    i += p_grf->sprite_insert("tnt_dark1.spr", FIRST_TNT_DARK);
    i += p_grf->sprite_insert("tnt_paper1.spr", FIRST_TNT_PAPER);
    i += p_grf->sprite_insert("tnt_snow1.spr", FIRST_TNT_SNOW);
    i += p_grf->sprite_insert("tnt_swamp1.spr", FIRST_TNT_SWAMP);
    i += p_grf->sprite_insert("wall_iron_blue1.spr", FIRST_WALL_IRON_BLUE);
    i += p_grf->sprite_insert("wall_iron_brown1.spr", FIRST_WALL_IRON_BROWN);
    i += p_grf->sprite_insert("wall_iron_dark1.spr", FIRST_WALL_IRON_DARK);
    i += p_grf->sprite_insert("wall_iron_gray1.spr", FIRST_WALL_IRON_GRAY);
    i += p_grf->sprite_insert("wall_machine1.spr", FIRST_WALL_MACHINE);
    i += p_grf->sprite_insert("wall_repro1.spr", FIRST_WALL_REPRO);
    i += p_grf->sprite_insert("wall_snow1.spr", FIRST_WALL_SNOW);
    i += p_grf->sprite_insert("wall_swamp1.spr", FIRST_WALL_SWAMP);
    i += p_grf->sprite_insert("wall_wood1.spr", FIRST_WALL_WOOD);
  
    i += p_grf->sprite_insert("floor_danger1.spr", FIRST_FLOOR_DANGER_SRC);
    i += p_grf->sprite_insert("floor_elevators1.spr", FIRST_FLOOR_ELEVATORS_SRC);
    i += p_grf->sprite_insert("floor_gray1.spr", FIRST_FLOOR_GRAY_SRC);
    
    i += p_grf->sprite_insert("floor_iron_1.spr", FIRST_FLOOR_IRON);
    i += p_grf->sprite_insert("floor_iron_2.spr", FIRST_FLOOR_IRON+5);
    i += p_grf->sprite_insert("floor_iron_3.spr", FIRST_FLOOR_IRON+10);
    i += p_grf->sprite_insert("floor_iron_4.spr", FIRST_FLOOR_IRON+15);
    i += p_grf->sprite_insert("floor_iron_5.spr", FIRST_FLOOR_IRON+20);
    i += p_grf->sprite_insert("floor_snow.spr",   FIRST_FLOOR_SNOW);
    
    // Generate rest of the items
    graphics_generate();
  }

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
  p_grf->sprite_delete(FIRST_OTHER + ROT_SHIFT, GAME_SPRITES);

  p_grf->sprite_delete(FIRST_CURSOR, CURSOR_SPRITES);
  
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

  sprite::color_key_set(COLOR_KEY_BLACK);
  i   = p_grf->sprite_insert("menu1.spr", MENU_SPRIT_ROCK);
  i  += p_grf->sprite_insert("menu2.spr", MENU_SPRIT_LOGO);
  i  += p_grf->sprite_insert("menu3.spr", MENU_SPRIT_BACK);
  i  += p_grf->sprite_insert("menu6.spr", MENU_SPRIT_WALL);
  
  sprite::color_key_set(COLOR_KEY_MENU);
  i  += p_grf->sprite_insert("menu_back1.spr", MENU_SPRIT_BACK1);
  i  += p_grf->sprite_insert("menu_back2.spr", MENU_SPRIT_BACK2);
  i  += p_grf->sprite_insert("menu_back3.spr", MENU_SPRIT_BACK3);

  sprite::color_key_set(COLOR_KEY_MENU);
  i  += p_grf->sprite_insert("menu4.spr", MENU_SPRIT_ARROWS);
  i  += p_grf->sprite_insert("controls.spr", MENU_CHECKBOX_CHECKED);
  i  += p_grf->sprite_insert("slidebar.spr", MENU_SLIDEBAR);
  i  += p_grf->sprite_insert("slider.spr", MENU_SLIDER);

  sprite::color_key_set(COLOR_KEY_GAME);
  i  += p_grf->sprite_insert("menu5.spr", MENU_SPRIT_LOGO_SMALL_1);

  sprite::color_key_set(COLOR_KEY_BLACK_FULL);
  i  += p_grf->sprite_insert("back1.spr", MENU_SPRIT_START);
  sprite::color_key_set(COLOR_KEY_BLACK);
  i  += p_grf->sprite_insert("back2.spr", MENU_SPRIT_START+1);
  i  += p_grf->sprite_insert("back3.spr", MENU_SPRIT_START+2);
  i  += p_grf->sprite_insert("back4.spr", MENU_SPRIT_START+3);
  
  sprite::color_key_set(COLOR_KEY_GAME);
  i  += p_grf->sprite_insert("mask1.spr",  EDITOR_MARK_BLACK);
  i  += p_grf->sprite_insert("mask2.spr",  EDITOR_MARK_RED);
  i  += p_grf->sprite_insert("mask3.spr",  EDITOR_MARK_YELLOW);
  
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

/*
  // ####
  // ##@@         
  CHANGE_FLOOR(x,y,0);
  //   ##        
  //   @@         
  CHANGE_FLOOR(x,y,3);
  //
  // ##@@         
  CHANGE_FLOOR(x,y,2);
  // ##
  //   @@
  CHANGE_FLOOR(x,y,1);
  // 
  //   @@
  CHANGE_FLOOR(x,y,4); 
*/
// TODO -> randomize the shadow
void graphics_generate_floor(spr_handle spr, int type)
{
  SURFACE *p_surf = (p_grf->sprite_get(spr))->surf_get();
  tcolor color = p_surf->color_map(30, 30, 30);
  
  switch(type) {
    case 0:
      {      
        p_surf->blend(0, 0, 12, p_surf->height_get(), color, BLEND_SUB);
        p_surf->blend(12, 0, p_surf->width_get()-12, 14, color, BLEND_SUB);
      }
      break;
    case 1:
      {
        SURFACE *p_surf = (p_grf->sprite_get(spr))->surf_get();
        p_surf->blend(0, 0, 12, 14, color, BLEND_SUB);
      }
      break;
    case 2:
      {      
        SURFACE *p_surf = (p_grf->sprite_get(spr))->surf_get();
        p_surf->blend(0, 0, 12, p_surf->height_get(), color, BLEND_SUB);
      }
      break;
    case 3:
      {      
        SURFACE *p_surf = (p_grf->sprite_get(spr))->surf_get();
        p_surf->blend(0, 0, p_surf->width_get(), 14, color, BLEND_SUB);
      }
      break;
    case 4: // no action
      break;
    default:
      break;  
  }
}

// Regenerate rest of graphics
void graphics_generate(void)
{
  // Create black sprite for blending
  p_grf->sprite_copy(SPRITE_BLACK, FIRST_CLASSIC_LEVEL+57, TRUE);
  SDL_Surface *p_surf = ((p_grf->sprite_get(SPRITE_BLACK))->surf_get())->surf_get();
  SDL_SetAlpha(p_surf, SDL_SRCALPHA, 150);

  int i;

  // Generate floor graphics
  for(i = 1; i < 5; i++) {
    p_grf->sprite_copy(FIRST_FLOOR_IRON+i, FIRST_FLOOR_IRON, TRUE);
    graphics_generate_floor(FIRST_FLOOR_IRON+i, i);
  }
  graphics_generate_floor(FIRST_FLOOR_IRON, 0);

  for(i = 1; i < 5; i++) {
    p_grf->sprite_copy(FIRST_FLOOR_IRON+5+i, FIRST_FLOOR_IRON+5, TRUE);
    graphics_generate_floor(FIRST_FLOOR_IRON+5+i, i);
  }
  graphics_generate_floor(FIRST_FLOOR_IRON+5, 0);

  for(i = 1; i < 5; i++) {
    p_grf->sprite_copy(FIRST_FLOOR_IRON+10+i, FIRST_FLOOR_IRON+10, TRUE);
    graphics_generate_floor(FIRST_FLOOR_IRON+10+i, i);
  }
  graphics_generate_floor(FIRST_FLOOR_IRON+10, 0);

  for(i = 1; i < 5; i++) {
    p_grf->sprite_copy(FIRST_FLOOR_IRON+15+i, FIRST_FLOOR_IRON+15, TRUE);
    graphics_generate_floor(FIRST_FLOOR_IRON+15+i, i);
  }
  graphics_generate_floor(FIRST_FLOOR_IRON+15, 0);

  for(i = 1; i < 5; i++) {
    p_grf->sprite_copy(FIRST_FLOOR_IRON+20+i, FIRST_FLOOR_IRON+20, TRUE);
    graphics_generate_floor(FIRST_FLOOR_IRON+20+i, i);
  }
  graphics_generate_floor(FIRST_FLOOR_IRON+20, 0);

  for(i = 1; i < 5; i++) {
    p_grf->sprite_copy(FIRST_FLOOR_SNOW+i, FIRST_FLOOR_SNOW, TRUE);
    graphics_generate_floor(FIRST_FLOOR_SNOW+i, i);
  }
  graphics_generate_floor(FIRST_FLOOR_SNOW, 0);

  for(i = 0; i < 5; i++) {
    // 4 sprites
    p_grf->sprite_copy(FIRST_FLOOR_DANGER+i, FIRST_FLOOR_DANGER_SRC, TRUE);
    graphics_generate_floor(FIRST_FLOOR_DANGER+i, i);
    p_grf->sprite_copy(FIRST_FLOOR_DANGER+5+i, FIRST_FLOOR_DANGER_SRC+1, TRUE);
    graphics_generate_floor(FIRST_FLOOR_DANGER+5+i, i);
    p_grf->sprite_copy(FIRST_FLOOR_DANGER+10+i, FIRST_FLOOR_DANGER_SRC+2, TRUE);
    graphics_generate_floor(FIRST_FLOOR_DANGER+10+i, i);
    p_grf->sprite_copy(FIRST_FLOOR_DANGER+15+i, FIRST_FLOOR_DANGER_SRC+3, TRUE);
    graphics_generate_floor(FIRST_FLOOR_DANGER+15+i, i);
  
    // 3 sprites
    p_grf->sprite_copy(FIRST_FLOOR_ELEVATORS+i, FIRST_FLOOR_ELEVATORS_SRC, TRUE);
    graphics_generate_floor(FIRST_FLOOR_ELEVATORS+i, i);
    p_grf->sprite_copy(FIRST_FLOOR_ELEVATORS+5+i, FIRST_FLOOR_ELEVATORS_SRC+1, TRUE);
    graphics_generate_floor(FIRST_FLOOR_ELEVATORS+5+i, i);
    p_grf->sprite_copy(FIRST_FLOOR_ELEVATORS+10+i, FIRST_FLOOR_ELEVATORS_SRC+2, TRUE);
    graphics_generate_floor(FIRST_FLOOR_ELEVATORS+10+i, i);
  
    // 2 sprites
    p_grf->sprite_copy(FIRST_FLOOR_GRAY+i, FIRST_FLOOR_GRAY_SRC, TRUE);
    graphics_generate_floor(FIRST_FLOOR_GRAY+i, i);
    p_grf->sprite_copy(FIRST_FLOOR_GRAY+5+i, FIRST_FLOOR_GRAY_SRC+1, TRUE);
    graphics_generate_floor(FIRST_FLOOR_GRAY+5+i, i);
  }
}

/* It tries to create the user directory (~./berusky)
 * and copy berusky.ini file there
 *
 */
void user_directory_create(void)
{
  // Check ~./berusky
#ifdef WINDOWS
  dir_create(DIRECTORY_GET(INI_ANAKREON_DIR));
  dir_create(DIRECTORY_GET(INI_BERUSKY_DIR));
#endif

  dir_create(DIRECTORY_GET(INI_USER_DIRECTORY));
  dir_create(DIRECTORY_GET(INI_USER_LEVELS));
  dir_create(DIRECTORY_GET(INI_USER_PROFILES));

  // Check ~./berusky/berusky.ini
  bprintfnl(_("Checking %s/%s..."),DIRECTORY_GET(INI_USER_DIRECTORY),INI_FILE_NAME);
  if(!file_exists(DIRECTORY_GET(INI_USER_DIRECTORY),INI_FILE_NAME)) {
    bprintfnl(_("missing, try to copy it from %s..."),FILE_GET(INI_FILE_GLOBAL));
    bool ret = file_copy(FILE_GET(INI_FILE_GLOBAL),
                         NULL,
                         INI_FILE_NAME, 
                         DIRECTORY_GET(INI_USER_DIRECTORY),
                         FALSE);
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

