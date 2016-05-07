#ifndef Object_h__
#define Object_h__

#include "AABB.h"
#include "vec3.h"

class Object
{
public:
    int getHealth() const { return _health; }
    void setHealth(int val) { _health = val; }

    Vec3 getPosition() const { return _position; }
    void setPosition(Vec3 val) { _position = val; }

    Vec3 getRotation() const { return _rotation; }
    void setRotation(Vec3 val) { _rotation = val; }

    Vec3 getSize() const { return _size; }

    AABB getHitbox() const { return _hitbox; }

    virtual void update() = 0;

private:
    Vec3 _position, _size, _rotation;
    int _health;
    AABB _hitbox;

};

#endif // Object_h__
