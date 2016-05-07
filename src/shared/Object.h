#ifndef Object_h__
#define Object_h__

#include "AABB.h"

class Object
{
public:
    int getHealth() const { return _health; }
    void setHealth(int val) { _health = val; }

    vec3 getPosition() const { return _position; }
    void setPosition(vec3 val) { _position = val; }

    vec3 getRotation() const { return _rotation; }
    void setRotation(vec3 val) { _rotation = val; }

    vec3 getSize() const { return _size; }

    AABB getHitbox() const { return _hitbox; }

    virtual void update() = 0;

private:
    vec3 _position, _size, _rotation;
    int _health;
    AABB _hitbox;

};

#endif // Object_h__
