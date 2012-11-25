/*
 *        .þÛÛþ þ    þ þÛÛþ.     þ    þ þÛÛÛþ.  þÛÛÛþ .þÛÛþ. þ    þ
 *       .þ   Û Ûþ.  Û Û   þ.    Û    Û Û    þ  Û.    Û.   Û Ûþ.  Û
 *       Û    Û Û Û  Û Û    Û    Û   þ. Û.   Û  Û     Û    Û Û Û  Û
 *     .þþÛÛÛÛþ Û  Û Û þÛÛÛÛþþ.  þþÛÛ.  þþÛÛþ.  þÛ    Û    Û Û  Û Û
 *    .Û      Û Û  .þÛ Û      Û. Û   Û  Û    Û  Û.    þ.   Û Û  .þÛ
 *    þ.      þ þ    þ þ      .þ þ   .þ þ    .þ þÛÛÛþ .þÛÛþ. þ    þ
 *
 * AGE (C) AnakreoN
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
 * Linear lists
 */
 
#ifndef __LLIST_H__
#define __LLIST_H__

class llist_item;
typedef class llist_item LLIST_ITEM;
class llist_head;
typedef class llist_head LLIST_HEAD; 

typedef bool (* LLIST_ITEM_COMPARE_FUNC)(void *p_search_data, LLIST_ITEM *p_current);

typedef class llist_item {

  friend class llist_head;
  
  class llist_item *p_next;
  class llist_item *p_prev;
  
public:

  class llist_item * list_append(class llist_item *p_new)
  {
    p_next = p_new;
    if(p_new) {
      p_new->p_prev = this;
    }
    return(this);
  }
  
  void list_remove(void)
  {
    if(p_next) {
      p_next->p_prev = p_prev;
    } 
    if(p_prev) {
      p_prev->p_next = p_next;
    }
  }

  class llist_item * list_next(void)
  {
    return(p_next);
  }
  
public:
  
  llist_item(void) : p_next(NULL), p_prev(NULL) {};
  ~llist_item(void) {};

} LLIST_ITEM;

inline int llist_count(LLIST_ITEM *p_first)
{
  int i;
  for(i = 0; p_first; p_first = p_first->list_next(), i++);
  return(i);
}

typedef class llist_head {
  
  LLIST_ITEM *p_first;
  LLIST_ITEM *p_last;
  
public:

  LLIST_ITEM * list_get_first(void)
  {
    return(p_first);
  }
  LLIST_ITEM * list_get_last(void)
  {
    return(p_last);
  }
  
  LLIST_ITEM * list_insert_first(LLIST_ITEM *p_pol)
  {
    if(p_first) {
      p_pol->list_append(p_first);
      p_pol->p_prev = NULL;
      p_first = p_pol;
    }
    else {
      p_last = p_first = p_pol;
      p_pol->p_next = p_pol->p_prev = NULL;
    }    
    return(p_pol);
  }
  
  LLIST_ITEM * list_insert_last(LLIST_ITEM *p_pol)
  {
    if(p_last) {    
      p_last->list_append(p_pol);
      p_pol->p_next = NULL;
      p_last = p_pol;
    } else {
      p_last = p_first = p_pol;
      p_pol->p_next = p_pol->p_prev = NULL;
    }
    return(p_pol);
  }

  LLIST_ITEM * list_insert(LLIST_ITEM *p_pol)
  {
    return(list_insert_last(p_pol));
  }
  
  void list_remove(LLIST_ITEM *p_pol)
  {
    if(p_pol == p_first)
      p_first = p_pol->p_next;
    
    if(p_pol == p_last)
      p_last = p_pol->p_prev;
      
    p_pol->list_remove();
  }

  LLIST_ITEM * find(LLIST_ITEM *p_searched)
  {
    LLIST_ITEM *p_tmp = list_get_first();
    while(p_tmp) {
      if(p_tmp == p_searched)
        return(p_tmp);
      p_tmp = p_tmp->list_next();
    }
    return(NULL);
  }  
  
  LLIST_ITEM * find(void *p_search_data, LLIST_ITEM_COMPARE_FUNC compare_func)
  {
    LLIST_ITEM *p_tmp = list_get_first();
    while(p_tmp) {
      if(compare_func(p_search_data,p_tmp))
        return(p_tmp);
      p_tmp = p_tmp->list_next();
    }
    return(NULL);
  }  
  
public:

  void list_clear(void)
  {
    p_first = NULL;
    p_last = NULL;
  }
  
  bool is_empty(void)
  {
    return(!p_first && !p_last);
  }
  
public:
  
  llist_head(void) : p_first(NULL), p_last(NULL) {};
  ~llist_head(void) {};
    
} LLIST_HEAD;

#endif //__LLIST_H__
