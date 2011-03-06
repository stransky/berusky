
0. Homepage

  http://www.anakreon.cz/
  
1. Configuration

  The configuration is stored at your installation dir in berusky.ini file. 
  You can switch fulscreen/windows mode, number of colors and tweak game directories there.

2. Game run

  beruskyw.exe                - runs main game
  beruskyw.exe -u test.lv3    - runs user level named "test.lv3"
  beruskyw.exe -e             - runs editor
  beruskyw.exe -e test.lv3    - runs editor and loads level "test.lv3" for editing

3. Game Rules

  In order to leave each level it is necessary to own five keys and also
  to have a free way to the exit.

  You will be meeting miscellaneous game elements while completing individual
  missions, we will try to explain their meaning now.

  Box             - It is possible to push it.
  Explosive       - Can destroy the boxes.

  Key             - You need five of them.
  Exit            - A gate to next level.

  Stone           - Can be broken by a pickax.
  Pickax          - A tool for stone crushing.

  Color key       - Used to unlock color door, only a bug with identical 
                    color can pick them up.
  Color door      - Can be opened by the respective color key only.

  Color gate-way  - Only a bug with identical color is allowed to go
                    through. Boxes cannot be pushed through.

  One-pass door   - Can be used only once, then they close them off 
                    and there's no way to open them.

  Other elements not listed here are just walls, which have no interesting
  properties. They cannot be push away nor it is possible 
  to break them anywise.


4. Game keys

  Arrows      - Move selected bug
        
  Tabulator   - Switch among the bugs

  1           - Choose the first bug
  2           - Choose the second bug
  3           - Choose the third bug
  4           - Choose the fourth bug
  5           - Choose the fifth bug

  N           - change the music (not implemented yet)
  D           - start recording a demo (not implemented yet)
  ALT+X       - quit quickly

  F1          - In game help
  F2          - Save level
  F3          - Load level

5. Editor keys

  F1      - help

  CTRL+N  - new level
  F2      - save
  CTRL+F2 - save as
  F3      - load
  ESC     - end

  F9      - run level
   
  CTRL+U  - undo  

  SHIFT+R - rotation of selected item
  CTRL+S  - level shader

  CTRL+1  - on/off grid/background image
  CTRL+2  - on/off floor layer
  CTRL+3  - on/off items layer
  CTRL+4  - on/off players layer

  1       - select floor layer
  2       - select items layer
  3       - select players layer
  4       - select all layer
  
  B       - change background image   

Mouse editation:
  
  first mouse button      - insert selected item
  third mouse button      - clear selected cell
  
  R + mouse wheel         - rotation of item in level
  V + mouse wheel         - variation of item in level

  F + first mouse button  - fill rectangle with selected item
  D + first mouse button  - draw rectangle with selected item
  
  F + third mouse button  - clear solid rectangle with selected item
  D + third mouse button  - clear empty rectangle with selected item

Mouse item panel control:

  If you place a mouse cursor on item panel you can use key shortcuts
  for fast panel scroll:

  mouse wheel             - scroll the panel by one item
  Page Up/Down            - scroll the panel by one page (12 items)
  Home/End                - move panel to first/last item
  
