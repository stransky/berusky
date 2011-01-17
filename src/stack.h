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

#ifndef __STACK_H__
#define __STACK_H__

#include <assert.h>

#define STACK_DEFAULT_ITEMS 1000

#ifndef NULL
#define NULL (0)
#endif

template<class T> class stack {

  T * p_stack;

  int max_size;
  int act_size;  

public:
  
  stack(void)
  {
    p_stack = NULL;
    max_size = act_size = 0;
  }

private:
  
  void extend(void)
  {
    T * p_tmp = new T[max_size + STACK_DEFAULT_ITEMS];
  
    int i;
    for(i = 0; i < max_size; i++)
      p_tmp[i] = p_stack[i];
    
    max_size += STACK_DEFAULT_ITEMS;
  
    delete [] p_stack;
    p_stack = p_tmp;
  }

public:

  void push(T itm)
  {
    if(act_size + 1 >= max_size) {
      extend();
    }      
    p_stack[act_size++] = itm;
  }

  T pop(void)
  {
    assert(act_size > 0);
    return(p_stack[--act_size]);
  }

  bool is_empty(void)
  {
    return(act_size <= 0);
  }

  ~stack(void)
  {
    if(p_stack) {  
      delete [] p_stack;
      max_size = act_size = 0;
    }
  }
};

#define CIRCLE_STACK_DEFAULT_ITEMS 100

template<class T> class circle_stack {

  T store[CIRCLE_STACK_DEFAULT_ITEMS];

  int first;  // the first used
  int next;   // the next one

private:
  
  int next_index(int index)
  {
    return((index+1 >= CIRCLE_STACK_DEFAULT_ITEMS) ? 0 : index+1);
  }

  int prev_index(int index)
  {
    return((index-1 >= 0) ? index-1 : CIRCLE_STACK_DEFAULT_ITEMS-1);
  }
  
public:

  void clear(void)
  {
    first = next = 0;
  }

  // Is empty when next == first
  bool is_empty(void)
  {
    return(first == next);
  }

  // Is full when next+1 == first
  bool is_full(void)
  {
    return(next_index(next) == first);
  }

public:

  void push(T &itm)
  {
    if(is_full()) {
      first = next_index(first);
    } 
    store[next] = itm;
    next = next_index(next);
  }

  T * push_pointer(void)
  {
    if(is_full()) {
      first = next_index(first);
    } 
  
    T *p_tmp = store+next;
    next = next_index(next);
  
    return(p_tmp);
  }

  T pop(void)
  {
    if(!is_empty()) {
      next = prev_index(next);
      return(store[next]);
    }    
  }

  T * pop_pointer(void)
  {
    if(!is_empty()) {
      next = prev_index(next);
      return(store+next);
    }
    else {
      return(NULL);
    }
  }

  T * top(void)
  {
    if(!is_empty()) {
      return(store+prev_index(next));
    } 
    else {
      return(NULL);
    }
  }

public:

  circle_stack(void)
  {
    clear();
  }
};

#endif
