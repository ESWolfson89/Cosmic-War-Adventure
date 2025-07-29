#include "station.h"

/*
Currently unused

// in initStationTradeModules:
sell_ratio = (double)(randInt(1,danger_level))/100.0;

// in station:
sell_ratio = 0.0;

bool station::hasStationChoice(basic_station_trade_choice bstc)
{
    for (int i = 0; i < (int)choices_lev0.size(); ++i)
    {
        if (bstc == choices_lev0[i])
            return true;
    }
    return false;
}

double station::getSellRatio()
{
    return sell_ratio;
}
*/

// constructor (do not call implicitly)
station::station()
{

}

// use this one
station::station(point loc, int dl)
{
    subarea_loc = loc;
    danger_level = dl;
    fuel_cost = repair_cost = hull_upgrade_cost = slot_upgrade_cost = crew_cost = 1ULL;
}

// get num modules for sale in "buy modules" option
int station::getNumModulesForTrade()
{
    return (int)modules_for_trade.size();
}

// get number of menu "level 0" space station choices (shown when initially docking in space station)
int station::getNumBasicStationTradeChoices()
{
    return (int)choices_lev0.size();
}

// get danger level of region space station is in
int station::getDangerLevel()
{
    return danger_level;
}

// get location of sub area on starmap that the space station is in
point station::getSubareaLoc()
{
    return subarea_loc;
}

// initialize space station fields
void station::initStation()
{
    initGenericStationServices();

    initStationBuyModules();

    initStationSellModules();
}

void station::initGenericStationServices()
{
    fuel_cost = (uint_64)danger_level * 2ULL;
    repair_cost = (uint_64)danger_level * 3ULL;
    hull_upgrade_cost = (uint_64)(danger_level * danger_level) * 100ULL;
    slot_upgrade_cost = (uint_64)(danger_level * danger_level) * 2500ULL;
    crew_cost = (uint_64)(danger_level * 10ULL);

    choices_lev0.push_back(STATIONCHOICE_FUEL);
    choices_lev0.push_back(STATIONCHOICE_HULLREPAIR);

    if (roll(2))
        choices_lev0.push_back(STATIONCHOICE_HULLUPGRADE);
    if (roll(3) && danger_level >= 10)
        choices_lev0.push_back(STATIONCHOICE_INCNUMMODULECAPACITY);
    if (roll(3))
        choices_lev0.push_back(STATIONCHOICE_HIRECREW);
}

void station::initStationBuyModules()
{
    for (int i = 1; i < NUM_TOTAL_WEAPON_TYPES; ++i)
    {
        if (rollPerc((int)std::min(100,procgen_weapons_base_chance[i]+danger_level)) && roll(3))
        {
            addWeaponModuleToStation(i); i--;
        }
    }

    for (int i = 1; i < NUM_TOTAL_SHIELD_TYPES; ++i)
    {
        if (rollPerc((int)std::min(100,procgen_shields_base_chance[i]+danger_level)) && roll(4))
        {
            addShieldModuleToStation(i); i--;
        }
    }

    for (int i = 1; i < NUM_TOTAL_ENGINE_TYPES; ++i)
    {
        if (rollPerc((int)std::min(100,procgen_engines_base_chance[i]+danger_level)) && roll(5))
        {
            addEngineModuleToStation(i); i--;
        }
    }

    for (int i = 1; i <= 6; ++i)
    {
        if (roll(10*i))
        {
            addCrewpodModuleToStation(i); i--;
        }
    }

    while(roll(7))
    {
        addFuelModuleToStation();
    }

    if ((int)modules_for_trade.size() > 0)
        choices_lev0.push_back(STATIONCHOICE_BUYMODULE);
}

void station::initStationSellModules()
{
    if (roll(4))
        choices_lev0.push_back(STATIONCHOICE_SELLMODULE);
}

void station::addWeaponModuleToStation(int i)
{
    int fill_usage = allbasicweapon_stats[i].consumption_rate*allbasicweapon_stats[i].num_shots;
    int fill_val = randInt(fill_usage,fill_usage*danger_level) + randInt(fill_usage,fill_usage*10);
    module mod_added = module(MODULE_WEAPON,fill_val,fill_val);
    mod_added.setWeaponStruct(allbasicweapon_stats[i]);
    uint_64 mod_cost = getModuleBuyCost(MODULE_WEAPON,(fill_val/fill_usage),mod_added.getBaseCost());
    modules_for_trade.push_back(std::make_pair(mod_added,mod_cost));
}

void station::addShieldModuleToStation(int i)
{
    int fill_val = allbasicshield_stats[i].base_num_layers;
    module mod_added = module(MODULE_SHIELD,fill_val,fill_val);
    mod_added.setShieldStruct(allbasicshield_stats[i]);
    uint_64 mod_cost = getModuleBuyCost(MODULE_SHIELD,fill_val,mod_added.getBaseCost());
    modules_for_trade.push_back(std::make_pair(mod_added,mod_cost));
}

void station::addEngineModuleToStation(int i)
{
    int fill_val = danger_level;
    module mod_added = module(MODULE_ENGINE,fill_val,fill_val);
    mod_added.setEngineStruct(allbasicengine_stats[i]);
    uint_64 mod_cost = getModuleBuyCost(MODULE_ENGINE,fill_val,mod_added.getBaseCost());
    modules_for_trade.push_back(std::make_pair(mod_added,mod_cost));
}

void station::addCrewpodModuleToStation(int i)
{
    int fill_val = 16*i;
    module mod_added = module(MODULE_CREW,0,fill_val);
    uint_64 mod_cost = getModuleBuyCost(MODULE_CREW,fill_val,mod_added.getBaseCost());
    modules_for_trade.push_back(std::make_pair(mod_added,mod_cost));
}

void station::addFuelModuleToStation()
{
    int fill_val = randInt(50,danger_level*20+50);
    module mod_added = module(MODULE_FUEL,0,fill_val);
    uint_64 mod_cost = getModuleBuyCost(MODULE_FUEL,fill_val,mod_added.getBaseCost());
    modules_for_trade.push_back(std::make_pair(mod_added,mod_cost));
}

void station::cleanupEverything()
{
    std::vector<std::pair<module,uint_64>>().swap(modules_for_trade);
    std::vector<basic_station_trade_choice>().swap(choices_lev0);
}

void station::eraseModule(int i)
{
    if (i >= getNumModulesForTrade())
        return;

    modules_for_trade.erase(modules_for_trade.begin() + i);
}

uint_64 station::getFuelCost()
{
    return fuel_cost;
}

uint_64 station::getHullFixCost()
{
    return repair_cost;
}

uint_64 station::getHullUpgradeCost()
{
    return hull_upgrade_cost;
}

uint_64 station::getSlotCapUpgradeCost()
{
    return slot_upgrade_cost;
}

uint_64 station::getCrewCost()
{
    return crew_cost;
}

uint_64 station::getModuleForTradeCost(int i)
{
    return modules_for_trade[i].second;
}

uint_64 station::getModuleBuyCost(module_type mt, int power_val, int base_cost)
{
    uint_64 cost = 1ULL;

    switch(mt)
    {
        case(MODULE_WEAPON):
        case(MODULE_SHIELD):
        {
            cost *= (uint_64)(base_cost*power_val);
            break;
        }
        case(MODULE_ENGINE):
        {
            cost *= (uint_64)(base_cost*danger_level);
            break;
        }
        case(MODULE_FUEL):
        {
            cost *= (uint_64)power_val * 300ULL;
            break;
        }
        case(MODULE_CREW):
        {
            cost *= (uint_64)power_val * 1000ULL + 8000ULL;
            break;
        }
        default:
            break;
    }

    return cost;
}

uint_64 station::getModuleSellCost(module *m)
{
    uint_64 buy_cost = getModuleBuyCost(m->getModuleType(),m->getMaxFillQuantity(),m->getBaseCost());
    return buy_cost/((uint_64)(30 - danger_level));
}

module *station::getModuleForTrade(int i)
{
    return &(modules_for_trade[i].first);
}

basic_station_trade_choice station::getBasicStationTradeChoice(int i)
{
    return choices_lev0[i];
}

void station::save(std::ofstream& os) const
{
    os.write(reinterpret_cast<const char*>(&fuel_cost), sizeof(uint_64));
    os.write(reinterpret_cast<const char*>(&repair_cost), sizeof(uint_64));
    os.write(reinterpret_cast<const char*>(&hull_upgrade_cost), sizeof(uint_64));
    os.write(reinterpret_cast<const char*>(&slot_upgrade_cost), sizeof(uint_64));
    os.write(reinterpret_cast<const char*>(&crew_cost), sizeof(uint_64));
    os.write(reinterpret_cast<const char*>(&danger_level), sizeof(int));
    subarea_loc.save(os);

    // Save modules_for_trade
    int tradeCount = static_cast<int>(modules_for_trade.size());
    os.write(reinterpret_cast<const char*>(&tradeCount), sizeof(int));
    for (const auto& pair : modules_for_trade) {
        pair.first.save(os);                      // module
        os.write(reinterpret_cast<const char*>(&pair.second), sizeof(uint_64));  // cost
    }

    // Save choices_lev0
    int choiceCount = static_cast<int>(choices_lev0.size());
    os.write(reinterpret_cast<const char*>(&choiceCount), sizeof(int));
    for (const auto& choice : choices_lev0)
        os.write(reinterpret_cast<const char*>(&choice), sizeof(basic_station_trade_choice));
}

void station::load(std::ifstream& is)
{
    is.read(reinterpret_cast<char*>(&fuel_cost), sizeof(uint_64));
    is.read(reinterpret_cast<char*>(&repair_cost), sizeof(uint_64));
    is.read(reinterpret_cast<char*>(&hull_upgrade_cost), sizeof(uint_64));
    is.read(reinterpret_cast<char*>(&slot_upgrade_cost), sizeof(uint_64));
    is.read(reinterpret_cast<char*>(&crew_cost), sizeof(uint_64));
    is.read(reinterpret_cast<char*>(&danger_level), sizeof(int));
    subarea_loc.load(is);

    // Load modules_for_trade
    int tradeCount = 0;
    is.read(reinterpret_cast<char*>(&tradeCount), sizeof(int));
    modules_for_trade.resize(tradeCount);
    for (int i = 0; i < tradeCount; ++i) {
        modules_for_trade[i].first.load(is);
        is.read(reinterpret_cast<char*>(&modules_for_trade[i].second), sizeof(uint_64));
    }

    // Load choices_lev0
    int choiceCount = 0;
    is.read(reinterpret_cast<char*>(&choiceCount), sizeof(int));
    choices_lev0.resize(choiceCount);
    for (int i = 0; i < choiceCount; ++i)
        is.read(reinterpret_cast<char*>(&choices_lev0[i]), sizeof(basic_station_trade_choice));
}

// constructor (do not call implicitly)
EntertainmentStation::EntertainmentStation()
{

}

// use this one
EntertainmentStation::EntertainmentStation(point loc, int dl)
{
    subarea_loc = loc;
    danger_level = dl;
}

void EntertainmentStation::initStation()
{
    do
    {
        addSlotMachines();
        addDiamondMachines();
    } while (getNumOptions() == 0);
}

void EntertainmentStation::addSlotMachines()
{
    int numMachines = randInt(0, randInt(1, randInt(1, 6)));

    for (int i = 0; i < numMachines; i++)
    {
        slotMachines.push_back(slot(subarea_loc, std::min(20, (int)randIntZ(5 + (int)((danger_level + 5) / 2)) + 1), 0));
        slotMachines[slotMachines.size() - 1].initSlots();
    }
}

void EntertainmentStation::addDiamondMachines()
{
    int numMachines = randInt(0, randInt(1, randInt(1, 6)));

    for (int i = 0; i < numMachines; i++)
    {
        diamondMachines.push_back(diamond(subarea_loc, std::min(20, (int)randIntZ(5 + (int)((danger_level + 5) / 2)) + 1), 0));
        diamondMachines[diamondMachines.size() - 1].initDiamonds();
    }
}

void EntertainmentStation::save(std::ofstream& os) const
{
    os.write(reinterpret_cast<const char*>(&danger_level), sizeof(int));
    subarea_loc.save(os);

    // Save slot machines
    int slotCount = static_cast<int>(slotMachines.size());
    os.write(reinterpret_cast<const char*>(&slotCount), sizeof(int));
    for (const auto& s : slotMachines)
        s.save(os);

    // Save diamond machines
    int diamondCount = static_cast<int>(diamondMachines.size());
    os.write(reinterpret_cast<const char*>(&diamondCount), sizeof(int));
    for (const auto& d : diamondMachines)
        d.save(os);
}

void EntertainmentStation::load(std::ifstream& is)
{
    is.read(reinterpret_cast<char*>(&danger_level), sizeof(int));
    subarea_loc.load(is);

    // Load slot machines
    int slotCount = 0;
    is.read(reinterpret_cast<char*>(&slotCount), sizeof(int));
    slotMachines.resize(slotCount);
    for (int i = 0; i < slotCount; ++i)
        slotMachines[i].load(is);

    // Load diamond machines
    int diamondCount = 0;
    is.read(reinterpret_cast<char*>(&diamondCount), sizeof(int));
    diamondMachines.resize(diamondCount);
    for (int i = 0; i < diamondCount; ++i)
        diamondMachines[i].load(is);
}

slot * EntertainmentStation::getSlotMachine(int i)
{
    return &slotMachines[i];
}

diamond* EntertainmentStation::getDiamondsMachine(int i)
{
    return &diamondMachines[i];
}

// get danger level of region space station is in
int EntertainmentStation::getDangerLevel()
{
    return danger_level;
}

int EntertainmentStation::getNumSlotMachines()
{
    return (int)slotMachines.size();
}

int EntertainmentStation::getNumDiamondMachines()
{
    return (int)diamondMachines.size();
}

int EntertainmentStation::getNumOptions()
{
    return (int)slotMachines.size() + (int)diamondMachines.size();
}

// get location of sub area on starmap that the space station is in
point EntertainmentStation::getSubareaLoc()
{
    return subarea_loc;
}