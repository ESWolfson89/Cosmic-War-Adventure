#ifndef NAMEGEN_H_
#define NAMEGEN_H_

#include "globals.h"
#include "rng.h"

std::string genName(int);

static std::string starting_consonant[10] =
{
	"b","f","j","m","p","q","v","w","x","z"
};

static std::string middle_consonant[20] =
{
    "b","c","d","f","g","h","j","k","l","m","n","p","r","s","t","v","w","x","z","q"
};

static std::string vowel[5] =
{
	"a","e","i","o","u"
};

static std::string final_name_modifier[15] =
{
    "ion","egan","ilon","eydi","ootin","ius","oon","irri","auri","oryus","edon","aplox","ugg","elius","aquin"
};

#endif
