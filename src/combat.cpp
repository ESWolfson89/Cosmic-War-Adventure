#include "combat.h"

damage_report damageShipMobFromBlastRadius(ship_mob *target, point origin, int hull_dam, int crew_dam)
{
    damage_report ret_val = {0,0,0};
    int dist = shortestPath(origin,target->at());

    if (dist <= 0)
        return ret_val;

    hull_dam /= (dist+1);
    crew_dam /= (dist+1);

    ret_val.hull_damage = hull_dam;
    target->setHullStatus(target->getHullStatus() - hull_dam);

    if (target->crewOperable())
    {
        ret_val.crew_damage = crew_dam;
        target->decTotalFillAmount(crew_dam,MODULE_CREW);
    }

    return ret_val;
}

// do damage to shields, hull, crew...
damage_report damageShipMob(ship_mob *attacker, ship_mob *target, point lof)
{
    damage_report ret_val = {0,0,0};
    module *weapon_mod = getCurrentMobSelectedModule(attacker);

    int hull_damage = rollMultipleDice(weapon_mod->getWeaponStruct().hull_damage);
    int crew_damage = rollMultipleDice(weapon_mod->getWeaponStruct().crew_damage);
    int shield_damage = weapon_mod->getWeaponStruct().shield_damage_count;

    if (!weapon_mod->getWeaponStruct().ignores_shields)
    if (target->getTotalMTFillRemaining(MODULE_SHIELD) > 0)
    {
        target->decTotalFillAmount(shield_damage,MODULE_SHIELD);
        ret_val.shield_damage = shield_damage;
        return ret_val;
    }

    if (rollPerc(weapon_mod->getWeaponStruct().anti_hull_crit_perc))
        hull_damage *= weapon_mod->getWeaponStruct().anti_hull_crit_multiplier;

    ret_val.hull_damage = hull_damage;
    target->setHullStatus(target->getHullStatus() - hull_damage);

    // ship is not automated
    if (target->crewOperable())
    {
        if (rollPerc(weapon_mod->getWeaponStruct().anti_personnel_crit_perc))
            crew_damage *= weapon_mod->getWeaponStruct().anti_personnel_crit_multiplier;

        ret_val.crew_damage = crew_damage;
        target->decTotalFillAmount(crew_damage,MODULE_CREW);
    }

    return ret_val;
}

// chance of successful weapon hit
int baseHitChance(ship_mob *attacker, ship_mob *target)
{
    int acc = (int)attacker->getAccuracy();

    int eva = (int)target->getEvasion();

    if (acc < 1 )
        acc = 1;

    if (eva < 1)
        eva = 1;

    if (!target->isActivated())
        return 0;

    return randInt(0,acc) - randInt(0,eva);
}

int rollMultipleDice(dice_roll d)
{
    int ret_val = 0;
    for (uint i = 0; i < d.num_dice; ++i)
    {
        ret_val += randInt(1,d.num_sides);
    }
    ret_val += d.num_bonus;
    return ret_val;
}

module *getCurrentMobSelectedModule(ship_mob *mb)
{
    return mb->getModule(mb->getModuleSelectionIndex());
}

