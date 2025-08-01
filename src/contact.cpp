#include "contact.h"


void loadStationInitialContactMenu(station *station_obj, menu *menu_obj)
{
    for (int i = 0; i < station_obj->getNumBasicStationTradeChoices(); ++i)
    {
        switch(station_obj->getBasicStationTradeChoice(i))
        {
            case(STATIONCHOICE_FUEL):
            {
                menu_obj->addMenuItem("Buy Fuel: " + uint642String(station_obj->getFuelCost()) + " CRED", fuelupgrade_symbol);
                break;
            }
            case(STATIONCHOICE_HULLREPAIR):
            {
                menu_obj->addMenuItem("Repair Hull: " + uint642String(station_obj->getHullFixCost()) + " CRED",hull_repair_symbol);
                break;
            }
            case(STATIONCHOICE_HULLUPGRADE):
            {
                menu_obj->addMenuItem("Upgrade Hull: " + uint642String(station_obj->getHullUpgradeCost()) + " CRED",hull_upgrade_symbol);
                break;
            }
            case(STATIONCHOICE_INCNUMMODULECAPACITY):
            {
                menu_obj->addMenuItem("Upgrade Slot Capacity: " + uint642String(station_obj->getSlotCapUpgradeCost()) + " CRED",slot_upgrade_symbol);
                break;
            }
            case(STATIONCHOICE_HIRECREW):
                menu_obj->addMenuItem("Hire Crew: " + uint642String(station_obj->getCrewCost()) + " CRED",crew_upgrade_symbol);
                break;
            case(STATIONCHOICE_BUYMODULE):
                menu_obj->addMenuItem("Buy modules",module_buy_symbol);
                break;
            case(STATIONCHOICE_SELLMODULE):
                menu_obj->addMenuItem("Sell modules",module_sell_symbol);
                break;
            default:
                break;
        }
    }
}

void loadEntertainmentCenterInitialContactMenu(EntertainmentStation * station_obj, menu * menu_obj)
{
    chtype slotSymbol;
    chtype diamondSymbol;

    for (int i = 0; i < station_obj->getNumSlotMachines(); ++i)
    {
        slotSymbol = { station_obj->getSlotMachine(i)->getMachineColor() , '$'};
        menu_obj->addMenuItem("SLOTS ", slotSymbol);
    }

    for (int i = 0; i < station_obj->getNumDiamondMachines(); ++i)
    {
        diamondSymbol = { station_obj->getDiamondsMachine(i)->getMachineColor() , 4 };
        menu_obj->addMenuItem("DIAMONDS ", diamondSymbol);
    }
}

void loadStationBuyModulesMenu(station *station_obj, menu *menu_obj)
{
    std::string added_module_string = "";
    chtype added_module_chtype = blank_ch;

    for (int i = 0; i < station_obj->getNumModulesForTrade(); ++i)
    {
        added_module_string = "";
        switch(station_obj->getModuleForTrade(i)->getModuleType())
        {
            case(MODULE_WEAPON):
                added_module_string += "<WEAPON> " + station_obj->getModuleForTrade(i)->getWeaponStruct().name_modifier + " (POWER " +
                                                     int2String((int)(station_obj->getModuleForTrade(i)->getFillQuantity()/getWeaponModuleConsumptionPerTurn(station_obj->getModuleForTrade(i)))) + ")";
                added_module_chtype = station_obj->getModuleForTrade(i)->getWeaponStruct().disp_chtype;
                break;
            case(MODULE_ENGINE):
                added_module_string += "<ENGINE> " + station_obj->getModuleForTrade(i)->getEngineStruct().name_modifier;
                added_module_chtype = station_obj->getModuleForTrade(i)->getEngineStruct().disp_chtype;
                break;
            case(MODULE_SHIELD):
                added_module_string += "<SHIELD> " + station_obj->getModuleForTrade(i)->getShieldStruct().name_modifier;
                added_module_chtype = station_obj->getModuleForTrade(i)->getShieldStruct().disp_chtype;
                break;
            case(MODULE_CREW):
                added_module_string += "Crew Pod (MAX " + int2String((int)(station_obj->getModuleForTrade(i)->getMaxFillQuantity())) + ")";
                added_module_chtype = crewpod_symbol;
                break;
            case(MODULE_FUEL):
                added_module_string += "Fuel Tank (MAX " + int2String((int)(station_obj->getModuleForTrade(i)->getMaxFillQuantity())) + ")";
                added_module_chtype = fueltank_symbol;
                break;
            default:
                break;
        }
        added_module_string += ": " + uint642String(station_obj->getModuleForTradeCost(i)) + " CRED";
        menu_obj->addMenuItem(added_module_string,added_module_chtype);
    }
    menu_obj->addMenuItem("[back]",blank_ch);
}

void loadStationSellModulesMenu(station *station_obj, menu *menu_obj, MobShip *player_ship)
{
    std::string added_module_string = "";
    chtype added_module_chtype = blank_ch;

    for (int i = 0; i < player_ship->getNumInstalledModules(); ++i)
    {
        added_module_string = "";
        switch(player_ship->getModule(i)->getModuleType())
        {
            case(MODULE_WEAPON):
                added_module_string += "<WEAPON> " + player_ship->getModule(i)->getWeaponStruct().name_modifier + " (POWER " +
                                                     int2String((int)(player_ship->getModule(i)->getFillQuantity()/getWeaponModuleConsumptionPerTurn(player_ship->getModule(i)))) + ")";
                added_module_chtype = player_ship->getModule(i)->getWeaponStruct().disp_chtype;
                break;
            case(MODULE_ENGINE):
                added_module_string += "<ENGINE> " + player_ship->getModule(i)->getEngineStruct().name_modifier;
                added_module_chtype = player_ship->getModule(i)->getEngineStruct().disp_chtype;
                break;
            case(MODULE_SHIELD):
                added_module_string += "<SHIELD> " + player_ship->getModule(i)->getShieldStruct().name_modifier;
                added_module_chtype = player_ship->getModule(i)->getShieldStruct().disp_chtype;
                break;
            case(MODULE_CREW):
                added_module_string += "Crew Pod (" + int2String((int)(player_ship->getModule(i)->getFillQuantity())) +
                                       "/" + int2String((int)(player_ship->getModule(i)->getMaxFillQuantity())) + ")";
                added_module_chtype = crewpod_symbol;
                break;
            case(MODULE_FUEL):
                added_module_string += "Fuel Tank (" + int2String((int)(player_ship->getModule(i)->getFillQuantity())) +
                                       "/" + int2String((int)(player_ship->getModule(i)->getMaxFillQuantity())) + ")";
                added_module_chtype = fueltank_symbol;
                break;
            default:
                break;
        }
        added_module_string += ": " + uint642String(station_obj->getModuleSellCost(player_ship->getModule(i))) + " CRED";
        menu_obj->addMenuItem(added_module_string,added_module_chtype);
    }
    menu_obj->addMenuItem("[back]",blank_ch);
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

ContactTree createFullNonHostileContactTree(race* controlRace, race* nativeRace, bool& enterSubArea)
{
    ContactTree tree;

    tree.startingScenarioID = nativeRace->playerIdentifiedByRace() && (controlRace->getRaceID() == nativeRace->getRaceID()) ? 5 : 0;

    std::string raceNameString = controlRace->getNameString();

    addInitialContactScenarios(tree, controlRace, nativeRace);
    
    tree.scenarios[5] = ContactScenario
    {
        .id = 5,
        .message = "\"Welcome traveler...\"",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {6},
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
        .nextScenarioIDs = {8,9,10,11,12},
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
        .nextScenarioIDs = {8,9,10,11,12},
        .endConversation = false
    };

    tree.scenarios[8] = ContactScenario
    {
        .id = 8,
        .message = "You may enter.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .onSelectCallback = [&enterSubArea]() {
            enterSubArea = true;
         },
        .endConversation = true
    };

    tree.scenarios[9] = ContactScenario
    {
        .id = 9,
        .message = "We offer modules, fuel, and repair services.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .onSelectCallback = [&enterSubArea]() {
            enterSubArea = true;
         },
        .endConversation = true
    };

    tree.scenarios[10] = ContactScenario
    {
        .id = 10,
        .message = "We don't have any information to give you, unfortunately.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {7},
        .endConversation = false
    };

    tree.scenarios[11] = ContactScenario
    {
        .id = 11,
        .message = "Who sent you!? Nevermind that... You won't get away alive.",
        .menuOptions = {"[continue]"},
        .nextScenarioIDs = {-1},
        .onSelectCallback = [&enterSubArea, controlRace]() {
            controlRace->setPlayerAttStatus((int)std::min(-1,controlRace->getPlayerAttStatus() - 1));
            enterSubArea = true;
        },
        .endConversation = true
    };

    tree.scenarios[12] = ContactScenario
    {
        .id = 12,
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
        .message = "\"Interloper...\"",
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
        .onSelectCallback = [&enterSubArea, controlRace]() {
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
