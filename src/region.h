#ifndef REGION_H_
#define REGION_H_

#include "contact.h"
#include "item.h"
#include "map.h"

/*
Extra stuff not needed yet (ever?)

if (num_extra_ship_types > 0)
for (int i = 0; i < num_extra_ships; ++i)
{
    p = getRandNPCShipOpenPoint(getMap(),point(1,1),addPoints(getMap()->getSize(),point(-2,-2)));
    createNPCShip(race_obj->getExtraShip(randInt(0,num_extra_ship_types-1)),p,native_race_id);
}

in generate fn:
race_vec[(int)race_vec.size()-1].generateExtraShips();

void subarea_region::loadLayoutFromFile(std::string file_name, race *race_obj)
{
    std::vector <char> layout_vec;

    std::ifstream layout_file;

    cell c;

    int dimx, dimy;
    int locx, locy;

    point p;

    layout_file.open(file_name);

    layout_file >> dimx;
    layout_file >> dimy;
    layout_file >> locx;
    layout_file >> locy;

    for (int i = 0; i < dimx * dimy; ++i)
    {
      layout_vec.push_back('.');
    }

    for (int i = 0; i < dimx * dimy; ++i)
    {
      layout_file >> layout_vec[i];
    }

    layout_file.close();

    for (int x = 0; x < dimx; ++x)
    for (int y = 0; y < dimy; ++y)
    {
        p.set(locx + x,locy + y);

        c.setBackdrop(m.getBackdrop(p));

        switch(layout_vec[(dimx * y) + x])
        {
            case('!'):
                     c.setBackdrop(LBACKDROP_PLANET);
                     race_obj->addHomeworldStruct(p,race_obj->getRaceID(),RMS_FREE);
                     break;
            case('^'):
                     c.setBackdrop(LBACKDROP_SPACESTATION);
                     createStationAt(p,race_obj->getDangerLevel());
                     break;
            case('#'):
                     c.setBackdrop(LBACKDROP_SPACEWALL);
                     break;
            case('0'):
                     createNPCShip(race_obj->getNativeShip(0),p,native_race_id);
                     break;
            case('1'):
                     createNPCShip(race_obj->getNativeShip(1),p,native_race_id);
                     break;
            case('2'):
                     createNPCShip(race_obj->getNativeShip(2),p,native_race_id);
                     break;
            case('3'):
                     createNPCShip(race_obj->getNativeShip(3),p,native_race_id);
                     break;
            case('a'):
                     createNPCShip(race_obj->getExtraShip(0),p,native_race_id);
                     break;
            case('b'):
                     createNPCShip(race_obj->getExtraShip(1),p,native_race_id);
                     break;
            default:
                     break;
        }

        m.setBackdrop(p,c.getBackdrop());
    }

    std::vector<char>().swap(layout_vec);
}

void initSubAreaLayoutConfiguration(race *);

void subarea_region::initSubAreaLayoutConfiguration(race *race_obj)
{
    switch(race_obj->getRaceType())
    {
        case(RACETYPE_AHRKON):
             layout_config_roller = 0;
             break;
        case(RACETYPE_OOLIG):
             layout_config_roller = 1;
             break;
        case(RACETYPE_PROCGEN):
             if (race_obj->getRaceID() == 2)
                 layout_config_roller = 2;
             else
                 layout_config_roller = 3;
             break;
        default:
             layout_config_roller = -1;
             break;
    }
    switch(layout_config_roller)
    {
        case(0):
             loadLayoutFromFile("layout1.txt",race_obj);
             break;
        case(1):
             loadLayoutFromFile("layout2.txt",race_obj);
             break;
        case(2):
             loadLayoutFromFile("layout3.txt",race_obj);
             break;
        case(3):
             loadLayoutFromFile("layout4.txt",race_obj);
             break;
        default:
            break;
    }
    subarea_name = race_obj->getNameString();
    att_towards_player = race_obj->getPlayerAttStatus();
}

void starmap_region::populateStarmap()
{
    point p;

    for (int i = 0; i < (int)race_vec.size(); ++i)
    {
        m.setBackdrop(race_vec[i].getStarmapLoc(),(backdrop_t)((int)SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE +
                                                               (int)race_vec[i].getStarType()));
    }
}

in setupUniverse:
populateStarmap();

void subarea_region::addOneHomeworldPattern(race *race_obj)
{
    point p;
    if (getMap()->getSize().x() % 2 == 1 && getMap()->getSize().y() % 2 == 1)
    {
        p = divPoint(getMap()->getSize(),2,2);
        if (!isBlockingCell(p))
        {
            insertHomeworld(race_obj,p);
            return;
        }
        else
        {
            p = point(getMap()->getSize().x()/2,getMap()->getSize().y()-3);
            insertHomeworld(race_obj,p);
        }
    }
}

if (num_wall_adj_moore == 7 && num_wall_adj_vn == 4)
    createNPCShip(race_obj->getNativeShip(num_types - 1),p,native_race_id);
else if (num_wall_adj_moore == 7 && num_wall_adj_vn == 3 && num_types >= 2)
    createNPCShip(race_obj->getNativeShip(num_types - 2),p,native_race_id);
else if (num_wall_adj_moore == 6 && num_wall_adj_vn == 2 && num_types >= 2)
    createNPCShip(race_obj->getNativeShip(num_types - 2),p,native_race_id);
else if (num_wall_adj_moore == 6 && num_wall_adj_vn == 3 && num_types >= 3)
    createNPCShip(race_obj->getNativeShip(num_types - 3),p,native_race_id);
else if (num_wall_adj_moore == 5 && num_wall_adj_vn == 3 && num_types >= 4)
    createNPCShip(race_obj->getNativeShip(num_types - 4),p,native_race_id);
else if (num_wall_adj_moore == 5 && num_wall_adj_vn == 2 && num_types >= 5)
    createNPCShip(race_obj->getNativeShip(num_types - 5),p,native_race_id);
else if (num_wall_adj_moore == 5 && num_wall_adj_vn == 1 && num_types >= 6)
    createNPCShip(race_obj->getNativeShip(num_types - 6),p,native_race_id);

if (num_types > 3 && num_wall_adj == 7)
    createNPCShip(race_obj->getNativeShip(3),p,native_race_id);

    void starmap_region::syncAllRegionWithRaceATP()
{
    point loc;
    int race_index;
    for (int i = 0; i < (int)subarea_vec.size(); ++i)
    {
         loc = subarea_vec[i].getSubAreaLoc();
         race_index = getRaceIndex(loc);
         if (race_index >= 0)
             subarea_vec[i].setAttTowardsPlayer(race_vec[race_index].getPlayerAttStatus());
    }
}

void starmap_region::syncAllRaceWithRegionATP()
{
    point loc;
    int reg_index;
    for (int i = 0; i < (int)race_vec.size(); ++i)
    {
         loc = race_vec[i].getStarmapLoc();
         reg_index = getSubAreaIndex(loc);
         if (reg_index >= 0)
             race_vec[i].setPlayerAttStatus(subarea_vec[reg_index].getAttTowardsPlayer());
    }
}

void syncAllRegionWithRaceATP();
void syncAllRaceWithRegionATP();

int subarea_region::getAttTowardsPlayer()
{
    return att_towards_player;
}

int getAttTowardsPlayer();

void subarea_region::setAttTowardsPlayer(int atp)
{
    att_towards_player = atp;
}

void setAttTowardsPlayer(int);
*/

static const int npc_spawn_conditions[3][7] =
{
  {
      1, 10, 19, 21, 29, 35, 40
  },
  {
      10, 15, 20, 25, 30, 34, 40
  },
  {
      9, 12, 16, 21, 27, 34, 36
  }
};

class subarea_region
{
    public:
        subarea_region();
        subarea_region(int,bool,star_type);
        int getNativeRaceID();
        void initMap(point);
        void setupSubAreaMapGenerics(point, backdrop_t);
        void setupProcgenTerritorialSubArea(race *);
        void setupProcgenNonTerritorialSubArea(point);
        void evolveSpaceWallCA();
        void smoothenSpaceWallStructure();
        void placeInitialSpaceWallConfiguration(race_domain_type);
        void cleanupEverything();
        //void loadLayoutFromFile(std::string, race *);
        map *getMap();
        point getSize();
        int getNumShipNPCs();
        int getNumActiveNativeShipsPresent();
        int getNumStations();
        void destroyNPC(int);
        void createNPCShip(ship_mob *,point,int);
        void createStationAt(point,int);
        void addNPCShipModules(ship_mob *);
        void addAllNPCShips(race *);
        void rollOneNPC(race *);
        subarea_specific_type getSubAreaSpecificType();
        // item related methods
        void createPileAt(point);
        void deletePileAt(point);
        void deleteAllPiles();
        void deleteItemFromPile(point, int, int);
        void addItem(item_stats, int, point);
        cell_pile* getPile(point);
        cell_pile* getPile(int);
        int getNumItemPiles();
        bool isPileAt(point);
        bool isLevelBlocked();
        bool isRaceAffiliated();
        // end item methods
        void executeFloodFill(point);
        void initFloodFillFlags();
        void addProcgenSubAreaNativeShips(race *,int);
        void addAllHomeworlds(race *);
        void generateAllSpaceStations(int,int);
        void insertOneHomeworld(race *, point);
        void setSubAreaName(std::string);
        void setSubAreaSpecificType(subarea_specific_type);
        std::string getSubAreaName();
        ship_mob *getNPCShip(int);
        ship_mob *getNPCShip(point);
        station *getStation(int);
        station *getStation(point);
        point getSubAreaLoc();
        // save load functions
        void saveAllNPCShips(std::ofstream &) const;
        void loadAllNPCShips(std::ifstream &);
        void save(std::ofstream &) const;
        void load(std::ifstream &);
    private:
        map m;
        point subarea_loc;
        int npc_id_counter;
        int native_race_id;
        int layout_config_roller;
        int star_radius;
        bool race_affiliated;
        subarea_specific_type sast;
        star_type s_type;
        std::string subarea_name;
        std::vector <ship_mob> NPC_ships;
        std::vector <station> station_objs;
        std::vector <cell_pile> piles;
        std::vector <std::vector <bool> > flood_fill_flags;
};

class starmap_region
{
    public:
        starmap_region();
        void setupUniverse();
        void populateStarmap();
        void generateAllRaces();
        void generateOneGuaranteedRace(int,int,race_domain_type,point,point,race_type,race_personality_type,std::string, int);
        void cleanupEverything();
        map *getMap();
        point getSize();
        int getSubAreaIndex(point);
        int getRaceIndex(point);
        int getNumSubAreas();
        int getNumRaces();
        subarea_region *getSubArea(int);
        subarea_region *getNPSubArea();
        subarea_map_type getSubAreaMapType();
        race *getRace(int);
        void setSubAreaMapType(subarea_map_type);
        void createSubArea(point,subarea_map_type,subarea_specific_type,bool,bool,star_type);
        void createNonPersistentSubArea(point,bool,subarea_specific_type);
        void createPersistentSubArea(point,bool,star_type,subarea_specific_type);
        void populateWarZone(int);
        void saveAllSubAreas(std::ofstream &) const;
        void loadAllSubAreas(std::ifstream &);
        void save(std::ofstream &) const;
        void load(std::ifstream &);
    private:
        map m;
        std::vector <race> race_vec;
        std::vector <subarea_region> subarea_vec;
        subarea_region nonpersistent_subarea;
        subarea_map_type samt;
};

void initEmptyTiles(map *, backdrop_t);

void checkAddGiantSpaceWallCircle(map *, point, int);
void checkAddGiantSpaceWallCube(map *, point, int);

void apply2468Rule(map *, int);

void generateStarBackdrop(map *, point, int, star_type);
void generateGivenStarBackdrop(map *, point, int, backdrop_t);

void carveBackdropCircle(map *, point, int, backdrop_t);

point getRandNPCShipOpenPoint(map *,point,point,backdrop_t,backdrop_t);

int getNumCAGens(point);

int getNumGlobalCutIterations(point);

#endif
