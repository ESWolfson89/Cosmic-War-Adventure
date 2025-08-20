#include "menu.h"

menu::menu()
{
    selection_index = 0;
    menu_level = 0;
    scroll_offset = 0;
}

menu::menu(point l, point a)
{
    loc = l;
    area = a;
    selection_index = 0;
    menu_level = 0;
    scroll_offset = 0;
}

menu_item menu::getMenuItem(int index)
{
    return menu_items[index];
}

void menu::cleanupEverything()
{
    std::vector<menu_item>().swap(menu_items);
    std::vector<std::string>().swap(menu_main_text);
    selection_index = 0;
    scroll_offset = 0;
}

void menu::clearMenuMainText()
{
    menu_main_text.clear();
}

void menu::clearMenuItems()
{
    menu_items.clear();
    selection_index = 0;
    scroll_offset = 0;
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
    clampSelectionToItems_();
    ensureSelectionVisible_();
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

void menu::addMenuItem(std::string s, chtype ct)
{
    menu_item m = { s, ct };
    menu_items.push_back(m);
    clampSelectionToItems_();
    ensureSelectionVisible_();
}

void menu::incSelectionIndex()
{
    if (menu_items.empty()) return;

    selection_index++;
    if (selection_index > (int)menu_items.size() - 1)
        selection_index = 0;

    ensureSelectionVisible_();
}

int menu::getNumMenuItems()
{
    return (int)menu_items.size();
}

void menu::decSelectionIndex()
{
    if (menu_items.empty()) return;

    selection_index--;
    if (selection_index < 0)
        selection_index = (int)menu_items.size() - 1;

    ensureSelectionVisible_();
}

void menu::setSelectionIndex(int i)
{
    if (i >= 0 && i <= (int)menu_items.size() - 1)
        selection_index = i;

    ensureSelectionVisible_();
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
    ensureSelectionVisible_();
}

int menu::getScrollOffset() const
{
    return scroll_offset;
}

// Visible rows inside the frame: title consumes 1 row, top/bottom borders 2 rows ? area.y - 3
int menu::getVisibleRows() const
{
    return std::max(0, area.y() - 3);
}

// ----- persistence -----

void menu::save(std::ofstream& os) const
{
    loc.save(os);
    area.save(os);

    os.write(reinterpret_cast<const char*>(&selection_index), sizeof(int));
    os.write(reinterpret_cast<const char*>(&menu_level), sizeof(int));
    os.write(reinterpret_cast<const char*>(&scroll_offset), sizeof(int)); // NEW

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
    is.read(reinterpret_cast<char*>(&scroll_offset), sizeof(int)); // NEW

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

    // Make sure loaded indices are valid for the current content/area.
    clampSelectionToItems_();
    ensureSelectionVisible_();
}

// ----- private helpers -----

void menu::clampSelectionToItems_()
{
    if (menu_items.empty()) {
        selection_index = 0;
        scroll_offset = 0;
        return;
    }
    if (selection_index < 0) selection_index = 0;
    if (selection_index >= (int)menu_items.size())
        selection_index = (int)menu_items.size() - 1;
}

void menu::ensureSelectionVisible_()
{
    const int rows = getVisibleRows();
    if (rows <= 0) { scroll_offset = 0; return; }

    // Keep scroll_offset within [0, maxStart]
    const int maxStart = std::max(0, (int)menu_items.size() - rows);
    if (scroll_offset < 0) scroll_offset = 0;
    if (scroll_offset > maxStart) scroll_offset = maxStart;

    // If selection is above current window, scroll up
    if (selection_index < scroll_offset)
        scroll_offset = selection_index;

    // If selection is below current window, scroll down
    if (selection_index >= scroll_offset + rows)
        scroll_offset = selection_index - rows + 1;

    // Clamp again for safety
    if (scroll_offset < 0) scroll_offset = 0;
    if (scroll_offset > maxStart) scroll_offset = maxStart;
}

// --- free helpers (unchanged) ---
point getSmallStationMenuSize()
{
    return point(GRIDWID - 4, SHOWHGT / 2 - 2);
}

point getLargeStationMenuSize()
{
    return point(GRIDWID - 4, SHOWHGT - 2);
}