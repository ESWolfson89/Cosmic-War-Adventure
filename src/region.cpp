#include "region.h"

StarMapRegion universe;
MapType current_maptype;
int current_subarea_id;
int current_mob_turn;

// starmap implementation

StarMapRegion::StarMapRegion()
{

}

void StarMapRegion::setupUniverse()
{
    m = map(point(STARMAPWID,STARMAPHGT));

    initEmptyTiles(&m,SMBACKDROP_SPACE);
    generateAllRaces();

    point genLoc;

    for (int i = 0; i < NUM_STAR_SYSTEMS; ++i)
    {
        do
        {
            int maxX = m.getSize().x() - 1;
            int maxY = m.getSize().y() - 1;
            genLoc = randZeroBasedPoint(maxX, maxY);
        } while(m.getBackdrop(genLoc) != SMBACKDROP_SPACE);

        backdrop_t starType = (backdrop_t)randInt(MIN_UNDSC_STAR_INT, MAX_UNDSC_STAR_INT);

        m.setBackdrop(genLoc,starType);
    }

    nonpersistent_subarea = SubAreaRegion(-1,false,STARTYPE_NONE);
    nonpersistent_subarea.initMap(point(15,15));
}

void StarMapRegion::cleanupEverything()
{
    m.clearAllCells();

    nonpersistent_subarea.cleanupEverything();

    for (auto& s : subareaVector)
        s.cleanupEverything();

    for (auto& r : raceVector)
        r.cleanupEverything();

    subareaVector.clear();
    subareaVector.shrink_to_fit();

    raceVector.clear();
    raceVector.shrink_to_fit();
}

void StarMapRegion::generateAllRaces()
{
    point smLoc;
    point mapSize;
    int dangerLevel;
    int maxDangerLevel;
    int attitudeTowardsPlayer = 0;
    race_domain_type raceDomainType;
    race_type raceType = RACETYPE_PROCGEN;
    race_personality_type racePersonalityType = PERSONALITY_FRIENDLY_GUARDED;

    for (int i = 0; i < NUM_TOTAL_RACES; ++i)
    {
        do
        {
            smLoc = point(randInt(0,m.getSize().x()-1),randInt(0,m.getSize().y()-1));
        } while(m.getBackdrop(smLoc) != SMBACKDROP_SPACE);


        maxDangerLevel = getMaxDangerLevel(smLoc);
        dangerLevel = randInt(2, maxDangerLevel);
        raceDomainType = getRaceDomainTypeFromDangerLevel(dangerLevel);
        mapSize = getMapSizeBasedOnDomainType(raceDomainType);
        attitudeTowardsPlayer = getStartingRaceAttitudeTowardsPlayer();
        std::string raceName = genName(randInt(5, 10));


        generateOneGuaranteedRace(i,
                                  dangerLevel,
                                  raceDomainType,
                                  mapSize, 
                                  smLoc, 
                                  raceType, 
                                  racePersonalityType, 
                                  raceName, 
                                  attitudeTowardsPlayer);

        m.setBackdrop(raceVector[i].getStarmapLoc(), (backdrop_t)((int)SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE + (int)raceVector[i].getStarType() - 1));
    }
}

int getStartingRaceAttitudeTowardsPlayer()
{
    if (roll(2))
    {
        return randInt(-200, -1);
    }

    return 0;
}

int getMaxDangerLevel(point smLoc)
{
    return std::min(25, 50 - (int)sqrt(distanceSquared(point(smLoc.x(), STARMAPHGT - smLoc.y() - 1), point(STARMAPWID - 1, STARMAPHGT - 1))) + 4);
}

race_domain_type getRaceDomainTypeFromDangerLevel(int dangerLevel)
{
    if (dangerLevel > 17)
    {
        return (race_domain_type)(randInt(0, 2));
    }
    else if (dangerLevel > 8)
    {
        return (race_domain_type)(randInt(0, 1));
    }

    return RACEDOMAIN_SECTOR;
}

point getMapSizeBasedOnDomainType(race_domain_type rdt)
{
    point msze;

    switch (rdt)
    {
       case (RACEDOMAIN_SECTOR):
           msze = point(randInt(8, 18) * 2, randInt(8, 18) * 2);
           break;
       case (RACEDOMAIN_DOMAIN):
           msze = point(randInt(36, 43), randInt(36, 43));
           break;
       case (RACEDOMAIN_EMPIRE):
           msze = point(randInt(43, 52), randInt(43, 52));
       default:
           break;
    }

    return msze;
}

void StarMapRegion::generateOneGuaranteedRace(int race_id, int danger_level, race_domain_type rdt, point subarea_size, point sm_loc,
                                               race_type rtype, race_personality_type rptype, std::string race_name_str, int att_status)
{
    raceVector.push_back(race(race_id,danger_level,rdt,subarea_size,sm_loc,rtype,rptype,race_name_str,att_status));
    raceVector[(int)raceVector.size()-1].generateNativeShips();
    raceVector[(int)raceVector.size()-1].setupRelationshipVectors();
}

map *StarMapRegion::getMap()
{
    return &m;
}

point StarMapRegion::getSize()
{
    return m.getSize();
}

int StarMapRegion::getSubAreaIndex(point p)
{
    auto it = std::find_if(subareaVector.begin(), subareaVector.end(),
        [&p](const auto& subarea) {
            return isAt(subarea.getSubAreaLoc(), p);
        });

    if (it != subareaVector.end())
        return static_cast<int>(std::distance(subareaVector.begin(), it));

    return -1;
}

int StarMapRegion::getRaceIndex(point p)
{
    auto it = std::find_if(raceVector.begin(), raceVector.end(),
        [&p](const auto& race) {
            return isAt(race.getStarmapLoc(), p);
        });

    if (it != raceVector.end())
        return static_cast<int>(std::distance(raceVector.begin(), it));

    return -1;
}

int StarMapRegion::getNumSubAreas()
{
    return (int)subareaVector.size();
}

int StarMapRegion::getNumRaces()
{
    return (int)raceVector.size();
}

void StarMapRegion::createSubArea(point p, subarea_MapType samt_param, subarea_specific_type sast_param, bool wild_encounter, bool race_affiliated, star_type st)
{
    samt = samt_param;

    if (samt == SMT_PERSISTENT)
    {
        createPersistentSubArea(p,race_affiliated,st,sast_param);
    }
    else
    {
        createNonPersistentSubArea(p,wild_encounter,sast_param);
    }
}

void StarMapRegion::createPersistentSubArea(point loc, bool race_affiliated, star_type st, subarea_specific_type sast_param)
{
    int subarea_idx;

    int race_idx = getRaceIndex(loc);

    int race_id = -1;

    point msze = point(randInt(18,52),randInt(18,52));

    if (race_affiliated)
    {
        race_id = raceVector[race_idx].getRaceID();
        msze = raceVector[race_idx].getSubAreaSize();
    }

    subareaVector.push_back(SubAreaRegion(race_id,race_affiliated,st));

    subarea_idx = (int)subareaVector.size() - 1;

    subareaVector[subarea_idx].initMap(msze);

    subareaVector[subarea_idx].setupSubAreaMapGenerics(loc,LBACKDROP_SPACE_LIT);

    subareaVector[subarea_idx].setSubAreaSpecificType(sast_param);

    if (race_affiliated)
    {
        subareaVector[subarea_idx].setupProcgenTerritorialSubArea(&raceVector[race_idx]);
    }
    else
    {
        subareaVector[subarea_idx].setupProcgenNonTerritorialSubArea(loc);
        if (roll(3))
        {
            subareaVector[subarea_idx].setSubAreaSpecificType(SST_WARZONE);
            populateWarZone(subarea_idx);
        }
    }
}

void StarMapRegion::populateWarZone(int subarea_idx)
{
    int num_ships = randInt(6,35);
    int max_num_races = randInt(3,8);
    int current_id = 0;
    int ns_index = 0;
    point msize = subareaVector[subarea_idx].getMap()->getSize();
    point spawn_loc;

    std::vector<int> race_ids;

    for (int i = 0; i < max_num_races; ++i)
    {
         race_ids.push_back(randInt(0,(int)raceVector.size()-1));
    }

    for (int i = 0; i < num_ships; ++i)
    {
         current_id = race_ids[randInt(0,(int)race_ids.size()-1)];
         spawn_loc = getRandNPCShipOpenPoint(subareaVector[subarea_idx].getMap(),point(1,1),addPoints(msize,point(-2,-9)),LBACKDROP_SPACE_UNLIT,LBACKDROP_WHITESTARBACKGROUND);
         ns_index = randInt(0,getRace(current_id)->getNumNativeShips()-1);
         subareaVector[subarea_idx].createNPCShip(getRace(current_id)->getNativeShip(ns_index),spawn_loc,current_id);
         subareaVector[subarea_idx].getNPCShip(spawn_loc)->setRoveChance(100);
    }

    race_ids.clear();
}

void StarMapRegion::createNonPersistentSubArea(point loc, bool wild_encounter, subarea_specific_type sast_param)
{
    point msze = wild_encounter ? point(randInt(15,SHOWWID),randInt(15,SHOWHGT)) : point(SHOWWID,SHOWHGT);

    std::string aname = wild_encounter ? "Wild Encounter" : "Subregion Void";

    nonpersistent_subarea.cleanupEverything();

    nonpersistent_subarea.setSubAreaName(aname);
    nonpersistent_subarea.getMap()->createGrid(msze);
    nonpersistent_subarea.setupSubAreaMapGenerics(loc, LBACKDROP_SPACE_UNLIT);
    nonpersistent_subarea.setSubAreaSpecificType(sast_param);
}

SubAreaRegion *StarMapRegion::getSubArea(int i)
{
    return &subareaVector[i];
}

SubAreaRegion *StarMapRegion::getNPSubArea()
{
    return &nonpersistent_subarea;
}

subarea_MapType StarMapRegion::getSubAreaMapType()
{
    return samt;
}

race *StarMapRegion::getRace(int i)
{
    return &raceVector[i];
}

void StarMapRegion::setSubAreaMapType(subarea_MapType samt_param)
{
    samt = samt_param;
}

void StarMapRegion::saveAllSubAreas(std::ofstream &os) const
{
    int temp_num_subareas = (int)subareaVector.size();
    os.write((const char *)&temp_num_subareas,sizeof(int));
    for (int i = 0; i < temp_num_subareas; ++i)
    {
        subareaVector[i].save(os);
    }
}

void StarMapRegion::loadAllSubAreas(std::ifstream &is)
{
    int temp_num_subareas = 0;
    is.read((char *)&temp_num_subareas,sizeof(int));
    std::vector < SubAreaRegion > temp_lvec(temp_num_subareas);
    subareaVector.swap(temp_lvec);
    for (int i = 0; i < temp_num_subareas; ++i)
    {
        subareaVector[i].load(is);
    }
}

void StarMapRegion::save(std::ofstream & os) const
{
    m.save(os);
    saveAllSubAreas(os);
}

void StarMapRegion::load(std::ifstream & is)
{
    m.load(is);
    loadAllSubAreas(is);
}

// star system implementation

SubAreaRegion::SubAreaRegion()
{
    subarea_name = "";
    npc_id_counter = 1;
}

SubAreaRegion::SubAreaRegion(int nrid, bool ra, star_type st)
{
    subarea_name = "";
    npc_id_counter = 1;
    nativeRaceID = nrid;
    race_affiliated = ra;
    star_radius = 5;
    s_type = st;
}

subarea_specific_type SubAreaRegion::getSubAreaSpecificType()
{
    return sast;
}

void SubAreaRegion::setSubAreaSpecificType(subarea_specific_type sast_param)
{
    sast = sast_param;
}

int SubAreaRegion::getNativeRaceID()
{
    return nativeRaceID;
}

void SubAreaRegion::initMap(point lsize)
{
    m = map(lsize);
}

void SubAreaRegion::setupSubAreaMapGenerics(point lloc, backdrop_t filler)
{
    initEmptyTiles(getMap(),filler);
    flood_fill_flags.resize(m.getSize().y(), std::vector<bool>(m.getSize().x()));
    subarea_loc = lloc;
}

void SubAreaRegion::initFloodFillFlags()
{
    for (int y = 0; y < getMap()->getSize().y(); ++y)
    for (int x = 0; x < getMap()->getSize().x(); ++x)
    {
        flood_fill_flags[y][x] = false;
    }
}

void SubAreaRegion::cleanupEverything()
{
    m.clearAllCells();
    for (int i = 0; i < getNumShipNPCs(); ++i)
    {
        NPCShips[i].cleanupEverything();
    }
    for (int i = 0; i < getNumStations(); ++i)
    {
        station_objs[i].cleanupEverything();
    }
    std::vector<MobShip>().swap(NPCShips);
    std::vector<station>().swap(station_objs);
    std::vector<std::vector<bool>>().swap(flood_fill_flags);
    deleteAllPiles();
}

void SubAreaRegion::destroyNPC(int i)
{
    NPCShips.erase(NPCShips.begin() + i);
}

map *SubAreaRegion::getMap()
{
    return &m;
}

void SubAreaRegion::setSubAreaName(std::string san_param)
{
    subarea_name = san_param;
}

std::string SubAreaRegion::getSubAreaName()
{
    return subarea_name;
}

point SubAreaRegion::getSize()
{
    return m.getSize();
}

point SubAreaRegion::getSubAreaLoc() const
{
    return subarea_loc;
}

int SubAreaRegion::getNumActiveNativeShipsPresent()
{
    auto id = nativeRaceID;
    int retVal = std::count_if(NPCShips.begin(), NPCShips.end(),
        [id](const auto& ship) {
            return ship.getMobSubAreaGroupID() == id && ship.isActivated();
        });
    return retVal;
}

int SubAreaRegion::getNumShipNPCs()
{
    return static_cast<int>(NPCShips.size());
}

int SubAreaRegion::getNumStations()
{
    return (int)station_objs.size();
}

void SubAreaRegion::addAllNPCShips(race *race_obj)
{
    int num_npc_ships = randInt(0,race_obj->getDangerLevel()) + 3;

    for (int i = 0; i < num_npc_ships; ++i)
    {
        rollOneNPC(race_obj);
    }
}

void SubAreaRegion::rollOneNPC(race *race_obj)
{
    int ship_index;

    point p;

    bool condition_freq;

    do
    {
        ship_index = randInt(0,race_obj->getNumNativeShips()-1);
        condition_freq = !rollPerc(race_obj->getNativeShip(ship_index)->getStatStruct().frequency);
    }
    while (condition_freq);

    p = getRandNPCShipOpenPoint(getMap(),point(1,1),addPoints(getMaxMapPoint(getMap()),point(-1,-1)),LBACKDROP_SPACE_UNLIT,LBACKDROP_SPACE_LIT);

    createNPCShip(race_obj->getNativeShip(ship_index),p,nativeRaceID);
}

void SubAreaRegion::createNPCShip(MobShip *added_ship,point p,int gid)
{
    int index = 0;
    NPCShips.push_back(*added_ship);
    index = (int)NPCShips.size() - 1;
    NPCShips[index].setLoc(p);
    NPCShips[index].setDestination(p);
    NPCShips[index].setInitLoc(p);
    NPCShips[index].initMobSubAreaID(npc_id_counter);
    NPCShips[index].setMobSubAreaGroupID(gid);
    NPCShips[index].setMobSubAreaAttackID(-1);
    NPCShips[index].setNumCredits((uint_64)(NPCShips[index].getStatStruct().danger_level * ((int)NPCShips[index].getStatStruct().sctype+1)) * CREDIT_DL_MULTIPLIER);
    npc_id_counter++;
    m.setMob(p,NPCShips[index].getStatStruct().mtype);
}

void SubAreaRegion::createStationAt(point p, int dl)
{
    station_objs.push_back(station(p,dl));
    int index = (int)station_objs.size() - 1;
    station_objs[index].initStation();
}

void SubAreaRegion::createEntertainmentStationAt(point p, int dl)
{
    entertainmentStations.push_back(EntertainmentStation(p, dl));
    int index = (int)entertainmentStations.size() - 1;
    entertainmentStations[index].initStation();
}

station *SubAreaRegion::getStation(int i)
{
    return &station_objs[i];
}

EntertainmentStation* SubAreaRegion::getEntertainmentCenter(int i)
{
    return &entertainmentStations[i];
}


station *SubAreaRegion::getStation(point p)
{
    for (int i = 0; i < (int)station_objs.size(); ++i)
    {
        if (isAt(station_objs[i].getSubareaLoc(),p))
            return &station_objs[i];
    }
    return &station_objs[0];
}

EntertainmentStation* SubAreaRegion::getEntertainmentCenter(point p)
{
    for (int i = 0; i < (int)entertainmentStations.size(); ++i)
    {
        if (isAt(entertainmentStations[i].getSubareaLoc(), p))
            return &entertainmentStations[i];
    }
    return &entertainmentStations[0];
}

MobShip *SubAreaRegion::getNPCShip(point p)
{
    for (int i = 0; i < (int)NPCShips.size(); ++i)
    {
        if (isAt(NPCShips[i].at(),p))
            return &NPCShips[i];
    }
    return &NPCShips[0];
}

MobShip* SubAreaRegion::getNPCShip(int i)
{
    return &NPCShips[i];
}

void SubAreaRegion::saveAllNPCShips(std::ofstream &os) const
{
    int temp_num_npcs = (int)NPCShips.size();
    os.write((const char *)&temp_num_npcs,sizeof(int));
    for (int i = 0; i < temp_num_npcs; ++i)
    {
        NPCShips[i].save(os);
    }
}

void SubAreaRegion::loadAllNPCShips(std::ifstream &is)
{
    int temp_num_npcs = 0;
    is.read((char *)&temp_num_npcs,sizeof(int));
    std::vector < MobShip > temp_npcvec(temp_num_npcs);
    NPCShips.swap(temp_npcvec);
    for (int i = 0; i < temp_num_npcs; ++i)
    {
        NPCShips[i].load(is);
    }
}

void SubAreaRegion::save(std::ofstream & os) const
{
    subarea_loc.save(os);
    stringSave(os,subarea_name);
    m.save(os);
    saveAllNPCShips(os);
}

void SubAreaRegion::load(std::ifstream & is)
{
    subarea_loc.load(is);
    stringLoad(is,subarea_name);
    m.load(is);
    loadAllNPCShips(is);
}

void SubAreaRegion::setupProcgenNonTerritorialSubArea(point loc)
{
    point map_size = getMap()->getSize();
    int sze = randInt(4,5);
    generateStarBackdrop(getMap(),divPoint(map_size,2,2),sze,s_type);
    subarea_name = "Sector (" + int2String(loc.x()+1) + "," + int2String(STARMAPHGT-loc.y()) + ")";
}

void SubAreaRegion::setupProcgenTerritorialSubArea(race *race_obj)
{
    placeInitialSpaceWallConfiguration(race_obj->getRaceDomainType());

    evolveSpaceWallCA();

    smoothenSpaceWallStructure();

    addProcgenSubAreaNativeShips(race_obj,race_obj->getNumNativeShips());

    addAllHomeworlds(race_obj);

    generateAllSpaceStations(race_obj->getDangerLevel(),randInt(1,randInt(1,randInt(1,4))));

    generateAllEntertainmentStations(race_obj->getDangerLevel(), randInt(0, randInt(0, randInt(1, 3))));

    subarea_name = race_obj->getNameString();
}

void SubAreaRegion::smoothenSpaceWallStructure()
{
    backdrop_t bdt;
    point p;
    std::vector<point> cut_locs;
    int num_global_cuts = getNumGlobalCutIterations(getMap()->getSize());
    for (int i = 0; i < num_global_cuts; ++i)
    {
        for (int x = 2; x < getMap()->getSize().x() - 2; ++x)
        for (int y = 2; y < getMap()->getSize().y() - 2; ++y)
        {
            p = point(x,y);

            bdt = getMap()->getBackdrop(p);

            if (bdt == LBACKDROP_SPACEWALL && numBackdropCellsAdj(getMap(),p,LBACKDROP_SPACE_LIT,1,true) >= 3)
                cut_locs.push_back(p);
        }

        for (int c = 0; c < (int)cut_locs.size(); ++c)
            getMap()->setBackdrop(cut_locs[c],LBACKDROP_SPACE_LIT);

        cut_locs.clear();
    }

    std::vector<point>().swap(cut_locs);
}

void SubAreaRegion::placeInitialSpaceWallConfiguration(race_domain_type rdt)
{
    int sze;

    if (rdt == RACEDOMAIN_SECTOR)
        sze = randInt(1,3);
    if (rdt == RACEDOMAIN_DOMAIN)
        sze = randInt(2,11);
    if (rdt == RACEDOMAIN_EMPIRE)
        sze = randInt(3,16);

    point div_point = divPoint(getMap()->getSize(),2,2);

    if (roll(2))
    {
        checkAddGiantSpaceWallCircle(getMap(),div_point,sze);
        if (getMap()->getSize().x() % 2 == 0)
            checkAddGiantSpaceWallCircle(getMap(),addPoints(div_point,point(-1,0)),sze);
        if (getMap()->getSize().y() % 2 == 0)
            checkAddGiantSpaceWallCircle(getMap(),addPoints(div_point,point(0,-1)),sze);
        if (getMap()->getSize().x() % 2 == 0 && getMap()->getSize().y() % 2 == 0)
            checkAddGiantSpaceWallCircle(getMap(),addPoints(div_point,point(-1,-1)),sze);
    }
    else
    {
        checkAddGiantSpaceWallCube(getMap(),div_point,sze);
        if (getMap()->getSize().x() % 2 == 0)
            checkAddGiantSpaceWallCube(getMap(),addPoints(div_point,point(-1,0)),sze);
        if (getMap()->getSize().y() % 2 == 0)
            checkAddGiantSpaceWallCube(getMap(),addPoints(div_point,point(0,-1)),sze);
        if (getMap()->getSize().x() % 2 == 0 && getMap()->getSize().y() % 2 == 0)
            checkAddGiantSpaceWallCube(getMap(),addPoints(div_point,point(-1,-1)),sze);
    }

}

void SubAreaRegion::evolveSpaceWallCA()
{
    int sze = 5;

    point div_point = divPoint(getMap()->getSize(),2,2);

    apply2468Rule(getMap(),getNumCAGens(getMap()->getSize()));

    do
    {
        generateStarBackdrop(getMap(),div_point,sze,s_type);

        initFloodFillFlags();

        executeFloodFill(point(0,0));

        sze++;
    } while (isLevelBlocked());

    star_radius = sze;
}

void SubAreaRegion::generateAllSpaceStations(int dl, int num_space_stations)
{
    point loc;

    point min_loc = point(1,1);

    point max_loc = addPoints(getMap()->getSize(),point(-2,-2));

    for (int i = 0; i < num_space_stations; ++i)
    {
         loc = getRandNPCShipOpenPoint(getMap(),min_loc,max_loc,LBACKDROP_SPACE_UNLIT,LBACKDROP_WHITESTARBACKGROUND);
         getMap()->setBackdrop(loc, LBACKDROP_SPACESTATION_SHIP);
         createStationAt(loc, dl);
    }
}

void SubAreaRegion::generateAllEntertainmentStations(int dl, int numStations)
{
    point loc;

    point minLoc = point(1, 1);

    point maxLoc = addPoints(getMap()->getSize(), point(-2, -2));

    for (int i = 0; i < numStations; i++)
    {
        loc = getRandNPCShipOpenPoint(getMap(), minLoc, maxLoc, LBACKDROP_SPACE_UNLIT, LBACKDROP_WHITESTARBACKGROUND);
        getMap()->setBackdrop(loc, LBACKDROP_SPACESTATION_ENTERTAINMENT);
        createEntertainmentStationAt(loc, dl);
    }
}

// item code
void SubAreaRegion::createPileAt(point p)
{
    piles.push_back(cell_pile(p));
}

void SubAreaRegion::deleteItemFromPile(point p, int selection, int select_q)
{
    cell_pile * cp = getPile(p);

    cp->removeItem(selection,select_q);

    if (cp->getNumItems() == 0)
    {
        deletePileAt(p);
        m.setItem(p,NIL_i);
    }
    else
        m.setItem(p,cp->getItem(0)->getType());
}

void SubAreaRegion::deletePileAt(point p)
{
    int piles_index = -1;

    for (uint i = 0; i < piles.size(); ++i)
        if (isAt(piles[i].getLoc(),p))
            piles_index = i;

    if (piles_index != -1)
        piles.erase(piles.begin() + piles_index);
}

void SubAreaRegion::addItem(item_stats istats, int quant, point p)
{
    int piles_index = -1;

    int num_piles = piles.size();

    for (int i = 0; i < num_piles; ++i)
        if (isAt(piles[i].getLoc(),p))
            piles_index = i;

    if (piles_index != -1)
        piles[piles_index].addToContainer(istats,quant);
    else
    {
        createPileAt(p);
        piles[num_piles].addToContainer(istats,quant);
        m.setItem(p,istats.i_type);
    }
}

bool SubAreaRegion::isPileAt(point p)
{
    for (uint i = 0; i < piles.size(); ++i)
        if (isAt(piles[i].getLoc(),p))
            return true;

    return false;
}

int SubAreaRegion::getNumItemPiles()
{
    return piles.size();
}

cell_pile *SubAreaRegion::getPile(int i)
{
    return &piles[i];
}

cell_pile *SubAreaRegion::getPile(point p)
{
    for (uint i = 0; i < piles.size(); ++i)
        if (isAt(piles[i].getLoc(),p))
            return &piles[i];

    // should never happen
    return &piles[0];
}

void SubAreaRegion::deleteAllPiles()
{
    for (uint i = 0; i < piles.size(); ++i)
        piles[i].deleteAllItems();

    std::vector<cell_pile>().swap(piles);
}

void SubAreaRegion::executeFloodFill(point p)
{
    if (!inMapRange(p,getMap()->getSize()))
        return;

    if (getMap()->getBackdrop(p) != LBACKDROP_SPACEWALL && !flood_fill_flags[p.y()][p.x()])
        flood_fill_flags[p.y()][p.x()] = true;
    else
        return;

    executeFloodFill(point(p.x()+1,p.y()));
    executeFloodFill(point(p.x()-1,p.y()));
    executeFloodFill(point(p.x(),p.y()+1));
    executeFloodFill(point(p.x(),p.y()-1));
    executeFloodFill(point(p.x()+1,p.y()+1));
    executeFloodFill(point(p.x()-1,p.y()-1));
    executeFloodFill(point(p.x()-1,p.y()+1));
    executeFloodFill(point(p.x()+1,p.y()-1));
}

bool SubAreaRegion::isLevelBlocked()
{
    point p;
    for (int y = 0; y < getMap()->getSize().y(); ++y)
    for (int x = 0; x < getMap()->getSize().x(); ++x)
    {
        p.set(x,y);
        if (getMap()->getBackdrop(p) != LBACKDROP_SPACEWALL && !flood_fill_flags[y][x])
        {
            return true;
        }
    }
    return false;
}

bool SubAreaRegion::isRaceAffiliated()
{
    return race_affiliated;
}

void SubAreaRegion::addAllHomeworlds(race *race_obj)
{
    // rewrite this
    point p;
    point smp;
    int num_ships_before_hw_added = getNumShipNPCs();
    int num_homeworlds = randInt(1,randInt(1,4));
    int num_native_ship_types = race_obj->getNumNativeShips();
    point maxp;
    point minp;
    point msze = getMap()->getSize();
    for (int i = 0; i < num_homeworlds; ++i)
    {
        minp = point(msze.x()/2 - star_radius,msze.y()/2 - star_radius);
        maxp = point(msze.x()/2 + star_radius,msze.y()/2 + star_radius);
        p = getRandNPCShipOpenPoint(getMap(),minp,maxp,LBACKDROP_MAINSEQSTARBACKGROUND,LBACKDROP_WHITESTARBACKGROUND);
        insertOneHomeworld(race_obj,p);
        if (num_ships_before_hw_added == 0 && num_native_ship_types > 0)
        {
            if (p.y() >= getMap()->getSize().y()/2)
                smp = point(p.x(),p.y()-1);
            else
                smp = point(p.x(),p.y()+1);

            createNPCShip(race_obj->getNativeShip(num_native_ship_types - 1),smp,nativeRaceID);
        }
    }
}

void SubAreaRegion::insertOneHomeworld(race *race_obj, point p)
{
    getMap()->setBackdrop(p,LBACKDROP_PLANET);
    race_obj->addHomeworldStruct(p,race_obj->getRaceID(),RMS_FREE);
}

void SubAreaRegion::addProcgenSubAreaNativeShips(race *race_obj, int num_types)
{
    if (num_types == 0)
        return;

    int condition_offset = randInt(0,3);

    point p, adj;

    int num_wall_adj_moore;

    for (int y = 3; y < getMap()->getSize().y() - 3; ++y)
    for (int x = 3; x < getMap()->getSize().x() - 3; ++x)
    {
        p.set(x,y);
        if (m.getBackdrop(p) == LBACKDROP_SPACE_LIT && !isAt(p,getSubAreaSystemStartPoint(getMap()->getSize())))
        {
            num_wall_adj_moore = 0;
            for (int dy = -3; dy <= 3; ++dy)
            for (int dx = -3; dx <= 3; ++dx)
            {
                adj.set(dx+x,dy+y);
                if (dy != 0 || dx != 0)
                if (m.getBackdrop(adj) == LBACKDROP_SPACEWALL)
                {
                    num_wall_adj_moore++;
                }
            }

            for (int i = 0; i < 7; ++i)
            {
                if (num_wall_adj_moore == npc_spawn_conditions[(int)race_obj->getRaceDomainType()][i] + condition_offset && num_types >= i + 1)
                    createNPCShip(race_obj->getNativeShip(i),p,nativeRaceID);
            }
        }
    }
}

void initEmptyTiles(map * m, backdrop_t filler)
{
    point p;

    for (int x = 0; x < m->getSize().x(); ++x)
    for (int y = 0; y < m->getSize().y(); ++y)
    {
        p.set(x,y);
        m->setAllCellValuesToNIL(p);
        //m->setm(p,true);
        m->setBackdrop(p,filler);
    }
}

point getRandNPCShipOpenPoint(map *m, point minp, point maxp, backdrop_t bd_lowest, backdrop_t bd_highest)
{
    if (minp.x() < 1)
        minp.setx(1);
    if (minp.y() < 1)
        minp.sety(1);
    if (maxp.x() > m->getSize().x() - 2)
        maxp.setx(m->getSize().x() - 2);
    if (maxp.y() > m->getSize().y() - 2)
        maxp.sety(m->getSize().y() - 2);

    int tries = 0;

    point ret_val = point(0,0);

    do
    {
        ret_val.set(randInt(minp.x(),maxp.x()),randInt(minp.y(),maxp.y()));
        tries++;
    } while ((isSpawnBlockingCell(m->getCell(ret_val),bd_lowest,bd_highest) && tries < 1000) || isAt(ret_val,getSubAreaSystemStartPoint(m->getSize())));

    return ret_val;
}

void checkAddGiantSpaceWallCircle(map *m, point loc, int sze)
{
    if (!inRange(point(loc.x()-sze,loc.y()-sze),point(1,1),addPoints(m->getSize(),point(-2,-2))) ||
        !inRange(point(loc.x()+sze,loc.y()+sze),point(1,1),addPoints(m->getSize(),point(-2,-2))))
        return;

    point p;

    int x_range;

    for (int y = -sze; y <= sze; ++y)
    {
        x_range = (int)std::sqrt(((double)((sze * sze) - (y * y))));
        for (int x = -x_range; x <= x_range; ++x)
        {
            p = addPoints(point(x,y),loc);

            m->setBackdrop(p,LBACKDROP_SPACEWALL);
        }
    }
}

void checkAddGiantSpaceWallCube(map *m, point loc, int sze)
{
    if (!inRange(point(loc.x()-sze,loc.y()-sze),point(1,1),addPoints(m->getSize(),point(-2,-2))) ||
        !inRange(point(loc.x()+sze,loc.y()+sze),point(1,1),addPoints(m->getSize(),point(-2,-2))))
        return;

    point p;

    for (int i = -sze; i <= sze; ++i)
    for (int j = -sze; j <= sze; ++j)
    {
        p = addPoints(loc,point(i,j));
        m->setBackdrop(p,LBACKDROP_SPACEWALL);
    }
}


void apply2468Rule(map *m, int num_gens)
{
    int num_wall_adj = 0;

    std::vector<std::vector<bool>> wall_flags;
    wall_flags.resize(m->getSize().y(), std::vector<bool>(m->getSize().x()));

    point p;
    point adj;

    for (int y = 0; y < m->getSize().y(); ++y)
    for (int x = 0; x < m->getSize().x(); ++x)
    {
        p.set(x,y);
        if (m->getBackdrop(point(x,y)) == LBACKDROP_SPACEWALL)
            wall_flags[y][x] = true;
        else
            wall_flags[y][x] = false;
    }

    for (int i = 0; i < num_gens; ++i)
    {
        for (int y = 3; y < m->getSize().y() - 3; ++y)
        for (int x = 3; x < m->getSize().x() - 3; ++x)
        {
            p.set(x,y);
            num_wall_adj = 0;

            for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx)
            {
                adj.set(dx+x,dy+y);
                if (dy != 0 || dx != 0)
                if (m->getBackdrop(adj) == LBACKDROP_SPACEWALL)
                    num_wall_adj++;
            }

            if (num_wall_adj == 2 || num_wall_adj == 4 || num_wall_adj == 6 || num_wall_adj == 8)
                wall_flags[y][x] = true;
            else
                wall_flags[y][x] = false;
        }

        for (int y = 3; y < m->getSize().y() - 3; ++y)
        for (int x = 3; x < m->getSize().x() - 3; ++x)
        {
            p.set(x,y);
            if (wall_flags[y][x])
                m->setBackdrop(p,LBACKDROP_SPACEWALL);
            else
                m->setBackdrop(p,LBACKDROP_SPACE_LIT);
        }
    }

    std::vector<std::vector<bool>>().swap(wall_flags);
}

void generateStarBackdrop(map *m, point star_loc, int star_radius, star_type st)
{
    backdrop_t star_bdt;

    switch(st)
    {
        case(STARTYPE_MAINSEQ):
             star_bdt = LBACKDROP_MAINSEQSTARBACKGROUND;
             break;
        case(STARTYPE_RED):
             star_bdt = LBACKDROP_REDSTARBACKGROUND;
             break;
        case(STARTYPE_BLUE):
             star_bdt = LBACKDROP_BLUESTARBACKGROUND;
             break;
        case(STARTYPE_WHITE):
             star_bdt = LBACKDROP_WHITESTARBACKGROUND;
             break;
        default:
             return;
    }

    generateGivenStarBackdrop(m,star_loc,star_radius,star_bdt);
}

void generateGivenStarBackdrop(map * m, point star_loc, int star_radius, backdrop_t star_bdt)
{
    carveBackdropCircle(m,star_loc,star_radius,LBACKDROP_SPACE_LIT);
    if (m->getSize().x() % 2 == 0)
        carveBackdropCircle(m,addPoints(star_loc,point(-1,0)),star_radius,LBACKDROP_SPACE_LIT);
    if (m->getSize().y() % 2 == 0)
        carveBackdropCircle(m,addPoints(star_loc,point(0,-1)),star_radius,LBACKDROP_SPACE_LIT);
    if (m->getSize().x() % 2 == 0 && m->getSize().y() % 2 == 0)
        carveBackdropCircle(m,addPoints(star_loc,point(-1,-1)),star_radius,LBACKDROP_SPACE_LIT);

    carveBackdropCircle(m,star_loc,star_radius - 3,star_bdt);
    if (m->getSize().x() % 2 == 0)
        carveBackdropCircle(m,addPoints(star_loc,point(-1,0)),star_radius - 3,star_bdt);
    if (m->getSize().y() % 2 == 0)
        carveBackdropCircle(m,addPoints(star_loc,point(0,-1)),star_radius - 3,star_bdt);
    if (m->getSize().x() % 2 == 0 && m->getSize().y() % 2 == 0)
        carveBackdropCircle(m,addPoints(star_loc,point(-1,-1)),star_radius - 3,star_bdt);
}

void carveBackdropCircle(map * m, point loc, int rad, backdrop_t bt)
{
    point p;

    int x_range;

    for (int y = -rad; y <= rad; ++y)
    {
        x_range = (int)std::sqrt(((double)((rad * rad) - (y * y))));
        for (int x = -x_range; x <= x_range; ++x)
        {
            p = addPoints(point(x,y),loc);

            if (inMapRange(p,m->getSize()))
            {
                m->setBackdrop(p,bt);
            }
        }
    }
}

int getNumCAGens(point msze)
{
    int min_coord = (int)std::min(msze.x(),msze.y());

    if (min_coord <= 25)
        return randInt(3,25);
    else if (min_coord <= 40)
        return randInt(15,35);

    return randInt(25,45);
}

int getNumGlobalCutIterations(point msze)
{
    int min_coord = (int)std::min(msze.x(),msze.y());

    if (min_coord <= 18)
        return 1;
    else if (min_coord <= 36)
        return randInt(1,2);

    return randInt(1,3);
}

map* getMap()
{
    switch (current_maptype)
    {
    case(MAPTYPE_LOCALEMAP):
    {
        return CSYS->getMap();
    }
    default:
    {
        break;
    }
    }

    return universe.getMap();
}

point getMapSize()
{
    switch (current_maptype)
    {
    case(MAPTYPE_LOCALEMAP):
    {
        return CSYS->getSize();
    }
    default:
    {
        break;
    }
    }

    return universe.getSize();
}

MobShip* getSubAreaShipMobAt(point p)
{
    if (getMap()->getMob(p) == SHIP_PLAYER)
        return getPlayerShip();
    return CSYS->getNPCShip(p);
}

MobShip* getMobFromID(int id)
{
    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        if (CSYS->getNPCShip(i)->getMobSubAreaID() == id)
            return CSYS->getNPCShip(i);
    }

    return getPlayerShip();
}

MobShip* getCurrentMobTurn()
{
    for (int i = 0; i < CSYS->getNumShipNPCs(); ++i)
    {
        if (CSYS->getNPCShip(i)->getMobSubAreaID() == current_mob_turn)
            return CSYS->getNPCShip(i);
    }

    return getPlayerShip();
}
