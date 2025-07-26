#ifndef CONTACT_H_
#define CONTACT_H_

#include "menu.h"
#include "race.h"
#include "station.h"
#include "mob.h"

#define MAX_CONTACT_MENU_LEVEL 2

enum enter_subarea_event
{
    ENTERSUBAREA_ENTER,
    ENTERSUBAREA_CONVERSE,
    ENTERSUBAREA_IGNORE
};

enum converse_event
{
    CONVERSE_ENTERSUBAREA,
    CONVERSE_SERVICE,
    CONVERSE_INFORMATION,
    CONVERSE_FIGHT,
    CONVERSE_EXIT
};

static std::string contact_menu_options_type1[4] =
{
    "[Request Entry]",
    "[Trade]",
    "[Ask For Information]",
    "[Intimidate]"
};

static std::string contact_menu_options_type2[2] =
{
    "\"Who are you at war with?\"",
    "\"Who are your allies?\""
};

static std::string prerecorded_message_string = "<<<<<INCOMING PRERECORDED TRANSMISSION>>>>>";

static std::string live_message_string = "<<<<<INCOMING LIVE TRANSMISSION>>>>>";


void loadContactMainTextSequence(menu *, race *, std::string, int, int);

void loadDiscoveryContactMenuText(race *, menu *, entrance_contact_struct *);

void loadCurrentContactMenuText(race *, menu *, entrance_contact_struct *);

void setEntranceContactData(race *, menu *, int);

void setStationContactData(race *, station *, menu *, MobShip *, int);

void setEntertainmentCenterContactData(race *, EntertainmentStation *, menu *, int);

void loadStationInitialContactMenu(station *, menu *);

void loadEntertainmentCenterInitialContactMenu(EntertainmentStation *, menu *);

void loadStationBuyModulesMenu(station *, menu *);

void loadStationSellModulesMenu(station *, menu *, MobShip *);

std::string loadConverseStringFromFile(std::string, int, int);

#endif
