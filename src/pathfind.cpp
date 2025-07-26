#include "pathfind.h"

pathfind pathfinder;

pathfind::pathfind()
{
}

// slow, but its ok for now until we add terrain...
point pathfind::dijkstra(map *m, point source, point sink)
{
    if (!inRange(source,point(0,0),addPoints(getMaxMapPoint(m),point(-1,-1))) ||
        !inRange(sink,point(0,0),addPoints(getMaxMapPoint(m),point(-1,-1))))
        return source;

    if (isAt(source,sink))
        return source;

    /*
        Add in possible terrain blocking when the time comes
    */

    curr_min_dist = INFINITE;

    initPathfind(m,source,sink);

    do
    {
        computeDistance(m->getSize(),source);

        if (has_ended)
            break;
    }
    while(1);

    return getMinDistPoint(source);
}

point pathfind::getMinDistPoint(point p)
{
    point ret_val = point(1,1);

    curr_min_dist = INFINITE;

    point offset;

    point adj;

    for (int i = 0; i < 8; ++i)
    {
        offset = pfind_retval_priority[i];
        adj.set(p.x() + offset.x(), p.y() + offset.y());
        if (dist_val[adj.y()][adj.x()] > -1)
            if (dist_val[adj.y()][adj.x()] < curr_min_dist)
            {
                curr_min_dist = dist_val[adj.y()][adj.x()];
                ret_val.set(adj.x(), adj.y());
            }
    }

    if (curr_min_dist == INFINITE)
        return p;

    return ret_val;
}

void pathfind::computeDistance(point msize, point source)
{
    point adj;

    bool reached_source = false;

    point new_min_scan = min_scan;
    point new_max_scan = max_scan;

    has_ended = true;

    int px,py,dx,dy;

    point loc;

    for (px = min_scan.x(); px <= max_scan.x(); ++px)
    for (py = min_scan.y(); py <= max_scan.y(); ++py)
    {
        if (dist_val[py][px] > -1)
        {
            curr_min_dist = INFINITE;
            for (dx = -1; dx <= 1; ++dx)
            for (dy = -1; dy <= 1; ++dy)
                if (dx != 0 || dy != 0)
                {
                    adj.set(dx+px,dy+py);
                    if (inMapRange(adj,msize))
                    if (dist_val[adj.y()][adj.x()] > -1)
                    if (dist_val[adj.y()][adj.x()] < curr_min_dist)
                        curr_min_dist = dist_val[adj.y()][adj.x()];
                }

            if (dist_val[py][px] >= 2 + curr_min_dist)
            {
                dist_val[py][px] = curr_min_dist + 1;
                has_ended = false;


                if (px == min_scan.x())
                    new_min_scan.setx(px-1);
                if (py == min_scan.y())
                    new_min_scan.sety(py-1);
                if (px == max_scan.x())
                    new_max_scan.setx(px+1);
                if (py == max_scan.y())
                    new_max_scan.sety(py+1);

                loc.set(px,py);

                if (shortestPath(source,loc) <= 1)
                {
                    reached_source = true;
                }
            }
        }
    }

    if (reached_source)
        has_ended = true;

    min_scan = point((int)std::max(0,new_min_scan.x()),(int)std::max(0,new_min_scan.y()));
    max_scan = point((int)std::min(msize.x()-1,new_max_scan.x()),(int)std::min(msize.y()-1,new_max_scan.y()));
}

void pathfind::setupPathfindDistVector(point msize)
{
    std::vector<std::vector<int>>().swap(dist_val);
    dist_val.resize(msize.y());
    for (int i = 0; i < (int)dist_val.size(); ++i)
    {
        dist_val[i].resize(msize.x());
    }
}

void pathfind::initPathfind(map *m, point source, point sink)
{
    point p;

    bool b;

    min_scan.set(sink.x()-1,sink.y()-1);
    max_scan.set(sink.x()+1,sink.y()+1);

    for (int y = 0; y < m->getSize().y(); ++y)
    for (int x = 0; x < m->getSize().x(); ++x)
    {
        p.set(x,y);

        b = isBlockingCell(m->getCell(p));

        if (!isAt(p,sink))
        {
            if (!b)
                dist_val[y][x] = INFINITE;
            else
                dist_val[y][x] = -1;
        }
        else
            dist_val[y][x] = 0;
    }
}
