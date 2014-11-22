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
*/
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <math.h>

#include "berusky.h"


/*
  Some code of this file comes from
  http://www.libsdl.org/cgi/docwiki.cgi/Introduction_to_SDL_Video
*/

/*
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
Uint32 getpixel(SDL_Surface *surf, int x, int y)
{
    int bpp = surf->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surf->pixels + y * surf->pitch + x * bpp;

    switch (bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    } // switch
}

bool getpixel(SDL_Surface *p_surf, int x, int y, RGB *p_color)
{
  Uint8 r,g,b;
  SDL_GetRGB(getpixel(p_surf, x, y), p_surf->format, &r, &g, &b);
  RGB key = sprite::color_key_get();
  if(r != key.r || g != key.g || b != key.b) {
    // it's not a transparent color - return it
    p_color->r = r;
    p_color->g = g;
    p_color->b = b;
    return(TRUE);
  }
  return(FALSE);
}

/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void putpixel(SDL_Surface *surf, int x, int y, Uint32 pixel)
{
    int bpp = surf->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surf->pixels + y * surf->pitch + x * bpp;

    switch (bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;

   default:
        break;           /* shouldn't happen, but avoids warnings */
    } // switch
}

bool putpixel(SDL_Surface *p_surf, int x, int y, RGB color)
{
  Uint8 r,g,b;

  r = color.r;
  g = color.g;
  b = color.b;

  putpixel(p_surf, x, y, SDL_MapRGB(p_surf->format, r, g, b));
  return(TRUE);
}

static RGB interpolate(RGB *p_color, int num, int *p_hits)
{
  int i, hits;

  RGB color = *p_color;

  for(i = 1, hits = 1; i < num; i++, p_color++) {
    if(p_hits[i]) {
      color.r += p_color->r;
      color.g += p_color->g;
      color.b += p_color->b;
      hits++;
    }
  }
  color.r /= hits;
  color.g /= hits;
  color.b /= hits;

  return(color);
}

static RGB interpolate(RGB c1, RGB c2)
{
  RGB color((c1.r+c2.r)/2, (c1.g+c2.g)/2, (c1.b+c2.b)/2);
  return(color);
}

// -------------------------------------------------------
//   the surface class
// -------------------------------------------------------
char surface::graphics_dir[MAX_FILENAME] = "";

void surface::load(char *p_file)
{
  char file[MAX_FILENAME];

  return_path(graphics_dir_get(), p_file, file, MAX_FILENAME);

  SDL_Surface *p_tmp = IMG_Load(file);
  if(p_tmp)
  {
    p_surf = SDL_DisplayFormat(p_tmp);
    assert(p_surf);
    SDL_FreeSurface(p_tmp);        
  } 
  else {
    bprintf("Unable to load %s",file);
    assert(p_tmp);
    p_surf = NULL;
  }     
  used = 0;
}

void surface::create(tpos width, tpos height, bool display_format)
{
  assert(!p_surf);

  p_surf = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, 32,0,0,0,0);
  if(!p_surf) {
    berror("Unable to create surface! (%dx%d)", width, height);
  }

  if(display_format) {
    SDL_Surface *p_tmp = SDL_DisplayFormat(p_surf);
    assert(p_tmp);
    SDL_FreeSurface(p_surf);
    p_surf = p_tmp;
  }

  used = 0;
}

void surface::copy(class surface *p_src, RECT *p_src_rect)
{
  assert(p_src->p_surf);
  if(p_surf) {
    SDL_FreeSurface(p_surf);
    p_surf = NULL;
  }  

  if(p_src_rect) {
    SDL_PixelFormat *p_format = p_src->p_surf->format;
    p_surf = SDL_CreateRGBSurface(SDL_HWSURFACE, p_src_rect->w, p_src_rect->h,
                                  p_format->BitsPerPixel,
                                  p_format->Rmask,
                                  p_format->Gmask,
                                  p_format->Bmask,
                                  p_format->Amask);
    if(!p_surf) {
      berror("Unable to create surface! (%dx%d)", p_src_rect->w, p_src_rect->h);
    }
    SDL_BlitSurface(p_src->p_surf, p_src_rect, p_surf, NULL);
  }
  else {
    p_surf = SDL_ConvertSurface(p_src->p_surf, p_src->p_surf->format, SDL_HWSURFACE);
  }
  assert(p_surf);

  used = 0;
}

void surface::free(void)
{
  assert(used == 0);

  if(p_surf) {
    SDL_FreeSurface(p_surf);
    p_surf = NULL;
  }
}

surface::surface(void)
: used(0), p_surf(NULL)
{
}

surface::surface(char *p_file)
{
  load(p_file);
}

surface::surface(SDL_Surface *p_surf_, int used_)
{
  p_surf = p_surf_;
  used = used_;    
}

surface::surface(class surface *p_src)
{
  copy(p_src);
}

surface::surface(class surface &src)
{
  copy(&src);
}

surface::surface(tpos width, tpos height, bool display_format)
: used(0), p_surf(NULL)
{  
  create(width, height, display_format);
}

surface::surface(class surface *p_src, tpos sx, tpos sy, tpos width, tpos height)
: used(0), p_surf(NULL)
{ 
  RECT src = {sx,sy,width,height};
  copy(p_src, &src);
}

surface::surface(class surface *p_src, int scale, bool display_format)
: used(0), p_surf(NULL)
{  
  create(p_src->width_get()*scale, p_src->height_get()*scale, display_format);
}

surface::~surface(void)
{ 
  free();
}

// Color-key set
void surface::ckey_set(trgbcomp r, trgbcomp g, trgbcomp b)
{
  assert(p_surf);
  int ret = SDL_SetColorKey(p_surf, SDL_SRCCOLORKEY, SDL_MapRGB(p_surf->format, r, g, b));
  assert(ret != -1);
}

void surface::ckey_set(tcolor color)
{
  assert(p_surf);
  int ret = SDL_SetColorKey(p_surf, SDL_SRCCOLORKEY, color);
  assert(ret != -1);
}

void surface::fill(tcolor color)
{ 
  assert(p_surf);
  SDL_FillRect(p_surf, NULL, color);
}

void surface::fill(tpos x, tpos y, tpos dx, tpos dy, tcolor color)
{ 
  assert(p_surf);
  SDL_Rect rec = { x, y, dx, dy };
  SDL_FillRect(p_surf, &rec, color);
}

// blit whole source surtace to destination surface
void surface::blit(class surface *p_dst, tpos tx, tpos ty)
{
  assert(p_surf);
  SDL_Rect dst_rec = {tx,ty,p_surf->w,p_surf->h};
  SDL_BlitSurface(p_surf, NULL, p_dst->p_surf, &dst_rec);
}

// blit part of this surface to a destination (target) surface
void surface::blit(tpos sx, tpos sy, tpos dx, tpos dy, class surface *p_dst, tpos tx, tpos ty)
{
  assert(p_surf);
  SDL_Rect src_rec = {sx,sy,dx,dy};
  SDL_Rect dst_rec = {tx,ty,dx,dy};
  SDL_BlitSurface(p_surf, &src_rec, p_dst->p_surf, &dst_rec);  
}

void surface::blend(tpos sx, tpos sy, tpos dx, tpos dy, tcolor color, BLEND_OP operation)
{
  // Lock the surface
  if(SDL_MUSTLOCK(p_surf) ) {
    if(SDL_LockSurface(p_surf) < 0 ) {
      berror("Can't lock surface: %s\n", SDL_GetError());      
    }
  }

  assert(sx+dx <= width_get());
  assert(sy+dy <= height_get());

  int   x,y;
  int   width = sx+dx;
  int   height = sy+dy;
  RGB   color_rgb;
  RGB   pixel;

  byte  r,g,b;
  color_unmap(color, &r, &g, &b);
  color_rgb.r = r;
  color_rgb.g = g;
  color_rgb.b = b;

  // Adjust original pixels
  for(y = sy; y < height; y++) {
    for(x = sx; x < width; x++) {
      if(getpixel(p_surf, x, y, &pixel)) {
        switch(operation) {
          case BLEND_SET:
            pixel = color_rgb;
            break;
          case BLEND_ADD:
            pixel.r += color_rgb.r;
            pixel.g += color_rgb.g;
            pixel.b += color_rgb.b;
            pixel.norm();
            break;
          case BLEND_SUB:
            pixel.r -= color_rgb.r;
            pixel.g -= color_rgb.g;
            pixel.b -= color_rgb.b;

            int rn = (int)floor(((float)rand()/RAND_MAX)*10);
            pixel.r -= rn;
            pixel.g -= rn;
            pixel.b -= rn;          
            pixel.norm();
            break;
        }
        putpixel(p_surf, x, y, pixel);
      }
    }
  }

  if(SDL_MUSTLOCK(p_surf)) {
    SDL_UnlockSurface(p_surf);
  }
}

// Double scale from the source surface
// (src_x, src_y) are coordinates in source surface
// (width, height) is a size of scaled rectangle in source surface
// (dst_x, dst_y) are coordinates in target surface
void surface::scale(class surface *p_src, tpos src_x, tpos src_y, 
                    tpos width, tpos height, tpos dst_x, tpos dst_y)
{
  SDL_Surface *p_src_surf = p_src->p_surf;

  // Lock the surface
  if(SDL_MUSTLOCK(p_src_surf) ) {
    if(SDL_LockSurface(p_src_surf) < 0 ) {
      berror("Can't lock surface: %s\n", SDL_GetError());      
    }
  }

  // Lock the surface
  if(SDL_MUSTLOCK(p_surf) ) {
    if(SDL_LockSurface(p_surf) < 0 ) {
      berror("Can't lock surface: %s\n", SDL_GetError());      
    }
  }

  assert(src_x+width <= p_src->width_get());
  assert(src_y+height <= p_src->height_get());
  assert(dst_x+2*width <= width_get());
  assert(dst_y+2*height <= height_get());

  int   x,y;  
  // Copy & interpolate original pixels
  for(y = 0; y < height; y++) {
    for(x = 0; x < width; x++) {
      RGB   color[4];
      int   hits[4];
    
      /* Pixel positions:
        0 | 1
        -----
        2 | 3
      */
      if(!(hits[0] = getpixel(p_src_surf, src_x+x, src_y+y, color))) {
        // It's a transparent color - just replicate it
        RGB transparent = sprite::color_key_get();
        putpixel(p_surf, dst_x+2*x,  dst_y+2*y,  transparent);
        putpixel(p_surf, dst_x+2*x+1,dst_y+2*y,  transparent);
        putpixel(p_surf, dst_x+2*x,  dst_y+2*y+1,transparent);
        putpixel(p_surf, dst_x+2*x+1,dst_y+2*y+1,transparent);
        continue;
      }
      
      hits[1] = (x+1 < width && getpixel(p_src_surf, src_x+x+1, src_y+y, color+1));
      hits[2] = (y+1 < height && getpixel(p_src_surf, src_x+x, src_y+y+1, color+2));
      hits[3] = (x+1 < width && y+1 < height && getpixel(p_src_surf, src_x+x+1, src_y+y+1, color+3));

      // 0
      putpixel(p_surf, dst_x+2*x, dst_y+2*y, color[0]);
      
      // 1
      putpixel(p_surf, dst_x+2*x+1, dst_y+2*y, hits[1] ? interpolate(color[0], color[1]) : color[0]);
    
      // 2
      putpixel(p_surf, dst_x+2*x, dst_y+2*y+1, hits[2] ? interpolate(color[0], color[2]) : color[0]);

      // 3
      putpixel(p_surf, dst_x+2*x+1, dst_y+2*y+1, interpolate(color, 4, hits));
    }
  }

  if(SDL_MUSTLOCK(p_src_surf)) {
    SDL_UnlockSurface(p_src_surf);
  }
  if(SDL_MUSTLOCK(p_surf)) {
    SDL_UnlockSurface(p_surf);
  }
}

// Switch content of surfaces
void surface::content_switch(class surface *p_new)
{
  SDL_Surface *p_tmp = p_new->p_surf;
  p_new->p_surf = p_surf;
  p_surf = p_tmp;
}

// -------------------------------------------------------
//   the sprite class
// -------------------------------------------------------

RGB  sprite::key;

void sprite::rect_check(void)
{
  if((flag&SDL_SPRITE_RECT) && p_surf) {
    assert(rec.x+rec.w <= p_surf->width_get());
    assert(rec.y+rec.h <= p_surf->height_get());
  }  
}

void sprite::load(SURFACE *p_surf_, tflag flag_, RECT *p_rect)
{
  flag = flag_;
  p_surf = p_surf_;

  if(p_rect) {
    rec = *p_rect;
    flag |= SDL_SPRITE_RECT;
  } else {
    if(p_surf)
      rec = p_surf->rect_get();
  }

  if(p_surf)
    p_surf->inc_ref();
  
  rect_check();
}

void sprite::free(void)
{
  if(p_surf)
    p_surf->dec_ref();

  flag = 0;
  p_surf = NULL;
}

sprite::sprite(void)
:flag(0), p_surf(NULL)
{
}

sprite::sprite(SURFACE *p_surf_, tflag flag_, RECT *p_rect)
{
  load(p_surf_, flag_,p_rect);
}

sprite::sprite(class sprite &src)
{
  flag = src.flag;
  rec = src.rec;
  p_surf = src.p_surf;
  if(p_surf)
    p_surf->inc_ref();
  rect_check();
}

sprite::~sprite(void)
{
  flag = 0;

  if(p_surf)
    p_surf->dec_ref();
}

void sprite::fill(tcolor color)
{ 
  assert(p_surf);
  if(flag&SDL_SPRITE_RECT)
    p_surf->fill(rec.x,rec.y,rec.w,rec.h,color);
  else
    p_surf->fill(color);
}

void sprite::fill(tpos x, tpos y, tpos dx, tpos dy, tcolor color)
{    
  assert(p_surf);
  if(flag&SDL_SPRITE_RECT) {
    p_surf->fill(rec.x + x, rec.y + y,
                 dx > rec.w ? rec.w : dx,
                 dy > rec.h ? rec.h : dy,
                 color);
  } else {
    p_surf->fill(x,y,dx,dy,color);
  }
}

// blit whole source sprite to destination sprite
void sprite::blit(class sprite *p_dst, tpos tx, tpos ty)
{
  SDL_Rect *p_src_rec;      
  SDL_Rect dst_rec;
  
  assert(p_surf && p_dst->p_surf);

  p_src_rec = (flag&SDL_SPRITE_RECT) ? &rec : NULL;

  if(p_dst->flag&SDL_SPRITE_RECT) {
    dst_rec.x = tx + p_dst->rec.x;
    dst_rec.y = ty + p_dst->rec.y;
  } else {
    dst_rec.x = tx;
    dst_rec.y = ty;
  }
      
  if(p_src_rec) {
    dst_rec.w = p_src_rec->w;
    dst_rec.h = p_src_rec->h;
  } else {        
    dst_rec.w = p_surf->width_get();
    dst_rec.h = p_surf->height_get();
  }
  
  SDL_BlitSurface(p_surf->surf_get(), p_src_rec, p_dst->p_surf->surf_get(), &dst_rec);
  //bprintf("Sprit [%d,%d -> %d,%d]",p_src_rec->x,p_src_rec->y,p_src_rec->w,p_src_rec->h);
}

// blit part of source sprite to destination sprite
void sprite::blit(tpos sx, tpos sy, tpos dx, tpos dy, class sprite *p_dst, tpos tx, tpos ty)
{
  SDL_Rect src_rec = {sx, sy, dx, dy};
  SDL_Rect dst_rec;
  
  assert(p_surf && p_dst->p_surf);

  if(p_dst->flag&SDL_SPRITE_RECT) {
    dst_rec.x = tx + p_dst->rec.x;
    dst_rec.y = ty + p_dst->rec.y;
  } else {
    dst_rec.x = tx;
    dst_rec.y = ty;
  }
      
  dst_rec.w = dx;
  dst_rec.h = dy;
  
  SDL_BlitSurface(p_surf->surf_get(), &src_rec, p_dst->p_surf->surf_get(), &dst_rec);
}

/* 
*/
void sprite::rect_clamp(RECT *p_rect)
{
  tpos width = p_rect->w;
  tpos height = p_rect->h;

  if(p_rect->x < 0) {
    width += p_rect->x;
    p_rect->x = 0;
    if(width < 0)
      width = 0;
  }
  if(p_rect->y < 0) {
    height += p_rect->y;
    p_rect->y = 0;
    if(height < 0)
      height = 0;
  }
  if(p_rect->x+width > rec.w) {
    assert(p_rect->x < rec.w && rec.x == 0);
    width = rec.w-p_rect->x;
  }
  if(p_rect->y+height > rec.h) {
    assert(p_rect->y < rec.h && rec.y == 0);
    height = rec.h-p_rect->y;
  }
  p_rect->w = width;
  p_rect->h = height;

  assert(width > 0 && height > 0);
  assert(p_rect->x+p_rect->w >= 0 && p_rect->x+p_rect->w <= rec.w);
  assert(p_rect->y+p_rect->h >= 0 && p_rect->y+p_rect->h <= rec.h);
}


// -------------------------------------------------------
//   the sprite store class
// -------------------------------------------------------

sprite_store::sprite_store(surf_handle surf_num, spr_handle spr_num)
{
  p_surfaces = (SURFACE *)mmalloc(sizeof(p_surfaces[0])*surf_num);
  surface_num = surf_num;
  surface_last = 0;

  p_sprites = (SPRITE *)mmalloc(sizeof(p_sprites[0])*spr_num);
  sprite_num = spr_num;
  sprite_last = 0;
}

sprite_store::~sprite_store(void)
{
  ffree(p_surfaces);
  ffree(p_sprites);
}

void sprite_store::sprite_flag_set(tflag flag, spr_handle first, spr_handle num)
{
  spr_handle max = first+num;

  assert(max < sprite_num);

  for(; first < max; first++)    
    p_sprites[first].flag_set(flag);
}

void sprite_store::sprite_flag_clear(tflag flag, spr_handle first, spr_handle num)
{
  spr_handle max = first+num;

  assert(max < sprite_num);
  
  for(; first < max; first++)    
    p_sprites[first].flag_clear(flag);
}

static RECT rect_scale(RECT src)
{
  src.x *= SCALE_FACTOR; src.y *= SCALE_FACTOR;
  src.w *= SCALE_FACTOR; src.h *= SCALE_FACTOR;
  return(src);
}

spr_handle sprite_store::sprite_insert(const char *p_file, spr_handle first, spr_handle *p_last)
{  
  spr_handle i, j, max;
  int x, y, w, h;
  SURFACE *p_surf = NULL;
  SURFACE *p_orig = NULL;
  FHANDLE f;  
  int     scale = FALSE;
  char    filename[200];
  char    line[200];

  strncpy(filename, p_file, 200);
  change_tail(filename, BITMAP_FORMAT);

  f = file_open(surface::graphics_dir_get(),p_file, "r");

  RECT rec = {0,0,0,0};

  i = first;
  while (fgets(line, 200, f))
  {
    if (line[0] == ';')
      continue;
    else if (line[0] == 's') {
      int ret = sscanf(line, "s %d %d %d %d %d", &x, &y, &w, &h, &scale);
      if(!DOUBLE_SIZE || ret == 4)
        scale = FALSE;
    
      if(!p_surf) {
        surf_handle s_handle = surface_insert(filename);
        p_surf = surface_get(s_handle);
        if(!p_surf) {
          berror("Can't load surface %s",line);
        }
        if(scale) {
          p_orig = new SURFACE(p_surf, SCALE_FACTOR);
          p_surf = surface_switch(s_handle, p_orig);
          // p_orig is the loaded one
          // p_surf is a new (scaled) one
        }
      }
      
      rec.x = x; rec.y = y; rec.w = w; rec.h = h;
      if(scale) {
        p_surf->scale(p_orig, x, y, w, h, x*SCALE_FACTOR, y*SCALE_FACTOR);
      }
      
      RECT rec_sprite = scale ? rect_scale(rec) : rec;
      SPRITE tmp(p_surf,SDL_SPRITE_RECT,&rec_sprite);
      tmp.color_key_apply();
      sprite_insert(&tmp,1, i);
      i++;
    }
    else if (line[0] == 'f') {
      sscanf(line, "f %d %d %d %d %d", &x, &y, &w, &h, &max);
      for (j = 0; j < max; j++) {
        rec.x += x; rec.y += y; rec.w += w; rec.h += h;
        if(scale) {
          p_surf->scale(p_orig, rec.x, rec.y, rec.w, rec.h, 
                        rec.x*SCALE_FACTOR, rec.y*SCALE_FACTOR);
        }
        RECT rec_sprite = scale ? rect_scale(rec) : rec;
        SPRITE tmp(p_surf,SDL_SPRITE_RECT,&rec_sprite);
        sprite_insert(&tmp,1, i);
        i++;
      }
    }
  }
  fclose(f);

  if(p_orig) {
    delete p_orig;
  }

  if(p_last)
    *p_last = i;
  
  sprite_flag_set(SDL_SPRITE_USED, first, i - first);
  
  return (i - first);
}

spr_handle sprite_store::sprite_insert(surf_handle sf_handle, tflag flag_, RECT *p_rect, spr_handle first)
{            
  spr_handle shandle;
  if(first == INSERT_APPEND) {
    shandle = sprite_last++;      
  } else {
    shandle = first;
    if(shandle > sprite_last)
      sprite_last = shandle;      
  }
  assert(shandle < sprite_num);      
  flag_ &= ~SDL_SPRITE_SEPARATE_SURFACE;
  SPRITE tmp(p_surfaces+sf_handle, flag_, p_rect);
  memcpy(p_sprites+shandle,&tmp, sizeof(tmp));
  sprite_flag_set(SDL_SPRITE_USED, shandle);
  return shandle;
}

spr_handle sprite_store::sprite_insert(SURFACE *p_surf, tflag flag_, RECT *p_rect, spr_handle first)
{             
  spr_handle shandle;
  if(first == INSERT_APPEND) {
    shandle = sprite_last++;      
  } else {
    shandle = first;
    if(shandle > sprite_last)
      sprite_last = shandle;
  }
  assert(shandle < sprite_num);
  flag_ |= SDL_SPRITE_SEPARATE_SURFACE;
  SPRITE tmp(p_surf, flag_, p_rect);
  memcpy(p_sprites+shandle,&tmp, sizeof(tmp));      
  sprite_flag_set(SDL_SPRITE_USED, shandle);
  return shandle;
}

spr_handle sprite_store::sprite_insert(SPRITE *p_spr, int num, spr_handle first)
{
  spr_handle start = (first == INSERT_APPEND) ? sprite_last : first;
  assert(start+num < sprite_num);
  if(start+num > sprite_last)
    sprite_last = start+num;
  memcpy(p_sprites+start,p_spr,sizeof(p_spr[0])*num);
  sprite_flag_set(SDL_SPRITE_USED, start, num);
  return(start);
}

spr_handle sprite_store::sprite_copy(spr_handle dst_handle, spr_handle src_handle, bool copy_surface)
{
  assert(dst_handle < sprite_num && src_handle < sprite_num);
  SPRITE *p_src = p_sprites + src_handle;

  if(copy_surface && p_src->surf_get()) {
    RECT *p_src_rec = p_src->rect_get();
    RECT r = {0, 0, p_src_rec->w, p_src_rec->h};
    sprite_insert(surface_copy(p_src->surf_get(), p_src_rec),
                  p_src->flag_get(), &r, dst_handle);    
  } else {
    p_sprites[dst_handle] = *p_src;
    // -- add ref-counnt?
  }

  return(dst_handle);
}

void sprite_store::sprite_delete(spr_handle handle, int num)
{
  // TODO - removing sprites/surfaces
  sprite_flag_clear(SDL_SPRITE_USED, handle, num);
}


// -------------------------------------------------------
//   the graph 2d store class
// -------------------------------------------------------
void graph_2d::screen_create(int flag, int width, int height, int bpp, int fullscreen)
{

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
  if(SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "unable to init SDL: %s", SDL_GetError());
    exit(0);
  }      
        
  sdl_video_flags = flag|SDL_HWSURFACE;
  
  graphics_fullscreen = fullscreen;
  if(fullscreen)
    sdl_video_flags |= SDL_FULLSCREEN;

  graphics_bpp = bpp;
  
  screen_resize(width, height);  
}

// Obtain the screen from SDL
// If we have any screen/surface, release them first
bool graph_2d::screen_regenerate(void)
{
  screen_destroy();

  bprintf("Init video surface...\n");
  SDL_Surface *p_hwscreen = SDL_SetVideoMode(graphics_width, graphics_height, 
                                             graphics_bpp, sdl_video_flags);
  
  if(!p_hwscreen) {
    fprintf (stderr, "Unable to set the video mode: %s", SDL_GetError());
    exit(0);
  }
   
  p_screen_surface = new SURFACE(p_hwscreen);
  p_screen = new SPRITE(p_screen_surface, SDL_SPRITE_SEPARATE_SURFACE, NULL);

  redraw_reset();
  rect_whole = FALSE;

  return(TRUE);
}

void graph_2d::screen_destroy(void)
{
  if(p_screen) {
    delete p_screen;
    p_screen = NULL;
  }

  if(p_screen_surface) {
    delete p_screen_surface;
    p_screen_surface = NULL;
  }

  if(p_screen_surface) {
    delete p_screen_surface;
    p_screen_surface = NULL;
  }
}

void graph_2d::screen_resize(tpos width, tpos height)
{
  graphics_width = width;
  graphics_height = height;
  
  screen_regenerate();
}

void graph_2d::check(void)
{  
  int i;

  SURFACE *p_srf;
  p_srf = p_screen->surf_get();

  bprintf("Screen sprite %p, p_surf %p, HW surf %p", p_screen,
         p_srf, p_srf->surf_get());

  bprintf("Stored surfaces:");
  SURFACE *p_srlist = store.get_surface_interface();
  
  for(i = 0; i < store.get_surface_num(); i++) {
    p_srf = p_srlist+i;
    bprintf("%d Surface %p, HW surface %p, used %d", i, p_srf, p_srf->surf_get(), p_srf->used_get());
  }

  bprintf("Stored sprites:");
  SPRITE *p_splist = store.sprite_interface_get();
  SPRITE *p_sp;  
  
  for(i = 0; i < store.sprite_num_get(); i++) {
    p_sp = p_splist+i;
    if(p_sp->surf_get()) {
      RECT *p_rec = p_sp->rect_get();
      bprintf("%d Sprite %p, Surface %p, flag %d, rec = {%d, %d, %d, %d}", 
      i, p_sp, p_sp->surf_get(), p_sp->flag_get(), p_rec->x, p_rec->y,
      p_rec->w, p_rec->h);
    }
  }
  bprintf("--- ALL ---");
}

void graph_2d::fullscreen_toggle(void)
{
#ifdef LINUX
  if(!SDL_WM_ToggleFullScreen(p_screen_surface->surf_get())) {
    bprintf("SDL_WM_ToggleFullScreen() failed!");
    return;
  }
#endif  
  graphics_fullscreen = !graphics_fullscreen;  
}

// -------------------------------------------------------
//   the font table interface
// -------------------------------------------------------

bool font_lookup_table::load(char *p_file)
{  
  FHANDLE f = file_open(surface::graphics_dir_get(),p_file,"r");
  
  memset(position,0,sizeof(position));
  
  int pos = 0;
  char line[10];
  while(fgets(line,10,f)) {
    position[toupper(line[0])] = position[tolower(line[0])] = pos;
    pos++;
  }

  fclose(f);
  return(TRUE);
}

// -------------------------------------------------------
//   the font_info interface
// -------------------------------------------------------
GRAPH_2D * font_info::p_grf = NULL;

font_info::font_info(void)
{
  font_sprite_first = font_sprite_num = 0;
}

font_info::~font_info(void)
{
  p_grf->sprite_delete(font_sprite_first, font_sprite_num);
  font_sprite_first = font_sprite_num = 0;
}

bool font_info::load(int font_index, int first, int num)
{
  assert(!font_sprite_num && font_index < FONT_NUM);

  char  tmp[2000];
  sprintf(tmp, "font%d.spr", font_index);
  
  int i = p_grf->sprite_insert(tmp, first);
  if(i != num)
    return(FALSE);
  
  font_sprite_first = first;
  font_sprite_num = num;
  
  return(ftable.load(change_tail(tmp,".tab")));
}

void font_info::free(void)
{ 
  p_grf->sprite_delete(font_sprite_first, font_sprite_num);
}
// -------------------------------------------------------
//   the font_info interface
// -------------------------------------------------------
font::font(GRAPH_2D *p_graph)
{
  memset(this,0,sizeof(*this));

  font_info::p_grf = p_graph;
  align = MENU_LEFT;
}

font::~font(void)
{  
}

void font::print(char *p_string, RECT *p_res, int lines)
{
  if(!p_string)
    return;

  char *p_start = strdup(p_string);
  char *p_tmp = p_start;
  char *p_nl;

  FONT_INFO *p_font = finfo+font_selected;

  if(p_res) {
    p_res->x = ax;
    p_res->y = ay;
    p_res->w = p_res->h = 0;
  }

  while(*p_tmp) {
    p_nl = strchr(p_tmp, '\n');
    if(p_nl)
      *p_nl = '\0';
    
    tpos width_string = p_font->width_get(p_tmp);
    tpos height_string = p_font->height_get(p_tmp);
    tpos width_screen = p_font->screen_width_get();
    tpos px = ax+offset_x,
         py = ay+offset_y;
    
    switch(align) {
      case MENU_LEFT:
        break;
      case MENU_CENTER:
        px = (width_screen - width_string) / 2;
        break;
      case MENU_RIGHT:
        px -= width_string;
        break;
    }

    assert(px >= 0);
  
    if(p_res) {
      p_res->x = px;
      //bprintf("before print - [%d %d] -> [%d %d]",px, py, width_string, height_string);
      rect_adjust(p_res, px, py, width_string, height_string);
      //bprintf("after print - [%d %d] -> [%d %d]",p_res->x,p_res->y,p_res->w,p_res->h);
    }
  
    while(*p_tmp) {
      px += p_font->print(*p_tmp,px,py,!try_run);
      p_tmp++;
    }
 
    if(p_nl) {
      new_line();
      p_tmp = p_nl+1;
    }
  
    if(lines > 0) {
      lines--;
      if(!lines)
        break;
    }
  }

  ::free(p_start);
}

void font::print(RECT *p_res, tpos x, tpos y, const char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  if(!p_text)
    return;

  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);
  
  start_set(x, y);
  print(text,p_res);
}

void font::print(RECT *p_res, const char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  if(!p_text)
    return;

  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);
  
  print(text,p_res);
}

void font::print(RECT *p_res, tpos x, tpos y, int lines, const char *p_text,...)
{
  char      text[2000];
  va_list   arguments;  

  if(!p_text)
    return;
  
  va_start(arguments,p_text);
  vsnprintf(text,2000,p_text,arguments);
  va_end(arguments);
  
  start_set(x, y);
  print(text,p_res,lines);
}

void rect_adjust(RECT *p_r, tpos x, tpos y, tpos w, tpos h)
{
  assert(p_r);

  if(x < p_r->x) {
    p_r->w += p_r->x - x;
    p_r->x = x;
  }
  if(y < p_r->y) {
    p_r->h += p_r->y - y;
    p_r->y = y;
  }
  if(p_r->x + p_r->w < x + w) {
    p_r->w = x + w - p_r->x;
  }
  if(p_r->y + p_r->h < y + h) {
    p_r->h = y + h - p_r->y;
  }
}

void rect_adjust(RECT *p_r, RECT *p_s)
{
  rect_adjust(p_r, p_s->x, p_s->y, p_s->w, p_s->h);
}

// -------------------------------------------------------
//   Global interfaces
// -------------------------------------------------------

GRAPH_2D *p_grf  = NULL;
FONT     *p_font = NULL;

void graphics_start(tpos dx, tpos dy, int depth, bool fullscreen)
{
  if(!p_grf) {
    p_grf = new GRAPH_2D(dx, dy, depth, fullscreen);
  } else {
    p_grf->screen_resize(dx, dy);
  }

  if(!p_font) {
    p_font = new FONT(p_grf);
  }
}

void graphics_stop(void)
{
  if(p_font)
    delete p_font;
  if(p_grf)
    delete p_grf;
}
