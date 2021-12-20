#include "Entity.h"

#include "Universe/World/World.h"

Int3 Entity::getChunkPosition() const noexcept
{ return Int3(mPosition, World::GetChunkPos<int>); }

void Entity::move(const EntityBVH& bvh) {
	if (!doCollisionCheck()) {
		mPosition += mVelocity;
		return;
	}
	auto currentHitbox = bounding_box();
	auto currentMovementHitbox = movement_bounding_box();
	auto hitboxes = World::getHitboxes(currentMovementHitbox);

	const auto collisions = bvh.intersect(currentMovementHitbox);
	for (const auto& entity : collisions) {
		if (entity == this) continue;
		hitboxes.push_back(entity->bounding_box());
	}

	auto actualMovement = getVelocity();
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
	mPosition += actualMovement;
	afterMove(actualMovement);
}
