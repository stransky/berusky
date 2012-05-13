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

#ifndef __UTILS_H__
#define __UTILS_H__

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#include "portability.h"

#ifdef LINUX
#include <syslog.h>
#include <unistd.h>

#endif // LINUX

#define  LOG_ENABLED 1

#define  MAX_FILENAME PATH_MAX

#ifndef  FALSE
#define  FALSE (1!=1)
#endif

#ifndef  TRUE
#define  TRUE  (1==1)
#endif

#ifndef  ERROR
#define  ERROR (-1)
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#define POINTER_TO_INT(pointer) (*((int*)&(pointer)))
#define INT_TO_POINTER(integer) (reinterpret_cast<void *>(integer))

// -------------------------------------------------------
// file interfaces
// -------------------------------------------------------
#define NO_DIR (NULL)

typedef class fhandle {
	
public:

	FILE *f;

public:

  fhandle(void) : f(NULL) {};
  fhandle(FILE *f_in) : f(f_in) {};

  void operator=(void *f_)
  {
    f = (FILE *)f_;
  }

  void operator=(class fhandle *f_)
  {
    f = f_->f;
  }

  bool operator==(void *f_)
  {
    return(f == f_);
  }

  operator FILE*(void)
  {
    return(f);
  }

  operator bool(void)
  {
    return(f != NULL);
  }

} FHANDLE;

typedef unsigned int t_off;

char *    return_path(const char *p_dir, const char *p_file, char *p_buffer, int max_lenght);

FHANDLE   file_open(const char *p_dir, const char *p_file, const char *p_mode, bool safe = TRUE);
void      file_close(FHANDLE f);

int    		file_load(const char *p_dir, const char *p_file, char *p_mem, t_off max_lenght, t_off start_address = 0, bool safe = TRUE);
int    		file_load_text(const char * p_dir, const char * p_file, char * p_mem, t_off max_lenght, t_off start_address = 0, bool safe = TRUE);

int    	  file_load(FHANDLE f, char * p_mem, t_off max_lenght, t_off start_address = 0);
int       file_load_text(FHANDLE f, char * p_mem, t_off max_lenght, t_off start_address = 0);

void *    file_load(const char *p_dir, const char *p_file, t_off *p_lenght, t_off start_address = 0, bool safe = TRUE);

bool      file_save(const char *p_dir, const char *p_file, void *p_buffer, t_off lenght);

bool      file_exists(const char *p_dir, const char *p_file);

int       file_size_get(const char * p_dir, const char * p_file);
int       file_size_get(FHANDLE f);

bool      file_copy(const char *p_src, const char *p_src_dir, const char *p_dest, const char *p_dest_dir, bool safe = TRUE);
bool      file_copy(FHANDLE f_in, FHANDLE f_out, int len = 0);

void      print_errno(bool new_line = FALSE);

bool      dir_create(const char *p_dir);
char   *  dir_home_get(char *p_dir, int max);

// Don't use standart I/O
#define fopen(a,b)
#define fclose(a)
#define FILE

#define DEFAULT_DIR_MASK   0755
#define DEFAULT_FILE_MASK  0644

// -------------------------------------------------------
// log file management
// -------------------------------------------------------

extern FHANDLE log_file;

void log_open(const char *p_file);
void log_open_ini(const char *p_ini_file);
void log_close(void);
void log_flush(void);

// -------------------------------------------------------
// memory allocation
// -------------------------------------------------------

inline void * mmalloc(int size)
{
   void *p_tmp = malloc(size);

   if(!p_tmp) {
#ifdef LINUX
     syslog(3,"Out of memory! file: %s line: %d",__FILE__,__LINE__);
#endif // LINUX
     assert(0);
     exit(0);
   } else {
     memset(p_tmp,0,size);
     return(p_tmp);
   }
}

inline void * mmemcpy(void *p_src, int size)
{
  void *p_tmp = mmalloc(size);
  memcpy(p_tmp, p_src, size);
  return(p_tmp);
}

inline void * rrealloc(void *p_mem, int size)
{
   void *p_tmp = realloc(p_mem,size);

   if(!p_tmp) {
#ifdef LINUX   
     syslog(3,"Out of memory! file: %s line: %d",__FILE__,__LINE__);
#endif // LINUX   
     assert(0);
     exit(0);
   } else {
     return(p_tmp);
   }
}

inline void xfree(void **p_mem)
{
   if(p_mem && *p_mem) {
     free(*p_mem);
     *p_mem = NULL;     
   }
}

#define ffree(ptr) { if(ptr) { free(ptr); ptr = NULL; }}

// -------------------------------------------------------
// logging - helper function
// -------------------------------------------------------
#ifdef  LOG_ENABLED

inline void berror(const char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);

  fprintf(stderr,"Error at line %d file %s: %s\n",__LINE__,__FILE__,text);

  assert(0);
  exit(255);
}

inline void bprintf(const char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);

  fprintf(stderr,"%s\n",text);

  if(log_file) {
    fprintf(log_file,"%s\n",text);
  }
}

inline void bprintfnl(const char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);

  fprintf(stderr,"%s",text);

  if(log_file) {
    fprintf(log_file,"%s",text);
  }
}

inline int fgets_correction(char * p_kor)
{
  int delka = strlen(p_kor);
  if (p_kor[delka - 1] == '\n') {
    p_kor[delka - 1] = 0;
    return (delka - 1);
  }
  else {
    return (delka);
  }
}

#else

#define berror(p_text,...)
#define bprintf(p_text,...)
#define fgets_correction(p_kor)

#endif

// -------------------------------------------------------
// configuration management
// -------------------------------------------------------

typedef class dir_list {

  char levels[MAX_FILENAME];
  char levels_user[MAX_FILENAME];
  char gamedata[MAX_FILENAME];
  char graphics[MAX_FILENAME];
  char tmp[MAX_FILENAME];
  char game_binary[MAX_FILENAME];
  char cwd[MAX_FILENAME];

public:
  
  dir_list(void)
  {
    levels[0] = '\0';
    levels_user[0] = '\0';
    gamedata[0] = '\0';
    graphics[0] = '\0';
    tmp[0] = '\0';
    game_binary[0] = '\0';    
    cwd[0] = '\0';
  }

  ~dir_list(void) {};

private:
  
  void update_path(char *p_dir);

public:
  
  void load(const char *p_ini);

  char * levels_get(void)
  {
    return(levels);
  }

  char * levels_user_get(void)
  {
    return(levels_user);
  }

  char * gamedata_get(void)
  {
    return(gamedata);
  }

  char * graphics_get(void)
  {
    return(graphics);
  }

  char * tmp_get(void)
  {
    return(tmp);
  }

  char * game_binary_get(void)
  {
    return(game_binary);
  }

  char * cwd_get(void)
  {
    return(cwd);
  }

} DIR_LIST;

bool get_fullscreen(const char *p_ini_file);
bool set_fullscreen(const char *p_ini_file, bool state);

int  get_colors(const char *p_ini_file, int default_color_depth);

// -------------------------------------------------------
// the rest
// -------------------------------------------------------

inline char * change_tail(char * p_str, const char * p_end)
{
  char *p_act = p_str, *p_last = NULL;
  while((p_act = strchr(p_act, '.'))) {
    p_last = p_act;
    p_act++;
  }  
  if (p_last)
    *p_last = 0;
  return (p_end ? (char *) strcat(p_str, p_end) : p_str);
}

#ifdef LINUX
char * itoa(int base, char *buf, int d);
#endif

#endif // __UTILS_H__
