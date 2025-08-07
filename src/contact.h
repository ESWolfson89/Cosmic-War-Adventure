#ifndef CONTACT_H_
#define CONTACT_H_

#include "menu.h"
#include "race.h"
#include "station.h"
#include "mob.h"
#include <functional>

struct ContactScenario
{
    int id = 0;
    std::string message;
    std::vector<std::string> menuOptions;
    std::vector<int> nextScenarioIDs;
    std::function<void()> onSelectCallback = nullptr;
    bool endConversation = false;
};

class ContactTree
{
public:
    std::unordered_map<int, ContactScenario> scenarios;
    int startingScenarioID = 0;

    const ContactScenario& getScenario(int id) const
    {
        return scenarios.at(id);
    }
};

ContactTree createFullNonHostileContactTree(race*, race*, bool&);

ContactTree createFullHostileContactTree(race*, race*, bool&);

ContactTree createSurrenderToPlayerContactTree(race* nativeRace);

ContactTree createCapturedRaceByPlayerFreeContactTree(race*, race*);

void addInitialContactScenarios(ContactTree& tree, race *, race *);

void setStationContactData(race *, station *, menu *, MobShip *, int);

void setEntertainmentCenterContactData(race *, EntertainmentStation *, menu *, int);

void loadStationInitialContactMenu(station *, menu *);

void loadEntertainmentCenterInitialContactMenu(EntertainmentStation *, menu *);

void loadStationBuyModulesMenu(station *, menu *);

void loadStationSellModulesMenu(station *, menu *, MobShip *);

#endif
