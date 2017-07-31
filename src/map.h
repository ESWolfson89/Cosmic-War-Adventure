#ifndef MAP_H
#define MAP_H

#include "cell.h"
#include "point.h"

// a map is a two dimensional grid of cells as defined in cell.h

class map
{
	public:
		map();
		map(point);
		cell getCell(point);
		mob_t getMob(point);
		item_t getItem(point);
		backdrop_t getBackdrop(point);
		fire_t getFire(point);
		bool getv(point);
		bool getm(point);
		void setMob(point, mob_t);
		void setItem(point, item_t);
		void setBackdrop(point, backdrop_t);
		void setFire(point, fire_t);
		void setAllCellValuesToNIL(point);
		void setAllCellsUnvisible();
		void clearAllCells();
		void createGrid(point);
		void setv(point,bool);
		void setm(point,bool);
		point getSize();
		// save cell
        void save(std::ofstream &) const;
        // load cell
        void load(std::ifstream &);
	private:
		std::vector < std::vector < cell > > grid;
        point grid_size;
};

bool isStarmapStarTile(backdrop_t);

bool isBlockingCell(cell);

bool isSpawnBlockingCell(cell, backdrop_t, backdrop_t);

bool terrainBlocked(backdrop_t);

bool mobBlocked(mob_t);

bool notOnMapBorder(point,point);

bool inMapRange(point,point);

int numBackdropCellsAdj(map *, point, backdrop_t, int, bool);

point getMaxMapPoint(map *);

point getSubAreaSystemStartPoint(point);

fire_t selectFireCell(point,point,fire_t);

#endif
