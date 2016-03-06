#include "OnlinePlayer.h"
#include "Renderer.h"

map<SkinID, pair<VBOID, vtxCount>> playerSkins;
vector<OnlinePlayer> players;

void OnlinePlayer::GenVAOVBO(int skinID)
{
    return;
    if (skinID != -1)   //默认皮肤
    {
        using Renderer::TexCoord2d;
        using Renderer::Vertex3f;
        using Renderer::Color3f;
        Renderer::Init(0, 0);
        //===Head===
        //Left
        Color3f(1, 1, 1);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 8);
        Vertex3f(-0.125f, 0.125f, 0.125f);
        TexCoord2d(1.0 / 8 * 0, 1.0 / 8 * 8);
        Vertex3f(-0.125f, 0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 0, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, 0.125f);
        TexCoord2d(1.0 / 8 * 8, 1.0 / 8 * 8);
        Vertex3f(-0.141f, 0.141f, 0.141f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 8);
        Vertex3f(-0.141f, 0.141f, -0.141f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 6);
        Vertex3f(-0.141f, -0.375f, -0.141f);
        TexCoord2d(1.0 / 8 * 8, 1.0 / 8 * 6);
        Vertex3f(-0.141f, -0.375f, 0.141f);
        //Right
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 8);
        Vertex3f(0.125f, 0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 8);
        Vertex3f(0.125f, 0.125f, 0.125f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, 0.125f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 8, 1.0 / 8 * 4);
        Vertex3f(0.141f, 0.141f, -0.141f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 4);
        Vertex3f(0.141f, 0.141f, 0.141f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 2);
        Vertex3f(0.141f, -0.375f, 0.141f);
        TexCoord2d(1.0 / 8 * 8, 1.0 / 8 * 2);
        Vertex3f(0.141f, -0.375f, -0.141f);
        //Front
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 8);
        Vertex3f(0.125f, 0.125f, 0.125f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 8);
        Vertex3f(-0.125f, 0.125f, 0.125f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, 0.125f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, 0.125f);
        TexCoord2d(1.0 / 8 * 8, 1.0 / 8 * 6);
        Vertex3f(0.141f, 0.141f, 0.141f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 6);
        Vertex3f(-0.141f, 0.141f, 0.141f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 4);
        Vertex3f(-0.141f, -0.375f, 0.141f);
        TexCoord2d(1.0 / 8 * 8, 1.0 / 8 * 4);
        Vertex3f(0.141f, -0.375f, 0.141f);
        //Back
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 8);
        Vertex3f(-0.125f, 0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 8);
        Vertex3f(0.125f, 0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 8, 1.0 / 8 * 2);
        Vertex3f(-0.141f, 0.141f, -0.141f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 2);
        Vertex3f(0.141f, 0.141f, -0.141f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 0);
        Vertex3f(0.141f, -0.375f, -0.141f);
        TexCoord2d(1.0 / 8 * 8, 1.0 / 8 * 0);
        Vertex3f(-0.141f, -0.375f, -0.141f);
        //Top
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 8);
        Vertex3f(0.125f, 0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 8);
        Vertex3f(-0.125f, 0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 7);
        Vertex3f(-0.125f, 0.125f, 0.125f);
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 7);
        Vertex3f(0.125f, 0.125f, 0.125f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 8);
        Vertex3f(0.141f, 0.141f, -0.141f);
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 8);
        Vertex3f(-0.141f, 0.141f, -0.141f);
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 7);
        Vertex3f(-0.141f, 0.141f, 0.141f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 7);
        Vertex3f(0.141f, 0.141f, 0.141f);
        //Bottom
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 8);
        Vertex3f(-0.125f, -0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 8);
        Vertex3f(0.125f, -0.125f, -0.125f);
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, 0.125f);
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, 0.125f);

        //===Body===
        //Left
        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, 0.0625f);
        //Right
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, -0.0625f);
        //Front
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 0, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 0, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, 0.0625f);
        //Back
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, -0.0625f);

        //===Left leg===
        //Left
        TexCoord2d(1.0 / 8 * 0.5, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 0, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 0, 1.0 / 8 * 2);
        Vertex3f(-0.125f, -1.375f, -0.0625f);
        TexCoord2d(1.0 / 8 * 0.5, 1.0 / 8 * 2);
        Vertex3f(-0.125f, -1.375f, 0.0625f);
        TexCoord2d(1.0 / 8 * 4.5, 1.0 / 8 * 5);
        Vertex3f(-0.141f, -0.625f, 0.0785f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 5);
        Vertex3f(-0.141f, -0.625f, -0.0785f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 2);
        Vertex3f(-0.141f, -1.391f, -0.0785f);
        TexCoord2d(1.0 / 8 * 4.5, 1.0 / 8 * 2);
        Vertex3f(-0.141f, -1.391f, 0.0785f);
        //Right

        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 0.5, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 0.5, 1.0 / 8 * 2);
        Vertex3f(0, -1.375f, 0.0625f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 2);
        Vertex3f(0, -1.375f, -0.0625f);
        //Front
        TexCoord2d(1.0 / 8 * 1.5, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 2);
        Vertex3f(-0.125f, -1.375f, 0.0625f);
        TexCoord2d(1.0 / 8 * 1.5, 1.0 / 8 * 2);
        Vertex3f(0, -1.375f, 0.0625f);
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, 0.0785f);
        TexCoord2d(1.0 / 8 * 4.5, 1.0 / 8 * 5);
        Vertex3f(-0.141f, -0.625f, 0.0785f);
        TexCoord2d(1.0 / 8 * 4.5, 1.0 / 8 * 2);
        Vertex3f(-0.141f, -1.391f, 0.0785f);
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 2);
        Vertex3f(0, -1.391f, 0.0785f);
        //Back
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 1.5, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 1.5, 1.0 / 8 * 2);
        Vertex3f(0, -1.375f, -0.0625f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 2);
        Vertex3f(-0.125f, -1.375f, -0.0625f);
        TexCoord2d(1.0 / 8 * 5.5, 1.0 / 8 * 5);
        Vertex3f(-0.141f, -0.625f, -0.0785f);
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, -0.0785f);
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 2);
        Vertex3f(0, -1.391f, -0.0785f);
        TexCoord2d(1.0 / 8 * 5.5, 1.0 / 8 * 2);
        Vertex3f(-0.141f, -1.391f, -0.0785f);
        //Bottom
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 1);
        Vertex3f(-0.125f, -1.375f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 1);
        Vertex3f(0, -1.375f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 0.5);
        Vertex3f(0, -1.375f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 0.5);
        Vertex3f(-0.125f, -1.375f, 0.0625f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 1);
        Vertex3f(-0.141f, -1.391f, -0.0785f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 1);
        Vertex3f(0, -1.391f, -0.0785f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 0.5);
        Vertex3f(0, -1.391f, 0.0785f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 0.5);
        Vertex3f(-0.141f, -1.391f, 0.0785f);

        //===Right leg===
        //Left

        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 2);
        Vertex3f(0, -1.375f, -0.0625f);
        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 2);
        Vertex3f(0, -1.375f, 0.0625f);
        //Right

        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 2);
        Vertex3f(0.125f, -1.375f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 2);
        Vertex3f(0.125f, -1.375f, -0.0625f);
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 5);
        Vertex3f(0.141f, -0.625f, -0.0785f);
        TexCoord2d(1.0 / 8 * 5.5, 1.0 / 8 * 5);
        Vertex3f(0.141f, -0.625f, 0.0785f);
        TexCoord2d(1.0 / 8 * 5.5, 1.0 / 8 * 2);
        Vertex3f(0.141f, -1.391f, 0.0785f);
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 2);
        Vertex3f(0.141f, -1.391f, -0.0785f);
        //Front

        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 2);
        Vertex3f(0, -1.375f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 2);
        Vertex3f(0.125f, -1.375f, 0.0625f);
        TexCoord2d(1.0 / 8 * 6.5, 1.0 / 8 * 5);
        Vertex3f(0.141f, -0.625f, 0.0785f);
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, 0.0785f);
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 2);
        Vertex3f(0, -1.391f, 0.0785f);
        TexCoord2d(1.0 / 8 * 6.5, 1.0 / 8 * 2);
        Vertex3f(0.141f, -1.391f, 0.0785f);
        //Back

        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 2);
        Vertex3f(0.125f, -1.375f, -0.0625f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 2);
        Vertex3f(0, -1.375f, -0.0625f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 5);
        Vertex3f(0, -0.625f, -0.0785f);
        TexCoord2d(1.0 / 8 * 6.5, 1.0 / 8 * 5);
        Vertex3f(0.141f, -0.625f, -0.0785f);
        TexCoord2d(1.0 / 8 * 6.5, 1.0 / 8 * 2);
        Vertex3f(0.141f, -1.391f, -0.0785f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 2);
        Vertex3f(0, -1.391f, -0.0785f);
        //Bottom
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 0.5);
        Vertex3f(0.125f, -1.375f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 0.5);
        Vertex3f(0, -1.375f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 0);
        Vertex3f(0, -1.375f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 0);
        Vertex3f(0.125f, -1.375f, -0.0625f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 0.5);
        Vertex3f(0.141f, -1.391f, 0.0785f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 0.5);
        Vertex3f(0, -1.391f, 0.0785f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 1);
        Vertex3f(0, -1.391f, -0.0785f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 1);
        Vertex3f(0.141f, -1.391f, -0.0785f);

        //===Left arm===
        //Left

        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 7);
        Vertex3f(-0.25f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 7);
        Vertex3f(-0.25f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 5);
        Vertex3f(-0.25f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 5);
        Vertex3f(-0.25f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 0.5, 1.0 / 8 * 2);
        Vertex3f(-0.266f, -0.125f, 0.0785f);
        TexCoord2d(1.0 / 8 * 0, 1.0 / 8 * 2);
        Vertex3f(-0.266f, -0.125f, -0.0785f);
        TexCoord2d(1.0 / 8 * 0, 1.0 / 8 * 0);
        Vertex3f(-0.266f, -0.625f, -0.0785f);
        TexCoord2d(1.0 / 8 * 0.5, 1.0 / 8 * 0);
        Vertex3f(-0.266f, -0.625f, 0.0785f);
        //Right

        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, -0.0625f);
        //Front
        TexCoord2d(1.0 / 8 * 4.5, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 7);
        Vertex3f(-0.25f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 5);
        Vertex3f(-0.25f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 4.5, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 2);
        Vertex3f(-0.125f, -0.125f, 0.0785f);
        TexCoord2d(1.0 / 8 * 0.5, 1.0 / 8 * 2);
        Vertex3f(-0.266f, -0.125f, 0.0785f);
        TexCoord2d(1.0 / 8 * 0.5, 1.0 / 8 * 0);
        Vertex3f(-0.266f, -0.625f, 0.0785f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 0);
        Vertex3f(-0.125f, -0.625f, 0.0785f);
        //Back
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 7);
        Vertex3f(-0.25f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 4.5, 1.0 / 8 * 7);
        Vertex3f(-0.125f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 4.5, 1.0 / 8 * 5);
        Vertex3f(-0.125f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 5);
        Vertex3f(-0.25f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 1.5, 1.0 / 8 * 2);
        Vertex3f(-0.266f, -0.125f, -0.0785f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 2);
        Vertex3f(-0.125f, -0.125f, -0.0785f);
        TexCoord2d(1.0 / 8 * 1, 1.0 / 8 * 0);
        Vertex3f(-0.125f, -0.625f, -0.0785f);
        TexCoord2d(1.0 / 8 * 1.5, 1.0 / 8 * 0);
        Vertex3f(-0.266f, -0.625f, -0.0785f);
        //Top
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 2);
        Vertex3f(-0.125f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 2);
        Vertex3f(-0.25f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 1.5);
        Vertex3f(-0.25f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 1.5);
        Vertex3f(-0.125f, -0.125f, 0.0625f);
        //Bottom
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 2);
        Vertex3f(-0.125f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 2);
        Vertex3f(-0.25f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 1.5);
        Vertex3f(-0.25f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 1.5);
        Vertex3f(-0.125f, -0.625f, -0.0625f);

        //===Right arm===
        //Left
        TexCoord2d(1.0 / 8 * 5.5, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 5, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 5.5, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, 0.0625f);
        //Right
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 7);
        Vertex3f(0.25f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 5.5, 1.0 / 8 * 7);
        Vertex3f(0.25f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 5.5, 1.0 / 8 * 5);
        Vertex3f(0.25f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 5);
        Vertex3f(0.25f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 2);
        Vertex3f(0.266f, -0.125f, -0.0785f);
        TexCoord2d(1.0 / 8 * 1.5, 1.0 / 8 * 2);
        Vertex3f(0.266f, -0.125f, 0.0785f);
        TexCoord2d(1.0 / 8 * 1.5, 1.0 / 8 * 0);
        Vertex3f(0.266f, -0.625f, 0.0785f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 0);
        Vertex3f(0.266f, -0.625f, -0.0785f);
        //Front
        TexCoord2d(1.0 / 8 * 6.5, 1.0 / 8 * 7);
        Vertex3f(0.25f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 6, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 6.5, 1.0 / 8 * 5);
        Vertex3f(0.25f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 2);
        Vertex3f(0.266f, -0.125f, 0.0785f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 2);
        Vertex3f(0.125f, -0.125f, 0.0785f);
        TexCoord2d(1.0 / 8 * 2, 1.0 / 8 * 0);
        Vertex3f(0.125f, -0.625f, 0.0785f);
        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 0);
        Vertex3f(0.266f, -0.625f, 0.0785f);
        //Back
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 7);
        Vertex3f(0.125f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 6.5, 1.0 / 8 * 7);
        Vertex3f(0.25f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 6.5, 1.0 / 8 * 5);
        Vertex3f(0.25f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 7, 1.0 / 8 * 5);
        Vertex3f(0.125f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 2);
        Vertex3f(0.125f, -0.125f, -0.0785f);
        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 2);
        Vertex3f(0.266f, -0.125f, -0.0785f);
        TexCoord2d(1.0 / 8 * 2.5, 1.0 / 8 * 0);
        Vertex3f(0.266f, -0.625f, -0.0785f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 0);
        Vertex3f(0.125f, -0.625f, -0.0785f);
        //Top
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 1.5);
        Vertex3f(0.25f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 1.5);
        Vertex3f(0.125f, -0.125f, -0.0625f);
        TexCoord2d(1.0 / 8 * 3, 1.0 / 8 * 1);
        Vertex3f(0.125f, -0.125f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 1);
        Vertex3f(0.25f, -0.125f, 0.0625f);
        //Bottom
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 1.5);
        Vertex3f(0.25f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 1.5);
        Vertex3f(0.125f, -0.625f, 0.0625f);
        TexCoord2d(1.0 / 8 * 3.5, 1.0 / 8 * 1);
        Vertex3f(0.125f, -0.625f, -0.0625f);
        TexCoord2d(1.0 / 8 * 4, 1.0 / 8 * 1);
        Vertex3f(0.25f, -0.625f, -0.0625f);

        Renderer::Flush(VBO, vtxs);
    }
}

void OnlinePlayer::buildRenderIfNeed()
{
    if (VBO == 0 || vtxs == 0)
    {
        auto iter = playerSkins.find(_skinID);
        if (iter != playerSkins.end())
        {
            VBO = iter->second.first;
            vtxs = iter->second.second;
        }
        else
        {
            VBO = 0;
            vtxs = 0;
            GenVAOVBO(_skinID); //生成玩家的VAO/VBO
            playerSkins[_skinID] = std::make_pair(VBO, vtxs);
        }
    }
}

void OnlinePlayer::render() const
{
    glDisable(GL_CULL_FACE);
    glNormal3f(0, 0, 0);
    glColor4f(1.0, 1.0, 1.0, 0.5);
    glBindTexture(GL_TEXTURE_2D, _skinID == 0 ? DefaultSkin : _skinID);
    Renderer::renderbuffer(VBO, vtxs, true, true);
    glEnable(GL_CULL_FACE);
}
