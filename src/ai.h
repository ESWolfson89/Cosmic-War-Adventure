#ifndef AI_H_
#define AI_H_

#include "region.h"
#include "output.h"
#include "combat.h"
#include "pathfind.h"

void activateAllNPCAI();
void activateOneNPC(MobShip* mb);
void checkNPCFlags(MobShip* mb);
void setNPCAIPattern(MobShip* mb);
void setNPCGoalDestinationLoc(MobShip* mb);
void setNPCRandDestination(MobShip* mb);
void setNPCRandFreePlanetDestination(MobShip* mb);
void setNPCAttackPositionDestination(MobShip* mb);
void checkNPCMoveEvent(MobShip* mb);
void checkNPCPlanetMoveEvent(MobShip* mb);
void checkMobHasEngine(MobShip* mb);
void checkNPCPlanetAttackEvent(MobShip* mb);
void checkNPCAggroEvent(MobShip* mb);
void resetWaitCounter();
void moveNPC(MobShip* mb, point new_loc);

point getClosestNPCTargetInRangeLine(MobShip* mb, weapon_struct current_ws);

#endif