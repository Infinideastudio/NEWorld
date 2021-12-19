#include "Entity.h"
#include "../World/World.h"

void Entity::move(const EntityBVH& bvh)
{
	if (!doCollisionCheck()) {
		mPosition += mVelocity;
		return;
	}
	auto currentHitbox = movement_bounding_box();
	auto collisions = bvh.intersect(currentHitbox);
	auto actualMovement = getVelocity();
	auto hitboxes = World::getHitboxes(currentHitbox);
	for (const auto& entity : collisions) {
		hitboxes.push_back(entity->bounding_box());
	}

	for (const auto& box : hitboxes) {
		actualMovement.Y = AABB::MaxMove(currentHitbox, box, actualMovement.Y, 1);
	}
	currentHitbox.min += Vector3(0.0, actualMovement.Y, 0.0);
	currentHitbox.max += Vector3(0.0, actualMovement.Y, 0.0);
	for (const auto& box : hitboxes) {
		actualMovement.X = AABB::MaxMove(currentHitbox, box, actualMovement.X, 0);
	}
	currentHitbox.min += Vector3(actualMovement.X, 0.0, 0.0);
	currentHitbox.max += Vector3(actualMovement.X, 0.0, 0.0);
	for (const auto& box : hitboxes) {
		actualMovement.Z = AABB::MaxMove(currentHitbox, box, actualMovement.Z, 2);
	}
	currentHitbox.min += Vector3(0.0, 0.0, actualMovement.Y);
	currentHitbox.max += Vector3(0.0, 0.0, actualMovement.X);
}
