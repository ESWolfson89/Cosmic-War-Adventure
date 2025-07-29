#ifndef ITEM_H_
#define ITEM_H_

#include "globals.h"
#include "point.h"

struct item_stats
{
    item_t i_type;
};

static const item_stats standard_item_stats[1] =
{
    {
        ITEM_WRECKAGECREDIT
    }
};

class item
{
public:
    item();

    item(item_stats, int);

    void setQuant(int);

    void setStats(item_stats);

    int getQuant();

    item_t getType();

    item_stats *getStats();

    void save(std::ofstream&) const;

    void load(std::ifstream&);

private:
    int quantity;
    item_stats stats;
};

// can be used as an inventory, pile on the ground, or vending machine contents
class item_container
{
public:

    item_container();

    void addToContainer(item_stats, int);

    void removeItem(int, int);

    void deleteAllItems();

    void initContainer();

    bool isEmpty();

    int getNumItems();

    int getPos(item_stats *);

    item *getItem(int);

    virtual void save(std::ofstream&) const;

    virtual void load(std::ifstream&);


protected:

    std::vector <item> items;

    int total_weight;
};

// inherits from item_container
class cell_pile : public item_container
{
    public:
    
        cell_pile() = default;

        cell_pile(point);
    
        point getLoc();

        void save(std::ofstream&) const override;

        void load(std::ifstream&) override;
    
    private:
        point loc;
};

bool itemsEqual(item_stats*, item_stats*);

#endif
