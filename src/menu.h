#ifndef MENU_H_
#define MENU_H_

#include "globals.h"
#include "point.h"

struct menu_item
{
    std::string description;
    chtype menu_symbol;
};

class menu
{
    public:
        menu();
        menu(point,point);
        menu_item getMenuItem(int);
        std::string getMenuMainText(int);
        void cleanupEverything();
        void clearMenuMainText();
        void clearMenuItems();
        void addMenuMainText(std::string);
        void addMenuItem(std::string,chtype);
        void eraseMenuItem(int);
        void traverseMenuMainText();
        void incSelectionIndex();
        void decSelectionIndex();
        void setSelectionIndex(int);
        void setMenuLevel(int);
        int getNumMenuMainTextStrings();
        int getSelectionIndex();
        int getMenuLevel();
        int getNumMenuItems();
        point getLoc();
        point getArea();
    private:
        point loc;
        point area;
        int selection_index;
        int menu_level;
        std::vector<std::string> menu_main_text;
        std::vector<menu_item> menu_items;
};

#endif
