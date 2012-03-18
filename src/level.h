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

/* Level dependencies */

#ifndef __LEVEL_H__
#define __LEVEL_H__

/*
  rozdeleni:
  ----------
  struktura:
  word pocet druhu grafiky
  word varianty prvku - cisla grafiky [100] (tj. pole indexu do spr)

  word cislo fce pri prichazeni
  word cislo fce pri dojiti

  veci zapsane v levelu
  ---------------------
  cislo prvku (vlastne soucastna varianta druhu)
  cislo variany (tj. cislo druhu zobrazeni, tzn. poradove cislo SPRITES od 0 do urciteho cisla)
  rotace prvku (pouze u jednoprvkovych SPRITES)

  rozdeleni:
  ----------
  word podlaha[YPOLI][XPOLI][10]   ;   // -> podlaha
  word level[YPOLI][XPOLI][10] ;   //-> herni plocha (dnesni level)
  word PLAYERi[YPOLI][XPOLI]     ;   //-> mapa PLAYERu (dneska sou PLAYERi n levelu)
  [0] -> cislo prvku v poli
  [1] -> poradove cislo grafiky
  [3] -> rotace
  [4] ... [9] -> rezerva
*/

#define GO_SUB_NUM       2

typedef class game_object {

public:

  int                  item;

  // type of item (boor, box, ...), 0xff -> blind item
  int                  func;

  // flags
  int                  flag;

  // x - corrections for main item
  int                  minus_x;
  int                  plus_x;

  // x/y corrections (for sub-objects)
  int                  x_cor;     
  int                  y_cor;

  spr_handle           sprite;    // sprite for this item
  anim_template_handle animation; // animation for this item  

  void                *p_so[GO_SUB_NUM]; // sub-items

  void                 dump(void);

} GAME_OBJECT;

typedef class item_repository {

  GAME_OBJECT         *p_obj[GAME_ITEMS];
  int                  obj_num[GAME_ITEMS];

  #define MAX_ITEM_NAMES      80
  static char         *item_desc_name[MAX_ITEM_NAMES];
  #define MAX_ITEM_ROTATIONS  4
  static char         *item_desc_rotation[MAX_ITEM_ROTATIONS];

public:
    
  item_repository(void)
  {
    memset(p_obj,0,sizeof(p_obj));
  }
  ~item_repository(void)
  {
    int i;
    for(i = 0; i < GAME_ITEMS; i++) {
      if(p_obj[i]) {
        free(p_obj[i]);
        p_obj[i] = NULL;        
      }
    }
  }

  int  load(const char *p_item_definition_file, const char *p_item_animation_file, const char *p_dir);
  void create(int *p_indexes, int indexnum);

  bool item_valid(int item)
  {    
    return(item != NO_ITEM && item < GAME_ITEMS);
  }

  int item_num_get(void)
  {
    return(GAME_ITEMS);
  }

  char * item_get_name(int item)
  {
    assert(item_valid(item));
    assert(item < (int)(sizeof(item_desc_name) / sizeof(item_desc_name[0])));
  
    return(item_desc_name[item]);
  }

  char * item_get_rotation(int rot)
  {    
    assert(rot < (int)(sizeof(item_desc_rotation) / sizeof(item_desc_rotation[0])));
    return(item_desc_rotation[rot]);
  }

  bool item_player(int item)
  {        
    return(item >= P_PLAYER_1 && item <= P_PLAYER_5);
  }

  int item_get_layer(int item)
  {
    if(item == P_GROUND)
      return(LAYER_FLOOR);
    else if (item >= P_PLAYER_1 && item <= P_PLAYER_5)
      return(LAYER_PLAYER);
    else
      return(LAYER_ITEMS);      
  }

  bool item_can_rotate(int item)
  {
    // We can't rotate item if it has this flag OR
    // it has more than one item
    return(!(p_obj[item][0].flag&SPRITE_NO_ROTATE) && 
           p_obj[item][0].p_so[0] == NULL && 
           p_obj[item][0].p_so[1] == NULL);
  }

  int item_flag_get(int item, int variant)
  {
    return(p_obj[item][variant].flag);
  }

  spr_handle sprite_get(int item, int variant, int rotation)
  {
    return(p_obj[item][variant].sprite + rotation * ROT_SHIFT);
  }

  int variants_get(int item)
  {
    return(obj_num[item]);
  }

  bool variant_valid(int item, int variant)
  {
    return(variant >= 0 && variant < obj_num[item]);
  }
 
  anim_template_handle animation_get(int item, int variant)
  {
    return(p_obj[item][variant].animation);
  }

  int subitems_num_get(int item, int variant)
  {  
    return(p_obj[item][variant].p_so[0] && p_obj[item][variant].p_so[1] ? 2 : 0);
  }

  void subitems_position_get(int item, int variant, int sum_item, tpos *p_x, tpos *p_y)
  {  
    assert(sum_item >= 0 && sum_item < GO_SUB_NUM);
    GAME_OBJECT *p_tmp = (GAME_OBJECT *)p_obj[item][variant].p_so[sum_item];
    assert(p_tmp);

    *p_x = p_tmp->x_cor;
    *p_y = p_tmp->y_cor;
  }

  spr_handle subitems_sprite_get(int item, int variant, int sum_item)
  {  
    assert(sum_item >= 0 && sum_item < GO_SUB_NUM);
    GAME_OBJECT *p_tmp = (GAME_OBJECT *)p_obj[item][variant].p_so[sum_item];
    assert(p_tmp);

    return(p_tmp->sprite);
  }

  anim_template_handle subitems_animation_get(int item, int variant, int sum_item)
  {  
    assert(sum_item >= 0 && sum_item < GO_SUB_NUM);
    GAME_OBJECT *p_tmp = (GAME_OBJECT *)p_obj[item][variant].p_so[sum_item];
    assert(p_tmp);

    return(p_tmp->animation);
  }

  item_handle subitems_item_get(int item, int variant, int sum_item)
  {  
    assert(sum_item >= 0 && sum_item < GO_SUB_NUM);
    GAME_OBJECT *p_tmp = (GAME_OBJECT *)p_obj[item][variant].p_so[sum_item];
    assert(p_tmp);

    return(p_tmp->item);
  }

  int subitems_variation_get(int item, int variant, int sum_item)
  {
    assert(sum_item >= 0 && sum_item < GO_SUB_NUM);
    GAME_OBJECT *p_tmp = (GAME_OBJECT *)(p_obj[item][variant].p_so[sum_item]);
    assert(p_tmp);

    return(p_tmp - p_obj[p_tmp->item]);
  }

  void draw(tpos x, tpos y, int item, int variation, int rotation, spr_handle dst = 0)
  {
    if(item_valid(item)) {
      p_grf->draw(sprite_get(item, variation, rotation), x, y, dst);
    
      int i,sitems = subitems_num_get(item, variation);
      for(i = 0; i < sitems; i++) {
        tpos dx,dy;
      
        subitems_position_get(item, variation, i, &dx, &dy);

        item_handle sub_item = subitems_item_get(item, variation, i);
        int sub_variation = subitems_variation_get(item, variation, i);

        p_grf->draw(sprite_get(sub_item, sub_variation, rotation), x+dx*CELL_SIZE_X, y+dy*CELL_SIZE_Y, dst);
      }
    }
  }

} ITEM_REPOSITORY;

#endif // __LEVEL_H__
