#ifndef MODULE_H_
#define MODULE_H_

#include "misc.h"

static const int procgen_weapons_base_chance[NUM_TOTAL_WEAPON_TYPES] =
{
    0,100,50,25,50,25,12,9,5,2,10,2,3,12,4,2,1
};

static const int procgen_engines_base_chance[NUM_TOTAL_ENGINE_TYPES] =
{
    0,100,20
};

static const int procgen_shields_base_chance[NUM_TOTAL_SHIELD_TYPES] =
{
    0,60,10
};

enum module_type
{
    MODULE_CREW,
    MODULE_FUEL,
    MODULE_WEAPON,
    MODULE_SHIELD,
    MODULE_ENGINE,
    MODULE_ARTIFACT
};

enum weapon_t
{
    WEAPONTYPE_NONE,
    WEAPONTYPE_BLAST,
    WEAPONTYPE_BEAM,
    WEAPONTYPE_MISSILE,
    WEAPONTYPE_SPREAD,
    WEAPONTYPE_PULSE,
    WEAPONTYPE_WALLOP,
    WEAPONTYPE_INSTANT,
    WEAPONTYPE_HELL,
    WEAPONTYPE_MECH
};

struct shipengine_struct
{
    int bonus_speed;
    int bonus_evasion;
    int fuel_penalty;
    chtype disp_chtype;
    std::string name_modifier;
    int base_cost;
};

struct shield_struct
{
    int base_num_layers;
    int regen_rate;
    // for display class use only
    chtype disp_chtype;
    std::string name_modifier;
    int base_cost;
};

struct weapon_struct
{
    weapon_t wt;
    // 2 separate weapon damages for crew and hull
    dice_roll hull_damage;
    dice_roll crew_damage;
    // to hit ship (then damage is done for hull/crew)
    int to_hit;
    // will do this much damage to shields (which regen over time)
    int shield_damage_count;
    // chance of going undetected
    int stealth_chance;
    // travels through ships on hit?
    int travel_through_chance;
    // crit factors for crew on board
    int anti_personnel_crit_perc;
    int anti_personnel_crit_multiplier;
    // hull crit factors
    int anti_hull_crit_perc;
    int anti_hull_crit_multiplier;
    // range of weapon (-1 => infinite, else 1 or above)
    int travel_range;
    // will create shockwave damaging neighboring ships? If <= 0 no, else
    // 1 or above is blast radius
    int blast_radius;
    // how many shots per turn (takes up more module fill quantity per unit factor)
    int num_shots;
    // how much battery (fillquant) does it take per use?
    int consumption_rate;
    int regen_rate;
    // which fire tile does it use?
    // ----------------------------
    // if true, display directional dependent straight line
    bool is_firet_line;
    bool ignores_shields;
    fire_t ftile;
    // color shown in hud
    chtype disp_chtype;
    // laser, beam, missile etc...
    std::string name_modifier;
    int base_cost;
};

struct artifact_struct
{
    double accuracy_bonus;
    double evasion_bonus;
    double speed_bonus;
    int hull_damage_bonus;
    int crew_damage_bonus;
    int shield_damage_bonus;
    int fuel_penalty;
    int detect_radius_bonus;
    int weapon_range_bonus;
    chtype disp_chtype[6];
    std::string name_modifier;
};

static const std::string module_basic_names[NUM_TOTAL_MODULE_TYPES] =
{
    "crew pod",
    "fuel tank",
    "weapon",
    "shield",
    "engine",
    "cargo",
    "artifact"
};

static const weapon_struct allbasicweapon_stats[NUM_TOTAL_WEAPON_TYPES] =
{
  {
      WEAPONTYPE_NONE, {0,0,0}, {0,0,0}, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, false, false, NIL_f, {cp_blackonblack,(int)'N'}, "", 0
  },
  {
      WEAPONTYPE_BLAST, {1,6,9}, {1,3,0}, 2, 1, 0, 0,
      0, 0, 0, 0, 4, 0, 1, 5, 10, true, false, FIRET_REDLINE, {cp_redonblack,92}, "laser", 250
  },
  {
      WEAPONTYPE_BLAST, {1,6,9}, {1,3,0}, 2, 1, 0, 0,
      0, 0, 0, 0, 4, 0, 2, 5, 10, true, false, FIRET_REDLINE, {cp_redonblack,92}, "laser II", 1000
  },
  {
      WEAPONTYPE_BLAST, {1,6,9}, {1,3,0}, 2, 1, 0, 0,
      0, 0, 0, 0, 4, 0, 3, 5, 10, true, false, FIRET_REDLINE, {cp_redonblack,92}, "laser III", 5000
  },
  {
      WEAPONTYPE_BLAST, {2,9,8}, {1,4,0}, 3, 1, 0, 0,
      0, 0, 0, 0, 4, 0, 1, 7, 15, true, false, FIRET_BLUELINE, {cp_cyanonblack,92}, "ion bolt", 1000
  },
  {
      WEAPONTYPE_BLAST, {2,9,8}, {1,4,0}, 3, 1, 0, 0,
      0, 0, 0, 0, 4, 0, 2, 7, 15, true, false, FIRET_BLUELINE, {cp_cyanonblack,92}, "ion bolt II", 5000
  },
  {
      WEAPONTYPE_BLAST, {2,9,8}, {1,4,0}, 3, 1, 0, 0,
      0, 0, 0, 0, 4, 0, 3, 7, 15, true, false, FIRET_BLUELINE, {cp_cyanonblack,92}, "ion bolt III", 10000
  },
  {
      WEAPONTYPE_BLAST, {3,12,7}, {1,5,0}, 1, 1, 0, 0,
      0, 0, 0, 0, 4, 0, 1, 9, 20, true, false, FIRET_PURPLELINE, {cp_purpleonblack,92}, "energy blaster I", 12500
  },
  {
      WEAPONTYPE_BLAST, {3,12,7}, {1,5,0}, 1, 1, 0, 0,
      0, 0, 0, 0, 4, 0, 2, 9, 20, true, false, FIRET_PURPLELINE, {cp_purpleonblack,92}, "energy blaster II", 75000
  },
  {
      WEAPONTYPE_BLAST, {3,12,7}, {1,5,0}, 1, 1, 0, 0,
      0, 0, 0, 0, 4, 0, 3, 9, 20, true, false, FIRET_PURPLELINE, {cp_purpleonblack,92}, "energy blaster III", 300000
  },
  {
      WEAPONTYPE_SPREAD, {6,7,6}, {1,4,7}, 6, 1, 0, 0,
      0, 0, 0, 0, 4, 0, 1, 4, 12, false, false, FIRET_EXPLOSION, {cp_orangeonblack,(int)247}, "flamer", 25000
  },
  {
      WEAPONTYPE_PULSE, {4,5,4}, {1,2,3}, 5, 1, 0, 0,
      0, 0, 0, 0, 7, 0, 1, 8, 25, false, false, FIRET_PURPLEPLASMA, {cp_purpleonblack,(int)'^'}, "pulse cannon", 750000
  },
  {
      WEAPONTYPE_WALLOP, {0,0,0}, {12,3,6}, 7, 0, 0, 0,
      4, 2, 0, 0, 4, 0, 1, 7, 30, false, true, FIRET_GREENWALLOP, {cp_greenonblack,(int)'*'}, "plague cannon", 150000
  },
  {
      WEAPONTYPE_MISSILE, {6,19,9}, {1,6,8}, 0, 0, 0, 0,
      5, 2, 5, 3, 5, 2, 1, 1, 15, true, true, FIRET_GRAYLINE, {cp_grayonblack,(int)'!'}, "missile launcher", 50000
  },
  {
      WEAPONTYPE_MISSILE, {6,19,9}, {1,6,8}, 0, 0, 0, 0,
      5, 2, 5, 3, 5, 2, 2, 1, 15, true, true, FIRET_GRAYLINE, {cp_grayonblack,(int)'!'}, "dual missile launcher", 100000
  },
  {
      WEAPONTYPE_MECH, {6,7,8}, {1,6,6}, 15, 3, 0, 100,
      0, 0, 8, 3, 6, 0, 1, 1, 20, false, false, FIRET_DARKRAZOR, {cp_darkgrayonblack,15}, "giant spinning razor", 250000
  },
  {
      WEAPONTYPE_HELL, {8,9,9}, {1,10,5}, 20, 0, 0, 0,
      1, 3, 2, 2, 8, 5, 1, 10, 50, false, true, FIRET_DAMAGINGEXPLOSION, {cp_darkredonblack,239}, "hell cannon", 1000000
  }
};

static const shield_struct allbasicshield_stats[3] =
{
    {0,1,{cp_blackonblack, (int)'N'},"",0},
    {1,150,{cp_redonblack, (int)'#'},"basic shield",1000},
    {3,100,{cp_whiteonblack, (int)'#'},"advanced shield",5000}
};

static const shipengine_struct allbasicengine_stats[3] =
{
    {0,0,0,{cp_blackonblack,(int)'N'},"",0},
    {0,1,0,{cp_orangeonblack, (int)'+'},"basic propulsion system",2500},
    {10,2,1,{cp_blueonblack, (int)'+'},"advanced thruster",15000}
};

class module
{
    public:
        module();
        module(module_type, int, int);
        void setModule(module_type, int, int);
        module_type getModuleType();
        void offFillQuantity(int);
        void setFillQuantity(int);
        void checkFillQuantity();
        void setWeaponStruct(weapon_struct);
        void setShieldStruct(shield_struct);
        void setEngineStruct(shipengine_struct);
        weapon_struct getWeaponStruct();
        shield_struct getShieldStruct();
        shipengine_struct getEngineStruct();
        int getFillQuantity();
        int getMaxFillQuantity();
        int getBaseCost();
        bool isItemContainer();
        void save(std::ofstream&) const;
        void load(std::ifstream&);
    private:
        module_type mt;
        int fill_quantity;
        int max_fill_quantity;
        bool is_item_container;
        weapon_struct weapon_stats;
        shield_struct shield_stats;
        shipengine_struct engine_stats;
        std::string module_name;
        std::string name_modifier;
};

int getWeaponModuleConsumptionPerTurn(module *);

module rollOneModule(int);

#endif
