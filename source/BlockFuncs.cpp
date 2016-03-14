#include "stdinclude.h"
#include "Typedefs.h"
#include "Definitions.h"
#include "API.h"
#include "BlockFuncs.h"
#include "Blocks.h"
#include "World.h"

bool GrassBUF(Blocks::BUDDP * args)
{
	int x = args->cp >> 8;
	int y = (args->cp >> 4) % 16;
	int z = args->cp % 16;
	long long bx = args->cx * 16 + x;
	long long by = args->cy * 16 + y;
	long long bz = args->cz * 16 + z;
	block* b;
	b = &World::getblock(bx - 1, by, bz);
	if (b->ID != Blocks::AIR) {
		*(args->slf)=block(Blocks::DIRT);
		return true;
	} else return false;
}
