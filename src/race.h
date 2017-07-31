#ifndef RACE_H_
#define RACE_H_

#include "mob.h"
#include "namegen.h"
#include <map>

/*
Extra stuff not needed yet (ever?)

private:
std::vector<ship_mob> extra_ships;

public:
int getNumExtraShips();
ship_mob *getExtraShip(int);
void generateOneProcgenExtraShip(int, shipmob_classtype);
void generateOneGuaranteedExtraShip(mob_t);
void generateExtraShips();
void addExtraShip(ship_mob *);

void race::addExtraShip(ship_mob *added_ship)
{
    extra_ships.push_back(*added_ship);
}

void race::generateOneGuaranteedExtraShip(mob_t mt)
{
    extra_ships.push_back(ship_mob(false,allshipmob_data[(int)mt],allshipmob_data[(int)mt].max_hull,point(0,0)));
    generateGuaranteedNPCShipMobModules(&extra_ships[(int)extra_ships.size()-1]);
}

void race::generateOneProcgenExtraShip(int dl, shipmob_classtype smct)
{
    shipmobstat_struct procgen_struct = getProcgenShipStatStruct(dl, smct);
    procgen_struct.ship_name = genName(3) + " " + classtype_name_strings[(int)smct];
    procgen_struct.ship_symbol = getRandShipSymbol(smct,dl,(int)procgen_struct.ship_name[0]);
    extra_ships.push_back(ship_mob(false,procgen_struct,procgen_struct.max_hull,point(0,0)));
    // for now there are no modified weapons
    generateGuaranteedNPCShipMobModules(&extra_ships[(int)extra_ships.size()-1]);
}

void race::generateExtraShips()
{
    int num_extra_ships = randInt(0,3);

    shipmob_classtype smct;
    shipmob_classtype max_class_type = CLASSTYPE_FIGHTER;

    if (danger_level > 7)
        max_class_type = CLASSTYPE_DREADNOUGHT;
    if (danger_level > 12)
        max_class_type = CLASSTYPE_DESTROYER;
    if (danger_level > 18)
        max_class_type = CLASSTYPE_JUGGERNAUT;

    int ship_dl = 1;

    switch(rtype)
    {
        case(RACETYPE_PROCGEN):
             for (int i = 0; i < num_extra_ships; ++i)
             {
                 if (i == 0 && roll(2))
                     generateOneGuaranteedExtraShip(SHIP_PATROL);
                 else
                 {
                     smct = (shipmob_classtype)randInt(2,randInt(2,(int)max_class_type));
                     if (smct == CLASSTYPE_FIGHTER)
                         ship_dl = randInt(1,danger_level);
                     if (smct == CLASSTYPE_DREADNOUGHT)
                         ship_dl = randInt(8,danger_level);
                     if (smct == CLASSTYPE_DESTROYER)
                         ship_dl = randInt(13,danger_level);
                     if (smct == CLASSTYPE_JUGGERNAUT)
                         ship_dl = randInt(19,danger_level);
                     generateOneProcgenExtraShip(ship_dl,smct);
                 }
             }
             break;
        default:
            break;
    }
}

void generateOneGuaranteedNativeShip(mob_t);

void race::generateOneGuaranteedNativeShip(mob_t mt)
{
    native_ships.push_back(ship_mob(false,allshipmob_data[(int)mt],allshipmob_data[(int)mt].max_hull,point(0,0)));
    generateGuaranteedNPCShipMobModules(&native_ships[(int)native_ships.size()-1]);
}

calls:
std::vector<ship_mob>().swap(extra_ships);

for (int i = 0; i < (int)extra_ships.size(); ++i)
     extra_ships[i].cleanupEverything();
std::vector<ship_mob>().swap(extra_ships);

ship_mob *race::getExtraShip(int i)
{
    return &extra_ships[i];
}

int race::getNumExtraShips()
{
    return (int)extra_ships.size();
}
*/

enum star_type
{
    STARTYPE_NONE,
    STARTYPE_MAINSEQ,
    STARTYPE_RED,
    STARTYPE_BLUE,
    STARTYPE_WHITE
};

enum race_major_status
{
    RMS_FREE,
    RMS_ENSLAVED,
    RMS_DESTROYED
};

enum rel_status
{
    REL_NEUTRAL,
    REL_HOSTILE,
    REL_ALLIANCE
};

enum race_personality_type
{
    PERSONALITY_NONE,
    PERSONALITY_FRIENDLY_GUARDED,
    PERSONALITY_SERIOUS_GUARDED,
    PERSONALITY_RUDE,
    PERSONALITY_FUNNY
};

enum race_domain_type
{
    RACEDOMAIN_SECTOR,
    RACEDOMAIN_DOMAIN,
    RACEDOMAIN_EMPIRE
};

static std::string race_domain_suffix_string[3] =
{
    "Sector",
    "Domain",
    "Empire"
};

struct homeworld_struct
{
    point loc;
    int race_owner_id;
    race_major_status rms_planet_state;
};

struct entrance_contact_struct
{
    int num_prerecorded_lines;
    int num_welcome_lines;
    int num_converse_choices;
};

class race
{
    public:
        race();
        race(int,int,race_domain_type,point, point, race_type, race_personality_type, std::string, int);
        void cleanupEverything();
        void addNativeShip(ship_mob *);
        void setRaceRelStatus(int, rel_status);
        void setRaceAttStatus(int, int);
        void incRaceAttStatus(int, int);
        void setPlayerAttStatus(int);
        void incPlayerAttStatus(int);
        void setupRelationshipVectors();
        void generateNativeShips();
        void generateOneProcgenNativeShip(int, shipmob_classtype, npc_ship_type);
        void setHomeworldMajorStatus(point,race_major_status);
        void setHomeworldOwnerRaceID(point,int);
        void addHomeworldStruct(point,int,race_major_status);
        homeworld_struct getHomeworldStruct(int);
        homeworld_struct getHomeworldStruct(point);
        entrance_contact_struct *getEntranceContactStruct();
        ship_mob *getNativeShip(int);
        point getStarmapLoc();
        point getSubAreaSize();
        point getFirstFreeHomeworldLoc();
        rel_status getRaceRelStatus(int);
        race_major_status getRaceOverallMajorStatus();
        race_domain_type getRaceDomainType();
        int getPlayerAttStatus();
        int getRaceAttStatus(int);
        int getNumNativeShips();
        int getRaceID();
        int getNumHomeworlds();
        int getDangerLevel();
        bool raceIdentifiedByPlayer();
        bool playerIdentifiedByRace();
        void setRaceIDByPlayerStatus(bool);
        void setPlayerIDByRaceStatus(bool);
        star_type getStarType();
        std::string getNameString();
        race_type getRaceType();
        race_personality_type getRacePersonalityType();
    private:
        std::vector<ship_mob> native_ships;
        std::vector<homeworld_struct> homeworld_objs;
        std::map<int,rel_status> rel_race_map;
        std::map<int,int> att_race_map;
        point sm_loc;
        point subarea_size;
        int att_towards_player;
        int race_id;
        int danger_level;
        bool race_identified_by_player;
        bool player_identified_by_race;
        race_domain_type rdtype;
        race_personality_type rptype;
        race_type rtype;
        star_type parent_star;
        std::string race_name_str;
        entrance_contact_struct ecs_obj;
};

void generateGuaranteedNPCShipMobModules(ship_mob *);

int weaponFillValFormula(ship_mob *, int);

chtype getRandShipSymbol(shipmob_classtype, int, int);

shipmobstat_struct getProcgenShipStatStruct(int , shipmob_classtype);

double getProcgenAccuracy(int);

double getProcgenEvasion(shipmob_classtype, int);

double getProcgenBaseSpeed(shipmob_classtype, int);

#endif
