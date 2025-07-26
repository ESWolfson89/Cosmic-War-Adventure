#ifndef STATION_H_
#define STATION_H_

#include "mob.h"
#include "casino.h"

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

enum basic_station_trade_choice
{
    STATIONCHOICE_FUEL,
    STATIONCHOICE_HULLREPAIR,
    STATIONCHOICE_HULLUPGRADE,
    STATIONCHOICE_INCNUMMODULECAPACITY,
    STATIONCHOICE_HIRECREW,
    STATIONCHOICE_BUYMODULE,
    STATIONCHOICE_SELLMODULE
};

class station
{
    public:
        //unused: bool hasStationChoice(basic_station_trade_choice);
        //unused: double getSellRatio();
        station();
        station(point, int);
        int getNumModulesForTrade();
        int getNumBasicStationTradeChoices();
        int getDangerLevel();
        void initStation();
        void initGenericStationServices();
        void initStationBuyModules();
        void initStationSellModules();
        void addWeaponModuleToStation(int);
        void addShieldModuleToStation(int);
        void addEngineModuleToStation(int);
        void addCrewpodModuleToStation(int);
        void addFuelModuleToStation();
        void cleanupEverything();
        void eraseModule(int i);
        basic_station_trade_choice getBasicStationTradeChoice(int);
        module *getModuleForTrade(int);
        uint_64 getModuleForTradeCost(int);
        uint_64 getFuelCost();
        uint_64 getHullFixCost();
        uint_64 getHullUpgradeCost();
        uint_64 getSlotCapUpgradeCost();
        uint_64 getCrewCost();
        uint_64 getModuleSellCost(module *);
        uint_64 getModuleBuyCost(module_type, int, int);
        point getSubareaLoc();
    private:
        // unused: double sell_ratio;
        std::vector<std::pair<module,uint_64>> modules_for_trade;
        std::vector<basic_station_trade_choice> choices_lev0;
        uint_64 fuel_cost;
        uint_64 repair_cost;
        uint_64 hull_upgrade_cost;
        uint_64 slot_upgrade_cost;
        uint_64 crew_cost;
        int danger_level;
        point subarea_loc;
};

class EntertainmentStation
{
    public:
        //unused: bool hasStationChoice(basic_station_trade_choice);
        //unused: double getSellRatio();
        EntertainmentStation();
        EntertainmentStation(point, int);
        int getDangerLevel();
        int getNumSlotMachines();
        int getNumDiamondMachines();
        point getSubareaLoc();
        void initStation();
        void addSlotMachines();
        void addDiamondMachines();
        slot * getSlotMachine(int);
        diamond * getDiamondsMachine(int);
        int getNumOptions();
    private:
        int danger_level;
        point subarea_loc;
        std::vector<EntertainmentType> entertainmentTypes;
        std::vector<slot> slotMachines;
        std::vector<diamond> diamondMachines;
};

#endif
