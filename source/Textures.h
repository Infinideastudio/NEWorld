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

	struct TEXTURE_RGB
	{
		unsigned int sizeX;
		unsigned int sizeY;
		unique_ptr<ubyte[]> buffer;
		TEXTURE_RGB()
		{
		}
		TEXTURE_RGB(unsigned int _sizeX, unsigned int _sizeY)
			:sizeX(_sizeX), sizeY(_sizeY)
		{
			buffer = unique_ptr<ubyte[]>(new ubyte[_sizeX * _sizeY * 3]);
		}
		TEXTURE_RGB(const std::string& filename)
		{
			unsigned int ind = 0;
			std::ifstream bmpfile(filename, std::ios::binary | std::ios::in); //位图文件（二进制）
			BITMAPINFOHEADER bih;
			BITMAPFILEHEADER bfh;
			bmpfile.read((char*)&bfh, sizeof(BITMAPFILEHEADER));
			bmpfile.read((char*)&bih, sizeof(BITMAPINFOHEADER));
			*this = TEXTURE_RGB(bih.biWidth, bih.biHeight);
			bmpfile.read((char*)buffer.get(), sizeX * sizeY * 3);
			bmpfile.close();
			for (unsigned int i = 0; i < sizeX * sizeY; i++)
			{
				unsigned char t = buffer[ind];
				buffer[ind] = buffer[ind + 2];
				buffer[ind + 2] = t;
				ind += 3;
			}
		}
	};

	struct TEXTURE_RGBA
	{
		unsigned int sizeX;
		unsigned int sizeY;
		unique_ptr<ubyte[]> buffer;
		TEXTURE_RGBA()
		{
		}
		TEXTURE_RGBA(unsigned int _sizeX, unsigned int _sizeY)
			:sizeX(_sizeX), sizeY(_sizeY), buffer(unique_ptr<ubyte[]>(new ubyte[_sizeX * _sizeY * 4]))
		{
		}
		TEXTURE_RGBA(const std::string& filename, const std::string& mask = "")
		{
			std::ifstream bmpfile(filename, std::ios::binary | std::ios::in), maskfile;
			BITMAPFILEHEADER bfh, mbfh;
			BITMAPINFOHEADER bih, mbih;
			if (mask != "")
			{
				maskfile.open(mask, std::ios::binary | std::ios::in);
				maskfile.read((char*)&mbfh, sizeof(BITMAPFILEHEADER));
				maskfile.read((char*)&mbih, sizeof(BITMAPINFOHEADER));
			}
			bmpfile.read((char*)&bfh, sizeof(BITMAPFILEHEADER));
			bmpfile.read((char*)&bih, sizeof(BITMAPINFOHEADER));
			*this = TEXTURE_RGBA(bih.biWidth, bih.biHeight);
			unsigned char* rgb = new unsigned char[sizeX * sizeY * 3], *a = new unsigned char[sizeX * sizeY * 3];
			if (mask != "")
				maskfile.read((char*)a, sizeX * sizeY * 3);
			bmpfile.read((char*)rgb, sizeX * sizeY * 3);
			unsigned ind = 0;
			for (unsigned int i = 0; i < sizeX * sizeY; i++)
			{
				buffer[ind] = rgb[i * 3 + 2];
				buffer[ind + 1] = rgb[i * 3 + 1];
				buffer[ind + 2] = rgb[i * 3];
				buffer[ind + 3] = mask == "" ? 255 : 255 - a[i * 3];
				ind += 4;
			}
			delete[] a;
			delete[] rgb;
		}
	};

	enum BlockTextureID
	{
		ROCK, GRASS_TOP, GRASS_SIDE, DIRT, STONE, PLANK, WOOD_TOP, WOOD_SIDE, BEDROCK, LEAF,
		GLASS, WATER, LAVA, GLOWSTONE, SAND, CEMENT, ICE, COAL, IRON, TNT, UNKNOWN
	};

	enum
	{
		NULLBLOCK = 63
	};

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

	static TextureID LoadRGBTexture(string Filename);
	static TextureID LoadFontTexture(string Filename);
	static TextureID LoadRGBATexture(string Filename, string MkFilename);
	static TextureID LoadBlock3DTexture(string Filename, string MkFilename);

	static void SaveRGBImage(string filename, TEXTURE_RGB& image);

	static void Build2DMipmaps(GLenum format, int w, int h, int level, const ubyte* src);
};
