#ifndef Object_h__
#define Object_h__

#include "AABB.h"

class Object
{
public:
    int getHealth() const { return health; }
    void setHealth(int val) { health = val; }

    vec3 getPosition() const { return position; }
    void setPosition(vec3 val) { position = val; }

    vec3 getRotation() const { return rotation; }
    void setRotation(vec3 val) { rotation = val; }

    vec3 getSize() const { return size; }

    virtual void update() = 0;

private:
    vec3 position, size, rotation;
    int health;
    AABB hitbox;

};

#endif // Object_h__
