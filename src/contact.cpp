#include "contact.h"

void loadContactMainTextSequence(menu *menu_obj, race *race_obj, std::string initial_text, int num_lines, int file_line_offset)
{
    menu_obj->addMenuMainText(initial_text);

    for (int i = 0; i < num_lines; ++i)
    {
        menu_obj->addMenuMainText("\"" + loadConverseStringFromFile(race_obj->getNameString(), i + file_line_offset + 1, race_obj->getPlayerAttStatus()) + "\"");
    }
}

void loadDiscoveryContactMenuText(race *race_obj, menu *menu_obj, entrance_contact_struct *ecs_obj)
{
    if (!race_obj->playerIdentifiedByRace())
    {
        loadContactMainTextSequence(menu_obj,race_obj,prerecorded_message_string,ecs_obj->num_prerecorded_lines,0);
    }
}

void loadCurrentContactMenuText(race *race_obj, menu *menu_obj, entrance_contact_struct *ecs_obj)
{
    switch(menu_obj->getMenuLevel())
    {
        case(0):
        {
            loadContactMainTextSequence(menu_obj, race_obj, live_message_string, ecs_obj->num_welcome_lines, ecs_obj->num_prerecorded_lines);
            menu_obj->addMenuItem("[continue]", blank_ch);
            race_obj->setPlayerIDByRaceStatus(true);
            break;
        }
        case(1):
        {
            for (int i = 0; i < ecs_obj->num_converse_choices; ++i)
            {
                menu_obj->addMenuItem(contact_menu_options_type1[i], blank_ch);
            }
            menu_obj->addMenuItem("[leave]", blank_ch);
            break;
        }
        case(2):
        {
            menu_obj->clearMenuMainText();
            menu_obj->addMenuMainText("\"" + loadConverseStringFromFile(race_obj->getNameString(),
                ecs_obj->num_prerecorded_lines + ecs_obj->num_welcome_lines +
                menu_obj->getSelectionIndex() + 1, race_obj->getPlayerAttStatus()) + "\"");
            menu_obj->addMenuItem("[continue]", blank_ch);
            break;
        }        
        default:
        {
            break;
        }
    }
}

void setEntranceContactData(race *race_obj, menu *menu_obj, int new_menu_lev)
{
    if (new_menu_lev <= MAX_CONTACT_MENU_LEVEL)
    {
        menu_obj->setMenuLevel(new_menu_lev);
        menu_obj->clearMenuItems();

        loadDiscoveryContactMenuText(race_obj,menu_obj,race_obj->getEntranceContactStruct());
        loadCurrentContactMenuText(race_obj,menu_obj,race_obj->getEntranceContactStruct());
    }
}

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

std::string loadConverseStringFromFile(std::string race_str, int converse_line, int atp)
{
    int line_iter = 0;
    std::string ret_val = "";
    std::string token = "";
    std::ifstream string_file;
    if (atp >= 0)
        string_file.open("converse1.txt");
    else
        string_file.open("converse2.txt");
    // first token
    string_file >> token;
    // first @
    string_file >> token;
    do
    {
        // reset token
        token = "";
        ret_val = "";
        // check for delimiter
        while (token != "@" && token != "[EOF_STRING]")
        {
            if (token == "<caprace>")
                ret_val += capitalizeString(race_str);
            else if (token == "<race>")
                ret_val += race_str;
            else
                ret_val += token;
            ret_val += " ";
            string_file >> token;
        }
        line_iter++;
    } while (line_iter < converse_line);
    string_file.close();
    ret_val.erase(ret_val.begin());
    ret_val.pop_back();
    return ret_val;
}
