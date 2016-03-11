#include "Textures.h"
#include "Items.h"
#include "Blocks.h"
#include <fstream>

int BLOCKTEXTURE_SIZE, BLOCKTEXTURE_UNITSIZE, BLOCKTEXTURE_UNITS;

namespace Textures
{

void Init()
{
    BLOCKTEXTURE_SIZE = 256;
    BLOCKTEXTURE_UNITSIZE = 32;
    BLOCKTEXTURE_UNITS = 8;
}

ubyte getTextureIndex(block blockname, ubyte side)
{
    switch (blockname.ID)
    {
    case Blocks::ROCK:
        return ROCK;
    case Blocks::GRASS:
        switch (side)
        {
        case 1:
            return GRASS_TOP;
        case 2:
            return GRASS_SIDE;
        case 3:
            return DIRT;
        }
    case Blocks::DIRT:
        return DIRT;
    case Blocks::STONE:
        return STONE;
    case Blocks::PLANK:
        return PLANK;
    case Blocks::WOOD:
        switch (side)
        {
        case 1:
            return WOOD_TOP;
        case 2:
            return WOOD_SIDE;
        case 3:
            return WOOD_TOP;
        }
    case Blocks::BEDROCK:
        return BEDROCK;
    case Blocks::LEAF:
        return LEAF;
    case Blocks::GLASS:
        return GLASS;
    case Blocks::WATER:
        return WATER;
    case Blocks::LAVA:
        return LAVA;
    case Blocks::GLOWSTONE:
        return GLOWSTONE;
    case Blocks::SAND:
        return SAND;
    case Blocks::CEMENT:
        return CEMENT;
    case Blocks::ICE:
        return ICE;
    case Blocks::COAL:
        return COAL;
    case Blocks::IRON:
        return IRON;
    case Blocks::TNT:
        return TNT;
    default:
        return NULLBLOCK;
    }
}

double getTexcoordX(item item, ubyte side)
{
    if (isBlock(item)) //如果为方块
        return (getTextureIndex(item, side) & 7) / 8.0;
    else
        return NULLBLOCK;
}

double getTexcoordY(item item, ubyte side)
{
    if (isBlock(item)) //如果为方块
        return (getTextureIndex(item, side) >> 3) / 8.0;
    else
        return NULLBLOCK;
}

void LoadRGBImage(TEXTURE_RGB& tex, string Filename)
{
    unsigned int ind = 0;
    TEXTURE_RGB& bitmap = tex; //返回位图
    bitmap.buffer = nullptr;
    bitmap.sizeX = bitmap.sizeY = 0;
    std::ifstream bmpfile(Filename, std::ios::binary | std::ios::in); //位图文件（二进制）
    if (!bmpfile.is_open())
    {
        printf("[console][Warning] Cannot load %s\n", Filename.c_str());
        return;
    }
    BITMAPINFOHEADER bih; //各种关于位图的参数
    BITMAPFILEHEADER bfh; //各种关于文件的参数
    //开始读取
    bmpfile.read((char*)&bfh, sizeof(BITMAPFILEHEADER));
    bmpfile.read((char*)&bih, sizeof(BITMAPINFOHEADER));
    bitmap.sizeX = bih.biWidth;
    bitmap.sizeY = bih.biHeight;
    bitmap.buffer = unique_ptr<ubyte[]>(new unsigned char[bitmap.sizeX * bitmap.sizeY * 3]);
    //¶ÁÈ¡Êý¾Ý
    bmpfile.read((char*)bitmap.buffer.get(), bitmap.sizeX*bitmap.sizeY * 3);
    bmpfile.close();
    //ºÏ²¢Óë×ª»»
    for (unsigned int i = 0; i < bitmap.sizeX * bitmap.sizeY; i++)
    {
        //°ÑBGR¸ñÊ½×ª»»ÎªRGB¸ñÊ½
        unsigned char t = bitmap.buffer[ind];
        bitmap.buffer[ind] = bitmap.buffer[ind + 2];
        bitmap.buffer[ind + 2] = t;
        ind += 3;
    }
}

void LoadRGBAImage(TEXTURE_RGBA& tex, string Filename, string MkFilename)
{
    unsigned char *rgb = nullptr, *a = nullptr;
    unsigned int ind = 0;
    bool noMaskFile = (MkFilename == "");
    TEXTURE_RGBA& bitmap = tex; //·µ»ØÎ»Í¼
    bitmap.buffer = nullptr;
    bitmap.sizeX = bitmap.sizeY = 0;
    std::ifstream bmpfile(Filename, std::ios::binary | std::ios::in); //Î»Í¼ÎÄ¼þ£¨¶þ½øÖÆ£©
    std::ifstream maskfile;
    if (!noMaskFile)maskfile.open(MkFilename, std::ios::binary | std::ios::in); //ÕÚÕÖÎ»Í¼ÎÄ¼þ£¨¶þ½øÖÆ£©
    if (!bmpfile.is_open())
    {
        std::stringstream ss;
        ss << "Cannot load bitmap " << Filename;
        DebugWarning(ss.str());
        return;
    }
    if (!noMaskFile && !maskfile.is_open())
    {
        std::stringstream ss;
        ss << "Cannot load bitmap " << MkFilename;
        DebugWarning(ss.str());
        return;
    }
    BITMAPFILEHEADER bfh; //¸÷ÖÖ¹ØÓÚÎÄ¼þµÄ²ÎÊý
    BITMAPINFOHEADER bih; //¸÷ÖÖ¹ØÓÚÎ»Í¼µÄ²ÎÊý
    //¿ªÊ¼¶ÁÈ¡
    if (!noMaskFile)
    {
        maskfile.read((char*)&bfh, sizeof(BITMAPFILEHEADER)); //ÕâÁ½¸öÊÇÕ¼Î»maskÎÄ¼þµÄ
        maskfile.read((char*)&bih, sizeof(BITMAPINFOHEADER)); //µ½ÁËºóÃæmask¿ÉÒÔÖ±½Ó´ÓÑÕÉ«²¿·Ö¿ªÊ¼¶ÁÈ¡
    }
    bmpfile.read((char*)&bfh, sizeof(BITMAPFILEHEADER)); //ÕæÕýµÄinfoÒÔÕâ¸öbmpÎÄ¼þÎª×¼
    bmpfile.read((char*)&bih, sizeof(BITMAPINFOHEADER)); //Ëü½«¸²¸ÇÖ®Ç°´ÓmaskÎÄ¼þ¶Á³öÀ´µÄinfoÊý¾Ý
    bitmap.sizeX = bih.biWidth;
    bitmap.sizeY = bih.biHeight;
    bitmap.buffer = unique_ptr<ubyte[]>(new unsigned char[bitmap.sizeX * bitmap.sizeY * 4]);
    //¶ÁÈ¡Êý¾Ý
    rgb = new unsigned char[bitmap.sizeX * bitmap.sizeY * 3];
    bmpfile.read((char*)rgb, bitmap.sizeX*bitmap.sizeY * 3);
    bmpfile.close();
    if (!noMaskFile)
    {
        a = new unsigned char[bitmap.sizeX*bitmap.sizeY * 3];
        maskfile.read((char*)a, bitmap.sizeX*bitmap.sizeY * 3);
        maskfile.close();
    }
    //ºÏ²¢Óë×ª»»
    for (unsigned int i = 0; i < bitmap.sizeX * bitmap.sizeY; i++)
    {
        //°ÑBGR¸ñÊ½×ª»»ÎªRGB¸ñÊ½
        bitmap.buffer[ind] = rgb[i * 3 + 2];
        bitmap.buffer[ind + 1] = rgb[i * 3 + 1];
        bitmap.buffer[ind + 2] = rgb[i * 3];
        //Alpha
        if (noMaskFile) bitmap.buffer[ind + 3] = 255;
        else bitmap.buffer[ind + 3] = 255 - a[i * 3];
        ind += 4;
    }
}

TextureID LoadRGBTexture(string Filename)
{
    TEXTURE_RGB image;
    TextureID ret;
    LoadRGBImage(image, Filename);
    glGenTextures(1, &ret);
    glBindTexture(GL_TEXTURE_2D, ret);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    Build2DMipmaps(GL_RGB, image.sizeX, image.sizeY, (int)log2(image.sizeX), image.buffer.get());
    return ret;
}

TextureID LoadFontTexture(string Filename)
{
    TEXTURE_RGBA Texture;
    TEXTURE_RGB image;
    ubyte *ip, *tp;
    TextureID ret;
    LoadRGBImage(image, Filename);
    Texture.sizeX = image.sizeX;
    Texture.sizeY = image.sizeY;
    Texture.buffer = unique_ptr<ubyte[]>(new unsigned char[image.sizeX * image.sizeY * 4]);
    if (Texture.buffer == nullptr)
    {
        printf("[console][Warning] Cannot alloc memory when loading %s\n", Filename.c_str());
        return 0;
    }
    ip = image.buffer.get();
    tp = Texture.buffer.get();
    for (unsigned int i = 0; i != image.sizeX*image.sizeY; i++)
    {
        *tp = 255;
        tp++;
        *tp = 255;
        tp++;
        *tp = 255;
        tp++;
        *tp = 255 - *ip;
        tp++;
        ip += 3;
    }
    glGenTextures(1, &ret);
    glBindTexture(GL_TEXTURE_2D, ret);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture.sizeX, Texture.sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, Texture.buffer.get());
    return ret;
}

TextureID LoadRGBATexture(string Filename, string MkFilename)
{
    TextureID ret;
    TEXTURE_RGBA image;
    LoadRGBAImage(image, Filename, MkFilename);
    glGenTextures(1, &ret);
    glBindTexture(GL_TEXTURE_2D, ret);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    Build2DMipmaps(GL_RGBA, image.sizeX, image.sizeY, (int)log2(BLOCKTEXTURE_UNITSIZE), image.buffer.get());
    return ret;
}

TextureID LoadBlock3DTexture(string Filename, string MkFilename)
{
    int sz = BLOCKTEXTURE_UNITSIZE, cnt = BLOCKTEXTURE_UNITS*BLOCKTEXTURE_UNITS;
    //int mipmapLevel = (int)log2(BLOCKTEXTURE_UNITSIZE), sum = 0, cursize = 0, scale = 1;
    ubyte *src, *cur;
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_3D);
    TextureID ret;
    TEXTURE_RGBA image;
    LoadRGBAImage(image, Filename, MkFilename);
    src = image.buffer.get();
    cur = new ubyte[sz*sz*cnt * 4];
    glGenTextures(1, &ret);
    glBindTexture(GL_TEXTURE_3D, ret);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
    //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, mipmapLevel);
    //glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_LOD, 0);
    //glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAX_LOD, mipmapLevel);
    //glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, 0.0);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, sz, sz, cnt, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.buffer.get());
    /*
    for (int i = 1; i <= mipmapLevel; i++) {
    	scale *= 2; cursize = sz / scale;
    	for (int z = 0; z < cnt; z++) {
    		for (int x = 0; x < cursize; x++) for (int y = 0; y < cursize; y++) {
    			for (int col = 0; col < 4; col++) {
    				sum = 0;
    				for (int xx = 0; xx < scale; xx++) for (int yy = 0; yy < scale; yy++) {
    					sum += src[(z*sz*sz + (x * scale + xx) * sz + y * scale + yy) * 4 + col];
    				}
    				cur[(z*cursize*cursize + x*cursize + y) * 4 + col] = sum / (scale*scale);
    			}
    		}
    	}
    	glTexImage3D(GL_TEXTURE_3D, i, GL_RGBA, cursize, cursize, cnt / scale, 0, GL_RGBA, GL_UNSIGNED_BYTE, cur);
    }
    */
    glDisable(GL_TEXTURE_3D);
    glEnable(GL_TEXTURE_2D);
    delete[] cur;
    return ret;
}

void SaveRGBImage(string filename, TEXTURE_RGB& image)
{
    BITMAPFILEHEADER bitmapfileheader;
    BITMAPINFOHEADER bitmapinfoheader;
    bitmapfileheader.bfSize = image.sizeX*image.sizeY * 3 + 54;
    bitmapinfoheader.biWidth = image.sizeX;
    bitmapinfoheader.biHeight = image.sizeY;
    bitmapinfoheader.biSizeImage = image.sizeX*image.sizeY * 3;
    for (unsigned int i = 0; i != image.sizeX*image.sizeY * 3; i += 3)
    {
        //°ÑRGB¸ñÊ½×ª»»ÎªBGR¸ñÊ½
        ubyte t = image.buffer.get()[i];
        image.buffer.get()[i] = image.buffer.get()[i + 2];
        image.buffer.get()[i + 2] = t;
    }
    std::ofstream ofs(filename, std::ios::out | std::ios::binary);
    ofs.write((char*)&bitmapfileheader, sizeof(bitmapfileheader));
    ofs.write((char*)&bitmapinfoheader, sizeof(bitmapinfoheader));
    ofs.write((char*)image.buffer.get(), sizeof(ubyte)*image.sizeX*image.sizeY * 3);
    ofs.close();
}

void Build2DMipmaps(GLenum format, int w, int h, int level, const ubyte* src)
{
    int sum = 0, scale = 1, cur_w = 0, cur_h = 0, cc = 0;
    if (format == GL_RGBA) cc = 4;
    else if (format == GL_RGB) cc = 3;
    ubyte *cur = new ubyte[w*h*cc];
    memset(cur, 0, w*h*cc);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, level);
    glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, 0.0f);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, src);
    for (int i = 1; i <= level; i++)
    {
        scale <<= 1;
        cur_w = w / scale;
        cur_h = h / scale;
        for (int y = 0; y < cur_h; y++) for (int x = 0; x < cur_w; x++)
            {
                for (int col = 0; col < cc; col++)
                {
                    sum = 0;
                    for (int yy = 0; yy < scale; yy++) for (int xx = 0; xx < scale; xx++)
                        {
                            sum += src[((y * scale + yy) * w + x * scale + xx) * cc + col];
                        }
                    cur[(y * cur_w + x) * cc + col] = (ubyte)(sum / (scale*scale));
                }
            }
        glTexImage2D(GL_TEXTURE_2D, i, format, cur_w, cur_h, 0, format, GL_UNSIGNED_BYTE, cur);
    }
    delete[] cur;
}

}