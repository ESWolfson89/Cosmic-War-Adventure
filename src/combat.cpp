#include "combat.h"

damage_report damageShipMobFromBlastRadius(MobShip *target, point origin, int hull_dam, int crew_dam)
{
    damage_report ret_val = {0,0,0};
    
    int dist = shortestPath(origin,target->at());

    if (dist <= 0)
    {
        return ret_val;
    }

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
damage_report damageShipMob(MobShip *attacker, MobShip *target, point lof)
{
    damage_report ret_val = {0,0,0};
    Module *weapon_mod = getCurrentMobSelectedModule(attacker);

    int hull_damage = rollMultipleDice(weapon_mod->getWeaponStruct().hull_damage);
    int crew_damage = rollMultipleDice(weapon_mod->getWeaponStruct().crew_damage);
    int shield_damage = weapon_mod->getWeaponStruct().shield_damage_count;

    if (!weapon_mod->getWeaponStruct().ignores_shields)
    {
        if (target->getTotalMTFillRemaining(MODULE_SHIELD) > 0)
        {
            target->decTotalFillAmount(shield_damage, MODULE_SHIELD);
            ret_val.shield_damage = shield_damage;
            return ret_val;
        }
    }

    if (rollPerc(weapon_mod->getWeaponStruct().anti_hull_crit_perc))
    {
        hull_damage *= weapon_mod->getWeaponStruct().anti_hull_crit_multiplier;
    }

    ret_val.hull_damage = hull_damage;
    target->setHullStatus(target->getHullStatus() - hull_damage);

    // ship is not automated
    if (target->crewOperable())
    {
        if (rollPerc(weapon_mod->getWeaponStruct().anti_personnel_crit_perc))
        {
            crew_damage *= weapon_mod->getWeaponStruct().anti_personnel_crit_multiplier;
        }

        ret_val.crew_damage = crew_damage;

        target->decTotalFillAmount(crew_damage,MODULE_CREW);
    }

    return ret_val;
}

// chance of successful weapon hit
int baseHitChance(MobShip *attacker, MobShip *target)
{
    int accuracy = (int)attacker->getAccuracy();

    int evasion = (int)target->getEvasion();

    accuracy = accuracy >= 1 ? accuracy : 1;
    evasion = evasion >= 1 ? evasion : 1;

    if (!target->isActivated())
    {
        return 0;
    }

    return randInt(0,accuracy) - randInt(0,evasion);
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

Module *getCurrentMobSelectedModule(MobShip *mb)
{
    return mb->getModule(mb->getModuleSelectionIndex());
}

bool checkNPCWeaponEvent(MobShip* mb)
{
    int attack_id = mb->getMobSubAreaAttackID();
    if (attack_id >= 0)
    {
        MobShip* mob_being_attacked = getMobFromID(attack_id);

        const int weaponChangeChance = mb->getStatStruct().weapon_change_chance;

        if (rollPerc(weaponChangeChance))
            if (mb->getNumInstalledModulesOfType(MODULE_WEAPON) >= 2)
            {
                mobChangeSelectedWeapon(mb);
                printShipmobWeaponEventMessage(mb, "activates");
            }

        if (isInStraightLine(mob_being_attacked->at(), mb->at()) || !getCurrentMobSelectedModule(mb)->getWeaponStruct().eightDirectionRestricted)
            if (shortestPath(mob_being_attacked->at(), mb->at()) <= getCurrentMobSelectedModule(mb)->getWeaponStruct().travel_range)
                if (getCurrentMobSelectedModule(mb)->getFillQuantity() >= getWeaponModuleConsumptionPerTurn(getCurrentMobSelectedModule(mb)))
                    if (!tracer.isBlocking(getMap(), mb->at(), mob_being_attacked->at(), false, false))
                        if (rollPerc(mb->getStatStruct().shoot_frequency))
                        {
                            mobFire(mb, mob_being_attacked->at());
                            return true;
                        }
    }
    return false;
}

void mobChangeSelectedWeapon(MobShip* mb)
{
    int module_index;

    do
    {
        module_index = randInt(0, mb->getNumInstalledModules() - 1);
    } while (module_index == mb->getModuleSelectionIndex() || mb->getModule(module_index)->getModuleType() != MODULE_WEAPON);

    mb->setModuleSelectionIndex(module_index);
}

void printShipmobWeaponEventMessage(MobShip* mb, std::string action_str)
{
    if (!getMap()->getv(mb->at()))
    {
        return;
    }
    msgeAdd(getNamePrefix(mb), cp_grayonblack);
    msgeAdd(mb->getShipName(), mb->getShipSymbol().color);
    msgeAdd(action_str + " its", cp_grayonblack);
    msgeAdd(getCurrentMobSelectedModule(mb)->getWeaponStruct().name_modifier + ".",
        getCurrentMobSelectedModule(mb)->getWeaponStruct().disp_chtype.color);
}

void mobFire(MobShip* mb, point p)
{
    Module* weapon_mod = getCurrentMobSelectedModule(mb);

    weapon_struct current_ws = weapon_mod->getWeaponStruct();

    weapon_t selected_weapon = current_ws.wt;

    printShipmobWeaponEventMessage(mb, "fires");

    color_pair weapon_cp = current_ws.disp_chtype.color;

    weapon_mod->offFillQuantity(-1 * current_ws.consumption_rate * current_ws.num_shots);

    for (int i = 0; i < weapon_mod->getWeaponStruct().num_shots; ++i)
    {
        switch (selected_weapon)
        {
        case(WEAPONTYPE_BLAST):
        {
            msgeAdd("*choom!*", weapon_cp);
            break;
        }
        case(WEAPONTYPE_BEAM):
        {
            msgeAdd("*VREEEEEEEE p-chooooooom*", weapon_cp);
            break;
        }
        case(WEAPONTYPE_MISSILE):
        {
            msgeAdd("*chhhuuggg*", weapon_cp);
            break;
        }
        case(WEAPONTYPE_SPREAD):
        {
            msgeAdd("*FWOOOOSHHHH!*", weapon_cp);
            break;
        }
        case(WEAPONTYPE_PULSE):
        {
            msgeAdd("*VREEEEEEEEE!*", weapon_cp);
            break;
        }
        case(WEAPONTYPE_WALLOP):
        {
            msgeAdd("*VRRRRUPP*", weapon_cp);
            break;
        }
        case(WEAPONTYPE_MECH):
        {
            msgeAdd("*VRRRRRUUUMMMM*", weapon_cp);
            break;
        }
        case(WEAPONTYPE_HELL):
        {
            msgeAdd("*VWAAAAAAAAAAAA*", weapon_cp);
            break;
        }
        default:
            break;
        }
    }

    for (int i = 0; i < weapon_mod->getWeaponStruct().num_shots; ++i)
    {
        switch (selected_weapon)
        {
        case(WEAPONTYPE_MISSILE):
        case(WEAPONTYPE_WALLOP):
        case(WEAPONTYPE_MECH):
        case(WEAPONTYPE_BLAST):
        case(WEAPONTYPE_BEAM):
        case(WEAPONTYPE_HELL):
        {
            mobShootSingleProjectile(mb, p);
            break;
        }
        case(WEAPONTYPE_SPREAD):
        {
            mobShootSpread(mb, p, 3);
            break;
        }
        case(WEAPONTYPE_PULSE):
        {
            mobShootPulse(mb, p);
            break;
        }
        default:
            break;
        }
    }
}

// returns true if fire projectile/laser/wallop etc... hits something
bool checkForMobInLOF(MobShip* mb, point lof, bool add_hitsprite, bool guaranteed_hit)
{
    if (getMap()->getMob(lof) == NIL_m)
        return false;

    MobShip* mob_in_lof = getSubAreaShipMobAt(lof);

    checkIfRaceAggroEvent(mb, mob_in_lof);

    int accuracy_val;
    int blast_radius = 0;

    accuracy_val = baseHitChance(mb, mob_in_lof);

    // base hit chance determines if it was a hit or a miss
    if (accuracy_val >= 0 || guaranteed_hit)
    {
        if (add_hitsprite && getMap()->getv(lof))
        {
            addHitSprite(getMap(), lof);
        }

        damage_report dr = damageShipMob(mb, mob_in_lof, lof);

        if (mb->getMobType() == SHIP_PLAYER || mob_in_lof->getMobType() == SHIP_PLAYER)
        {
            displayDamageReport(dr, mob_in_lof, false);
        }

        blast_radius = getCurrentMobSelectedModule(mb)->getWeaponStruct().blast_radius;

        checkCreateDamagingExplosion(lof, blast_radius, 15, FIRET_DAMAGINGEXPLOSION, true, dr, true);

        return !rollPerc(getCurrentMobSelectedModule(mb)->getWeaponStruct().travel_through_chance);
    }
    else
    {
        if (mb->getMobType() == SHIP_PLAYER || mob_in_lof->getMobType() == SHIP_PLAYER)
        {
            displayEvasionReport(accuracy_val, mob_in_lof);
        }
    }

    return false;
}

void displayEvasionReport(int acc_val, MobShip* mb)
{
    if (!getMap()->getv(mb->at()))
        return;

    if (!mb->isActivated() || mb->getHullStatus() <= 0)
        return;

    std::string evasion_str = "";

    if (acc_val <= -75)
        evasion_str = "effortlessly dodges.";
    else if (acc_val <= -50)
        evasion_str = "very easily dodges.";
    else if (acc_val <= -25)
        evasion_str = "easily dodges.";
    else if (acc_val <= -15)
        evasion_str = "dodges.";
    else if (acc_val <= -5)
        evasion_str = "narrowly dodges.";
    else
        evasion_str = "just barely dodges.";

    msgeAdd(getNamePrefix(mb) + " " + mb->getShipName() + " " + evasion_str, mb->getShipSymbol().color);
}

void endOfProjectileLoop(MobShip* mb, point lof, bool is_detectable)
{
    if (is_detectable)
        clearAllFireCells(getMap());

    if (terrainBlocked(getMap()->getBackdrop(lof)))
        checkCreateDamagingExplosionRollDamage(mb, lof, getCurrentMobSelectedModule(mb)->getWeaponStruct().blast_radius);
}

void checkIfRaceAggroEvent(MobShip* attacker, MobShip* target)
{
    int tgid = target->getMobSubAreaGroupID();
    int agid = attacker->getMobSubAreaGroupID();

    if (universe.getSubAreaMapType() == SMT_PERSISTENT)
        //if (CSYS->isRaceAffiliated())
        if (!target->isCurrentPlayerShip())
        {
            // player attack ship outside of home region and home region WILL
            // know about it
            if (attacker->isCurrentPlayerShip())
            {
                if (target->isActivated())
                    universe.getRace(tgid)->incPlayerAttStatus(-1);
            }
            else if (agid != tgid && agid != CSYS->getNativeRaceID())
            {
                universe.getRace(tgid)->incRaceAttStatus(agid, -1);
            }
        }
}

void displayDamageReport(damage_report dr, MobShip* mb, bool exp_damage)
{
    if (!getMap()->getv(mb->at()))
        return;

    if (mb->getHullStatus() + dr.hull_damage <= 0)
        return;

    std::string report_str = mb->getShipName();

    std::string hull_perc_str = "";

    if (!exp_damage)
        report_str += " hit!";
    else
        report_str += " within blast radius!";

    int shield_perc = mb->getTotalFillPercentageOfType(MODULE_SHIELD);

    if (mb->getHullStatus() > 0)
        hull_perc_str = int2String((int)(((double)mb->getHullStatus() / (double)mb->getMaxHull()) * 100.0));
    else
        hull_perc_str = int2String(0);

    if (dr.shield_damage > 0)
    {
        report_str += " Shields ";
        if (shield_perc <= 0)
            report_str += "down!";
        else
            report_str += "to " + int2String(shield_perc) + "%!";

        msgeAdd(report_str, mb->getShipSymbol().color);
        return;
    }

    if (dr.hull_damage > 0)
    {
        report_str += " Hull to " + hull_perc_str + "%!";
    }

    if (mb->getHullStatus() > 0 && dr.crew_damage > 0 && mb->getTotalMTFillRemaining(MODULE_CREW) > 0)
    {
        report_str += " " + int2String(dr.crew_damage) + " crew perished!";
    }

    msgeAdd(report_str, mb->getShipSymbol().color);
}

void checkCreateDamagingExplosion(point lof, int blast_radius, int ms_delay, fire_t ft, bool print_msge, damage_report dr, bool caused_by_ship)
{
    if (blast_radius > 0 && inMapRange(lof, getMapSize()))
    {
        createDamagingExplosionAnimation(lof, blast_radius, ms_delay, ft, print_msge);
        checkMobExplosionRadiusDamage(lof, blast_radius, dr, caused_by_ship);
    }
}

void checkCreateDamagingExplosionRollDamage(MobShip* mb, point lof, int blast_radius)
{
    if (blast_radius <= 0)
        return;

    clearAllFireCellsInRange(getMap(), lof, 1);
    damage_report dr = { 0,0,0 };
    Module* weapon_mod = getCurrentMobSelectedModule(mb);
    dr.hull_damage = rollMultipleDice(weapon_mod->getWeaponStruct().hull_damage);
    dr.crew_damage = rollMultipleDice(weapon_mod->getWeaponStruct().crew_damage);
    checkCreateDamagingExplosion(lof, blast_radius, 15, FIRET_DAMAGINGEXPLOSION, false, dr, true);
}

// details to follow regarding mechanic here...
// mradius must be 2 or more
void checkMobExplosionRadiusDamage(point p, int mradius, damage_report dr, bool causedByShip)
{

    point loc;
    damage_report dr_mod;

    if (mradius < 2)
    {
        return;
    }

    for (int i = -(mradius - 1); i <= (mradius - 1); ++i)
    {
        for (int j = -(mradius - 1); j <= (mradius - 1); ++j)
        {
            loc.set(i + p.x(), j + p.y());
            if (inMapRange(loc, getMapSize()))
            {
                if (!(i == 0 && j == 0))
                {
                    if (getMap()->getMob(loc) != NIL_m)
                    {
                        if (causedByShip)
                        {
                            checkIfRaceAggroEvent(getCurrentMobTurn(), getSubAreaShipMobAt(loc));
                        }
                        dr_mod = damageShipMobFromBlastRadius(getSubAreaShipMobAt(loc), p, dr.hull_damage, dr.crew_damage);
                    }
                }
            }
        }
    }
}

void mobShootSingleProjectile(MobShip* mb, point dest)
{
    Module* current_mod = getCurrentMobSelectedModule(mb);
    const auto& weapon = current_mod->getWeaponStruct();
    point source = mb->at(), lof = point(1, 1);
    int point_iter = 0;
    int primary_iter = 0;
    int range = weapon.travel_range;
    bool is_detectable = (getMap()->getv(mb->at()) || isAt(dest, getPlayerShip()->at()));
    bool hitmob_condition;

    tracer.bresenham(getMapSize(), source, dest);

    do
    {
      point_iter = tracer.getLineSize() - 2;
      
      do
      {
        lof = tracer.getLinePoint(point_iter);
        
        if (is_detectable)
        {
          outputLOFTransition(lof, mb->at(), dest, weapon.ftile, weapon.extender_line_disp, weapon.delay_value);
        }
        
        hitmob_condition = checkForMobInLOF(mb, lof, true, false);
        
        if (hitmob_condition || primary_iter == range - 1 || terrainBlocked(getMap()->getBackdrop(lof)))
        {
          endOfProjectileLoop(mb, lof, is_detectable);
          return;
        }
        
        point_iter--;
        primary_iter++;
      } while (point_iter >= 0);
      
      extrapolateLine(source, dest, getMapSize());
      
      if (tracer.getLineSize() <= 1)
      {
        if (is_detectable)
        {
          clearAllFireCells(getMap());
        }
        return;
      }
    } while (1);
}


// shoot spread burst sprites that are width tiles wide
void mobShootSpread(MobShip* mb, point dest, int width)
{
    Module* current_mod = getCurrentMobSelectedModule(mb);
    const auto& weapon = current_mod->getWeaponStruct();
    int range = weapon.travel_range;

    point source = mb->at();
    point delta(dest.x() - source.x(), dest.y() - source.y());
    point adjp = std::abs(delta.y()) > std::abs(delta.x()) ? point(1, 0) : point(0, 1);

    std::vector<point> lof(width);
    std::vector<bool> block(width, false);

    bool is_detectable = getMap()->getv(source) || isAt(dest, getPlayerShip()->at());

    tracer.bresenham(getMapSize(), source, dest);
    int point_iter = tracer.getLineSize() - 2;
    int dist_counter = 0;

    bool all_blocked = true;

    do
    {
      dest = tracer.getLinePoint(point_iter);
      
      const int half_width = width / 2;

      for (int i = 0; i < width; ++i) 
      {
          int offset = i - half_width;
          lof[i] = point(dest.x() + offset * adjp.x(), dest.y() + offset * adjp.y());
      }

      for (int i = 0; i < width; ++i) 
      {
          if (block[i]) continue;

          const point& p = lof[i];

          if (!inMapRange(p, getMapSize()) || terrainBlocked(getMap()->getBackdrop(p))) 
          {
              block[i] = true;
              continue;
          }

          if (is_detectable) 
          {
              printAndSetFireCell(getMap(), p, weapon.ftile);
          }

          checkForMobInLOF(mb, p, false, false);
      }
      
      if (is_detectable)
      {
        display_obj.delayAndUpdate(weapon.delay_value);
      }
      
      dist_counter++;
      
      point_iter--;
      
      if (point_iter == -1)
      {
        extrapolateLine(source, dest, getMapSize());
      
        if (!inMapRange(dest, getMapSize()))
        {
          break;
        }
      
        point_iter = tracer.getLineSize() - 2;
      }
      
      all_blocked = std::all_of(block.begin(), block.end(), [](bool b) { return b; });

    } while ((dist_counter < range) && !all_blocked);

    if (is_detectable)
    {
      clearAllFireCells(getMap());
    }
}

void mobShootPulse(MobShip* mb, point dest)
{
    Module* current_mod = getCurrentMobSelectedModule(mb);
    const auto& weapon = current_mod->getWeaponStruct();

    bool end_hit = false;
    bool hit_being = false;

    point lof, lof_hit, source = mb->at();

    tracer.bresenham(getMapSize(), source, dest);

    int point_iter = tracer.getLineSize() - 1;

    int max_dist = weapon.travel_range;

    int dist_counter = 0;

    bool is_detectable = (getMap()->getv(mb->at()) || isAt(dest, getPlayerShip()->at()));

    do
    {
      // condition: fire_t has NOT hit something
      if (!end_hit)
      {
        if (point_iter == 0)
        {
          extrapolateLine(source, dest, getMapSize());
          if (tracer.getLineSize() <= 1)
          {
              break;
          }
          point_iter = tracer.getLineSize() - 1;
        }
        point_iter--;
        lof = tracer.getLinePoint(point_iter);
       
        if (checkForMobInLOF(mb, lof, false, false))
        {
          hit_being = true;
        }
       
        if (hit_being || dist_counter >= max_dist || terrainBlocked(getMap()->getBackdrop(lof)))
        {
          lof_hit = lof;
          tracer.bresenham(getMapSize(), mb->at(), lof_hit);
          point_iter = tracer.getLineSize() - 1;
          end_hit = true;
        }
       
        if (is_detectable)
        {
          printAndSetFireCell(getMap(), lof, current_mod->getWeaponStruct().ftile);
          display_obj.delayAndUpdate(weapon.delay_value);
        }
      }
      // it has hit something or reached end
      else
      {
        if (point_iter <= 0)
        {
            break;
        }
      
        lof = tracer.getLinePoint(point_iter);
      
        //set farthest cell from enemy/player with "t" fire_t value to NIL
        if (is_detectable)
        {
          clearAllFireCellsInRange(getMap(), lof, 1);
          display_obj.delayAndUpdate(15);
        }
      
        if (hit_being)
        {
          checkForMobInLOF(mb, lof_hit, false, true);
        }
      
        point_iter--;
      }
      dist_counter++;
    } while (1);

    if (is_detectable)
    {
      clearAllFireCells(getMap());
    }
}

