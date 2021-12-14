#include "GameView.h"
#include <utility>
#include <Renderer/World/ShadowMaps.h>
#include "Universe/World/Blocks.h"
#include "Textures.h"
#include "Renderer/Renderer.h"
#include "Player.h"
#include "Universe/World/World.h"
#include "Renderer/World/WorldRenderer.h"
#include "Particles.h"
#include "Hitbox.h"
#include "GUI/GUI.h"
#include "Menus.h"
#include "Command.h"
#include "Setup.h"
#include "Universe/Game.h"
#include "Common/Logger.h"
#include "NsApp/NotifyPropertyChangedBase.h"
#include "NsGui/Button.h"
#include "NsGui/Image.h"
#include "NsDrawing/Int32Rect.h"
#include "NsDrawing/Thickness.h"
#include "NsGui/CroppedBitmap.h"
#include "NsRender/GLFactory.h"
#include "NsGui/TextureSource.h"
#include <NsRender/Texture.h>
#include "NsGui/TextBlock.h"
#include "NsGui/Label.h"
#include "GUI/Menus/Menus.h"
#include "Renderer/BufferBuilder.h"

namespace NoesisApp {
    class Window;
}

ThreadFunc updateThreadFunc(void *);

class GameView;
// pretty hacky. try to remove later.
GameView* currentGame = nullptr;

int getMouseScroll() { return mw; }

int getMouseButton() { return mb; }


class GameViewViewModel : public NoesisApp::NotifyPropertyChangedBase {
public:
    const char* getDebugInfo() const {
        return mDebugInfo.c_str();
    }

    void setDebugInfo(std::string debugInfo) {
        if (debugInfo != mDebugInfo) {
            mDebugInfo = std::move(debugInfo);
            OnPropertyChanged("DebugInfo");
        }
    }
    bool getGamePaused() const {
        return mGamePaused;
    }

    void setGamePaused(bool gamePaused) {
        if (gamePaused != mGamePaused) {
            mGamePaused = gamePaused;
            OnPropertyChanged("GamePaused");
        }
    }
private:
    std::string mDebugInfo;
    bool mGamePaused = false;

    NS_IMPLEMENT_INLINE_REFLECTION(GameViewViewModel, NotifyPropertyChangedBase) {
        NsProp("DebugInfo", &GameViewViewModel::getDebugInfo);
        NsProp("GamePaused", &GameViewViewModel::getGamePaused);
    }
};

class GameView : public virtual GUI::Scene, public Game {
private:
    GUI::FpsCounter mUpsCounter;
    Noesis::Image* mHotBar[10];
    Noesis::TextBlock* mHotBarCnt[10];

    int selface{};
    float selt{};
    bool selce{};
    Noesis::Ptr<GameViewViewModel> mViewModel;

    int getMouseScroll() { return mw; }

    int getMouseButton() { return mb; }

public:
    GameView() : Scene("InGame.xaml", false), mViewModel(Noesis::MakePtr<GameViewViewModel>()) {}
    
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

            while (updateTimer - lastupdate >= 1.0 / 30.0 && mUpsCounter.getFPS() < 60) {
                lastupdate += 1.0 / 30.0;
                mUpsCounter.frame();
                updategame();
                FirstUpdateThisFrame = false;
            }

            mUpsCounter.check();
        }
        MutexUnlock(Mutex);
    }

    void Grender() {
        //画场景
        const auto curtime = timer();
        double TimeDelta;
        auto renderedChunk = 0;

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

        if(!mViewModel->getGamePaused()) {
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
        glEnable(GL_BLEND);

        MutexLock(Mutex);

        if (mBlockDestructionProgress > 0.0) {
            glTranslated(mCurrentSelectedBlockPos.X - xpos, mCurrentSelectedBlockPos.Y - ypos, mCurrentSelectedBlockPos.Z - zpos);
            renderDestroy(mBlockDestructionProgress, 0, 0, 0);
            glTranslated(-mCurrentSelectedBlockPos.X + xpos, -mCurrentSelectedBlockPos.Y + ypos, -mCurrentSelectedBlockPos.Z + zpos);
        }
        glBindTexture(GL_TEXTURE_2D, BlockTextures);
        Particles::renderall(xpos, ypos, zpos);

        glDisable(GL_TEXTURE_2D);
        if (mShouldRenderGUI && mIsSelectingBlock) {
            glTranslated(mCurrentSelectedBlockPos.X - xpos, mCurrentSelectedBlockPos.Y - ypos, mCurrentSelectedBlockPos.Z - zpos);
            drawBorder(0, 0, 0);
            glTranslated(-mCurrentSelectedBlockPos.X + xpos, -mCurrentSelectedBlockPos.Y + ypos, -mCurrentSelectedBlockPos.Z + zpos);
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

        glLoadIdentity();
        glRotated(plookupdown, 1, 0, 0);
        glRotated(360.0 - pheading, 0, 1, 0);
        glTranslated(-xpos, -ypos, -zpos);

        MutexLock(Mutex);

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
        mxl = mx;
        myl = my;
        //屏幕刷新，千万别删，后果自负！！！
        //====refresh====//
    }

    void onRender() override {
        MutexLock(Mutex);
        Grender();
        MutexUnlock(Mutex);
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
    
    void debugInfo() const {
        std::stringstream ss;

        if (DebugMode) {
            ss << "NEWorld v" << VERSION << " [OpenGL " << GLVersionMajor << "." << GLVersionMinor << " "
                << GLVersionRev << "]" << std::endl
                << "Fps:" << mFPS.getFPS() << " Ups:" << mUpsCounter.getFPS() << std::endl
                << "Debug Mode:" << boolstr(DebugMode) << std::endl;
            if (Renderer::AdvancedRender) {
                ss << "Shadow View:" << boolstr(DebugShadow) << std::endl;
            }
            ss << "X: " << Player::Pos.X << " Y: " << Player::Pos.Y << " Z: " << Player::Pos.Z << std::endl
                << "Direction:" << Player::heading << " Head:" << Player::lookupdown << std::endl
                << "Jump speed:" << Player::jump << std::endl
                << "Stats:";
            if (Player::Flying) ss << " Flying";
            if (Player::OnGround) ss << " On_ground";
            if (Player::NearWall) ss << " Near_wall";
            if (Player::inWater) ss << " In_water";
            if (Player::CrossWall) ss << " Cross_Wall";
            if (Player::Glide) ss << " Gliding_enabled";
            if (Player::glidingNow) ss << "Gliding";
            ss << std::endl;
            ss << "Energy:" << Player::glidingEnergy << std::endl;
            ss << "Speed:" << Player::glidingSpeed << std::endl;
            auto h = gametime / (30 * 60);
            auto m = gametime % (30 * 60) / 30;
            auto s = gametime % 30 * 2;
            ss << "Time: "
                << (h < 10 ? "0" : "") << h << ":"
                << (m < 10 ? "0" : "") << m << ":"
                << (s < 10 ? "0" : "") << s
                << " (" << gametime << "/" << gameTimeMax << ")" << std::endl;
            ss  << "load:" << World::chunks.size() << " unload:" << World::unloadedChunks
                << " render:" << WorldRenderer::RenderChunkList.size() << " update:" << World::updatedChunks;

#ifdef NEWORLD_DEBUG_PERFORMANCE_REC
            ss << c_getChunkPtrFromCPA << " CPA requests" << std::endl;
            ss << c_getChunkPtrFromSearch << " search requests" << std::endl;
            ss << c_getHeightFromHMap << " heightmap requests" << std::endl;
            ss << c_getHeightFromWorldGen << " worldgen requests" << std::endl;
#endif
        }
        else {
            ss << "v" << VERSION << "  Fps:" << mFPS.getFPS();
        }
        mViewModel->setDebugInfo(ss.str());
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

    void onViewBinding() override {
        mRoot->SetDataContext(mViewModel);
        mRoot->FindName<Noesis::Button>("Resume")->Click() += [this](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
            mViewModel->setGamePaused(false);
            updateThreadPaused = false;
            glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        };
        mRoot->FindName<Noesis::Button>("Exit")->Click() += [this](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
            mViewModel->setGamePaused(false);
            updateThreadPaused = false;
            requestLeave();
            GUI::pushScene(Menus::startMenu());
        };
        for (int i = 0; i <= 9; ++i) {
            mHotBar[i] = mRoot->FindName<Noesis::Image>((std::string("Hotbar") + std::to_string(i)).c_str());
            mHotBarCnt[i] = mRoot->FindName<Noesis::TextBlock>((std::string("HotbarCnt") + std::to_string(i)).c_str());
        }
    }

    void onLoad() override {
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_TEXTURE_2D);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();

        Mutex = MutexCreate();
        //MutexLock(Mutex);
        currentGame = this;
        updateThread = ThreadCreate(&updateThreadFunc, nullptr);
        if (multiplayer) {
            fastSrand(static_cast<unsigned int>(time(nullptr)));
            Player::name = "";
            Player::onlineID = rand();
        }
        //初始化游戏状态
        infostream << "Init player...";
        if (loadGame()) Player::init(Player::Pos);
        else Player::spawn();
        infostream << "Init world...";
        World::Init();
        registerCommands();

        mShouldRenderGUI = true;
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        setupNormalFog();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();
        infostream << "Game start!";

        //这才是游戏开始!
        glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mxl = mx;
        myl = my;
        infostream << "Main loop started";
        updateThreadRun = true;
        lastupdate = timer();
    }

    static Noesis::ImageSource* getTextureForItem(item i) {
        static auto blockTextures = Noesis::MakePtr<Noesis::TextureSource>(NoesisApp::GLFactory::WrapTexture(
            BlockTextures, 256, 256, 0, false, true
        ));
        static std::unordered_map<item, Noesis::Ptr<Noesis::CroppedBitmap>> itemTextures;

        if (i == Blocks::ENV) return nullptr;
        // find from cache first
        auto itemTextureIter = itemTextures.find(i);

        if (itemTextureIter != itemTextures.end())
            return (*itemTextureIter).second.GetPtr();

        const auto tcX = Textures::getTexcoordX(i, 1) * 256;
        const auto tcY = Textures::getTexcoordY(i, 1) * 256;
        return itemTextures[i] = Noesis::MakePtr<Noesis::CroppedBitmap>(
            blockTextures.GetPtr(), Noesis::Int32Rect(tcX, tcY, 32, 32)  // TODO: refactor
        );
    }

    void onUpdate() override {
        glfwGetCursorPos(MainWindow, &mx, &my);
        
        debugInfo();
        for (int i = 0; i < 10; ++i) {
            mHotBar[i]->SetSource(getTextureForItem(Player::inventory[3][i]));
            mHotBarCnt[i]->SetText(std::to_string(Player::inventoryAmount[3][i]).c_str());
            // should have used proper data binding to separate logic from styling
            // but hey it works
            static_cast<Noesis::Label*>(mHotBar[i]->GetParent()->GetParent())->SetBorderThickness(
                Noesis::Thickness(i == Player::indexInHand ? 5.f : 1.f)
            );
        }

        if (glfwGetKey(MainWindow, GLFW_KEY_ESCAPE) == 1) {
            mViewModel->setGamePaused(true);
            updateThreadPaused = true;
            glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            createThumbnail();
        }
    }

    ~GameView() override {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();
        infostream << "Terminate threads";
        updateThreadRun = false;
        //MutexUnlock(Mutex);
        ThreadWait(updateThread);
        ThreadDestroy(updateThread);
        currentGame = nullptr;
        MutexDestroy(Mutex);
        saveGame();
        World::destroyAllChunks();
        if (!World::vbuffersShouldDelete.empty()) {
            glDeleteBuffersARB(World::vbuffersShouldDelete.size(), World::vbuffersShouldDelete.data());
            World::vbuffersShouldDelete.clear();
        }
        commands.clear();
        mChatMessages.clear();
        //GUI::popScene();
    }
};

void pushGameView() { GUI::pushScene(std::make_unique<GameView>()); }

ThreadFunc updateThreadFunc(void *) {
    if(currentGame) currentGame->GameThreadloop();
    return 0;
}
