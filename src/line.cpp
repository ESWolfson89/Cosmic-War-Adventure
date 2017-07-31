#include "line.h"

line::line()
{
}

int line::getLineSize()
{
    return points.size();
}

void line::clearPoints()
{
    points.clear();
}

bool line::validLinePoint(point msize, point source)
{
    return (inRange(source,point(0,0),point(msize.x()-1,msize.y()-1)));
}

void line::checkAllBoundaries(point msize)
{
    int line_size = (int)getLineSize();

    for (int i = 0; i < line_size; ++i)
    {
        if (!validLinePoint(msize,points[0]))
            points.erase(points.begin());
        else
            break;
    }
}

// step through Bresenham's Line Algorithm
void line::bresenham(point msize, point a, point b)
{
    clearPoints();

    if (isAt(a,b) || !validLinePoint(msize,a))
        return;

    point dp = point(std::abs(a.x()-b.x()),std::abs(a.y()-b.y()));

    point sp;

    if (b.x() < a.x())
        sp.setx(1);
    else
        sp.setx(-1);

    if (b.y() < a.y())
        sp.sety(1);
    else
        sp.sety(-1);

    int error = dp.x() - dp.y();
    int error2;

    while (!isAt(a,b))
    {
        points.push_back(b);
        error2 = 2 * error;
        if (error2 > -dp.y())
        {
            error -= dp.y();
            b.setx(b.x() + sp.x());
        }
        if (error2 < dp.x())
        {
            error += dp.x();
            b.sety(b.y() + sp.y());
        }
    }
    points.push_back(b);

    checkAllBoundaries(msize);
}

bool line::isBlocking(map * m, point a, point b, bool include_NPCs, bool include_endpoint)
{
    bresenham(m->getSize(),a,b);

    int min_i = 1;

    if (include_endpoint)
        min_i = 0;

    if (isAt(a,b))
        return false;

    if (getLineSize() == 0)
        return true;

    if (min_i >= getLineSize())
        return true;

    for (int i = min_i; i < getLineSize(); ++i)
    {
        if (terrainBlocked(m->getBackdrop(points[i])))
            return true;
        if (include_NPCs && mobBlocked(m->getMob(points[i])))
            return true;
    }

    return false;
}

point line::getLinePoint(int i)
{
    if (i > getLineSize() - 1)
        return point(1,1);

    return points[i];
}
