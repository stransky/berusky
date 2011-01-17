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

#ifndef __DATA_PARSER_H__
#define __DATA_PARSER_H__

#include <stdio.h>
#include <ctype.h>

#define MAX_POS_NAME 50

typedef struct position_table_item {

  char name[MAX_POS_NAME];
  int  position;

} POSITION_TABLE_ITEM;

typedef struct data_record {

  int base;
  int offset;
  int num;

} DATA_RECORD;

typedef class data_parser {

  // ***********************************************************************
  // Main data
  // ***********************************************************************
  POSITION_TABLE_ITEM *p_table;
  int                  table_items;

  #define MAX_RECORD_LEN 2000
  char                 record[MAX_RECORD_LEN];

  FHANDLE              datafile;


  // ***********************************************************************
  // Helper functions
  // ***********************************************************************
  char * token_get_next(char *p_line)
  {
    while(*p_line && isblank(*p_line))
      *p_line++;
    return(p_line);
  }
  char * token_get_next_end(char *p_line)
  {
    while(*p_line && !isblank(*p_line) && *p_line != '+' && *p_line != '|' && 
          *p_line != '\n' && *p_line != '\r')
      *p_line++;
    return(p_line);
  }

  char * token_translate(char *p_token, int *p_sum, int *p_base);
  int    record_load(void);

public:
  
  data_parser(POSITION_TABLE_ITEM *p_pos_table, int pos_table_items) : 
  p_table(p_pos_table), table_items(pos_table_items) {};
  
  ~data_parser(void);

  int  open(const char *p_file, const char *p_dir);
  void close(void);
  
  int  get_max_index(void);
  void get_indexes(int *p_indexes, int index_num);
  
  int  records_get(DATA_RECORD *p_records, int record_num);

} DATA_PARSER;

#endif // __DATA_PARSER_H__
