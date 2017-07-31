// point class implementation:

#include "point.h"

// default const.
point::point()
{
	x_val = y_val = 0;
}

point::point(int xx, int yy)
{
	x_val = xx;
	y_val = yy;
}

// set...
void point::set(int xx, int yy)
{
	x_val = xx;
	y_val = yy;
}

// set y only
void point::sety(int yy)
{
	y_val = yy;
}

// set x only
void point::setx(int xx)
{
	x_val = xx;
}

// get y value of point.
int point::y()
{
	return y_val;
}

// get x value of point.
int point::x()
{
	return x_val;
}

// assignment overload
point point::operator=(point p)
{
	x_val = p.x();
	y_val = p.y();
	return *this;
}

void point::save(std::ofstream &os) const
{
    os.write((const char *)&x_val,sizeof(int));
    os.write((const char *)&y_val,sizeof(int));
}

// load point
void point::load(std::ifstream &is)
{
    is.read((char *)&x_val,sizeof(int));
    is.read((char *)&y_val,sizeof(int));
}


// is point p within the rectangle formed by pmin as the top left
// point and pmax as the bottom right point
bool inRange(point p, point pmin, point pmax)
{
	return (p.x() >= pmin.x() && p.y() >= pmin.y() && p.x() <= pmax.x() && p.y() <= pmax.y());
}

// are a and b's fields (x and y) both equivalent
bool isAt(point a, point b)
{
	return (a.x() == b.x() && a.y() == b.y());
}

// is the minimum x,y distance between a and b 1?
// This means they are adjacent
bool isAdj(point a, point b)
{
	return shortestPath(a,b) == 1;
}

// return the minimum of the distance between x and
// the distance between the y components of both points
int shortestPath(point a, point b)
{
	point dab = point(std::abs(a.y() - b.y()), std::abs(a.x()-b.x()));
	return (dab.y() > dab.x()) ? dab.y() : dab.x();
}

// divide p.x() by xval and p.y() by yval ->
// round down to nearest integer
point divPoint(point p, int xval, int yval)
{
    return point((int)(p.x()/xval),(int)(p.y()/yval));
}

// return the actual distance between a and b
// squared
int distanceSquared(point a, point b)
{
    point delta = point(b.x() - a.x(),b.y() - a.y());
    return (delta.x() * delta.x()) + (delta.y() * delta.y());
}

// return the sum of points a and b
point addPoints(point a, point b)
{
	return point(a.x() + b.x(),a.y() + b.y());
}
