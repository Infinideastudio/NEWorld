#include "stdinclude.h"
#include "Typedefs.h"
#include "Definitions.h"
#include "API.h"
#include "BlockFuncs.h"
#include "Blocks.h"
#include "World.h"

bool GrassBUF(Blocks::BUDDP * args)
{
	long long bx = args->cx;
	long long by = args->cy;
	long long bz = args->cz;
	block b;
	b = World::getblock(bx, by + 1, bz);
	if (b.ID != Blocks::AIR) {
		*(args->slf)=block(Blocks::DIRT);
		return true;
	} 
	return false;
}

bool WaterBUF(Blocks::BUDDP * args)
{
	if (args->slf->Data81==1){
		long long bx = args->cx;
		long long by = args->cy;
		long long bz = args->cz;
		bool set = false;
		block b=World::getblock(bx, by - 1, bz);
		if (b.ID == Blocks::AIR || ((b.ID == Blocks::WATER) && (b.Data81 == 0))) {
			World::setblock(bx, by - 1, bz, block(Blocks::WATER, 1, 255));
			set = true;
		}
		b = World::getblock(bx - 1, by, bz);
		if (b.ID == Blocks::AIR || ((b.ID == Blocks::WATER) && (b.Data81 == 0))) {
			World::setblock(bx - 1, by, bz, block(Blocks::WATER, 1, 255));
			set = true;
		}
		b = World::getblock(bx + 1, by, bz);
		if (b.ID == Blocks::AIR || ((b.ID == Blocks::WATER) && (b.Data81 == 0))) {
			World::setblock(bx + 1, by, bz, block(Blocks::WATER, 1, 255));
			set = true;
		}
		b = World::getblock(bx, by, bz - 1);
		if (b.ID == Blocks::AIR || ((b.ID == Blocks::WATER) && (b.Data81 == 0))) {
			World::setblock(bx, by, bz - 1, block(Blocks::WATER, 1, 255));
			set = true;
		}
		b = World::getblock(bx, by, bz + 1);
		if (b.ID == Blocks::AIR || ((b.ID == Blocks::WATER) && (b.Data81 == 0))) {
			World::setblock(bx, by, bz + 1, block(Blocks::WATER, 1, 255));
			set = true;
		}
		if (set) return true;
	}
	else if (args->slf->Data81 == 0) {
		long long bx = args->cx;
		long long by = args->cy;
		long long bz = args->cz;
		bool set = false, u = false;
		block b = World::getblock(bx, by - 1, bz);
		if (b.ID == Blocks::WATER && b.Data81 == 0) {
			int s=b.Data82 + args->slf->Data82;
			if (s < 255) {
				World::setblock(bx, by - 1, bz, block(Blocks::WATER, 0, s));
				return true;
			}
			else{
				World::setblock(bx, by - 1, bz, block(Blocks::WATER, 0, 255));
				args->slf->Data82 = s - 255;
				u = true;
			}
			bool left = false, right = false, front = false, back = false;
			bool total = args->slf->Data82;
			b = World::getblock(bx - 1, by, bz);
			if (b.ID == Blocks::WATER && b.Data81 == 0 && (b.Data82 < args->slf->Data82 - 2)) {
				left = u = true;
				total += b.Data82;
			}
			b = World::getblock(bx + 1, by, bz);
			if (b.ID == Blocks::WATER && b.Data81 == 0 && (b.Data82 < args->slf->Data82 - 2)) {
				right = u = true;
				total += b.Data82;
			}
			b = World::getblock(bx, by, bz - 1);
			if (b.ID == Blocks::WATER && b.Data81 == 0 && (b.Data82 < args->slf->Data82 - 2)) {
				front = u = true;
				total += b.Data82;
			}
			b = World::getblock(bx, by, bz + 1);
			if (b.ID == Blocks::WATER && b.Data81 == 0 && (b.Data82 < args->slf->Data82 - 2)) {
				back = u = true;
				total += b.Data82;
			}
			total /= (left+right+front+back);
			if (left) World::setblock(bx - 1, by, bz, block(Blocks::WATER, 0, total));
			if (right) World::setblock(bx + 1, by, bz, block(Blocks::WATER, 0, total));
			if (front) World::setblock(bx, by, bz - 1, block(Blocks::WATER, 0, total));
			if (back) World::setblock(bx, by, bz + 1, block(Blocks::WATER, 0, total));
			if (u) return u;
		}
	}
	return false;
}
