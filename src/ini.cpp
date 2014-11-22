/* Martin Stransky <stransky@redhat.com>
 *
 * Copyright (C) 2003,2005 Red Hat, Inc.
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

/* ini file handling
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ini.h"
#include "utils.h"

#define SPR '='

char *ini_remove_end_of_line(char *p_line)
{
  char *p_start = p_line;

  while (*p_line && *p_line != '\n')
    p_line++;

  if (*p_line == '\n')
    *p_line = '\0';

  return (p_start);
}

char *ini_skip_spaces(char *p_line)
{
  while (*p_line && (*p_line == ' ' || *p_line == '\t'))
    p_line++;
  return (p_line);
}

char *ini_skip_separator(char *p_line)
{
  char *p_tmp = strchr(p_line, SPR);
  if (p_tmp) {
    return (ini_skip_spaces(p_tmp + 1));
  }
  else {
    return (NULL);
  }
}

char *ini_read_param(char *p_line, char *p_param, int max_len)
{
  char *p_start = p_param;
  int i = 0;

  while (*p_line && i < max_len - 1 && *p_line != '\n') {
    *p_param++ = *p_line++;
    i++;
  }
  *p_param = '\0';

  return (p_start);
}


char *ini_read_string(FHANDLE f, const char *p_template, char *p_out,
                      int max_len, const char *p_default)
{
  char line[MAX_TOKEN_LEN];

  fseek(f, SEEK_SET, 0);
  while (fgets(line, MAX_TOKEN_LEN, f)) {
    int len = is_token(line, p_template);
    char *p_rest;
    if (len && (p_rest = ini_skip_separator(line + len))) {
      return (ini_read_param(p_rest, p_out, max_len));
    }
  }

  return (strcpy(p_out, p_default));
}

char *ini_read_string_file(const char *p_file, const char *p_template, char *p_out,
                           int max_len, const char *p_default)
{
  char line[MAX_TOKEN_LEN];
  FHANDLE f = file_open(NULL, p_file, "r", FALSE);

  if (!f)
    return (strcpy(p_out, p_default));

  while (fgets(line, MAX_TOKEN_LEN, f)) {
    int len = is_token(line, p_template);
    char *p_rest;
    if (len && (p_rest = ini_skip_separator(line + len))) {
      fclose(f);
      return (ini_read_param(p_rest, p_out, max_len));
    }
  }

  fclose(f);
  return (strcpy(p_out, p_default));
}

int ini_read_int(FHANDLE f, char *p_template, int dflt)
{
  char line[MAX_TOKEN_LEN];

  fseek(f, SEEK_SET, 0);
  while (fgets(line, MAX_TOKEN_LEN, f)) {
    int len = is_token(line, p_template);
    char *p_rest;
    if (len && (p_rest = ini_skip_separator(line + len))) {
      return (atoi(ini_remove_end_of_line(p_rest)));
    }
  }
  return (dflt);
}

int ini_read_int_file(const char *p_file, const char *p_template, int dflt)
{
  char line[MAX_TOKEN_LEN];
  FHANDLE f = file_open(NULL, p_file, "r", FALSE);

  if (!f)
    return (dflt);

  while (fgets(line, MAX_TOKEN_LEN, f)) {
    int len = is_token(line, p_template);
    char *p_rest;
    if (len && (p_rest = ini_skip_separator(line + len))) {
      fclose(f);
      return (atoi(ini_remove_end_of_line(p_rest)));
    }
  }
  fclose(f);
  return (dflt);
}


int ini_read_bool(FHANDLE f, const char *p_template, int dflt)
{
  char line[MAX_TOKEN_LEN];

  ini_read_string(f, p_template, line, MAX_TOKEN_LEN, "");
  if (line[0] == '\0')
    return (dflt);
  else {
    if (is_token(line, TOKEN_FALSE1) || is_token(line, TOKEN_FALSE2))
      return (FALSE);
    else if (is_token(line, TOKEN_TRUE1) || is_token(line, TOKEN_TRUE2))
      return (TRUE);
    else
      return (dflt);
  }
}

int ini_read_bool_file(const char *p_file, const char *p_template, int dflt)
{
  char line[MAX_TOKEN_LEN];
  FHANDLE f = file_open(NULL, p_file, "r", FALSE);

  if (!f)
    return (dflt);

  ini_read_string(f, p_template, line, MAX_TOKEN_LEN, "");
  fclose(f);

  if (line[0] == '\0')
    return (dflt);
  else {
    if (is_token(line, TOKEN_FALSE1) || is_token(line, TOKEN_FALSE2))
      return (FALSE);
    else if (is_token(line, TOKEN_TRUE1) || is_token(line, TOKEN_TRUE2))
      return (TRUE);
    else
      return (dflt);
  }
}


/* Check if the given string is a token
*/
int is_token(char *p_line, const char *p_token)
{
  char *p_start = p_line;

  while (*p_line && *p_token && tolower(*p_line) == tolower(*p_token)) {
    p_line++;
    p_token++;
  }

  return (*p_token ? 0 : p_line - p_start);
}

/* Reading token (between %) from file
*/
int read_token(FHANDLE f_in, char *p_line, size_t max, char separator)
{
  size_t len;
  int c;

  for (len = 0;
       (c = fgetc(f_in)) != EOF && c != separator && len + 2 < max;
       len++, p_line++) {
    *p_line = c;
  }

  if (c == separator) {
    *p_line++ = c;
  }
  *p_line = 0;

  return (c == separator);
}

bool ini_find_token(FHANDLE f, const char *p_template,
                    long *p_file_start, long *p_file_end)
{
  char line[MAX_TOKEN_LEN];
  long file_pos = 0;

  fseek(f, 0, SEEK_SET);
  while (fgets(line, MAX_TOKEN_LEN, f)) {
    if(is_token(line, p_template)) {
      *p_file_start = file_pos;
      *p_file_end = ftell(f);
      return(TRUE);
    }
    file_pos = ftell(f);
  }

  // no token - write a new one
  return(FALSE);
}

int ini_write_string(FHANDLE f_in, FHANDLE f_out, const char *p_template, const char *p_value)
{
  long file_start,
       file_end;

  int found = ini_find_token(f_in, p_template, &file_start, &file_end);

  fseek(f_in, 0, SEEK_SET);
  fseek(f_out, 0, SEEK_SET);

  if(found) {
    file_copy(f_in, f_out, file_start);
    fprintf(f_out,"%s = %s\n",p_template, p_value);
    fseek(f_in, file_end, SEEK_SET);
    file_copy(f_in, f_out);
  }
  else {
    file_copy(f_in, f_out);    
    fprintf(f_out,"\n%s = %s\n",p_template, p_value);
  }

  return(TRUE);
}

bool ini_write_string(const char *p_file,
                      const char *p_template, const char *p_value)
{
  int ret;

  FHANDLE f_orig = file_open(NULL, p_file, "r", FALSE);
  if (!f_orig)
    return(FALSE);
  
#ifdef WINDOWS  
  FHANDLE f_new(tmpfile_get());
#else
  FHANDLE f_new(tmpfile());
#endif
  if (!f_new)
    return(FALSE);

  ret = file_copy(f_orig, f_new);
  if(!ret) {
    fclose(f_orig);
    fclose(f_new);
    return(FALSE);
  }

  fclose(f_orig);

  if((f_orig = file_open(NULL, p_file, "w", FALSE))) {
    ret = ini_write_string(f_new, f_orig, p_template, p_value);
    fclose(f_orig);
  }

  fclose(f_new);

  return (ret);
}
