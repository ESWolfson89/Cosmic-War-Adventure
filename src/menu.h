#ifndef MENU_H_
#define MENU_H_

#include "globals.h"
#include "point.h"
#include "misc.h"

struct menu_item
{
    std::string description;
    chtype menu_symbol;
};

class menu
{
public:
    menu();
    menu(point, point);

    menu_item getMenuItem(int);
    std::string getMenuMainText(int);

    void cleanupEverything();
    void clearMenuMainText();
    void clearMenuItems();

    void addMenuMainText(std::string);
    void addMenuItem(std::string, chtype);
    void eraseMenuItem(int);
    void traverseMenuMainText();

    void incSelectionIndex();
    void decSelectionIndex();
    void setSelectionIndex(int);
    void setMenuLevel(int);

    void save(std::ofstream&) const;
    void load(std::ifstream&);

    void setView(point, point);

    int  getNumMenuMainTextStrings();
    int  getSelectionIndex();
    int  getMenuLevel();
    int  getNumMenuItems();
    int  getScrollOffset() const;          // NEW
    int  getVisibleRows() const;           // NEW

    point getLoc();
    point getArea();

private:
    // helpers to keep scroll in range
    void clampSelectionToItems_();         // NEW
    void ensureSelectionVisible_();        // NEW

private:
    point loc;
    point area;
    int selection_index;
    int menu_level;
    int scroll_offset;                     // NEW: first visible item index
    std::vector<std::string> menu_main_text;
    std::vector<menu_item>    menu_items;
};

point getSmallStationMenuSize();
point getLargeStationMenuSize();

#endif