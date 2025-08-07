// Header file for point class

#ifndef POINT_H_
#define POINT_H_

#include <cmath>
#include <cassert>
#include <fstream>
#include <tuple>

class point
{
	public:
		// default const.
 		point();
 		// parmaeters
 		point(int, int);
 		// set...
		void set(int, int);
		// set y only
		void sety(int);
		// set x only
		void setx(int);
		// get y value of point.
		int y() const;
		// get x value of point.
		int x() const;
		// assignment overload
		point operator=(point);
		point operator+(const point& other) const;
		point operator-(const point& other) const;
		// save point to file
        void save(std::ofstream &) const;
        // load point to file
        void load(std::ifstream &);

		bool operator<(const point& other) const;

	private:
		// member variables for x and y coord.
		int x_val,y_val;
};

bool inRange(point, point, point);

bool isAt(point,point);

bool isAdj(point, point);

int shortestPath(point, point);

int distanceSquared(point, point);

point multiplyPoints(point, point);

point pointDistance(point, point);

point divPoint(point, int, int);

point addPoints(point, point);

bool isOnDiagonal(point, point);

bool isInStraightLine(point, point);


#endif
