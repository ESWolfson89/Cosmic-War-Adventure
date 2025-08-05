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

void menu::setView(point l, point a)
{
    loc = l;
    area = a;
}

void menu::save(std::ofstream & os) const
{
    loc.save(os);
    area.save(os);

    os.write(reinterpret_cast<const char*>(&selection_index), sizeof(int));
    os.write(reinterpret_cast<const char*>(&menu_level), sizeof(int));

    int numMainStrings = static_cast<int>(menu_main_text.size());
    os.write(reinterpret_cast<const char*>(&numMainStrings), sizeof(int));
    for (const auto& str : menu_main_text)
        stringSave(os, str);

    int numItems = static_cast<int>(menu_items.size());
    os.write(reinterpret_cast<const char*>(&numItems), sizeof(int));
    for (const auto& item : menu_items)
    {
        stringSave(os, item.description);
        chtypeSave(os, item.menu_symbol);
    }
}

void menu::load(std::ifstream& is)
{
    loc.load(is);
    area.load(is);

    is.read(reinterpret_cast<char*>(&selection_index), sizeof(int));
    is.read(reinterpret_cast<char*>(&menu_level), sizeof(int));

    int numMainStrings = 0;
    is.read(reinterpret_cast<char*>(&numMainStrings), sizeof(int));
    menu_main_text.resize(numMainStrings);
    for (int i = 0; i < numMainStrings; ++i)
        stringLoad(is, menu_main_text[i]);

    int numItems = 0;
    is.read(reinterpret_cast<char*>(&numItems), sizeof(int));
    menu_items.resize(numItems);
    for (int i = 0; i < numItems; ++i)
    {
        stringLoad(is, menu_items[i].description);
        chtypeLoad(is, menu_items[i].menu_symbol);
    }
}

point getSmallStationMenuSize()
{
    return point(GRIDWID - 4, SHOWHGT / 2 - 2);
}

point getLargeStationMenuSize()
{
    return point(GRIDWID - 4, SHOWHGT - 2);
}
