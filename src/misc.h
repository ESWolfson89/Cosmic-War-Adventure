#ifndef MISC_H_
#define MISC_H_

#include "globals.h"
#include <fstream>

struct dice_roll
{
    uint num_dice;
    uint num_sides;
    uint num_bonus;
};

void stringSave(std::ofstream &, std::string);

void stringLoad(std::ifstream &, std::string &);

void chtypeSave(std::ofstream &, chtype);

void chtypeLoad(std::ifstream &, chtype &);

std::string double2String(double);

std::string int2String(int);

std::string uint642String(uint_64);

std::string capitalizeString(std::string);

#endif
