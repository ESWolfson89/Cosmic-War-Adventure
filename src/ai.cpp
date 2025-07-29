#include "ai.h"

void activateAllNPCAI()
{
    if (current_maptype != MAPTYPE_LOCALEMAP || !playerHasMoved)
        return;

    gmti = 0;

    const double playerSpeed = getPlayerShip()->getSpeed();
    const int initialNPCCount = CSYS->getNumShipNPCs();

    while (gmti < CSYS->getNumShipNPCs())
    {
        MobShip* npc = CSYS->getNPCShip(gmti);
        if (!npc || !npc->isActivated())
        {
            ++gmti;
            continue;
        }

        // Update turn timer based on relative speed
        double updatedTimer = npc->getTurnTimer() + (npc->getSpeed() / playerSpeed);
        npc->setTurnTimer(updatedTimer);

        if (npc->inTimerRange())
            npc->setTurnTimer(0.0);

        // Process as many actions as turn timer allows
        while (npc->getTurnTimer() > 0.0)
        {
            npc->decrementTurnTimer();
            activateOneNPC(npc);

            // If the number of NPCs changed (e.g. spawned/destroyed), abort loop
            if (CSYS->getNumShipNPCs() != initialNPCCount)
                return;
        }

        ++gmti;
    }
}

void activateOneNPC(MobShip* mb)
{
    current_mob_turn = mb->getMobSubAreaID();

    checkNPCFlags(mb);
    checkNPCAggroEvent(mb);
    setNPCAIPattern(mb);
    setNPCGoalDestinationLoc(mb);

    if (checkNPCWeaponEvent(mb))
        return;

    checkNPCMoveEvent(mb);
}

void checkNPCFlags(MobShip* mb)
{
    checkMobHasEngine(mb);
    checkNPCPlanetEnslaveEvent(mb);
}

void setNPCAIPattern(MobShip* mob)
{
    if (!mob)
        return;

    const int attackTargetID = mob->getMobSubAreaAttackID();

    if (attackTargetID >= 0)
    {
        module* weaponMod = getCurrentMobSelectedModule(mob);
        if (weaponMod && weaponMod->getFillQuantity() >= getWeaponModuleConsumptionPerTurn(weaponMod))
            mob->setAIPattern(AIPATTERN_ATTACKING);
        else
            mob->setAIPattern(AIPATTERN_FLEEING);
    }
    else
    {
        const int roveChance = mob->getStatStruct().rove_chance;
        mob->setAIPattern(rollPerc(roveChance) ? AIPATTERN_ROVING : AIPATTERN_NEUTRAL);
    }

    checkNPCPlanetMoveEvent(mob);
}

void setNPCGoalDestinationLoc(MobShip* mob)
{
    if (!mob)
        return;

    switch (mob->getAIPattern())
    {
        case AIPATTERN_NEUTRAL:
            mob->setDestination(mob->getInitLoc());
            mob->setGoalStatus(GOALSTATUS_COMPLETE);
            break;
        
        case AIPATTERN_ATTACKING:
            setNPCAttackPositionDestination(mob);
            mob->setGoalStatus(GOALSTATUS_COMPLETE);
            break;
        
        case AIPATTERN_ROVING:
        case AIPATTERN_FLEEING:
            if (mob->getGoalStatus() == GOALSTATUS_COMPLETE)
            {
                setNPCRandDestination(mob);
                mob->setGoalStatus(GOALSTATUS_INCOMPLETE);
            }
            break;
        
        case AIPATTERN_APPROACHPLANET:
            if (mob->getGoalStatus() == GOALSTATUS_COMPLETE)
            {
                setNPCRandFreePlanetDestination(mob);
                mob->setGoalStatus(GOALSTATUS_INCOMPLETE);
            }
            break;
        
        default:
            // Unknown or unhandled pattern — do nothing
            break;
    }
}

void setNPCRandDestination(MobShip* mob)
{
    if (!mob) 
        return;

    const point minBound(1, 1);
    const point maxBound = addPoints(getMapSize(), point(-2, -2));

    const point destination = getRandNPCShipOpenPoint(
        getMap(), minBound, maxBound, LBACKDROP_SPACE_UNLIT, LBACKDROP_WHITESTARBACKGROUND
    );

    mob->setDestination(destination);
}

void setNPCRandFreePlanetDestination(MobShip* mob)
{
    if (!mob) 
        return;

    const int nativeRaceID = CSYS->getNativeRaceID();
    const point destination = universe.getRace(nativeRaceID)->getFirstFreeHomeworldLoc();

    mob->setDestination(destination);
}

void setNPCAttackPositionDestination(MobShip* mob)
{
    if (!mob) 
        return;

    module * weaponMod = getCurrentMobSelectedModule(mob);
    if (!weaponMod) 
        return;

    weapon_struct weapon = weaponMod->getWeaponStruct();

    const point target = weapon.eightDirectionRestricted
        ? getClosestNPCTargetInRangeLine(mob, weapon.travel_range)
        : getMobFromID(mob->getMobSubAreaAttackID())->at();

    mob->setDestination(target);
}

point getClosestNPCTargetInRangeLine(MobShip* mob, int travelRange)
{
    const point attackLoc = getMobFromID(mob->getMobSubAreaAttackID())->at();
    const point origin = mob->at();

    std::vector<point> candidatePoints;
    std::vector<int> distancesSquared;

    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            if (dx == 0 && dy == 0)
                continue;

            const point target = addPoints(attackLoc, point(dx * travelRange, dy * travelRange));

            if (!inRange(target, point(0, 0), point(STARMAPWID - 1, STARMAPHGT - 1)))
                continue;

            candidatePoints.push_back(target);
            distancesSquared.push_back(distanceSquared(origin, target));
        }
    }

    if (candidatePoints.empty())
        return attackLoc;

    auto minIt = std::min_element(distancesSquared.begin(), distancesSquared.end());
    const size_t index = std::distance(distancesSquared.begin(), minIt);

    return candidatePoints[index];
}

void checkNPCMoveEvent(MobShip* mob)
{
    if (!mob || !mob->getMoveState())
        return;

    const point source = mob->at();
    const point dest = mob->getDestination();

    if (isAt(source, dest))
    {
        mob->setGoalStatus(GOALSTATUS_COMPLETE);
        return;
    }

    const bool inDetectionRange = (shortestPath(source, dest) <= mob->getDetectRadius());
    const bool notAttacking = (mob->getAIPattern() != AIPATTERN_ATTACKING);

    if (!inDetectionRange && !notAttacking)
        return;

    const bool isBlocked = tracer.isBlocking(getMap(), source, dest, true, false);
    const auto& mapSize = getMap()->getSize();
    const bool notOnBorder = notOnMapBorder(source, mapSize) && notOnMapBorder(dest, mapSize);

    const point nextLoc = (isBlocked && notOnBorder)
        ? pathfinder.dijkstra(getMap(), source, dest)
        : tracer.getLinePoint(tracer.getLineSize() - 2);

    moveNPC(mob, nextLoc);
}

void checkNPCPlanetMoveEvent(MobShip* mb)
{
    if (universe.getSubAreaMapType() == SMT_PERSISTENT && CSYS->isRaceAffiliated())
    {
        const int victim_race_id = CSYS->getNativeRaceID();
        const int offending_race_id = mb->getMobSubAreaGroupID();
        
        if (offending_race_id == victim_race_id)
            return;
        
        if (approachPlanetPatternSetCondition(offending_race_id, victim_race_id))
        {
            mb->setAIPattern(AIPATTERN_APPROACHPLANET);
        }
    }
}

void checkNPCPlanetEnslaveEvent(MobShip* mob)
{
    if (!planetEnslaveEventCondition(mob))
        return;

    const point& loc = mob->at();
    const int mobRaceID = mob->getMobSubAreaGroupID();
    const int nativeRaceID = CSYS->getNativeRaceID();
    auto* nativeRace = universe.getRace(nativeRaceID);

    nativeRace->setHomeworldMajorStatus(loc, RMS_ENSLAVED);
    nativeRace->setHomeworldControllerRace(loc, mobRaceID, getMobRaceDangerLevel(mob));
    getMap()->setBackdrop(loc, LBACKDROP_ENSLAVEDPLANET);

    if (getMap()->getv(loc))
    {
        msgeAdd("The " + mob->getShipName() + " deploys a slave shield over the homeworld!", cp_purpleonblack);
    }

    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        MobShip* npc = CSYS->getNPCShip(i);
        if (!npc)
            continue;

        if (npc->getAIPattern() == AIPATTERN_APPROACHPLANET &&
            npc->getMobSubAreaGroupID() == mobRaceID)
        {
            npc->setAIPattern(AIPATTERN_NEUTRAL);
            npc->setGoalStatus(GOALSTATUS_COMPLETE);
        }
    }
}

void checkNPCAggroEvent(MobShip* mob)
{
    if (!mob || mob->getMobType() == SHIP_PLAYER)
        return;

    const int mobGroupID = mob->getMobSubAreaGroupID();
    const int playerAttitude = universe.getRace(mobGroupID)->getPlayerAttStatus();
    int maxAngerFactor = 0;

    // Check if player is hated and within detection range
    if (playerAttitude <= -1 &&
        shortestPath(mob->at(), getPlayerShip()->at()) <= mob->getDetectRadius())
    {
        maxAngerFactor = playerAttitude;
        mob->setMobSubAreaAttackID(0);
        resetWaitCounter();
    }

    // Check nearby hostile NPCs
    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        auto* npc = CSYS->getNPCShip(i);
        if (!npc || mob->getMobSubAreaID() == npc->getMobSubAreaID())
            continue;

        const int npcGroupID = npc->getMobSubAreaGroupID();
        if (npcGroupID == mobGroupID)
            continue;

        const int npcAnger = universe.getRace(mobGroupID)->getRaceAttStatus(npcGroupID);
        if (npcAnger > -1 || npcAnger >= maxAngerFactor)
            continue;

        if (shortestPath(mob->at(), npc->at()) <= mob->getDetectRadius() && npc->isActivated())
        {
            mob->setMobSubAreaAttackID(npc->getMobSubAreaID());
            maxAngerFactor = npcAnger;
        }
    }
}

void resetWaitCounter()
{
    if (wait_counter > 0)
    {
        wait_counter = 0;
        msgeAdd("Hostile ship is pursing you! Cancelling wait.", cp_darkredonblack);
    }
}

void moveNPC(MobShip* mob, point newLoc)
{
    if (!mob || !moveNPCCondition(mob, newLoc))
        return;

    changeMobTile(mob->at(), newLoc, mob->getMobType());
    mob->setLoc(newLoc);
}

void checkMobHasEngine(MobShip* mob)
{
    if (!mob) return;
    mob->setMoveState(mob->getNumInstalledModulesOfType(MODULE_ENGINE) > 0);
}

bool planetEnslaveEventCondition(MobShip* mob)
{
    if (!mob) return false;

    auto* race = universe.getRace(mob->getMobSubAreaGroupID());

    return universe.getSubAreaMapType() == SMT_PERSISTENT &&
        CSYS->isRaceAffiliated() &&
        race->getRaceOverallMajorStatus() != RMS_ENSLAVED &&
        mob->getAIPattern() == AIPATTERN_APPROACHPLANET &&
        getMap()->getBackdrop(mob->at()) == LBACKDROP_PLANET;
}

bool approachPlanetPatternSetCondition(int offendingRaceID, int victimRaceID)
{
    auto* offender = universe.getRace(offendingRaceID);
    auto* victim = universe.getRace(victimRaceID);

    return victim->getNumHomeworlds() > 0 &&
        victim->getRaceOverallMajorStatus() != RMS_ENSLAVED &&
        offender->getRaceOverallMajorStatus() != RMS_ENSLAVED &&
        CSYS->getNumActiveNativeShipsPresent() == 0 &&
        offender->getRaceAttStatus(victimRaceID) <= -1;
}

bool moveNPCCondition(MobShip* mob, point newLoc)
{
    const point mapSize = getMapSize();
    const point minBound(1, 1);
    const point maxBound = addPoints(mapSize, point(-2, -2));

    return inRange(newLoc, minBound, maxBound) &&
        !isBlockingCell(getMap()->getCell(newLoc)) &&
        !isAt(newLoc, getSubAreaSystemStartPoint(mapSize));
}