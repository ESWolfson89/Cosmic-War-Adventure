#ifndef LINE_H_
#define LINE_H_

#include "map.h"

class line
{
	public:
		line();
		int getLineSize();
		void clearPoints();
		void bresenham(point, point, point);
		void checkAllBoundaries(point);
		bool isBlocking(map *, point, point, bool, bool);
		bool validLinePoint(point, point);
		point getLinePoint(int);
	private:
		std::vector <point> points;
};

#endif
