#include "aabb.h"

using std::max;
using std::min;

AABB::AABB() :xmin(0.0), xmax(0.0), ymin(0.0), ymax(0.0), zmin(0.0), zmax(0.0)
{
}

AABB::AABB(double _xmin, double _ymin, double _zmin, double _xmax, double _ymax, double _zmax)
    :xmin(_xmin), ymin(_ymin), zmin(_zmin), xmax(_xmax), ymax(_ymax), zmax(_xmax)
{
}

bool AABB::intersectX(const AABB &box) const
{
    if ((xmin > box.xmin && xmin < box.xmax) || (xmax > box.xmin && xmax < box.xmax) ||
        (box.xmin > xmin && box.xmin < xmax) || (box.xmax > xmin && box.xmax < xmax))
        return true;

    return false;
}

bool AABB::intersectY(const AABB &box) const
{
    if ((ymin > box.ymin && ymin < box.ymax) || (ymax > box.ymin && ymax < box.ymax) ||
        (box.ymin > ymin && box.ymin < ymax) || (box.ymax > ymin && box.ymax < ymax))
        return true;

    return false;
}

bool AABB::intersectZ(const AABB &box) const
{
    if ((zmin > box.zmin && zmin < box.zmax) || (zmax > box.zmin && zmax < box.zmax) ||
        (box.zmin > zmin && box.zmin < zmax) || (box.zmax > zmin && box.zmax < zmax))
        return true;

    return false;
}

bool AABB::intersect(const AABB &box) const
{
    return intersectX(box) && intersectY(box) && intersectZ(box);
}

double AABB::maxMoveOnXclip(const AABB &box, double orgmove) const
{
    if (!(intersectY(box) && intersectZ(box)))
        return orgmove;

    else if (xmin >= box.xmax && orgmove < 0.0)
        return max(box.xmax - xmin, orgmove);

    else if (xmax <= box.xmin && orgmove > 0.0)
        return min(box.xmin - xmax, orgmove);

    return orgmove;
}

double AABB::maxMoveOnYclip(const AABB &box, double orgmove) const
{
    if (!(intersectX(box) && intersectZ(box)))
        return orgmove;

    else if (ymin >= box.ymax && orgmove < 0.0)
        return max(box.ymax - ymin, orgmove);

    else if (ymax <= box.ymin && orgmove > 0.0)
        return min(box.ymin - ymax, orgmove);

    return orgmove;
}

double AABB::maxMoveOnZclip(const AABB &box, double orgmove) const
{
    if (!(intersectX(box) && intersectY(box)))
        return orgmove;

    else if (zmin >= box.zmax && orgmove < 0.0)
        return max(box.zmax - zmin, orgmove);

    else if (zmax <= box.zmin && orgmove > 0.0)
        return min(box.zmin - zmax, orgmove);

    return orgmove;
}

AABB AABB::expand(double x, double y, double z) const
{
    AABB res = *this;

    if (x > 0.0)
        res.xmax += x;
    else
        res.xmin += x;

    if (y > 0.0)
        res.ymax += y;
    else
        res.ymin += y;

    if (z > 0.0)
        res.zmax += z;
    else
        res.zmin += z;

    return res;
}

void AABB::move(double x, double y, double z)
{
    xmin += x;
    ymin += y;
    zmin += z;
    xmax += x;
    ymax += y;
    zmax += z;
}

void AABB::moveTo(double x, double y, double z)
{
    double l, w, h;
    l = (xmax - xmin) / 2;
    w = (ymax - ymin) / 2;
    h = (zmax - zmin) / 2;
    xmin = x - l;
    ymin = y - w;
    zmin = z - h;
    xmax = x + l;
    ymax = y + w;
    zmax = z + h;
}
