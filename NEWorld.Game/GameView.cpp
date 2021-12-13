#include "GameView.h"
#include <utility>
#include <Renderer/World/ShadowMaps.h>
#include "Universe/World/Blocks.h"
#include "Textures.h"
#include "Renderer/Renderer.h"
#include "TextRenderer.h"
#include "Player.h"
#include "Universe/World/World.h"
#include "Renderer/World/WorldRenderer.h"
#include "Particles.h"
#include "GUI.h"
#include "Menus.h"
#include "Command.h"
#include "Setup.h"
#include "Universe/Game.h"
#include "Renderer/BufferBuilder.h"

ThreadFunc updateThreadFunc(void *);

int getMouseScroll() { return mw; }

int getMouseButton() { return mb; }


class GameDView : public virtual GUI::Form, public Game {
private:

    int fps{}, fpsc{}, ups{}, upsc{};
    double fctime{}, uctime{};

    int selface{};
    float selt{};
    bool selce{};

    int getMouseScroll() { return mw; }

    int getMouseButton() { return mb; }

public:
    void GameThreadloop() {

        //Wait until start...
        MutexLock(Mutex);
        while (!updateThreadRun) {
            MutexUnlock(Mutex);
            SleepMs(1);
            MutexLock(Mutex);
        }
        MutexUnlock(Mutex);

        //Thread start
        MutexLock(Mutex);
        lastupdate = timer();

        while (updateThreadRun) {
            MutexUnlock(Mutex);
            std::this_thread::yield();
            MutexLock(Mutex);

            while (updateThreadPaused) {
                MutexUnlock(Mutex);
                std::this_thread::yield();
                MutexLock(Mutex);
                lastupdate = updateTimer = timer();
            }

            FirstUpdateThisFrame = true;
            updateTimer = timer();
            if (updateTimer - lastupdate >= 5.0) lastupdate = updateTimer;

            while ((updateTimer - lastupdate) >= 1.0 / 30.0 && upsc < 60) {
                lastupdate += 1.0 / 30.0;
                upsc++;
                updategame();
                FirstUpdateThisFrame = false;
            }

            if ((timer() - uctime) >= 1.0) {
                uctime = timer();
                ups = upsc;
                upsc = 0;
            }

        }
        MutexUnlock(Mutex);
    }


    static void debugText(std::string s, bool init) {
        static auto pos = 0;
        if (init) {
            pos = 0;
            return;
        }
        TextRenderer::renderASCIIString(0, 16 * pos, std::move(s));
        pos++;
    }

    void Grender() {
        //画场景
        const auto curtime = timer();
        double TimeDelta;
        auto renderedChunk = 0;

        //检测帧速率
        if (timer() - fctime >= 1.0) {
            fps = fpsc;
            fpsc = 0;
            fctime = timer();
        }
        fpsc++;

        lastframe = curtime;

        if (Player::Running) {
            if (FOVyExt < 9.8) {
                TimeDelta = curtime - SpeedupAnimTimer;
                FOVyExt = 10.0f - (10.0f - FOVyExt) * static_cast<float>(pow(0.8, TimeDelta * 30));
                SpeedupAnimTimer = curtime;
            } else FOVyExt = 10.0;
        } else {
            if (FOVyExt > 0.2) {
                TimeDelta = curtime - SpeedupAnimTimer;
                FOVyExt *= static_cast<float>(pow(0.8, TimeDelta * 30));
                SpeedupAnimTimer = curtime;
            } else FOVyExt = 0.0;
        }
        SpeedupAnimTimer = curtime;

        if (Player::OnGround) {
            //半蹲特效
            if (Player::jump < -0.005) {
                if (Player::jump <= -(Player::height - 0.5f))
                    Player::heightExt = -(Player::height - 0.5f);
                else
                    Player::heightExt = static_cast<float>(Player::jump);
                TouchdownAnimTimer = curtime;
            } else {
                if (Player::heightExt <= -0.005) {
                    Player::heightExt *= static_cast<float>(pow(0.8, (curtime - TouchdownAnimTimer) * 30));
                    TouchdownAnimTimer = curtime;
                }
            }
        }

        const auto xpos = Player::Pos.X - Player::xd + (curtime - lastupdate) * 30.0 * Player::xd;
        const auto ypos = Player::Pos.Y + Player::height + Player::heightExt - Player::yd +
                          (curtime - lastupdate) * 30.0 * Player::yd;
        const auto zpos = Player::Pos.Z - Player::zd + (curtime - lastupdate) * 30.0 * Player::zd;

        if (!bagOpened) {
            //转头！你治好了我多年的颈椎病！
            if (mx != mxl) Player::xlookspeed -= (mx - mxl) * mousemove;
            if (my != myl) Player::ylookspeed += (my - myl) * mousemove;
            if (glfwGetKey(MainWindow, GLFW_KEY_RIGHT) == 1)
                Player::xlookspeed -= mousemove * 16 * (curtime - lastframe) * 30.0;
            if (glfwGetKey(MainWindow, GLFW_KEY_LEFT) == 1)
                Player::xlookspeed += mousemove * 16 * (curtime - lastframe) * 30.0;
            if (glfwGetKey(MainWindow, GLFW_KEY_UP) == 1)
                Player::ylookspeed -= mousemove * 16 * (curtime - lastframe) * 30.0;
            if (glfwGetKey(MainWindow, GLFW_KEY_DOWN) == 1)
                Player::ylookspeed += mousemove * 16 * (curtime - lastframe) * 30.0;
            //限制角度，别把头转掉下来了 ←_←
            if (Player::lookupdown + Player::ylookspeed < -90.0) Player::ylookspeed = -90.0 - Player::lookupdown;
            if (Player::lookupdown + Player::ylookspeed > 90.0) Player::ylookspeed = 90.0 - Player::lookupdown;
        }

        Player::cxt = World::GetChunkPos(static_cast<int>(Player::Pos.X));
        Player::cyt = World::GetChunkPos(static_cast<int>(Player::Pos.Y));
        Player::czt = World::GetChunkPos(static_cast<int>(Player::Pos.Z));

        //更新区块VBO
        World::sortChunkBuildRenderList(RoundInt(Player::Pos.X), RoundInt(Player::Pos.Y), RoundInt(Player::Pos.Z));
        const auto brl =
                World::chunkBuildRenders > World::MaxChunkRenders ? World::MaxChunkRenders : World::chunkBuildRenders;
        for (auto i = 0; i < brl; i++) {
            const auto ci = World::chunkBuildRenderList[i][1];
            World::chunks[ci]->buildRender();
        }

        //删除已卸载区块的VBO
        if (!World::vbuffersShouldDelete.empty()) {
            glDeleteBuffersARB(World::vbuffersShouldDelete.size(), World::vbuffersShouldDelete.data());
            World::vbuffersShouldDelete.clear();
        }

        glFlush();

        const auto plookupdown = Player::lookupdown + Player::ylookspeed;
        const auto pheading = Player::heading + Player::xlookspeed;

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        //daylight = clamp((1.0 - cos((double)gametime / gameTimeMax * 2.0 * M_PI) * 2.0) / 2.0, 0.05, 1.0);
        //Renderer::sunlightXrot = 90 * daylight;
        if (Renderer::AdvancedRender) {
            //Build shadow map
            if (!DebugShadow) ShadowMaps::BuildShadowMap(xpos, ypos, zpos, curtime);
            else ShadowMaps::RenderShadowMap(xpos, ypos, zpos, curtime);
        }
        glClearColor(skycolorR, skycolorG, skycolorB, 1.0);
        if (!DebugShadow) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);

        Player::ViewFrustum.LoadIdentity();
        Player::ViewFrustum.SetPerspective(FOVyNormal + FOVyExt, static_cast<float>(windowwidth) / windowheight, 0.05f,
                                           viewdistance * 16.0f);
        Player::ViewFrustum.MultRotate(static_cast<float>(plookupdown), 1, 0, 0);
        Player::ViewFrustum.MultRotate(360.0f - static_cast<float>(pheading), 0, 1, 0);
        Player::ViewFrustum.update();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMultMatrixf(Player::ViewFrustum.getProjMatrix());
        glMatrixMode(GL_MODELVIEW);

        World::calcVisible(xpos, ypos, zpos, Player::ViewFrustum);
        renderedChunk = WorldRenderer::ListRenderChunks(Player::cxt, Player::cyt, Player::czt, viewdistance, curtime);

        MutexUnlock(Mutex);

        glBindTexture(GL_TEXTURE_2D, BlockTextures);

        // 渲染层1
        glLoadIdentity();
        glRotated(plookupdown, 1, 0, 0);
        glRotated(360.0 - pheading, 0, 1, 0);
        glDisable(GL_BLEND);
        Renderer::EnableShaders();
        if (!DebugShadow) WorldRenderer::RenderChunks(xpos, ypos, zpos, 0);
        Renderer::DisableShaders();
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
        glEnable(GL_BLEND);

        MutexLock(Mutex);

        if (seldes > 0.0) {
            glTranslated(selx - xpos, sely - ypos, selz - zpos);
            renderDestroy(seldes, 0, 0, 0);
            glTranslated(-selx + xpos, -sely + ypos, -selz + zpos);
        }
        glBindTexture(GL_TEXTURE_2D, BlockTextures);
        Particles::renderall(xpos, ypos, zpos);

        glDisable(GL_TEXTURE_2D);
        if (GUIrenderswitch && sel) {
            glTranslated(selx - xpos, sely - ypos, selz - zpos);
            drawBorder(0, 0, 0);
            glTranslated(-selx + xpos, -sely + ypos, -selz + zpos);
        }

        MutexUnlock(Mutex);

        // 渲染层2&3
        glLoadIdentity();
        glRotated(plookupdown, 1, 0, 0);
        glRotated(360.0 - pheading, 0, 1, 0);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_CULL_FACE);
        glBindTexture(GL_TEXTURE_2D, BlockTextures);
        Renderer::EnableShaders();
        if (!DebugShadow) WorldRenderer::RenderChunks(xpos, ypos, zpos, 1);
        glDisable(GL_CULL_FACE);
        if (!DebugShadow) WorldRenderer::RenderChunks(xpos, ypos, zpos, 2);
        Renderer::DisableShaders();

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

        glLoadIdentity();
        glRotated(plookupdown, 1, 0, 0);
        glRotated(360.0 - pheading, 0, 1, 0);
        glTranslated(-xpos, -ypos, -zpos);

        MutexLock(Mutex);

        if (DebugHitbox) {
            glDisable(GL_CULL_FACE);
            glDisable(GL_TEXTURE_2D);
            for (const auto &Hitboxe: Player::Hitboxes) {
                renderAABB(Hitboxe, GUI::FgR, GUI::FgG, GUI::FgB, 3, 0.002);
            }

            glLoadIdentity();
            glRotated(plookupdown, 1, 0, 0);
            glRotated(360.0 - pheading, 0, 1, 0);
            glTranslated(-Player::Pos.X, -Player::Pos.Y - Player::height - Player::heightExt, -Player::Pos.Z);

            renderAABB(Player::playerbox, 1.0f, 1.0f, 1.0f, 1);
            renderAABB(Hitbox::Expand(Player::playerbox, Player::xd, Player::yd, Player::zd), 1.0f, 1.0f, 1.0f,
                       1);
        }

        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);

        //Time_renderscene = timer() - Time_renderscene;
        //Time_renderGUI_ = timer();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, windowwidth, windowheight, 0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (World::GetBlock({RoundInt(xpos), RoundInt(ypos), RoundInt(zpos)}) == Blocks::WATER) {
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glBindTexture(GL_TEXTURE_2D, BlockTextures);
            const auto tcX = Textures::getTexcoordX(Blocks::WATER, 1);
            const auto tcY = Textures::getTexcoordY(Blocks::WATER, 1);
            glBegin(GL_QUADS);
            glTexCoord2d(tcX, tcY + 1 / 8.0);
            glVertex2i(0, 0);
            glTexCoord2d(tcX, tcY);
            glVertex2i(0, windowheight);
            glTexCoord2d(tcX + 1 / 8.0, tcY);
            glVertex2i(windowwidth, windowheight);
            glTexCoord2d(tcX + 1 / 8.0, tcY + 1 / 8.0);
            glVertex2i(windowwidth, 0);
            glEnd();
        }
        if (GUIrenderswitch) {
            drawGUI();
            drawBag();
        }

        glDisable(GL_TEXTURE_2D);
        if (curtime - screenshotAnimTimer <= 1.0 && !shouldGetScreenshot) {
            const auto col = 1.0f - static_cast<float>(curtime - screenshotAnimTimer);
            glColor4f(1.0f, 1.0f, 1.0f, col);
            glBegin(GL_QUADS);
            glVertex2i(0, 0);
            glVertex2i(0, windowheight);
            glVertex2i(windowwidth, windowheight);
            glVertex2i(windowwidth, 0);
            glEnd();
        }
        glEnable(GL_TEXTURE_2D);

        if (shouldGetScreenshot) {
            shouldGetScreenshot = false;
            screenshotAnimTimer = curtime;
            auto t = time(nullptr);
            char tmp[64];
            const auto timeinfo = localtime(&t);
            strftime(tmp, sizeof(tmp), "%Y年%m月%d日%H时%M分%S秒", timeinfo);
            delete timeinfo;
            std::stringstream ss;
            ss << "Screenshots/" << tmp << ".bmp";
            saveScreenshot(0, 0, windowwidth, windowheight, ss.str());
        }
        if (shouldGetThumbnail) {
            shouldGetThumbnail = false;
            createThumbnail();
        }

        //屏幕刷新，千万别删，后果自负！！！
        //====refresh====//
        MutexUnlock(Mutex);
    }

    void onRender() override {
        MutexLock(Mutex);
        //==refresh end==//
    }

    static void drawBorder(int x, int y, int z) {
        //绘制选择边框
        static auto eps = 0.002f; //实际上这个边框应该比方块大一些，否则很难看
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(1);
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_LINES);
        // Left Face
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        // Front Face
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        // Right Face
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        // Back Face
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glEnd();
        glDisable(GL_LINE_SMOOTH);
    }

    void drawGUI() {
        int windowuswidth = windowwidth / stretch, windowusheight = windowheight / stretch;
        glDepthFunc(GL_ALWAYS);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LINE_SMOOTH);
        auto seldes_100 = seldes / 100.0f;
        auto disti = static_cast<int>(seldes_100 * linedist);

        if (DebugMode) {

            if (selb != Blocks::ENV) {
                glLineWidth(1);
                glBegin(GL_LINES);
                glColor4f(GUI::FgR, GUI::FgG, GUI::FgB, 0.8f);
                UIVertex(windowuswidth / 2, windowusheight / 2);
                UIVertex(windowuswidth / 2 + 50, windowusheight / 2 + 50);
                UIVertex(windowuswidth / 2 + 50, windowusheight / 2 + 50);
                UIVertex(windowuswidth / 2 + 250, windowusheight / 2 + 50);
                glEnd();
                TextRenderer::setFontColor(1.0f, 1.0f, 1.0f, 0.8f);
                glEnable(GL_TEXTURE_2D);
                glDisable(GL_CULL_FACE);
                std::stringstream ss;
                ss << BlockInfo(selb).getBlockName() << " (ID " << static_cast<int>(selb) << ")";
                TextRenderer::renderString(windowuswidth / 2 + 50, windowusheight / 2 + 50 - 16, ss.str());
                glDisable(GL_TEXTURE_2D);
                glEnable(GL_CULL_FACE);
                glColor4f(0.0f, 0.0f, 0.0f, 0.9f);
            } else {
                glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
            }

            glLineWidth(2);

            glBegin(GL_LINES);
            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 - linedist + disti);
            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 - linedist + linelength + disti);
            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 - linedist + disti);
            UIVertex(windowuswidth / 2 - linedist + linelength + disti, windowusheight / 2 - linedist + disti);

            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 - linedist + disti);
            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 - linedist + linelength + disti);
            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 - linedist + disti);
            UIVertex(windowuswidth / 2 + linedist - linelength - disti, windowusheight / 2 - linedist + disti);

            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 + linedist - disti);
            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 + linedist - linelength - disti);
            UIVertex(windowuswidth / 2 - linedist + disti, windowusheight / 2 + linedist - disti);
            UIVertex(windowuswidth / 2 - linedist + linelength + disti, windowusheight / 2 + linedist - disti);

            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 + linedist - disti);
            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 + linedist - linelength - disti);
            UIVertex(windowuswidth / 2 + linedist - disti, windowusheight / 2 + linedist - disti);
            UIVertex(windowuswidth / 2 + linedist - linelength - disti, windowusheight / 2 + linedist - disti);

            glEnd();

        }

        glLineWidth(4 * stretch);
        glBegin(GL_LINES);
        glColor4f(0.0, 0.0, 0.0, 1.0);
        UIVertex(windowuswidth / 2 - 16, windowusheight / 2);
        UIVertex(windowuswidth / 2 + 16, windowusheight / 2);
        UIVertex(windowuswidth / 2, windowusheight / 2 - 16);
        UIVertex(windowuswidth / 2, windowusheight / 2 + 16);
        glEnd();
        glLineWidth(2 * stretch);
        glBegin(GL_LINES);
        glColor4f(1.0, 1.0, 1.0, 1.0);
        UIVertex(windowuswidth / 2 - 15, windowusheight / 2);
        UIVertex(windowuswidth / 2 + 15, windowusheight / 2);
        UIVertex(windowuswidth / 2, windowusheight / 2 - 15);
        UIVertex(windowuswidth / 2, windowusheight / 2 + 15);
        glEnd();

        if (seldes > 0.0) {

            glBegin(GL_LINES);
            glColor4f(0.5, 0.5, 0.5, 1.0);
            glVertex2i(windowwidth / 2 - 15, windowheight / 2);
            glVertex2i(windowwidth / 2 - 15 + static_cast<int>(seldes_100 * 15), windowheight / 2);
            glVertex2i(windowwidth / 2 + 15, windowheight / 2);
            glVertex2i(windowwidth / 2 + 15 - static_cast<int>(seldes_100 * 15), windowheight / 2);
            glVertex2i(windowwidth / 2, windowheight / 2 - 15);
            glVertex2i(windowwidth / 2, windowheight / 2 - 15 + static_cast<int>(seldes_100 * 15));
            glVertex2i(windowwidth / 2, windowheight / 2 + 15);
            glVertex2i(windowwidth / 2, windowheight / 2 + 15 - static_cast<int>(seldes_100 * 15));
            glEnd();

        }

        glDisable(GL_CULL_FACE);

        RenderHealthBar();

        TextRenderer::setFontColor(1.0f, 1.0f, 1.0f, 0.9f);
        if (chatmode) {
            glColor4f(GUI::FgR, GUI::FgG, GUI::FgB, GUI::FgA);
            glDisable(GL_TEXTURE_2D);
            glBegin(GL_QUADS);
            glVertex2i(1, windowheight - 33);
            glVertex2i(windowwidth - 1, windowheight - 33);
            glVertex2i(windowwidth - 1, windowheight - 51);
            glVertex2i(1, windowheight - 51);
            glEnd();
            glEnable(GL_TEXTURE_2D);
            TextRenderer::renderString(0, windowheight - 50, chatword);
        }
        auto posy = 0;
        int size = chatMessages.size();
        if (size != 0) {
            for (auto i = size - 1; i >= (size - 10 > 0 ? size - 10 : 0); --i) {
                TextRenderer::renderString(0, windowheight - 80 - 18 * posy++, chatMessages[i]);
            }
        }

        //if (DebugShadow) ShadowMaps::DrawShadowMap(windowwidth / 2, windowheight / 2, windowwidth, windowheight);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextRenderer::Font);

        RenderDebugText();
        glFlush();
    }

    void RenderHealthBar() const {
        if (Player::gamemode == Player::Survival) {
            glColor4d(0.8, 0.0, 0.0, 0.3);
            glBegin(GL_QUADS);
            UIVertex(10, 10);
            UIVertex(200, 10);
            UIVertex(200, 30);
            UIVertex(10, 30);
            glEnd();

            auto healthPercent = static_cast<double>(Player::health) / Player::healthMax;
            glColor4d(1.0, 0.0, 0.0, 0.5);
            glBegin(GL_QUADS);
            UIVertex(20, 15);
            UIVertex(static_cast<int>(20 + healthPercent * 170), 15);
            UIVertex(static_cast<int>(20 + healthPercent * 170), 25);
            UIVertex(20, 25);
            glEnd();
        }
    }

    void RenderDebugText() const {
        if (DebugMode) {
            std::stringstream ss;
            //ss << std::fixed << std::setprecision(4);
            ss << "NEWorld v" << VERSION << " [OpenGL " << GLVersionMajor << "." << GLVersionMinor << "|"
               << GLVersionRev << "]";
            debugText(ss.str(), false);
            ss.str("");
            ss << "Fps:" << fps << "|" << "Ups:" << ups;
            debugText(ss.str(), false);
            ss.str("");

            ss << "Debug Mode:" << boolstr(DebugMode);
            debugText(ss.str(), false);
            ss.str("");
            if (Renderer::AdvancedRender) {
                ss << "Shadow View:" << boolstr(DebugShadow);
                debugText(ss.str(), false);
                ss.str("");
            }
            ss << "X:" << Player::Pos.X << " Y:" << Player::Pos.Y << " Z:" << Player::Pos.Z;
            debugText(ss.str(), false);
            ss.str("");
            ss << "Direction:" << Player::heading << " Head:" << Player::lookupdown << "Jump speed:" << Player::jump;
            debugText(ss.str(), false);
            ss.str("");

            ss << "Stats:";
            if (Player::Flying) ss << " Flying";
            if (Player::OnGround) ss << " On_ground";
            if (Player::NearWall) ss << " Near_wall";
            if (Player::inWater) ss << " In_water";
            if (Player::CrossWall) ss << " Cross_Wall";
            if (Player::Glide) ss << " Gliding_enabled";
            if (Player::glidingNow) ss << "Gliding";
            debugText(ss.str(), false);
            ss.str("");

            ss << "Energy:" << Player::glidingEnergy;
            debugText(ss.str(), false);
            ss.str("");
            ss << "Speed:" << Player::glidingSpeed;
            debugText(ss.str(), false);
            ss.str("");

            auto h = gametime / (30 * 60);
            auto m = gametime % (30 * 60) / 30;
            auto s = gametime % 30 * 2;
            ss << "Time: "
               << (h < 10 ? "0" : "") << h << ":"
               << (m < 10 ? "0" : "") << m << ":"
               << (s < 10 ? "0" : "") << s
               << " (" << gametime << "/" << gameTimeMax << ")";
            debugText(ss.str(), false);
            ss.str("");

            ss << "load:" << World::chunks.size() << " unload:" << World::unloadedChunks
               << " render:" << WorldRenderer::RenderChunkList.size() << " update:" << World::updatedChunks;
            debugText(ss.str(), false);
            ss.str("");

#ifdef NEWORLD_DEBUG_PERFORMANCE_REC
            ss << c_getChunkPtrFromCPA << " CPA requests";
            debugText(ss.str()); ss.str("");
            ss << c_getChunkPtrFromSearch << " search requests";
            debugText(ss.str()); ss.str("");
            ss << c_getHeightFromHMap << " heightmap requests";
            debugText(ss.str()); ss.str("");
            ss << c_getHeightFromWorldGen << " worldgen requests";
            debugText(ss.str()); ss.str("");
#endif
            debugText("", true);
        } else {
            TextRenderer::setFontColor(1.0f, 1.0f, 1.0f, 0.9f);
            std::stringstream ss;
            ss << "v" << VERSION << "  Fps:" << fps;
            TextRenderer::renderString(10, 30, ss.str());
        }
    }

    static void renderDestroy(float level, int x, int y, int z) {
        static auto eps = 0.002f;
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        if (level < 100.0) glBindTexture(GL_TEXTURE_2D, DestroyImage[int(level / 10) + 1]);
        else glBindTexture(GL_TEXTURE_2D, DestroyImage[10]);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);

        glTexCoord2f(1.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, (0.5f + eps) + y, -(0.5f + eps) + z);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, -(0.5f + eps) + z);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f((0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-(0.5f + eps) + x, -(0.5f + eps) + y, (0.5f + eps) + z);
        glEnd();
    }

    static void drawBagRow(int row, int itemid, int xbase, int ybase, int spac, float alpha) {
        //画出背包的一行
        for (auto i = 0; i < 10; i++) {
            if (i == itemid) glBindTexture(GL_TEXTURE_2D, tex_select);
            else glBindTexture(GL_TEXTURE_2D, tex_unselect);
            glColor4f(1.0f, 1.0f, 1.0f, alpha);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0, 1.0);
            UIVertex(xbase + i * (32 + spac), ybase);
            glTexCoord2f(0.0, 0.0);
            UIVertex(xbase + i * (32 + spac) + 32, ybase);
            glTexCoord2f(1.0, 0.0);
            UIVertex(xbase + i * (32 + spac) + 32, ybase + 32);
            glTexCoord2f(1.0, 1.0);
            UIVertex(xbase + i * (32 + spac), ybase + 32);
            glEnd();
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            if (Player::inventory[row][i] != Blocks::ENV) {
                glBindTexture(GL_TEXTURE_2D, BlockTextures);
                const auto tcX = Textures::getTexcoordX(Player::inventory[row][i], 1);
                const auto tcY = Textures::getTexcoordY(Player::inventory[row][i], 1);
                glBegin(GL_QUADS);
                glTexCoord2d(tcX, tcY + 1 / 8.0);
                UIVertex(xbase + i * (32 + spac) + 2, ybase + 2);
                glTexCoord2d(tcX + 1 / 8.0, tcY + 1 / 8.0);
                UIVertex(xbase + i * (32 + spac) + 30, ybase + 2);
                glTexCoord2d(tcX + 1 / 8.0, tcY);
                UIVertex(xbase + i * (32 + spac) + 30, ybase + 30);
                glTexCoord2d(tcX, tcY);
                UIVertex(xbase + i * (32 + spac) + 2, ybase + 30);
                glEnd();
                std::stringstream ss;
                ss << static_cast<int>(Player::inventoryAmount[row][i]);
                TextRenderer::renderString(xbase + i * (32 + spac), ybase, ss.str());
            }
        }
    }

    void drawBag() {
        //背包界面与更新
        static int si, sj, sf;
        auto csi = -1, csj = -1;
        const int leftp = (windowwidth / stretch - 392) / 2;
        const int upp = windowheight / stretch - 152 - 16;
        static int mousew, mouseb, mousebl;
        static Block indexselected = Blocks::ENV;
        static short Amountselected = 0;
        const auto curtime = timer();
        const auto TimeDelta = curtime - bagAnimTimer;
        const auto bagAnim = static_cast<float>(1.0 - pow(0.9, TimeDelta * 60.0) +
                                                pow(0.9, bagAnimDuration * 60.0) / bagAnimDuration * TimeDelta);

        if (bagOpened) {

            glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mousew = mw;
            mouseb = mb;
            glDepthFunc(GL_ALWAYS);
            glDisable(GL_CULL_FACE);
            glDisable(GL_TEXTURE_2D);

            if (curtime - bagAnimTimer > bagAnimDuration) glColor4f(0.2f, 0.2f, 0.2f, 0.6f);
            else glColor4f(0.2f, 0.2f, 0.2f, 0.6f * bagAnim);
            glBegin(GL_QUADS);
            UIVertex(0, 0);
            UIVertex(static_cast<int>(windowwidth / stretch), 0);
            UIVertex(static_cast<int>(windowwidth / stretch), static_cast<int>(windowheight / stretch));
            UIVertex(0, static_cast<int>(windowheight / stretch));
            glEnd();

            glEnable(GL_TEXTURE_2D);
            glDisable(GL_CULL_FACE);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            sf = 0;

            if (curtime - bagAnimTimer > bagAnimDuration) {
                for (auto i = 0; i < 4; i++) {
                    for (auto j = 0; j < 10; j++) {
                        if (mx >= j * 40 + leftp && mx <= j * 40 + 32 + leftp &&
                            my >= i * 40 + upp && my <= i * 40 + 32 + upp) {
                            csi = si = i;
                            csj = sj = j;
                            sf = 1;
                            if (mousebl == 0 && mouseb == 1 && indexselected == Player::inventory[i][j]) {
                                if (Player::inventoryAmount[i][j] + Amountselected <= 255) {
                                    Player::inventoryAmount[i][j] += Amountselected;
                                    Amountselected = 0;
                                } else {
                                    Amountselected = Player::inventoryAmount[i][j] + Amountselected - 255;
                                    Player::inventoryAmount[i][j] = 255;
                                }
                            }
                            if (mousebl == 0 && mouseb == 1 && indexselected != Player::inventory[i][j]) {
                                std::swap(Amountselected, Player::inventoryAmount[i][j]);
                                std::swap(indexselected, Player::inventory[i][j]);
                            }
                            if (mousebl == 0 && mouseb == 2 && indexselected == Player::inventory[i][j] &&
                                Player::inventoryAmount[i][j] < 255) {
                                Amountselected--;
                                Player::inventoryAmount[i][j]++;
                            }
                            if (mousebl == 0 && mouseb == 2 && Player::inventory[i][j] == Blocks::ENV) {
                                Amountselected--;
                                Player::inventoryAmount[i][j] = 1;
                                Player::inventory[i][j] = indexselected;
                            }

                            if (Amountselected == 0) indexselected = Blocks::ENV;
                            if (indexselected == Blocks::ENV) Amountselected = 0;
                            if (Player::inventoryAmount[i][j] == 0) Player::inventory[i][j] = Blocks::ENV;
                            if (Player::inventory[i][j] == Blocks::ENV) Player::inventoryAmount[i][j] = 0;
                        }
                    }
                    drawBagRow(i, (csi == i ? csj : -1), (windowwidth / stretch - 392) / 2,
                               windowheight / stretch - 152 - 16 + i * 40, 8, 1.0f);
                }
            }
            if (indexselected != Blocks::ENV) {
                glBindTexture(GL_TEXTURE_2D, BlockTextures);
                const auto tcX = Textures::getTexcoordX(indexselected, 1);
                const auto tcY = Textures::getTexcoordY(indexselected, 1);
                glBegin(GL_QUADS);
                glTexCoord2d(tcX, tcY + 1 / 8.0);
                UIVertex(mx - 16, my - 16);
                glTexCoord2d(tcX + 1 / 8.0, tcY + 1 / 8.0);
                UIVertex(mx + 16, my - 16);
                glTexCoord2d(tcX + 1 / 8.0, tcY);
                UIVertex(mx + 16, my + 16);
                glTexCoord2d(tcX, tcY);
                UIVertex(mx - 16, my + 16);
                glEnd();
                std::stringstream ss;
                ss << Amountselected;
                TextRenderer::renderString(static_cast<int>(mx) - 16, static_cast<int>(my) - 16, ss.str());
            }
            if (Player::inventory[si][sj] != 0 && sf == 1) {
                glColor4f(1.0, 1.0, 0.0, 1.0);
                TextRenderer::renderString(static_cast<int>(mx), static_cast<int>(my) - 16,
                                           BlockInfo(Player::inventory[si][sj]).getBlockName());
            }

            auto xbase = 0, ybase = 0, spac = 0;
            const auto alpha = 0.5f + 0.5f * bagAnim;
            if (curtime - bagAnimTimer <= bagAnimDuration) {
                xbase = static_cast<int>(round(((windowwidth / stretch - 392) / 2) * bagAnim));
                ybase = static_cast<int>(round(
                        (windowheight / stretch - 152 - 16 + 120 - (windowheight / stretch - 32)) * bagAnim +
                        (windowheight / stretch - 32)));
                spac = static_cast<int>(round(8 * bagAnim));
                drawBagRow(3, -1, xbase, ybase, spac, alpha);
                xbase = static_cast<int>(round(
                        ((windowwidth / stretch - 392) / 2 - windowwidth / stretch) * bagAnim + windowwidth / stretch));
                ybase = static_cast<int>(round(
                        (windowheight / stretch - 152 - 16 - (windowheight / stretch - 32)) * bagAnim +
                        (windowheight / stretch - 32)));
                for (auto i = 0; i < 3; i++) {
                    glColor4f(1.0f, 1.0f, 1.0f, bagAnim);
                    drawBagRow(i, -1, xbase, ybase + i * 40, spac, alpha);
                }
            }

            glEnable(GL_TEXTURE_2D);
            glDisable(GL_CULL_FACE);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

            mousebl = mouseb;
        } else {

            glEnable(GL_TEXTURE_2D);
            glDisable(GL_CULL_FACE);
            if (curtime - bagAnimTimer <= bagAnimDuration) {
                glDisable(GL_TEXTURE_2D);
                glColor4f(0.2f, 0.2f, 0.2f, 0.6f - 0.6f * bagAnim);
                glBegin(GL_QUADS);
                glVertex2i(0, 0);
                glVertex2i(windowwidth, 0);
                glVertex2i(windowwidth, windowheight);
                glVertex2i(0, windowheight);
                glEnd();
                glEnable(GL_TEXTURE_2D);
                auto xbase = 0, ybase = 0, spac = 0;
                const auto alpha = 1.0f - 0.5f * bagAnim;
                xbase = static_cast<int>(round(
                        ((windowwidth / stretch - 392) / 2) - ((windowwidth / stretch - 392) / 2) * bagAnim));
                ybase = static_cast<int>(round(
                        (windowheight / stretch - 152 - 16 + 120 - (windowheight / stretch - 32)) -
                        (windowheight / stretch - 152 - 16 + 120 - (windowheight - 32)) * bagAnim +
                        (windowheight / stretch - 32)));
                spac = static_cast<int>(round(8 - 8 * bagAnim));
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                drawBagRow(3, Player::indexInHand, xbase, ybase, spac, alpha);
                xbase = static_cast<int>(round(((windowwidth / stretch - 392) / 2 - windowwidth / stretch) -
                                               ((windowwidth / stretch - 392) / 2 - windowwidth / stretch) * bagAnim +
                                               windowwidth / stretch));
                ybase = static_cast<int>(round((windowheight / stretch - 152 - 16 - (windowheight / stretch - 32)) -
                                               (windowheight / stretch - 152 - 16 - (windowheight / stretch - 32)) *
                                               bagAnim +
                                               (windowheight / stretch - 32)));
                for (auto i = 0; i < 3; i++) {
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f - bagAnim);
                    drawBagRow(i, -1, xbase, ybase + i * 40, spac, alpha);
                }
            } else drawBagRow(3, Player::indexInHand, 0, windowheight / stretch - 32, 0, 0.5f);
        }
        glFlush();
    }

    static void saveScreenshot(int x, int y, int w, int h, std::string filename) {
        Textures::TEXTURE_RGB scrBuffer;
        auto bufw = w, bufh = h;
        while (bufw % 4 != 0) { bufw += 1; }
        while (bufh % 4 != 0) { bufh += 1; }
        scrBuffer.sizeX = bufw;
        scrBuffer.sizeY = bufh;
        scrBuffer.buffer = std::unique_ptr<ubyte[]>(new ubyte[bufw * bufh * 3]);
        glReadPixels(x, y, bufw, bufh, GL_RGB, GL_UNSIGNED_BYTE, scrBuffer.buffer.get());
        Textures::SaveRGBImage(std::move(filename), scrBuffer);
    }

    void createThumbnail() {
        std::stringstream ss;
        ss << "Worlds/" << World::worldname << "/Thumbnail.bmp";
        saveScreenshot(0, 0, windowwidth, windowheight, ss.str());
    }


    void onLoad() override {
        Background = nullptr;

        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_TEXTURE_2D);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();

        Mutex = MutexCreate();
        MutexLock(Mutex);
        updateThread = ThreadCreate(&updateThreadFunc, nullptr);
        if (multiplayer) {
            fastSrand(static_cast<unsigned int>(time(nullptr)));
            Player::name = "";
            Player::onlineID = rand();
        }
        //初始化游戏状态
        printf("[Console][Game]Init player...\n");
        if (loadGame()) Player::init(Player::Pos);
        else Player::spawn();
        printf("[Console][Game]Init world...\n");
        World::Init();
        registerCommands();
        printf("[Console][Game]Loading Mods...\n");

        GUIrenderswitch = true;
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        setupNormalFog();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();
        printf("[Console][Game]Game start!\n");

        //这才是游戏开始!
        glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mxl = mx;
        myl = my;
        printf("[Console][Game]Main loop started\n");
        updateThreadRun = true;
        fctime = uctime = lastupdate = timer();
    }

    void onUpdate() override {

        MutexUnlock(Mutex);
        MutexLock(Mutex);

        if ((timer() - uctime) >= 1.0) {
            uctime = timer();
            ups = upsc;
            upsc = 0;
        }

        Grender();

        if (glfwGetKey(MainWindow, GLFW_KEY_ESCAPE) == 1) {
            updateThreadPaused = true;
            createThumbnail();
            GUI::clearTransition();
            Menus::gamemenu();
        }
    }

    void onLeave() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        TextRenderer::setFontColor(1.0, 1.0, 1.0, 1.0);
        TextRenderer::renderString(0, 0, "Saving world...");
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();
        printf("[Console][Game]Terminate threads\n");
        updateThreadRun = false;
        MutexUnlock(Mutex);
        ThreadWait(updateThread);
        ThreadDestroy(updateThread);
        MutexDestroy(Mutex);
        saveGame();
        World::destroyAllChunks();
        if (!World::vbuffersShouldDelete.empty()) {
            glDeleteBuffersARB(World::vbuffersShouldDelete.size(), World::vbuffersShouldDelete.data());
            World::vbuffersShouldDelete.clear();
        }
        commands.clear();
        chatMessages.clear();
        GUI::BackToMain();
    }


    void renderAABB(const Hitbox::AABB &box, float colR, float colG, float colB, int mode, double EPS = 0.0) {
        //Debug only!
        //碰撞箱渲染出来很瞎狗眼的QAQ而且又没用QAQ

        glLineWidth(2.0);
        glEnable(GL_LINE_SMOOTH);
        glColor4f(colR, colG, colB, 1.0);

        if (mode == 1 || mode == 3) {
            glBegin(GL_LINE_LOOP);
            glVertex3d(box.xmin, box.ymin, box.zmin);
            glVertex3d(box.xmax, box.ymin, box.zmin);
            glVertex3d(box.xmax, box.ymax, box.zmin);
            glVertex3d(box.xmin, box.ymax, box.zmin);
            glEnd();
            glBegin(GL_LINE_LOOP);
            glVertex3d(box.xmin, box.ymin, box.zmax);
            glVertex3d(box.xmax, box.ymin, box.zmax);
            glVertex3d(box.xmax, box.ymax, box.zmax);
            glVertex3d(box.xmin, box.ymax, box.zmax);
            glEnd();
            glBegin(GL_LINE_LOOP);
            glVertex3d(box.xmin, box.ymin, box.zmin);
            glVertex3d(box.xmax, box.ymin, box.zmin);
            glVertex3d(box.xmax, box.ymin, box.zmax);
            glVertex3d(box.xmin, box.ymin, box.zmax);
            glEnd();
            glBegin(GL_LINE_LOOP);
            glVertex3d(box.xmin, box.ymax, box.zmin);
            glVertex3d(box.xmax, box.ymax, box.zmin);
            glVertex3d(box.xmax, box.ymax, box.zmax);
            glVertex3d(box.xmin, box.ymax, box.zmax);
            glEnd();
            glBegin(GL_LINE_LOOP);
            glVertex3d(box.xmin, box.ymin, box.zmin);
            glVertex3d(box.xmin, box.ymin, box.zmax);
            glVertex3d(box.xmin, box.ymax, box.zmax);
            glVertex3d(box.xmin, box.ymax, box.zmin);
            glEnd();
            glBegin(GL_LINE_LOOP);
            glVertex3d(box.xmax, box.ymin, box.zmin);
            glVertex3d(box.xmax, box.ymin, box.zmax);
            glVertex3d(box.xmax, box.ymax, box.zmax);
            glVertex3d(box.xmax, box.ymax, box.zmin);
            glEnd();
        }

        glColor4f(colR, colG, colB, 0.5);

        if (mode == 2 || mode == 3) {
            glBegin(GL_QUADS);
            glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmin - EPS);
            glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmin - EPS);
            glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmin - EPS);
            glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmin - EPS);
            glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmax + EPS);
            glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmax + EPS);
            glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmax + EPS);
            glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmax + EPS);
            glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmin - EPS);
            glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmin - EPS);
            glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmax + EPS);
            glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmax + EPS);
            glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmin - EPS);
            glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmin - EPS);
            glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmax + EPS);
            glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmax + EPS);
            glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmin - EPS);
            glVertex3d(box.xmin - EPS, box.ymin - EPS, box.zmax + EPS);
            glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmax + EPS);
            glVertex3d(box.xmin - EPS, box.ymax + EPS, box.zmin - EPS);
            glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmin - EPS);
            glVertex3d(box.xmax + EPS, box.ymin - EPS, box.zmax + EPS);
            glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmax + EPS);
            glVertex3d(box.xmax + EPS, box.ymax + EPS, box.zmin - EPS);
            glEnd();
        }
    }
};

GameDView *Game = nullptr;

void GameView() { GUI::PushPage(Game = (new GameDView)); }

ThreadFunc updateThreadFunc(void *) {
    Game->GameThreadloop();
    return 0;
}
