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
#include <dirent.h>
#include <fnmatch.h>

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

#define INI_FULLSCREEN "fullscreen"
bool get_fullscreen(const char *p_ini_file)
{  
  return(ini_read_int_file(p_ini_file, INI_FULLSCREEN, FALSE));
}

bool set_fullscreen(const char *p_ini_file, bool state)
{  
  char tmp[100];
  return(ini_write_string(p_ini_file, INI_FULLSCREEN, itoa(10, tmp, state ? 1 : 0)));
}

int  get_colors(const char *p_ini_file, int default_color_depth)
{ 
  #define INI_COLOR "color_depth"
  return(ini_read_int_file(p_ini_file, INI_COLOR, default_color_depth));
}

#ifdef LINUX
char * itoa(int base, char *buf, int d)
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
bool file_save(const char * p_dir, const char * p_file, void *p_buffer, t_off lenght)
{  
  FHANDLE f = file_open(p_dir, p_file, "wb");
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
int file_list_get(const char *p_dir, const char *p_mask, DIRECTORY_ENTRY *p_list)
{
//int pr_FindFileToProfile(WCHAR * wName, char *cFile)
//{
	PLAYER_PROFILE	Profile;
	FILE *file;
	long Done, error;
	struct _finddata_t	Data;

	Done = _findfirst(p_mask,&Data);
	error = Done;
			
	while(error != -1)
	{
		if(error != -1)
		{
			file = fopen(Data.name, "rb");

			if(file)
			{
				fread(&Profile, sizeof(PLAYER_PROFILE), 1, file);
				fclose(file);

				if(!wcscmp(Profile.cName, wName))
				{
					strcpy(cFile, Data.name);
					_findclose(Done); 
					return 1;
				}
			}

			error = _findnext(Done,&Data);
		}
	}
	_findclose(Done); 

  return 0;
}
#endif
