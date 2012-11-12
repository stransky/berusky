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

#include "berusky.h"

char * item_repository::item_desc_name[MAX_ITEM_NAMES] = 
{ 
  _("Floor"),

  _("Player 1"),
  _("Player 2"),
  _("Player 3"),
  _("Player 4"),
  _("Player 5"),

  _("Box"),
  _("Explosive"),
  _("Wall"),
  _("Exit"),
  _("Stone"),
  _("Key (to exit)"),
  _("Pickax"),

  // ---
  _("Color Key (for player 1)"),
  _("Color Key (for player 2)"),
  _("Color Key (for player 3)"),
  _("Color Key (for player 4)"),
  _("Color Key (for player 5)"),

  // ---
  _("Color door (for player 1)"),
  _("Color door (for player 2)"),
  _("Color door (for player 3)"),
  _("Color door (for player 4)"),
  _("Color door (for player 5)"),

  _("Color door (for player 1)"),
  _("Color door (for player 2)"),
  _("Color door (for player 3)"),
  _("Color door (for player 4)"),
  _("Color door (for player 5)"),

  _("Color door (for player 1)"),
  _("Color door (for player 2)"),
  _("Color door (for player 3)"),
  _("Color door (for player 4)"),
  _("Color door (for player 5)"),

  _("Color door (for player 1)"),
  _("Color door (for player 2)"),
  _("Color door (for player 3)"),
  _("Color door (for player 4)"),
  _("Color door (for player 5)"),
  
  // ---
  _("Color gate-way (for player 1)"),
  _("Color gate-way (for player 2)"),
  _("Color gate-way (for player 3)"),
  _("Color gate-way (for player 4)"),
  _("Color gate-way (for player 5)"),

  _("Color gate-way (for player 1)"),
  _("Color gate-way (for player 2)"),
  _("Color gate-way (for player 3)"),
  _("Color gate-way (for player 4)"),
  _("Color gate-way (for player 5)"),

  _("Color gate-way (for player 1)"),
  _("Color gate-way (for player 2)"),
  _("Color gate-way (for player 3)"),
  _("Color gate-way (for player 4)"),
  _("Color gate-way (for player 5)"),

  _("Color gate-way (for player 1)"),
  _("Color gate-way (for player 2)"),
  _("Color gate-way (for player 3)"),
  _("Color gate-way (for player 4)"),
  _("Color gate-way (for player 5)"),

  // ---
  _("One-pass door"),
  _("One-pass door"),
  _("One-pass door"),
  _("One-pass door"),

  _("One-pass door"),
  _("One-pass door"),

  // --
  _("Jamb (Wall)"),
  _("Jamb (Wall)"),
  _("Jamb (Wall)"),
  _("Jamb (Wall)"),
  
  _("Jamb (Wall)"),
  _("Jamb (Wall)"),
  _("Jamb (Wall)"),
  _("Jamb (Wall)"),
  
  _("Light box")
};

char * item_repository::item_desc_rotation[MAX_ITEM_ROTATIONS] = 
{ 
  _("0 DG."),
  _("90 DG."),
  _("180 DG."),
  _("270 DG.")
};

void item_repository::create(int *p_indexes, int indexnum)
{

  int i;
  for(i = 0; i < indexnum; i++) {
    if(p_indexes[i]) {
      p_obj[i] = (GAME_OBJECT *)mmalloc(sizeof(GAME_OBJECT)*p_indexes[i]);
    }
  }  
}

/*
  TODO - load game items from disk
*/
int item_repository::load(const char *p_item_definition_file, const char *p_item_animation_file, const char *p_dir)
{
  DATA_PARSER data_file(get_parser_table(), get_parser_table_items());

  /* Load item definitions
  */

  if(!data_file.open(p_item_definition_file, p_dir)) {    
    return(FALSE);
  }
  
  data_file.get_indexes(obj_num,GAME_ITEMS);
  create(obj_num,GAME_ITEMS); 
  
  #define REC_NUM 12
  
  DATA_RECORD rec[REC_NUM];
  while(data_file.records_get(rec, REC_NUM)) {
  
      assert(rec[0].base < GAME_ITEMS);
      assert((rec[0].num-rec[0].base) < obj_num[rec[0].base]);
  
      GAME_OBJECT *p_tmp = p_obj[rec[0].base] + rec[0].offset;

      p_tmp->item      = rec[0].base;
      p_tmp->func      = rec[1].num;
      p_tmp->flag      = rec[2].num|rec[9].num;
      p_tmp->minus_x   = rec[3].num;
      p_tmp->plus_x    = rec[4].num;
      p_tmp->x_cor     = rec[5].num;
      p_tmp->y_cor     = rec[6].num;
      p_tmp->sprite    = rec[7].num;
      p_tmp->animation = rec[8].num;
      p_tmp->p_so[0]   = rec[10].num ? p_obj[rec[10].base] + rec[10].offset : NULL;
      p_tmp->p_so[1]   = rec[11].num ? p_obj[rec[11].base] + rec[11].offset : NULL;
  }

  data_file.close();

  /* Load item animations
  */
  if(!data_file.open(p_item_animation_file, p_dir))
    return(FALSE);
    
  while(data_file.records_get(rec, REC_NUM)) {
  
      assert(rec[0].base < GAME_ITEMS);
      assert((rec[0].num-rec[0].base) < obj_num[rec[0].base]);
  
      GAME_OBJECT *p_tmp = p_obj[rec[0].base] + rec[0].offset;
      
      p_tmp->flag      = rec[2].num;
      p_tmp->animation = rec[1].num;
  }

  data_file.close();

  return(TRUE);
}

void game_object::dump(void)
{
  bprintf("func(%d) min_x(%d) plus_x(%d) x_cor(%d) y_cor(%d) sprite(%d) anim(%d)",
          func,minus_x,plus_x,x_cor,y_cor,sprite,animation);  
}
