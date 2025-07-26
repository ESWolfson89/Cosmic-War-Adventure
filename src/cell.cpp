#include "cell.h"

// see .h file for more info
// NIL values are enum 0 for mob_t, item_t,
// backdrop_t and fire_t enums
cell::cell()
{
	mob = NIL_m;
	itm = NIL_i;
	backdrop = NIL_b;
	fire = NIL_f;
	in_mem = false;
	is_vis = false;
	currentFireSymbol = blank_ch;
	currentMobSymbol = blank_ch;
	currentItemSymbol = blank_ch;
	currentBackdropSymbol = blank_ch;
	lastFireSymbol = blank_ch;
	lastMobSymbol = blank_ch;
	lastItemSymbol = blank_ch;
	lastBackdropSymbol = blank_ch;
}

mob_t cell::getMob()
{
	return mob;
}

item_t cell::getItem()
{
	return itm;
}

backdrop_t cell::getBackdrop()
{
	return backdrop;
}

fire_t cell::getFire()
{
	return fire;
}

bool cell::getv()
{
	return is_vis;
}

bool cell::getm()
{
	return in_mem;
}

void cell::setMob(mob_t m)
{
	mob = m;
	currentMobSymbol = mob_symbol[(int)m];
}

void cell::setItem(item_t i)
{
	itm = i;
	currentItemSymbol = item_symbol[(int)i];
}

void cell::setBackdrop(backdrop_t b)
{
	backdrop = b;
	currentBackdropSymbol = backdrop_symbol[(int)b];
}

void cell::setFire(fire_t f)
{
	fire = f;
	currentFireSymbol = fire_symbol[(int)f];
}

void cell::setv(bool v)
{
	is_vis = v;
}

void cell::setm(bool m)
{
	in_mem = m;
}

void cell::setCurrentFireSymbol(chtype symbol)
{
	currentFireSymbol = symbol;
}

void cell::setCurrentMobSymbol(chtype symbol)
{
	currentMobSymbol = symbol;
}

void cell::setCurrentItemSymbol(chtype symbol)
{
	currentItemSymbol = symbol;
}

void cell::setCurrentBackdropSymbol(chtype symbol)
{
	currentBackdropSymbol = symbol;
}

void cell::setLastFireSymbol(chtype symbol)
{
	lastFireSymbol = symbol;
}

void cell::setLastMobSymbol(chtype symbol)
{
	lastMobSymbol = symbol;
}

void cell::setLastItemSymbol(chtype symbol)
{
	lastItemSymbol = symbol;
}

void cell::setLastBackdropSymbol(chtype symbol)
{
	lastBackdropSymbol = symbol;
}

chtype cell::getCurrentFireSymbol()
{
	return currentFireSymbol;
}

chtype cell::getCurrentMobSymbol()
{
	return currentMobSymbol;
}

chtype cell::getCurrentItemSymbol()
{
	return currentItemSymbol;
}

chtype cell::getCurrentBackdropSymbol()
{
	return currentBackdropSymbol;
}

chtype cell::getLastFireSymbol()
{
	return lastFireSymbol;
}

chtype cell::getLastMobSymbol()
{
	return lastMobSymbol;
}

chtype cell::getLastItemSymbol()
{
	return lastItemSymbol;
}

chtype cell::getLastBackdropSymbol()
{
	return lastBackdropSymbol;
}

void cell::save(std::ofstream& os) const
{
	os.write(reinterpret_cast<const char*>(&fire), sizeof(fire_t));
	os.write(reinterpret_cast<const char*>(&mob), sizeof(mob_t));
	os.write(reinterpret_cast<const char*>(&itm), sizeof(item_t));
	os.write(reinterpret_cast<const char*>(&backdrop), sizeof(backdrop_t));
	os.write(reinterpret_cast<const char*>(&in_mem), sizeof(bool));
	os.write(reinterpret_cast<const char*>(&is_vis), sizeof(bool));

	chtypeSave(os, currentFireSymbol);
	chtypeSave(os, currentMobSymbol);
	chtypeSave(os, currentItemSymbol);
	chtypeSave(os, currentBackdropSymbol);
	chtypeSave(os, lastFireSymbol);
	chtypeSave(os, lastMobSymbol);
	chtypeSave(os, lastItemSymbol);
	chtypeSave(os, lastBackdropSymbol);
}

void cell::load(std::ifstream& is)
{
	is.read(reinterpret_cast<char*>(&fire), sizeof(fire_t));
	is.read(reinterpret_cast<char*>(&mob), sizeof(mob_t));
	is.read(reinterpret_cast<char*>(&itm), sizeof(item_t));
	is.read(reinterpret_cast<char*>(&backdrop), sizeof(backdrop_t));
	is.read(reinterpret_cast<char*>(&in_mem), sizeof(bool));
	is.read(reinterpret_cast<char*>(&is_vis), sizeof(bool));

	chtypeLoad(is, currentFireSymbol);
	chtypeLoad(is, currentMobSymbol);
	chtypeLoad(is, currentItemSymbol);
	chtypeLoad(is, currentBackdropSymbol);
	chtypeLoad(is, lastFireSymbol);
	chtypeLoad(is, lastMobSymbol);
	chtypeLoad(is, lastItemSymbol);
	chtypeLoad(is, lastBackdropSymbol);
}