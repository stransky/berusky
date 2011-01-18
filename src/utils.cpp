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

#ifdef WINDOWS
#include <direct.h>
#endif

#include "ini.h"
#include "utils.h"
#include "types.h"

#undef fopen
#undef fclose

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
  #define INI_TMP         "tmp_data"

  ini_read_string_file(p_ini, INI_LEVEL, levels, sizeof(levels), "./Lihen/Levels");
  ini_read_string_file(p_ini, INI_GAME, gamedata, sizeof(gamedata), "./Lihen/GameData");
  ini_read_string_file(p_ini, INI_GRAPHICS, graphics, sizeof(graphics), "./Lihen/Graphics");
  ini_read_string_file(p_ini, INI_LEVEL_USER, levels_user, sizeof(levels_user), "./Lihen/User");
  ini_read_string_file(p_ini, INI_TMP, tmp, sizeof(tmp), "/var/tmp");

  getcwd(cwd,MAX_FILENAME);
  
  update_path(levels);
  update_path(gamedata);
  update_path(graphics);  
  update_path(levels_user);
  update_path(tmp);
  chdir(cwd);

  #define INI_BINARY      "game_binary"
  ini_read_string_file(p_ini, INI_BINARY, game_binary, sizeof(game_binary), "berusky");
  
  bprintf("level_data: %s",levels);
  bprintf("game_data: %s",gamedata);
  bprintf("graphics_data: %s",graphics);
  bprintf("level_data_user: %s",levels_user);
  bprintf("tmp_data: %s",tmp);
  bprintf("current working dir: %s",cwd);
}

bool get_fullscreen(const char *p_ini_file)
{  
  #define INI_FULLSCREEN "fullscreen"
  return(ini_read_int_file(p_ini_file, INI_FULLSCREEN, FALSE));
}

int  get_colors(const char *p_ini_file, int default_color_depth)
{ 
  #define INI_COLOR "color_depth"
  return(ini_read_int_file(p_ini_file, INI_COLOR, default_color_depth));
}

#ifdef LINUX
void itoa(int base, char *buf, int d)
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
}
#endif // LINUX

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
    strcat(p_buffer,"/");
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
bool file_save(const char * p_dir, const char * p_file, void *p_buffer, t_off lenght)
{  
  FHANDLE f = file_open(p_dir, p_file, "wb");
  t_off wrt = fwrite(p_buffer, 1, lenght, f);
  fclose(f);  
  return(wrt == lenght);
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
  
  char buffer[4096];
  int  readed;
  
  while((readed = fread(buffer,1,4096,src))) {
    fwrite(buffer,1,readed,dst);
  }

  file_close(src);
  file_close(dst);

  return(TRUE);
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
    bprintf("\n%s",strerror(errno));
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

  char tmp_dir[MAX_FILENAME];
  return_path(p_dir, "", tmp_dir, MAX_FILENAME);

  struct stat st;

  // Check the dir
  bprintfnl("Checking %s...",tmp_dir);
  if(stat(tmp_dir,&st) == -1 && errno == ENOENT) {
    bprintfnl("missing, try to create it...");
    if(mkdir(tmp_dir) != -1) {
      bprintf("ok");
      return(TRUE);
    } else {
      print_errno(TRUE);
      bprintf("failed");
      return(FALSE);
    }
  } else {
    bprintf("ok");
    return(TRUE);
  }
}
