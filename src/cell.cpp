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
}

void cell::setItem(item_t i)
{
	itm = i;
}

void cell::setBackdrop(backdrop_t b)
{
	backdrop = b;
}

void cell::setFire(fire_t f)
{
	fire = f;
}

void cell::setv(bool v)
{
	is_vis = v;
}

void cell::setm(bool m)
{
	in_mem = m;
}

void cell::save(std::ofstream& os) const
{
    os.write((const char *)&fire,sizeof(fire_t));
    os.write((const char *)&mob,sizeof(mob_t));
    os.write((const char *)&itm,sizeof(item_t));
    os.write((const char *)&backdrop,sizeof(backdrop_t));
    os.write((const char *)&in_mem,sizeof(bool));
    os.write((const char *)&is_vis,sizeof(bool));
}

void cell::load(std::ifstream &is)
{
    is.read((char *)&fire,sizeof(fire_t));
    is.read((char *)&mob,sizeof(mob_t));
    is.read((char *)&itm,sizeof(item_t));
    is.read((char *)&backdrop,sizeof(backdrop_t));
    is.read((char *)&in_mem,sizeof(bool));
    is.read((char *)&is_vis,sizeof(bool));
}
