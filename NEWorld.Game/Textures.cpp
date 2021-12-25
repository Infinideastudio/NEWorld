#include "Textures.h"
#include "Items.h"
#include "Universe/World/Blocks.h"
#include <cstring>
#include <fstream>
#include "Common/Logger.h"

int BLOCKTEXTURE_SIZE, BLOCKTEXTURE_UNITSIZE, BLOCKTEXTURE_UNITS;

namespace Textures {

    void Init() {
        BLOCKTEXTURE_SIZE = 256;
        BLOCKTEXTURE_UNITSIZE = 32;
        BLOCKTEXTURE_UNITS = 8;
    }

    ubyte getTextureIndex(Block blockname, ubyte side) {
        switch (blockname) {
            case Blocks::ROCK:
                return ROCK;
            case Blocks::GRASS:
                switch (side) {
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
                switch (side) {
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

    double getTexcoordX(item item, ubyte side) {
        if (isBlock(item)) //如果为方块
            return (getTextureIndex(item, side) & 7) / 8.0;
        else
            return NULLBLOCK;
    }

    double getTexcoordY(item item, ubyte side) {
        if (isBlock(item)) //如果为方块
            return (getTextureIndex(item, side) >> 3) / 8.0;
        else
            return NULLBLOCK;
    }

    void LoadRGBImage(TEXTURE_RGB &tex, std::string Filename) {
        unsigned int ind = 0;
        auto& bitmap = tex; //返回位图
        bitmap.buffer = nullptr;
        bitmap.sizeX = bitmap.sizeY = 0;
        std::ifstream bmpfile(Filename, std::ios::binary | std::ios::in); //位图文件（二进制）
        if (!bmpfile.is_open()) {
            warningstream << "Cannot load " << Filename;
            return;
        }
        BITMAPINFOHEADER bih; //各种关于位图的参数
        BITMAPFILEHEADER bfh; //各种关于文件的参数
        //开始读取
        bmpfile.read((char *) &bfh, sizeof(BITMAPFILEHEADER));
        bmpfile.read((char *) &bih, sizeof(BITMAPINFOHEADER));
        bitmap.sizeX = bih.biWidth;
        bitmap.sizeY = bih.biHeight;
        bitmap.buffer = std::unique_ptr<ubyte[]>(new unsigned char[bitmap.sizeX * bitmap.sizeY * 3]);
        bmpfile.read((char *) bitmap.buffer.get(), bitmap.sizeX * bitmap.sizeY * 3);
        bmpfile.close();
        for (unsigned int i = 0; i < bitmap.sizeX * bitmap.sizeY; i++) {
            const auto t = bitmap.buffer[ind];
            bitmap.buffer[ind] = bitmap.buffer[ind + 2];
            bitmap.buffer[ind + 2] = t;
            ind += 3;
        }
    }

    void LoadRGBAImage(TEXTURE_RGBA &tex, std::string Filename, std::string MkFilename) {
        unsigned char *rgb = nullptr, *a = nullptr;
        unsigned int ind = 0;
        auto noMaskFile = (MkFilename == "");
        auto& bitmap = tex; //·µ»ØÎ»Í¼
        bitmap.buffer = nullptr;
        bitmap.sizeX = bitmap.sizeY = 0;
        std::ifstream bmpfile(Filename, std::ios::binary | std::ios::in);
        std::ifstream maskfile;
        if (!noMaskFile)maskfile.open(MkFilename, std::ios::binary | std::ios::in);
        if (!bmpfile.is_open()) {
            warningstream << "Cannot load bitmap " << Filename;
            return;
        }
        if (!noMaskFile && !maskfile.is_open()) {
            warningstream << "Cannot load bitmap " << MkFilename;
            return;
        }
        BITMAPFILEHEADER bfh;
        BITMAPINFOHEADER bih;

        if (!noMaskFile) {
            maskfile.read((char *) &bfh, sizeof(BITMAPFILEHEADER));
            maskfile.read((char *) &bih, sizeof(BITMAPINFOHEADER));
        }
        bmpfile.read((char *) &bfh, sizeof(BITMAPFILEHEADER));
        bmpfile.read((char *) &bih, sizeof(BITMAPINFOHEADER));
        bitmap.sizeX = bih.biWidth;
        bitmap.sizeY = bih.biHeight;
        bitmap.buffer = std::unique_ptr<ubyte[]>(new unsigned char[bitmap.sizeX * bitmap.sizeY * 4]);
        //¶ÁÈ¡Êý¾Ý
        rgb = new unsigned char[bitmap.sizeX * bitmap.sizeY * 3];
        bmpfile.read((char *) rgb, bitmap.sizeX * bitmap.sizeY * 3);
        bmpfile.close();
        if (!noMaskFile) {
            a = new unsigned char[bitmap.sizeX * bitmap.sizeY * 3];
            maskfile.read((char *) a, bitmap.sizeX * bitmap.sizeY * 3);
            maskfile.close();
        }
        //ºÏ²¢Óë×ª»»
        for (unsigned int i = 0; i < bitmap.sizeX * bitmap.sizeY; i++) {
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

    TextureID LoadRGBTexture(std::string Filename) {
        TEXTURE_RGB image;
        TextureID ret;
        LoadRGBImage(image, Filename);
        glGenTextures(1, &ret);
        glBindTexture(GL_TEXTURE_2D, ret);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        Build2DMipmaps(GL_RGB, image.sizeX, image.sizeY, static_cast<int>(log2(image.sizeX)), image.buffer.get());
        return ret;
    }

    TextureID LoadFontTexture(std::string Filename) {
        TEXTURE_RGBA Texture;
        TEXTURE_RGB image;
        TextureID ret;
        LoadRGBImage(image, Filename);
        Texture.sizeX = image.sizeX;
        Texture.sizeY = image.sizeY;
        Texture.buffer = std::unique_ptr<ubyte[]>(new unsigned char[image.sizeX * image.sizeY * 4]);
        if (Texture.buffer == nullptr) {
            printf("[console][Warning] Cannot alloc memory when loading %s\n", Filename.c_str());
            return 0;
        }
        auto ip = image.buffer.get();
        auto tp = Texture.buffer.get();
        for (unsigned int i = 0; i != image.sizeX * image.sizeY; i++) {
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Texture.sizeX, Texture.sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     Texture.buffer.get());
        return ret;
    }

    TextureID LoadRGBATexture(std::string Filename, std::string MkFilename) {
        TextureID ret;
        TEXTURE_RGBA image;
        LoadRGBAImage(image, Filename, MkFilename);
        glGenTextures(1, &ret);
        glBindTexture(GL_TEXTURE_2D, ret);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        Build2DMipmaps(GL_RGBA, image.sizeX, image.sizeY, static_cast<int>(log2(BLOCKTEXTURE_UNITSIZE)), image.buffer.get());
        return ret;
    }

    void SaveRGBImage(std::string filename, TEXTURE_RGB &image) {
        BITMAPFILEHEADER bitmapfileheader;
        BITMAPINFOHEADER bitmapinfoheader;
        bitmapfileheader.bfSize = image.sizeX * image.sizeY * 3 + 54;
        bitmapinfoheader.biWidth = image.sizeX;
        bitmapinfoheader.biHeight = image.sizeY;
        bitmapinfoheader.biSizeImage = image.sizeX * image.sizeY * 3;
        for (unsigned int i = 0; i != image.sizeX * image.sizeY * 3; i += 3) {
            const auto t = image.buffer.get()[i];
            image.buffer.get()[i] = image.buffer.get()[i + 2];
            image.buffer.get()[i + 2] = t;
        }
        std::ofstream ofs(filename, std::ios::out | std::ios::binary);
        ofs.write((char *) &bitmapfileheader, sizeof(bitmapfileheader));
        ofs.write((char *) &bitmapinfoheader, sizeof(bitmapinfoheader));
        ofs.write((char *) image.buffer.get(), sizeof(ubyte) * image.sizeX * image.sizeY * 3);
        ofs.close();
    }

    void Build2DMipmaps(GLenum format, int w, int h, int level, const ubyte *src) {
        auto sum = 0, scale = 1, cur_w = 0, cur_h = 0, cc = 0;
        if (format == GL_RGBA) cc = 4;
        else if (format == GL_RGB) cc = 3;
        const auto cur = new ubyte[w * h * cc];
        memset(cur, 0, w * h * cc);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, level);
        glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, 0.0f);
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, src);
        for (auto i = 1; i <= level; i++) {
            scale <<= 1;
            cur_w = w / scale;
            cur_h = h / scale;
            for (auto y = 0; y < cur_h; y++)
                for (auto x = 0; x < cur_w; x++) {
                    for (auto col = 0; col < cc; col++) {
                        sum = 0;
                        for (auto yy = 0; yy < scale; yy++)
                            for (auto xx = 0; xx < scale; xx++) {
                                sum += src[((y * scale + yy) * w + x * scale + xx) * cc + col];
                            }
                        cur[(y * cur_w + x) * cc + col] = static_cast<ubyte>(sum / (scale * scale));
                    }
                }
            glTexImage2D(GL_TEXTURE_2D, i, format, cur_w, cur_h, 0, format, GL_UNSIGNED_BYTE, cur);
        }
        delete[] cur;
    }

}