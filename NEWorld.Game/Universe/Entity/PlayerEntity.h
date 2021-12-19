#include "Entity.h"

class PlayerEntity: public Entity {
public:
	PlayerEntity(Double3 pos) :Entity(pos, { .6,1.7,.6 }) {}

	void setVelocity(Double3 vel) { mVelocity = vel; }
	void spawn() {
		mPosition = { 0,60,0 };
		mVelocity = { 0,0,0 };
	}
};
