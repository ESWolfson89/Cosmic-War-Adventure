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
    surrenderedToPlayer = false;
    canSurrenderToPlayer = true;
    player_identified_by_race = false;
    race_identified_by_player = false;
    rtype = rt;
    rptype = rpt;
    att_towards_player = atp;
    subarea_size = ls;
    race_name_str = name;
    rdtype = rdt;
    raceIDCommandedToAttack = -1;
    ecs_obj.num_converse_choices = 4;
    ecs_obj.num_prerecorded_lines = 3;
    ecs_obj.num_welcome_lines = 2;
    shipsDestroyedByPlayerAtRegion = 0;
    numStartingShipsAtRegion = 0;
    setControllerRaceID(id);
}

void race::setRaceIDCommandedToAttack(int id)
{
    raceIDCommandedToAttack = id;
}

int race::getRaceIDCommandedToAttack()
{
    return raceIDCommandedToAttack;
}

void race::setSurrenderToPlayerPossibility(bool canSurrender)
{
    canSurrenderToPlayer = canSurrender;
}

bool race::surrenderToPlayerPossible()
{
    return canSurrenderToPlayer;
}

void race::setNumStartingShipsAtRegion(int numShips)
{
    numStartingShipsAtRegion = numShips;
}

int race::getNumStartingShipsAtRegion()
{
    return numStartingShipsAtRegion;
}

void race::setSurrenderedToPlayer(bool surrendered)
{
    surrenderedToPlayer = surrendered;
}

bool race::isSurrenderedToPlayer()
{
    return surrenderedToPlayer;
}

void race::setNumShipsDestroyedByPlayerAtRegion(int numDestroyed)
{
    shipsDestroyedByPlayerAtRegion = numDestroyed;
}

int race::getNumShipsDestroyedByPlayerAtRegion()
{
    return shipsDestroyedByPlayerAtRegion;
}

void race::setControllerRaceID(int id)
{
    controller_race_id = id;
}

void race::generateNativeShips()
{
    const int numShips = randInt(1, 7);

    const shipmob_classtype maxClass = [&] {
        if (danger_level > 15) return CLASSTYPE_JUGGERNAUT;
        if (danger_level > 9)  return CLASSTYPE_DESTROYER;
        if (danger_level > 4)  return CLASSTYPE_DREADNOUGHT;
        return CLASSTYPE_FIGHTER;
        }();

    if (rtype != RACETYPE_PROCGEN)
        return;

    const int lowClassInt = static_cast<int>(CLASSTYPE_FIGHTER);
    const int maxClassInt = static_cast<int>(maxClass);

    for (int i = 0; i < numShips; ++i)
    {
        // Ensure Fighter (2) is included as a possible class
        const int classInt = randInt(lowClassInt, randInt(lowClassInt, maxClassInt));
        const shipmob_classtype smct = static_cast<shipmob_classtype>(classInt);

        // Determine danger-level spawn range per class
        int shipDl = 1;
        switch (smct)
        {
            case CLASSTYPE_FIGHTER:
                shipDl = randInt(1, danger_level);
                break;
            case CLASSTYPE_DREADNOUGHT:
                shipDl = randInt(5, danger_level);
                break;
            case CLASSTYPE_DESTROYER:
                shipDl = randInt(10, danger_level);
                break;
            case CLASSTYPE_JUGGERNAUT:
                shipDl = randInt(16, danger_level);
                break;
            default:
                break;
        }

        generateOneProcgenNativeShip(shipDl, smct, NPCSHIPTYPE_WAR);
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


//     Q
//   q   q
//    5 5
//   q   q
//     Q
//      
//     S
//      
//     Q     
//   q   q  
//    5 5 
//   q   q
//     Q
void race::generateOneProcgenNativeShip(int dl, shipmob_classtype smct, npc_ship_type nst)
{
    shipmobstat_struct procgen_struct = getProcgenShipStatStruct(dl, smct);
    procgen_struct.ship_name = race_name_str;
    procgen_struct.ship_symbol = getRandShipSymbol(smct,dl,(int)procgen_struct.ship_name[0]);
    native_ships.push_back(MobShip(false,procgen_struct,nst,procgen_struct.max_hull,point(0,0)));
    // for now there are no modified weapons
    MobShip* newShip = &native_ships[(int)native_ships.size() - 1];
    generateGuaranteedNPCShipMobModules(newShip);
    newShip->setShipName(fullShipName(newShip, race_name_str, smct, false));
}

std::string fullShipName(MobShip* ship, const std::string& raceName, shipmob_classtype smct, bool isPirate)
{
    const bool hasExplosiveWeapon = hasWeaponOfType(ship, WEAPONTYPE_MISSILE) || hasWeaponOfType(ship, WEAPONTYPE_HELL);

    if (isPirate)
    {
        return hasExplosiveWeapon
            ? raceName + " pirate bomber"
            : raceName + " pirate";
    }

    if (hasExplosiveWeapon && smct == CLASSTYPE_FIGHTER)
    {
        return raceName + " bomber";
    }

    return raceName + " " + classtype_name_strings[static_cast<int>(smct)];
}

void race::cleanupEverything()
{
    for (int i = 0; i < (int)native_ships.size(); ++i)
         native_ships[i].cleanupEverything();
    std::vector<MobShip>().swap(native_ships);
    std::vector<HomeWorld>().swap(homeworlds);
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

void race::setHomeworldControllerRace(point p, int cID, int dl)
{
    for (int i = 0; i < (int)homeworlds.size(); ++i)
    {
        if (isAt(homeworlds[i].getLoc(), p))
            homeworlds[i].setControlRace(cID, dl);
    }
    setControllerRaceID(cID);
}

void race::setHomeworldMajorStatus(point p, RaceMajorStatus rms)
{
    for (int i = 0; i < (int)homeworlds.size(); ++i)
    {
        if (isAt(homeworlds[i].getLoc(), p))
            homeworlds[i].setRaceMajorStatus(rms);
    }
}

void race::addHomeworld(point p, int oid, int cid, int dl)
{
    homeworlds.push_back(HomeWorld(p, oid, cid, dl));
}

entrance_contact_struct *race::getEntranceContactStruct()
{
    return &ecs_obj;
}

HomeWorld * race::getHomeworld(int i)
{
    return &homeworlds[i];
}

HomeWorld * race::getHomeworld(point p)
{
    for (int i = 0; i < (int)homeworlds.size(); ++i)
    {
        if (isAt(p,homeworlds[i].getLoc()))
            return &homeworlds[i];
    }

    // should never happen
    return &homeworlds[0];
}

MobShip *race::getNativeShip(int i)
{
    return &native_ships[i];
}

int race::getControllerRaceID()
{
    return controller_race_id;
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
    return (int)homeworlds.size();
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
    for (int i = 0; i < (int)homeworlds.size(); ++i)
    {
        if (homeworlds[i].getRaceMajorStatus() == RMS_FREE)
            return homeworlds[i].getLoc();
    }

    // should never happen
    return homeworlds[0].getLoc();
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

RaceMajorStatus race::getRaceOverallMajorStatus()
{
    if (!race_identified_by_player)
        return RMS_FREE;

    if ((int)homeworlds.size() == 0)
        return RMS_DESTROYED;

    for (int i = 0; i < (int)homeworlds.size(); ++i)
    {
        if (homeworlds[i].getRaceMajorStatus() == RMS_FREE)
            return RMS_FREE;
    }

    return RMS_ENSLAVED;
}

std::string race::getNameString()
{
    return race_name_str;
}

void race::save(std::ofstream& os) const
{
    sm_loc.save(os);
    subarea_size.save(os);

    os.write(reinterpret_cast<const char*>(&att_towards_player), sizeof(int));
    os.write(reinterpret_cast<const char*>(&race_id), sizeof(int));
    os.write(reinterpret_cast<const char*>(&controller_race_id), sizeof(int));
    os.write(reinterpret_cast<const char*>(&danger_level), sizeof(int));
    os.write(reinterpret_cast<const char*>(&race_identified_by_player), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&player_identified_by_race), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&shipsDestroyedByPlayerAtRegion), sizeof(int));
    os.write(reinterpret_cast<const char*>(&numStartingShipsAtRegion), sizeof(int));
    os.write(reinterpret_cast<const char*>(&raceIDCommandedToAttack), sizeof(int));
    os.write(reinterpret_cast<const char*>(&surrenderedToPlayer), sizeof(bool));
    os.write(reinterpret_cast<const char*>(&canSurrenderToPlayer), sizeof(bool));

    os.write(reinterpret_cast<const char*>(&rdtype), sizeof(race_domain_type));
    os.write(reinterpret_cast<const char*>(&rptype), sizeof(race_personality_type));
    os.write(reinterpret_cast<const char*>(&rtype), sizeof(race_type));
    os.write(reinterpret_cast<const char*>(&parent_star), sizeof(star_type));

    stringSave(os, race_name_str);

    // Save ecs_obj
    os.write(reinterpret_cast<const char*>(&ecs_obj.num_prerecorded_lines), sizeof(int));
    os.write(reinterpret_cast<const char*>(&ecs_obj.num_welcome_lines), sizeof(int));
    os.write(reinterpret_cast<const char*>(&ecs_obj.num_converse_choices), sizeof(int));

    // Save native_ships
    int ship_count = static_cast<int>(native_ships.size());
    os.write(reinterpret_cast<const char*>(&ship_count), sizeof(int));
    for (const MobShip& s : native_ships)
        s.save(os);

    // Save homeworld_objs
    int hw_count = static_cast<int>(homeworlds.size());
    os.write(reinterpret_cast<const char*>(&hw_count), sizeof(int));
    for (const HomeWorld& hw : homeworlds)
        hw.save(os);

    // Save att_race_map
    int att_count = static_cast<int>(att_race_map.size());
    os.write(reinterpret_cast<const char*>(&att_count), sizeof(int));
    for (const auto& [k, v] : att_race_map) {
        os.write(reinterpret_cast<const char*>(&k), sizeof(int));
        os.write(reinterpret_cast<const char*>(&v), sizeof(int));
    }
}

void race::load(std::ifstream& is)
{
    sm_loc.load(is);
    subarea_size.load(is);

    is.read(reinterpret_cast<char*>(&att_towards_player), sizeof(int));
    is.read(reinterpret_cast<char*>(&race_id), sizeof(int));
    is.read(reinterpret_cast<char*>(&controller_race_id), sizeof(int));
    is.read(reinterpret_cast<char*>(&danger_level), sizeof(int));
    is.read(reinterpret_cast<char*>(&race_identified_by_player), sizeof(bool));
    is.read(reinterpret_cast<char*>(&player_identified_by_race), sizeof(bool));
    is.read(reinterpret_cast<char*>(&shipsDestroyedByPlayerAtRegion), sizeof(int));
    is.read(reinterpret_cast<char*>(&numStartingShipsAtRegion), sizeof(int));
    is.read(reinterpret_cast<char*>(&raceIDCommandedToAttack), sizeof(int));
    is.read(reinterpret_cast<char*>(&surrenderedToPlayer), sizeof(bool));
    is.read(reinterpret_cast<char*>(&canSurrenderToPlayer), sizeof(bool));

    is.read(reinterpret_cast<char*>(&rdtype), sizeof(race_domain_type));
    is.read(reinterpret_cast<char*>(&rptype), sizeof(race_personality_type));
    is.read(reinterpret_cast<char*>(&rtype), sizeof(race_type));
    is.read(reinterpret_cast<char*>(&parent_star), sizeof(star_type));

    stringLoad(is, race_name_str);

    is.read(reinterpret_cast<char*>(&ecs_obj.num_prerecorded_lines), sizeof(int));
    is.read(reinterpret_cast<char*>(&ecs_obj.num_welcome_lines), sizeof(int));
    is.read(reinterpret_cast<char*>(&ecs_obj.num_converse_choices), sizeof(int));

    int ship_count = 0;
    is.read(reinterpret_cast<char*>(&ship_count), sizeof(int));
    native_ships.resize(ship_count);
    for (int i = 0; i < ship_count; ++i)
        native_ships[i].load(is);

    int hw_count = 0;
    is.read(reinterpret_cast<char*>(&hw_count), sizeof(int));
    homeworlds.resize(hw_count);
    for (int i = 0; i < hw_count; ++i)
        homeworlds[i].load(is);

    int att_count = 0;
    is.read(reinterpret_cast<char*>(&att_count), sizeof(int));
    att_race_map.clear();
    for (int i = 0; i < att_count; ++i) {
        int key = 0, val = 0;
        is.read(reinterpret_cast<char*>(&key), sizeof(int));
        is.read(reinterpret_cast<char*>(&val), sizeof(int));
        att_race_map[key] = val;
    }
}

void generateGuaranteedNPCShipMobModules(MobShip* sm)
{
    const auto& stats = sm->getStatStruct();
    Module mod;

    // Weapons
    for (int w = 0; w < NUM_TOTAL_WEAPON_TYPES; ++w)
    {
        int count = stats.num_weapons_of_type[w];
        const auto& wstat = allbasicweapon_stats[w];

        for (int i = 0; i < count; ++i)
        {
            int fill = weaponFillValFormula(
                sm,
                wstat.consumption_rate * wstat.num_shots
            );
            mod = Module(MODULE_WEAPON, fill, fill);
            mod.setWeaponStruct(wstat);
            sm->addModule(mod);
        }
    }

    // Engines
    for (int e = 0; e < NUM_TOTAL_ENGINE_TYPES; ++e)
    {
        int count = stats.num_engines_of_type[e];
        const auto& estat = allbasicengine_stats[e];

        for (int i = 0; i < count; ++i)
        {
            int fill = sm->getDangerLevel();
            mod = Module(MODULE_ENGINE, fill, fill);
            mod.setEngineStruct(estat);
            sm->addModule(mod);
        }
    }

    // Shields
    for (int s = 0; s < NUM_TOTAL_SHIELD_TYPES; ++s)
    {
        int count = stats.num_shields_of_type[s];
        const auto& sstat = allbasicshield_stats[s];

        for (int i = 0; i < count; ++i)
        {
            int fill = sstat.base_num_layers;
            mod = Module(MODULE_SHIELD, fill, fill);
            mod.setShieldStruct(sstat);
            sm->addModule(mod);
        }
    }

    // Fuel & Crew—exactly one each
    sm->addModule(Module(MODULE_FUEL, stats.num_fuel, stats.num_fuel));
    sm->addModule(Module(MODULE_CREW, stats.num_crew, stats.num_crew));
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

MobShip* getHighestDangerLevelShipForRace(race* r)
{
    int iteration = 0;
    int maxDangerLevel = 0;

    for (int i = 0; i < r->getNumNativeShips(); i++)
    {
        int dangerLevel = r->getNativeShip(i)->getDangerLevel();
        if (dangerLevel > maxDangerLevel)
        {
            maxDangerLevel = dangerLevel;
            iteration = i;
        }
    }

    return r->getNativeShip(iteration);
}
