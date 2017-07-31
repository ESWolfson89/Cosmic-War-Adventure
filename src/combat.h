#ifndef COMBAT_H_
#define COMBAT_H_

#include "mob.h"

struct damage_report
{
    int shield_damage;
    int crew_damage;
    int hull_damage;
};

damage_report damageShipMobFromBlastRadius(ship_mob *,point,int,int);
damage_report damageShipMob(ship_mob *, ship_mob *, point);
int baseHitChance(ship_mob *, ship_mob *);
int rollMultipleDice(dice_roll);
module *getCurrentMobSelectedModule(ship_mob *);

#endif
