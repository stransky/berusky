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

#ifndef __INI_H__
#define __INI_H__

#define MAX_TOKEN_LEN       1000

#define TOKEN_TRUE1         "yes"
#define TOKEN_TRUE2         "on"

#define TOKEN_FALSE1        "no"
#define TOKEN_FALSE2        "off"

#ifndef TRUE
#define TRUE                (1==1)
#endif

#ifndef FALSE
#define FALSE               (1!=1)
#endif

#define SPR '='

char *ini_remove_end_of_line(char *p_line);
char *ini_skip_spaces(char *p_line);
char *ini_skip_separator(char *p_line);
char *ini_read_param(char *p_line, char *p_param, int max_len);
char *ini_read_string(FILE *f, const char *p_template, char *p_out, int max_len, const char *p_default);
char *ini_read_string_file(const char *p_file, const char *p_template, char *p_out, int max_len, const char *p_default);
int ini_read_int(FILE *f, const char *p_template, int dflt);
int ini_read_int_file(const char *p_file, const char *p_template, int dflt);
int ini_read_bool(FILE *f, const char *p_template, int dflt);
int ini_read_bool_file(const char *p_file, const char *p_template, int dflt);
bool ini_write_string(const char *p_file, const char *p_template, const char *p_value);
int is_token(char *p_line, const char *p_token);
int read_token(FILE *f_in, char *p_line, size_t max, char separator);

#endif  // __INI_H__
