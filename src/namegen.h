#ifndef NAMEGEN_H_
#define NAMEGEN_H_

#include "globals.h"
#include "rng.h"
#include "mob.h"

std::string genAlienName(int);
std::string getNamePrefix(MobShip*);

static std::vector<std::string> startingConsonant =
{
	"b","f","j","m","p","q","v","w","x","z","n","r",
};

static std::vector<std::string> middleConsonant =
{
    "b","c","d","f","g","h","j","k","l","m","n","p","r","s","t","v","w","x","z","q"
};

static std::vector<std::string> vowel =
{
	"a","e","i","o","u"
};

static std::vector<std::string> finalNameModifier =
{
    "ion","egan","ilon","eydi","ootin","ius","oon","irri","auri","oryus","edon","aplox","ugg","elius","aquin"
};

static std::vector<std::string> exoticPrefixes = 
{
    "Bra", "Fro", "Jex", "Mar", "Prek", "Qua", "Vex", "Wol", "Xal", "Zor",
    "Brek", "Fim", "Jal", "Mok", "Prax", "Qel", "Vul", "Wrax", "Xel", "Zhek",
    "Bla", "Farn", "Jux", "Mern", "Plek", "Nal", "Nai", "Nor", "Rig", "Rel", "Ryl"
};

static std::vector<std::string> exoticSuffixes = 
{
    "thor", "ax", "an", "zar", "ith", "aarn", "dok", "uun", "ek", "yss",
    "rax", "moth", "orin", "vok", "lium", "drax", "zeer", "aruk", "qor", "ix",
    "ven", "xil", "jarn", "omir", "zoth"
};

static std::vector<std::string> doubleVowelCombos =
{
    "ae", "oo", "ii", "ua", "ei", "io"
};

static std::vector<std::string> doubleConsonantCombos =
{
    "rr", "kk", "zz", "ll", "mn", "kt", "sh", "th", "zh"
};

static std::vector<std::string> numberSet =
{
    "1","2","3","4","5","6","7","8","9","0"
};

static std::vector<std::string> codeSet = 
{
    std::string(1, static_cast<char>(224)),
    std::string(1, static_cast<char>(225)),
    std::string(1, static_cast<char>(226)),
    std::string(1, static_cast<char>(227)),
    std::string(1, static_cast<char>(228)),
    std::string(1, static_cast<char>(229)),
    std::string(1, static_cast<char>(230)),
    std::string(1, static_cast<char>(231)),
    std::string(1, static_cast<char>(232)),
    std::string(1, static_cast<char>(233)),
    std::string(1, static_cast<char>(234)),
    std::string(1, static_cast<char>(235)),
    std::string(1, static_cast<char>(238)),
    std::string(1, static_cast<char>(239))
};

#endif
