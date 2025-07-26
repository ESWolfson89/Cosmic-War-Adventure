#ifndef COMBAT_H_
#define COMBAT_H_

#include "mob.h"
#include "output.h"
#include "namegen.h"
#include "region.h"
#include "line.h"

struct damage_report
{
    int shield_damage;
    int crew_damage;
    int hull_damage;
};

void outputLOFTransition(point, point, point, fire_t);

damage_report damageShipMobFromBlastRadius(MobShip *,point,int,int);

damage_report damageShipMob(MobShip *, MobShip *, point);

int baseHitChance(MobShip *, MobShip *);

int rollMultipleDice(dice_roll);

module *getCurrentMobSelectedModule(MobShip *);

void mobChangeSelectedWeapon(MobShip*);

bool checkNPCWeaponEvent(MobShip*);

void printShipmobWeaponEventMessage(MobShip*, std::string);

void mobFire(MobShip*, point);

void mobShootSingleProjectile(MobShip *, point);

bool checkForMobInLOF(MobShip* mb, point lof, bool add_hitsprite, bool guaranteed_hit);

void displayEvasionReport(int, MobShip*);

void endOfProjectileLoop(MobShip*, point, bool);

void checkIfRaceAggroEvent(MobShip*, MobShip*);

void displayDamageReport(damage_report, MobShip*, bool);

void checkCreateDamagingExplosionRollDamage(MobShip*, point, int);

void checkCreateDamagingExplosion(point, int, int, fire_t, bool, damage_report, bool);

void checkMobExplosionRadiusDamage(point, int, damage_report, bool);

void mobShootSpread(MobShip* mb, point dest, int width);
void mobShootPulse(MobShip* mb, point dest);



#endif
