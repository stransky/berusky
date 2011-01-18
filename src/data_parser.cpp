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

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

#include "portability.h"
#include "ini.h"
#include "utils.h"
#include "data_parser.h"

#define STATE_TOKEN       1
#define STATE_ADD         2
#define STATE_END         3

#define OPERATION_ADD     1
#define OPERATION_OR      2

#define NO_VALUE          INT_MAX

char * data_parser::token_translate(char *p_token, int *p_sum, int *p_base = NULL)
{  
  int sum = NO_VALUE;
  int base = NO_VALUE;
  int state = STATE_TOKEN;
  int add_operation = OPERATION_ADD;

  while(state != STATE_END) {
  
    p_token = token_get_next(p_token);
  
    if(!p_token || *p_token == '#' || *p_token == '\n' || *p_token == '\r') {
      p_token = NULL;
      break;
    
    } else if(state == STATE_TOKEN) {
    
      char *p_end = token_get_next_end(p_token+1);
      assert(p_end);
      char c = *p_end;
      *p_end = '\0';
      
      if(isdigit((int)(*p_token)) || *p_token == '-') {
        sum = (sum == NO_VALUE) ? atoi(p_token) : (add_operation == OPERATION_ADD ? sum + atoi(p_token) : sum | atoi(p_token));
        state = STATE_ADD;
      } else {
        int i;
        state = STATE_END;
        for(i = 0; i < table_items; i++) {
          if(!strcmp(p_token, p_table[i].name)) {
            sum = (sum == NO_VALUE) ? p_table[i].position : sum + p_table[i].position;
            state = STATE_ADD;
            break;
          }
        }
      
        if(i == table_items) {
          bprintf("Can't find '%s' token!",p_token);
          assert(0);
        }
      }

      if(base == NO_VALUE)
        base = sum;
      *p_end = c;
      
      // skip to next token
      p_token = p_end;
      
    } else if(state == STATE_ADD) {
      if(*p_token == '+') {
        p_token = p_token+1;
        add_operation = OPERATION_ADD;
        state = STATE_TOKEN;
      } else if(*p_token == '|') {
        p_token = p_token+1;
        add_operation = OPERATION_OR;
        state = STATE_TOKEN;
      } else {
        state = STATE_END;
      }
    }
  }

  if(p_base)
    *p_base = base;
  *p_sum = sum;
  return(p_token);
}

int data_parser::records_get(DATA_RECORD *p_records, int record_num)
{ 
  char *p_token;
  int num;
  int base;

  int akt_rec = 0;

  while(!akt_rec && (p_token = fgets(record, MAX_RECORD_LEN, datafile)) && !feof(datafile)) {
    while(p_token && (p_token = token_translate(p_token, &num, &base), num != NO_VALUE) && akt_rec < record_num) {
      p_records->base = base;
      p_records->offset = num - base;
      p_records->num = num;
      p_records++;
      akt_rec++;
    }
  }

  return(akt_rec);
}

void data_parser::get_indexes(int *p_indexes, int index_num)
{
  int base, num;

  memset(p_indexes, 0, sizeof(p_indexes[0])*index_num);

  while(fgets(record, MAX_RECORD_LEN, datafile)) {
    fprintf(stderr, "record = '%s'",record);
    char *p_tmp = token_translate(record, &num, &base);
    if(!p_tmp) {
      continue;
    } else {
      if(base < index_num) {
        if(p_indexes[base] <= (num-base))
          p_indexes[base] = (num-base)+1;
      }
    }
  }

  rewind(datafile);
}

int data_parser::get_max_index(void)
{
  int max = 0, num;

  while(fgets(record, MAX_RECORD_LEN, datafile)) {
    char *p_tmp = token_translate(record, &num);
    if(!p_tmp) {
      continue;
    } else {
      if(max < num)
        max = num;      
    }
  }

  rewind(datafile);

  return(max);
}

int data_parser::open(const char *p_file, const char *p_dir)
{
  close();
  
  datafile = file_open(p_dir,p_file,"r");

  return((bool)datafile);
}

void data_parser::close(void)
{
  if(datafile) {
    fclose(datafile);
    datafile = (void *)NULL;
  }
}

data_parser::~data_parser(void)
{
  close();
}
