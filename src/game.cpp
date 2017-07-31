#include "game.h"

game::game()
{
    game_active = true;
    player_has_moved = false;
}

void game::run()
{
    if(display_obj.getGFXEngine()->initSDL())
    {
        /*


        NOTE:

        Save/Load at the moment is BROKEN. It has been disabled.


        */
        display_obj.addString("<<< Cosmic War Adventure >>>",cp_redondarkgray,point(GRIDWID/2 - 12,1));
        display_obj.addString("Press n to begin.",cp_whiteonblack,point(GRIDWID/2 - 6,5));
        display_obj.addString("NOTE: This game is very incomplete...",cp_whiteonblack,point(16,14));
        display_obj.addString("Some of the many unimplemented features:",cp_whiteonblack,point(16,15));
        display_obj.addString("1) Back to main menu upon exit",cp_whiteonblack,point(16,16));
        display_obj.addString("2) Fully functional save/load",cp_whiteonblack,point(16,17));
        display_obj.addString("3) A main plot; ability to win",cp_whiteonblack,point(16,18));
        display_obj.addString("4) Dozens of half-finished core aspects",cp_whiteonblack,point(16,19));
        display_obj.addString("5) Better balance/flow",cp_whiteonblack,point(16,20));

        display_obj.addString("Movement: keys yujklbn, numpad, arrows.",cp_lightgrayonblack,point(16,24));
        display_obj.addString("Target/fire at ship: 't'",cp_lightgrayonblack,point(16,25));
        display_obj.addString("Tab toggle window area (change weapon): 'tab'",cp_lightgrayonblack,point(16,26));
        display_obj.addString("Enter star system/space station: [space]",cp_lightgrayonblack,point(16,27));
        display_obj.addString("Display some debug info in terminal: 'enter'",cp_lightgrayonblack,point(16,28));
        display_obj.addString("Wait for 100 turns in starmap only: '0'",cp_lightgrayonblack,point(16,29));
        display_obj.addString("quit (in game, to quit now: press n first): 'q' (twice)",cp_lightgrayonblack,point(16,30));
        display_obj.addString("Copyright Eric Wolfson 2015-2017",cp_greenonblack,point(GRIDWID/2 - 15,40));
        display_obj.getGFXEngine()->updateScreen();
        event_handler.waitForKey('n');
        initGameObjects();
        primaryGameLoop();
    }
    else
    {
        std::cout << "failed to initialize... press enter to terminate.";
        std::cin.get();
    }
    event_handler.waitForKey('q');
    cleanupEverything();
}

void game::cleanupEverything()
{
    universe.cleanupEverything();
    contact_menu_obj.cleanupEverything();
    player_ship.cleanupEverything();
}

// 1) clear messages, 2) print new message with space bar prompt 3) update screen,
// 4) wait indefinitely until user presses the space bar
void game::msgeAddPromptSpace(std::string s, color_pair col)
{
    display_obj.clearAndDeleteAllMessages();
    msgeAdd(s,col);
    msgeAdd(" [space]",cp_grayonblack);
    display_obj.getGFXEngine()->updateScreen();
    event_handler.waitForKey(' ');
    display_obj.clearAndDeleteAllMessages();
    display_obj.getGFXEngine()->updateScreen();
}

// ask for player input via event_handler "input" class object
void game::promptInput()
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

    // set player input
    event_handler.setAction();

    // flush and clear msgbuffer
    display_obj.clearAndDeleteAllMessages();

    current_mob_turn = getPlayerShip()->getMobSubAreaID();
    player_has_moved = true;

    // get action indicating what action was triggered by player
    switch(event_handler.getAction())
    {
        // if directional key was pressed
        case(INP_DELTA):
            // get delta corresponding to directional key
            delta = event_handler.getDelta();
            if (current_tab == TABTYPE_PLAYAREA)
                movePlayerShip(delta);
            else
                changeSelectedModule(delta);
            break;
        case(INP_SELECT):
            enterArea();
            break;
        case(INP_TOGGLE):
            for (int i = 0; i < universe.getNumRaces(); ++i)
            {
                for (int j = 0; j < universe.getNumRaces(); ++j)
                {
                    if (i != j)
                    {
                        std::cout << universe.getRace(i)->getRaceAttStatus(j) << " ";
                    }
                    else
                    {
                        std::cout << "X ";
                    }
                }
                std::cout << universe.getRace(i)->getDangerLevel() << " ";
                std::cout << "\n";
            }
            break;
        case(INP_DEBUG):
            if (current_maptype == MAPTYPE_LOCALEMAP)
            {
                for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
                {
                    std::cout << CSYS->getNPCShip(i)->getMobSubAreaID() << " " << CSYS->getNPCShip(i)->getMobSubAreaAttackID() <<
                                 " " << (int)(CSYS->getNPCShip(i)->getAIPattern()) << (int)(CSYS->getNPCShip(i)->getGoalStatus()) << "\n";
                }
            }
            break;
        case(INP_QUIT):
            game_active = false;
            break;
        case(INP_CHANGETABFOCUS):
            changeGameTabFocus();
            break;
        case(INP_WEAPONFIRE):
            playerTargetToggle();
            break;
        case(INP_WAIT):
            player_has_moved = true;
            break;
        case(INP_WAITSPECIFY):
            player_has_moved = true;
            if (current_maptype == MAPTYPE_STARMAP)
                wait_counter = 100;
            break;
        default:
            player_has_moved = false;
            break;
    }
}

void game::changeGameTabFocus()
{
    player_has_moved = false;

    if (current_tab == TABTYPE_PLAYAREA)
        current_tab = TABTYPE_PLAYERSHIP;
    else
        current_tab = TABTYPE_PLAYAREA;
}

void game::changeSelectedModule(point delta)
{
    player_has_moved = false;

    if (delta.x() == 1)
        getPlayerShip()->offModuleSelectionIndex(1);
    if (delta.x() == -1)
        getPlayerShip()->offModuleSelectionIndex(-1);
}

void game::movePlayerShip(point delta)
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
    if (mobBlocked(getMap()->getMob(new_loc)))
    if (!CSYS->getNPCShip(new_loc)->isActivated())
    {
        stripShipMobOfResources(getPlayerShip(),CSYS->getNPCShip(new_loc));
        return;
    }

    if (!isBlockingCell(getMap()->getCell(new_loc)))
    {
        changeMobTile(getPlayerShip()->at(),new_loc,getPlayerShip()->getMobType());
        setPlayerLoc(new_loc);
    }
}

// make fuel relevant as well...
void game::stripShipMobOfResources(ship_mob *offender, ship_mob *victim)
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
            msgeAdd("You fail to find any valuable resources aboard the defunct " + victim->getShipName() + ".",cp_grayonblack);
    }
}

void game::initGameObjects()
{
    universe.setupUniverse();
    universe.setSubAreaMapType(SMT_NONE);
    contact_menu_obj = menu(point(2,GRIDHGT/4),point(GRIDWID-4,GRIDHGT/2));
    station_menu_obj = menu(point(2,2),point(GRIDWID-4,SHOWHGT/2-2));
    turn_timer = 0U;
    wait_counter = 0;
    current_maptype = MAPTYPE_STARMAP;
    current_tab = TABTYPE_PLAYAREA;
    current_subarea_id = -1;
    current_player_target = 0;
    getPlayerShip()->setShipMob(true,allshipmob_data[(int)SHIP_PLAYER],
                                NPCSHIPTYPE_NONE,allshipmob_data[(int)SHIP_PLAYER].max_hull,point(PLAYER_START_X,PLAYER_START_Y));
    getPlayerShip()->initMobSubAreaID(0);
    getPlayerShip()->setMobSubAreaAttackID(-1);
    getPlayerShip()->setMobSubAreaGroupID(-1);
    getPlayerShip()->setNumCredits(5000ULL);
    display_obj.updateUpperLeft(getPlayerShip()->at(),getMap()->getSize());
    changeMobTile(point(0,0),getPlayerShip()->at(),getPlayerShip()->getMobType());

    //getPlayerShip()->setNumMaxModules(25);

    module cm1 = module(MODULE_CREW,24,24);
    getPlayerShip()->addModule(cm1);

    module fm1 = module(MODULE_FUEL,100,100);
    getPlayerShip()->addModule(fm1);

    module sm1 = module(MODULE_SHIELD,allbasicshield_stats[1].base_num_layers,allbasicshield_stats[1].base_num_layers);
    sm1.setShieldStruct(allbasicshield_stats[1]);
    getPlayerShip()->addModule(sm1);

    module em1 = module(MODULE_ENGINE,10,10);
    em1.setEngineStruct(allbasicengine_stats[1]);
    getPlayerShip()->addModule(em1);

    for (int i = 1; i < 2; ++i)
    {
    module wm1 = module(MODULE_WEAPON,50,50);
    wm1.setWeaponStruct(allbasicweapon_stats[1]);
    getPlayerShip()->addModule(wm1);
    }
/*
    module wm2 = module(MODULE_WEAPON,100,100);
    wm2.setWeaponStruct(allbasicweapon_stats[1]);
    getPlayerShip()->addModule(wm2);

    module wm3 = module(MODULE_WEAPON,100,100);
    wm3.setWeaponStruct(allbasicweapon_stats[13]);
    getPlayerShip()->addModule(wm3);

    module wm4 = module(MODULE_WEAPON,100,100);
    wm4.setWeaponStruct(allbasicweapon_stats[9]);
    getPlayerShip()->addModule(wm4);
*/
    getPlayerShip()->setModuleSelectionIndex(4);
    last_smloc = getPlayerShip()->at();
}

void game::primaryGameLoop()
{
    msgeAdd("Welcome to Cosmic War Adventure.", cp_purpleonblack);
    msgeAdd("Press 'q' twice to quit.",cp_whiteonblack);
    do
    {
        reDisplay(true);
        promptInput();
        executeMiscPlayerTurnBasedData();
        reDisplay(true);
        if (game_active)
        {
            activateAllNPCAI();
            executeMiscNPCTurnBasedData();
            turn_timer++;
        }
    } while (game_active);

    reDisplay(true);
}

void game::checkForUniversalRaceEvent()
{
    if (current_maptype != MAPTYPE_STARMAP)
        return;

    checkForUnEnslavementRaceEvent();
    checkForRaceWarEvent();
}

void game::checkForRaceWarEvent()
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
                    if (universe.getRace(j)->getRaceOverallMajorStatus() == RMS_FREE)
                    if (randInt(0,RACE_EVENT_CHANCE) < event_roller)
                    {
                        raceInvasionEvent(universe.getRace(i),universe.getRace(j));
                    }
                }
            }
        }
    }
}

void game::raceInvasionEvent(race *offender, race *defender)
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
            return;

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
                msgeAddPromptSpace("You detect a lone, hostile " + offender_name + " ship entering the " +
                                    defender->getNameString() + " " + race_domain_suffix_string[(int)defender->getRaceDomainType()] + " at (" +
                                    int2String(defender->getStarmapLoc().x()) + "," + int2String(defender->getStarmapLoc().y()) + ").",cp_whiteonblack);
            else
                msgeAddPromptSpace("You detect a fleet of " + int2String(num_spawned_ships) + " hostile " + offender_name + " ships entering the " +
                                    defender->getNameString() + " " + race_domain_suffix_string[(int)defender->getRaceDomainType()] + " at (" +
                                    int2String(defender->getStarmapLoc().x()) + "," + int2String(defender->getStarmapLoc().y()) + ").",cp_whiteonblack);
        }
    }
}

void game::checkForSubAreaRaceEvent()
{
    if (current_maptype != MAPTYPE_LOCALEMAP || universe.getSubAreaMapType() != SMT_PERSISTENT)
        return;

    if (!CSYS->isRaceAffiliated())
        return;

    if (CSYS->getNumActiveNativeShipsPresent() == CSYS->getNumShipNPCs() &&
        CSYS->getNumShipNPCs() > 0)
        return;

    if (CSYS->getNumShipNPCs() >= (int)std::min(75,getMapSize().x() * 2))
        return;

    int plnt_race_id = -1;
    int plnt_race_dl = 1;
    int num_plnt_race_native_ships = 0;
    int hw_race_id = CSYS->getNativeRaceID();
    point plnt_loc;

    for (int i = 0; i < universe.getRace(hw_race_id)->getNumHomeworlds(); ++i)
    {
        plnt_loc = universe.getRace(hw_race_id)->getHomeworldStruct(i).loc;
        plnt_race_id = universe.getRace(hw_race_id)->getHomeworldStruct(i).race_owner_id;
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

void game::resetAttackIDs(int id_defeated)
{
    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        if (CSYS->getNPCShip(i)->getMobSubAreaID() != id_defeated)
        if (CSYS->getNPCShip(i)->getMobSubAreaAttackID() == id_defeated)
            CSYS->getNPCShip(i)->setMobSubAreaAttackID(-1);
    }
}

void game::changeMobTile(point from, point to, mob_t whom)
{
    // set null to the mob tile at old location
    getMap()->setMob(from,NIL_m);
    // put mob tile at new location
    getMap()->setMob(to,whom);
    // print cells to avoid characters appearing twice
    display_obj.printCell(getMap(),from,display_obj.getSymbol(getMap(),from));
    display_obj.printCell(getMap(),to,display_obj.getSymbol(getMap(),to));
    printMobCells();
}

ship_mob *game::getPlayerShip()
{
    return &player_ship;
}

map *game::getMap()
{
    switch(current_maptype)
    {
        case(MAPTYPE_LOCALEMAP):
             return CSYS->getMap();
        default:
             break;
    }

    return universe.getMap();
}

point game::getMapSize()
{
    switch(current_maptype)
    {
        case(MAPTYPE_LOCALEMAP):
             return CSYS->getSize();
        default:
             break;
    }

    return universe.getSize();
}

void game::reDisplay(bool calc_los)
{
    // print map
    if (wait_counter == 0)
    {
        display_obj.getGFXEngine()->clearScreen();
        if (calc_los)
            calculatePlayerLOS();
        printWindowBorders();
        display_obj.clearRange(point(1,1),point(SHOWWID,SHOWHGT));
        display_obj.printMap(getMap());
        printMobCells();
        display_obj.printShipStatsSection(getPlayerShip());
        display_obj.printMessages();
    }
    display_obj.getGFXEngine()->updateScreen();
}

void game::printWindowBorders()
{
    std::string current_map_name = "STAR MAP";
    if (current_maptype == MAPTYPE_LOCALEMAP)
    {
        if (universe.getSubAreaMapType() == SMT_PERSISTENT && CSYS->isRaceAffiliated())
        {
            current_map_name = CSYS->getSubAreaName() + " " +
            race_domain_suffix_string[(int)universe.getRace(CSYS->getNativeRaceID())->getRaceDomainType()];
        }
        else
        {
            current_map_name = CSYS->getSubAreaName();
        }
    }
    display_obj.printWindowBorders(current_map_name,getPlayerShip()->getShipName(),current_tab);
}

void game::calculatePlayerLOS()
{
    point p;

    int detect_radius = (current_maptype == MAPTYPE_LOCALEMAP ? player_ship.getDetectRadius() : (player_ship.getDetectRadius() / 2));

    int x_range;

    getMap()->setAllCellsUnvisible();

    for (int y = -detect_radius; y <= detect_radius; ++y)
    {
        x_range = (int)std::sqrt(((double)((detect_radius * detect_radius) - (y * y))));
        for (int x = -x_range; x <= x_range; ++x)
        {
            p = addPoints(point(x,y),getPlayerShip()->at());

            if (inMapRange(p,getMapSize()))
            if (!tracer.isBlocking(getMap(),getPlayerShip()->at(),p,false,false))
            {
                getMap()->setv(p,true);
                getMap()->setm(p,true);
            }
        }
    }
}

void game::msgeAdd(std::string msg, color_pair col_p)
{
    bool reached_buffer_limit = false;
    // adding a message returns a boolean that says whether
    // or not the message buffer is full after adding last
    // message
    if (game_active)
    {
        reached_buffer_limit = display_obj.addMessage(msg,col_p);
    }

    display_obj.printMessages();
    // clear buffer if full
    if (reached_buffer_limit)
    {
        reDisplay(false);
        event_handler.waitForKey(' ');
        display_obj.clearAndDeleteAllMessages();
    }
}

void game::executeMiscPlayerTurnBasedData()
{
    if (!player_has_moved)
        return;

    checkNPCDefeatedEvent();
    checkPlayerDefeatedEvent();

    checkPickUpItems(getPlayerShip());

    if (event_handler.getAction() != INP_WAIT)
    {
        checkMobRegenerateEvent(getPlayerShip(),MODULE_WEAPON);
        checkMobRegenerateEvent(getPlayerShip(),MODULE_SHIELD);
    }

    checkMobHasEngine(getPlayerShip());

    if (getPlayerShip()->getMoveState() == true && event_handler.getAction() != INP_WAIT && event_handler.getAction() != INP_WAITSPECIFY)
        checkPlayerFuelEvent();

    printTileCharacteristics();

    checkForPlayerEncounterEvent();
    checkForUniversalRaceEvent();
    checkForSubAreaRaceEvent();

    if (current_maptype == MAPTYPE_LOCALEMAP && (int)explosion_data.size() > 0)
    {
        createShipDestructionAnimations();
    }
}

void game::checkForPlayerEncounterEvent()
{
    if (current_maptype == MAPTYPE_STARMAP)
    if (wait_counter == 0)
    if (getMap()->getBackdrop(getPlayerShip()->at()) == SMBACKDROP_SPACE)
    {
        spacePirateEncounter();
    }
}

void game::spacePirateEncounter()
{
    if (roll(20))
    {
        reDisplay(true);
        enterSubArea(true);
    }
}

void game::checkPickUpItems(ship_mob *mb)
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

void game::printTileCharacteristics()
{
    if (wait_counter > 0)
        return;

    switch(getMap()->getBackdrop(getPlayerShip()->at()))
    {
        case(SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE):
        case(SMBACKDROP_REDSTARSUBAREAENTRANCE):
        case(SMBACKDROP_BLUESTARSUBAREAENTRANCE):
        case(SMBACKDROP_WHITESTARSUBAREAENTRANCE):
             if (universe.getSubAreaIndex(getPlayerShip()->at()) <= -1)
                 msgeAdd("an unknown star system",cp_grayonblack);
             else
             {
                 if (universe.getRaceIndex(getPlayerShip()->at()) >= 0)
                     msgeAdd(universe.getSubArea(universe.getSubAreaIndex(getPlayerShip()->at()))->getSubAreaName() + " " +
                             race_domain_suffix_string[(int)universe.getRace(universe.getRaceIndex(getPlayerShip()->at()))->getRaceDomainType()],cp_whiteonblack);
                 else
                     msgeAdd("an uninhabited star system",cp_whiteonblack);
             }
             break;
        case(LBACKDROP_SPACESTATION):
             msgeAdd("A space station is here.",cp_grayonblack);
             break;
        case(LBACKDROP_PLANET):
             msgeAdd("There is a home-world here belonging to The " + CSYS->getSubAreaName() + ".",cp_lightblueonblack);
             break;
        case(LBACKDROP_ENSLAVEDPLANET):
             msgeAdd("There is an enslaved planet here under the authority of The " + universe.getRace(universe.getRace(CSYS->getNativeRaceID())->getHomeworldStruct(getPlayerShip()->at()).race_owner_id)->getNameString() + ".",cp_purpleonblack);
             break;
        default:
             break;
    }
}

void game::checkForUnEnslavementRaceEvent()
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
                        if (universe.getRace(j)->getHomeworldStruct(pl).race_owner_id == universe.getRace(i)->getRaceID())
                        {
                            hw_loc = universe.getRace(j)->getHomeworldStruct(pl).loc;
                            universe.getRace(j)->setHomeworldMajorStatus(hw_loc,RMS_FREE);
                            universe.getRace(j)->setHomeworldOwnerRaceID(hw_loc,universe.getRace(j)->getRaceID());
                            universe.getSubArea(universe.getSubAreaIndex(universe.getRace(j)->getStarmapLoc()))->getMap()->setBackdrop(hw_loc,LBACKDROP_PLANET);
                        }
                    }
                }
            }
        }
    }
}

void game::executeMiscNPCTurnBasedData()
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

void game::checkMobRegenerateEvent(ship_mob *s, module_type mt)
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

void game::checkPlayerFuelEvent()
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

void game::setPlayerLoc(point new_loc)
{
    // set the new player coordinates stored in the player's
    getPlayerShip()->setLoc(new_loc);
    // update upperleft display coordinates accordingly
    display_obj.updateUpperLeft(getPlayerShip()->at(),getMap()->getSize());
}

void game::enterStation()
{
    point p = getPlayerShip()->at();
    if (getMap()->getBackdrop(p) == LBACKDROP_SPACESTATION)
    {
       useSpaceStation(p);
    }
    else
    {
       msgeAdd("You can't dock your ship here!",cp_grayonblack);
    }
}

void game::enterSubArea(bool is_encounter)
{
    int subarea_index;
    bool converse_result;
    bool race_affiliated = false;
    star_type st = STARTYPE_NONE;
    point smloc = getPlayerShip()->at();
    if (isStarmapStarTile(getMap()->getBackdrop(smloc)))
    {
        if (universe.getRaceIndex(smloc) >= 0)
            race_affiliated = true;

        if (race_affiliated)
        {
            converse_result = converseViaContactMenu(universe.getRace(universe.getRaceIndex(smloc)));

            if (!converse_result)
                return;

            universe.getRace(universe.getRaceIndex(smloc))->setRaceIDByPlayerStatus(true);
        }

        last_smloc = smloc;
        getMap()->setMob(last_smloc,NIL_m);
        subarea_index = universe.getSubAreaIndex(smloc);
        st = (star_type)((int)getMap()->getBackdrop(smloc) - (int)SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE + 1);

        if (subarea_index >= 0)
        {
            universe.setSubAreaMapType(SMT_PERSISTENT);
            current_subarea_id = subarea_index;
        }
        else
        {
            universe.createSubArea(smloc,SMT_PERSISTENT,(race_affiliated ? SST_RACEHOME : SST_EMPTYSYSTEM),false,race_affiliated,st);
            current_subarea_id = universe.getNumSubAreas() - 1;
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
        getMap()->setMob(last_smloc,NIL_m);
        subarea_index = -1;
        universe.createSubArea(getPlayerShip()->at(),SMT_NONPERSISTENT,(is_encounter ? SST_PIRATEVOID : SST_EMPTYVOID),is_encounter,false,STARTYPE_NONE);
        current_maptype = MAPTYPE_LOCALEMAP;
        pathfinder.setupPathfindDistVector(getMapSize());
        setPlayerLoc(point(getMapSize().x()/2,getMapSize().y()/2));
        changeMobTile(point(0,0),getPlayerShip()->at(),getPlayerShip()->getMobType());
        if (is_encounter)
        {
            player_has_moved = false;
            addEncounterShips();
        }
    }

    printSubAreaEntranceMessage();
}

void game::printSubAreaEntranceMessage()
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
// map_type must represent subarea!
void game::addEncounterShips()
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

void game::enterArea()
{
    player_has_moved = false;

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

void game::navigateMenu(menu *mu)
{
    if (event_handler.getDelta().y() < 0)
        mu->decSelectionIndex();
    else
        mu->incSelectionIndex();
}

void game::useSpaceStation(point p)
{
    bool exit_menu = false;
    msgeAddPromptSpace("Your ship docks at the space station...",cp_blackonwhite);
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
    msgeAdd("Your ship leaves the space station.",cp_grayonblack);
    station_menu_obj.clearMenuItems();
    station_menu_obj.clearMenuMainText();
}

void game::upgradePlayerHull(station *ss_obj)
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

void game::repairPlayerHull(station *ss_obj)
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

void game::playerBuyFuel(station *ss_obj)
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
        display_obj.printShipGraphic(getPlayerShip());
        msgeAdd("You purchase 1 unit of fuel. A low-pitched \"*GLUGG*\" sound echoes throughout your ship.",cp_whiteonblack);
    }
}

void game::upgradePlayerSlotCapacity(station *ss_obj)
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
        display_obj.printShipGraphic(getPlayerShip());
        msgeAdd("Your ships module slot capacity has been upgraded!",cp_whiteonblack);
    }
}

void game::playerHireCrew(station * ss_obj)
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
        display_obj.printShipGraphic(getPlayerShip());
        msgeAdd("A very bizarre looking alien joins your ship's crew.",cp_whiteonblack);
    }
}

void game::playerPurchaseModule(station *ss_obj, menu *ss_mu)
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
            display_obj.printShipGraphic(getPlayerShip());
            ss_mu->eraseMenuItem(selection);
            msgeAdd("Your new module has been installed!",cp_purpleonblack);
        }
        else
        {
            msgeAdd("Your ship already has the maximum number of modules it can hold!",cp_whiteonblack);
        }
    }
}

void game::playerSellModule(station *ss_obj, menu *ss_mu)
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
    display_obj.printShipGraphic(getPlayerShip());
    msgeAdd("You sell the module; it is recycled into raw materials.",cp_whiteonblack);
}

// returns true if conversation leads to entering subarea
bool game::converseViaContactMenu(race *race_obj)
{
    bool exit_menu = false;
    bool converse_result = false;

    if (race_obj->getRacePersonalityType() == PERSONALITY_NONE)
        return true;

    if (!race_obj->playerIdentifiedByRace())
        msgeAddPromptSpace("You encounter an unidentified alien race!",cp_blackonwhite);
    else
    {
        msgeAdd("(e)nter, (c)onverse, (i)gnore?",cp_blackonwhite);
        display_obj.getGFXEngine()->updateScreen();
        SDL_Keycode enter_action = event_handler.getKeyPressed();
        display_obj.clearAndDeleteAllMessages();
        switch(enter_action)
        {
            case(SDLK_e):
                return true;
            case(SDLK_c):
                if (race_obj->getPlayerAttStatus() <= -1)
                {
                    msgeAdd("The " + race_obj->getNameString() + " ignore your attempt to communicate.",cp_grayonblack);
                    return false;
                }
                else
                    break;
            default:
                return false;
        }
    }
    msgeAdd("You make contact.",cp_whiteonblack);
    setEntranceContactData(race_obj,&contact_menu_obj,0);
    while(!exit_menu)
    {
        // set player input for menu
        display_obj.displayMenu(&contact_menu_obj);
        event_handler.setAction();
        switch(event_handler.getAction())
        {
            case(INP_DELTA):
            {
                navigateMenu(&contact_menu_obj);
                break;
            }
            case(INP_SELECT):
            {
                contact_menu_obj.traverseMenuMainText();
                if (contact_menu_obj.getNumMenuMainTextStrings() == 1)
                {
                    if (contact_menu_obj.getMenuLevel() == MAX_CONTACT_MENU_LEVEL)
                    {
                        exit_menu = true;
                        break;
                    }
                    setEntranceContactData(race_obj,&contact_menu_obj,contact_menu_obj.getMenuLevel() + 1);
                    converse_result = executeConverseEvent(race_obj);
                    contact_menu_obj.setSelectionIndex(0);
                }
                break;
            }
            default:
                break;
        }
    }
    contact_menu_obj.clearMenuItems();
    contact_menu_obj.clearMenuMainText();
    display_obj.clearAndDeleteAllMessages();
    return converse_result;
}

// returns true if conversation leads to entering subarea
bool game::executeConverseEvent(race *race_obj)
{
    switch((converse_event)contact_menu_obj.getSelectionIndex())
    {
        case(CONVERSE_EXIT):
             return false;
        case(CONVERSE_ENTERSUBAREA):
        case(CONVERSE_SERVICE):
        case(CONVERSE_INFORMATION):
             return true;
        case(CONVERSE_FIGHT):
             race_obj->setPlayerAttStatus((int)std::min(-1,race_obj->getPlayerAttStatus() - 1));
             return true;
        default:
             break;
    }
    return false;
}

void game::returnToStarMap()
{
    getMap()->setMob(getPlayerShip()->at(),NIL_m);
    current_maptype = MAPTYPE_STARMAP;
    universe.setSubAreaMapType(SMT_NONE);
    current_subarea_id = -1;
    setPlayerLoc(last_smloc);
    changeMobTile(point(0,0),getPlayerShip()->at(),getPlayerShip()->getMobType());
    reDisplay(true);
}

void game::moveNPC(ship_mob *mb, point new_loc)
{
    if (inRange(new_loc,point(1,1),addPoints(getMapSize(),point(-2,-2))))
    if (!isBlockingCell(getMap()->getCell(new_loc)))
    if (!isAt(new_loc,getSubAreaSystemStartPoint(getMapSize())))
    {
        changeMobTile(mb->at(),new_loc,mb->getMobType());
        mb->setLoc(new_loc);
    }
}

void game::checkNPCMoveEvent(ship_mob *mb)
{
    if (mb->getMoveState() == false)
        return;

    point source = mb->at();

    point dest = mb->getDestination();

    if (!isAt(source,dest))
    {
        if (shortestPath(source,dest) <= mb->getDetectRadius() ||
            mb->getAIPattern() != AIPATTERN_ATTACKING)
        {
            point next_loc;

            bool blockage;

            bool not_on_map_border;

            blockage = tracer.isBlocking(getMap(),source,dest,true,false);

            not_on_map_border = (notOnMapBorder(dest,getMap()->getSize()) &&
                                 notOnMapBorder(source,getMap()->getSize()));

            if (blockage && not_on_map_border)
                next_loc = pathfinder.dijkstra(getMap(),source,dest);
            else
                next_loc = tracer.getLinePoint(tracer.getLineSize()-2);

            moveNPC(mb,next_loc);
        }
    }
    else
        mb->setGoalStatus(GOALSTATUS_COMPLETE);
}

bool game::checkNPCWeaponEvent(ship_mob *mb)
{
    int attack_id = mb->getMobSubAreaAttackID();
    if (attack_id >= 0)
    {
        ship_mob *mob_being_attacked = getMobFromID(attack_id);

        if (rollPerc(mb->getStatStruct().weapon_change_chance))
        if (mb->getNumInstalledModulesOfType(MODULE_WEAPON) >= 2)
        {
            mobChangeSelectedWeapon(mb);
            printShipmobWeaponEventMessage(mb,"activates");
        }

        if (shortestPath(mob_being_attacked->at(),mb->at()) <= getCurrentMobSelectedModule(mb)->getWeaponStruct().travel_range)
        if (getCurrentMobSelectedModule(mb)->getFillQuantity() >= getWeaponModuleConsumptionPerTurn(getCurrentMobSelectedModule(mb)))
        if (!tracer.isBlocking(getMap(),mb->at(),mob_being_attacked->at(),false,false))
        if (rollPerc(mb->getStatStruct().shoot_frequency))
        {
            mobFire(mb,mob_being_attacked->at());
            return true;
        }
    }
    return false;
}

void game::mobChangeSelectedWeapon(ship_mob *mb)
{
    int module_index;

    do
    {
        module_index = randInt(0,mb->getNumInstalledModules()-1);
    }while (module_index == mb->getModuleSelectionIndex() || mb->getModule(module_index)->getModuleType() != MODULE_WEAPON);

    mb->setModuleSelectionIndex(module_index);
}

void game::checkNPCAggroEvent(ship_mob *mb)
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
    if (shortestPath(mb->at(),getPlayerShip()->at()) <= mb->getDetectRadius())
    {
        max_anger_factor = pattstatus;
        mb->setMobSubAreaAttackID(0);
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
                if (shortestPath(mb->at(),CSYS->getNPCShip(i)->at()) <= mb->getDetectRadius())
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


// Set behavior of NPC AI
void game::setNPCAIPattern(ship_mob *mb)
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

void game::setNPCGoalDestinationLoc(ship_mob *mb)
{
    switch(mb->getAIPattern())
    {
        case(AIPATTERN_NEUTRAL):
            mb->setDestination(mb->getInitLoc());
            mb->setGoalStatus(GOALSTATUS_COMPLETE);
            break;
        case(AIPATTERN_ATTACKING):
            mb->setDestination(getMobFromID(mb->getMobSubAreaAttackID())->at());
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

void game::checkNPCPlanetMoveEvent(ship_mob *mb)
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

void game::setNPCRandFreePlanetDestination(ship_mob *mb)
{
    mb->setDestination(universe.getRace(CSYS->getNativeRaceID())->getFirstFreeHomeworldLoc());
}

void game::activateOneNPC(ship_mob *mb)
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

void game::setNPCRandDestination(ship_mob *mb)
{
    //mb->setDestination(getRandNPCShipOpenPoint(getMap(),point(mb->at().x()-mb->getDetectRadius(),mb->at().y()-mb->getDetectRadius()),
    //                                                    point(mb->at().x()+mb->getDetectRadius(),mb->at().y()+mb->getDetectRadius()),
    //                                                    LBACKDROP_SPACE,LBACKDROP_WHITESTARBACKGROUND));
    mb->setDestination(getRandNPCShipOpenPoint(getMap(),point(1,1),addPoints(getMapSize(),point(-2,-2)),LBACKDROP_SPACE_UNLIT,LBACKDROP_WHITESTARBACKGROUND));
}

void game::checkNPCFlags(ship_mob *mb)
{
    checkMobHasEngine(mb);
    checkNPCPlanetAttackEvent(mb);
}

void game::checkNPCPlanetAttackEvent(ship_mob *mb)
{
    if (universe.getSubAreaMapType() == SMT_PERSISTENT)
    if (CSYS->isRaceAffiliated())
    if (universe.getRace(mb->getMobSubAreaGroupID())->getRaceOverallMajorStatus() != RMS_ENSLAVED)
    if (mb->getAIPattern() == AIPATTERN_APPROACHPLANET)
    {
        if (getMap()->getBackdrop(mb->at()) == LBACKDROP_PLANET)
        {
            universe.getRace(CSYS->getNativeRaceID())->setHomeworldMajorStatus(mb->at(),RMS_ENSLAVED);
            universe.getRace(CSYS->getNativeRaceID())->setHomeworldOwnerRaceID(mb->at(),mb->getMobSubAreaGroupID());
            getMap()->setBackdrop(mb->at(),LBACKDROP_ENSLAVEDPLANET);
            if (getMap()->getv(mb->at()))
                msgeAdd("The " + mb->getShipName() + " deploys a slave shield over the home-world!",cp_whiteonblack);
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

void game::checkMobHasEngine(ship_mob *mb)
{
    if (mb->getNumInstalledModulesOfType(MODULE_ENGINE) == 0)
        mb->setMoveState(false);
    else
        mb->setMoveState(true);
}

void game::activateAllNPCAI()
{
    if (current_maptype != MAPTYPE_LOCALEMAP)
        return;

    if (!player_has_moved)
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
            while(CSYS->getNPCShip(gmti)->getTurnTimer() > 0.0)
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

// only works in star systems for now...
void game::playerTargetToggle()
{
    if (current_tab != TABTYPE_PLAYAREA)
        return;

    if (current_maptype != MAPTYPE_LOCALEMAP)
    {
        msgeAdd("You can't target anything here!",cp_grayonblack);
        return;
    }

    point fire_at = getPlayerShip()->at();

    point temp_loc;

    fire_at = getNextTargetedNPC();

    if (getCurrentMobSelectedModule(getPlayerShip())->getModuleType() != MODULE_WEAPON)
    {
        msgeAdd("Your selected module is not a weapon!",cp_grayonblack);
        return;
    }

    if (getCurrentMobSelectedModule(getPlayerShip())->getFillQuantity() >=
        getWeaponModuleConsumptionPerTurn(getCurrentMobSelectedModule(getPlayerShip())))
    {
        msgeAdd("Target where?",cp_whiteonblack);

        do
        {
            printPlayerFirePath(getPlayerShip()->at(),fire_at);

            if ((int)getMap()->getMob(fire_at) > (int)SHIP_PLAYER)
                display_obj.displayMonitor(MONITOR_TARGETINFO,CSYS->getNPCShip(fire_at));

            display_obj.getGFXEngine()->updateScreen();

            event_handler.setAction();

            if (event_handler.getAction() == INP_DELTA)
            {
                temp_loc = addPoints(fire_at,event_handler.getDelta());
                if (inRange(temp_loc,point(0,0),getMaxMapPoint(getMap())))
                if (getMap()->getv(temp_loc))
                    fire_at = temp_loc;
            }

            if (isAt(temp_loc,fire_at))
                clearAllFireCells(getMap());

            if (event_handler.getAction() == INP_TOGGLE)
            {
                clearAllFireCells(getMap());
                current_player_target = (current_player_target > 0 ? current_player_target - 1 : CSYS->getNumShipNPCs() - 1);
                return;
            }

            if (event_handler.getAction() == INP_SELECT)
            {
                clearAllFireCells(getMap());
                fire_at = getNextTargetedNPC();
            }

        } while (event_handler.getAction() != INP_WEAPONFIRE);

        clearAllFireCells(getMap());

        display_obj.getGFXEngine()->updateScreen();

        if (!isAt(fire_at,getPlayerShip()->at()))
            mobFire(getPlayerShip(),fire_at);
        else
            msgeAdd("Cannot target your own ship...",cp_grayonblack);
    }
    else
        msgeAdd("You do not have enough power or projectiles to fire that weapon now!",cp_grayonblack);

    current_player_target = (current_player_target > 0 ? current_player_target - 1 : CSYS->getNumShipNPCs() - 1);
}

void game::printPlayerFirePath(point a, point b)
{
    tracer.bresenham(getMap()->getSize(),a,b);

    getMap()->setFire(b,FIRET_CURSOR);

    for (int i = 1; i < tracer.getLineSize(); ++i)
        getMap()->setFire(tracer.getLinePoint(i),FIRET_CURSORPATH);

    if (!isAt(a,b))
        getMap()->setFire(a,NIL_f);

    display_obj.printMap(getMap());
    printMobCells();
}

point game::getNextTargetedNPC()
{
    bool found_target = false;

    int iter = 0;

    point loc = getPlayerShip()->at();

    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        if (isTargetableNPC(i))
            iter++;
    }

    if (iter == 0)
        return loc;

    current_player_target = (int)std::max(current_player_target,0);
    current_player_target = (int)std::min(CSYS->getNumShipNPCs()-1,current_player_target);

    do
    {
        if (isTargetableNPC(current_player_target))
        {
            loc = CSYS->getNPCShip(current_player_target)->at();
            found_target = true;
        }

        current_player_target = (current_player_target + 1) % CSYS->getNumShipNPCs();
    }
    while (!found_target);

    return loc;
}

bool game::isTargetableNPC(int n)
{
    return getMap()->getv(CSYS->getNPCShip(n)->at()) &&
           shortestPath(getPlayerShip()->at(),CSYS->getNPCShip(n)->at()) <= getCurrentMobSelectedModule(getPlayerShip())->getWeaponStruct().travel_range;
}

void game::mobShootSingleProjectile(ship_mob *mb, point dest)
{
    module *current_mod = getCurrentMobSelectedModule(mb);
    point source = mb->at(), lof = point(1,1);
    int point_iter = 0;
    int primary_iter = 0;
    int range = current_mod->getWeaponStruct().travel_range;
    bool is_detectable = (getMap()->getv(mb->at()) || isAt(dest,getPlayerShip()->at()));
    bool hitmob_condition;
    tracer.bresenham(getMapSize(),source,dest);
    do
    {
        point_iter = tracer.getLineSize() - 2;
        do
        {
            lof = tracer.getLinePoint(point_iter);
            if (is_detectable)
                outputLOFTransition(lof,mb->at(),dest,current_mod->getWeaponStruct().ftile);
            hitmob_condition = checkForMobInLOF(mb,lof,true,false);
            if (hitmob_condition || primary_iter == range - 1 || terrainBlocked(getMap()->getBackdrop(lof)))
            {
                endOfProjectileLoop(mb,lof,is_detectable);
                return;
            }
            point_iter--;
            primary_iter++;
        }
        while (point_iter >= 0);
        extrapolateLine(source,dest);
        if (tracer.getLineSize() <= 1)
        {
            if (is_detectable)
                clearAllFireCells(getMap());
            return;
        }
    }
    while (1);
}

// shoot spread burst sprites that are width tiles wide
void game::mobShootSpread(ship_mob *mb, point dest, int width)
{
    module *current_mod = getCurrentMobSelectedModule(mb);

    int dist_counter = 0;

    int range = current_mod->getWeaponStruct().travel_range;

    point source = mb->at(), adjp;

    point dl = point(dest.x() - source.x(), dest.y() - source.y());

    std::vector<bool> block;
    std::vector<point> lof;

    for (int i = 0; i < width; ++i)
        block.push_back(false);

    lof.resize(width);

    bool is_detectable = (getMap()->getv(mb->at()) || isAt(dest,getPlayerShip()->at()));

    bool all_blocked;

    int point_iter = 0;

    if (std::abs(dl.y()) > std::abs(dl.x()))
        adjp = point(1,0);
    else
        adjp = point(0,1);

    tracer.bresenham(getMapSize(),source,dest);

    point_iter = tracer.getLineSize() - 2;

    do
    {
        dest = tracer.getLinePoint(point_iter);

        for (int i = 0; i < width; ++i)
             lof[i].set(dest.x() + (i - (int)(width/2))*adjp.x(), dest.y() + (i - (int)(width/2))*adjp.y());

        for (int i = 0; i < width; ++i)
        if (!block[i])
        {
            if (!inMapRange(lof[i],getMapSize()) || terrainBlocked(getMap()->getBackdrop(lof[i])))
                block[i] = true;
            else
            {
                if (is_detectable)
                    printAndSetFireCell(getMap(),lof[i],current_mod->getWeaponStruct().ftile);
                checkForMobInLOF(mb,lof[i],false,false);
            }
        }

        if (is_detectable)
            display_obj.delayAndUpdate(15);

        dist_counter++;

        point_iter--;

        if (point_iter == -1)
        {
            extrapolateLine(source,dest);
            point_iter = tracer.getLineSize() - 2;
        }

        all_blocked = true;
        for (int i = 0; i < width; ++i)
             if (!block[i])
                 all_blocked = false;
    }
    while ((dist_counter < range) && !all_blocked);

    if (is_detectable)
        clearAllFireCells(getMap());
}

void game::mobShootPulse(ship_mob *mb, point dest)
{
    module *current_mod = getCurrentMobSelectedModule(mb);

    bool end_hit = false;
    bool hit_being = false;

    point lof, lof_hit, source = mb->at();

    tracer.bresenham(getMapSize(),source,dest);

    int point_iter = tracer.getLineSize() - 1;

    int max_dist = current_mod->getWeaponStruct().travel_range;

    int dist_counter = 0;

    bool is_detectable = (getMap()->getv(mb->at()) || isAt(dest,getPlayerShip()->at()));

    do
    {
        // condition: fire_t has NOT hit something
        if (!end_hit)
        {
            if (point_iter == 0)
            {
                extrapolateLine(source,dest);
                if (tracer.getLineSize() <= 1)
                    break;
                point_iter = tracer.getLineSize() - 1;
            }
            point_iter--;
            lof = tracer.getLinePoint(point_iter);

            if (checkForMobInLOF(mb,lof,false,false))
                hit_being = true;

            if (hit_being || dist_counter >= max_dist || terrainBlocked(getMap()->getBackdrop(lof)))
            {
                lof_hit = lof;
                tracer.bresenham(getMapSize(),mb->at(),lof_hit);
                point_iter = tracer.getLineSize() - 1;
                end_hit = true;
            }

            if (is_detectable)
            {
                printAndSetFireCell(getMap(),lof,current_mod->getWeaponStruct().ftile);
                display_obj.delayAndUpdate(15);
            }
        }
        // it has hit something or reached end
        else
        {
            if (point_iter <= 0)
                break;

            lof = tracer.getLinePoint(point_iter);

            //set farthest cell from enemy/player with "t" fire_t value to NIL
            if (is_detectable)
            {
                clearAllFireCellsInRange(getMap(),lof,1);
                display_obj.delayAndUpdate(15);
            }

            if (hit_being)
                checkForMobInLOF(mb,lof_hit,false,true);

            point_iter--;
        }
        dist_counter++;
    }
    while (1);

    if (is_detectable)
        clearAllFireCells(getMap());
}

void game::extrapolateLine(point &source, point &dest)
{
    point extrap = dest;
    dest.setx(2*dest.x() - source.x());
    dest.sety(2*dest.y() - source.y());
    // source is now former target
    source = extrap;

    tracer.bresenham(getMapSize(),source,dest);
}

void game::printShipmobWeaponEventMessage(ship_mob * mb, std::string action_str)
{
    if (!getMap()->getv(mb->at()))
        return;
    msgeAdd(getNamePrefix(mb),cp_grayonblack);
    msgeAdd(mb->getShipName(),mb->getShipSymbol().color);
    msgeAdd(action_str + " its",cp_grayonblack);
    msgeAdd(getCurrentMobSelectedModule(mb)->getWeaponStruct().name_modifier + ".",
            getCurrentMobSelectedModule(mb)->getWeaponStruct().disp_chtype.color);
}

void game::mobFire(ship_mob *mb, point p)
{
    module *weapon_mod = getCurrentMobSelectedModule(mb);

    weapon_struct current_ws = weapon_mod->getWeaponStruct();

    weapon_t selected_weapon = current_ws.wt;

    printShipmobWeaponEventMessage(mb,"fires");

    color_pair weapon_cp = current_ws.disp_chtype.color;

    weapon_mod->offFillQuantity(-1*current_ws.consumption_rate*current_ws.num_shots);

    for (int i = 0; i < weapon_mod->getWeaponStruct().num_shots; ++i)
    {
        switch(selected_weapon)
        {
            case(WEAPONTYPE_BLAST):
            {
                msgeAdd("*choom!*",weapon_cp);
                break;
            }
            case(WEAPONTYPE_MISSILE):
            {
                msgeAdd("*chhhuuggg*",weapon_cp);
                break;
            }
            case(WEAPONTYPE_SPREAD):
            {
                msgeAdd("*FWOOOOSHHHH!*",weapon_cp);
                break;
            }
            case(WEAPONTYPE_PULSE):
            {
                msgeAdd("*VREEEEEEEEE!*",weapon_cp);
                break;
            }
            case(WEAPONTYPE_WALLOP):
            {
                msgeAdd("*VRRRRUPP*",weapon_cp);
                break;
            }
            case(WEAPONTYPE_MECH):
            {
                msgeAdd("*VRRRRRUUUMMMM*",weapon_cp);
                break;
            }
            case(WEAPONTYPE_HELL):
            {
                msgeAdd("*VWAAAAAAAAAAAA*",weapon_cp);
                break;
            }
            default:
                break;
        }
    }

    for (int i = 0; i < weapon_mod->getWeaponStruct().num_shots; ++i)
    {
        switch(selected_weapon)
        {
            case(WEAPONTYPE_MISSILE):
            case(WEAPONTYPE_WALLOP):
            case(WEAPONTYPE_MECH):
            case(WEAPONTYPE_BLAST):
            case(WEAPONTYPE_HELL):
            {
                mobShootSingleProjectile(mb,p);
                break;
            }
            case(WEAPONTYPE_SPREAD):
            {
                mobShootSpread(mb,p,3);
                break;
            }
            case(WEAPONTYPE_PULSE):
            {
                mobShootPulse(mb,p);
                break;
            }
            default:
                break;
        }
    }
}

ship_mob * game::getSubAreaShipMobAt(point p)
{
    if (getMap()->getMob(p) == SHIP_PLAYER)
        return getPlayerShip();
    return CSYS->getNPCShip(p);
}

void game::checkCreateDamagingExplosion(point lof, int blast_radius, int ms_delay, fire_t ft, bool print_msge, damage_report dr, bool caused_by_ship)
{
    if (blast_radius > 0 && inMapRange(lof,getMapSize()))
    {
        createDamagingExplosionAnimation(lof,blast_radius,ms_delay,ft,print_msge);
        checkMobExplosionRadiusDamage(lof,blast_radius,dr,caused_by_ship);
    }
}

void game::checkCreateDamagingExplosionRollDamage(ship_mob * mb, point lof, int blast_radius)
{
    if (blast_radius <= 0)
        return;

    clearAllFireCellsInRange(getMap(),lof,1);
    damage_report dr = {0,0,0};
    module *weapon_mod = getCurrentMobSelectedModule(mb);
    dr.hull_damage = rollMultipleDice(weapon_mod->getWeaponStruct().hull_damage);
    dr.crew_damage = rollMultipleDice(weapon_mod->getWeaponStruct().crew_damage);
    checkCreateDamagingExplosion(lof,blast_radius,15,FIRET_DAMAGINGEXPLOSION,false,dr,true);
}

void game::checkIfRaceAggroEvent(ship_mob *attacker, ship_mob *target)
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
            universe.getRace(tgid)->incRaceAttStatus(agid,-1);
        }
    }
}

// returns true if fire projectile/laser/wallop etc... hits something
bool game::checkForMobInLOF(ship_mob *mb, point lof, bool add_hitsprite, bool guaranteed_hit)
{
    if (getMap()->getMob(lof) == NIL_m)
        return false;

    ship_mob * mob_in_lof = getSubAreaShipMobAt(lof);

    checkIfRaceAggroEvent(mb,mob_in_lof);

    int accuracy_val;
    int blast_radius = 0;

    accuracy_val = baseHitChance(mb,mob_in_lof);

    // base hit chance determines if it was a hit or a miss
    if (accuracy_val >= 0 || guaranteed_hit)
    {
        if (add_hitsprite && getMap()->getv(lof))
            addHitSprite(getMap(),lof);

        damage_report dr = damageShipMob(mb,mob_in_lof,lof);

        //displayDamageReport(dr,mob_in_lof,false);

        blast_radius = getCurrentMobSelectedModule(mb)->getWeaponStruct().blast_radius;

        checkCreateDamagingExplosion(lof,blast_radius,15,FIRET_DAMAGINGEXPLOSION,true,dr,true);

        if (rollPerc(getCurrentMobSelectedModule(mb)->getWeaponStruct().travel_through_chance))
            return false;
        else
            return true;
    }
    else
    {
        //displayEvasionReport(accuracy_val, mob_in_lof);
    }

    return false;
}

void game::displayEvasionReport(int acc_val, ship_mob *mb)
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

    msgeAdd(getNamePrefix(mb) + " " + mb->getShipName() + " " + evasion_str,mb->getShipSymbol().color);
}

void game::displayDamageReport(damage_report dr,ship_mob *mb, bool exp_damage)
{
    if (!getMap()->getv(mb->at()))
        return;

    if (mb->getHullStatus() + dr.hull_damage <= 0)
        return;

    std::string report_str = mb->getShipName();

    std::string hull_perc_str = "";

    if(!exp_damage)
        report_str += " hit!";
    else
        report_str += " within blast radius!";

    int shield_perc = mb->getTotalFillPercentageOfType(MODULE_SHIELD);

    if (mb->getHullStatus() > 0)
        hull_perc_str = int2String((int)(((double)mb->getHullStatus()/(double)mb->getMaxHull())*100.0));
    else
        hull_perc_str = int2String(0);

    if (dr.shield_damage > 0)
    {
        report_str += " Shields ";
        if (shield_perc <= 0)
            report_str += "down!";
        else
            report_str += "to " + int2String(shield_perc) + "\%!";

        msgeAdd(report_str,mb->getShipSymbol().color);
        return;
    }

    if (dr.hull_damage > 0)
    {
        report_str += " Hull to " + hull_perc_str + "\%!";
    }

    if (mb->getHullStatus() > 0 && dr.crew_damage > 0 && mb->getTotalMTFillRemaining(MODULE_CREW) > 0)
    {
        report_str += " " + int2String(dr.crew_damage) + " crew perished!";
    }

    msgeAdd(report_str,mb->getShipSymbol().color);
}

void game::checkNPCDefeatedEvent()
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
            getMap()->setMob(p,NIL_m);
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

void game::checkPlayerDefeatedEvent()
{
    point p;
    if (getPlayerShip()->getHullStatus() <= 0)
    {
        p = getPlayerShip()->at();
        msgeAdd("Your ship has been destroyed!",cp_darkredonblack);
        msgeAdd("Press 'q' to quit...",cp_whiteonblack);
        display_obj.printShipStatsSection(getPlayerShip());
        getMap()->setMob(p,NIL_m);
        explosion_data.push_back({p,getPlayerShip()->getStatStruct().destruction_radius,0});
        resetAttackIDs(0);
        game_active = false;
        return;
    }

    if (getPlayerShip()->getTotalMTFillRemaining(MODULE_CREW) <= 0)
    {
        msgeAdd("All crew members aboard your ship have vanquished!",cp_darkredonblack);
        display_obj.printShipStatsSection(getPlayerShip());
        resetAttackIDs(0);
        game_active = false;
        return;
    }
}

// details to follow regarding mechanic here...
// mradius must be 2 or more
void game::checkMobExplosionRadiusDamage(point p, int mradius, damage_report dr, bool caused_by_ship)
{
    if (mradius < 2)
        return;

    point loc;
    damage_report dr_mod;
    for (int i = -(mradius - 1); i <= (mradius - 1); ++i)
    for (int j = -(mradius - 1); j <= (mradius - 1); ++j)
    {
        loc.set(i+p.x(),j+p.y());
        if (inMapRange(loc,getMapSize()))
        if (!(i == 0 && j == 0))
        if (getMap()->getMob(loc) != NIL_m)
        {
            if (caused_by_ship)
                checkIfRaceAggroEvent(getCurrentMobTurn(),getSubAreaShipMobAt(loc));
            dr_mod = damageShipMobFromBlastRadius(getSubAreaShipMobAt(loc),p,dr.hull_damage,dr.crew_damage);
            //displayDamageReport(dr_mod,getSubAreaShipMobAt(loc),true);
        }
    }
}

ship_mob * game::getMobFromID(int id)
{
    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        if (CSYS->getNPCShip(i)->getMobSubAreaID() == id)
            return CSYS->getNPCShip(i);
    }

    return getPlayerShip();
}

ship_mob * game::getCurrentMobTurn()
{
    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        if (CSYS->getNPCShip(i)->getMobSubAreaID() == current_mob_turn)
            return CSYS->getNPCShip(i);
    }

    return getPlayerShip();
}

void game::createShipDestructionAnimations()
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

void game::createDamagingExplosionAnimation(point p, int radius, int ms_update, fire_t exp_sprite, bool print_msge)
{
    if (print_msge)
        msgeAdd("KABOOOOOOOOOOOOM!!!",fire_symbol[(int)exp_sprite].color);

    if (!getMap()->getv(p))
        return;

    printFireCircle(p,radius,exp_sprite);

    display_obj.delayAndUpdate(ms_update);

    clearAllFireCells(getMap());
}

void game::printFireCircle(point p, int r, fire_t ft)
{
    int x_range;

    for (int y = -r; y <= r; ++y)
    {
        x_range = (int)std::sqrt(((double)((r * r) - (y * y))));
        for (int x = -x_range; x <= x_range; ++x)
        if (inMapRange(addPoints(p,point(x,y)),getMapSize()))
            printAndSetFireCell(getMap(),addPoints(p,point(x,y)),ft);
    }
}

void game::endOfProjectileLoop(ship_mob *mb, point lof, bool is_detectable)
{
    if (is_detectable)
        clearAllFireCellsInRange(getMap(),lof,1);

    if (terrainBlocked(getMap()->getBackdrop(lof)))
        checkCreateDamagingExplosionRollDamage(mb,lof,getCurrentMobSelectedModule(mb)->getWeaponStruct().blast_radius);
}

void game::outputLOFTransition(point lof, point source, point dest, fire_t ft)
{
    clearAllFireCellsInRange(getMap(),lof,1);
    printAndSetFireCell(getMap(),lof,selectFireCell(source,dest,ft));
    display_obj.delayAndUpdate(16);
}

void game::clearAllFireCellsInRange(map *m, point lof, int r)
{
    point dxy;
    for (int x = lof.x() - r; x <= lof.x() + r; ++x)
    for (int y = lof.y() - r; y <= lof.y() + r; ++y)
    {
        dxy.set(x,y);
        printAndSetFireCell(m,dxy,NIL_f);
    }
}

void game::clearAllFireCells(map *m)
{
    point p;
    for (int x = 0; x < m->getSize().x(); ++x)
    for (int y = 0; y < m->getSize().y(); ++y)
    {
        p.set(x,y);
        m->setFire(p,NIL_f);
    }
    display_obj.printMap(m);
    printMobCells();
}

void game::printAndSetFireCell(map *m, point p, fire_t f)
{
    if (inMapRange(p,m->getSize()))
    {
        m->setFire(p,f);
        display_obj.printCell(m,p,display_obj.getSymbol(m,p));
        printMobCells();
    }
}

void game::addHitSprite(map *m, point p)
{
    printAndSetFireCell(m,p,FIRET_EXPLOSION);
    display_obj.delayAndUpdate(15);
    clearAllFireCellsInRange(m,p,1);
}

void game::printMobCells()
{
    if (current_maptype != MAPTYPE_LOCALEMAP)
        return;

    point m_loc;
    chtype ct;

    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        m_loc = CSYS->getNPCShip(i)->at();

        if (getMap()->getv(m_loc) && getMap()->getFire(m_loc) == NIL_f && getMap()->getMob(m_loc) != NIL_m)
        {
            ct = CSYS->getNPCShip(i)->getShipSymbol();
            if (!CSYS->getNPCShip(i)->isActivated())
            {
                ct.color.bg = CSYS->getNPCShip(i)->getShipSymbol().color.fg;
                ct.color.fg = color_black;
            }
            display_obj.printCell(getMap(),m_loc,ct);
        }
    }
}

void game::pickUpItems(ship_mob *mb, int s, int q)
{
    if (current_maptype != MAPTYPE_LOCALEMAP)
        return;

    item_stats istats = *(CSYS->getPile(mb->at())->getItem(s)->getStats());

    if (istats.i_type == ITEM_WRECKAGECREDIT)
        mb->setNumCredits(mb->getNumCredits() + (uint_64)q);

    CSYS->deleteItemFromPile(mb->at(),s,q);
}

void game::save()
{
    std::ofstream os("cosmicsave.txt");
    os.write((const char *)&current_maptype,sizeof(map_type));
    os.write((const char *)&current_subarea_id,sizeof(int));
    last_smloc.save(os);
    last_subarealoc.save(os);
    player_ship.save(os);
    display_obj.save(os);
    universe.save(os);
    os.close();
}

void game::load()
{
    std::ifstream is("cosmicsave.txt");
    is.read((char *)&current_maptype,sizeof(map_type));
    is.read((char *)&current_subarea_id,sizeof(int));
    last_smloc.load(is);
    last_subarealoc.load(is);
    player_ship.load(is);
    display_obj.load(is);
    universe.load(is);
    is.close();
}

std::string getNamePrefix(ship_mob *mb)
{
    if (mb->isCurrentPlayerShip())
        return "Your";
    return "The";
}
