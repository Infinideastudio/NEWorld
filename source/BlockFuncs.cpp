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
	cout<<"grass.call"<<b.ID<<endl;
	if (b.ID != Blocks::AIR) {
		*(args->slf)=block(Blocks::DIRT);
		return true;
	} 
	return false;
}
