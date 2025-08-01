#ifndef MOB_H_
#define MOB_H_

#include "point.h"
#include "module.h"
#include "rng.h"
#include "line.h"

#define CREDIT_DL_MULTIPLIER 1000ULL

// fighter                           abcdefgh...
// advanced fighter                 ABCDEFGH...
// dreadnought                       @ :) 8)
// destroyer                        0123456789
// juggernaut                           &

static const std::string classtype_name_strings[6] =
{
    "",
    "patrol",
    "fighter",
    "dreadnought",
    "destroyer",
    "juggernaut"
};

struct ship_design_struct
{
    int ca_ship_gens;
    int ca_start_val;
    int x_extension;
    color_type ca_secondary_color;
};

enum shipmob_classtype
{
    CLASSTYPE_NONE,
    CLASSTYPE_PATROL,
    CLASSTYPE_FIGHTER,
    CLASSTYPE_DREADNOUGHT,
    CLASSTYPE_DESTROYER,
    CLASSTYPE_JUGGERNAUT
};

enum shipmob_goalStatus
{
    GOALSTATUS_COMPLETE,
    GOALSTATUS_INCOMPLETE
};

enum shipmob_AIpattern
{
    AIPATTERN_NEUTRAL,
    AIPATTERN_FLEEING,
    AIPATTERN_ATTACKING,
    AIPATTERN_APPROACHPLANET,
    AIPATTERN_ROVING
};

struct shipmobstat_struct
{
    mob_t mtype;

    int danger_level;

    double accuracy;
    double evasion;
    // base speed assumes 1 engine with no bonus
    double base_speed;

    int max_hull;
    int detect_radius;

    int num_max_modules;

    int crash_factor;

    int num_crew;
    int num_fuel;

    int frequency;
    int shoot_frequency;

    int destruction_radius;

    int weapon_change_chance;

    int rove_chance;

    int min_attack_pfind_distance;

    int num_weapons_of_type[NUM_TOTAL_WEAPON_TYPES];
    int num_engines_of_type[NUM_TOTAL_ENGINE_TYPES];
    int num_shields_of_type[NUM_TOTAL_SHIELD_TYPES];

    bool crew_operable;

    shipmob_classtype sctype;

    race_type rtype;

    chtype ship_symbol;

    std::string ship_name;
};

static const shipmobstat_struct allshipmob_data[2] =
{
    {
        NIL_m, 1,
        1.0,1.0,100.0,
        1,1,1,1,0,0,
        0,0,1,0,0,1,
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0},
        {0,0,0},
        false,
        CLASSTYPE_NONE,
        RACETYPE_NONE,
        mob_symbol[(int)NIL_m],
        "NULL ENEMY"
    },
    {
        SHIP_PLAYER, 1,
        10.0,10.0,100.0,
        50,10,8,1,0,0,
        0,0,3,0,0,1,
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0},
        {0,0,0},
        true,
        CLASSTYPE_NONE,
        RACETYPE_NONE,
        mob_symbol[(int)SHIP_PLAYER],
        "USS Player"
    }
};

class MobShip
{
    public:
        MobShip();
        MobShip(bool,shipmobstat_struct,npc_ship_type,int,point);
        void setShipMob(bool,shipmobstat_struct,npc_ship_type,int,point);
        point at();
        shipmobstat_struct getStatStruct();
        shipmob_goalStatus getGoalStatus();
        shipmob_AIpattern getAIPattern();
        ship_design_struct getDesignStruct();
        npc_ship_type getNPCShipType();
        void setGoalStatus(shipmob_goalStatus);
        void setAIPattern(shipmob_AIpattern);
        void setRoveChance(int);
        void setTurnTimer(double);
        void decrementTurnTimer();
        void setMoveState(bool);
        bool getMoveState();
        bool inTimerRange();
        double getTurnTimer();
        double getSpeed();
        double getEvasion();
        double getAccuracy();
        double getBonusSpeed();
        double getModifiedEvasion();
        void setLoc(point p);
        void offLoc(point p);
        void addModule(Module);
        void removeModule(int);
        void cleanupEverything();
        void setModuleSelectionIndex(int);
        void offModuleSelectionIndex(int);
        void checkSelectedModuleBoundary();
        void setHullStatus(int);
        void setMaxHull(int);
        void setNumMaxModules(int);
        void setCrewStatus(int);
        void setMobSubAreaGroupID(int);
        void setMobSubAreaAttackID(int);
        void setInitLoc(point);
        void initShipDesign();
        Module *getModule(int);
        mob_t getMobType();
        int getFuelConsumptionRate();
        int getModuleSelectionIndex();
        int getDetectRadius();
        int getHullStatus();
        int getCrewStatus();
        int getMaxHull();
        int getMaxNumModules();
        int getNumInstalledModules();
        int getCrashFactor();
        int getTotalMTFillRemaining(module_type);
        int getTotalMTFillCapacity(module_type);
        int getDangerLevel();
        int getNumInstalledModulesOfType(module_type);
        int getTotalFillPercentageOfType(module_type);
        int getMobSubAreaID();
        int getMobSubAreaGroupID() const;
        int getMobSubAreaAttackID();
        void initMobSubAreaID(int);
        void setDestination(point);
        void useFuelEvent(int);
        void decTotalFillAmount(int,module_type);
        void incTotalFillAmount(int,module_type);
        void setActivationStatus(bool);
        void eraseModule(int);
        bool hasNoFuel();
        bool isLowOnFuel();
        bool isCurrentPlayerShip();
        bool crewOperable();
        bool isActivated() const;
        point getDestination();
        point getInitLoc();
        uint_64 getNumCredits();
        void setNumCredits(uint_64);
        void save(std::ofstream&) const;
        void load(std::ifstream&);
        chtype getShipSymbol();
        std::string getShipName();
        void setShipName(std::string);
    private:
        std::vector < Module > module_vec;
        shipmobstat_struct sms_data;
        npc_ship_type npc_ship_type_obj;
        bool player_controlled;
        bool is_activated;
        bool can_move;
        shipmob_goalStatus smgs;
        shipmob_AIpattern smAIp;
        double NPC_turn_timer;
        int mob_subarea_id;
        int hull_status;
        int danger_level;
        int module_selection_index;
        int mob_subarea_group_id;
        int mob_subarea_attack_id;
        ship_design_struct design_obj;
        point destination;
        point loc;
        point init_loc;
        uint_64 num_credits;
};

MobShip* getMobFromID(int);

MobShip* getPlayerShip();

MobShip* getCurrentMobTurn();

void saveShipMobStatStruct(std::ofstream& os, const shipmobstat_struct& s);

void loadShipMobStatStruct(std::ifstream& is, shipmobstat_struct& s);

extern MobShip player_ship;

#endif
