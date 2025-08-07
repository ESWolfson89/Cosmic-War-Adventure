#ifndef GAME_H_
#define GAME_H_

#include "output.h"
#include "combat.h"
#include "pathfind.h"
#include "line.h"
#include "namegen.h"
#include "mob.h"
#include "region.h"
#include "station.h"
#include "ai.h"

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

class Game
{
public:
    Game();
    void run();
    void primaryGameLoop(bool loaded);
    void checkForUniversalRaceEvent();
    void checkForSubAreaRaceEvent();
    void checkForUnEnslavementRaceEvent();
    void checkForRaceWarEvent();
    void checkForPlayerEncounterEvent();
    void spacePirateEncounter();
    void addEncounterShips();
    void raceInvasionEvent(race *, race *);
    void resetAttackIDs(int);
    void playerTargetToggle(input_t);
    void printPlayerFirePath(point,point);
    void playerFireWeapon(point);
    void checkPlayerFuelEvent();
    void checkMobRegenerateEvent(MobShip *s,module_type);
    void initGameObjects();
    void promptInput();
    void cleanupEverything();
    void movePlayerShip(point);
    void msgeAddPromptSpace(std::string, color_pair);
    void executeMiscPlayerTurnBasedData();
    void executeMiscNPCTurnBasedData();
    void returnToStarMap();
    void setPlayerLoc(point);
    void setVisitedStarMapTileBackdrops(point smloc);
    void setVisitedStarMapTileBackdrop(point smloc, int increment);
    void checkSetRegionTileForRaceHome();
    void setRegionTileForRaceHome(race* dominantRace);
    void enterArea();
    void enterSubArea(bool);
    void enterStation();
    void useSpaceStation(point);
    void useEntertainmentCenter(point);
    void changeGameTabFocus();
    void changeSelectedModule(point);
    void checkNPCDefeatedEvent();
    void checkPlayerDefeatedEvent();
    void createShipDestructionAnimations();
    void printTileCharacteristics();
    void navigateMenu(menu *);
    void playerPurchaseModule(station *, menu *);
    void playerSellModule(station *, menu *);
    void stripShipMobOfResources(MobShip *, MobShip *);
    void upgradePlayerSlotCapacity(station *);
    void upgradePlayerHull(station *);
    void repairPlayerHull(station *);
    void playerHireCrew(station *);
    void playerBuyFuel(station *);
    void pickUpItems(MobShip*, int, int);
    void checkPickUpItems(MobShip *);
    void printSubAreaEntranceMessage();
    void printTogglePromptMessage(input_t, bool);
    void cycleTarget();
    void setPointIfInMapRangeAndLOS(point, point&);
    void runContactScenario(ContactTree&);
    void executeSubareaEntranceContactScenario(race*, race*, bool&);
    void checkNPCShipDefeatedByPlayer(MobShip *);
    void updateFinalMiscEndGameDisplayTiles();
    void initMenus();
    void checkForRaceSurrenderToPlayerEvent();
    void promptQuit();
    void runRaceSurrenderToPlayerScript(race*);
    void checkPlayerRecapturingRaceEvent(race*, race*);
    bool isTargetableNPC(int, input_t);
    bool converseViaContactMenu(race *);
    bool checkCanTargetBasedOnModule(input_t);
    bool checkCanTargetBasedOnStraightLine(MobShip *, input_t);
    bool sufficientFillQuantity(MobShip *);
    point getNextTargetedNPC(input_t);
    point getNextToggleDeltaForFireWeapon(point, point);
    void save();
    void load();
    void setupLoadedGame();
    const int maxPirateDangerLevelFromTurnTimer();
    // template functions
    template <typename M> void useMachinePlayer(M*);
    template <typename M> void featurePlayerToggle(M*);
    template <typename M> void featurePlayerSelect(M*, point);
private:
    point last_smloc;
    point last_subarealoc;
    int gmti;
    int current_player_target;
    bool usingMachine;
    uint turn_timer;
    menu contact_menu_obj;
    menu station_menu_obj;
    menu entertainmentStationMenu;
    std::vector<ship_explosion_struct> explosion_data;
};

bool eightDirectionRestrictedWeaponSelected(MobShip*);

bool inRangeStarMapBackdropGreen(backdrop_t);

bool inRangeStarMapBackdropRed(backdrop_t);

bool inRangeStarMapBackdropUnhighlighted(backdrop_t starTile);

bool isMainSequenceBackdropTile(backdrop_t starTile);

bool isBlueStarBackdropTile(backdrop_t starTile);

bool isRedStarBackdropTile(backdrop_t starTile);

bool isWhiteStarBackdropTile(backdrop_t starTile);

void printExitPromptMessage();

star_type getStarTypeFromStarMapTile(point);

std::string getRaceRegionNameAtLocation(point loc);

std::string getUserInputPrompt(std::string& promptText, point& loc, int maxLength);

int getMinNumShipsDestroyedForSurrenderToPlayer(race* r);

double getMinNumShipDestroyedPercentageForSurrenderToPlayer(race* r);

void resetSurrenderedShips();

#endif
