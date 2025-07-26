#include "map.h"

// IMPORTANT
// in the grid array the outermost index is X and innermost Y (opposite of point)
// the reason for this is that it is more natural to think each subvector in grid
// as a row...

map::map()
{

}

map::map(point sze)
{
    createGrid(sze);
}

cell map::getCell(point p)
{
	return grid[p.y()][p.x()];
}

cell * map::getCellP(point p)
{
	return &(grid[p.y()][p.x()]);
}

mob_t map::getMob(point p)
{
	return grid[p.y()][p.x()].getMob();
}

item_t map::getItem(point p)
{
	return grid[p.y()][p.x()].getItem();
}

backdrop_t map::getBackdrop(point p)
{
	return grid[p.y()][p.x()].getBackdrop();
}

fire_t map::getFire(point p)
{
	return grid[p.y()][p.x()].getFire();
}

bool map::getv(point p)
{
	return grid[p.y()][p.x()].getv();
}

bool map::getm(point p)
{
	return grid[p.y()][p.x()].getm();
}

void map::setMob(point p, mob_t m)
{
	grid[p.y()][p.x()].setMob(m);
}

void map::setItem(point p, item_t i)
{
	grid[p.y()][p.x()].setItem(i);
}

void map::setBackdrop(point p, backdrop_t t)
{
	grid[p.y()][p.x()].setBackdrop(t);
}

void map::setFire(point p, fire_t f)
{
	grid[p.y()][p.x()].setFire(f);
}

void map::setv(point p, bool b)
{
	grid[p.y()][p.x()].setv(b);
}

void map::setm(point p, bool b)
{
	grid[p.y()][p.x()].setm(b);
}

// backdrop can never be NIL
void map::setAllCellValuesToNIL(point p)
{
	setv(p,false);
	setm(p,false);
	setFire(p,NIL_f);
	setMob(p,NIL_m);
	setItem(p,NIL_i);
	setBackdrop(p,NIL_b);
}

void map::setAllCellsUnvisible()
{
	for (int x = 0; x < grid_size.x(); ++x)
    {
        for (int y = 0; y < grid_size.y(); ++y)
        {
            grid[y][x].setv(false);
        }
    }
}

void map::createGrid(point sze)
{
    grid_size = sze;
    std::vector < std::vector< cell > > temp_grid(sze.y(), std::vector< cell >(sze.x()));
    grid.swap(temp_grid);
}

void map::clearAllCells()
{
    std::vector < std::vector < cell > >().swap(grid);
}

point map::getSize()
{
    return grid_size;
}

void map::save(std::ofstream& os) const
{
    grid_size.save(os);  // Save the size (point with x and y)

    for (int y = 0; y < grid_size.y(); ++y)
    {
        for (int x = 0; x < grid_size.x(); ++x)
        {
            grid[y][x].save(os);
        }
    }
}

void map::load(std::ifstream& is)
{
    grid_size.load(is);

    std::vector<std::vector<cell>> temp_grid(
        grid_size.y(),
        std::vector<cell>(grid_size.x())
    );

    grid.swap(temp_grid);

    for (int y = 0; y < grid_size.y(); ++y)
    {
        for (int x = 0; x < grid_size.x(); ++x)
        {
            grid[y][x].load(is);
        }
    }
}

bool isStarmapStarTile(backdrop_t b)
{
    return ((int)b >= SMBACKDROP_MAINSEQSTARSUBAREAENTRANCE &&
            (int)b <= SMBACKDROP_EMPTY_WHITESTARSUBAREAENTRANCE);
}

bool isBlockingCell(cell c)
{
    return mobBlocked(c.getMob()) || terrainBlocked(c.getBackdrop());
}

bool isSpawnBlockingCell(cell c, backdrop_t bd_lowest, backdrop_t bd_highest)
{
    return mobBlocked(c.getMob()) || ((int)c.getBackdrop() < (int)bd_lowest || (int)c.getBackdrop() > (int)bd_highest);
}

bool terrainBlocked(backdrop_t bd)
{
    return bd == LBACKDROP_SPACEWALL;
}

bool mobBlocked(mob_t mb)
{
    return (mb != NIL_m);
}

bool notOnMapBorder(point p, point mapsize)
{
    return inRange(p,point(1,1),addPoints(mapsize,point(-2,-2)));
}

bool inMapRange(point p, point mapsize)
{
    return inRange(p,point(0,0),addPoints(mapsize,point(-1,-1)));
}

point getMaxMapPoint(map *m)
{
    return point(m->getSize().x()-1,m->getSize().y()-1);
}

point getSubAreaSystemStartPoint(point msze)
{
    return point(msze.x()/2,msze.y()-3);
}

fire_t selectFireCell(point source, point dest, fire_t ft)
{
    if (ft >= (int)FIRET_EXPLOSION)
        return ft;

    point dl = point(dest.x() - source.x(), dest.y() - source.y());

    if (std::abs(dl.x()) == 0)
        return ft;
    if (std::abs(dl.y()) == 0)
        return (fire_t)((int)ft + 1);
    if ((dl.x() < 0 && dl.y() < 0) || (dl.x() > 0 && dl.y() > 0))
        return (fire_t)((int)ft + 2);

    return (fire_t)((int)ft + 3);
}

int numBackdropCellsAdj(map *m, point p, backdrop_t b, int rad, bool vn)
{
	int adj = 0;

    // adjacent point
	point ap;

	for (int x = -rad + p.x(); x <= rad + p.x(); ++x)
	for (int y = -rad + p.y(); y <= rad + p.y(); ++y)
	{
		ap.set(x,y);
		if (!isAt(ap,p))
		if (inRange(ap,point(0,0),point(m->getSize().x()-1,m->getSize().y()-1)))
		if (m->getBackdrop(ap) == b)
        if (!vn || (x == p.x() || y == p.y()))
            adj++;
	}

	return adj;
}
