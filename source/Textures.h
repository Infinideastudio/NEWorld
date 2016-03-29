#pragma once
#include "Definitions.h"
#include "Blocks.h"
#include "Items.h"
#include "GLProc.h"

extern int BLOCKTEXTURE_SIZE, BLOCKTEXTURE_UNITSIZE;
const short BITMAP_ID = 0x4D42;

class Textures
{
public:
	struct TEXTURE_RGB
	{
		unsigned int sizeX;
		unsigned int sizeY;
		unique_ptr<ubyte[]> buffer;
	};

	struct TEXTURE_RGBA
	{
		unsigned int sizeX;
		unsigned int sizeY;
		unique_ptr<ubyte[]> buffer;
	};

#pragma pack(push)
#pragma pack(1)
	struct BITMAPINFOHEADER
	{
		int biSize = 40, biWidth, biHeight;
		short biPlanes = 1, biBitCount = 24;
		int biCompression = 0, biSizeImage, biXPelsPerMeter = 0, biYPelsPerMeter = 0, biClrUsed = 0, biClrImportant = 0;
	};

	struct BITMAPFILEHEADER
	{
		short bfType = BITMAP_ID;
		int bfSize;
		short bfReserved1 = 0, bfReserved2 = 0;
		int bfOffBits = 54;
	};
#pragma pack(pop)

	enum BlockTextureID
	{
		ROCK, GRASS_TOP, GRASS_SIDE, DIRT, STONE, PLANK, WOOD_TOP, WOOD_SIDE, BEDROCK, LEAF,
		GLASS, WATER, LAVA, GLOWSTONE, SAND, CEMENT, ICE, COAL, IRON, TNT, UNKNOWN
	};
	static const int NULLBLOCK = 63;

	static ubyte getTextureIndex(block blockname, ubyte side);
	static double getTexcoordX(item item, ubyte side)
	{
		if (isBlock(item)) //如果为方块
			return (getTextureIndex(item, side) & 7) / 8.0;
		else
			return NULLBLOCK;
	}
	static double getTexcoordY(item item, ubyte side)
	{
		if (isBlock(item)) //如果为方块
			return (getTextureIndex(item, side) >> 3) / 8.0;
		else
			return NULLBLOCK;
	}
	static void LoadRGBImage(TEXTURE_RGB& tex, string Filename);
	static void LoadRGBAImage(TEXTURE_RGBA& tex, string Filename, string MkFilename);

	static TextureID LoadRGBTexture(string Filename);
	static TextureID LoadFontTexture(string Filename);
	static TextureID LoadRGBATexture(string Filename, string MkFilename);
	static TextureID LoadBlock3DTexture(string Filename, string MkFilename);

	static void SaveRGBImage(string filename, TEXTURE_RGB& image);

	static void Build2DMipmaps(GLenum format, int w, int h, int level, const ubyte* src);
};
