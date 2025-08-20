#include "contact.h"

void loadStationInitialContactMenu(station* stationObj, menu* menuObj)
{
    const int numChoices = stationObj->getNumBasicStationTradeChoices();

    for (int i = 0; i < numChoices; ++i)
    {
        const basic_station_trade_choice choice = stationObj->getBasicStationTradeChoice(i);
        std::string label;
        chtype symbol = blank_ch;

        switch (choice)
        {
        case STATIONCHOICE_FUEL:
            label = "Buy Fuel: " + uint642String(stationObj->getFuelCost()) + " CRED";
            symbol = fuelupgrade_symbol;
            break;

        case STATIONCHOICE_HULLREPAIR:
            label = "Repair Hull: " + uint642String(stationObj->getHullFixCost()) + " CRED";
            symbol = hull_repair_symbol;
            break;

        case STATIONCHOICE_HULLUPGRADE:
            label = "Upgrade Hull: " + uint642String(stationObj->getHullUpgradeCost()) + " CRED";
            symbol = hull_upgrade_symbol;
            break;

        case STATIONCHOICE_INCNUMMODULECAPACITY:
            label = "Upgrade Slot Capacity: " + uint642String(stationObj->getSlotCapUpgradeCost()) + " CRED";
            symbol = slot_upgrade_symbol;
            break;

        case STATIONCHOICE_HIRECREW:
            label = "Hire Crew: " + uint642String(stationObj->getCrewCost()) + " CRED";
            symbol = crew_upgrade_symbol;
            break;

        case STATIONCHOICE_BUYMODULE:
            label = "Buy modules";
            symbol = module_buy_symbol;
            break;

        case STATIONCHOICE_SELLMODULE:
            label = "Sell modules";
            symbol = module_sell_symbol;
            break;

        default:
            continue;
        }

        menuObj->addMenuItem(label, symbol);
    }
}

void loadEntertainmentCenterInitialContactMenu(EntertainmentStation* stationObj, menu* menuObj)
{
    for (int i = 0; i < stationObj->getNumSlotMachines(); ++i)
    {
        chtype slotSymbol = { stationObj->getSlotMachine(i)->getMachineColor(), '$' };
        menuObj->addMenuItem("SLOTS", slotSymbol);
    }

    for (int i = 0; i < stationObj->getNumDiamondMachines(); ++i)
    {
        chtype diamondSymbol = { stationObj->getDiamondsMachine(i)->getMachineColor(), 4 };
        menuObj->addMenuItem("DIAMONDS", diamondSymbol);
    }
}

void loadStationBuyModulesMenu(station* stationObj, menu* menuObj)
{
    const int numModules = stationObj->getNumModulesForTrade();

    for (int i = 0; i < numModules; ++i)
    {
        Module* mod = stationObj->getModuleForTrade(i);
        std::string label;
        chtype sym = blank_ch;

        switch (mod->getModuleType())
        {
        case MODULE_WEAPON:
            label = mod->getWeaponStruct().name_modifier +
                " (POWER " + int2String(mod->getFillQuantity() /
                    getWeaponModuleConsumptionPerTurn(mod)) + ")";
            sym = mod->getWeaponStruct().disp_chtype;
            break;

        case MODULE_ENGINE:
            label = mod->getEngineStruct().name_modifier;
            sym = mod->getEngineStruct().disp_chtype;
            break;

        case MODULE_SHIELD:
            label = mod->getShieldStruct().name_modifier;
            sym = mod->getShieldStruct().disp_chtype;
            break;

        case MODULE_CREW:
            label = "Crew Pod (MAX " + int2String(mod->getMaxFillQuantity()) + ")";
            sym = crewpod_symbol;
            break;

        case MODULE_FUEL:
            label = "Fuel Tank (MAX " + int2String(mod->getMaxFillQuantity()) + ")";
            sym = fueltank_symbol;
            break;

        default:
            continue;
        }

        label += ": " + uint642String(stationObj->getModuleForTradeCost(i)) + " CRED";
        menuObj->addMenuItem(label, sym);
    }

    menuObj->addMenuItem("[back]", blank_ch);
}

void loadStationSellModulesMenu(station* stationObj, menu* menuObj, MobShip* playerShip)
{
    const int numMods = playerShip->getNumInstalledModules();

    for (int i = 0; i < numMods; ++i)
    {
        Module* mod = playerShip->getModule(i);
        std::string label;
        chtype sym = blank_ch;

        switch (mod->getModuleType())
        {
        case MODULE_WEAPON:
            label = mod->getWeaponStruct().name_modifier +
                " (POWER " + int2String(mod->getFillQuantity() /
                    getWeaponModuleConsumptionPerTurn(mod)) + ")";
            sym = mod->getWeaponStruct().disp_chtype;
            break;

        case MODULE_ENGINE:
            label = mod->getEngineStruct().name_modifier;
            sym = mod->getEngineStruct().disp_chtype;
            break;

        case MODULE_SHIELD:
            label = mod->getShieldStruct().name_modifier;
            sym = mod->getShieldStruct().disp_chtype;
            break;

        case MODULE_CREW:
            label = "Crew Pod (" + int2String(mod->getFillQuantity()) + "/" +
                int2String(mod->getMaxFillQuantity()) + ")";
            sym = crewpod_symbol;
            break;

        case MODULE_FUEL:
            label = "Fuel Tank (" + int2String(mod->getFillQuantity()) + "/" +
                int2String(mod->getMaxFillQuantity()) + ")";
            sym = fueltank_symbol;
            break;

        default:
            continue;
        }

        label += ": " + uint642String(stationObj->getModuleSellCost(mod)) + " CRED";
        menuObj->addMenuItem(label, sym);
    }

    menuObj->addMenuItem("[back]", blank_ch);
}

void setStationContactData(race *race_obj, station *station_obj, menu *menu_obj, MobShip *player_ship, int menu_lev)
{
    menu_obj->setMenuLevel(menu_lev);
    menu_obj->clearMenuItems();
    menu_obj->clearMenuMainText();
    menu_obj->addMenuMainText("--- " + race_obj->getNameString() + " Galactic Ship Station ---");

    switch(menu_obj->getMenuLevel())
    {
        case(0):
            loadStationInitialContactMenu(station_obj,menu_obj);
            break;
        case(1):
            loadStationBuyModulesMenu(station_obj,menu_obj);
            break;
        case(2):
            menu_obj->setView(menu_obj->getLoc(), getLargeStationMenuSize());
            loadStationSellModulesMenu(station_obj,menu_obj,player_ship);
            break;
        default:
            break;
    }
    menu_obj->addMenuItem("[leave]",blank_ch);
}

void setEntertainmentCenterContactData(race* race_obj, EntertainmentStation * station_obj, menu* menu_obj, int menu_lev)
{
    menu_obj->setMenuLevel(menu_lev);
    menu_obj->clearMenuItems();
    menu_obj->clearMenuMainText();
    menu_obj->addMenuMainText("--- " + race_obj->getNameString() + " Cosmic Entertainment Center ---");

    switch (menu_obj->getMenuLevel())
    {
        case(0):
            loadEntertainmentCenterInitialContactMenu(station_obj, menu_obj);
            break;
        default:
            break;
    }
    menu_obj->addMenuItem("[leave]", blank_ch);
}

void addInitialContactScenarios(ContactTree& tree, race* controlRace, race* nativeRace)
{
    tree.scenarios[0] = ContactScenario
    {
         .id = 0,
         .message = "<<<<<INCOMING PRERECORDED TRANSMISSION>>>>>",
         .menuOptions = {"[continue]"},
         .nextScenarioIDs = {1}
    };

    tree.scenarios[1] = ContactScenario
    {
        .id = 1,
        .message = "!!!ATTENTION!!!",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {2}
    };

    if (controlRace->getRaceID() == nativeRace->getRaceID())
    {
        tree.scenarios[2] = ContactScenario
        {
            .id = 2,
            .message = "YOU ARE ENTERING " + capitalizeString(controlRace->getNameString()) + " TERRITORY",
            .menuOptions = {"[continue]"},
            .nextScenarioIDs = {3}
        };

        tree.scenarios[3] = ContactScenario
        {
            .id = 3,
            .message = "BE WARNED -- ALL ACTS OF HOSTILITY ARE PUNISHABLE BY DEATH",
            .menuOptions = {"[continue]"},
            .nextScenarioIDs = {4}
        };
    }
    else
    {
        tree.scenarios[2] = ContactScenario
        {
            .id = 2,
            .message = "THIS IS " + capitalizeString(controlRace->getNameString()) + " TERRITORY NOW",
            .menuOptions = {"[continue]"},
            .nextScenarioIDs = {3}
        };
        tree.scenarios[3] = ContactScenario
        {
            .id = 3,
            .message = "THE " + capitalizeString(nativeRace->getNameString()) + " HAVE BEEN ENSLAVED",
            .menuOptions = {"[continue]"},
            .nextScenarioIDs = {4}
        };
    }

    tree.scenarios[4] = ContactScenario
    {
        .id = 4,
        .message = "<<<<<INCOMING LIVE TRANSMISSION>>>>>",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {5}
    };
}

ContactTree createFullNonHostileContactTree(race* controlRace, race* nativeRace, bool& enterSubArea, std::map<int, std::string>& raceIDNameMapDiscovered)
{
    ContactTree tree;

    tree.startingScenarioID = nativeRace->playerIdentifiedByRace() && (controlRace->getRaceID() == nativeRace->getRaceID()) ? 5 : 0;

    std::string raceNameString = controlRace->getNameString();

    addInitialContactScenarios(tree, controlRace, nativeRace);

    bool surrendered = controlRace->isSurrenderedToPlayer();

    tree.scenarios[5] = ContactScenario
    {
        .id = 5,
        .message = (surrendered ? "Greetings master." : "Greetings traveler."),
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {surrendered ? 8 : 6},
        .endConversation = false
    };

    tree.scenarios[6] = ContactScenario
    {
        .id = 6,
        .message = "What can The " + raceNameString + " do for you?",
        .menuOptions = {
            "Request Entry",
            "Trade",
            "Ask for Information",
            "Threaten",
            "Leave"
        },
        .nextScenarioIDs = {11, 12, 13, 14, 15},
        .endConversation = false
    };

    tree.scenarios[7] = ContactScenario
    {
        .id = 7,
        .message = "What else can The " + raceNameString + " do for you?",
        .menuOptions = {
            "Request Entry",
            "Trade",
            "Ask for Information",
            "Threaten",
            "Leave"
        },
        .nextScenarioIDs = {11, 12, 13, 14, 15},
        .endConversation = false
    };

    if (surrendered)
    {
        tree.scenarios[8] = ContactScenario{
            .id = 8,
            .message = "What can The " + raceNameString + " do for you?",
            .menuOptions = {
                "Request Entry",
                "Trade",
                "Command To Attack Region",
                "Leave"
            },
            .nextScenarioIDs = {11, 12, 9, 15},
            .endConversation = false
        };

        // Vector to preserve menu order of attackable race IDs
        std::vector<int> filteredAttackableRaceIDs;

        ContactScenario scenario9;
        scenario9.id = 9;
        scenario9.message = "Who do you want us to attack?";
        scenario9.endConversation = false;

        for (const auto& [raceID, name] : raceIDNameMapDiscovered)
        {
            if (raceID != controlRace->getRaceID())
            {
                filteredAttackableRaceIDs.push_back(raceID);
                scenario9.menuOptions.push_back(name);
                scenario9.nextScenarioIDs.push_back(10);
            }
        }

        // Add "Nevermind" option
        scenario9.menuOptions.push_back("[Nevermind - go back].");
        scenario9.nextScenarioIDs.push_back(8);

        tree.scenarios[9] = std::move(scenario9);

        tree.scenarios[10] = ContactScenario{
            .id = 10,
            .message = "As you wish.",
            .menuOptions = {"[continue]"},
            .nextScenarioIDs = {11},
            .onSelectCallback = [controlRace, &enterSubArea, filteredAttackableRaceIDs](int selectedIndex)
             {         
                if (selectedIndex < static_cast<int>(filteredAttackableRaceIDs.size()))
                {
                    int targetRaceID = filteredAttackableRaceIDs[selectedIndex];
                    controlRace->setRaceIDCommandedToAttack(targetRaceID);
                    controlRace->setRaceAttStatus(targetRaceID, std::min(-15, controlRace->getRaceAttStatus(targetRaceID) - 15));
                    playerHasMoved = true;
                }
                else
                {
                    controlRace->setRaceIDCommandedToAttack(-1);
                }
                enterSubArea = false;
             },
            .endConversation = true
        };
    }

    tree.scenarios[11] = ContactScenario
    {
        .id = 11,
        .message = "You may enter.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .onSelectCallback = [&enterSubArea](int) {
            enterSubArea = true;
        },
        .endConversation = true
    };

    tree.scenarios[12] = ContactScenario
    {
        .id = 12,
        .message = "We offer modules, fuel, and repair services.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .onSelectCallback = [&enterSubArea](int) {
            enterSubArea = true;
        },
        .endConversation = true
    };

    tree.scenarios[13] = ContactScenario
    {
        .id = 13,
        .message = "We don't have any information to give you, unfortunately.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {7},
        .endConversation = false
    };

    tree.scenarios[14] = ContactScenario
    {
        .id = 14,
        .message = "Who sent you!? Nevermind that... You won't get away alive.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .onSelectCallback = [&enterSubArea, controlRace](int) {
            controlRace->setPlayerAttStatus(std::min(-1, controlRace->getPlayerAttStatus() - 1));
            enterSubArea = true;
        },
        .endConversation = true
    };

    tree.scenarios[15] = ContactScenario
    {
        .id = 15,
        .message = "Bye.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .endConversation = true
    };

    return tree;
}

ContactTree createFullHostileContactTree(race *controlRace, race *nativeRace, bool& enterSubArea)
{
    ContactTree tree;

    tree.startingScenarioID = nativeRace->playerIdentifiedByRace() && (controlRace->getRaceID() == nativeRace->getRaceID()) ? 5 : 0;

    std::string raceNameStringNative = nativeRace->getNameString();
    std::string raceNameStringController = controlRace->getNameString();

    std::string raceDomainStringNative = race_domain_suffix_string[(int)nativeRace->getRaceDomainType()];

    addInitialContactScenarios(tree, controlRace, nativeRace);

    tree.scenarios[5] = ContactScenario
    {
        .id = 5,
        .message = "Interloper...",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {6},
        .endConversation = false
    };

    if (controlRace->getRaceID() == nativeRace->getRaceID())
    {
        tree.scenarios[6] = ContactScenario
        {
            .id = 6,
            .message = "Your attempt to enter The " + raceNameStringNative + " " + raceDomainStringNative + " is objectionable.",
            .menuOptions = {"[continue]"},
            .nextScenarioIDs = {7},
            .endConversation = false
        };
    }
    else
    {
        tree.scenarios[6] = ContactScenario
        {
            .id = 6,
            .message = "The " + raceNameStringController + " control The " + raceNameStringNative + " " + raceDomainStringNative + " now.",
            .menuOptions = {"[continue]"},
            .nextScenarioIDs = {7},
            .endConversation = false
        };
    }

    tree.scenarios[7] = ContactScenario
    {
        .id = 7,
        .message = "Any further advancement *will* result in your ship being attacked.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {8},
        .endConversation = false
    };

    tree.scenarios[8] = ContactScenario
    {
        .id = 8,
        .message = "Back away from this area immediately.",
        .menuOptions = {
            "Request Entry",
            "Trade",
            "Ask for Information",
            "Threaten",
            "Leave"
        },
        .nextScenarioIDs = {9,10,11,12,13},
        .endConversation = false
    };

    tree.scenarios[9] = ContactScenario
    {
        .id = 9,
        .message = "Request denied.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {7},
        .endConversation = false
    };

    tree.scenarios[10] = ContactScenario
    {
        .id = 10,
        .message = "Absolutely not.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {8},
        .endConversation = false
    };

    tree.scenarios[11] = ContactScenario
    {
        .id = 11,
        .message = "We will not exchange information with you.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {8},
        .endConversation = false
    };

    tree.scenarios[12] = ContactScenario
    {
        .id = 12,
        .message = "Prepare to be destroyed.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .onSelectCallback = [&enterSubArea, controlRace](int) {
            controlRace->setPlayerAttStatus((int)std::min(-1,controlRace->getPlayerAttStatus() - 1));
            enterSubArea = true;
        },
        .endConversation = true
    };

    tree.scenarios[13] = ContactScenario
    {
        .id = 13,
        .message = "Do not return.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .endConversation = true
    };

    return tree;
}

ContactTree createSurrenderToPlayerContactTree(race* nativeRace)
{
    ContactTree tree;

    tree.startingScenarioID = 0;

    std::string raceNameStringNative = nativeRace->getNameString();

    tree.scenarios[0] = ContactScenario
    {
        .id = 0,
        .message = "INTERLOPER. THIS IS THE " + capitalizeString(raceNameStringNative) + ".",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {1},
        .endConversation = false
    };

    tree.scenarios[1] = ContactScenario
    {
        .id = 1,
        .message = "WE SURRENDER!!!",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {2},
        .endConversation = false
    };

    tree.scenarios[2] = ContactScenario
    {
        .id = 2,
        .message = "PLEASE! WE'LL DO ANYTHING YOU WANT. STOP ATTACKING US!",
        .menuOptions = {
            "I am your master now. You will obey me.",
            "No, we will not stop attacking you. Prepare to be annihilated."
        },
        .nextScenarioIDs = {3,4},
        .endConversation = false
    };

    tree.scenarios[3] = ContactScenario
    {
        .id = 3,
        .message = "FINE. YOUR WISH IS OUR COMMAND.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .onSelectCallback = [nativeRace](int) {
            nativeRace->setSurrenderedToPlayer(true);
         },
        .endConversation = true
    };

    tree.scenarios[4] = ContactScenario
    {
        .id = 4,
        .message = "NO!!! YOU WILL PAY FOR THIS!",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .endConversation = true
    };

    return tree;
}

ContactTree createCapturedRaceByPlayerFreeContactTree(race * controllerRace, race* nativeRace)
{
    ContactTree tree;

    tree.startingScenarioID = 0;

    std::string raceNameStringNative = nativeRace->getNameString();
    std::string raceNameStringController = controllerRace->getNameString();

    std::string domainNameController = race_domain_suffix_string[(int)controllerRace->getRaceDomainType()];

    tree.scenarios[0] = ContactScenario
    {
        .id = 0,
        .message = "<<<<<INCOMING LIVE TRANSMISSION>>>>>",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {1},
        .endConversation = false
    };

    tree.scenarios[1] = ContactScenario
    {
        .id = 1,
        .message = "This is The " + raceNameStringNative + " reporting.",
        .nextScenarioIDs = {2},
        .endConversation = false
    };

    tree.scenarios[2] = ContactScenario
    {
        .id = 2,
        .message = "Due to the situation in The " + raceNameStringController + " " + domainNameController + ", we are free once again.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {3},
        .endConversation = true
    };

    tree.scenarios[3] = ContactScenario
    {
        .id = 3,
        .message = "Your wish is our command.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .endConversation = true
    };

    return tree;
}
