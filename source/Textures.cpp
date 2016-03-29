#include "Textures.h"
#include <fstream>

int BLOCKTEXTURE_SIZE = 256, BLOCKTEXTURE_UNITSIZE = 32, BLOCKTEXTURE_UNITS = 8;

ubyte Textures::getTextureIndex(block blockname, ubyte side)
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

void Textures::LoadRGBImage(TEXTURE_RGB& tex, string Filename)
{
    unsigned int ind = 0;
    tex.buffer = nullptr;
    tex.sizeX = tex.sizeY = 0;
    std::ifstream bmpfile(Filename, std::ios::binary | std::ios::in); //位图文件（二进制）
#ifdef _DEBUG
    assert(bmpfile.is_open());
#endif
    BITMAPINFOHEADER bih;
    BITMAPFILEHEADER bfh;
    bmpfile.read((char*)&bfh, sizeof(BITMAPFILEHEADER));
    bmpfile.read((char*)&bih, sizeof(BITMAPINFOHEADER));
    tex.sizeX = bih.biWidth;
    tex.sizeY = bih.biHeight;
    tex.buffer = unique_ptr<ubyte[]>(new unsigned char[tex.sizeX * tex.sizeY * 3]);
    bmpfile.read((char*)tex.buffer.get(), tex.sizeX * tex.sizeY * 3);
    bmpfile.close();
    for (unsigned int i = 0; i < tex.sizeX * tex.sizeY; i++)
    {
        unsigned char t = tex.buffer[ind];
        tex.buffer[ind] = tex.buffer[ind + 2];
        tex.buffer[ind + 2] = t;
        ind += 3;
    }
}

void Textures::LoadRGBAImage(TEXTURE_RGBA& tex, string Filename, string MkFilename)
{
    unsigned char *rgb = nullptr, *a = nullptr;
    unsigned int ind = 0;
    bool noMaskFile = (MkFilename == "");
    TEXTURE_RGBA& bitmap = tex;
    bitmap.buffer = nullptr;
    bitmap.sizeX = bitmap.sizeY = 0;
    std::ifstream bmpfile(Filename, std::ios::binary | std::ios::in);
    std::ifstream maskfile;
    if (!noMaskFile)maskfile.open(MkFilename, std::ios::binary | std::ios::in);
#ifdef _DEBUG
	assert(bmpfile.is_open());
	assert(noMaskFile || maskfile.is_open());
#endif
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    if (!noMaskFile)
    {
        maskfile.read((char*)&bfh, sizeof(BITMAPFILEHEADER));
        maskfile.read((char*)&bih, sizeof(BITMAPINFOHEADER));
    }
    bmpfile.read((char*)&bfh, sizeof(BITMAPFILEHEADER));
    bmpfile.read((char*)&bih, sizeof(BITMAPINFOHEADER));
    bitmap.sizeX = bih.biWidth;
    bitmap.sizeY = bih.biHeight;
    bitmap.buffer = unique_ptr<ubyte[]>(new unsigned char[bitmap.sizeX * bitmap.sizeY * 4]);
    rgb = new unsigned char[bitmap.sizeX * bitmap.sizeY * 3];
    bmpfile.read((char*)rgb, bitmap.sizeX*bitmap.sizeY * 3);
    bmpfile.close();
    if (!noMaskFile)
    {
        a = new unsigned char[bitmap.sizeX*bitmap.sizeY * 3];
        maskfile.read((char*)a, bitmap.sizeX*bitmap.sizeY * 3);
        maskfile.close();
    }
    for (unsigned int i = 0; i < bitmap.sizeX * bitmap.sizeY; i++)
    {
        bitmap.buffer[ind] = rgb[i * 3 + 2];
        bitmap.buffer[ind + 1] = rgb[i * 3 + 1];
        bitmap.buffer[ind + 2] = rgb[i * 3];
        if (noMaskFile) bitmap.buffer[ind + 3] = 255;
        else bitmap.buffer[ind + 3] = 255 - a[i * 3];
        ind += 4;
    }
}

TextureID Textures::LoadRGBTexture(string Filename)
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

TextureID Textures::LoadFontTexture(string Filename)
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

TextureID Textures::LoadRGBATexture(string Filename, string MkFilename)
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

TextureID Textures::LoadBlock3DTexture(string Filename, string MkFilename)
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

void Textures::SaveRGBImage(string filename, TEXTURE_RGB& image)
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

void Textures::Build2DMipmaps(GLenum format, int w, int h, int level, const ubyte* src)
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
            for (int col = 0; col < cc; col++)
            {
                sum = 0;
                for (int yy = 0; yy < scale; yy++) for (int xx = 0; xx < scale; xx++)
                    sum += src[((y * scale + yy) * w + x * scale + xx) * cc + col];
                cur[(y * cur_w + x) * cc + col] = (ubyte)(sum / (scale*scale));
            }
        glTexImage2D(GL_TEXTURE_2D, i, format, cur_w, cur_h, 0, format, GL_UNSIGNED_BYTE, cur);
    }
    delete[] cur;
}
