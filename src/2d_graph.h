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
  2D Graphics library

  TODO:

  - surface list
  - sprite list
  - graf2d class
  - sprite class -> reference to surface list
  
*/

#ifndef  __2D_GRAPH_H__
#define  __2D_GRAPH_H__

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>

#include "portability.h"

#ifdef LINUX
#include <SDL/SDL.h>
#endif

#ifdef WINDOWS
#include "SDL.h"
#endif

#include "ini.h"
#include "utils.h"

#ifndef INSERT_APPEND
#define INSERT_APPEND            (-1)
#endif

#ifndef  TRUE
#define  TRUE  (1==1)
#endif

#ifndef  FALSE
#define  FALSE (1!=1)
#endif

#define  ADJUST_MIN(min, a)   { if((a) < (min)) (min) = (a); }
#define  ADJUST_MAX(max, a)   { if((a) > (max)) (max) = (a); }
#define  ADJUST(min, max, a)  { if((a) < (min)) (min) = (a); if((a) > (max)) (max) = (a); }

#define  INSIDE(base, a, lenght)  (((base) <= (a)) && ((a) < (base+lenght)))
#define  INSIDE_ABS(min, a, max)  (((min) <= (a)) && ((a) <= (max)))

#define  TPOS_MIN       (-INT_MAX)
#define  TPOS_MAX       INT_MAX

#define  SCALE_FACTOR   2
#define  SPRITE_SCREEN  0

#define  BITMAP_FORMAT  ".png"

typedef int             tpos;
typedef unsigned int    tcolor;
typedef int             tcount;
typedef unsigned char   trgbcomp;
typedef unsigned int    tflag;

typedef short int       trec;

typedef SDL_Rect        RECT;
typedef SDL_PixelFormat PIXELFORMAT;

typedef class rgb {

public:

  int r,g,b;

public:

  void norm(void)
  {
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
  
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
  }

  rgb(void) {};
  
  rgb(int r_, int g_, int b_)
  {
    r = r_;
    g = g_;
    b = b_;
  }

} RGB;

void rect_adjust(RECT *p_r, tpos x, tpos y, tpos w, tpos h);
void rect_adjust(RECT *p_r, RECT *p_s);

// -------------------------------------------------------
//   the surface class
// -------------------------------------------------------
typedef enum {

  BLEND_SET = 0,
  BLEND_ADD,
  BLEND_SUB,

} BLEND_OP;

typedef class surface
{
  static char graphics_dir[MAX_FILENAME];

public:

  static void graphics_dir_set(char *p_dir)
  {
    strncpy(graphics_dir,p_dir,MAX_FILENAME);
  }

  static const char * graphics_dir_get(void)
  {
    return(graphics_dir);
  }

private:

  tcount       used;    // number of sprites what use this surface
  SDL_Surface *p_surf;

public:
  
  surface(void);
  surface(char *p_file);  
  surface(SDL_Surface *p_surf_, int used_ = 0);
  surface(class surface *p_src);
  surface(class surface &src);
  surface(tpos width, tpos height, bool display_format = true);
  surface(class surface *p_src, int scale = 1, bool display_format = true);
  surface(class surface *p_src, tpos sx, tpos sy, tpos width, tpos height);
  ~surface(void);

  void create(tpos width, tpos height, bool display_format = true);
  void load(char *p_file);
  void copy(class surface *p_src, RECT *p_src_rect = NULL);
  void free(void);

  void ckey_set(trgbcomp r, trgbcomp g, trgbcomp b);
  void ckey_set(tcolor color);
  void fill(tcolor color);
  void fill(tpos x, tpos y, tpos dx, tpos dy, tcolor color);
  void blit(class surface *p_dst, tpos tx, tpos ty);
  void blit(tpos sx, tpos sy, tpos dx, tpos dy, class surface *p_dst, tpos tx, tpos ty);
  void blend(tpos sx, tpos sy, tpos dx, tpos dy, tcolor color, BLEND_OP operation);

  tcolor color_map(Uint8 r, Uint8 g, Uint8 b)
  {    
    return(SDL_MapRGB(p_surf->format, r, g, b));
  }

  void color_unmap(tcolor color, Uint8 *r, Uint8 *g, Uint8 *b)
  {    
    SDL_GetRGB(color, p_surf->format, r, g, b);    
  }
  
  bool is_loaded(void)
  {
    return(p_surf != NULL);
  }

  void inc_ref(void)
  {
    assert(used >= 0);
    used++;
  }

  void dec_ref(void)
  {
    used--;
    assert(used >= 0);
  }

  void size_get(tpos *p_dx, tpos *p_dy)
  {
    assert(p_surf);
    *p_dx = p_surf->w;
    *p_dy = p_surf->h;
  }

  SDL_Surface * surf_get(void)
  {
    return(p_surf);
  }

  tpos width_get(void)
  {
    assert(p_surf);
    return(p_surf->w);
  }

  tpos height_get(void)
  {
    assert(p_surf);
    return(p_surf->h);
  }

  tcount used_get(void)
  {      
    return(used);
  }

  RECT rect_get(void)
  {
    assert(p_surf);
    RECT r = {0,0,(Uint16)p_surf->w,(Uint16)p_surf->h};
    return(r);
  }

  // Double-size from the source surface
  void scale(class surface *p_src, tpos src_x, tpos src_y,
             tpos width, tpos height, tpos dst_x, tpos dst_y);

  // Swithches content of this and given surface
  void content_switch(class surface *p_new);
  
} SURFACE;

// -------------------------------------------------------
//   the sprite class
// -------------------------------------------------------

#define SDL_SPRITE_RECT             0x1     // Use rectangle
#define SDL_SPRITE_SEPARATE_SURFACE 0x2     // Surface isn't in Surface-list
#define SDL_SPRITE_USED             0x4     // for sprite_store

typedef class sprite
{
  tflag         flag;                   // Sprite flags
  RECT          rec;                    // Rectangle in destination surface
  SURFACE      *p_surf;                 // Surface for this sprite
  static RGB    key;                    // Color key

public:

  sprite(void);
  sprite(SURFACE *p_surf_, tflag flag_ = 0, RECT *p_rect = NULL);
  sprite(class sprite &src);
  ~sprite(void);

  void load(SURFACE *p_surf_, tflag flag_ = 0, RECT *p_rect = NULL);
  void free(void);

  void fill(tcolor color);
  void fill(tpos x, tpos y, tpos dx, tpos dy, tcolor color);
  
  void blit(class sprite *p_dst, tpos tx, tpos ty);
  void blit(tpos sx, tpos sy, tpos dx, tpos dy, class sprite *p_dst, tpos tx, tpos ty);

  tcolor color_map(Uint8 r, Uint8 g, Uint8 b)
  {
    return(p_surf->color_map(r,g,b));
  }
  
  static void color_key_set(RGB key_)
  {
    key = key_;
  }

  static RGB color_key_get(void)
  {
    return(key);
  }

  void color_key_apply(void)
  {
    if(p_surf)
      p_surf->ckey_set(key.r,key.g,key.b);
  }

  void flag_set(tflag flag_)
  {
    flag |= flag_;
  }

  void flag_clear(tflag flag_)
  {
    flag &= ~flag_;
  }

  bool flag_get(tflag flag_)
  {
    return((bool)(flag&flag_));
  }

  tflag flag_get(void)
  {
    return(flag);
  }
  
  tpos get_width(void)
  {
    return((tpos)rec.w);
  }

  tpos get_height(void)
  {
    return((tpos)rec.h);
  }

  SURFACE * surf_get(void)
  {
    return(p_surf);
  }
  
  RECT * rect_get(void)
  {
    return(&rec);
  }

  void rect_set(RECT *r)
  {
    rec = *r;
  }

  void rect_clamp(RECT *p_rect);
  void rect_check(void);

} SPRITE;

// -------------------------------------------------------
//   the sprite store class
// -------------------------------------------------------

typedef int spr_handle;
typedef int surf_handle;

typedef class sprite_store
{

  surf_handle  surface_num;
  surf_handle  surface_last;
  SURFACE     *p_surfaces;

  spr_handle   sprite_num;
  spr_handle   sprite_last;
  SPRITE      *p_sprites;

public:

  sprite_store(surf_handle surf_num, spr_handle spr_num);
  ~sprite_store(void);

  /********************************************************
    Surface interface
    *******************************************************
  */
  SURFACE * get_surface_interface(void)
  {
    return(p_surfaces);
  }
  surf_handle get_surface_num(void)
  {
    return(surface_last);
  }

  surf_handle surface_insert(char *p_file)
  {
    assert(surface_last < surface_num);
    p_surfaces[surface_last].load(p_file);
    return(surface_last++);
  }

  surf_handle surface_copy(SURFACE *p_surf, RECT *p_src_rec = NULL)
  {
    assert(surface_last < surface_num);
    p_surfaces[surface_last].copy(p_surf, p_src_rec);
    return(surface_last++);
  }

  surf_handle surface_copy(surf_handle src_handle, RECT *p_src_rec = NULL)
  {
    return(surface_copy(p_surfaces + src_handle, p_src_rec));
  }

  SURFACE * surface_get(surf_handle handle)
  {
    return(p_surfaces+handle);
  }

  // Switch content of the given surface with the new one
  SURFACE * surface_switch(surf_handle handle, SURFACE *p_new)
  {
    p_surfaces[handle].content_switch(p_new);
    return(p_surfaces+handle);
  }

  /********************************************************
    Sprite interface
    *******************************************************
  */

  spr_handle sprite_insert(const char *p_file, spr_handle first, spr_handle *p_last = NULL);
  spr_handle sprite_insert(surf_handle sf_handle, tflag flag_ = 0, RECT *p_rect = NULL, spr_handle first = INSERT_APPEND);
  spr_handle sprite_insert(SURFACE *p_surf, tflag flag_ = 0, RECT *p_rect = NULL, spr_handle first = INSERT_APPEND);
  spr_handle sprite_insert(SPRITE *p_spr, int num = 1, spr_handle first = INSERT_APPEND);
  spr_handle sprite_copy(spr_handle dst_handle, spr_handle src_handle, bool copy_surface = false);
  void       sprite_delete(spr_handle handle, int num = 1);

  void       sprite_flag_set(tflag flag, spr_handle first, spr_handle num = 1);
  void       sprite_flag_clear(tflag flag, spr_handle first, spr_handle num = 1);

  SPRITE * sprite_interface_get(void)
  {
    return(p_sprites);
  }

  SPRITE * sprite_get(spr_handle handle)
  {
    assert(handle < sprite_num);
    return(p_sprites+handle);
  }

  RECT * sprite_get_rect(spr_handle handle)
  {
    assert(handle < sprite_num);
    return(p_sprites[handle].rect_get());
  }

  tpos sprite_get_width(spr_handle handle)
  {
    RECT *p_rect = p_sprites[handle].rect_get();
    return(p_rect->w);
  }

  tpos sprite_get_height(spr_handle handle)
  {
    RECT *p_rect = p_sprites[handle].rect_get();
    return(p_rect->h);
  }

  spr_handle sprite_num_get(void)
  {
    return(sprite_last);
  }
    
} SPRITE_STORE;

// -------------------------------------------------------
//   the graph_2d store class
// -------------------------------------------------------

#define SURFACES 1000
#define SPRITES  100000
#define RECT_NUM      (2*800)
#define RECT_NUM_ALERT 800

typedef class graph_2d
{
  int          redraw;

  SURFACE     *p_screen_surface;
  SPRITE      *p_screen;

  // Array of surfaces
  SPRITE_STORE store;

  // Update rectangles (max number is X_cells * Y_cells)
  RECT         rects[RECT_NUM];
  int          rect_last;
  bool         rect_whole;

  tpos         graphics_width;
  tpos         graphics_height;
  int          graphics_bpp;
  bool         graphics_fullscreen;
  int          sdl_video_flags;

public:
  
  void graphics_dir_set(char *p_dir)
  {
    surface::graphics_dir_set(p_dir);
  }

  const char * graphics_dir_get(void)
  {
    return(surface::graphics_dir_get());
  }

public:

  SPRITE_STORE * store_get(void)
  {
    return(&store);
  }

  SPRITE * sprite_get(spr_handle spr = 0)
  {    
    return(spr ? store.sprite_get(spr) : p_screen);
  }

  tpos sprite_get_width(spr_handle spr = 0)
  {    
    return(spr ? store.sprite_get_width(spr) : p_screen->get_width());
  }

  tpos sprite_get_height(spr_handle spr = 0)
  {
    return(spr ? store.sprite_get_height(spr) : p_screen->get_height());
  }

  tpos sprite_get_width_center(spr_handle spr)
  {
    assert(spr);    
    return((p_screen->get_width() - store.sprite_get_width(spr)) / 2);
  }

  tpos sprite_get_height_center(spr_handle spr)
  {
    assert(spr);    
    return((p_screen->get_height() - store.sprite_get_height(spr)) / 2);
  }

  spr_handle sprite_insert(const char *p_file, spr_handle first, spr_handle *p_last = NULL)
  {
    return(store.sprite_insert(p_file,first,p_last));
  }

  spr_handle sprite_insert(SPRITE *p_spr, spr_handle first = INSERT_APPEND)
  {
    return(store.sprite_insert(p_spr,first));
  }

  spr_handle sprite_copy(spr_handle dst_handle, spr_handle src_handle, bool surface_copy = false)
  {
    return(store.sprite_copy(dst_handle, src_handle, surface_copy));
  }

  void sprite_delete(spr_handle handle, int num = 1)
  {
    store.sprite_delete(handle, num);
  }

  void fill(tcolor color, spr_handle dst = 0)
  {      
    SPRITE *p_dst = dst ? store.sprite_get(dst) : p_screen;
    if(p_dst)
       p_dst->fill(color);
  }

  void fill(tpos x, tpos y, tpos dx, tpos dy, tcolor color, spr_handle dst = 0)
  {        
    SPRITE *p_dst = dst ? store.sprite_get(dst) : p_screen;
    if(p_dst)
       p_dst->fill(x,y,dx,dy,color);
  }

  void fill(RECT *p_rect, tcolor color, spr_handle dst = 0)
  {
    fill(p_rect->x, p_rect->y, p_rect->w, p_rect->h, color, dst);
  }

  void fill(spr_handle dst = 0)
  {
    fill(0, 0, p_screen->get_width(), p_screen->get_height(), 0, dst);
  }

  void clear(spr_handle dst = 0)
  {
    fill(dst);
    if(dst == 0)
      redraw_add();
  }

  tcolor color_map(Uint8 r, Uint8 g, Uint8 b, spr_handle dst = 0)
  {
    SPRITE *p_dst = dst ? store.sprite_get(dst) : p_screen;
    return(p_dst->color_map(r,g,b));
  }

  // Draw surface to screen or another surface
  void draw(surface *p_src, tpos tx, tpos ty, surface *p_dst = NULL)
  {    
    SURFACE *p_dest = p_dst ? p_dst : p_screen_surface;
    assert(p_src && p_dest);
    p_src->blit(p_dest, tx, ty);
  }

  void draw(spr_handle spr, tpos tx, tpos ty, spr_handle dst = 0)
  {
    SPRITE *p_spr = store.sprite_get(spr);
    SPRITE *p_dst = dst ? store.sprite_get(dst) : p_screen;
    if(p_spr && p_dst)
       p_spr->blit(p_dst,tx,ty);
  }

  void draw(spr_handle spr, tpos sx, tpos sy, tpos dx, tpos dy, 
            spr_handle dst, tpos tx, tpos ty)
  {
    SPRITE *p_spr = store.sprite_get(spr);
    SPRITE *p_dst = dst ? store.sprite_get(dst) : p_screen;
    if(p_spr && p_dst)
       p_spr->blit(sx, sy, dx, dy, p_dst, tx, ty);
  }

  bool fullscreen_get(void)
  {
    return(graphics_fullscreen);
  }

  void fullscreen_toggle(void);

  SPRITE * screen_get(void)
  {
    return(p_screen);
  }

  SURFACE * screen_surface_get(void)
  {
    return(p_screen_surface);
  }

  void clamp_to_screen(RECT *p_rect)
  {
    p_screen->rect_clamp(p_rect);
  }

  void redraw_set(int red_) 
  {
    redraw = red_; 
  }

  int  redraw_get(void) 
  { 
    return redraw; 
  }

  void redraw_add(trec x, trec y, trec dx, trec dy)
  {
    assert(rect_last < RECT_NUM);
  
    if(!dx || !dy)
      return;
  
    RECT *p_tmp = rects+rect_last++;
  
    p_tmp->x = x;
    p_tmp->y = y;
  
    p_tmp->w = dx;
    p_tmp->h = dy;

    p_screen->rect_clamp(p_tmp);
  }

  void redraw_add(spr_handle spr, trec x, trec y)
  {
    assert(rect_last < RECT_NUM);
  
    RECT *p_tmp = rects+rect_last++;
  
    p_tmp->x = x;
    p_tmp->y = y;
  
    p_tmp->w = sprite_get_width(spr);
    p_tmp->h = sprite_get_height(spr);

    p_screen->rect_clamp(p_tmp);
  }

  void redraw_add(RECT *p_rect)
  {
    assert(rect_last < RECT_NUM);
  
    if(!p_rect->w || !p_rect->h)
      return;
  
    rects[rect_last++] = *p_rect;
    p_screen->rect_clamp(p_rect);
  }

  void redraw_add(void)
  {
    rect_whole = TRUE;
  }

  void redraw_reset(void)
  {
    rect_last = 0;
    redraw = false;  
  }

  int flip(void)
  {
    if(rect_last || rect_whole) {
      if(rect_last > RECT_NUM_ALERT)
        bprintf("RECT_NUM_ALERT: %d slots.", rect_last);
      if(rect_whole) {
        SDL_UpdateRect(p_screen_surface->surf_get(), 0, 0, 0, 0);
        rect_whole = FALSE;
      } else {
        SDL_UpdateRects(p_screen_surface->surf_get(), rect_last, rects);
        redraw_reset();
      }
    }
    return (TRUE);
  }

  void screen_create(int flag, int width, int height, int bpp, int fullscreen);
  void screen_destroy(void);
  void screen_resize(int width, int height);
  bool screen_regenerate(void);

  void check(void);

  graph_2d(tpos dx, tpos dy, int depth, bool fullscreen) 
    : store(SURFACES, SPRITES), 
      rect_last(0)
  {
    /* sdl init */
    bprintf("SDL Init...");
    SDL_Init(SDL_INIT_VIDEO);

     /* Create new screen */
    bprintf("Seting up screen %dx%d, color depth %d bits, fullscreen = %d...",dx, dy, depth, fullscreen);
    screen_create(0, dx, dy, depth, fullscreen);
  }

  ~graph_2d(void)
  {
    SDL_Quit();
  }

} GRAPH_2D;

// -------------------------------------------------------
//   Font interface
// -------------------------------------------------------

#define FONT_NUM                  3

typedef class font_lookup_table {

  int position[256];

public:
  bool load(char *p_file);
  int  lookup(int c)
  {
    assert(c >= 0 && c < 256);
    return(position[c]);
  }

} FONT_LOOKUP_TABLE;

typedef enum _MENU_TYPE {

  MENU_LEFT,
  MENU_CENTER,
  MENU_RIGHT,

} MENU_TYPE;

#define DEFAULT_NEW_LINE_SHIFT  4

typedef class font_info {

  spr_handle        font_sprite_first;
  spr_handle        font_sprite_num;

  FONT_LOOKUP_TABLE ftable;

  int               extra_height;

public:

  static GRAPH_2D  *p_grf;

public:
  
  font_info(void);
  ~font_info(void);

  bool load(int font_index, int first, int num);
  void free(void);

  int  width_get(char c)
  {
    return(p_grf->sprite_get_width(font_sprite_first+ftable.lookup(c)));
  }

  int  width_get(char *p_string)
  { 
    tpos width = 0;  
  
    while(*p_string) {
      width += width_get(*p_string++);
    }
  
    return(width);
  }

  int  height_get(char c = 'A')
  {
    return(p_grf->sprite_get_height(font_sprite_first+ftable.lookup(c)));
  }

  int  height_get(char *p_string)
  {
    return(height_get(p_string[0]));
  }

  int  height_get_new_line(char c)
  {
    return(p_grf->sprite_get_height(font_sprite_first+ftable.lookup(c))+extra_height);
  }
    
  int  height_get_new_line(char *p_string)
  {
    return(height_get_new_line(p_string[0]));
  }

  void height_set_new_line(int height = -1)
  {
    if(height == -1)
      extra_height = DEFAULT_NEW_LINE_SHIFT;
    else
      extra_height = height;
  }

  int  height_get_new_line(void)
  {
    return extra_height;
  }

  tpos print(int c, tpos px, tpos py, bool draw = TRUE)
  {  
    spr_handle spr = font_sprite_first+ftable.lookup(c);
    if(draw)
      p_grf->draw(spr, px, py);
    return(p_grf->sprite_get_width(spr));
  }

  tpos screen_width_get(void)
  {
    return(p_grf->sprite_get_width());
  }

  tpos screen_height_get(void)
  {
    return(p_grf->sprite_get_height());
  }

} FONT_INFO;

#define FONT_DEFAULT      0
#define FONT_SELECTED     1

typedef class font {

  int               font_selected;
  FONT_INFO         finfo[FONT_NUM];

  MENU_TYPE         align;

  // Start x,y
  tpos              sx,sy;
  // Actuall x,y
  tpos              ax,ay;

  // offset for print functions
  tpos              offset_x;
  tpos              offset_y;

  // Don't print anything, just return requested rectangle
  bool              try_run;

public:

  font(GRAPH_2D *p_graph);
  ~font(void);

public:
  
  int width_get(char c)
  {
    return(finfo[font_selected].width_get(c));
  }
  
  int height_get(char c = 'A')
  {
    return(finfo[font_selected].height_get(c));
  }

  int height_get_new_line(char c)
  {
    return(finfo[font_selected].height_get_new_line(c));
  }

  void extra_height_set(int height = -1, int font_index = -1)
  {
    if(font_index == -1)
      font_index = font_selected;
    finfo[font_index].height_set_new_line(height);
  }

  int  extra_height_get(int font_index = -1)
  {
    if(font_index == -1)
      font_index = font_selected;
    return(finfo[font_index].height_get_new_line());
  }

  int width_get(char *p_string)
  { 
    return(finfo[font_selected].width_get(p_string));
  }
  
  int height_get(char *p_string)
  { 
    return(finfo[font_selected].height_get(p_string));
  }

  int height_get_new_line(char *p_string)
  { 
    return(finfo[font_selected].height_get_new_line(p_string));
  }
  
  void start_set(tpos x, tpos y)
  {
    ax = sx = x; ay = sy = y;
  }

  void offset_set(tpos x, tpos y)
  {
    offset_x = x; offset_y = y;
  }
  
  void alignment_set(MENU_TYPE al)
  {
    align = al;  
  }
  
  void new_line(void)
  {
    ax = sx; ay += height_get_new_line('A');
  }

  void select(int font_index)
  {
    assert(font_index >= 0 && font_index < FONT_NUM);
    font_selected = font_index;
  }
  
  bool load(int font_index, int first, int num)
  {
    return(finfo[font_index].load(font_index,first,num));
  }  

  void free(int font_index)
  {
    finfo[font_index].free();
  }

  void try_run_set(bool try_)
  {
    try_run = try_;
  }

  void print(char *p_string, RECT *p_res = NULL, int lines = 0);
  void print(RECT *p_res, const char *p_text,...);
  void print(RECT *p_res, tpos x, tpos y, const char *p_text,...);
  void print(RECT *p_res, tpos x, tpos y, int lines, const char *p_text,...);  

} FONT;

// -------------------------------------------------------
//   Global interfaces
// -------------------------------------------------------

extern GRAPH_2D *p_grf;
extern FONT     *p_font;

void graphics_start(tpos dx, tpos dy, int depth, bool fullscreen);
void graphics_stop(void);

#define COLOR_MAP(r,g,b)  (p_grf->color_map(r,g,b))

#endif //__2D_GRAPH_H__
