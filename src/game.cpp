#include "Game.h"

Game::Game()
{
    Game_active = true;
    playerHasMoved = false;
    usingMachine = false;
    wait_counter = 0;
}

void Game::run()
{
    if(gfx_obj.initSDL())
    {
        bool loaded = false;

        point titleTextLocation = point(GRIDWID / 2 - 16, 8);

        display_obj.addString("      COSMIC WAR ADVENTURE       ", cp_darkredonblack,  titleTextLocation);
        display_obj.addString("           v0.1.0-dev            ", cp_darkgrayonblack, titleTextLocation + point(0,2));
        display_obj.addString(" Press ENTER to start a new Game ", cp_grayonblack,     titleTextLocation + point(0,8));
        display_obj.addString("Press R to reload a previous Game", cp_grayonblack,     titleTextLocation + point(0,10));
        display_obj.addString(" Copyright (c) Eric Wolfson 2025 ", cp_whiteonblack,    titleTextLocation + point(0,GRIDHGT-17));
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
        } while (event_handler.getAction() != INP_TOGGLE && event_handler.getAction() != INP_LOAD && event_handler.getAction() != INP_QUIT);

        if (event_handler.getAction() != INP_QUIT)
        {
            primaryGameLoop(loaded);
        }

        do
        {
            event_handler.setAction();
        } while (event_handler.getAction() != INP_TOGGLE);
    }
    else
    {
        std::cout << "failed to initialize... press enter to terminate.";
        std::cin.get();
    }

    cleanupEverything();
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
}

// 1) clear messages, 2) print new message with space bar prompt 3) update screen,
// 4) wait indefinitely until user presses the space bar
void Game::msgeAddPromptSpace(std::string s, color_pair col)
{
    display_obj.clearAndDeleteAllMessages();
    msgeAdd(s,col);
    msgeAdd(" [space]",cp_grayonblack);
    gfx_obj.updateScreen();
    event_handler.waitForKey(' ');
    display_obj.clearAndDeleteAllMessages();
    gfx_obj.updateScreen();
}

// ask for player input via event_handler "input" class object
void Game::promptInput()
{
    if (wait_counter > 0)
    {
        wait_counter--;
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
            if (!CSYS->getNPCShip(new_loc)->isActivated())
            {
                stripShipMobOfResources(getPlayerShip(), CSYS->getNPCShip(new_loc));
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
}

void Game::checkForUniversalRaceEvent()
{
    if (current_maptype != MAPTYPE_STARMAP)
    {
        return;
    }

    checkForUnEnslavementRaceEvent();
    checkForRaceWarEvent();
}

void Game::checkForRaceWarEvent()
{
    int event_roller = 0;
    int race_att_status = 0;
    bool sign_pos;

    for (int i = 0; i < universe.getNumRaces(); ++i)
    for (int j = 0; j < universe.getNumRaces(); ++j)
    {
        if (i != j)
        {
            race_att_status = universe.getRace(i)->getRaceAttStatus(j);
            if (race_att_status != 0)
            {
                sign_pos = (race_att_status >= 1 ? true : false);
                event_roller = (int)std::abs(race_att_status);
                // act of hostile towards race occurs (from race id=i to race id=j)
                if (!sign_pos)
                {
                    if (universe.getRace(i)->getRaceOverallMajorStatus() == RMS_FREE)
                    {
                        if (universe.getRace(j)->getRaceOverallMajorStatus() == RMS_FREE)
                        {
                            if (randInt(0, RACE_EVENT_CHANCE) < event_roller)
                            {
                                raceInvasionEvent(universe.getRace(i), universe.getRace(j));
                            }
                        }
                    }
                }
            }
        }
    }
}

void Game::raceInvasionEvent(race *offender, race *defender)
{
    int num_spawned_ships = 0;
    point maplimit;
    point offender_spawn_loc;
    point defender_loc = defender->getStarmapLoc();
    point offender_loc = offender->getStarmapLoc();
    std::string offender_name = "";

    int subarea_index = universe.getSubAreaIndex(defender_loc);

    if (subarea_index >= 0)
    {
        num_spawned_ships = randInt(1,RACE_EVENT_BATTLE_SHIPS_MAX_SPAWN);

        maplimit = addPoints(universe.getSubArea(subarea_index)->getMap()->getSize(),point(-1,-1));

        if (universe.getSubArea(subarea_index)->getNumShipNPCs() >= 100)
        {
            return;
        }

        for (int i = 0; i < num_spawned_ships; ++i)
        {
            offender_spawn_loc = getRandNPCShipOpenPoint(universe.getSubArea(subarea_index)->getMap(),point(0,0),maplimit,LBACKDROP_SPACE_UNLIT,LBACKDROP_SPACE_LIT);

            universe.getSubArea(subarea_index)->createNPCShip(offender->getNativeShip(0),offender_spawn_loc,offender->getRaceID());
            universe.getSubArea(subarea_index)->getNPCShip(offender_spawn_loc)->setRoveChance(100);
        }

        offender_name = (universe.getSubAreaIndex(offender_loc) <= -1 ? "unidentified" : offender->getNameString());

        reDisplay(false);

        if (getMap()->getv(defender_loc))
        {
            if (num_spawned_ships == 1)
            {
                msgeAddPromptSpace("You detect a lone, hostile " + offender_name + " ship entering the " +
                                    defender->getNameString() + " " + race_domain_suffix_string[(int)defender->getRaceDomainType()] + " at (" +
                                    int2String(defender->getStarmapLoc().x() + 1) + "," + int2String(STARMAPHGT - defender->getStarmapLoc().y()) + ").",cp_whiteonblack);
            }
            else
            {
                msgeAddPromptSpace("You detect a fleet of " + int2String(num_spawned_ships) + " hostile " + offender_name + " ships entering the " +
                                    defender->getNameString() + " " + race_domain_suffix_string[(int)defender->getRaceDomainType()] + " at (" +
                                    int2String(defender->getStarmapLoc().x() + 1) + "," + int2String(STARMAPHGT - defender->getStarmapLoc().y()) + ").",cp_whiteonblack);
            }
        }
    }
}

void Game::checkForSubAreaRaceEvent()
{
    if (current_maptype != MAPTYPE_LOCALEMAP || universe.getSubAreaMapType() != SMT_PERSISTENT)
    {
        return;
    }

    if (!CSYS->isRaceAffiliated())
    {
        return;
    }

    if (CSYS->getNumActiveNativeShipsPresent() == CSYS->getNumShipNPCs() &&
        CSYS->getNumShipNPCs() > 0)
    {
        return;
    }

    if (CSYS->getNumShipNPCs() >= (int)std::min(75,getMapSize().x() * 2))
    {
        return;
    }

    int plnt_race_id = -1;
    int plnt_race_dl = 1;
    int num_plnt_race_native_ships = 0;
    int hw_race_id = CSYS->getNativeRaceID();
    point plnt_loc;

    for (int i = 0; i < universe.getRace(hw_race_id)->getNumHomeworlds(); ++i)
    {
        plnt_loc = universe.getRace(hw_race_id)->getHomeworld(i)->getLoc();
        plnt_race_id = universe.getRace(hw_race_id)->getHomeworld(i)->getControlRaceID();
        plnt_race_dl = universe.getRace(plnt_race_id)->getDangerLevel();
        num_plnt_race_native_ships = universe.getRace(plnt_race_id)->getNumNativeShips();
        if (!mobBlocked(getMap()->getMob(plnt_loc)))
        if (roll(250 - plnt_race_dl*5))
        {
           CSYS->createNPCShip(universe.getRace(plnt_race_id)->getNativeShip(randInt(0,num_plnt_race_native_ships - 1)),
                               plnt_loc,
                               plnt_race_id);
           CSYS->getNPCShip(CSYS->getNumShipNPCs() - 1)->setInitLoc(getRandNPCShipOpenPoint(getMap(),
                                                                                            point(1,1),
                                                                                            addPoints(getMaxMapPoint(getMap()),point(-1,-1)),
                                                                                            LBACKDROP_SPACE_UNLIT,LBACKDROP_WHITESTARBACKGROUND));

           if (getMap()->getv(plnt_loc))
           {
               msgeAdd("One " + CSYS->getNPCShip(CSYS->getNumShipNPCs() - 1)->getShipName() + " emerges from the nearby planet!",cp_whiteonblack);
           }
        }
    }

}

void Game::resetAttackIDs(int id_defeated)
{
    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        if (CSYS->getNPCShip(i)->getMobSubAreaID() != id_defeated)
        {
            if (CSYS->getNPCShip(i)->getMobSubAreaAttackID() == id_defeated)
            {
                CSYS->getNPCShip(i)->setMobSubAreaAttackID(-1);
            }
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

    int item_quant = CSYS->getPile(mb->at())->getItem(0)->getQuant();
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

    switch(getMap()->getBackdrop(getPlayerShip()->at()))
    {
        case(SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE):
        case(SMBACKDROP_REDSTARSUBAREAENTRANCE):
        case(SMBACKDROP_BLUESTARSUBAREAENTRANCE):
        case(SMBACKDROP_WHITESTARSUBAREAENTRANCE):
             msgeAdd("an unknown star system",cp_grayonblack);
             break;
        case(SMBACKDROP_FRIENDRACE_MAINSEQSTARSUBAREAENTRANCE):
        case(SMBACKDROP_FRIENDRACE_REDSTARSUBAREAENTRANCE):
        case(SMBACKDROP_FRIENDRACE_BLUESTARSUBAREAENTRANCE):
        case(SMBACKDROP_FRIENDRACE_WHITESTARSUBAREAENTRANCE):
        case(SMBACKDROP_HOSTILERACE_MAINSEQSTARSUBAREAENTRANCE):
        case(SMBACKDROP_HOSTILERACE_REDSTARSUBAREAENTRANCE):
        case(SMBACKDROP_HOSTILERACE_BLUESTARSUBAREAENTRANCE):
        case(SMBACKDROP_HOSTILERACE_WHITESTARSUBAREAENTRANCE):
             msgeAdd(getRaceRegionNameAtLocation(getPlayerShip()->at()), cp_whiteonblack);
             break;
        case(SMBACKDROP_WARZONE_MAINSEQSTARSUBAREAENTRANCE):
        case(SMBACKDROP_WARZONE_REDSTARSUBAREAENTRANCE):
        case(SMBACKDROP_WARZONE_BLUESTARSUBAREAENTRANCE):
        case(SMBACKDROP_WARZONE_WHITESTARSUBAREAENTRANCE):
             msgeAdd("a war zone", cp_whiteonblack);
             break;
        case(SMBACKDROP_EMPTY_MAINSEQSTARSUBAREAENTRANCE):
        case(SMBACKDROP_EMPTY_REDSTARSUBAREAENTRANCE):
        case(SMBACKDROP_EMPTY_BLUESTARSUBAREAENTRANCE):
        case(SMBACKDROP_EMPTY_WHITESTARSUBAREAENTRANCE):
             msgeAdd("an uninhabited star system", cp_whiteonblack);
             break;
        case(LBACKDROP_SPACESTATION_SHIP):
             msgeAdd("A ship station is here.",cp_grayonblack);
             break;
        case(LBACKDROP_SPACESTATION_ENTERTAINMENT):
             msgeAdd("An entertainment center is here.", cp_grayonblack);
             break;
        case(LBACKDROP_PLANET):
             msgeAdd("There is a homeworld here belonging to The " + CSYS->getSubAreaName() + ".",cp_lightblueonblack);
             break;
        case(LBACKDROP_ENSLAVEDPLANET):
             msgeAdd("There is an enslaved planet here under the authority of The " + universe.getRace(universe.getRace(CSYS->getNativeRaceID())->getHomeworld(getPlayerShip()->at())->getControlRaceID())->getNameString() + ".",cp_purpleonblack);
             break;
        default:
             break;
    }
}

void Game::checkForUnEnslavementRaceEvent()
{
    point hw_loc;

    for (int i = 0; i < universe.getNumRaces(); ++i)
    {
        if (universe.getRace(i)->getRaceOverallMajorStatus() == RMS_ENSLAVED)
        {
            for (int j = 0; j < universe.getNumRaces(); ++j)
            {
                if (i != j)
                {
                    for (int pl = 0; pl < universe.getRace(j)->getNumHomeworlds(); ++pl)
                    {
                        if (universe.getRace(j)->getHomeworld(pl)->getControlRaceID() == universe.getRace(i)->getRaceID())
                        {
                            hw_loc = universe.getRace(j)->getHomeworld(pl)->getLoc();
                            universe.getRace(j)->setHomeworldMajorStatus(hw_loc,RMS_FREE);
                            universe.getRace(j)->setHomeworldControllerRace(hw_loc,universe.getRace(j)->getRaceID(),universe.getRace(j)->getDangerLevel());
                            universe.getSubArea(universe.getSubAreaIndex(universe.getRace(j)->getStarmapLoc()))->getMap()->setBackdrop(hw_loc,LBACKDROP_PLANET);
                            universe.getRace(j)->setControllerRaceID(getDominantRaceIDInRegion(universe.getSubArea(universe.getSubAreaIndex(universe.getRace(j)->getStarmapLoc()))));
                        }
                    }
                }
            }
        }
    }
}

void Game::executeMiscNPCTurnBasedData()
{
    if (current_maptype != MAPTYPE_LOCALEMAP)
        return;

    checkNPCDefeatedEvent();
    checkPlayerDefeatedEvent();

    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        checkMobRegenerateEvent(CSYS->getNPCShip(i),MODULE_WEAPON);
        checkMobRegenerateEvent(CSYS->getNPCShip(i),MODULE_SHIELD);
    }

    if ((int)explosion_data.size() > 0)
    {
        createShipDestructionAnimations();
    }
}

void Game::checkMobRegenerateEvent(MobShip *s, module_type mt)
{
    int regen_offst;

    int regen_delay;

    bool regen_check;

    for (int i = 0; i < s->getNumInstalledModules(); ++i)
    {
        if (s->getModule(i)->getModuleType() == mt)
        {
            if (s->getModule(i)->getFillQuantity() < s->getModule(i)->getMaxFillQuantity())
            {
                if (mt == MODULE_WEAPON)
                    regen_delay = s->getModule(i)->getWeaponStruct().regen_rate;
                if (mt == MODULE_SHIELD)
                    regen_delay = s->getModule(i)->getShieldStruct().regen_rate;
                if (regen_delay > 0)
                {
                    regen_offst = current_maptype == MAPTYPE_STARMAP ? (int)std::max((int)(LOCALEREGIONDEFAULTWID/regen_delay),1) : 1;
                    regen_check = current_maptype == MAPTYPE_STARMAP ? true : turn_timer % regen_delay == 0;
                    if (regen_check)
                    {
                        s->getModule(i)->offFillQuantity(regen_offst);
                    }
                }
            }
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
    point p = getPlayerShip()->at();

    if (getMap()->getBackdrop(p) == LBACKDROP_SPACESTATION_SHIP)
    {
       int dominantRace = universe.getRace(CSYS->getNativeRaceID())->getControllerRaceID();

       if (universe.getRace(dominantRace)->getPlayerAttStatus() >= 0)
       {
           useSpaceStation(p);
       }
       else
       {
           msgeAdd("You are blocked from docking!", cp_grayonblack);
       }
    }
    else if (getMap()->getBackdrop(p) == LBACKDROP_SPACESTATION_ENTERTAINMENT)
    {
        int dominantRace = universe.getRace(CSYS->getNativeRaceID())->getControllerRaceID();

        if (universe.getRace(dominantRace)->getPlayerAttStatus() >= 0)
        {
            useEntertainmentCenter(p);
        }
        else
        {
            msgeAdd("You are blocked from docking!", cp_grayonblack);
        }
    }
    else
    {
       msgeAdd("You can't dock your ship here!",cp_grayonblack);
    }
}

void Game::setRegionTileForRaceHome(race *dominantRace)
{
    backdrop_t starMapTile = universe.getMap()->getBackdrop(last_smloc);

    if (dominantRace->getPlayerAttStatus() < 0)
    {
        if (inRangeStarMapBackdropGreen(starMapTile))
        {
            setVisitedStarMapTileBackdrop(last_smloc, NUM_STAR_TYPES);
        }
        if (inRangeStarMapBackdropUnhighlighted(starMapTile))
        {
            setVisitedStarMapTileBackdrop(last_smloc, NUM_STAR_TYPES*2);
        }
    }
    else
    {
        if (inRangeStarMapBackdropRed(starMapTile))
        {
            setVisitedStarMapTileBackdrop(last_smloc, -NUM_STAR_TYPES);
        }
        if (inRangeStarMapBackdropUnhighlighted(starMapTile))
        {
            setVisitedStarMapTileBackdrop(last_smloc, NUM_STAR_TYPES);
        }
    }
}

void Game::checkSetRegionTileForRaceHome()
{
    if (CSYS->getSubAreaSpecificType() == SST_RACEHOME)
    {
        int dominantRaceID = getDominantRaceIDInRegion(CSYS);

        setRegionTileForRaceHome(universe.getRace(dominantRaceID));
    }
}

void Game::setVisitedStarMapTileBackdrop(point smloc, int increment)
{
    universe.getMap()->setBackdrop(smloc, (backdrop_t)((int)universe.getMap()->getBackdrop(smloc) + increment));
}

void Game::setVisitedStarMapTileBackdrops(point smloc)
{
    switch (CSYS->getSubAreaSpecificType())
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
            setVisitedStarMapTileBackdrop(smloc, NUM_STAR_TYPES*3);
            break;
        }
        case(SST_EMPTYSYSTEM):
        {
            setVisitedStarMapTileBackdrop(smloc, NUM_STAR_TYPES*4);
        }
        default:
            break;
    }
}

void Game::enterSubArea(bool is_encounter)
{
    int subarea_index;
    bool race_affiliated = false;
    star_type st = STARTYPE_NONE;
    point smloc = getPlayerShip()->at();
    if (isStarmapStarTile(getMap()->getBackdrop(smloc)))
    {
        if (universe.getRaceIndex(smloc) >= 0)
            race_affiliated = true;

        last_smloc = smloc;

        if (race_affiliated)
        {
            if (!converseViaContactMenu(universe.getRace(universe.getRaceIndex(smloc))))
            {
                setRegionTileForRaceHome(universe.getRace(universe.getRace(universe.getRaceIndex(smloc))->getControllerRaceID()));
                return;
            }

            universe.getRace(universe.getRaceIndex(smloc))->setRaceIDByPlayerStatus(true);
        }

        setMobTileToNIL(getMap(), last_smloc);
        subarea_index = universe.getSubAreaIndex(smloc);
        st = getStarTypeFromStarMapTile(smloc);

        if (subarea_index >= 0)
        {
            universe.setSubAreaMapType(SMT_PERSISTENT);
            current_subarea_id = subarea_index;
        }
        else
        {
            universe.createSubArea(smloc,SMT_PERSISTENT,(race_affiliated ? SST_RACEHOME : SST_EMPTYSYSTEM),false,race_affiliated,st);
            current_subarea_id = universe.getNumSubAreas() - 1;
            setVisitedStarMapTileBackdrops(smloc);
        }

        current_maptype = MAPTYPE_LOCALEMAP;
        pathfinder.setupPathfindDistVector(getMapSize());
        setPlayerLoc(getSubAreaSystemStartPoint(getMapSize()));
        changeMobTile(point(0,0),getPlayerShip()->at(),getPlayerShip()->getMobType());
    }
    else
    {
        if (is_encounter)
            msgeAddPromptSpace("An encounter!",cp_whiteonblack);
        last_smloc = getPlayerShip()->at();
        setMobTileToNIL(getMap(), last_smloc);
        subarea_index = -1;
        universe.createSubArea(getPlayerShip()->at(),SMT_NONPERSISTENT,(is_encounter ? SST_PIRATEVOID : SST_EMPTYVOID),is_encounter,false,STARTYPE_NONE);
        current_maptype = MAPTYPE_LOCALEMAP;
        pathfinder.setupPathfindDistVector(getMapSize());
        setPlayerLoc(point(getMapSize().x()/2,getMapSize().y()/2));
        changeMobTile(point(0,0),getPlayerShip()->at(),getPlayerShip()->getMobType());
        if (is_encounter)
        {
            playerHasMoved = false;
            addEncounterShips();
        }
    }

    printSubAreaEntranceMessage();
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

void Game::printSubAreaEntranceMessage()
{
    switch(CSYS->getSubAreaSpecificType())
    {
        case(SST_EMPTYVOID):
             msgeAdd("You enter the void.",cp_whiteonblack);
             break;
        case(SST_PIRATEVOID):
             msgeAdd("Your ship is being targeted by space pirates!",cp_whiteonblack);
             break;
        case(SST_EMPTYSYSTEM):
             msgeAdd("This is a nondescript star system.",cp_whiteonblack);
             break;
        case(SST_RACEHOME):
             msgeAdd("You enter an occupied star system.",cp_whiteonblack);
             if (universe.getRace(CSYS->getNativeRaceID())->getPlayerAttStatus() <= -1)
                 msgeAdd("Hostile ships suddenly pursue you!",cp_redonblack);
             break;
        case(SST_WARZONE):
             msgeAdd("You have entered a war zone!",cp_whiteonblack);
             break;
        default:
             break;
    }
}

// space pirates for now
// assumes player is in sub area!!!
// MapType must represent subarea!
void Game::addEncounterShips()
{
    int race_id = -1;

    int iters = 0;

    int danger_level = 1;

    shipmob_classtype c_type;

    do
    {
        iters++;
        race_id = randInt(0,universe.getNumRaces()-1);
        danger_level = universe.getRace(race_id)->getDangerLevel();
        c_type = universe.getRace(race_id)->getNativeShip(0)->getStatStruct().sctype;
    }  while (iters < 50 && (danger_level > 12 || c_type != CLASSTYPE_FIGHTER));

    int num_ships = randInt(1,randInt(1,4));

    point spawn_loc;

    for (int i = 0; i < num_ships; ++i)
    {
        spawn_loc = getRandNPCShipOpenPoint(getMap(),point(1,1),addPoints(getMapSize(),point(-2,-2)),LBACKDROP_SPACE_UNLIT,LBACKDROP_SPACE_LIT);
        CSYS->createNPCShip(universe.getRace(race_id)->getNativeShip(0),spawn_loc,race_id);
        CSYS->getNPCShip(i)->setShipName(universe.getRace(race_id)->getNameString() + " pirate");
        CSYS->getNPCShip(i)->setMobSubAreaAttackID(0);
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
    bool exit_menu = false;
    msgeAddPromptSpace("Your ship docks at the space station.",cp_blackonwhite);
    int index = 0;
    setStationContactData(universe.getRace(CSYS->getNativeRaceID()),
                          CSYS->getStation(p),&station_menu_obj,getPlayerShip(),0);
    station_menu_obj.setSelectionIndex(0);
    while(!exit_menu)
    {
        // set player input for menu
        display_obj.displayMenu(&station_menu_obj);
        event_handler.setAction();
        switch(event_handler.getAction())
        {
            case(INP_DELTA):
            {
                navigateMenu(&station_menu_obj);
                break;
            }
            case(INP_SELECT):
            {
                index = station_menu_obj.getSelectionIndex();
                if (station_menu_obj.getMenuLevel() == 0)
                {
                    if (index >= CSYS->getStation(p)->getNumBasicStationTradeChoices())
                    {
                        exit_menu = true;
                    }
                    else
                    {
                        switch(CSYS->getStation(p)->getBasicStationTradeChoice(index))
                        {
                            case(STATIONCHOICE_BUYMODULE):
                                setStationContactData(universe.getRace(CSYS->getNativeRaceID()),
                                                      CSYS->getStation(p),&station_menu_obj,getPlayerShip(),1);
                                station_menu_obj.setSelectionIndex(0);
                                break;
                            case(STATIONCHOICE_SELLMODULE):
                                setStationContactData(universe.getRace(CSYS->getNativeRaceID()),
                                                      CSYS->getStation(p),&station_menu_obj,getPlayerShip(),2);
                                station_menu_obj.setSelectionIndex(0);
                                break;
                            case(STATIONCHOICE_HULLREPAIR):
                                repairPlayerHull(CSYS->getStation(p));
                                break;
                            case(STATIONCHOICE_HULLUPGRADE):
                                upgradePlayerHull(CSYS->getStation(p));
                                break;
                            case(STATIONCHOICE_INCNUMMODULECAPACITY):
                                upgradePlayerSlotCapacity(CSYS->getStation(p));
                                break;
                            case(STATIONCHOICE_FUEL):
                                playerBuyFuel(CSYS->getStation(p));
                                break;
                            case(STATIONCHOICE_HIRECREW):
                                playerHireCrew(CSYS->getStation(p));
                                break;
                            default:
                                break;
                        }
                    }
                }
                else if (station_menu_obj.getMenuLevel() == 1)
                {
                    if (station_menu_obj.getSelectionIndex() == CSYS->getStation(p)->getNumModulesForTrade())
                    {
                        setStationContactData(universe.getRace(CSYS->getNativeRaceID()),CSYS->getStation(p),
                                              &station_menu_obj,getPlayerShip(),0);
                        station_menu_obj.setSelectionIndex(0);
                    }
                    else if (station_menu_obj.getSelectionIndex() == CSYS->getStation(p)->getNumModulesForTrade() + 1)
                    {
                        exit_menu = true;
                    }
                    else
                    {
                        playerPurchaseModule(CSYS->getStation(p), &station_menu_obj);
                    }
                }
                else
                {
                    if (station_menu_obj.getSelectionIndex() == getPlayerShip()->getNumInstalledModules())
                    {
                        setStationContactData(universe.getRace(CSYS->getNativeRaceID()),CSYS->getStation(p),
                                              &station_menu_obj,getPlayerShip(),0);
                        station_menu_obj.setSelectionIndex(0);
                    }
                    else if (station_menu_obj.getSelectionIndex() == getPlayerShip()->getNumInstalledModules() + 1)
                    {
                        exit_menu = true;
                    }
                    else
                    {
                        playerSellModule(CSYS->getStation(p), &station_menu_obj);
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    msgeAdd("Your ship leaves the station.",cp_grayonblack);
    station_menu_obj.clearMenuItems();
    station_menu_obj.clearMenuMainText();
}

void Game::useEntertainmentCenter(point p)
{
    bool exit_menu = false;
    msgeAddPromptSpace("Your ship docks at the space station.", cp_blackonwhite);
    int index = 0;
    int numSlotMachines = CSYS->getEntertainmentCenter(p)->getNumSlotMachines();
    int numDiamondMachines = CSYS->getEntertainmentCenter(p)->getNumDiamondMachines();
    setEntertainmentCenterContactData(universe.getRace(CSYS->getNativeRaceID()), CSYS->getEntertainmentCenter(p), &entertainmentStationMenu, 0);
    entertainmentStationMenu.setSelectionIndex(0);
    EntertainmentStation* entertainmentStation = CSYS->getEntertainmentCenter(p);

    while (!exit_menu)
    {
        // set player input for menu
        display_obj.displayMenu(&entertainmentStationMenu);
        event_handler.setAction();
        switch (event_handler.getAction())
        {
            case(INP_DELTA):
            {
                navigateMenu(&entertainmentStationMenu);
                break;
            }
            case(INP_SELECT):
            {
                index = entertainmentStationMenu.getSelectionIndex();
                if (index >= entertainmentStation->getNumOptions())
                {
                    exit_menu = true;
                }
                else if (index < entertainmentStation->getNumOptions() && index >= numSlotMachines)
                {
                    useMachinePlayer(entertainmentStation->getDiamondsMachine(index - numSlotMachines));
                    reDisplayWithoutUpdate(false);
                }
                else if (index < numSlotMachines)
                {
                    useMachinePlayer(entertainmentStation->getSlotMachine(index));
                    reDisplayWithoutUpdate(false);
                }
            }
            default:
                break;
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
        flushKeyPresses();
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

void Game::upgradePlayerHull(station *ss_obj)
{
    display_obj.clearAndDeleteAllMessages();

    if (ss_obj->getHullUpgradeCost() > getPlayerShip()->getNumCredits())
        msgeAdd("You can't afford to upgrade your hull here!",cp_grayonblack);
    else if (getPlayerShip()->getHullStatus() < getPlayerShip()->getMaxHull())
        msgeAdd("You must completely repair your hull before you can upgrade it.",cp_grayonblack);
    else
    {
        getPlayerShip()->setMaxHull(getPlayerShip()->getMaxHull() + 1);
        getPlayerShip()->setHullStatus(getPlayerShip()->getMaxHull());
        getPlayerShip()->setNumCredits(getPlayerShip()->getNumCredits() - ss_obj->getHullUpgradeCost());
        display_obj.printShipStatsSection(getPlayerShip());
        msgeAdd("Your hull has been upgraded!",cp_whiteonblack);
    }
}

void Game::repairPlayerHull(station *ss_obj)
{
    display_obj.clearAndDeleteAllMessages();

    if (ss_obj->getHullFixCost() > getPlayerShip()->getNumCredits())
        msgeAdd("You can't afford to repair your hull here!",cp_grayonblack);
    else if (getPlayerShip()->getHullStatus() >= getPlayerShip()->getMaxHull())
        msgeAdd("Your ship's hull is in perfect condition already.",cp_grayonblack);
    else
    {
        getPlayerShip()->setHullStatus(getPlayerShip()->getHullStatus() + 1);
        getPlayerShip()->setNumCredits(getPlayerShip()->getNumCredits() - ss_obj->getHullFixCost());
        display_obj.printShipStatsSection(getPlayerShip());
        msgeAdd("The crew at the station work on repairing your ship.",cp_whiteonblack);
    }
}

void Game::playerBuyFuel(station *ss_obj)
{
    display_obj.clearAndDeleteAllMessages();

    if (ss_obj->getFuelCost() > getPlayerShip()->getNumCredits())
        msgeAdd("You can't afford to buy fuel here!",cp_grayonblack);
    else if (getPlayerShip()->getTotalMTFillCapacity(MODULE_FUEL) == getPlayerShip()->getTotalMTFillRemaining(MODULE_FUEL))
        msgeAdd("Your ship's fuel tank capacity has already been reached!",cp_grayonblack);
    else
    {
        getPlayerShip()->incTotalFillAmount(1,MODULE_FUEL);
        getPlayerShip()->setNumCredits(getPlayerShip()->getNumCredits() - ss_obj->getFuelCost());
        display_obj.printShipStatsSection(getPlayerShip());
        display_obj.printShipGraphic(getPlayerShip(), SHOWHGT/2);
        msgeAdd("You purchase 1 unit of fuel. A low-pitched \"*GLUGG*\" sound echoes throughout your ship.",cp_whiteonblack);
    }
}

void Game::upgradePlayerSlotCapacity(station *ss_obj)
{
    display_obj.clearAndDeleteAllMessages();

    if (ss_obj->getSlotCapUpgradeCost() > getPlayerShip()->getNumCredits())
        msgeAdd("You can't afford to increase your module slot capacity here!",cp_grayonblack);
    else if (getPlayerShip()->getMaxNumModules() >= MAX_PLAYER_SLOTS)
        msgeAdd("Your ship's structure does not support having more than " + int2String(MAX_PLAYER_SLOTS) + " slots.",cp_grayonblack);
    else
    {
        getPlayerShip()->setNumMaxModules(getPlayerShip()->getMaxNumModules() + 1);
        getPlayerShip()->setNumCredits(getPlayerShip()->getNumCredits() - ss_obj->getSlotCapUpgradeCost());
        display_obj.printShipStatsSection(getPlayerShip());
        display_obj.printShipGraphic(getPlayerShip(), SHOWHGT/2);
        msgeAdd("Your ships module slot capacity has been upgraded!",cp_whiteonblack);
    }
}

void Game::playerHireCrew(station * ss_obj)
{
    display_obj.clearAndDeleteAllMessages();

    if (ss_obj->getCrewCost() > getPlayerShip()->getNumCredits())
        msgeAdd("You can't afford to hire any crew here!",cp_grayonblack);
    else if (getPlayerShip()->getTotalMTFillCapacity(MODULE_CREW) == getPlayerShip()->getTotalMTFillRemaining(MODULE_CREW))
        msgeAdd("Your ship is currently at the maximum crew capacity.",cp_grayonblack);
    else
    {
        getPlayerShip()->incTotalFillAmount(1,MODULE_CREW);
        getPlayerShip()->setNumCredits(getPlayerShip()->getNumCredits() - ss_obj->getCrewCost());
        display_obj.printShipStatsSection(getPlayerShip());
        display_obj.printShipGraphic(getPlayerShip(), SHOWHGT/2);
        msgeAdd("A very bizarre looking alien joins your ship's crew.",cp_whiteonblack);
    }
}

void Game::playerPurchaseModule(station *ss_obj, menu *ss_mu)
{
    int selection = ss_mu->getSelectionIndex();

    // make sure not out of bounds
    if (selection >= ss_obj->getNumModulesForTrade())
        return;

    uint_64 cost = ss_obj->getModuleForTradeCost(selection);

    int module_slots_available = getPlayerShip()->getMaxNumModules() - getPlayerShip()->getNumInstalledModules();

    display_obj.clearAndDeleteAllMessages();

    if (cost > getPlayerShip()->getNumCredits())
        msgeAdd("You do not have enough credits for that module!",cp_whiteonblack);
    else
    {
        if (module_slots_available >= 1)
        {
            getPlayerShip()->addModule(*(ss_obj->getModuleForTrade(selection)));
            getPlayerShip()->setNumCredits(getPlayerShip()->getNumCredits() - cost);
            ss_obj->eraseModule(selection);
            display_obj.printShipStatsSection(getPlayerShip());
            display_obj.printShipGraphic(getPlayerShip(), SHOWHGT/2);
            ss_mu->eraseMenuItem(selection);
            msgeAdd("Your new module has been installed!",cp_purpleonblack);
        }
        else
        {
            msgeAdd("Your ship already has the maximum number of modules it can hold!",cp_whiteonblack);
        }
    }
}

void Game::playerSellModule(station *ss_obj, menu *ss_mu)
{
    int selection = ss_mu->getSelectionIndex();

    // make sure not out of bounds
    if (selection >= getPlayerShip()->getNumInstalledModules())
        return;

    uint_64 value = ss_obj->getModuleSellCost(getPlayerShip()->getModule(selection));

    display_obj.clearAndDeleteAllMessages();

    if (getPlayerShip()->getNumInstalledModulesOfType(MODULE_ENGINE) == 1 &&
        getPlayerShip()->getModule(selection)->getModuleType() == MODULE_ENGINE)
    {
        msgeAdd("You cannot sell your ship's sole engine!",cp_grayonblack);
        return;
    }

    if (getPlayerShip()->getModule(selection)->getModuleType() == MODULE_FUEL &&
        getPlayerShip()->getModule(selection)->getFillQuantity() > 0)
    {
        msgeAdd("You cannot sell a non-empty fuel tank!",cp_grayonblack);
        return;
    }

    if (getPlayerShip()->getModule(selection)->getModuleType() == MODULE_CREW &&
        getPlayerShip()->getModule(selection)->getFillQuantity() > 0)
    {
        msgeAdd("You cannot sell a non-empty crew pod!",cp_grayonblack);
        return;
    }

    getPlayerShip()->setNumCredits(getPlayerShip()->getNumCredits() + value);
    getPlayerShip()->eraseModule(selection);
    getPlayerShip()->setModuleSelectionIndex(0);
    ss_mu->eraseMenuItem(selection);
    display_obj.printShipStatsSection(getPlayerShip());
    display_obj.printShipGraphic(getPlayerShip(), SHOWHGT/2);
    msgeAdd("You sell the module; it is recycled into raw materials.",cp_whiteonblack);
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
        msgeAddPromptSpace("You encounter an unidentified alien region!", cp_blackonwhite);
    }
    else
    {
        msgeAdd("(e)nter, (c)onverse, (i)gnore?",cp_blackonwhite);
        gfx_obj.updateScreen();
        SDL_Keycode enter_action = event_handler.getKeyPressed();
        display_obj.clearAndDeleteAllMessages();
        switch(enter_action)
        {
            case(SDLK_e):
                if (controllerRace->getPlayerAttStatus() >= 0)
                    return true;
                break;
            case(SDLK_c):
                break;
            default:
                return false;
        }
    }

    bool enterSubArea = false;
    msgeAddPromptSpace("You make contact.", cp_whiteonblack);
    executeSubareaEntranceContactScenario(controllerRace, nativeRace, enterSubArea);
    nativeRace->setPlayerIDByRaceStatus(true);
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

    if (inRange(fireLocation, point(0, 0), point(STARMAPWID - 1, STARMAPHGT - 1)))
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
    if (type == INP_EXAMINE)
    {
        if (success)
        {
            msgeAdd("Look where?", cp_whiteonblack);
        }
        else
        {
            msgeAdd("You can't examine anything here!", cp_grayonblack);
        }
    }
    else
    {
        if (success)
        {
            msgeAdd("Fire where?", cp_whiteonblack);
        }
        else
        {
            msgeAdd("You can't fire at anything here!", cp_grayonblack);
        }
    }
}

bool Game::checkCanTargetBasedOnModule(input_t type)
{
    return type != INP_WEAPONFIRE || getCurrentMobSelectedModule(getPlayerShip())->getModuleType() == MODULE_WEAPON;
}

bool Game::checkCanTargetBasedOnStraightLine(MobShip * mb, input_t type) 
{
    return type != INP_WEAPONFIRE || isInStraightLine(mb->at(), getPlayerShip()->at()) || !eightDirectionRestrictedWeaponSelected(getPlayerShip());
}

bool Game::sufficientFillQuantity(MobShip * mb)
{
    return getCurrentMobSelectedModule(mb)->getFillQuantity() >= getWeaponModuleConsumptionPerTurn(getCurrentMobSelectedModule(mb));
}

void Game::cycleTarget()
{
    current_player_target = (current_player_target > 0 ? current_player_target - 1 : CSYS->getNumShipNPCs() - 1);
}

point Game::getNextToggleDeltaForFireWeapon(point at, point delta)
{
    point nextPoint = addPoints(at, delta);

    if (!eightDirectionRestrictedWeaponSelected(getPlayerShip()))
    {
        return nextPoint;
    }
    else
    {
        point playerAt = getPlayerShip()->at();

        if (!isInStraightLine(playerAt, nextPoint))
        {
            point playerTargetDelta = pointDistance(playerAt, at);

            int maxCoord = std::max(abs(playerTargetDelta.x()), abs(playerTargetDelta.y()));

            point distancePoint = multiplyPoints(point(maxCoord, maxCoord), delta);

            nextPoint = addPoints(distancePoint, playerAt);
        }
    }

    if (inRange(nextPoint, point(0, 0), point(CSYS->getMap()->getSize().x() - 1, CSYS->getMap()->getSize().y() - 1)))
    {
        return nextPoint;
    }

    return at;
}

// only works in star systems for now...
void Game::playerTargetToggle(input_t type)
{
    if (current_tab != TABTYPE_PLAYAREA)
    {
        return;
    }

    if (current_maptype != MAPTYPE_LOCALEMAP)
    {
        printTogglePromptMessage(type, false);
        return;
    }

    point at = getPlayerShip()->at();

    point temp_loc;

    at = getNextTargetedNPC(type);

    printTogglePromptMessage(type, true);

    if (!checkCanTargetBasedOnModule(type))
    {
        msgeAdd("Your selected module is not a weapon!", cp_grayonblack);
        return;
    }

    if (type == INP_WEAPONFIRE && !sufficientFillQuantity(getPlayerShip()))
    {
        msgeAdd("You do not have enough power or projectiles to fire that weapon now!", cp_grayonblack);
        cycleTarget();
        return;
    }

    do
    {
        printPlayerFirePath(getPlayerShip()->at(),at);

        if ((int)getMap()->getMob(at) > (int)SHIP_PLAYER)
            display_obj.displayMonitor(MONITOR_TARGETINFO,CSYS->getNPCShip(at));

        gfx_obj.updateScreen();

        event_handler.setAction();

        if (event_handler.getAction() == INP_DELTA)
        {
            if (type == INP_WEAPONFIRE)
            {
                temp_loc = getNextToggleDeltaForFireWeapon(at, event_handler.getDelta());
            }
            else
            {
                temp_loc = addPoints(at, event_handler.getDelta());
            }
            setPointIfInMapRangeAndLOS(temp_loc, at);
        }

        if (isAt(temp_loc, at))
        {
            clearAllFireCells(getMap());
        }

        if (event_handler.getAction() == INP_TOGGLE)
        {
            clearAllFireCells(getMap());
            cycleTarget();
            return;
        }

        if (event_handler.getAction() == INP_SELECT)
        {
            clearAllFireCells(getMap());
            at = getNextTargetedNPC(type);
        }

    } while (event_handler.getAction() != INP_WEAPONFIRE);

    playerHasMoved = true;

    clearAllFireCells(getMap());

    gfx_obj.updateScreen();

    if (type == INP_WEAPONFIRE)
    {
        if (!isAt(at, getPlayerShip()->at()))
        {
            mobFire(getPlayerShip(), at);
        }
        else
        {
            msgeAdd("Cannot target your own ship...", cp_grayonblack);
        }
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

    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        if (isTargetableNPC(i, inp))
            iter++;
    }

    if (iter == 0)
        return loc;

    current_player_target = (int)std::max(current_player_target,0);
    current_player_target = (int)std::min(CSYS->getNumShipNPCs()-1,current_player_target);

    do
    {
        if (isTargetableNPC(current_player_target, inp))
        {
            loc = CSYS->getNPCShip(current_player_target)->at();
            found_target = true;
        }

        current_player_target = (current_player_target + 1) % CSYS->getNumShipNPCs();
    }
    while (!found_target);

    return loc;
}

bool Game::isTargetableNPC(int n, input_t inp)
{
    if (getMap()->getv(CSYS->getNPCShip(n)->at()))
    {
        if (inp == INP_EXAMINE)
        {
            return true;
        }
    }
    else
    {
        return false;
    }

    if (shortestPath(getPlayerShip()->at(), CSYS->getNPCShip(n)->at()) > getCurrentMobSelectedModule(getPlayerShip())->getWeaponStruct().travel_range)
    {
        return false;
    }
    
    if (!checkCanTargetBasedOnStraightLine(CSYS->getNPCShip(n), inp))
    {
        return false;
    }

    return true;
}

void Game::checkNPCDefeatedEvent()
{
    point p;

    int id;

    if (current_maptype != MAPTYPE_LOCALEMAP)
        return;

    uint_64 num_credits = 0ULL;

    // check to see which ships should be destroyed
    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        if (CSYS->getNPCShip(i)->getHullStatus() <= 0)
        {
            num_credits = CSYS->getNPCShip(i)->getNumCredits();
            id = CSYS->getNPCShip(i)->getMobSubAreaID();
            p = CSYS->getNPCShip(i)->at();
            setMobTileToNIL(getMap(), p);
            if (num_credits > 0ULL)
                CSYS->addItem(standard_item_stats[0],randInt(1,randInt(1,(int)num_credits)),p);
            explosion_data.push_back({p,CSYS->getNPCShip(i)->getStatStruct().destruction_radius,0});
            if (getMap()->getv(p))
                msgeAdd(CSYS->getNPCShip(i)->getShipName() + " destroyed!",CSYS->getNPCShip(i)->getShipSymbol().color);
            CSYS->destroyNPC(i);
            resetAttackIDs(id);
            i = 0;
        }
    }

    // check to see what ships have lost all crew
    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        if (CSYS->getNPCShip(i)->crewOperable())
        {
            if (CSYS->getNPCShip(i)->isActivated())
            {
                if (CSYS->getNPCShip(i)->getTotalMTFillRemaining(MODULE_CREW) <= 0)
                {
                    id = CSYS->getNPCShip(i)->getMobSubAreaID();
                    if (getMap()->getv(p))
                        msgeAdd("All crew aboard the " + CSYS->getNPCShip(i)->getShipName() + " have perished!",CSYS->getNPCShip(i)->getShipSymbol().color);
                    CSYS->getNPCShip(i)->setActivationStatus(false);
                    resetAttackIDs(id);
                }
            }
        }
    }
}

void Game::checkPlayerDefeatedEvent()
{
    point p;
    if (getPlayerShip()->getHullStatus() <= 0)
    {
        p = getPlayerShip()->at();
        msgeAdd("Your ship has been destroyed!",cp_darkredonblack);
        printExitPromptMessage();
        display_obj.printShipStatsSection(getPlayerShip());
        setMobTileToNIL(getMap(), p);
        explosion_data.push_back({p,getPlayerShip()->getStatStruct().destruction_radius,0});
        resetAttackIDs(0);
        Game_active = false;
        return;
    }

    if (getPlayerShip()->getTotalMTFillRemaining(MODULE_CREW) <= 0)
    {
        msgeAdd("All crew members aboard your ship have vanquished!",cp_darkredonblack);
        printExitPromptMessage();
        display_obj.printShipStatsSection(getPlayerShip());
        resetAttackIDs(0);
        Game_active = false;
        return;
    }
}

void Game::createShipDestructionAnimations()
{
    fire_t ft;

    int num_explosions = (int)explosion_data.size();

    int m_radius = 1;

    int num_visible = 0;

    for (int i = 0; i < num_explosions; ++i)
         m_radius = (int)std::max(m_radius,explosion_data[i].radius);

    for (int c = 0; c < m_radius*2; ++c)
    {
        num_visible = 0;

        for (int i = 0; i < num_explosions; ++i)
        {
            if (c < explosion_data[i].radius*2 && getMap()->getv(explosion_data[i].loc))
            {
                ft = c < explosion_data[i].radius ? FIRET_EXPLOSION : NIL_f;
                explosion_data[i].iteration = (c != explosion_data[i].radius ? explosion_data[i].iteration + 1 : 1);
                printFireCircle(explosion_data[i].loc,explosion_data[i].iteration,ft);
                num_visible++;
            }
        }

        if (num_visible > 0)
            display_obj.delayAndUpdate(30);
        else
            break;
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

    item_stats istats = *(CSYS->getPile(mb->at())->getItem(s)->getStats());

    if (istats.i_type == ITEM_WRECKAGECREDIT)
        mb->setNumCredits(mb->getNumCredits() + (uint_64)q);

    CSYS->deleteItemFromPile(mb->at(),s,q);
}

void Game::initMenus()
{
    contact_menu_obj = menu(point(2, GRIDHGT / 4), point(GRIDWID - 4, GRIDHGT / 2));
    station_menu_obj = menu(point(2, 2), point(GRIDWID - 4, SHOWHGT / 2 - 2));
    entertainmentStationMenu = menu(point(2, 2), point(GRIDWID - 4, SHOWHGT / 2 - 2));
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
    universe.setupUniverse();
    universe.setSubAreaMapType(SMT_NONE);
    initMenus();
    turn_timer = 0U;
    current_maptype = MAPTYPE_STARMAP;
    current_tab = TABTYPE_PLAYAREA;
    current_subarea_id = -1;
    current_player_target = 0;
    getPlayerShip()->setShipMob(true, allshipmob_data[(int)SHIP_PLAYER],
        NPCSHIPTYPE_NONE, allshipmob_data[(int)SHIP_PLAYER].max_hull, point(PLAYER_START_X, PLAYER_START_Y));
    getPlayerShip()->initMobSubAreaID(0);
    getPlayerShip()->setMobSubAreaAttackID(-1);
    getPlayerShip()->setMobSubAreaGroupID(-1);
    getPlayerShip()->setNumCredits(5000ULL);
    display_obj.updateUpperLeft(getPlayerShip()->at(), getMap()->getSize());
    changeMobTile(point(0, 0), getPlayerShip()->at(), getPlayerShip()->getMobType());

    //getPlayerShip()->setNumMaxModules(26);

    Module cm1 = Module(MODULE_CREW, 24, 32);
    getPlayerShip()->addModule(cm1);

    Module fm1 = Module(MODULE_FUEL, 100, 100);
    getPlayerShip()->addModule(fm1);

    Module sm1 = Module(MODULE_SHIELD, allbasicshield_stats[1].base_num_layers, allbasicshield_stats[1].base_num_layers);
    sm1.setShieldStruct(allbasicshield_stats[1]);
    getPlayerShip()->addModule(sm1);

    Module em1 = Module(MODULE_ENGINE, 10, 10);
    em1.setEngineStruct(allbasicengine_stats[1]);
    getPlayerShip()->addModule(em1);

    Module wm1 = Module(MODULE_WEAPON, 50, 50);
    wm1.setWeaponStruct(allbasicweapon_stats[1]);
    getPlayerShip()->addModule(wm1);
    /*

        for (int i = 1; i <= 17; i++)
        {
            Module wm = Module(MODULE_WEAPON, 100, 100);
            wm.setWeaponStruct(allbasicweapon_stats[i]);
            getPlayerShip()->addModule(wm);
        }

        Module cm2 = Module(MODULE_CREW, 96, 96);
        getPlayerShip()->addModule(cm2);

        Module cm3 = Module(MODULE_CREW, 96, 96);
        getPlayerShip()->addModule(cm3);

        Module cm4 = Module(MODULE_CREW, 96, 96);
        getPlayerShip()->addModule(cm4);

        Module cm5 = Module(MODULE_CREW, 96, 96);
        getPlayerShip()->addModule(cm5);
    */
    getPlayerShip()->setModuleSelectionIndex(4);
    last_smloc = getPlayerShip()->at();
}


void Game::save()
{
    std::ofstream os("save.sav", std::ios::binary);
    if (!os) return;

    // Save Game class members
    last_smloc.save(os); // DONE
    last_subarealoc.save(os); // NOT NEEDED
    os.write(reinterpret_cast<const char*>(&turn_timer), sizeof(uint)); // DONE

    // Save externs
    player_ship.save(os); //DONE
    universe.save(os); // DONE
    os.write(reinterpret_cast<const char*>(&current_maptype), sizeof(MapType));
    os.write(reinterpret_cast<const char*>(&current_subarea_id), sizeof(int)); // DONE
    os.write(reinterpret_cast<const char*>(&current_tab), sizeof(tab_type));
}

void Game::load()
{
    std::ifstream is("save.sav", std::ios::binary);
    if (!is) return;

    // Load Game class members
    last_smloc.load(is);
    last_subarealoc.load(is);
    is.read(reinterpret_cast<char*>(&turn_timer), sizeof(uint));

    // Load externs
    player_ship.load(is);
    universe.load(is);
    is.read(reinterpret_cast<char*>(&current_maptype), sizeof(MapType));
    is.read(reinterpret_cast<char*>(&current_subarea_id), sizeof(int));
    is.read(reinterpret_cast<char*>(&current_tab), sizeof(tab_type));
}


bool eightDirectionRestrictedWeaponSelected(MobShip* mb)
{
    return getCurrentMobSelectedModule(mb)->getWeaponStruct().eightDirectionRestricted;
}

bool inRangeStarMapBackdropGreen(backdrop_t starTile)
{
    return (int)starTile >= SMBACKDROP_FRIENDRACE_MAINSEQSTARSUBAREAENTRANCE &&
           (int)starTile <= SMBACKDROP_FRIENDRACE_WHITESTARSUBAREAENTRANCE;
}

bool inRangeStarMapBackdropRed(backdrop_t starTile)
{
    return (int)starTile >= SMBACKDROP_HOSTILERACE_MAINSEQSTARSUBAREAENTRANCE &&
           (int)starTile <= SMBACKDROP_HOSTILERACE_WHITESTARSUBAREAENTRANCE;
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
        starTile == SMBACKDROP_WARZONE_MAINSEQSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_EMPTY_MAINSEQSTARSUBAREAENTRANCE;
}

bool isBlueStarBackdropTile(backdrop_t starTile)
{
    return starTile == SMBACKDROP_BLUESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_FRIENDRACE_BLUESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_HOSTILERACE_BLUESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_WARZONE_BLUESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_EMPTY_BLUESTARSUBAREAENTRANCE;
}


bool isRedStarBackdropTile(backdrop_t starTile)
{
    return starTile == SMBACKDROP_REDSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_FRIENDRACE_REDSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_HOSTILERACE_REDSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_WARZONE_REDSTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_EMPTY_REDSTARSUBAREAENTRANCE;
}


bool isWhiteStarBackdropTile(backdrop_t starTile)
{
    return starTile == SMBACKDROP_WHITESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_FRIENDRACE_WHITESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_HOSTILERACE_WHITESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_WARZONE_WHITESTARSUBAREAENTRANCE ||
        starTile == SMBACKDROP_EMPTY_WHITESTARSUBAREAENTRANCE;
}

void printExitPromptMessage()
{
    msgeAdd("Press ENTER to exit Game now...", cp_whiteonblack);
}

void Game::runContactScenario(ContactTree& contactTree)
{
    int scenario_id = contactTree.startingScenarioID;

    while (true)
    {
        const ContactScenario& scenario = contactTree.getScenario(scenario_id);

        contact_menu_obj.clearMenuItems();
        contact_menu_obj.clearMenuMainText();
        contact_menu_obj.addMenuMainText(scenario.message);

        for (const auto& option : scenario.menuOptions)
        {
            contact_menu_obj.addMenuItem(option, blank_ch);
        }

        display_obj.displayMenu(&contact_menu_obj);
        event_handler.setAction();

        if (event_handler.getAction() == INP_DELTA)
        {
            navigateMenu(&contact_menu_obj);
        }
        else if (event_handler.getAction() == INP_SELECT)
        {
            int sel = contact_menu_obj.getSelectionIndex();

            if (scenario.onSelectCallback)
                scenario.onSelectCallback();

            if (scenario.endConversation || scenario.nextScenarioIDs.empty() || scenario.nextScenarioIDs[sel] == -1)
                break;

            scenario_id = scenario.nextScenarioIDs[sel];
            contact_menu_obj.setSelectionIndex(0);
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
        : createFullNonHostileContactTree(controlRace, nativeRace, enterSubArea);

        runContactScenario(tree);
}
