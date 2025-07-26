#ifndef RNG_H_
#define RNG_H_

#include <random>
#include <time.h>
#include "point.h"

static std::mt19937 random_number_generator(static_cast<unsigned int>(time(nullptr)));

bool roll(int sides);
bool rollPerc(int perc);
int randInt(int low, int high);
int randIntZ(int high);
point randZeroBasedPoint(int maxX, int maxY);
point randPoint(int minX, int minY, int maxX, int maxY);

#endif
