#ifndef PATHFIND_H_
#define PATHFIND_H_

#define INFINITE 999999

#include "map.h"


// ****done as of 7/13/2016*****

static const point pfind_retval_priority[8] =
{
	point(1,0),
	point(-1,0),
	point(0,-1),
	point(0,1),
	point(-1,-1),
	point(1,-1),
	point(-1,1),
	point(1,1)
};

class pathfind
{
	public:
		pathfind();
		point dijkstra(map *, point, point);
		point getMinDistPoint(point);
		void setupPathfindDistVector(point);
		void computeDistance(point,point);
		void initPathfind(map *, point, point);
	private:
	    // see comment above for this array
		std::vector<std::vector<int>>dist_val;
		bool has_ended;
		int curr_min_dist;
		point min_scan;
		point max_scan;
};

extern pathfind pathfinder;

#endif
