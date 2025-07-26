#include "ai.h"

void activateAllNPCAI()
{
    if (current_maptype != MAPTYPE_LOCALEMAP)
        return;

    if (!playerHasMoved)
        return;

    gmti = 0;

    double npc_speed, npc_timer;

    double spd_reference = getPlayerShip()->getSpeed();

    int initial_num_NPCs = CSYS->getNumShipNPCs();

    while (gmti < CSYS->getNumShipNPCs())
    {
        // if this NPC ship is activated
        if (CSYS->getNPCShip(gmti)->isActivated())
        {
            npc_speed = CSYS->getNPCShip(gmti)->getSpeed();

            npc_timer = CSYS->getNPCShip(gmti)->getTurnTimer();

            CSYS->getNPCShip(gmti)->setTurnTimer(npc_timer + (npc_speed / spd_reference));

            if (CSYS->getNPCShip(gmti)->inTimerRange())
                CSYS->getNPCShip(gmti)->setTurnTimer(0.0);

            // ActivateOneNPC gets called a certain number of times
            // for a particular NPC -> based on how fast it moves
            // relative to player
            while (CSYS->getNPCShip(gmti)->getTurnTimer() > 0.0)
            {
                CSYS->getNPCShip(gmti)->decrementTurnTimer();
                activateOneNPC(CSYS->getNPCShip(gmti));
                if (initial_num_NPCs != CSYS->getNumShipNPCs())
                    return;
            }
        }
        // increment global mob turn iterator
        gmti++;
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
    checkNPCPlanetAttackEvent(mb);
}

// Set behavior of NPC AI
void setNPCAIPattern(MobShip* mb)
{
    if (mb->getMobSubAreaAttackID() >= 0)
    {
        if (getCurrentMobSelectedModule(mb)->getFillQuantity() >= getWeaponModuleConsumptionPerTurn(getCurrentMobSelectedModule(mb)))
            mb->setAIPattern(AIPATTERN_ATTACKING);
        else
            mb->setAIPattern(AIPATTERN_FLEEING);
    }
    else
    {
        if (rollPerc(mb->getStatStruct().rove_chance))
            mb->setAIPattern(AIPATTERN_ROVING);
        else
            mb->setAIPattern(AIPATTERN_NEUTRAL);
    }

    checkNPCPlanetMoveEvent(mb);
}

void setNPCGoalDestinationLoc(MobShip* mb)
{
    switch (mb->getAIPattern())
    {
        case(AIPATTERN_NEUTRAL):
            mb->setDestination(mb->getInitLoc());
            mb->setGoalStatus(GOALSTATUS_COMPLETE);
            break;
        case(AIPATTERN_ATTACKING):
            setNPCAttackPositionDestination(mb);
            mb->setGoalStatus(GOALSTATUS_COMPLETE);
            break;
        case(AIPATTERN_ROVING):
        case(AIPATTERN_FLEEING):
            if (mb->getGoalStatus() == GOALSTATUS_COMPLETE)
            {
                setNPCRandDestination(mb);
                mb->setGoalStatus(GOALSTATUS_INCOMPLETE);
            }
            break;
        case(AIPATTERN_APPROACHPLANET):
            if (mb->getGoalStatus() == GOALSTATUS_COMPLETE)
            {
                setNPCRandFreePlanetDestination(mb);
                mb->setGoalStatus(GOALSTATUS_INCOMPLETE);
            }
            break;
        default:
            break;
    }
}

void setNPCRandDestination(MobShip* mb)
{
    mb->setDestination(getRandNPCShipOpenPoint(getMap(), point(1, 1), addPoints(getMapSize(), point(-2, -2)), LBACKDROP_SPACE_UNLIT, LBACKDROP_WHITESTARBACKGROUND));
}

void setNPCRandFreePlanetDestination(MobShip* mb)
{
    mb->setDestination(universe.getRace(CSYS->getNativeRaceID())->getFirstFreeHomeworldLoc());
}


void setNPCAttackPositionDestination(MobShip* mb)
{
    module* weapon_mod = getCurrentMobSelectedModule(mb);

    weapon_struct current_ws = weapon_mod->getWeaponStruct();

    bool eightDirectionRestricted = current_ws.eightDirectionRestricted;

    if (eightDirectionRestricted)
    {
        mb->setDestination(getClosestNPCTargetInRangeLine(mb, current_ws));
    }
    else
    {
        mb->setDestination(getMobFromID(mb->getMobSubAreaAttackID())->at());
    }
}

point getClosestNPCTargetInRangeLine(MobShip* mb, weapon_struct current_ws)
{
    int travelRange = current_ws.travel_range;

    point attackLoc = getMobFromID(mb->getMobSubAreaAttackID())->at();

    std::vector<point> eightFarthestShootPoints;
    std::vector<int> eightFarthestShootPointsDistances;

    int increment = 0;

    for (int xx = -1; xx <= 1; xx++)
    {
        for (int yy = -1; yy <= 1; yy++)
        {
            if (xx != 0 || yy != 0)
            {
                point addedPoints = addPoints(attackLoc, point(travelRange * xx, travelRange * yy));
                if (inRange(addedPoints, point(0, 0), point(STARMAPWID - 1, STARMAPHGT - 1)))
                {
                    eightFarthestShootPoints.push_back(addPoints(attackLoc, point(travelRange * xx, travelRange * yy)));
                    eightFarthestShootPointsDistances.push_back(distanceSquared(mb->at(), eightFarthestShootPoints[increment]));
                    increment++;
                }
            }
        }
    }

    if (increment == 0)
    {
        return attackLoc;
    }

    int minDistance = *min_element(eightFarthestShootPointsDistances.begin(), eightFarthestShootPointsDistances.end());

    int index = std::find(eightFarthestShootPointsDistances.begin(), eightFarthestShootPointsDistances.end(), minDistance) - eightFarthestShootPointsDistances.begin();

    return eightFarthestShootPoints[index];
}


void checkNPCMoveEvent(MobShip* mb)
{
    if (mb->getMoveState() == false)
        return;

    point source = mb->at();

    point dest = mb->getDestination();

    if (!isAt(source, dest))
    {
        if (shortestPath(source, dest) <= mb->getDetectRadius() ||
            mb->getAIPattern() != AIPATTERN_ATTACKING)
        {
            point next_loc;

            bool blockage;

            bool not_on_map_border;

            blockage = tracer.isBlocking(getMap(), source, dest, true, false);

            not_on_map_border = (notOnMapBorder(dest, getMap()->getSize()) &&
                notOnMapBorder(source, getMap()->getSize()));

            if (blockage && not_on_map_border)
                next_loc = pathfinder.dijkstra(getMap(), source, dest);
            else
                next_loc = tracer.getLinePoint(tracer.getLineSize() - 2);

            moveNPC(mb, next_loc);
        }
    }
    else
        mb->setGoalStatus(GOALSTATUS_COMPLETE);
}

void checkNPCPlanetMoveEvent(MobShip* mb)
{
    if (universe.getSubAreaMapType() == SMT_PERSISTENT)
        if (CSYS->isRaceAffiliated())
        {
            int victim_race_id = CSYS->getNativeRaceID();
            int offending_race_id = mb->getMobSubAreaGroupID();

            if (offending_race_id == victim_race_id)
                return;

            if (universe.getRace(victim_race_id)->getNumHomeworlds() != 0)
                if (universe.getRace(victim_race_id)->getRaceOverallMajorStatus() != RMS_ENSLAVED)
                    if (universe.getRace(offending_race_id)->getRaceOverallMajorStatus() != RMS_ENSLAVED)
                        if (CSYS->getNumActiveNativeShipsPresent() == 0)
                            if (universe.getRace(offending_race_id)->getRaceAttStatus(victim_race_id) <= -1)
                                mb->setAIPattern(AIPATTERN_APPROACHPLANET);
        }
}


void checkMobHasEngine(MobShip* mb)
{
    if (mb->getNumInstalledModulesOfType(MODULE_ENGINE) == 0)
        mb->setMoveState(false);
    else
        mb->setMoveState(true);
}

void checkNPCPlanetAttackEvent(MobShip* mb)
{
    if (universe.getSubAreaMapType() == SMT_PERSISTENT)
    {
        if (CSYS->isRaceAffiliated())
        {
            if (universe.getRace(mb->getMobSubAreaGroupID())->getRaceOverallMajorStatus() != RMS_ENSLAVED)
            {
                if (mb->getAIPattern() == AIPATTERN_APPROACHPLANET)
                {
                    if (getMap()->getBackdrop(mb->at()) == LBACKDROP_PLANET)
                    {
                        universe.getRace(CSYS->getNativeRaceID())->setHomeworldMajorStatus(mb->at(), RMS_ENSLAVED);
                        universe.getRace(CSYS->getNativeRaceID())->setHomeworldOwnerRaceID(mb->at(), mb->getMobSubAreaGroupID());
                        getMap()->setBackdrop(mb->at(), LBACKDROP_ENSLAVEDPLANET);
                        if (getMap()->getv(mb->at()))
                        {
                            msgeAdd("The " + mb->getShipName() + " deploys a slave shield over the homeworld!", cp_whiteonblack);
                        }
                        for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
                        {
                            if (CSYS->getNPCShip(i)->getAIPattern() == AIPATTERN_APPROACHPLANET && mb->getMobSubAreaGroupID() == CSYS->getNPCShip(i)->getMobSubAreaGroupID())
                            {
                                CSYS->getNPCShip(i)->setAIPattern(AIPATTERN_NEUTRAL);
                                CSYS->getNPCShip(i)->setGoalStatus(GOALSTATUS_COMPLETE);
                            }
                        }
                    }
                }
            }
        }
    }
}

void checkNPCAggroEvent(MobShip* mb)
{
    if (mb->getMobType() == SHIP_PLAYER)
        return;

    int max_anger_factor = 0;
    int npc_anger_factor = -1;
    int npc_tgid = -1;
    int agid = mb->getMobSubAreaGroupID();
    int pattstatus = universe.getRace(agid)->getPlayerAttStatus();
    // check if hated player is nearby to attack
    if (pattstatus <= -1)
    {
        if (shortestPath(mb->at(), getPlayerShip()->at()) <= mb->getDetectRadius())
        {
            max_anger_factor = pattstatus;
            mb->setMobSubAreaAttackID(0);
            resetWaitCounter();
        }
    }
    // check if hated npc is nearby to attack
    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        // mob cannot attack itself
        if (mb->getMobSubAreaID() != CSYS->getNPCShip(i)->getMobSubAreaID())
        {
            npc_tgid = CSYS->getNPCShip(i)->getMobSubAreaGroupID();
            // mob cannot attack an ally
            if (agid != npc_tgid)
            {
                npc_anger_factor = universe.getRace(agid)->getRaceAttStatus(npc_tgid);
                if (npc_anger_factor <= -1)
                {
                    if (shortestPath(mb->at(), CSYS->getNPCShip(i)->at()) <= mb->getDetectRadius())
                    {
                        if (CSYS->getNPCShip(i)->isActivated())
                        {
                            if (npc_anger_factor < max_anger_factor)
                            {
                                mb->setMobSubAreaAttackID(CSYS->getNPCShip(i)->getMobSubAreaID());
                                max_anger_factor = npc_anger_factor;
                            }
                        }
                    }
                }
            }
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

void moveNPC(MobShip* mb, point new_loc)
{
    if (inRange(new_loc, point(1, 1), addPoints(getMapSize(), point(-2, -2))))
    {
        if (!isBlockingCell(getMap()->getCell(new_loc)))
        {
            if (!isAt(new_loc, getSubAreaSystemStartPoint(getMapSize())))
            {
                changeMobTile(mb->at(), new_loc, mb->getMobType());
                mb->setLoc(new_loc);
            }
        }
    }
}