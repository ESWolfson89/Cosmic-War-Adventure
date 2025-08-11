#ifndef MISC_H_
#define MISC_H_

#include "globals.h"

struct dice_roll
{
    uint num_dice;
    uint num_sides;
    uint num_bonus;
};

void stringSave(std::ofstream &, const std::string &);

void stringLoad(std::ifstream &, std::string &);

void chtypeSave(std::ofstream &, const chtype &);

void chtypeLoad(std::ifstream &, chtype &);

void colorPairSave(std::ofstream&, const color_pair &);

void colorPairLoad(std::ifstream&, color_pair&);

void colorTypeSave(std::ofstream&, const color_type &);

void colorTypeLoad(std::ifstream&, color_type&);

void saveDiceRoll(std::ofstream& os, const dice_roll&);

void loadDiceRoll(std::ifstream& is, dice_roll&);

std::string capitalizeFirstLetter(std::string s);

std::string double2String(double);

std::string int2String(int);

std::string uint642String(uint_64);

std::string capitalizeString(std::string);

bool chtypeMatches(chtype a, chtype b);

#endif
