#include "region.h"

// starmap implementation

starmap_region::starmap_region()
{

}

void starmap_region::setupUniverse()
{
    m = map(point(STARMAPWID,STARMAPHGT));
    initEmptyTiles(&m,SMBACKDROP_SPACE);
    generateAllRaces();
    point gen_loc;
    for (int i = 0; i < 167; ++i)
    {
        do
        {
            gen_loc = point(randInt(0,m.getSize().x()-1),randInt(0,m.getSize().y()-1));
        }while(m.getBackdrop(gen_loc) != SMBACKDROP_SPACE);
        m.setBackdrop(gen_loc,(backdrop_t)randInt((int)SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE,(int)SMBACKDROP_WHITESTARSUBAREAENTRANCE));
    }
    nonpersistent_subarea = subarea_region(-1,false,STARTYPE_NONE);
    nonpersistent_subarea.initMap(point(15,15));
}

void starmap_region::cleanupEverything()
{
    m.clearAllCells();
    nonpersistent_subarea.cleanupEverything();
    for (int i = 0; i < (int)subarea_vec.size(); ++i)
         subarea_vec[i].cleanupEverything();
    for (int i = 0; i < (int)race_vec.size(); ++i)
         race_vec[i].cleanupEverything();
    std::vector<subarea_region>().swap(subarea_vec);
    std::vector<race>().swap(race_vec);
}

void starmap_region::generateAllRaces()
{
    point p;
    point msze;
    int danger_level;
    int atp = 0;
    race_domain_type rdt;
    for (int i = 0; i < NUM_TOTAL_RACES; ++i)
    {
        do
        {
            p = point(randInt(0,m.getSize().x()-1),randInt(0,m.getSize().y()-1));
        }while(m.getBackdrop(p) != SMBACKDROP_SPACE);

        if (roll(3))
            danger_level = randInt(2,25);
        else
            danger_level = randInt(2,14);

        if (danger_level > 17)
            rdt = (race_domain_type)(randInt(0,2));
        else if (danger_level > 8)
            rdt = (race_domain_type)(randInt(0,1));
        else
            rdt = RACEDOMAIN_SECTOR;

        if (rdt == RACEDOMAIN_SECTOR)
            msze = point(randInt(8,18)*2,randInt(8,18)*2);
        if (rdt == RACEDOMAIN_DOMAIN)
            msze = point(randInt(36,43),randInt(36,43));
        if (rdt == RACEDOMAIN_EMPIRE)
            msze = point(randInt(43,52),randInt(43,52));

        if (roll(2))
            atp = randInt(-200,-1);
        else
            atp = 0;

        generateOneGuaranteedRace(i,danger_level,rdt,msze,p,RACETYPE_PROCGEN,PERSONALITY_FRIENDLY_GUARDED,genName(randInt(5,10)),atp);
        m.setBackdrop(race_vec[i].getStarmapLoc(),(backdrop_t)((int)SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE + (int)race_vec[i].getStarType() - 1));
    }
}

void starmap_region::generateOneGuaranteedRace(int race_id, int danger_level, race_domain_type rdt, point subarea_size, point sm_loc,
                                               race_type rtype, race_personality_type rptype, std::string race_name_str, int att_status)
{
    race_vec.push_back(race(race_id,danger_level,rdt,subarea_size,sm_loc,rtype,rptype,race_name_str,att_status));
    race_vec[(int)race_vec.size()-1].generateNativeShips();
    race_vec[(int)race_vec.size()-1].setupRelationshipVectors();
}

map *starmap_region::getMap()
{
    return &m;
}

point starmap_region::getSize()
{
    return m.getSize();
}

int starmap_region::getSubAreaIndex(point p)
{
    for (int i = 0; i < (int)subarea_vec.size(); ++i)
    {
        if (isAt(subarea_vec[i].getSubAreaLoc(),p))
            return i;
    }
    return -1;
}

int starmap_region::getRaceIndex(point p)
{
    for (int i = 0; i < (int)race_vec.size(); ++i)
    {
        if (isAt(race_vec[i].getStarmapLoc(),p))
            return i;
    }
    return -1;
}

int starmap_region::getNumSubAreas()
{
    return (int)subarea_vec.size();
}

int starmap_region::getNumRaces()
{
    return (int)race_vec.size();
}

void starmap_region::createSubArea(point p, subarea_map_type samt_param, subarea_specific_type sast_param, bool wild_encounter, bool race_affiliated, star_type st)
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

void starmap_region::createPersistentSubArea(point loc, bool race_affiliated, star_type st, subarea_specific_type sast_param)
{
    int subarea_idx;

    int race_idx = getRaceIndex(loc);

    int race_id = -1;

    point msze = point(randInt(18,52),randInt(18,52));

    if (race_affiliated)
    {
        race_id = race_vec[race_idx].getRaceID();
        msze = race_vec[race_idx].getSubAreaSize();
    }

    subarea_vec.push_back(subarea_region(race_id,race_affiliated,st));

    subarea_idx = (int)subarea_vec.size() - 1;

    subarea_vec[subarea_idx].initMap(msze);

    subarea_vec[subarea_idx].setupSubAreaMapGenerics(loc,LBACKDROP_SPACE_LIT);

    subarea_vec[subarea_idx].setSubAreaSpecificType(sast_param);

    if (race_affiliated)
    {
        subarea_vec[subarea_idx].setupProcgenTerritorialSubArea(&race_vec[race_idx]);
    }
    else
    {
        subarea_vec[subarea_idx].setupProcgenNonTerritorialSubArea(loc);
        if (roll(3))
        {
            subarea_vec[subarea_idx].setSubAreaSpecificType(SST_WARZONE);
            populateWarZone(subarea_idx);
        }
    }
}

void starmap_region::populateWarZone(int subarea_idx)
{
    int num_ships = randInt(6,35);
    int max_num_races = randInt(3,8);
    int current_id = 0;
    int ns_index = 0;
    point msize = subarea_vec[subarea_idx].getMap()->getSize();
    point spawn_loc;

    std::vector<int> race_ids;

    for (int i = 0; i < max_num_races; ++i)
    {
         race_ids.push_back(randInt(0,(int)race_vec.size()-1));
    }

    for (int i = 0; i < num_ships; ++i)
    {
         current_id = race_ids[randInt(0,(int)race_ids.size()-1)];
         spawn_loc = getRandNPCShipOpenPoint(subarea_vec[subarea_idx].getMap(),point(1,1),addPoints(msize,point(-2,-9)),LBACKDROP_SPACE_UNLIT,LBACKDROP_WHITESTARBACKGROUND);
         ns_index = randInt(0,getRace(current_id)->getNumNativeShips()-1);
         subarea_vec[subarea_idx].createNPCShip(getRace(current_id)->getNativeShip(ns_index),spawn_loc,current_id);
         subarea_vec[subarea_idx].getNPCShip(spawn_loc)->setRoveChance(100);
    }

    race_ids.clear();
}

void starmap_region::createNonPersistentSubArea(point loc, bool wild_encounter, subarea_specific_type sast_param)
{
    point msze = wild_encounter ? point(randInt(15,SHOWWID),randInt(15,SHOWHGT)) : point(SHOWWID,SHOWHGT);

    std::string aname = wild_encounter ? "Wild Encounter" : "Subregion Void";

    nonpersistent_subarea.cleanupEverything();

    nonpersistent_subarea.setSubAreaName(aname);
    nonpersistent_subarea.getMap()->createGrid(msze);
    nonpersistent_subarea.setupSubAreaMapGenerics(loc, LBACKDROP_SPACE_UNLIT);
    nonpersistent_subarea.setSubAreaSpecificType(sast_param);
}

subarea_region *starmap_region::getSubArea(int i)
{
    return &subarea_vec[i];
}

subarea_region *starmap_region::getNPSubArea()
{
    return &nonpersistent_subarea;
}

subarea_map_type starmap_region::getSubAreaMapType()
{
    return samt;
}

race *starmap_region::getRace(int i)
{
    return &race_vec[i];
}

void starmap_region::setSubAreaMapType(subarea_map_type samt_param)
{
    samt = samt_param;
}

void starmap_region::saveAllSubAreas(std::ofstream &os) const
{
    int temp_num_subareas = (int)subarea_vec.size();
    os.write((const char *)&temp_num_subareas,sizeof(int));
    for (int i = 0; i < temp_num_subareas; ++i)
    {
        subarea_vec[i].save(os);
    }
}

void starmap_region::loadAllSubAreas(std::ifstream &is)
{
    int temp_num_subareas = 0;
    is.read((char *)&temp_num_subareas,sizeof(int));
    std::vector < subarea_region > temp_lvec(temp_num_subareas);
    subarea_vec.swap(temp_lvec);
    for (int i = 0; i < temp_num_subareas; ++i)
    {
        subarea_vec[i].load(is);
    }
}

void starmap_region::save(std::ofstream & os) const
{
    m.save(os);
    saveAllSubAreas(os);
}

void starmap_region::load(std::ifstream & is)
{
    m.load(is);
    loadAllSubAreas(is);
}

// star system implementation

subarea_region::subarea_region()
{
    subarea_name = "";
    npc_id_counter = 1;
}

subarea_region::subarea_region(int nrid, bool ra, star_type st)
{
    subarea_name = "";
    npc_id_counter = 1;
    native_race_id = nrid;
    race_affiliated = ra;
    star_radius = 5;
    s_type = st;
}

subarea_specific_type subarea_region::getSubAreaSpecificType()
{
    return sast;
}

void subarea_region::setSubAreaSpecificType(subarea_specific_type sast_param)
{
    sast = sast_param;
}

int subarea_region::getNativeRaceID()
{
    return native_race_id;
}

void subarea_region::initMap(point lsize)
{
    m = map(lsize);
}

void subarea_region::setupSubAreaMapGenerics(point lloc, backdrop_t filler)
{
    initEmptyTiles(getMap(),filler);
    flood_fill_flags.resize(m.getSize().y(), std::vector<bool>(m.getSize().x()));
    subarea_loc = lloc;
}

void subarea_region::initFloodFillFlags()
{
    for (int y = 0; y < getMap()->getSize().y(); ++y)
    for (int x = 0; x < getMap()->getSize().x(); ++x)
    {
        flood_fill_flags[y][x] = false;
    }
}

void subarea_region::cleanupEverything()
{
    m.clearAllCells();
    for (int i = 0; i < getNumShipNPCs(); ++i)
    {
        NPC_ships[i].cleanupEverything();
    }
    for (int i = 0; i < getNumStations(); ++i)
    {
        station_objs[i].cleanupEverything();
    }
    std::vector<ship_mob>().swap(NPC_ships);
    std::vector<station>().swap(station_objs);
    std::vector<std::vector<bool>>().swap(flood_fill_flags);
    deleteAllPiles();
}

void subarea_region::destroyNPC(int i)
{
    NPC_ships.erase(NPC_ships.begin() + i);
}

map *subarea_region::getMap()
{
    return &m;
}

void subarea_region::setSubAreaName(std::string san_param)
{
    subarea_name = san_param;
}

std::string subarea_region::getSubAreaName()
{
    return subarea_name;
}

point subarea_region::getSize()
{
    return m.getSize();
}

point subarea_region::getSubAreaLoc()
{
    return subarea_loc;
}

int subarea_region::getNumActiveNativeShipsPresent()
{
    int ret_val = 0;
    for (int i = 0; i < (int)NPC_ships.size(); ++i)
    {
        if (NPC_ships[i].getMobSubAreaGroupID() == native_race_id && NPC_ships[i].isActivated())
            ret_val++;
    }
    return ret_val;
}

int subarea_region::getNumShipNPCs()
{
    return (int)NPC_ships.size();
}

int subarea_region::getNumStations()
{
    return (int)station_objs.size();
}

void subarea_region::addAllNPCShips(race *race_obj)
{
    int num_npc_ships = randInt(0,race_obj->getDangerLevel()) + 3;

    for (int i = 0; i < num_npc_ships; ++i)
    {
        rollOneNPC(race_obj);
    }
}

void subarea_region::rollOneNPC(race *race_obj)
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

    createNPCShip(race_obj->getNativeShip(ship_index),p,native_race_id);
}

void subarea_region::createNPCShip(ship_mob *added_ship,point p,int gid)
{
    int index = 0;
    NPC_ships.push_back(*added_ship);
    index = (int)NPC_ships.size() - 1;
    NPC_ships[index].setLoc(p);
    NPC_ships[index].setDestination(p);
    NPC_ships[index].setInitLoc(p);
    NPC_ships[index].initMobSubAreaID(npc_id_counter);
    NPC_ships[index].setMobSubAreaGroupID(gid);
    NPC_ships[index].setMobSubAreaAttackID(-1);
    NPC_ships[index].setNumCredits((uint_64)(NPC_ships[index].getStatStruct().danger_level * ((int)NPC_ships[index].getStatStruct().sctype+1)) * CREDIT_DL_MULTIPLIER);
    npc_id_counter++;
    m.setMob(p,NPC_ships[index].getStatStruct().mtype);
}

void subarea_region::createStationAt(point p, int dl)
{
    station_objs.push_back(station(p,dl));
    int index = (int)station_objs.size() - 1;
    station_objs[index].initStation();
}

station *subarea_region::getStation(int i)
{
    return &station_objs[i];
}

ship_mob *subarea_region::getNPCShip(int i)
{
    return &NPC_ships[i];
}

station *subarea_region::getStation(point p)
{
    for (int i = 0; i < (int)station_objs.size(); ++i)
    {
        if (isAt(station_objs[i].getSubareaLoc(),p))
            return &station_objs[i];
    }
    return &station_objs[0];
}

ship_mob *subarea_region::getNPCShip(point p)
{
    for (int i = 0; i < (int)NPC_ships.size(); ++i)
    {
        if (isAt(NPC_ships[i].at(),p))
            return &NPC_ships[i];
    }
    return &NPC_ships[0];
}

void subarea_region::saveAllNPCShips(std::ofstream &os) const
{
    int temp_num_npcs = (int)NPC_ships.size();
    os.write((const char *)&temp_num_npcs,sizeof(int));
    for (int i = 0; i < temp_num_npcs; ++i)
    {
        NPC_ships[i].save(os);
    }
}

void subarea_region::loadAllNPCShips(std::ifstream &is)
{
    int temp_num_npcs = 0;
    is.read((char *)&temp_num_npcs,sizeof(int));
    std::vector < ship_mob > temp_npcvec(temp_num_npcs);
    NPC_ships.swap(temp_npcvec);
    for (int i = 0; i < temp_num_npcs; ++i)
    {
        NPC_ships[i].load(is);
    }
}

void subarea_region::save(std::ofstream & os) const
{
    subarea_loc.save(os);
    stringSave(os,subarea_name);
    m.save(os);
    saveAllNPCShips(os);
}

void subarea_region::load(std::ifstream & is)
{
    subarea_loc.load(is);
    stringLoad(is,subarea_name);
    m.load(is);
    loadAllNPCShips(is);
}

void subarea_region::setupProcgenNonTerritorialSubArea(point loc)
{
    point map_size = getMap()->getSize();
    int sze = randInt(4,5);
    generateStarBackdrop(getMap(),divPoint(map_size,2,2),sze,s_type);
    subarea_name = "Sector (" + int2String(loc.x()+1) + "," + int2String(STARMAPHGT-loc.y()) + ")";
}

void subarea_region::setupProcgenTerritorialSubArea(race *race_obj)
{
    placeInitialSpaceWallConfiguration(race_obj->getRaceDomainType());

    evolveSpaceWallCA();

    smoothenSpaceWallStructure();

    addProcgenSubAreaNativeShips(race_obj,race_obj->getNumNativeShips());

    addAllHomeworlds(race_obj);

    generateAllSpaceStations(race_obj->getDangerLevel(),randInt(1,randInt(1,randInt(1,4))));

    subarea_name = race_obj->getNameString();
}

void subarea_region::smoothenSpaceWallStructure()
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

void subarea_region::placeInitialSpaceWallConfiguration(race_domain_type rdt)
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

void subarea_region::evolveSpaceWallCA()
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

void subarea_region::generateAllSpaceStations(int dl, int num_space_stations)
{
    point loc;

    point min_loc = point(1,1);

    point max_loc = addPoints(getMap()->getSize(),point(-2,-2));

    for (int i = 0; i < num_space_stations; ++i)
    {
         loc = getRandNPCShipOpenPoint(getMap(),min_loc,max_loc,LBACKDROP_SPACE_UNLIT,LBACKDROP_WHITESTARBACKGROUND);
         getMap()->setBackdrop(loc,LBACKDROP_SPACESTATION);
         createStationAt(loc,dl);
    }

}

// item code
void subarea_region::createPileAt(point p)
{
    piles.push_back(cell_pile(p));
}

void subarea_region::deleteItemFromPile(point p, int selection, int select_q)
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

void subarea_region::deletePileAt(point p)
{
    int piles_index = -1;

    for (uint i = 0; i < piles.size(); ++i)
        if (isAt(piles[i].getLoc(),p))
            piles_index = i;

    if (piles_index != -1)
        piles.erase(piles.begin() + piles_index);
}

void subarea_region::addItem(item_stats istats, int quant, point p)
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

bool subarea_region::isPileAt(point p)
{
    for (uint i = 0; i < piles.size(); ++i)
        if (isAt(piles[i].getLoc(),p))
            return true;

    return false;
}

int subarea_region::getNumItemPiles()
{
    return piles.size();
}

cell_pile *subarea_region::getPile(int i)
{
    return &piles[i];
}

cell_pile *subarea_region::getPile(point p)
{
    for (uint i = 0; i < piles.size(); ++i)
        if (isAt(piles[i].getLoc(),p))
            return &piles[i];

    // should never happen
    return &piles[0];
}

void subarea_region::deleteAllPiles()
{
    for (uint i = 0; i < piles.size(); ++i)
        piles[i].deleteAllItems();

    std::vector<cell_pile>().swap(piles);
}

void subarea_region::executeFloodFill(point p)
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

bool subarea_region::isLevelBlocked()
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

bool subarea_region::isRaceAffiliated()
{
    return race_affiliated;
}

void subarea_region::addAllHomeworlds(race *race_obj)
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

            createNPCShip(race_obj->getNativeShip(num_native_ship_types - 1),smp,native_race_id);
        }
    }
}

void subarea_region::insertOneHomeworld(race *race_obj, point p)
{
    getMap()->setBackdrop(p,LBACKDROP_PLANET);
    race_obj->addHomeworldStruct(p,race_obj->getRaceID(),RMS_FREE);
}

void subarea_region::addProcgenSubAreaNativeShips(race *race_obj, int num_types)
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
                    createNPCShip(race_obj->getNativeShip(i),p,native_race_id);
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
