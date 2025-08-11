#ifndef REGION_H_
#define REGION_H_

#include "contact.h"
#include "item.h"
#include "map.h"
#include "mob.h"

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

class SubAreaRegion
{
    public:
        SubAreaRegion();
        SubAreaRegion(int,bool,star_type);
        int getNativeRaceID();
        void initMap(point);
        void setupSubAreaMapGenerics(point, backdrop_t);
        void setupProcgenTerritorialSubArea(race *);
        void setupProcgenWarzoneSubArea(point);
        void setupProcgenEmptySubArea(point);
        void evolveSpaceWallCA();
        void smoothenSpaceWallStructure();
        void placeInitialSpaceWallConfiguration(race_domain_type);
        void cleanupEverything();
        //void loadLayoutFromFile(std::string, race *);
        map *getMap();
        point getSize();
        int getNumShipNPCs();
        int getNumActiveShipsPresent();
        int getNumActiveNativeShipsPresent();
        int getNumStations();
        void destroyNPC(int);
        void createNPCShip(MobShip *,point,int);
        void createStationAt(point,int);
        void createEntertainmentStationAt(point p, int dl);
        void addNPCShipModules(MobShip *);
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
        void carveSpaceWall(point div_point, int sze);
        void generateAllSpaceStations(int,int);
        void generateAllEntertainmentStations(int dl, int numStations);
        void insertOneHomeworld(race *, point);
        void setSubAreaName(std::string);
        void setSubAreaSpecificType(subarea_specific_type);
        std::string getSubAreaName();
        MobShip *getNPCShip(int);
        MobShip *getNPCShip(point);
        station *getStation(int);
        station *getStation(point);
        EntertainmentStation* getEntertainmentCenter(int i);
        EntertainmentStation* getEntertainmentCenter(point p);
        point getSubAreaLoc() const;
        // save load functions
        void save(std::ofstream &) const;
        void load(std::ifstream &);
    private:
        map m;
        point subarea_loc;
        int npc_id_counter;
        int nativeRaceID;
        int layout_config_roller;
        int star_radius;
        bool race_affiliated;
        subarea_specific_type sast;
        star_type s_type;
        std::string subarea_name;
        std::vector <MobShip> NPCShips;
        std::vector <station> station_objs;
        std::vector <EntertainmentStation> entertainmentStations;
        std::vector <cell_pile> piles;
        std::vector <std::vector <uint8_t> > flood_fill_flags;
};

class StarMapRegion
{
    public:
        StarMapRegion();
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
        SubAreaRegion *getSubArea(int);
        SubAreaRegion *getNPSubArea();
        subarea_MapType getSubAreaMapType();
        race *getRace(int);
        void setSubAreaMapType(subarea_MapType);
        void createSubArea(point,subarea_MapType,subarea_specific_type,bool,star_type);
        void createNonPersistentSubArea(point,bool,subarea_specific_type);
        void createPersistentSubArea(point,star_type,subarea_specific_type);
        void populateWarZone(int);
        void save(std::ofstream &) const;
        void load(std::ifstream &);
        std::vector<point> getPointsWithBackdrop(backdrop_t target);
    private:
        map m;
        std::vector <race> raceVector;
        std::vector <SubAreaRegion> subareaVector;
        SubAreaRegion nonpersistent_subarea;
        subarea_MapType samt;
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

map* getMap();

point getMapSize();

MobShip* getSubAreaShipMobAt(point);

MobShip* getMobFromID(int);

MobShip* getCurrentMobTurn();

point getMapSizeBasedOnDomainType(race_domain_type rdt);

race_domain_type getRaceDomainTypeFromDangerLevel(int dangerLevel);

int getMaxDangerLevel(point smLoc);

int getStartingRaceAttitudeTowardsPlayer();

int getDominantRaceIDInRegion(SubAreaRegion* region);

int getMobRaceDangerLevel(MobShip*);

SubAreaRegion* currentRegion();


extern StarMapRegion universe;
extern MapType current_maptype;
extern int current_subarea_id;
extern int current_mob_turn;

#endif
