#include "race.h"

race::race()
{

}

race::race(int id, int dl, race_domain_type rdt, point ls, point sml, race_type rt, race_personality_type rpt, std::string name,int atp)
{
    parent_star = (roll(2) ? STARTYPE_MAINSEQ : (star_type)randInt((int)STARTYPE_MAINSEQ,(int)STARTYPE_WHITE));
    race_id = id;
    danger_level = dl;
    sm_loc = sml;
    player_identified_by_race = false;
    race_identified_by_player = false;
    rtype = rt;
    rptype = rpt;
    att_towards_player = atp;
    subarea_size = ls;
    race_name_str = name;
    rdtype = rdt;
    ecs_obj.num_converse_choices = 4;
    ecs_obj.num_prerecorded_lines = 3;
    ecs_obj.num_welcome_lines = 2;
}

void race::generateNativeShips()
{
    int num_native_ships = randInt(1,7);

    shipmob_classtype smct;
    shipmob_classtype max_class_type = CLASSTYPE_FIGHTER;

    if (danger_level > 4)
        max_class_type = CLASSTYPE_DREADNOUGHT;
    if (danger_level > 9)
        max_class_type = CLASSTYPE_DESTROYER;
    if (danger_level > 15)
        max_class_type = CLASSTYPE_JUGGERNAUT;

    int ship_dl = 1;

    switch(rtype)
    {
        case(RACETYPE_PROCGEN):
            for (int i = 0; i < num_native_ships; ++i)
            {
                smct = (shipmob_classtype)randInt(2,randInt(2,(int)max_class_type));
                if (smct == CLASSTYPE_FIGHTER)
                    ship_dl = randInt(1,danger_level);
                if (smct == CLASSTYPE_DREADNOUGHT)
                    ship_dl = randInt(5,danger_level);
                if (smct == CLASSTYPE_DESTROYER)
                    ship_dl = randInt(10,danger_level);
                if (smct == CLASSTYPE_JUGGERNAUT)
                    ship_dl = randInt(16,danger_level);
                generateOneProcgenNativeShip(ship_dl,smct,NPCSHIPTYPE_WAR);
            }
            break;
        default:
            break;
    }
}

void race::setupRelationshipVectors()
{
    for (int i = 0; i < NUM_TOTAL_RACES; ++i)
    {
        if (i != race_id)
        {
            if (roll(2))
                att_race_map[i] = randInt(-200,100);
            else
                att_race_map[i] = 0;
        }
    }
}

void race::generateOneProcgenNativeShip(int dl, shipmob_classtype smct, npc_ship_type nst)
{
    shipmobstat_struct procgen_struct = getProcgenShipStatStruct(dl, smct);
    procgen_struct.ship_name = race_name_str + " " + classtype_name_strings[(int)smct];
    procgen_struct.ship_symbol = getRandShipSymbol(smct,dl,(int)procgen_struct.ship_name[0]);
    native_ships.push_back(MobShip(false,procgen_struct,nst,procgen_struct.max_hull,point(0,0)));
    // for now there are no modified weapons
    generateGuaranteedNPCShipMobModules(&native_ships[(int)native_ships.size()-1]);
}

void race::cleanupEverything()
{
    for (int i = 0; i < (int)native_ships.size(); ++i)
         native_ships[i].cleanupEverything();
    std::vector<MobShip>().swap(native_ships);
    std::vector<homeworld_struct>().swap(homeworld_objs);
    std::map<int,rel_status>().swap(rel_race_map);
    std::map<int,int>().swap(att_race_map);
}

void race::addNativeShip(MobShip *added_ship)
{
    native_ships.push_back(*added_ship);
}

void race::setPlayerAttStatus(int ats)
{
    att_towards_player = ats;
}

void race::incPlayerAttStatus(int offset)
{
    att_towards_player += offset;
}

void race::setHomeworldOwnerRaceID(point p, int oid)
{
    for (int i = 0; i < (int)homeworld_objs.size(); ++i)
    {
        if (isAt(homeworld_objs[i].loc,p))
            homeworld_objs[i].race_owner_id = oid;
    }
}

void race::setHomeworldMajorStatus(point p, race_major_status rms)
{
    for (int i = 0; i < (int)homeworld_objs.size(); ++i)
    {
        if (isAt(homeworld_objs[i].loc,p))
            homeworld_objs[i].rms_planet_state = rms;
    }
}

void race::addHomeworldStruct(point p, int oid, race_major_status rms)
{
    homeworld_struct added_hs = {p,oid,rms};
    homeworld_objs.push_back(added_hs);
}

entrance_contact_struct *race::getEntranceContactStruct()
{
    return &ecs_obj;
}

homeworld_struct race::getHomeworldStruct(int i)
{
    return homeworld_objs[i];
}

homeworld_struct race::getHomeworldStruct(point p)
{
    for (int i = 0; i < (int)homeworld_objs.size(); ++i)
    {
        if (isAt(p,homeworld_objs[i].loc))
            return homeworld_objs[i];
    }

    // should never happen
    return homeworld_objs[0];
}

MobShip *race::getNativeShip(int i)
{
    return &native_ships[i];
}

int race::getNumNativeShips()
{
    return (int)native_ships.size();
}

int race::getRaceID()
{
    return race_id;
}

int race::getNumHomeworlds()
{
    return (int)homeworld_objs.size();
}

int race::getDangerLevel()
{
    return danger_level;
}

// identifier based access (NOT index based)
int race::getRaceAttStatus(int id)
{
    return att_race_map[id];
}

// identifier based access (NOT index based)
rel_status race::getRaceRelStatus(int id)
{
    return rel_race_map[id];
}

int race::getPlayerAttStatus()
{
    return att_towards_player;
}

point race::getStarmapLoc() const
{
    return sm_loc;
}

point race::getSubAreaSize()
{
    return subarea_size;
}

point race::getFirstFreeHomeworldLoc()
{
    for (int i = 0; i < (int)homeworld_objs.size(); ++i)
    {
        if (homeworld_objs[i].rms_planet_state == RMS_FREE)
            return homeworld_objs[i].loc;
    }

    // should never happen
    return homeworld_objs[0].loc;
}

void race::setRaceRelStatus(int rid, rel_status rs)
{
    rel_race_map[rid] = rs;
}

void race::setRaceAttStatus(int rid, int att)
{
    att_race_map[rid] = att;
}

void race::incRaceAttStatus(int rid, int offset)
{
    att_race_map[rid] += offset;
    if (att_race_map[rid] < -300)
        att_race_map[rid] = -300;
    if (att_race_map[rid] > 150)
        att_race_map[rid] = 150;
}

void race::setPlayerIDByRaceStatus(bool flag)
{
    player_identified_by_race = flag;
}

void race::setRaceIDByPlayerStatus(bool flag)
{
    race_identified_by_player = flag;
}

bool race::raceIdentifiedByPlayer()
{
    return race_identified_by_player;
}

bool race::playerIdentifiedByRace()
{
    return player_identified_by_race;
}

race_type race::getRaceType()
{
    return rtype;
}

star_type race::getStarType()
{
    return parent_star;
}

race_personality_type race::getRacePersonalityType()
{
    return rptype;
}

race_domain_type race::getRaceDomainType()
{
    return rdtype;
}

race_major_status race::getRaceOverallMajorStatus()
{
    if (!race_identified_by_player)
        return RMS_FREE;

    if ((int)homeworld_objs.size() == 0)
        return RMS_DESTROYED;

    for (int i = 0; i < (int)homeworld_objs.size(); ++i)
    {
        if (homeworld_objs[i].rms_planet_state == RMS_FREE)
            return RMS_FREE;
    }

    return RMS_ENSLAVED;
}

std::string race::getNameString()
{
    return race_name_str;
}

void generateGuaranteedNPCShipMobModules(MobShip *sm)
{
    module mod_ins;
    int weapon_fill_val;
    for (int w = 0; w < NUM_TOTAL_WEAPON_TYPES; ++w)
        for (int i = 0; i < sm->getStatStruct().num_weapons_of_type[w]; ++i)
        {
             weapon_fill_val = weaponFillValFormula(sm,allbasicweapon_stats[w].consumption_rate*allbasicweapon_stats[w].num_shots);
             mod_ins = module(MODULE_WEAPON,weapon_fill_val,weapon_fill_val);
             mod_ins.setWeaponStruct(allbasicweapon_stats[w]);
             sm->addModule(mod_ins);
        }

    for (int e = 0; e < NUM_TOTAL_ENGINE_TYPES; ++e)
        for (int i = 0; i < sm->getStatStruct().num_engines_of_type[e]; ++i)
        {
             mod_ins = module(MODULE_ENGINE,sm->getDangerLevel(),sm->getDangerLevel());
             mod_ins.setEngineStruct(allbasicengine_stats[e]);
             sm->addModule(mod_ins);
        }

    for (int s = 0; s < NUM_TOTAL_SHIELD_TYPES; ++s)
        for (int i = 0; i < sm->getStatStruct().num_shields_of_type[s]; ++i)
        {
             mod_ins = module(MODULE_SHIELD,allbasicshield_stats[s].base_num_layers,allbasicshield_stats[s].base_num_layers);
             mod_ins.setShieldStruct(allbasicshield_stats[s]);
             sm->addModule(mod_ins);
        }

    sm->addModule(module(MODULE_FUEL,sm->getStatStruct().num_fuel,sm->getStatStruct().num_fuel));
    sm->addModule(module(MODULE_CREW,sm->getStatStruct().num_crew,sm->getStatStruct().num_crew));
}

// standard for now
int weaponFillValFormula(MobShip *sm, int default_consumption_rate)
{
    return (sm->getDangerLevel() * default_consumption_rate * 3);
}

chtype getRandShipSymbol(shipmob_classtype smct, int dl, int ch)
{
    chtype ret_val;
    switch(smct)
    {
        case(CLASSTYPE_PATROL):
             ret_val.ascii = (int)'o';
             break;
        case(CLASSTYPE_FIGHTER):
             if (dl <= 13)
                 ret_val.ascii = ch + 32;
             else
                 ret_val.ascii = ch;
             break;
        case(CLASSTYPE_DREADNOUGHT):
             if (dl <= 5)
                 ret_val.ascii = (int)'@';
             else if (dl <= 15)
                 ret_val.ascii = 1;
             else
                 ret_val.ascii = 2;
             break;
        case(CLASSTYPE_DESTROYER):
             if (dl <= 10)
                 ret_val.ascii = (int)'0';
             else
                 ret_val.ascii = (int)(dl/3) + (int)'0';
             break;
        case(CLASSTYPE_JUGGERNAUT):
             ret_val.ascii = (int)'&';
             break;
        default:
             break;
    }
    ret_val.color = procgen_ship_colors[randInt(0,NUM_POSSIBLE_SHIP_COLORS-1)];
    return ret_val;
}

shipmobstat_struct getProcgenShipStatStruct(int dl, shipmob_classtype smct)
{
    int num_weapons = 1;
    int num_engines = 1;
    int num_shields = 0;
    shipmobstat_struct procgen_struct = allshipmob_data[0];
    procgen_struct.accuracy = getProcgenAccuracy(dl);
    procgen_struct.evasion = getProcgenEvasion(smct,dl);
    procgen_struct.base_speed = getProcgenBaseSpeed(smct,dl);
    procgen_struct.crew_operable = true;
    procgen_struct.num_crew = std::min(96,10 + randInt(1,dl+1)*2*(int)(smct));
    procgen_struct.num_fuel = 10 + randInt(1,dl+1)*(int)(smct);
    procgen_struct.max_hull = (smct != CLASSTYPE_FIGHTER ? 60 + randInt(1,dl)*7*(int)(smct) : 23 + randInt(2,dl+27));
    procgen_struct.shoot_frequency = 25 + randInt(0,dl)*2 + randInt(0,25);
    procgen_struct.destruction_radius = 2 + (int)(dl/4);
    procgen_struct.mtype = SHIP_PROCGEN;
    procgen_struct.rtype = RACETYPE_PROCGEN;
    procgen_struct.sctype = smct;
    procgen_struct.frequency = 1;
    procgen_struct.crash_factor = 1;
    procgen_struct.detect_radius = 3 + randInt(0,dl);
    procgen_struct.min_attack_pfind_distance = 1;
    procgen_struct.weapon_change_chance = randInt(0,15);
    procgen_struct.danger_level = dl;
    procgen_struct.rove_chance = 0;

    num_weapons += (randInt(0,1+(int)(dl/5)) + randInt(0,1+(int)(dl/5)));

    if (smct == CLASSTYPE_FIGHTER && num_weapons > 4)
        num_weapons = 4;

    num_engines += randInt(0,(int)(dl/5));

    if (smct == CLASSTYPE_FIGHTER && num_engines > 2)
        num_engines = 2;

    num_shields += randInt(0,(int)(dl/5));

    if (smct == CLASSTYPE_FIGHTER && num_shields > 1)
        num_shields = 1;

    if (smct == CLASSTYPE_DESTROYER)
        num_shields += 2;
    if (smct == CLASSTYPE_JUGGERNAUT)
        num_shields += 4;

    procgen_struct.num_max_modules = 2 + num_weapons + num_engines + num_shields;

    int roller = 0;

    for (int i = 0; i < num_weapons; ++i)
    {
        do
        {
            roller = randInt(1,NUM_TOTAL_WEAPON_TYPES-1);
        }while(!rollPerc(procgen_weapons_base_chance[roller]));
        procgen_struct.num_weapons_of_type[roller]++;
    }

    for (int i = 0; i < num_engines; ++i)
    {
        do
        {
            roller = randInt(1,NUM_TOTAL_ENGINE_TYPES-1);
        }while(!rollPerc(procgen_engines_base_chance[roller]));
        procgen_struct.num_engines_of_type[roller]++;
    }

    for (int i = 0; i < num_shields; ++i)
    {
        do
        {
            roller = randInt(1,NUM_TOTAL_SHIELD_TYPES-1);
        }while(!rollPerc(procgen_shields_base_chance[roller]));
        procgen_struct.num_shields_of_type[roller]++;
    }

    return procgen_struct;
}

double getProcgenAccuracy(int dl)
{
    return (double)(randInt(dl+2,10+(dl*2)));
}

double getProcgenEvasion(shipmob_classtype smct, int dl)
{
    double base_evasion = 10.0;

    switch(smct)
    {
        case(CLASSTYPE_FIGHTER):
        {
            base_evasion = (double)(randInt(2+dl,10+(dl*2)));
            break;
        }
        case(CLASSTYPE_DREADNOUGHT):
        case(CLASSTYPE_DESTROYER):
        case(CLASSTYPE_JUGGERNAUT):
        {
            base_evasion = (double)(randInt(3,dl));
            break;
        }
        default:
            break;
    }

    return base_evasion;
}

double getProcgenBaseSpeed(shipmob_classtype smct, int dl)
{
    double base_speed = 100.0;

    switch(smct)
    {
        case(CLASSTYPE_FIGHTER):
        {
            base_speed = (double)randInt(0,((int)dl/2)+1)*10.0 + 50.0;
            break;
        }
        case(CLASSTYPE_DREADNOUGHT):
        case(CLASSTYPE_DESTROYER):
        case(CLASSTYPE_JUGGERNAUT):
        {
            if (roll(3))
                base_speed = (double)randInt(1,(int)(dl/9)+1)*10.0 + 30.0;
            else
                base_speed = 100.0;
            break;
        }
        default:
            break;
    }

    return base_speed;
}
