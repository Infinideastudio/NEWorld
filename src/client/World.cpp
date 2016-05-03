
void renderblock(int x, int y, int z, chunk* chunkptr)
{

    double colors, color1, color2, color3, color4, tcx, tcy, size, EPS = 0.0;
    int cx = chunkptr->cx, cy = chunkptr->cy, cz = chunkptr->cz;
    int gx = cx * 16 + x, gy = cy * 16 + y, gz = cz * 16 + z;
    block blk[7] = { chunkptr->getblock(x,y,z) ,
        z < 15 ? chunkptr->getblock(x, y, z + 1) : getblock(gx, gy, gz + 1, block(Blocks::ROCK)),
        z>0 ? chunkptr->getblock(x, y, z - 1) : getblock(gx, gy, gz - 1, block(Blocks::ROCK)),
        x < 15 ? chunkptr->getblock(x + 1, y, z) : getblock(gx + 1, gy, gz, block(Blocks::ROCK)),
        x>0 ? chunkptr->getblock(x - 1, y, z) : getblock(gx - 1, gy, gz, block(Blocks::ROCK)),
        y < 15 ? chunkptr->getblock(x, y + 1, z) : getblock(gx, gy + 1, gz, block(Blocks::ROCK)),
        y>0 ? chunkptr->getblock(x, y - 1, z) : getblock(gx, gy - 1, gz, block(Blocks::ROCK))
    };

    brightness brt[7] = { chunkptr->getbrightness(x,y,z) ,
        z < 15 ? chunkptr->getbrightness(x,y,z + 1) : getbrightness(gx,gy,gz + 1),
        z>0 ? chunkptr->getbrightness(x,y,z - 1) : getbrightness(gx,gy,gz - 1),
        x < 15 ? chunkptr->getbrightness(x + 1,y,z) : getbrightness(gx + 1,gy,gz),
        x>0 ? chunkptr->getbrightness(x - 1,y,z) : getbrightness(gx - 1,gy,gz),
        y < 15 ? chunkptr->getbrightness(x,y + 1,z) : getbrightness(gx,gy + 1,gz),
        y>0 ? chunkptr->getbrightness(x,y - 1,z) : getbrightness(gx,gy - 1,gz)
    };

    size = 1 / 8.0f - EPS;

    if (NiceGrass && blk[0] == block(Blocks::GRASS) && getblock(gx, gy - 1, gz + 1, block(Blocks::ROCK), chunkptr) == block(Blocks::GRASS))
    {
        tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
    }
    else
    {
        tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
    }

    // Front Face
    if (!(BlockInfo(blk[1]).isOpaque() || (blk[1] == blk[0] && BlockInfo(blk[0]).isOpaque() == false)) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[1];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (colors + getbrightness(gx, gy - 1, gz + 1) + getbrightness(gx - 1, gy, gz + 1) + getbrightness(gx - 1, gy - 1, gz + 1)) / 4.0;
            color2 = (colors + getbrightness(gx, gy - 1, gz + 1) + getbrightness(gx + 1, gy, gz + 1) + getbrightness(gx + 1, gy - 1, gz + 1)) / 4.0;
            color3 = (colors + getbrightness(gx, gy + 1, gz + 1) + getbrightness(gx + 1, gy, gz + 1) + getbrightness(gx + 1, gy + 1, gz + 1)) / 4.0;
            color4 = (colors + getbrightness(gx, gy + 1, gz + 1) + getbrightness(gx - 1, gy, gz + 1) + getbrightness(gx - 1, gy + 1, gz + 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;
        if (blk[0] != block(Blocks::GLOWSTONE) && !Renderer::AdvancedRender)
        {
            color1 *= 0.5;
            color2 *= 0.5;
            color3 *= 0.5;
            color4 *= 0.5;
        }

        if (Renderer::AdvancedRender) Renderer::Attrib1f(0.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx, tcy);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, 0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size, tcy);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, 0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size, tcy + size);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, 0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx, tcy + size);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, 0.5 + z);

    }

    if (NiceGrass && blk[0] == block(Blocks::GRASS) && getblock(gx, gy - 1, gz - 1, block(Blocks::ROCK), chunkptr) == block(Blocks::GRASS))
    {
        tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
    }
    else
    {
        tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
    }

    // Back Face
    if (!(BlockInfo(blk[2]).isOpaque() || (blk[2] == blk[0] && BlockInfo(blk[0]).isOpaque() == false)) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[2];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (colors + getbrightness(gx, gy - 1, gz - 1) + getbrightness(gx - 1, gy, gz - 1) + getbrightness(gx - 1, gy - 1, gz - 1)) / 4.0;
            color2 = (colors + getbrightness(gx, gy + 1, gz - 1) + getbrightness(gx - 1, gy, gz - 1) + getbrightness(gx - 1, gy + 1, gz - 1)) / 4.0;
            color3 = (colors + getbrightness(gx, gy + 1, gz - 1) + getbrightness(gx + 1, gy, gz - 1) + getbrightness(gx + 1, gy + 1, gz - 1)) / 4.0;
            color4 = (colors + getbrightness(gx, gy - 1, gz - 1) + getbrightness(gx + 1, gy, gz - 1) + getbrightness(gx + 1, gy - 1, gz - 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;
        if (blk[0] != block(Blocks::GLOWSTONE) && !Renderer::AdvancedRender)
        {
            color1 *= 0.5;
            color2 *= 0.5;
            color3 *= 0.5;
            color4 *= 0.5;
        }

        if (Renderer::AdvancedRender) Renderer::Attrib1f(1.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*0.0);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, -0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*1.0);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, -0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*1.0);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, -0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*0.0);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, -0.5 + z);

    }

    if (NiceGrass && blk[0] == block(Blocks::GRASS) && getblock(gx + 1, gy - 1, gz, block(Blocks::ROCK), chunkptr) == block(Blocks::GRASS))
    {
        tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
    }
    else
    {
        tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
    }

    // Right face
    if (!(BlockInfo(blk[3]).isOpaque() || (blk[3] == blk[0] && !BlockInfo(blk[0]).isOpaque())) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[3];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (colors + getbrightness(gx + 1, gy - 1, gz) + getbrightness(gx + 1, gy, gz - 1) + getbrightness(gx + 1, gy - 1, gz - 1)) / 4.0;
            color2 = (colors + getbrightness(gx + 1, gy + 1, gz) + getbrightness(gx + 1, gy, gz - 1) + getbrightness(gx + 1, gy + 1, gz - 1)) / 4.0;
            color3 = (colors + getbrightness(gx + 1, gy + 1, gz) + getbrightness(gx + 1, gy, gz + 1) + getbrightness(gx + 1, gy + 1, gz + 1)) / 4.0;
            color4 = (colors + getbrightness(gx + 1, gy - 1, gz) + getbrightness(gx + 1, gy, gz + 1) + getbrightness(gx + 1, gy - 1, gz + 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;
        if (blk[0] != block(Blocks::GLOWSTONE) && !Renderer::AdvancedRender)
        {
            color1 *= 0.7;
            color2 *= 0.7;
            color3 *= 0.7;
            color4 *= 0.7;
        }

        if (Renderer::AdvancedRender) Renderer::Attrib1f(2.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*0.0);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, -0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*1.0);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, -0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*1.0);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, 0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*0.0);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, 0.5 + z);

    }

    if (NiceGrass && blk[0] == block(Blocks::GRASS) && getblock(gx - 1, gy - 1, gz, block(Blocks::ROCK), chunkptr) == block(Blocks::GRASS))
    {
        tcx = Textures::getTexcoordX(blk[0], 1) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 1) + EPS;
    }
    else
    {
        tcx = Textures::getTexcoordX(blk[0], 2) + EPS;
        tcy = Textures::getTexcoordY(blk[0], 2) + EPS;
    }

    // Left Face
    if (!(BlockInfo(blk[4]).isOpaque() || (blk[4] == blk[0] && BlockInfo(blk[0]).isOpaque() == false)) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[4];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (colors + getbrightness(gx - 1, gy - 1, gz) + getbrightness(gx - 1, gy, gz - 1) + getbrightness(gx - 1, gy - 1, gz - 1)) / 4.0;
            color2 = (colors + getbrightness(gx - 1, gy - 1, gz) + getbrightness(gx - 1, gy, gz + 1) + getbrightness(gx - 1, gy - 1, gz + 1)) / 4.0;
            color3 = (colors + getbrightness(gx - 1, gy + 1, gz) + getbrightness(gx - 1, gy, gz + 1) + getbrightness(gx - 1, gy + 1, gz + 1)) / 4.0;
            color4 = (colors + getbrightness(gx - 1, gy + 1, gz) + getbrightness(gx - 1, gy, gz - 1) + getbrightness(gx - 1, gy + 1, gz - 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;
        if (blk[0] != block(Blocks::GLOWSTONE) && !Renderer::AdvancedRender)
        {
            color1 *= 0.7;
            color2 *= 0.7;
            color3 *= 0.7;
            color4 *= 0.7;
        }

        if (Renderer::AdvancedRender) Renderer::Attrib1f(3.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*0.0);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, -0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*0.0);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, 0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*1.0);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, 0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*1.0);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, -0.5 + z);

    }

    tcx = Textures::getTexcoordX(blk[0], 1);
    tcy = Textures::getTexcoordY(blk[0], 1);

    // Top Face
    if (!(BlockInfo(blk[5]).isOpaque() || (blk[5] == blk[0] && BlockInfo(blk[0]).isOpaque() == false)) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[5];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (color1 + getbrightness(gx, gy + 1, gz - 1) + getbrightness(gx - 1, gy + 1, gz) + getbrightness(gx - 1, gy + 1, gz - 1)) / 4.0;
            color2 = (color2 + getbrightness(gx, gy + 1, gz + 1) + getbrightness(gx - 1, gy + 1, gz) + getbrightness(gx - 1, gy + 1, gz + 1)) / 4.0;
            color3 = (color3 + getbrightness(gx, gy + 1, gz + 1) + getbrightness(gx + 1, gy + 1, gz) + getbrightness(gx + 1, gy + 1, gz + 1)) / 4.0;
            color4 = (color4 + getbrightness(gx, gy + 1, gz - 1) + getbrightness(gx + 1, gy + 1, gz) + getbrightness(gx + 1, gy + 1, gz - 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;

        if (Renderer::AdvancedRender) Renderer::Attrib1f(4.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*1.0);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, -0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*0.0);
        Renderer::Vertex3d(-0.5 + x, 0.5 + y, 0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*0.0);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, 0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*1.0);
        Renderer::Vertex3d(0.5 + x, 0.5 + y, -0.5 + z);

    }

    tcx = Textures::getTexcoordX(blk[0], 3);
    tcy = Textures::getTexcoordY(blk[0], 3);

    // Bottom Face
    if (!(BlockInfo(blk[6]).isOpaque() || (blk[6] == blk[0] && BlockInfo(blk[0]).isOpaque() == false)) || blk[0] == block(Blocks::LEAF))
    {

        colors = brt[6];
        color1 = colors;
        color2 = colors;
        color3 = colors;
        color4 = colors;

        if (blk[0] != block(Blocks::GLOWSTONE) && SmoothLighting)
        {
            color1 = (colors + getbrightness(gx, gy - 1, gz - 1) + getbrightness(gx - 1, gy - 1, gz) + getbrightness(gx - 1, gy - 1, gz - 1)) / 4.0;
            color2 = (colors + getbrightness(gx, gy - 1, gz - 1) + getbrightness(gx + 1, gy - 1, gz) + getbrightness(gx + 1, gy - 1, gz - 1)) / 4.0;
            color3 = (colors + getbrightness(gx, gy - 1, gz + 1) + getbrightness(gx + 1, gy - 1, gz) + getbrightness(gx + 1, gy - 1, gz + 1)) / 4.0;
            color4 = (colors + getbrightness(gx, gy - 1, gz + 1) + getbrightness(gx - 1, gy - 1, gz) + getbrightness(gx - 1, gy - 1, gz + 1)) / 4.0;
        }

        color1 /= BRIGHTNESSMAX;
        color2 /= BRIGHTNESSMAX;
        color3 /= BRIGHTNESSMAX;
        color4 /= BRIGHTNESSMAX;

        if (Renderer::AdvancedRender) Renderer::Attrib1f(5.0f);
        Renderer::Color3d(color1, color1, color1);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*1.0);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, -0.5 + z);
        Renderer::Color3d(color2, color2, color2);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*1.0);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, -0.5 + z);
        Renderer::Color3d(color3, color3, color3);
        Renderer::TexCoord2d(tcx + size*0.0, tcy + size*0.0);
        Renderer::Vertex3d(0.5 + x, -0.5 + y, 0.5 + z);
        Renderer::Color3d(color4, color4, color4);
        Renderer::TexCoord2d(tcx + size*1.0, tcy + size*0.0);
        Renderer::Vertex3d(-0.5 + x, -0.5 + y, 0.5 + z);

    }
}

