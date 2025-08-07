#ifndef RACE_H_
#define RACE_H_

#include "planet.h"
#include "namegen.h"
#include <map>

/*
Extra stuff not needed yet (ever?)

private:
std::vector<MobShip> extra_ships;

public:
int getNumExtraShips();
MobShip *getExtraShip(int);
void generateOneProcgenExtraShip(int, shipmob_classtype);
void generateOneGuaranteedExtraShip(mob_t);
void generateExtraShips();
void addExtraShip(MobShip *);

void race::addExtraShip(MobShip *added_ship)
{
    extra_ships.push_back(*added_ship);
}

void race::generateOneGuaranteedExtraShip(mob_t mt)
{
    extra_ships.push_back(MobShip(false,allshipmob_data[(int)mt],allshipmob_data[(int)mt].max_hull,point(0,0)));
    generateGuaranteedNPCShipMobModules(&extra_ships[(int)extra_ships.size()-1]);
}

void race::generateOneProcgenExtraShip(int dl, shipmob_classtype smct)
{
    shipmobstat_struct procgen_struct = getProcgenShipStatStruct(dl, smct);
    procgen_struct.ship_name = genName(3) + " " + classtype_name_strings[(int)smct];
    procgen_struct.ship_symbol = getRandShipSymbol(smct,dl,(int)procgen_struct.ship_name[0]);
    extra_ships.push_back(MobShip(false,procgen_struct,procgen_struct.max_hull,point(0,0)));
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
    native_ships.push_back(MobShip(false,allshipmob_data[(int)mt],allshipmob_data[(int)mt].max_hull,point(0,0)));
    generateGuaranteedNPCShipMobModules(&native_ships[(int)native_ships.size()-1]);
}

calls:
std::vector<MobShip>().swap(extra_ships);

for (int i = 0; i < (int)extra_ships.size(); ++i)
     extra_ships[i].cleanupEverything();
std::vector<MobShip>().swap(extra_ships);

MobShip *race::getExtraShip(int i)
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
        void addNativeShip(MobShip *);
        void setRaceRelStatus(int, rel_status);
        void setRaceAttStatus(int, int);
        void incRaceAttStatus(int, int);
        void setPlayerAttStatus(int);
        void incPlayerAttStatus(int);
        void setupRelationshipVectors();
        void generateNativeShips();
        void generateOneProcgenNativeShip(int, shipmob_classtype, npc_ship_type);
        void setHomeworldMajorStatus(point, RaceMajorStatus);
        void setHomeworldControllerRace(point,int, int);
        void addHomeworld(point,int, int, int);
        void setControllerRaceID(int);
        void setNumStartingShipsAtRegion(int numShips);
        void setSurrenderedToPlayer(bool surrendered);
        void setNumShipsDestroyedByPlayerAtRegion(int numDestroyed);
        Planet * getHomeworld(int);
        Planet * getHomeworld(point);
        entrance_contact_struct *getEntranceContactStruct();
        MobShip *getNativeShip(int);
        point getStarmapLoc() const;
        point getSubAreaSize();
        point getFirstFreeHomeworldLoc();
        rel_status getRaceRelStatus(int);
        RaceMajorStatus getRaceOverallMajorStatus();
        race_domain_type getRaceDomainType();
        int getPlayerAttStatus();
        int getRaceAttStatus(int);
        int getNumNativeShips();
        int getRaceID();
        int getNumHomeworlds();
        int getDangerLevel();
        int getControllerRaceID();
        int getNumShipsDestroyedByPlayerAtRegion();
        int getNumStartingShipsAtRegion();
        bool raceIdentifiedByPlayer();
        bool playerIdentifiedByRace();
        bool isSurrenderedToPlayer();
        bool surrenderToPlayerPossible();
        void setRaceIDByPlayerStatus(bool);
        void setPlayerIDByRaceStatus(bool);
        void setSurrenderToPlayerPossibility(bool);
        void save(std::ofstream&) const;
        void load(std::ifstream&);
        star_type getStarType();
        std::string getNameString();
        race_type getRaceType();
        race_personality_type getRacePersonalityType();
    private:
        std::vector<MobShip> native_ships;
        std::vector<Planet> homeworlds;
        std::map<int,rel_status> rel_race_map;
        std::map<int,int> att_race_map;
        point sm_loc;
        point subarea_size;
        int att_towards_player;
        int race_id;
        int controller_race_id;
        int danger_level;
        int shipsDestroyedByPlayerAtRegion;
        int numStartingShipsAtRegion;
        bool surrenderedToPlayer;
        bool canSurrenderToPlayer;
        bool race_identified_by_player;
        bool player_identified_by_race;
        race_domain_type rdtype;
        race_personality_type rptype;
        race_type rtype;
        star_type parent_star;
        std::string race_name_str;
        entrance_contact_struct ecs_obj;
};

void generateGuaranteedNPCShipMobModules(MobShip *);

int weaponFillValFormula(MobShip *, int);

chtype getRandShipSymbol(shipmob_classtype, int, int);

shipmobstat_struct getProcgenShipStatStruct(int , shipmob_classtype);

double getProcgenAccuracy(int);

double getProcgenEvasion(shipmob_classtype, int);

double getProcgenBaseSpeed(shipmob_classtype, int);

MobShip* getHighestDangerLevelShipForRace(race* r);

std::string fullShipName(MobShip* ship, const std::string & raceName, shipmob_classtype smct, bool pirate);

#endif
