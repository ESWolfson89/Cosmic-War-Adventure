#include "rng.h"

bool roll(int sides)
{
    return randInt(0,sides-1) == 0;
}

bool rollPerc(int perc)
{
    return randInt(0,99) < perc;
}

int randInt(int low, int high)
{
    if (high <= low)
        return high;

    std::uniform_int_distribution<int> dist(low,high);
    return dist(random_number_generator);
}

int randIntZ(int high)
{
    return randInt(0, high);
}

point randZeroBasedPoint(int maxX, int maxY)
{
    return randPoint(0, 0, maxX, maxY);
}

point randPoint(int minX, int minY, int maxX, int maxY)
{
    return point(randInt(minX, maxX), randInt(minY, maxY));
}