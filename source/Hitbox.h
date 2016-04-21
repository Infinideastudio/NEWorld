#ifndef HITBOX_H
#define HITBOX_H

namespace Hitbox
{

//Axis Aligned Bounding Box
struct AABB
{
    double xmin, ymin, zmin;
    double xmax, ymax, zmax;
    AABB()
    {
    }
    AABB(double _xmin, double _ymin, double _zmin, double _xmax, double _ymax, double _zmax)
        :xmin(_xmin), ymin(_ymin), zmin(_zmin), xmax(_xmax), ymax(_ymax), zmax(_xmax)
    {
    }
};

extern bool stuck;

bool inXclip(const AABB& boxA, const AABB& boxB);
bool inYclip(const AABB& boxA, const AABB& boxB);
bool inZclip(const AABB& boxA, const AABB& boxB);

bool Hit(const AABB& boxA, const AABB& boxB);

double MaxMoveOnXclip(const AABB& boxA, const AABB& boxB, double movedist);
double MaxMoveOnYclip(const AABB& boxA, const AABB& boxB, double movedist);
double MaxMoveOnZclip(const AABB& boxA, const AABB& boxB, double movedist);

AABB Expand(const AABB& box, double xe, double ye, double ze);
void Move(AABB &box, double xa, double ya, double za);
void MoveTo(AABB &box, double x, double y, double z);
void renderAABB(const AABB& box, float colR, float colG, float colB, int mode, double EPS = 0.0);
}

#endif
