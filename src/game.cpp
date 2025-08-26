#include "Game.h"

Game::Game()
{
    programActive = true;
}

void Game::runGame()
{
    Game_active = true;
    playerHasMoved = false;
    usingMachine = false;
    wait_counter = 0;

    bool loaded = false;

    point titleTextLocation = point(GRIDWID / 2 - 16, 8);

    gfx_obj.clearScreen();
    display_obj.addString("      COSMIC WAR ADVENTURE       ", cp_darkredonblack, titleTextLocation);
    display_obj.addString("           v0.1.0-dev            ", cp_darkgrayonblack, titleTextLocation + point(0, 2));
    display_obj.addString(" Press ENTER to start a new Game ", cp_grayonblack, titleTextLocation + point(0, 8));
    display_obj.addString("Press R to reload a previous Game", cp_grayonblack, titleTextLocation + point(0, 10));
    display_obj.addString("        By Ewolf Gamedev         ", cp_whiteonblack, titleTextLocation + point(0, GRIDHGT - 17));
    gfx_obj.updateScreen();
    // set player input
    do
    {
        event_handler.setAction();
        if (event_handler.getAction() == INP_TOGGLE)
        {
            initGameObjects();
        }
        if (event_handler.getAction() == INP_LOAD)
        {
            setupLoadedGame();
            loaded = true;
        }
        if (event_handler.getAction() == INP_QUIT)
        {
            programActive = false;
        }
    } while (event_handler.getAction() != INP_TOGGLE && event_handler.getAction() != INP_LOAD && programActive);

    if (programActive)
    {
        primaryGameLoop(loaded);
        cleanupEverything();
    }

    do
    {
        event_handler.setAction();
    } while (event_handler.getAction() != INP_TOGGLE);
}

void Game::runProgram()
{
    if(gfx_obj.initSDL())
    {
        do
        {
            runGame();
            cleanupEverything();
        } while (programActive);
    }
    else
    {
        std::cout << "failed to initialize... press enter to terminate.";
        std::cin.get();
    }
}

void Game::promptQuit()
{
    display_obj.clearAndDeleteAllMessages();
    msgeAdd("Do you really want to quit? (y/n)", cp_whiteonblack);
    gfx_obj.updateScreen();

    // Wait for a valid input: yes or no
    int action = INP_NONE;
    do 
    {
        event_handler.setAction();
        action = event_handler.getAction();
    } while (action != INP_YES && action != INP_NO);

    if (action == INP_YES)
    {
        printExitPromptMessage();
        Game_active = false;
    }
    else
    {
        msgeAdd("Resuming Game...", cp_whiteonblack);
    }
}


void Game::cleanupEverything()
{
    universe.cleanupEverything();
    contact_menu_obj.cleanupEverything();
    player_ship.cleanupEverything();
    display_obj.clearAndDeleteAllMessages();
}

// 1) clear messages, 2) print new message with space bar prompt 3) update screen,
// 4) wait indefinitely until user presses the space bar
void Game::msgeAddPromptSpace(std::string s, color_pair col, bool finalUpdate)
{
    display_obj.clearAndDeleteAllMessages();
    msgeAdd(s,col);
    msgeAdd(" [space]",cp_grayonblack);
    gfx_obj.updateScreen();
    event_handler.waitForKey(' ');
    display_obj.clearAndDeleteAllMessages();
    if (finalUpdate)
        gfx_obj.updateScreen();
}

// ask for player input via event_handler "input" class object
void Game::promptInput()
{
    if (wait_counter > 0)
    {
        wait_counter--;
        printMobCells();
        display_obj.clearAndDeleteAllMessages();
        msgeAdd("Waiting... Turns left: " + int2String(wait_counter),cp_whiteonblack);
        return;
    }

    // offset to next location (abs(delta.x()) and abs(delta.y()) is probably always 1)
    point delta = point(0,0);

    current_mob_turn = getPlayerShip()->getMobSubAreaID();

    playerHasMoved = true;

    // set player input
    event_handler.setAction();

    if (!event_handler.isShiftOrCaps())
    {
        // flush and clear msgbuffer
        display_obj.clearAndDeleteAllMessages();
    }

    // get action indicating what action was triggered by player
    switch(event_handler.getAction())
    {
        // if directional key was pressed
        case(INP_DELTA):
        {
            // get delta corresponding to directional key
            delta = event_handler.getDelta();
            if (current_tab == TABTYPE_PLAYAREA)
            {
                movePlayerShip(delta);
            }
            else
            {
                changeSelectedModule(delta);
            }
            break;
        }
        case(INP_SELECT):
        {
            enterArea();
            break;
        }
        case(INP_QUIT):
        {
            playerHasMoved = false;
            promptQuit();
            break;
        }
        case(INP_CHANGETABFOCUS):
        {
            playerHasMoved = false;
            changeGameTabFocus();
            break;
        }
        case(INP_WEAPONFIRE):
        {
            playerHasMoved = false;
            playerTargetToggle(INP_WEAPONFIRE);
            break;
        }
        case(INP_WAIT):
        {
            playerHasMoved = true;
            break;
        }
        case(INP_WAITSPECIFY):
        {
            playerHasMoved = true;
            wait_counter = 100;
            break;
        }
        case(INP_EXAMINE):
        {
            playerHasMoved = false;
            playerTargetToggle(INP_EXAMINE);
            break;
        }
        case(INP_SAVE):
        {
            playerHasMoved = false;
            display_obj.clearAndDeleteAllMessages();
            msgeAdd("Saving...", cp_whiteonblack);
            gfx_obj.updateScreen();
            save();
            msgeAdd("Saved", cp_whiteonblack);
            break;
        }
        default:
        {
            playerHasMoved = false;
            break;
        }
    }
}

void Game::changeGameTabFocus()
{
    playerHasMoved = false;

    if (current_tab == TABTYPE_PLAYAREA)
    {
        current_tab = TABTYPE_PLAYERSHIP;
    }
    else
    {
        current_tab = TABTYPE_PLAYAREA;
    }
}

void Game::changeSelectedModule(point delta)
{
    playerHasMoved = false;

    if (delta.x() == 1)
    {
        getPlayerShip()->offModuleSelectionIndex(1);
    }

    if (delta.x() == -1)
    {
        getPlayerShip()->offModuleSelectionIndex(-1);
    }
}

void Game::movePlayerShip(point delta)
{
    if (getPlayerShip()->getMoveState() == false)
    {
        msgeAdd("You can't move your ship without an engine!",cp_grayonblack);
        return;
    }

    if (getPlayerShip()->getTotalMTFillRemaining(MODULE_FUEL) <= 0 && current_maptype == MAPTYPE_STARMAP)
    {
        msgeAdd("Your ship drifts without fuel!",cp_whiteonblack);
        return;
    }

    point new_loc = addPoints(getPlayerShip()->at(),delta);

    if (!inRange(new_loc,point(0,0),point(getMap()->getSize().x()-1,getMap()->getSize().y()-1)))
    {
        if (current_maptype == MAPTYPE_LOCALEMAP)
        {
            returnToStarMap();
        }
        return;
    }

    if (current_maptype == MAPTYPE_LOCALEMAP)
    {
        if (mobBlocked(getMap()->getMob(new_loc)))
        {
            if (!currentRegion()->getNPCShip(new_loc)->isActivated())
            {
                stripShipMobOfResources(getPlayerShip(), currentRegion()->getNPCShip(new_loc));
                return;
            }
        }
    }

    if (!isBlockingCell(getMap()->getCell(new_loc)))
    {
        changeMobTile(getPlayerShip()->at(),new_loc,getPlayerShip()->getMobType());
        setPlayerLoc(new_loc);
    }
}

// make fuel relevant as well...
void Game::stripShipMobOfResources(MobShip *offender, MobShip *victim)
{
    //int fuel_fill = victim->getTotalMTFillRemaining(MODULE_FUEL);
    uint_64 num_stolen_credits = victim->getNumCredits();

    if (num_stolen_credits > 0ULL)
    {
        offender->setNumCredits(offender->getNumCredits() + num_stolen_credits);
        victim->setNumCredits(0ULL);
        msgeAdd("The " + uint642String(num_stolen_credits) + " credits from the defunct " + victim->getShipName() + " is transferred to the " + offender->getShipName() + ".",cp_whiteonblack);
    }
    else
    {
        if (offender->isCurrentPlayerShip())
        {
            msgeAdd("You fail to find any valuable resources aboard the defunct " + victim->getShipName() + ".",cp_grayonblack);
        }
    }
}

void Game::primaryGameLoop(bool loaded)
{
    if (loaded)
    {
        msgeAdd("Welcome back to Cosmic War Adventure!", cp_purpleonblack);
    }
    else
    {
        msgeAdd("Welcome to Cosmic War Adventure!", cp_purpleonblack);
    }

    do
    {
        reDisplay(true);
        promptInput();
        if (Game_active)
        {
            executeMiscPlayerTurnBasedData();
            reDisplay(true);
            activateAllNPCAI();
            executeMiscNPCTurnBasedData();
            turn_timer++;
        }
    } while (Game_active);

    reDisplay(true);
    updateFinalMiscEndGameDisplayTiles();
}

void Game::updateFinalMiscEndGameDisplayTiles()
{
    if (!getPlayerShip()->isActivated())
    {
        display_obj.printCell(getMap(), getPlayerShip()->at(), { cp_blackonwhite, (int)'@' });
        gfx_obj.updateScreen();
    }
}

void Game::checkForUniversalRaceEvent()
{
    if (current_maptype != MAPTYPE_STARMAP)
    {
        return;
    }

    checkForUnEnslavementRaceEvent();
    checkForRaceWarEvent();
    checkForRaceBattleEvent();
}

void Game::checkForRaceBattleEvent()
{
    const int numRaces = universe.getNumRaces();

    for (int i = 0; i < numRaces; ++i)
    {
        race* aggressor = universe.getRace(i);
        if (aggressor->getRaceOverallMajorStatus() != RMS_FREE)
            continue;

        for (int j = 0; j < numRaces; ++j)
        {
            if (i == j)
                continue;

            race* target = universe.getRace(j);
            if (target->getRaceOverallMajorStatus() != RMS_FREE)
                continue;

            const int attStatus = aggressor->getRaceAttStatus(j);
            if (attStatus >= 0)
                continue;

            const int hostilityMagnitude = std::abs(attStatus);
            if (randInt(0, RACE_EVENT_CHANCE) < hostilityMagnitude)
            {
                raceInvasionEvent(aggressor, target);
            }
        }
    }
}

void Game::raceInvasionEvent(race* offender, race* defender)
{
    const point defenderLoc = defender->getStarmapLoc();
    const point offenderLoc = offender->getStarmapLoc();
    const int subareaIndex = universe.getSubAreaIndex(defenderLoc);

    std::string domainStr = race_domain_suffix_string[(int)defender->getRaceDomainType()];

    if (subareaIndex < 0)
        return;
    SubAreaRegion* region = universe.getSubArea(subareaIndex);
    map* subMap = region->getMap();

    std::cout << region->getNumShipNPCs();
     

    const point mapSize = getMapSize();
    const int mapShipLimit = std::min(75, std::min(mapSize.x() * 2, mapSize.y() * 2));
    if (region->getNumShipNPCs() >= mapShipLimit)
    {
        if (getMap()->getv(defenderLoc))
        {
            msgeAddPromptSpace("You detect a fleet of ships failing to enter the " + defender->getNameString() + " " + domainStr + ".", cp_whiteonblack, true);
        }
        return;
    }

    const int numShips = randInt(1, RACE_EVENT_BATTLE_SHIPS_MAX_SPAWN);
    const point mapLimit = addPoints(subMap->getSize(), point(-1, -1));

    for (int i = 0; i < numShips; ++i)
    {
        point spawnLoc = getRandNPCShipOpenPoint(subMap, point(0, 0), mapLimit,
            LBACKDROP_SPACE_UNLIT, LBACKDROP_SPACE_LIT);

        region->createNPCShip(offender->getNativeShip(0), spawnLoc, offender->getRaceID());
        region->getNPCShip(spawnLoc)->setRoveChance(100);
    }

    std::string offenderName = (universe.getSubAreaIndex(offenderLoc) >= 0)
        ? offender->getNameString()
        : "unidentified";

    reDisplay(false);

    if (!getMap()->getv(defenderLoc))
        return;

    std::string locationStr = "(" + int2String(defenderLoc.x() + 1) + "," +
        int2String(STARMAPHGT - defenderLoc.y()) + ")";

    std::string message;

    if (numShips == 1)
    {
        message = "You detect a lone, hostile " + offenderName + " ship entering the " +
            defender->getNameString() + " " + domainStr + " at " + locationStr + ".";
    }
    else
    {
        message = "You detect a fleet of " + int2String(numShips) + " hostile " + offenderName +
            " ships entering the " + defender->getNameString() + " " + domainStr +
            " at " + locationStr + ".";
    }

    msgeAddPromptSpace(message, cp_whiteonblack, true);
}

void Game::checkForSubAreaRaceEvent()
{
    // Preconditions
    if (current_maptype != MAPTYPE_LOCALEMAP ||
        universe.getSubAreaMapType() != SMT_PERSISTENT ||
        !currentRegion()->isRaceAffiliated())
    {
        return;
    }

    const int totalActiveNPCs = currentRegion()->getNumActiveShipsPresent();
    const int activeNatives = currentRegion()->getNumActiveNativeShipsPresent();

    int dominantRaceID = getDominantRaceIDInRegion(currentRegion());
    race* dominantRace = universe.getRace(dominantRaceID);
    int dominantRaceDangerLevel = dominantRace->getDangerLevel();

    if ((totalActiveNPCs >= dominantRaceDangerLevel || dominantRace->getPlayerAttStatus() == 0) && totalActiveNPCs == activeNatives)
        return;

    const point mapSize = getMapSize();
    const int maxAllowedNPCs = std::min(75, std::min(mapSize.x(), mapSize.y()));
    if (totalActiveNPCs >= maxAllowedNPCs)
        return;

    const int homeworldRaceID = currentRegion()->getNativeRaceID();
    race* homeRace = universe.getRace(homeworldRaceID);
    const int numHomeworlds = homeRace->getNumHomeworlds();

    for (int i = 0; i < numHomeworlds; ++i)
    {
        HomeWorld* planet = homeRace->getHomeworld(i);
        const point planetLoc = planet->getLoc();

        const int controllingRaceID = planet->getControlRaceID();
        race* controllingRace = universe.getRace(controllingRaceID);

        const int dangerLevel = controllingRace->getDangerLevel();
        const int numNativeShips = controllingRace->getNumNativeShips();

        if (mobBlocked(getMap()->getMob(planetLoc)))
            continue;

        if (!roll(155 - dangerLevel * 5))
            continue;

        // Spawn the ship
        const int nativeShipIndex = randInt(0, numNativeShips - 1);
        MobShip * ship = controllingRace->getNativeShip(nativeShipIndex);

        currentRegion()->createNPCShip(ship, planetLoc, controllingRaceID);

        // Set spawn destination
        MobShip* newNPC = currentRegion()->getNPCShip(currentRegion()->getNumShipNPCs() - 1);
        point openPoint = getRandNPCShipOpenPoint(
            getMap(),
            point(1, 1),
            addPoints(getMaxMapPoint(getMap()), point(-1, -1)),
            LBACKDROP_SPACE_UNLIT,
            LBACKDROP_WHITESTARBACKGROUND
        );
        newNPC->setInitLoc(openPoint);

        // Feedback if visible
        if (getMap()->getv(planetLoc))
        {
            msgeAdd("One " + newNPC->getShipName() + " emerges from the nearby planet!", cp_whiteonblack);
        }
    }
}

void Game::resetAttackIDs(int id)
{
    for (int i = 0; i < currentRegion()->getNumShipNPCs(); ++i)
    {
        MobShip* npc = currentRegion()->getNPCShip(i);
        if (npc->getMobSubAreaID() != id &&
            npc->getMobSubAreaAttackID() == id)
        {
            npc->setMobSubAreaAttackID(-1);
        }
    }
}

void Game::executeMiscPlayerTurnBasedData()
{
    if (!playerHasMoved)
        return;

    checkNPCDefeatedEvent();
    checkPlayerDefeatedEvent();

    checkPickUpItems(getPlayerShip());

    checkMobRegenerateEvent(getPlayerShip(),MODULE_WEAPON);
    checkMobRegenerateEvent(getPlayerShip(),MODULE_SHIELD);

    checkMobHasEngine(getPlayerShip());

    if (getPlayerShip()->getMoveState() == true && event_handler.getAction() != INP_WAIT && event_handler.getAction() != INP_WAITSPECIFY)
    {
        checkPlayerFuelEvent();
    }

    printTileCharacteristics();

    checkForPlayerEncounterEvent();
    checkForUniversalRaceEvent();
    checkForSubAreaRaceEvent();

    if (current_maptype == MAPTYPE_LOCALEMAP && (int)explosion_data.size() > 0)
    {
        createShipDestructionAnimations();
    }

    if (current_maptype == MAPTYPE_LOCALEMAP && currentRegion()->getSubAreaSpecificType() == SST_RACEHOME)
    {
        checkForRaceSurrenderToPlayerEvent();
    }
}

int getMinNumShipsDestroyedForSurrenderToPlayer(race* r)
{
    switch (r->getRaceDomainType())
    {
    case(RACEDOMAIN_SECTOR):
        return 15;
    case(RACEDOMAIN_DOMAIN):
        return 25;
    }

    // empire
    return 45;
}
 
double getMinNumShipDestroyedPercentageForSurrenderToPlayer(race* r)
{
    switch (r->getRaceDomainType())
    {
    case(RACEDOMAIN_SECTOR):
        return 0.75;
    case(RACEDOMAIN_DOMAIN):
        return 0.8;
    }

    // empire
    return 0.85;
}

void Game::checkForRaceSurrenderToPlayerEvent()
{
    race* homeRace = universe.getRace(currentRegion()->getNativeRaceID());

    if (homeRace->getRaceOverallMajorStatus() != RMS_FREE || homeRace->isSurrenderedToPlayer() || !homeRace->surrenderToPlayerPossible())
        return;

    const int numNPCShipsDestroyedByPlayerAtRegion = homeRace->getNumShipsDestroyedByPlayerAtRegion();
    const int numNPCShipsStartingAtRegion = homeRace->getNumStartingShipsAtRegion();

    if (numNPCShipsDestroyedByPlayerAtRegion < getMinNumShipsDestroyedForSurrenderToPlayer(homeRace))
        return;


    if (numNPCShipsDestroyedByPlayerAtRegion > getMinNumShipDestroyedPercentageForSurrenderToPlayer(homeRace) * static_cast<double>(numNPCShipsStartingAtRegion))
    {
        runRaceSurrenderToPlayerScript(homeRace);
    }
}

void Game::runRaceSurrenderToPlayerScript(race *r)
{
    display_obj.printMonitorWindow(color_white);

    msgeAddPromptSpace("Incoming Transmission...", cp_whiteonblack, true);

    ContactTree tree = createSurrenderToPlayerContactTree(r);
    runContactScenario(tree);

    r->setSurrenderToPlayerPossibility(false);

    if (r->isSurrenderedToPlayer())
    {
        r->setPlayerAttStatus(50);
        resetAttackIDs(0);
        for (auto i = 0; i < currentRegion()->getNumShipNPCs(); i++)
        {
            MobShip* ship = currentRegion()->getNPCShip(i);

            if (ship->getMobSubAreaGroupID() == r->getRaceID())
            {
                ship->setAIPattern(AIPATTERN_NEUTRAL);
                ship->setGoalStatus(GOALSTATUS_COMPLETE);
            }
        }
    }
}

void Game::checkForPlayerEncounterEvent()
{
    if (current_maptype == MAPTYPE_STARMAP)
    if (wait_counter == 0)
    if (getMap()->getBackdrop(getPlayerShip()->at()) == SMBACKDROP_SPACE)
    {
        spacePirateEncounter();
    }
}

void Game::spacePirateEncounter()
{
    if (roll(20))
    {
        reDisplay(true);
        enterSubArea(true);
    }
}

void Game::checkPickUpItems(MobShip *mb)
{
    if (current_maptype != MAPTYPE_LOCALEMAP)
        return;

    if (getMap()->getItem(mb->at()) == NIL_i)
        return;

    int item_quant = currentRegion()->getPile(mb->at())->getItem(0)->getQuant();
    uint_64 num_credits = (uint_64)item_quant;

    if (mb->isCurrentPlayerShip())
        msgeAdd("You salvage " + uint642String(num_credits) + " credits from the wreckage.",cp_whiteonblack);

    pickUpItems(getPlayerShip(),0,item_quant);
}

std::string getRaceRegionNameAtLocation(point loc)
{
    int raceIndexAtLoc = universe.getRaceIndex(getPlayerShip()->at());
    race* raceAtLoc = universe.getRace(raceIndexAtLoc);

    std::string raceName = raceAtLoc->getNameString();
    std::string domainName = race_domain_suffix_string[(int)raceAtLoc->getRaceDomainType()];

    return raceName + " " + domainName;
}

void Game::printTileCharacteristics()
{
    if (wait_counter > 0)
        return;

    const auto backdrop = getMap()->getBackdrop(getPlayerShip()->at());
    const point playerPos = getPlayerShip()->at();

    static const std::unordered_set<int> unknownStarBackdrops = {
        SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE, SMBACKDROP_REDSTARSUBAREAENTRANCE,
        SMBACKDROP_BLUESTARSUBAREAENTRANCE, SMBACKDROP_WHITESTARSUBAREAENTRANCE
    };

    static const std::unordered_set<int> knownRaceBackdrops = {
        SMBACKDROP_FRIENDRACE_MAINSEQSTARSUBAREAENTRANCE, SMBACKDROP_FRIENDRACE_REDSTARSUBAREAENTRANCE,
        SMBACKDROP_FRIENDRACE_BLUESTARSUBAREAENTRANCE, SMBACKDROP_FRIENDRACE_WHITESTARSUBAREAENTRANCE,
        SMBACKDROP_HOSTILERACE_MAINSEQSTARSUBAREAENTRANCE, SMBACKDROP_HOSTILERACE_REDSTARSUBAREAENTRANCE,
        SMBACKDROP_HOSTILERACE_BLUESTARSUBAREAENTRANCE, SMBACKDROP_HOSTILERACE_WHITESTARSUBAREAENTRANCE,
        SMBACKDROP_ALLYRACE_MAINSEQSTARSUBAREAENTRANCE, SMBACKDROP_ALLYRACE_REDSTARSUBAREAENTRANCE,
        SMBACKDROP_ALLYRACE_BLUESTARSUBAREAENTRANCE, SMBACKDROP_ALLYRACE_WHITESTARSUBAREAENTRANCE
    };

    static const std::unordered_set<int> warZoneBackdrops = {
        SMBACKDROP_WARZONE_MAINSEQSTARSUBAREAENTRANCE, SMBACKDROP_WARZONE_REDSTARSUBAREAENTRANCE,
        SMBACKDROP_WARZONE_BLUESTARSUBAREAENTRANCE, SMBACKDROP_WARZONE_WHITESTARSUBAREAENTRANCE
    };

    static const std::unordered_set<int> emptySystemBackdrops = {
        SMBACKDROP_EMPTY_MAINSEQSTARSUBAREAENTRANCE, SMBACKDROP_EMPTY_REDSTARSUBAREAENTRANCE,
        SMBACKDROP_EMPTY_BLUESTARSUBAREAENTRANCE, SMBACKDROP_EMPTY_WHITESTARSUBAREAENTRANCE
    };

    if (unknownStarBackdrops.count(backdrop))
    {
        msgeAdd("an unknown star system", cp_grayonblack);
    }
    else if (knownRaceBackdrops.count(backdrop))
    {
        msgeAdd(getRaceRegionNameAtLocation(playerPos), cp_whiteonblack);
    }
    else if (warZoneBackdrops.count(backdrop))
    {
        msgeAdd("a war zone", cp_whiteonblack);
    }
    else if (emptySystemBackdrops.count(backdrop))
    {
        msgeAdd("an uninhabited star system", cp_whiteonblack);
    }
    else if (backdrop == LBACKDROP_SPACESTATION_SHIP)
    {
        msgeAdd("A ship station is here.", cp_grayonblack);
    }
    else if (backdrop == LBACKDROP_SPACESTATION_ENTERTAINMENT)
    {
        msgeAdd("An entertainment center is here.", cp_grayonblack);
    }
    else if (backdrop == LBACKDROP_HOMEWORLD)
    {
        msgeAdd("There is a homeworld here belonging to The " +
            currentRegion()->getSubAreaName() + ".", cp_lightblueonblack);
    }
    else if (backdrop == LBACKDROP_ENSLAVEDHOMEWORLD)
    {
        const int controllingID = universe.getRace(
            currentRegion()->getNativeRaceID())->getHomeworld(playerPos)->getControlRaceID();

        msgeAdd("There is an enslaved planet here under the authority of The " +
            universe.getRace(controllingID)->getNameString() + ".", cp_purpleonblack);
    }
}

void Game::checkForUnEnslavementRaceEvent()
{
    for (int enslavedIdx = 0; enslavedIdx < universe.getNumRaces(); ++enslavedIdx)
    {
        race* enslavedRace = universe.getRace(enslavedIdx);
        if (enslavedRace->getRaceOverallMajorStatus() != RMS_ENSLAVED)
            continue;

        for (int otherIdx = 0; otherIdx < universe.getNumRaces(); ++otherIdx)
        {
            if (enslavedIdx == otherIdx)
                continue;

            race* otherRace = universe.getRace(otherIdx);
            for (int hw = 0; hw < otherRace->getNumHomeworlds(); ++hw)
            {
                HomeWorld* homeworld = otherRace->getHomeworld(hw);
                if (homeworld->getControlRaceID() == enslavedRace->getRaceID())
                {
                    const point hwLoc = homeworld->getLoc();
                    otherRace->setHomeworldMajorStatus(hwLoc, RMS_FREE);
                    otherRace->setHomeworldControllerRace(hwLoc, otherRace->getRaceID(), otherRace->getDangerLevel());

                    map* subMap = universe
                        .getSubArea(universe.getSubAreaIndex(otherRace->getStarmapLoc()))
                        ->getMap();
                    subMap->setBackdrop(hwLoc, LBACKDROP_HOMEWORLD);

                    const int oldControllerID = universe.getRace(otherRace->getControllerRaceID())->getRaceID();
                    const int newControllerID = getDominantRaceIDInRegion(
                        universe.getSubArea(universe.getSubAreaIndex(otherRace->getStarmapLoc())));

                    if (otherRace->getRaceID() == newControllerID && otherRace->getRaceID() != otherRace->getControllerRaceID())
                    {
                        checkPlayerRecapturingRaceEvent(universe.getRace(oldControllerID), otherRace);
                    }

                    otherRace->setControllerRaceID(newControllerID);
                }
            }
        }
    }
}

void Game::checkPlayerRecapturingRaceEvent(race *oldController, race *original)
{
    display_obj.printMonitorWindow(color_white);

    msgeAddPromptSpace("Incoming Transmission...", cp_whiteonblack, true);

    if (!original->isSurrenderedToPlayer())
        return;

    ContactTree tree = createCapturedRaceByPlayerFreeContactTree(oldController, original);
    runContactScenario(tree);
}

void Game::executeMiscNPCTurnBasedData()
{
    if (current_maptype != MAPTYPE_LOCALEMAP)
        return;

    checkNPCDefeatedEvent();
    checkPlayerDefeatedEvent();

    for (int i = 0; i < currentRegion()->getNumShipNPCs(); ++i)
    {
        checkMobRegenerateEvent(currentRegion()->getNPCShip(i),MODULE_WEAPON);
        checkMobRegenerateEvent(currentRegion()->getNPCShip(i),MODULE_SHIELD);
    }

    if ((int)explosion_data.size() > 0)
    {
        createShipDestructionAnimations();
    }
}

void Game::checkForRaceWarEvent()
{
    for (auto i = 0; i < universe.getNumRaces(); i++)
    {
        race* attackingRace = universe.getRace(i);
        int attackerRaceID = attackingRace->getRaceID();
        int attackedRaceID = attackingRace->getRaceIDCommandedToAttack();

        if (attackedRaceID == -1)
            continue;

        race* attackedRace = universe.getRace(attackedRaceID);
        std::string raceAttackerName = attackingRace->getNameString();
        std::string raceAttackedRegionName = attackedRace->getNameString() + + " " + race_domain_suffix_string[(int)attackedRace->getRaceDomainType()];
       
        int numAttacksFromAttackerRace = numAttacksFromRace(attackingRace);
        int numAttacksFromAttackedRace = numAttacksFromRace(attackedRace);

        for (auto i = 0; i < numAttacksFromAttackerRace; i++)
        {
            raceInvasionEvent(attackingRace, attackedRace);
        }

        attackingRace->setRaceIDCommandedToAttack(-1);

        if (attackedRace->getRaceOverallMajorStatus() == RMS_FREE)
        {
            msgeAddPromptSpace("The " + raceAttackedRegionName + " retaliates on the arrival of the " + raceAttackerName + " ships!", cp_whiteonblack, false);

            for (auto i = 0; i < numAttacksFromAttackedRace; i++)
            {
                raceInvasionEvent(attackedRace, attackingRace);
            }
        }
    }
}

int numAttacksFromRace(race* attacker)
{
    return 1;
}

void Game::checkMobRegenerateEvent(MobShip* s, module_type mt)
{
    const bool isStarmap = (current_maptype == MAPTYPE_STARMAP);

    for (int i = 0; i < s->getNumInstalledModules(); ++i)
    {
        Module* mod = s->getModule(i);
        if (mod->getModuleType() != mt)
            continue;

        if (mod->getFillQuantity() >= mod->getMaxFillQuantity())
            continue;

        int regenDelay = 0;
        switch (mt)
        {
        case MODULE_WEAPON:
            regenDelay = mod->getWeaponStruct().regen_rate;
            break;
        case MODULE_SHIELD:
            regenDelay = mod->getShieldStruct().regen_rate;
            break;
        default:
            break;
        }

        if (regenDelay <= 0)
            continue;

        const int regenAmt = isStarmap ? std::max(LOCALEREGIONDEFAULTWID / regenDelay, 1) : 1;
        const bool shouldRegen = isStarmap || (turn_timer % regenDelay == 0);

        if (shouldRegen)
        {
            mod->offFillQuantity(regenAmt);
        }
    }
}

void Game::checkPlayerFuelEvent()
{
    if (current_maptype != MAPTYPE_STARMAP)
        return;

    if (getPlayerShip()->hasNoFuel())
    {
        msgeAdd("<<< !!! OUT OF FUEL !!! >>>",cp_redonblack);
        return;
    }
    if (getPlayerShip()->isLowOnFuel())
    {
        msgeAdd("<<< WARNING: LOW FUEL >>>",cp_yellowonblack);
    }

    getPlayerShip()->useFuelEvent(getPlayerShip()->getFuelConsumptionRate());
}

void Game::setPlayerLoc(point new_loc)
{
    // set the new player coordinates stored in the player's
    getPlayerShip()->setLoc(new_loc);
    // update upperleft display coordinates accordingly
    display_obj.updateUpperLeft(getPlayerShip()->at(),getMap()->getSize());
}

void Game::enterStation()
{
    if (currentRegion()->getSubAreaSpecificType() != SST_RACEHOME)
    {
        msgeAdd("You can't dock your ship here!", cp_grayonblack);
        return;
    }

    const point p = getPlayerShip()->at();
    const backdrop_t backdrop = getMap()->getBackdrop(p);
    const int nativeID = currentRegion()->getNativeRaceID();
    const int dominantRaceID = universe.getRace(nativeID)->getControllerRaceID();
    const int attStatus = universe.getRace(dominantRaceID)->getPlayerAttStatus();

    const bool canDock = attStatus >= 0;

    if (backdrop == LBACKDROP_SPACESTATION_SHIP)
    {
        canDock ? useSpaceStation(p)
            : msgeAdd("You are blocked from docking!", cp_grayonblack);
    }
    else if (backdrop == LBACKDROP_SPACESTATION_ENTERTAINMENT)
    {
        canDock ? useEntertainmentCenter(p)
            : msgeAdd("You are blocked from docking!", cp_grayonblack);
    }
    else
    {
        msgeAdd("You can't dock your ship here!", cp_grayonblack);
    }
}

void Game::setRegionTileForRaceHome(race* dominantRace)
{
    const backdrop_t tile = universe.getMap()->getBackdrop(last_smloc);
    const bool       surrendered = dominantRace->isSurrenderedToPlayer();
    const int        attitude = dominantRace->getPlayerAttStatus();

    auto apply = [&](bool cond, int delta) {
        if (cond) setVisitedStarMapTileBackdrop(last_smloc, delta);
        };

    if (surrendered)
    {
        apply(inRangeStarMapBackdropOrange(tile), 2 * NUM_STAR_TYPES);
        apply(inRangeStarMapBackdropRed(tile), 1 * NUM_STAR_TYPES);
        return;
    }

    if (attitude < 0)
    {
        apply(inRangeStarMapBackdropGreen(tile), -1 * NUM_STAR_TYPES);
        apply(inRangeStarMapBackdropOrange(tile), 1 * NUM_STAR_TYPES);
        apply(inRangeStarMapBackdropUnhighlighted(tile), 2 * NUM_STAR_TYPES);
    }
    else
    {
        apply(inRangeStarMapBackdropGreen(tile), -2 * NUM_STAR_TYPES);
        apply(inRangeStarMapBackdropRed(tile), -1 * NUM_STAR_TYPES);
        apply(inRangeStarMapBackdropUnhighlighted(tile), 1 * NUM_STAR_TYPES);
    }
}

void Game::checkSetRegionTileForRaceHome()
{
    if (currentRegion()->getSubAreaSpecificType() == SST_RACEHOME)
    {
        int dominantRaceID = getDominantRaceIDInRegion(currentRegion());

        setRegionTileForRaceHome(universe.getRace(dominantRaceID));
    }
}

void Game::setVisitedStarMapTileBackdrop(point smloc, int increment)
{
    universe.getMap()->setBackdrop(smloc, (backdrop_t)((int)universe.getMap()->getBackdrop(smloc) + increment));
}

void Game::setVisitedStarMapTileBackdrops(point smloc)
{
    switch (currentRegion()->getSubAreaSpecificType())
    {
        case(SST_RACEHOME):
        {
            int nativeRaceIndex = universe.getRaceIndex(smloc);

            race* nativeRace = universe.getRace(nativeRaceIndex);
            race* dominantRace = universe.getRace(nativeRace->getControllerRaceID());

            setRegionTileForRaceHome(dominantRace);
            break;
        }
        case(SST_WARZONE):
        {
            setVisitedStarMapTileBackdrop(smloc, NUM_STAR_TYPES*4);
            break;
        }
        case(SST_EMPTYSYSTEM):
        {
            setVisitedStarMapTileBackdrop(smloc, NUM_STAR_TYPES*5);
        }
        default:
            break;
    }
}

void Game::enterSubArea(bool isEncounter)
{
    const point playerLoc = getPlayerShip()->at();
    last_smloc = playerLoc;

    const bool isStarTile = isStarmapStarTile(getMap()->getBackdrop(playerLoc));
    const int raceIndex = universe.getRaceIndex(playerLoc);
    const bool raceAffiliated = isStarTile && raceIndex >= 0;

    if (isStarTile)
    {
        // Race-affiliated system with possible dialogue
        if (raceAffiliated)
        {
            race* targetRace = universe.getRace(raceIndex);
            const int controllerID = targetRace->getControllerRaceID();
            std::cout << targetRace->getDangerLevel() << "\n";

            if (!converseViaContactMenu(targetRace))
            {
                setRegionTileForRaceHome(universe.getRace(controllerID));
                reDisplayWithoutUpdate(true);
                return;
            }

            targetRace->setRaceIDByPlayerStatus(true);
        }

        setMobTileToNIL(getMap(), playerLoc);

        const int subareaIndex = universe.getSubAreaIndex(playerLoc);
        const star_type starType = getStarTypeFromStarMapTile(playerLoc);

        if (subareaIndex >= 0)
        {
            universe.setSubAreaMapType(SMT_PERSISTENT);
            current_subarea_id = subareaIndex;
            resetSurrenderedShips();
            printSubAreaEntranceMessage(false);
        }
        else
        {
            universe.createSubArea(
                playerLoc,
                SMT_PERSISTENT,
                raceAffiliated ? SST_RACEHOME : SST_EMPTYSYSTEM,
                false,
                starType
            );

            current_subarea_id = universe.getNumSubAreas() - 1;
            setVisitedStarMapTileBackdrops(playerLoc);
            printSubAreaEntranceMessage(true);
        }

        current_maptype = MAPTYPE_LOCALEMAP;
        pathfinder.setupPathfindDistVector(getMapSize());
        setPlayerLoc(getSubAreaSystemStartPoint(getMapSize()));
    }
    else
    {
        if (isEncounter)
            msgeAddPromptSpace("An encounter!", cp_whiteonblack, true);

        setMobTileToNIL(getMap(), playerLoc);

        universe.createSubArea(
            playerLoc,
            SMT_NONPERSISTENT,
            isEncounter ? SST_PIRATEVOID : SST_EMPTYVOID,
            isEncounter,
            STARTYPE_NONE
        );

        current_maptype = MAPTYPE_LOCALEMAP;
        pathfinder.setupPathfindDistVector(getMapSize());
        setPlayerLoc(point(getMapSize().x() / 2, getMapSize().y() / 2));

        if (isEncounter)
        {
            playerHasMoved = false;
            addEncounterShips();
        }

        printSubAreaEntranceMessage(true);
    }

    changeMobTile(point(0, 0), getPlayerShip()->at(), getPlayerShip()->getMobType());
}

void resetSurrenderedShips()
{
    for (auto i = 0; i < currentRegion()->getNumShipNPCs(); i++)
    {
        MobShip* ship = currentRegion()->getNPCShip(i);
        const int shipRaceID = ship->getMobSubAreaGroupID();
        race* shipRace = universe.getRace(shipRaceID);

        if (shipRace->isSurrenderedToPlayer() && ship->getMobSubAreaAttackID() == 0)
        {
            ship->setMobSubAreaAttackID(-1);
            ship->setAIPattern(AIPATTERN_NEUTRAL);
            ship->setGoalStatus(GOALSTATUS_COMPLETE);
        }
    }
}

star_type getStarTypeFromStarMapTile(point loc)
{
    backdrop_t starMapTile = getMap()->getBackdrop(loc);

    if (isMainSequenceBackdropTile(starMapTile))
    {
        return STARTYPE_MAINSEQ;
    }
    if (isRedStarBackdropTile(starMapTile))
    {
        return STARTYPE_RED;
    }
    if (isBlueStarBackdropTile(starMapTile))
    {
        return STARTYPE_BLUE;
    }
    if (isWhiteStarBackdropTile(starMapTile))
    {
        return STARTYPE_WHITE;
    }

    return STARTYPE_NONE;
}

void Game::printSubAreaEntranceMessage(bool initialEnterance)
{
    switch(currentRegion()->getSubAreaSpecificType())
    {
        case(SST_EMPTYVOID):
             msgeAdd("You enter the void.",cp_whiteonblack);
             break;
        case(SST_PIRATEVOID):
             msgeAdd("Your ship is being targeted by space pirates!",cp_whiteonblack);
             break;
        case(SST_EMPTYSYSTEM):
             msgeAdd("You enter an uninhabited star system.",cp_whiteonblack);
             break;
        case(SST_RACEHOME):
             msgeAdd("You enter the occupied star system.",cp_whiteonblack);
             break;
        case(SST_WARZONE):
             if (initialEnterance)
                 msgeAdd("You have entered a star region of a race destroyed long ago. It appears to be a war zone now!", cp_whiteonblack);
             else
                 msgeAdd("You enter the war zone.", cp_whiteonblack);
             break;
        default:
             break;
    }
}

const int Game::maxPirateDangerLevelFromTurnTimer()
{
    static const std::vector<std::pair<unsigned int, int>> dangerThresholds = {
        {  400, 12 }, {  900, 13 }, { 1500, 14 }, { 2200, 15 },
        { 3000, 16 }, { 4000, 17 }, { 5500, 18 }, { 7000, 19 },
        { 8500, 20 }, {10000, 21 }, {12500, 22 }, {16000, 23 },
        {20000, 24 }
    };

    for (const auto& [threshold, level] : dangerThresholds)
    {
        if (turn_timer < threshold)
            return level;
    }

    return 25;
}

void Game::addEncounterShips()
{
    constexpr int maxTries = 50;
    int raceID = -1;
    int dangerLevel = 1;
    shipmob_classtype classType = CLASSTYPE_NONE;

    const int maxDangerLevel = maxPirateDangerLevelFromTurnTimer();

    // Select a suitable pirate race (low danger, must have fighter ship)
    for (int tries = 0; tries < maxTries; ++tries)
    {
        int candidateID = randInt(0, universe.getNumRaces() - 1);
        race* candidateRace = universe.getRace(candidateID);
        MobShip* nativeShip = candidateRace->getNativeShip(0);

        if (!nativeShip || candidateRace->getPlayerAttStatus() >= 0)
            continue;

        dangerLevel = candidateRace->getDangerLevel();
        classType = nativeShip->getStatStruct().sctype;

        if (dangerLevel <= maxDangerLevel && classType == CLASSTYPE_FIGHTER)
        {
            raceID = candidateID;
            break;
        }
    }

    // Fallback in case no suitable race found
    if (raceID == -1)
        raceID = randInt(0, universe.getNumRaces() - 1);

    const int numShips = randInt(1, randInt(1, 4));
    race* pirateRace = universe.getRace(raceID);

    for (int i = 0; i < numShips; ++i)
    {
        point spawnLoc;
        
        do {
            spawnLoc = getRandNPCShipOpenPoint(
                getMap(),
                point(1, 1),
                addPoints(getMapSize(), point(-2, -2)),
                LBACKDROP_SPACE_UNLIT,
                LBACKDROP_SPACE_LIT
            );
        } while (spawnLoc.x() == getMapSize().x() / 2 || spawnLoc.y() == getMapSize().y() / 2);

        currentRegion()->createNPCShip(pirateRace->getNativeShip(0), spawnLoc, raceID);
        MobShip* ship = currentRegion()->getNPCShip(i);

        ship->setShipName(fullShipName(ship, pirateRace->getNameString(), classType, true));
        ship->setMobSubAreaAttackID(0);
    }
}

void Game::enterArea()
{
    playerHasMoved = false;

    if (current_tab != TABTYPE_PLAYAREA)
    {
        msgeAdd("You must unselect your ship module configurator before you can proceed", cp_grayonblack);
        return;
    }

    switch(current_maptype)
    {
        case(MAPTYPE_STARMAP):
        {
            enterSubArea(false);
            return;
        }
        case(MAPTYPE_LOCALEMAP):
        {
            enterStation();
            return;
        }
        default:
            break;
    }
}

void Game::navigateMenu(menu *mu)
{
    if (event_handler.getDelta().y() < 0)
        mu->decSelectionIndex();
    else
        mu->incSelectionIndex();
}

void Game::useSpaceStation(point p)
{
    bool exitMenu = false;
    msgeAddPromptSpace("Your ship docks at the space station.", cp_blackonwhite, true);

    int selection = 0;
    station_menu_obj.setSelectionIndex(0);

    race* stationRace = universe.getRace(currentRegion()->getNativeRaceID());
    station* s = currentRegion()->getStation(p);

    setStationContactData(stationRace, s, &station_menu_obj, getPlayerShip(), 0);

    while (!exitMenu)
    {
        display_obj.displayMenu(&station_menu_obj);
        event_handler.setAction();

        switch (event_handler.getAction())
        {
        case INP_DELTA:
            navigateMenu(&station_menu_obj);
            break;

        case INP_SELECT:
        {
            selection = station_menu_obj.getSelectionIndex();

            if (station_menu_obj.getMenuLevel() == 0)
            {
                if (selection >= s->getNumBasicStationTradeChoices())
                {
                    exitMenu = true;
                    station_menu_obj.setSelectionIndex(0);
                    break;
                }

                basic_station_trade_choice choice = s->getBasicStationTradeChoice(selection);
                switch (choice)
                {
                case STATIONCHOICE_BUYMODULE:
                    setStationContactData(stationRace, s, &station_menu_obj, getPlayerShip(), 1);
                    station_menu_obj.setSelectionIndex(0);
                    break;

                case STATIONCHOICE_SELLMODULE:
                    setStationContactData(stationRace, s, &station_menu_obj, getPlayerShip(), 2);
                    station_menu_obj.setSelectionIndex(0);
                    break;

                case STATIONCHOICE_HULLREPAIR:
                    repairPlayerHull(s);
                    break;

                case STATIONCHOICE_HULLUPGRADE:
                    upgradePlayerHull(s);
                    break;

                case STATIONCHOICE_INCNUMMODULECAPACITY:
                    upgradePlayerSlotCapacity(s);
                    break;

                case STATIONCHOICE_FUEL:
                    playerBuyFuel(s);
                    break;

                case STATIONCHOICE_HIRECREW:
                    playerHireCrew(s);
                    break;

                default:
                    break;
                }
            }
            else if (station_menu_obj.getMenuLevel() == 1)
            {
                if (selection == s->getNumModulesForTrade())
                {
                    setStationContactData(stationRace, s, &station_menu_obj, getPlayerShip(), 0);
                    station_menu_obj.setSelectionIndex(0);
                }
                else if (selection == s->getNumModulesForTrade() + 1)
                {
                    exitMenu = true;
                    station_menu_obj.setSelectionIndex(0);
                }
                else
                {
                    playerPurchaseModule(s, &station_menu_obj);
                }
            }
            else // selling
            {
                if (selection == getPlayerShip()->getNumInstalledModules())
                {
                    setStationContactData(stationRace, s, &station_menu_obj, getPlayerShip(), 0);
                    station_menu_obj.setSelectionIndex(0);
                    station_menu_obj.setView(station_menu_obj.getLoc(), getSmallStationMenuSize());
                    reDisplayWithoutUpdate(true);
                }
                else if (selection == getPlayerShip()->getNumInstalledModules() + 1)
                {
                    exitMenu = true;
                    station_menu_obj.setSelectionIndex(0);
                }
                else
                {
                    playerSellModule(s, &station_menu_obj);
                }
            }
            break;
        }

        default:
            break;
        }
    }

    msgeAdd("Your ship leaves the station.", cp_grayonblack);
    station_menu_obj.clearMenuItems();
    station_menu_obj.clearMenuMainText();
    station_menu_obj.setView(station_menu_obj.getLoc(), getSmallStationMenuSize());
}

void Game::useEntertainmentCenter(point p)
{
    msgeAddPromptSpace("Your ship docks at the space station.", cp_blackonwhite, true);

    bool exitMenu = false;
    int selection = 0;

    EntertainmentStation* center = currentRegion()->getEntertainmentCenter(p);
    const int numSlot = center->getNumSlotMachines();
    const int numDiamond = center->getNumDiamondMachines();
    const int totalOptions = center->getNumOptions();

    const int raceID = currentRegion()->getNativeRaceID();

    setEntertainmentCenterContactData(universe.getRace(raceID), center, &entertainmentStationMenu, 0);
    entertainmentStationMenu.setSelectionIndex(0);

    while (!exitMenu)
    {
        display_obj.displayMenu(&entertainmentStationMenu);
        event_handler.setAction();

        if (event_handler.getAction() == INP_DELTA)
        {
            navigateMenu(&entertainmentStationMenu);
            continue;
        }

        if (event_handler.getAction() != INP_SELECT)
            continue;

        selection = entertainmentStationMenu.getSelectionIndex();

        if (selection >= totalOptions)
        {
            exitMenu = true;
        }
        else if (selection >= numSlot) // diamond machines
        {
            useMachinePlayer(center->getDiamondsMachine(selection - numSlot));
            reDisplayWithoutUpdate(false);
        }
        else // slot machines
        {
            useMachinePlayer(center->getSlotMachine(selection));
            reDisplayWithoutUpdate(false);
        }
    }

    msgeAdd("Your ship leaves the station.", cp_grayonblack);
    station_menu_obj.clearMenuItems();
    station_menu_obj.clearMenuMainText();
}

template <typename M>
void Game::featurePlayerSelect(M* mach_obj, point delta)
{
    mach_obj->gotoNextState(delta);
    mach_obj->drawMachine();
    gfx_obj.updateScreen();
}

template <typename M>
void Game::featurePlayerToggle(M* mach_obj)
{
    uint64_t machine_retval = 0;

    display_obj.clearAndDeleteAllMessages();

    msgeAdd("You press a button.", cp_grayonblack);

    machine_retval = mach_obj->useMachine(getPlayerShip()->getNumCredits());

    machine_state ms = mach_obj->getState();

    if (ms == MSTATE_SLOTCASHOUT || ms == MSTATE_DIAMONDCASHOUT)
    {
        getPlayerShip()->setNumCredits(getPlayerShip()->getNumCredits() + machine_retval);
    }

    if (ms == MSTATE_SLOTINSERT || ms == MSTATE_DIAMONDINSERT)
    {
        getPlayerShip()->setNumCredits(0ULL);
    }

    if (ms == MSTATE_SLOTLEAVE || ms == MSTATE_DIAMONDLEAVE)
    {
        usingMachine = false;
    }
}

template <typename M>
void Game::useMachinePlayer(M* mach_obj)
{
    usingMachine = true;
    do
    {
        display_obj.displayMachineBox();
        mach_obj->drawMachine();
        gfx_obj.updateScreen();
        event_handler.setAction();
        if (event_handler.getAction() == INP_SELECT)
        {
            featurePlayerToggle(mach_obj);
        }
        else if (event_handler.getAction() == INP_DELTA)
        {
            featurePlayerSelect(mach_obj, event_handler.getDelta());
        }
    } while (usingMachine);
}

void Game::upgradePlayerHull(station* ss)
{
    display_obj.clearAndDeleteAllMessages();

    auto* ps = getPlayerShip();
    const auto cost = ss->getHullUpgradeCost();

    if (cost > ps->getNumCredits()) {
        msgeAdd("You can't afford to upgrade your hull here!", cp_grayonblack);
        return;
    }
    if (ps->getHullStatus() < ps->getMaxHull()) {
        msgeAdd("You must completely repair your hull before you can upgrade it.", cp_grayonblack);
        return;
    }

    ps->setMaxHull(ps->getMaxHull() + 1);
    ps->setHullStatus(ps->getMaxHull());
    ps->setNumCredits(ps->getNumCredits() - cost);

    display_obj.printShipStatsSection(ps);
    msgeAdd("Your hull has been upgraded!", cp_whiteonblack);
}

void Game::repairPlayerHull(station* ss)
{
    display_obj.clearAndDeleteAllMessages();

    auto* ps = getPlayerShip();
    const auto cost = ss->getHullFixCost();

    if (cost > ps->getNumCredits()) {
        msgeAdd("You can't afford to repair your hull here!", cp_grayonblack);
        return;
    }
    if (ps->getHullStatus() >= ps->getMaxHull()) {
        msgeAdd("Your ship's hull is in perfect condition already.", cp_grayonblack);
        return;
    }

    ps->setHullStatus(ps->getHullStatus() + 1);
    ps->setNumCredits(ps->getNumCredits() - cost);

    display_obj.printShipStatsSection(ps);
    msgeAdd("The crew at the station work on repairing your ship.", cp_whiteonblack);
}

void Game::playerBuyFuel(station* ss)
{
    display_obj.clearAndDeleteAllMessages();

    auto* ps = getPlayerShip();
    const auto cost = ss->getFuelCost();
    const int cap = ps->getTotalMTFillCapacity(MODULE_FUEL);
    const int fill = ps->getTotalMTFillRemaining(MODULE_FUEL);

    if (cost > ps->getNumCredits()) {
        msgeAdd("You can't afford to buy fuel here!", cp_grayonblack);
        return;
    }
    if (cap == fill) {
        msgeAdd("Your ship's fuel tank capacity has already been reached!", cp_grayonblack);
        return;
    }

    ps->incTotalFillAmount(1, MODULE_FUEL);
    ps->setNumCredits(ps->getNumCredits() - cost);

    display_obj.printShipStatsSection(ps);
    display_obj.printShipGraphic(ps, SHOWHGT / 2);
    msgeAdd("You purchase 1 unit of fuel. A low-pitched \"*GLUGG*\" sound echoes throughout your ship.", cp_whiteonblack);
}

void Game::upgradePlayerSlotCapacity(station* ss)
{
    display_obj.clearAndDeleteAllMessages();

    auto* ps = getPlayerShip();
    const auto cost = ss->getSlotCapUpgradeCost();

    if (cost > ps->getNumCredits()) {
        msgeAdd("You can't afford to increase your module slot capacity here!", cp_grayonblack);
        return;
    }
    if (ps->getMaxNumModules() >= MAX_PLAYER_SLOTS) {
        msgeAdd("Your ship's structure does not support having more than " + int2String(MAX_PLAYER_SLOTS) + " slots.", cp_grayonblack);
        return;
    }

    ps->setNumMaxModules(ps->getMaxNumModules() + 1);
    ps->setNumCredits(ps->getNumCredits() - cost);

    display_obj.printShipStatsSection(ps);
    display_obj.printShipGraphic(ps, SHOWHGT / 2);
    msgeAdd("Your ships module slot capacity has been upgraded!", cp_whiteonblack);
}

void Game::playerHireCrew(station* ss)
{
    display_obj.clearAndDeleteAllMessages();

    auto* ps = getPlayerShip();
    const auto cost = ss->getCrewCost();
    const int cap = ps->getTotalMTFillCapacity(MODULE_CREW);
    const int fill = ps->getTotalMTFillRemaining(MODULE_CREW);

    if (cost > ps->getNumCredits()) {
        msgeAdd("You can't afford to hire any crew here!", cp_grayonblack);
        return;
    }
    if (cap == fill) {
        msgeAdd("Your ship is currently at the maximum crew capacity.", cp_grayonblack);
        return;
    }

    ps->incTotalFillAmount(1, MODULE_CREW);
    ps->setNumCredits(ps->getNumCredits() - cost);

    display_obj.printShipStatsSection(ps);
    display_obj.printShipGraphic(ps, SHOWHGT / 2);
    msgeAdd("A very bizarre looking alien joins your ship's crew.", cp_whiteonblack);
}

void Game::playerPurchaseModule(station* ss, menu* menuObj)
{
    const int selection = menuObj->getSelectionIndex();
    if (selection < 0 || selection >= ss->getNumModulesForTrade())
        return;

    display_obj.clearAndDeleteAllMessages();

    auto* ps = getPlayerShip();
    const uint_64 cost = ss->getModuleForTradeCost(selection);
    const int slotsAvail = ps->getMaxNumModules() - ps->getNumInstalledModules();

    if (cost > ps->getNumCredits()) {
        msgeAdd("You do not have enough credits for that module!", cp_whiteonblack);
        return;
    }
    if (slotsAvail < 1) {
        msgeAdd("Your ship already has the maximum number of modules it can hold!", cp_whiteonblack);
        return;
    }

    if (Module* src = ss->getModuleForTrade(selection)) {
        ps->addModule(*src);
        ps->setNumCredits(ps->getNumCredits() - cost);
        ss->eraseModule(selection);
        menuObj->eraseMenuItem(selection);

        display_obj.printShipStatsSection(ps);
        display_obj.printShipGraphic(ps, SHOWHGT / 2);
        msgeAdd("Your new module has been installed!", cp_purpleonblack);
    }
    else {
        msgeAdd("That module is no longer available.", cp_grayonblack);
    }
}

void Game::playerSellModule(station* ss, menu* menuObj)
{
    const int selection = menuObj->getSelectionIndex();
    auto* ps = getPlayerShip();

    if (selection < 0 || selection >= ps->getNumInstalledModules())
        return;

    display_obj.clearAndDeleteAllMessages();

    Module* mod = ps->getModule(selection);
    if (!mod) {
        msgeAdd("That module cannot be found.", cp_grayonblack);
        return;
    }

    // Safety checks
    if (mod->getModuleType() == MODULE_ENGINE &&
        ps->getNumInstalledModulesOfType(MODULE_ENGINE) == 1)
    {
        msgeAdd("You cannot sell your ship's sole engine!", cp_grayonblack);
        return;
    }
    if ((mod->getModuleType() == MODULE_FUEL || mod->getModuleType() == MODULE_CREW) &&
        mod->getFillQuantity() > 0)
    {
        msgeAdd(mod->getModuleType() == MODULE_FUEL
            ? "You cannot sell a non-empty fuel tank!"
            : "You cannot sell a non-empty crew pod!",
            cp_grayonblack);
        return;
    }

    const uint_64 value = ss->getModuleSellCost(mod);

    ps->setNumCredits(ps->getNumCredits() + value);
    ps->eraseModule(selection);
    ps->setModuleSelectionIndex(0);
    menuObj->eraseMenuItem(selection);

    display_obj.printShipStatsSection(ps);
    display_obj.printShipGraphic(ps, SHOWHGT / 2);
    msgeAdd("You sell the module; it is recycled into raw materials.", cp_whiteonblack);
}

// returns true if conversation leads to entering subarea
bool Game::converseViaContactMenu(race* nativeRace)
{
    bool exit_menu = false;
    bool converse_result = false;

    race* controllerRace = universe.getRace(nativeRace->getControllerRaceID());

    if (controllerRace->getRacePersonalityType() == PERSONALITY_NONE)
        return true;

    if (!nativeRace->playerIdentifiedByRace())
    {
        msgeAddPromptSpace("You encounter an unidentified alien region!", cp_blackonwhite, true);
    }
    else
    {
        msgeAdd("(e)nter, (c)onverse, (i)gnore?",cp_blackonwhite);
        gfx_obj.updateScreen();
        SDL_Keycode enter_action = event_handler.getKeyPressed();
        display_obj.clearAndDeleteAllMessages();

        bool nativeAreaSeiged = false;

        const int controllerNativeRegionIndex = universe.getSubAreaIndex(controllerRace->getStarmapLoc());

        if (controllerNativeRegionIndex >= 0)
        {
            const int totalActiveNPCs = universe.getSubArea(controllerNativeRegionIndex)->getNumActiveShipsPresent();
            const int activeNatives = universe.getSubArea(controllerNativeRegionIndex)->getNumActiveNativeShipsPresent();

            if (totalActiveNPCs > activeNatives || controllerRace->getRaceOverallMajorStatus() == RMS_ENSLAVED)
            {
                nativeAreaSeiged = true;
            }
        }

        switch(enter_action)
        {
            case(SDLK_e):
                if (controllerRace->getPlayerAttStatus() >= 0 || nativeAreaSeiged)
                    return true;
                break;
            case(SDLK_c):
            {
                if (nativeAreaSeiged)
                {
                    msgeAddPromptSpace("Due to the seige at The " + controllerRace->getNameString() + " " + race_domain_suffix_string[(int)controllerRace->getRaceDomainType()] + ", they are unwilling to make contact.", cp_whiteonblack, true);
                    return false;
                }
                break;
            }
            default:
                return false;
        }
    }

    bool enterSubArea = false;
    msgeAddPromptSpace("Incoming Transmission...", cp_whiteonblack, true);
    executeSubareaEntranceContactScenario(controllerRace, nativeRace, enterSubArea);
    nativeRace->setPlayerIDByRaceStatus(true);
    raceIDNameMapDiscovered[nativeRace->getRaceID()] = nativeRace->getNameString();
    return enterSubArea;
}

void Game::returnToStarMap()
{
    checkSetRegionTileForRaceHome();
    setMobTileToNIL(getMap(), getPlayerShip()->at());
    current_maptype = MAPTYPE_STARMAP;
    universe.setSubAreaMapType(SMT_NONE);
    current_subarea_id = -1;
    setPlayerLoc(last_smloc);
    changeMobTile(point(0,0),getPlayerShip()->at(),getPlayerShip()->getMobType());
    reDisplay(true);
}

void Game::playerFireWeapon(point delta)
{
    if (current_tab != TABTYPE_PLAYAREA)
        return;

    if (current_maptype != MAPTYPE_LOCALEMAP)
    {
        msgeAdd("You can't shoot anything here!", cp_grayonblack);
        return;
    }

    point fireLocation = addPoints(getPlayerShip()->at(), delta);

    if (inRange(fireLocation, point(0, 0), point(SHOWWID - 1, SHOWHGT - 1)))
    {
        mobFire(getPlayerShip(), fireLocation);
    }
    else
    {
        msgeAdd("You can't shoot in that direction!", cp_grayonblack);
    }
}

void Game::printTogglePromptMessage(input_t type, bool success)
{
    const bool isExamine = (type == INP_EXAMINE);

    const std::string message = success
        ? (isExamine ? "Look where?" : "Fire where?")
        : (isExamine ? "You can't examine anything here!" : "You can't fire at anything here!");

    const color_pair color = success ? cp_whiteonblack : cp_grayonblack;

    msgeAdd(message, color);
}

bool Game::checkCanTargetBasedOnModule(input_t actionType)
{
    if (actionType != INP_WEAPONFIRE)
        return true;

    return getCurrentMobSelectedModule(getPlayerShip())->getModuleType() == MODULE_WEAPON;
}

bool Game::checkCanTargetBasedOnStraightLine(MobShip* target, input_t actionType)
{
    if (actionType != INP_WEAPONFIRE)
        return true;

    const point& targetLoc = target->at();
    const point& playerLoc = getPlayerShip()->at();

    return isInStraightLine(targetLoc, playerLoc) ||
        !eightDirectionRestrictedWeaponSelected(getPlayerShip());
}

bool Game::sufficientFillQuantity(MobShip* ship)
{
    Module* selectedModule = getCurrentMobSelectedModule(ship);
    return selectedModule->getFillQuantity() >= getWeaponModuleConsumptionPerTurn(selectedModule);
}

void Game::cycleTarget()
{
    current_player_target = (current_player_target > 0 ? current_player_target - 1 : currentRegion()->getNumShipNPCs() - 1);
}

point Game::getNextToggleDeltaForFireWeapon(point at, point delta)
{
    const point next = addPoints(at, delta);

    // If weapon is NOT 8-dir restricted, return naive next (original behavior: no bounds check here)
    if (!eightDirectionRestrictedWeaponSelected(getPlayerShip()))
        return next;

    // 8-dir restricted path: snap to straight line from player toward target cell
    const point playerAt = getPlayerShip()->at();

    point snapped = next;
    if (!isInStraightLine(playerAt, next))
    {
        const point playerTargetDelta = pointDistance(playerAt, at);
        const int   maxCoord = std::max(std::abs(playerTargetDelta.x()), std::abs(playerTargetDelta.y()));
        const point distancePoint = multiplyPoints(point(maxCoord, maxCoord), delta);
        snapped = addPoints(playerAt, distancePoint);
    }

    // Only in the restricted path do we range-check (matches original)
    const point mapMax = point(currentRegion()->getMap()->getSize().x() - 1,
        currentRegion()->getMap()->getSize().y() - 1);

    if (inRange(snapped, point(0, 0), mapMax))
        return snapped;

    return at;
}

void Game::playerTargetToggle(input_t type)
{
    if (current_tab != TABTYPE_PLAYAREA)
        return;

    if (current_maptype != MAPTYPE_LOCALEMAP) {
        printTogglePromptMessage(type, false);
        return;
    }

    auto* const map = getMap();
    auto* const player = getPlayerShip();
    auto* const region = currentRegion();

    point at = getNextTargetedNPC(type);

    // Must have a weapon selected
    if (!checkCanTargetBasedOnModule(type)) {
        msgeAdd("Your selected module is not a weapon!", cp_grayonblack);
        return;
    }

    // Must have resources to fire
    if (type == INP_WEAPONFIRE && !sufficientFillQuantity(player)) {
        msgeAdd("You do not have enough power or projectiles to fire that weapon now!", cp_grayonblack);
        cycleTarget();
        return;
    }

    printTogglePromptMessage(type, true);

    point tempLoc = at;

    // Targeting loop until fire input
    do
    {
        printPlayerFirePath(player->at(), at);

        if ((int)map->getMob(at) > (int)SHIP_PLAYER)
            display_obj.displayMonitor(MONITOR_TARGETINFO, region->getNPCShip(at));

        gfx_obj.updateScreen();
        event_handler.setAction();

        if (event_handler.getAction() == INP_DELTA)
        {
            const point d = event_handler.getDelta();
            tempLoc = (type == INP_WEAPONFIRE)
                ? getNextToggleDeltaForFireWeapon(at, d)
                : addPoints(at, d);

            setPointIfInMapRangeAndLOS(tempLoc, at);
        }

        if (isAt(tempLoc, at))
            clearAllFireCells(map);

        if (event_handler.getAction() == INP_TOGGLE)
        {
            clearAllFireCells(map);
            cycleTarget();
            return;
        }

        if (event_handler.getAction() == INP_SELECT)
        {
            clearAllFireCells(map);
            at = getNextTargetedNPC(type);
        }

    } while (event_handler.getAction() != INP_WEAPONFIRE);

    playerHasMoved = true;

    clearAllFireCells(map);
    gfx_obj.updateScreen();

    if (type == INP_WEAPONFIRE)
    {
        if (!isAt(at, player->at()))
            mobFire(player, at);
        else
            msgeAdd("Cannot target your own ship...", cp_grayonblack);
    }

    cycleTarget();
}

void Game::printPlayerFirePath(point a, point b)
{
    point tracerP;

    tracer.bresenham(getMap()->getSize(),a,b);

    cell * cellAP = getMap()->getCellP(a);
    cell * cellBP = getMap()->getCellP(b);

    getMap()->setFire(b,FIRET_CURSOR);
    cellBP->setLastFireSymbol(cellBP->getCurrentFireSymbol());

    for (int i = 1; i < tracer.getLineSize(); ++i)
    {
        tracerP = tracer.getLinePoint(i);

	cell * cellTP = getMap()->getCellP(tracerP);

        getMap()->setFire(tracer.getLinePoint(i),FIRET_CURSORPATH);
        cellTP->setLastFireSymbol(cellTP->getCurrentFireSymbol());
    }

    if (!isAt(a,b))
    {
        getMap()->setFire(a,NIL_f);
        cellAP->setLastFireSymbol(cellAP->getCurrentFireSymbol());
    }

    display_obj.printMap(getMap());
    printMobCells();
}

point Game::getNextTargetedNPC(input_t inp)
{
    bool found_target = false;

    int iter = 0;

    point loc = getPlayerShip()->at();

    for (int i = 0; i < currentRegion()->getNumShipNPCs(); ++i)
    {
        if (isTargetableNPC(i, inp))
            iter++;
    }

    if (iter == 0)
        return loc;

    current_player_target = (int)std::max(current_player_target,0);
    current_player_target = (int)std::min(currentRegion()->getNumShipNPCs()-1,current_player_target);

    do
    {
        if (isTargetableNPC(current_player_target, inp))
        {
            loc = currentRegion()->getNPCShip(current_player_target)->at();
            found_target = true;
        }

        current_player_target = (current_player_target + 1) % currentRegion()->getNumShipNPCs();
    }
    while (!found_target);

    return loc;
}

bool Game::isTargetableNPC(int n, input_t inp)
{
    MobShip* nShip = currentRegion()->getNPCShip(n);

    const point& targetPos = nShip->at();

    // If the tile is visible
    if (getMap()->getv(targetPos))
    {
        // Examining doesn't require range or line-of-fire checks
        if (inp == INP_EXAMINE)
            return true;

        if (!nShip->isActivated())
            return false;
    }
    else
    {
        // Target not visible
        return false;
    }

    // Check weapon range
    const int dist = shortestPath(getPlayerShip()->at(), targetPos);
    const int range = getCurrentMobSelectedModule(getPlayerShip())->getWeaponStruct().travel_range;
    if (dist > range)
        return false;

    // Check if weapon can target through line-of-sight
    if (!checkCanTargetBasedOnStraightLine(nShip, inp))
        return false;

    return true;
}

void Game::checkNPCDefeatedEvent()
{
    if (current_maptype != MAPTYPE_LOCALEMAP)
        return;

    // Check for destroyed ships
    for (int i = 0; i < currentRegion()->getNumShipNPCs(); /* no increment here */)
    {
        MobShip* ship = currentRegion()->getNPCShip(i);

        if (ship->getHullStatus() <= 0)
        {
            const point loc = ship->at();
            const int id = ship->getMobSubAreaID();
            const uint_64 credits = ship->getNumCredits();

            setMobTileToNIL(getMap(), loc);

            if (credits > 0)
                currentRegion()->addItem(standard_item_stats[0], randInt(1, randInt(1, static_cast<int>(credits))), loc);

            explosion_data.push_back({ loc, ship->getStatStruct().destruction_radius, 0 });

            if (getMap()->getv(loc))
                msgeAdd(ship->getShipName() + " destroyed!", ship->getShipSymbol().color);

            checkNPCShipDefeatedByPlayer(ship);

            currentRegion()->destroyNPC(i);
            resetAttackIDs(id);

            // Restart loop due to change in ship vector
            i = 0;
            continue;
        }

        ++i;
    }

    // Check for ships that lost all crew
    for (int i = 0; i < currentRegion()->getNumShipNPCs(); ++i)
    {
        MobShip* ship = currentRegion()->getNPCShip(i);

        if (ship->crewOperable() && ship->isActivated() &&
            ship->getTotalMTFillRemaining(MODULE_CREW) <= 0)
        {
            const int id = ship->getMobSubAreaID();

            if (getMap()->getv(ship->at()))
                msgeAdd("All crew aboard the " + ship->getShipName() + " have perished!", ship->getShipSymbol().color);

            checkNPCShipDefeatedByPlayer(ship);

            ship->setActivationStatus(false);
            resetAttackIDs(id);
        }
    }
}

void Game::checkNPCShipDefeatedByPlayer(MobShip * ship)
{
    if (currentRegion()->getSubAreaSpecificType() != SST_RACEHOME || !ship->isActivated())
        return;

    race* shipRace = universe.getRace(ship->getMobSubAreaGroupID());
    race* nativeRace = universe.getRace(currentRegion()->getNativeRaceID());

    if (shipRace->getRaceID() == nativeRace->getRaceID() && ship->getMobIDLastAttackedBy() == 0)
    {
        shipRace->setNumShipsDestroyedByPlayerAtRegion(shipRace->getNumShipsDestroyedByPlayerAtRegion() + 1);
    }
}

void Game::checkPlayerDefeatedEvent()
{
    auto* const player = getPlayerShip();
    auto* const map = getMap();

    // Hull destroyed explosion, remove tile, end game
    if (player->getHullStatus() <= 0)
    {
        const point p = player->at();

        msgeAdd("Your ship has been destroyed!", cp_darkredonblack);
        printExitPromptMessage();
        display_obj.printShipStatsSection(player);

        setMobTileToNIL(map, p);
        explosion_data.push_back({ p, player->getStatStruct().destruction_radius, 0 });

        resetAttackIDs(0);
        Game_active = false;
        return;
    }

    // No crew remaining deactivate ship, end game (no explosion)
    if (player->getTotalMTFillRemaining(MODULE_CREW) <= 0)
    {
        msgeAdd("All crew members aboard your ship have vanquished!", cp_darkredonblack);
        player->setActivationStatus(false);
        printExitPromptMessage();
        display_obj.printShipStatsSection(player);

        resetAttackIDs(0);
        Game_active = false;
    }
}

void Game::createShipDestructionAnimations()
{
    if (explosion_data.empty()) {
        return;
    }

    auto* const map = getMap();

    // Find the maximum explosion radius (at least 1, matching original init)
    int max_radius = 1;
    for (const auto& e : explosion_data)
        max_radius = std::max(max_radius, static_cast<int>(e.radius));

    // Iterate "rings" out to max_radius*2 (same semantics as before)
    for (int c = 0; c < max_radius * 2; ++c)
    {
        int num_visible = 0;

        for (auto& e : explosion_data)
        {
            if (c < e.radius * 2 && map->getv(e.loc))
            {
                const fire_t ft = (c < e.radius) ? FIRET_EXPLOSION : NIL_f;
                e.iteration = (c != e.radius) ? (e.iteration + 1) : 1;
                printFireCircle(e.loc, e.iteration, ft);
                ++num_visible;
            }
        }

        if (num_visible > 0)
            display_obj.delayAndUpdate(30);
        else
            break; // nothing visible this ring; stop early
    }

    explosion_data.clear();
}

void Game::setPointIfInMapRangeAndLOS(point oldLoc, point& newLoc)
{
    if (inRange(oldLoc, point(0, 0), getMaxMapPoint(getMap())))
    {
        if (getMap()->getv(oldLoc))
        {
            newLoc = oldLoc;
        }
    }
}

void Game::pickUpItems(MobShip *mb, int s, int q)
{
    if (current_maptype != MAPTYPE_LOCALEMAP)
        return;

    item_stats istats = *(currentRegion()->getPile(mb->at())->getItem(s)->getStats());

    if (istats.i_type == ITEM_WRECKAGECREDIT)
        mb->setNumCredits(mb->getNumCredits() + (uint_64)q);

    currentRegion()->deleteItemFromPile(mb->at(),s,q);
}

void Game::initMenus()
{
    contact_menu_obj = menu(point(2, 2), point(GRIDWID - 4, GRIDHGT - 14));
    station_menu_obj = menu(point(2, 2), getSmallStationMenuSize());
    entertainmentStationMenu = menu(point(2, 2), getSmallStationMenuSize());
}

void Game::setupLoadedGame()
{
    load();
    initMenus();
    display_obj.updateUpperLeft(getPlayerShip()->at(), getMap()->getSize());
    current_player_target = 0;
    if (current_maptype != MAPTYPE_STARMAP)
    {
        pathfinder.setupPathfindDistVector(getMapSize());
    }
}

void Game::initGameObjects()
{
    // Reset discovery & world state
    raceIDNameMapDiscovered.clear();
    universe.setupUniverse();
    universe.setSubAreaMapType(SMT_NONE);

    // UI / meta state
    initMenus();
    turn_timer = 0U;
    current_maptype = MAPTYPE_STARMAP;
    current_tab = TABTYPE_PLAYAREA;
    current_subarea_id = -1;
    current_player_target = 0;

    // Player ship setup
    auto* ps = getPlayerShip();
    auto* map = getMap();

    ps->setShipMob(
        /*isShip*/ true,
        allshipmob_data[static_cast<int>(SHIP_PLAYER)],
        NPCSHIPTYPE_NONE,
        allshipmob_data[static_cast<int>(SHIP_PLAYER)].max_hull,
        point(PLAYER_START_X, PLAYER_START_Y)
    );
    ps->initMobSubAreaID(0);
    ps->setMobSubAreaAttackID(-1);
    ps->setMobSubAreaGroupID(-1);
    ps->setNumCredits(5000ULL);

    // Position/viewport
    display_obj.updateUpperLeft(ps->at(), map->getSize());
    changeMobTile(point(0, 0), ps->at(), ps->getMobType());

    // --- Starter modules ---

    // Crew
    {
        Module m{ MODULE_CREW, 24, 32 };
        ps->addModule(m);
    }

    // Fuel
    {
        Module m{ MODULE_FUEL, 100, 100 };
        ps->addModule(m);
    }

    // Shield
    {
        Module m{ MODULE_SHIELD, allbasicshield_stats[1].base_num_layers, allbasicshield_stats[1].base_num_layers };
        m.setShieldStruct(allbasicshield_stats[1]);
        ps->addModule(m);
    }

    // Engine
    {
        Module m{ MODULE_ENGINE, 10, 10 };
        m.setEngineStruct(allbasicengine_stats[1]);
        ps->addModule(m);
    }

    // Weapon
    {
        Module m{ MODULE_WEAPON, 50, 50 };
        m.setWeaponStruct(allbasicweapon_stats[1]);
        ps->addModule(m);
    }

    /*
    // Bulk add examples kept as-is (commented):
    for (int i = 1; i <= 18; ++i) {
        Module wm{ MODULE_WEAPON, 100, 100 };
        wm.setWeaponStruct(allbasicweapon_stats[i]);
        ps->addModule(wm);
    }

    ps->addModule(Module(MODULE_CREW, 96, 96));
    ps->addModule(Module(MODULE_CREW, 96, 96));
    ps->addModule(Module(MODULE_CREW, 96, 96));
    ps->addModule(Module(MODULE_CREW, 96, 96));
    */

    // Select the weapon by default (index 4 in the above order)
    ps->setModuleSelectionIndex(4);

    // Remember starting star-map location
    last_smloc = ps->at();
}

void Game::save()
{
    std::ofstream os("save.sav", std::ios::binary);
    if (!os) return;

    last_smloc.save(os); // DONE
    os.write(reinterpret_cast<const char*>(&turn_timer), sizeof(uint)); // DONE

    player_ship.save(os); //DONE
    universe.save(os); // DONE
    os.write(reinterpret_cast<const char*>(&current_maptype), sizeof(MapType));
    os.write(reinterpret_cast<const char*>(&current_subarea_id), sizeof(int)); // DONE
    os.write(reinterpret_cast<const char*>(&current_tab), sizeof(tab_type));

    uint32_t discoveredCount = static_cast<uint32_t>(raceIDNameMapDiscovered.size());
    os.write(reinterpret_cast<const char*>(&discoveredCount), sizeof(discoveredCount));
    for (const auto& [id, name] : raceIDNameMapDiscovered)
    {
        os.write(reinterpret_cast<const char*>(&id), sizeof(int));
        stringSave(os, name);
    }
}

void Game::load()
{
    std::ifstream is("save.sav", std::ios::binary);
    if (!is) return;

    last_smloc.load(is);
    is.read(reinterpret_cast<char*>(&turn_timer), sizeof(uint));

    player_ship.load(is);
    universe.load(is);
    is.read(reinterpret_cast<char*>(&current_maptype), sizeof(MapType));
    is.read(reinterpret_cast<char*>(&current_subarea_id), sizeof(int));
    is.read(reinterpret_cast<char*>(&current_tab), sizeof(tab_type));

    uint32_t discoveredCount = 0;
    is.read(reinterpret_cast<char*>(&discoveredCount), sizeof(discoveredCount));
    for (uint32_t i = 0; i < discoveredCount; ++i)
    {
        int id;
        std::string name;

        is.read(reinterpret_cast<char*>(&id), sizeof(int));
        stringLoad(is, name);

        raceIDNameMapDiscovered[id] = std::move(name);
    }
}


bool eightDirectionRestrictedWeaponSelected(MobShip* mb)
{
    return getCurrentMobSelectedModule(mb)->getWeaponStruct().eightDirectionRestricted;
}

bool inRangeStarMapBackdropOrange(backdrop_t starTile)
{
    return (int)starTile >= SMBACKDROP_FRIENDRACE_MAINSEQSTARSUBAREAENTRANCE &&
           (int)starTile <= SMBACKDROP_FRIENDRACE_WHITESTARSUBAREAENTRANCE;
}

bool inRangeStarMapBackdropRed(backdrop_t starTile)
{
    return (int)starTile >= SMBACKDROP_HOSTILERACE_MAINSEQSTARSUBAREAENTRANCE &&
           (int)starTile <= SMBACKDROP_HOSTILERACE_WHITESTARSUBAREAENTRANCE;
}

bool inRangeStarMapBackdropGreen(backdrop_t starTile)
{
    return (int)starTile >= SMBACKDROP_ALLYRACE_MAINSEQSTARSUBAREAENTRANCE &&
           (int)starTile <= SMBACKDROP_ALLYRACE_WHITESTARSUBAREAENTRANCE;
}

bool inRangeStarMapBackdropUnhighlighted(backdrop_t starTile)
{
    return (int)starTile >= SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE &&
           (int)starTile <= SMBACKDROP_WHITESTARSUBAREAENTRANCE;
}

bool isMainSequenceBackdropTile(backdrop_t starTile)
{
    return starTile == SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_FRIENDRACE_MAINSEQSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_HOSTILERACE_MAINSEQSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_ALLYRACE_MAINSEQSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_WARZONE_MAINSEQSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_EMPTY_MAINSEQSTARSUBAREAENTRANCE;
}

bool isBlueStarBackdropTile(backdrop_t starTile)
{
    return starTile == SMBACKDROP_BLUESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_FRIENDRACE_BLUESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_HOSTILERACE_BLUESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_ALLYRACE_BLUESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_WARZONE_BLUESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_EMPTY_BLUESTARSUBAREAENTRANCE;
}


bool isRedStarBackdropTile(backdrop_t starTile)
{
    return starTile == SMBACKDROP_REDSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_FRIENDRACE_REDSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_HOSTILERACE_REDSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_ALLYRACE_REDSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_WARZONE_REDSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_EMPTY_REDSTARSUBAREAENTRANCE;
}


bool isWhiteStarBackdropTile(backdrop_t starTile)
{
    return starTile == SMBACKDROP_WHITESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_FRIENDRACE_WHITESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_HOSTILERACE_WHITESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_ALLYRACE_WHITESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_WARZONE_WHITESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_EMPTY_WHITESTARSUBAREAENTRANCE;
}

void printExitPromptMessage()
{
    msgeAdd("Press ENTER to exit game now...", cp_whiteonblack);
}

void Game::runContactScenario(ContactTree& contactTree)
{
    int scenario_id = contactTree.startingScenarioID;
    int last_built_id = std::numeric_limits<int>::min(); // force initial build
    int preservedSel = 0;

    while (true)
    {
        const ContactScenario& scenario = contactTree.getScenario(scenario_id);

        // Rebuild only when the page changes
        if (last_built_id != scenario_id)
        {
            preservedSel = contact_menu_obj.getSelectionIndex();

            contact_menu_obj.clearMenuItems();
            contact_menu_obj.clearMenuMainText();
            contact_menu_obj.addMenuMainText(scenario.message);

            for (const auto& option : scenario.menuOptions)
                contact_menu_obj.addMenuItem(option, blank_ch);

            const int n = contact_menu_obj.getNumMenuItems();
            if (n > 0) {
                if (preservedSel < 0) preservedSel = 0;
                if (preservedSel >= n) preservedSel = n - 1;
                contact_menu_obj.setSelectionIndex(preservedSel);
            }
            else {
                contact_menu_obj.setSelectionIndex(0);
            }

            last_built_id = scenario_id;
        }

        display_obj.displayMenu(&contact_menu_obj);
        event_handler.setAction();

        if (event_handler.getAction() == INP_DELTA)
        {
            navigateMenu(&contact_menu_obj);
        }
        else if (event_handler.getAction() == INP_SELECT)
        {
            const int sel = contact_menu_obj.getSelectionIndex();

            // Figure out where this option would go
            int next_id = -1;
            if (!scenario.nextScenarioIDs.empty() &&
                sel >= 0 && sel < (int)scenario.nextScenarioIDs.size())
            {
                next_id = scenario.nextScenarioIDs[sel];
            }

            // If this page ends OR has no next (-1), we are NOT navigating.
            // Only in the "no next" case do we run the current page's callback.
            if (scenario.endConversation || next_id == -1)
            {
                if (next_id == -1 && scenario.onSelectCallback)
                    scenario.onSelectCallback(sel);   // e.g., Scenario 11 "enter subarea"

                break;  // conversation ends here
            }

            // We ARE navigating to another page deliver current selection to the NEXT page’s callback.
            // (This lets Scenario 10 consume the choice made on Scenario 9.)
            const ContactScenario& nextScenario = contactTree.getScenario(next_id);
            if (nextScenario.onSelectCallback)
                nextScenario.onSelectCallback(sel);

            // Switch to next page
            scenario_id = next_id;
            contact_menu_obj.setSelectionIndex(0);
            last_built_id = std::numeric_limits<int>::min(); // force rebuild
        }
    }

    contact_menu_obj.clearMenuItems();
    contact_menu_obj.clearMenuMainText();
    display_obj.clearAndDeleteAllMessages();
}

void Game::executeSubareaEntranceContactScenario(race *controlRace, race *nativeRace, bool& enterSubArea)
{
    ContactTree tree = controlRace->getPlayerAttStatus() <= -1 
        ? createFullHostileContactTree(controlRace, nativeRace, enterSubArea)
        : createFullNonHostileContactTree(controlRace, nativeRace, enterSubArea, raceIDNameMapDiscovered);

    runContactScenario(tree);
}

/*
std::string getUserInputPrompt(std::string& promptText, point& loc, int maxLength)
{
    std::string result;
    gfx_obj.addBitmapString(promptText, cp_whiteonblack, loc);
    gfx_obj.updateScreen();

    while (true)
    {
        event_handler.promptAction();
        input_t action = event_handler.getAction();

        if (action == INP_ESCAPE)
            return "";

        if (action == INP_SELECT || action == INP_TOGGLE)  // Enter key
            break;

        if (action == INP_BACKSPACE && !result.empty())
        {
            result.pop_back();
        }

        // Handle number keys (SDL interprets them as their ASCII values)
        SDL_Keycode key = event_handler.getKeyPressed();
        if (key >= SDLK_0 && key <= SDLK_9)
        {
            if ((int)result.size() < maxLength)
                result += static_cast<char>(key);
        }

        // Redraw input line
        gfx_obj.addBitmapString(promptText + result + " ", cp_whiteonblack, loc);
        gfx_obj.updateScreen();
    }

    return result;
}
*/