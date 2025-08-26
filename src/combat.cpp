#include "combat.h"

damage_report damageShipMobFromBlastRadius(MobShip * attacker, MobShip *target, point origin, int hull_dam, int crew_dam)
{
    target->setMobIDLastAttackedBy(attacker->getMobSubAreaID());

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
    target->setMobIDLastAttackedBy(attacker->getMobSubAreaID());

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

bool checkNPCWeaponEvent(MobShip* attacker)
{
    const int targetID = attacker->getMobSubAreaAttackID();
    if (targetID < 0)
        return false;

    MobShip* target = getMobFromID(targetID);
    const auto& stats = attacker->getStatStruct();
    Module* weaponModule = getCurrentMobSelectedModule(attacker);
    const auto& weapon = weaponModule->getWeaponStruct();

    // Chance to switch weapons
    if (rollPerc(stats.weapon_change_chance) && attacker->getNumInstalledModulesOfType(MODULE_WEAPON) >= 2)
    {
        mobChangeSelectedWeapon(attacker);
        printShipmobWeaponEventMessage(attacker, "activates");
    }

    // Check if weapon can be used against target
    const bool inLineOfFire = isInStraightLine(target->at(), attacker->at()) || !weapon.eightDirectionRestricted;
    const int distance = shortestPath(target->at(), attacker->at());
    const bool inRange = distance <= weapon.travel_range;
    const bool hasAmmo = weaponModule->getFillQuantity() >= getWeaponModuleConsumptionPerTurn(weaponModule);
    const bool hasLineOfSight = !tracer.isBlocking(getMap(), attacker->at(), target->at(), false, false);
    const bool shouldFire = rollPerc(stats.shoot_frequency);

    if (inLineOfFire && inRange && hasAmmo && hasLineOfSight && shouldFire)
    {
        mobFire(attacker, target->at());
        return true;
    }

    return false;
}

void mobChangeSelectedWeapon(MobShip* mob)
{
    setRandomWeaponModule(mob);
}

void printShipmobWeaponEventMessage(MobShip* mob, const std::string& action)
{
    if (!getMap()->getv(mob->at()))
        return;

    Module* currentModule = getCurrentMobSelectedModule(mob);
    weapon_struct weapon = currentModule->getWeaponStruct();

    msgeAdd(getNamePrefix(mob), cp_grayonblack);
    msgeAdd(mob->getShipName(), mob->getShipSymbol().color);
    msgeAdd(action + " its", cp_grayonblack);
    msgeAdd(weapon.name_modifier + ".", weapon.disp_chtype.color);
}

void mobFire(MobShip* mb, point p)
{
    Module* weapon_mod = getCurrentMobSelectedModule(mb);

    weapon_struct current_ws = weapon_mod->getWeaponStruct();

    weapon_t selected_weapon = current_ws.wt;

    color_pair weapon_cp = current_ws.disp_chtype.color;

    int numShots = current_ws.num_shots;

    weapon_mod->offFillQuantity(-1 * current_ws.consumption_rate * numShots);

    printShipmobWeaponEventMessage(mb, "fires");

    for (int i = 0; i < numShots; ++i)
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
            msgeAdd("*ZZZAAAPPP*", weapon_cp);
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
        case(WEAPONTYPE_TOXICWALLOP):
        {
            msgeAdd("*VRRRRUPP*", weapon_cp);
            break;
        }
        case(WEAPONTYPE_ENERGYWALLOP):
        {
            msgeAdd("*Bzzz-choooom*", weapon_cp);
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

    for (int i = 0; i < numShots; ++i)
    {
        switch (selected_weapon)
        {
            case(WEAPONTYPE_MISSILE):
            case(WEAPONTYPE_TOXICWALLOP):
            case(WEAPONTYPE_ENERGYWALLOP):
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
            addHitSprite(getMap(), lof, getCurrentMobSelectedModule(mb)->getWeaponStruct().extender_line_disp);
        }

        damage_report dr = damageShipMob(mb, mob_in_lof, lof);

        if (mb->getMobType() == SHIP_PLAYER || mob_in_lof->getMobType() == SHIP_PLAYER)
        {
            displayDamageReport(dr, mb, mob_in_lof, false);
        }

        blast_radius = getCurrentMobSelectedModule(mb)->getWeaponStruct().blast_radius;

        checkCreateDamagingExplosion(lof, blast_radius, 15, FIRET_DAMAGINGEXPLOSION, true, dr, true);

        return !rollPerc(getCurrentMobSelectedModule(mb)->getWeaponStruct().travel_through_chance);
    }
    else
    {
        if ((mb->getMobType() == SHIP_PLAYER || mob_in_lof->getMobType() == SHIP_PLAYER) && mob_in_lof->isActivated())
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

    msgeAdd(getNamePrefix(mb) + " " + mb->getShipName() + " " + evasion_str, cp_grayonblack);
}

void endOfProjectileLoop(MobShip* mb, point lof, bool is_detectable, bool line_extender)
{
    if (is_detectable || line_extender)
        clearAllFireCells(getMap());

    if (terrainBlocked(getMap()->getBackdrop(lof)))
        checkCreateDamagingExplosionRollDamage(mb, lof, getCurrentMobSelectedModule(mb)->getWeaponStruct().blast_radius);
}

void checkIfRaceAggroEvent(MobShip* attacker, MobShip* target)
{
    int tgid = target->getMobSubAreaGroupID();
    int agid = attacker->getMobSubAreaGroupID();

    if (universe.getSubAreaMapType() == SMT_PERSISTENT)
        //if (currentRegion()->isRaceAffiliated())
        if (!target->isCurrentPlayerShip())
        {
            // player attack ship outside of home region and home region WILL
            // know about it
            if (attacker->isCurrentPlayerShip())
            {
                if (target->isActivated())
                {
                    race* targetRace = universe.getRace(tgid);
                    targetRace->incPlayerAttStatus(-1);
                    if (targetRace->getPlayerAttStatus() < 0 &&
                        targetRace->isSurrenderedToPlayer())
                    {
                        targetRace->setSurrenderedToPlayer(false);
                    }
                }
            }
            else if (agid != tgid && agid != currentRegion()->getNativeRaceID())
            {
                universe.getRace(tgid)->incRaceAttStatus(agid, -1);
            }
        }
}

void displayDamageReport(damage_report dr, MobShip* attacker, MobShip* target, bool explosiveDamage)
{
    if (!getMap()->getv(target->at()) || target->getHullStatus() + dr.hull_damage <= 0)
        return;

    const int shieldPerc = target->getTotalFillPercentageOfType(MODULE_SHIELD);
    const std::string& targetName = target->getShipName();
    const weapon_struct& weapon = getCurrentMobSelectedModule(attacker)->getWeaponStruct();

    std::string reportStr;
    color_pair colorMsg = weapon.disp_chtype.color;
    bool shieldDamage = false;

    auto emitShieldDown = [&](const std::string& msg) {
        msgeAdd(msg, cp_grayonblack);
        msgeAdd("Shields down!", cp_blueonblack);
        };

    if (!explosiveDamage)
    {
        switch (weapon.wt)
        {
        case WEAPONTYPE_BLAST:
            if (shieldPerc <= 0 && dr.shield_damage == 0)
                reportStr = "The " + targetName + "'s hull is damaged from the blast!";
            else
            {
                reportStr = targetName + " is hit.";
                shieldDamage = true;
                if (shieldPerc <= 0)
                    return emitShieldDown(reportStr);
            }
            break;

        case WEAPONTYPE_MECH:
            reportStr = "The razor tears through the hull of the " + targetName + "!";
            break;

        case WEAPONTYPE_BEAM:
            reportStr = "The beam cuts through the " + targetName + "'s hull!";
            break;

        case WEAPONTYPE_SPREAD:
            if (shieldPerc <= 0 && dr.shield_damage == 0)
                reportStr = "The " + targetName + " is burned!";
            else
            {
                reportStr = "The " + targetName + "'s shields are burned.";
                shieldDamage = true;
                if (shieldPerc <= 0)
                    return emitShieldDown(reportStr);
            }
            break;

        case WEAPONTYPE_HELL:
            reportStr = "The " + targetName + " is shredded by the thunderous blast!";
            break;

        case WEAPONTYPE_PULSE:
            if (shieldPerc <= 0 && dr.shield_damage == 0)
                reportStr = "The " + targetName + " is blasted!";
            else
            {
                reportStr = "The " + targetName + " is hit.";
                shieldDamage = true;
                if (shieldPerc <= 0)
                    return emitShieldDown(reportStr);
            }
            break;

        case WEAPONTYPE_TOXICWALLOP:
            reportStr = "The " + targetName + " is engulfed by the toxic fumes!";
            break;

        case WEAPONTYPE_ENERGYWALLOP:
            reportStr = "The " + targetName + " is blasted by the electricity";
            break;

        case WEAPONTYPE_MISSILE:
            reportStr = "The " + targetName + " is hit by the missile!";
            break;

        default:
            break;
        }
    }
    else
    {
        reportStr = "The " + targetName + " is within blast radius!";
        colorMsg = cp_orangeonblack;
    }

    msgeAdd(reportStr, shieldDamage ? cp_grayonblack : colorMsg);

    if (shieldDamage)
    {
        msgeAdd("Shields to " + int2String(shieldPerc) + "%!", cp_lightblueonblack);
        return;
    }

    if (dr.hull_damage > 0)
    {
        const int hullPerc = target->getHullStatus() > 0
            ? static_cast<int>((target->getHullStatus() / static_cast<double>(target->getMaxHull())) * 100.0)
            : 0;

        msgeAdd("Hull to " + int2String(hullPerc) + "%!", cp_grayonblack);
    }

    if (target->getHullStatus() > 0 && dr.crew_damage > 0 && target->getTotalMTFillRemaining(MODULE_CREW) > 0)
    {
        msgeAdd(int2String(dr.crew_damage) + " crew perished!", cp_greenonblack);
    }
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

void checkMobExplosionRadiusDamage(point center, int radius, damage_report baseDamage, bool causedByShip)
{
    if (radius < 2)
        return;

    MobShip * attacker = getCurrentMobTurn();

    const int range = radius - 1;

    for (int dy = -range; dy <= range; ++dy)
    {
        for (int dx = -range; dx <= range; ++dx)
        {
            // Skip the explosion center
            if (dx == 0 && dy == 0)
                continue;

            point target = addPoints(center, point(dx, dy));

            if (!inMapRange(target, getMapSize()))
                continue;

            MobShip* targetMob = getSubAreaShipMobAt(target);

            if (getMap()->getMob(target) != NIL_m && targetMob != nullptr)
            {
                if (causedByShip)
                    checkIfRaceAggroEvent(attacker, targetMob);

                damageShipMobFromBlastRadius(attacker, targetMob, center, baseDamage.hull_damage, baseDamage.crew_damage);
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
          endOfProjectileLoop(mb, lof, is_detectable, weapon.extender_line_disp);
          return;
        }
        
        point_iter--;
        primary_iter++;
      } while (point_iter >= 0);
      
      extrapolateLine(source, dest, getMapSize());
      
      if (tracer.getLineSize() <= 1)
      {
        if (is_detectable || weapon.extender_line_disp)
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

