#ifndef GAME_H_
#define GAME_H_

#include "output.h"
#include "input.h"
#include "region.h"
#include "combat.h"
#include "pathfind.h"
#include "line.h"

#define RACE_EVENT_CHANCE 600000
#define RACE_EVENT_BATTLE_SHIPS_MAX_SPAWN 6
#define RACE_EVENT_WAR_SHIPS_MAX_SPAWN 18

#define MAX_PLAYER_SLOTS 27

struct ship_explosion_struct
{
    point loc;
    int radius;
    int iteration;
};

class game
{
public:
    game();
    void run();
    void primaryGameLoop();
    void checkForUniversalRaceEvent();
    void checkForSubAreaRaceEvent();
    void checkForUnEnslavementRaceEvent();
    void checkForRaceWarEvent();
    void checkForPlayerEncounterEvent();
    void spacePirateEncounter();
    void addEncounterShips();
    void raceInvasionEvent(race *, race *);
    void activateAllNPCAI();
    void activateOneNPC(ship_mob *);
    void setNPCRandDestination(ship_mob *);
    void setNPCRandFreePlanetDestination(ship_mob *);
    void setNPCAIPattern(ship_mob *);
    void setNPCGoalDestinationLoc(ship_mob *);
    void checkNPCMoveEvent(ship_mob *);
    void checkNPCAggroEvent(ship_mob *);
    void checkNPCPlanetAttackEvent(ship_mob *);
    void checkNPCPlanetMoveEvent(ship_mob *);
    void resetAttackIDs(int);
    void checkIfRaceAggroEvent(ship_mob *, ship_mob *);
    void moveNPC(ship_mob *, point);
    void playerTargetToggle();
    void printPlayerFirePath(point,point);
    void mobShootSingleProjectile(ship_mob*,point);
    void mobShootSpread(ship_mob*,point,int);
    void mobShootPulse(ship_mob*,point);
    void extrapolateLine(point&,point&);
    void mobFire(ship_mob*,point);
    void outputLOFTransition(point,point,point,fire_t);
    void checkPlayerFuelEvent();
    void checkMobRegenerateEvent(ship_mob *s,module_type);
    void initGameObjects();
    void reDisplay(bool);
    void promptInput();
    void changeMobTile(point,point,mob_t);
    void cleanupEverything();
    void movePlayerShip(point);
    void msgeAddPromptSpace(std::string, color_pair);
    void msgeAdd(std::string, color_pair);
    void printWindowBorders();
    void calculatePlayerLOS();
    void executeMiscPlayerTurnBasedData();
    void executeMiscNPCTurnBasedData();
    void enterStarSystem();
    void returnToStarMap();
    void setPlayerLoc(point);
    void enterArea();
    void enterSubArea(bool);
    void enterStation();
    void useSpaceStation(point);
    void changeGameTabFocus();
    void changeSelectedModule(point);
    void mobShootWeapon(ship_mob *);
    void endOfProjectileLoop(ship_mob *,point,bool);
    void checkNPCDefeatedEvent();
    void checkPlayerDefeatedEvent();
    void createDamagingExplosionAnimation(point, int, int, fire_t, bool);
    void createShipDestructionAnimations();
    void printFireCircle(point,int,fire_t);
    void checkMobExplosionRadiusDamage(point, int, damage_report,bool);
    void checkCreateDamagingExplosion(point,int,int,fire_t,bool,damage_report,bool);
    void checkNPCFlags(ship_mob *);
    void checkMobHasEngine(ship_mob *);
    void printShipmobWeaponEventMessage(ship_mob *, std::string);
    void printTileCharacteristics();
    void displayDamageReport(damage_report,ship_mob *,bool);
    void mobChangeSelectedWeapon(ship_mob *);
    void displayEvasionReport(int, ship_mob *);
    void checkCreateDamagingExplosionRollDamage(ship_mob *, point, int);
    void decrementPlayerRaceReputation(race *);
    void printMobCells();
    void printAndSetFireCell(map*,point,fire_t);
    void clearAllFireCellsInRange(map*,point,int);
    void clearAllFireCells(map *);
    void addHitSprite(map *, point);
    void navigateMenu(menu *);
    void playerPurchaseModule(station *, menu *);
    void playerSellModule(station *, menu *);
    void stripShipMobOfResources(ship_mob *, ship_mob *);
    void upgradePlayerSlotCapacity(station *);
    void upgradePlayerHull(station *);
    void repairPlayerHull(station *);
    void playerHireCrew(station *);
    void playerBuyFuel(station *);
    void pickUpItems(ship_mob*, int, int);
    void checkPickUpItems(ship_mob *);
    void printSubAreaEntranceMessage();
    bool checkNPCWeaponEvent(ship_mob *);
    bool checkForMobInLOF(ship_mob *, point, bool, bool);
    bool isTargetableNPC(int);
    bool converseViaContactMenu(race *);
    bool executeConverseEvent(race *);
    point getNextTargetedNPC();
    point getMapSize();
    map *getMap();
    ship_mob *getPlayerShip();
    ship_mob *getSubAreaShipMobAt(point);
    ship_mob *getCurrentMobTurn();
    ship_mob *getMobFromID(int);
    void save();
    void load();
private:
    ship_mob player_ship;
    display display_obj;
    starmap_region universe;
    point last_smloc;
    point last_subarealoc;
    int current_subarea_id;
    int gmti;
    int current_player_target;
    int current_mob_turn;
    int wait_counter;
    uint turn_timer;
    tab_type current_tab;
    map_type current_maptype;
    input event_handler;
    bool game_active;
    bool player_has_moved;
    line tracer;
    pathfind pathfinder;
    menu contact_menu_obj;
    menu station_menu_obj;
    std::vector<ship_explosion_struct> explosion_data;
};

std::string getNamePrefix(ship_mob *);

#endif
