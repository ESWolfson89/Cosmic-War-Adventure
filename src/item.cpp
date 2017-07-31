#include "item.h"

item::item()
{
    quantity = 1;
    stats.i_type = NIL_i;
}

item::item(item_stats i, int q)
{
    quantity = q;
    stats = i;
}

void item::setQuant(int q)
{
    quantity = q;
}

void item::setStats(item_stats new_stats)
{
    stats = new_stats;
}

int item::getQuant()
{
    return quantity;
}

item_t item::getType()
{
    return stats.i_type;
}

item_stats *item::getStats()
{
    return &stats;
}

item_container::item_container()
{

}

void item_container::addToContainer(item_stats istats, int quant)
{
    int item_position = getPos(&istats);

    if (item_position != -1)
        items[item_position].setQuant(items[item_position].getQuant() + quant);
    else
        items.push_back(item(istats,quant));
}

void item_container::removeItem(int i, int q)
{
    if (items[i].getQuant() > q)
        items[i].setQuant(items[i].getQuant() - q);
    else
        items.erase(items.begin() + i);
}

void item_container::deleteAllItems()
{
    std::vector<item>().swap(items);
}

void item_container::initContainer()
{
    items.clear();
}

bool item_container::isEmpty()
{
    return (items.size() == 0);
}

int item_container::getNumItems()
{
    return items.size();
}

int item_container::getPos(item_stats *s)
{
    for (uint i = 0; i < items.size(); ++i)
        if (itemsEqual(items[i].getStats(), s))
            return i;
    // item not found
    return -1;
}

item *item_container::getItem(int i)
{
    return &items[i];
}

cell_pile::cell_pile(point p)
{
    loc = p;
}

point cell_pile::getLoc()
{
    return loc;
}

bool itemsEqual(item_stats *a, item_stats *b)
{
    return (a->i_type == b->i_type);
}
