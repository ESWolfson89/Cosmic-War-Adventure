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
void checkNPCPlanetEnslaveEvent(MobShip* mob);
void checkNPCAggroEvent(MobShip* mb);
void resetWaitCounter();
void moveNPC(MobShip* mb, point new_loc);

bool planetEnslaveEventCondition(MobShip* mob);
bool approachPlanetPatternSetCondition(int offendingRaceID, int victimRaceID);
bool moveNPCCondition(MobShip* mob, point newLoc);

point getClosestNPCTargetInRangeLine(MobShip* mb, int travelRange);

#endif