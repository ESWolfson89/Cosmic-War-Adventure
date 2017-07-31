#include "menu.h"

menu::menu()
{
    selection_index = 0;
}

menu::menu(point l, point a)
{
    loc = l;
    area = a;
    selection_index = 0;
    menu_level = 0;
}

menu_item menu::getMenuItem(int index)
{
    return menu_items[index];
}

void menu::cleanupEverything()
{
    std::vector<menu_item>().swap(menu_items);
    std::vector<std::string>().swap(menu_main_text);
}

void menu::clearMenuMainText()
{
    menu_main_text.clear();
}

void menu::clearMenuItems()
{
    menu_items.clear();
}

void menu::setMenuLevel(int lev)
{
    menu_level = lev;
}

std::string menu::getMenuMainText(int i)
{
    return menu_main_text[i];
}

int menu::getNumMenuMainTextStrings()
{
    return (int)menu_main_text.size();
}

void menu::eraseMenuItem(int i)
{
    menu_items.erase(menu_items.begin() + i);
}

void menu::traverseMenuMainText()
{
    if (getNumMenuMainTextStrings() > 1)
        menu_main_text.erase(menu_main_text.begin());
}

void menu::addMenuMainText(std::string mmt)
{
    menu_main_text.push_back(mmt);
}

void menu::addMenuItem(std::string s,chtype ct)
{
    menu_item m = {s,ct};
    menu_items.push_back(m);
}

void menu::incSelectionIndex()
{
    selection_index++;
    if (selection_index > (int)menu_items.size() - 1)
        selection_index = 0;
}

int menu::getNumMenuItems()
{
    return (int)menu_items.size();
}

void menu::decSelectionIndex()
{
    selection_index--;
    if (selection_index < 0)
        selection_index = (int)menu_items.size() - 1;
}

void menu::setSelectionIndex(int i)
{
    if (i >= 0 && i <= (int)menu_items.size() - 1)
        selection_index = i;
}

int menu::getSelectionIndex()
{
    return selection_index;
}

int menu::getMenuLevel()
{
    return menu_level;
}

point menu::getLoc()
{
    return loc;
}

point menu::getArea()
{
    return area;
}
