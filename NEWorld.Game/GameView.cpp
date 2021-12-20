#include "GameView.h"

#include <optional>
#include <utility>
#include <Renderer/World/ShadowMaps.h>
#include "Universe/World/Blocks.h"
#include "Textures.h"
#include "Renderer/Renderer.h"
#include "Universe/World/World.h"
#include "Renderer/World/WorldRenderer.h"
#include "Particles.h"
#include "GUI/GUI.h"
#include "Command.h"
#include "Setup.h"
#include "Universe/Game.h"
#include "Common/Logger.h"
#include "NsApp/NotifyPropertyChangedBase.h"
#include "NsGui/Button.h"
#include "NsGui/Image.h"
#include "GUI/InventorySlot.h"
#include "NsGui/TextBlock.h"
#include "GUI/Menus/Menus.h"
#include "NsGui/StackPanel.h"
#include "NsGui/UIElementCollection.h"
#include "NsGui/WrapPanel.h"
#include "ControlContext.h"

namespace NoesisApp {
    class Window;
}

class GameView;
// pretty hacky. try to remove later.
GameView* currentGame = nullptr;

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
    bool getBagOpen() const {
        return mBagOpen;
    }

    void setBagOpen(bool bagOpen) {
        if (bagOpen != mBagOpen) {
            mBagOpen = bagOpen;
            OnPropertyChanged("BagOpen");
        }
    }
    double getHP() const { return mHealth; }
    double getHPMax() const { return mHealthMax; }
    void notifyHPChanges(PlayerEntity* player) {
        if (mHealth != player->getHealth() || mHealthMax != player->getMaxHealth()) {
            mHealth = player->getHealth();
            mHealthMax = player->getMaxHealth();

            OnPropertyChanged("HP");
            OnPropertyChanged("HPMax");
        }
    }

private:
    std::string mDebugInfo;
    bool mGamePaused = false;
    bool mBagOpen = false;
    double mHealth, mHealthMax;

    NS_IMPLEMENT_INLINE_REFLECTION(GameViewViewModel, NotifyPropertyChangedBase) {
        NsProp("DebugInfo", &GameViewViewModel::getDebugInfo);
        NsProp("GamePaused", &GameViewViewModel::getGamePaused);
        NsProp("BagOpen", &GameViewViewModel::getBagOpen);
        NsProp("HP", &GameViewViewModel::getHP);
        NsProp("HPMax", &GameViewViewModel::getHPMax);
    }
};

class GameView : public virtual GUI::Scene, public Game {
private:
    ControlContext mControls{ MainWindow };
    GUI::FpsCounter mUpsCounter;
    InventorySlot* mHotBar[10];
    InventorySlot* mInventory[4][10];
    Noesis::Ptr<GameViewViewModel> mViewModel;
    std::thread mUpdateThread;
    Frustum mFrustum;

    struct ItemMoveContext {
        int row, col;
        int quantity;
    };
    std::optional<ItemMoveContext> mInventoryMoveFrom;

public:
    GameView() : Scene("InGame.xaml", false), mViewModel(Noesis::MakePtr<GameViewViewModel>()) {}

    void gameThread() {
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
        lastUpdate = timer();

        while (updateThreadRun) {
            MutexUnlock(Mutex);
            std::this_thread::yield();
            MutexLock(Mutex);

            while (updateThreadPaused) {
                MutexUnlock(Mutex);
                std::this_thread::yield();
                MutexLock(Mutex);
                lastUpdate = timer();
            }

            FirstUpdateThisFrame = true;
            double currentTime = timer();
            if (currentTime - lastUpdate >= 5.0) lastUpdate = currentTime;
            
            while (currentTime - lastUpdate >= 1.0 / MaxUpdateFPS && mUpsCounter.getFPS() < 60) {
                lastUpdate += 1.0 / MaxUpdateFPS;
                mUpsCounter.frame();
                updateGame();
                FirstUpdateThisFrame = false;
            }

            mUpsCounter.check();
        }
        MutexUnlock(Mutex);
    }

    void gameRender() {
        //画场景
        const auto currentTime = timer();

        const auto camera = mPlayer->renderUpdate(
            mControls, 
            mBagOpened || mViewModel->getGamePaused(),
            mControlsForUpdate.Current.Time
        );

        const double xpos = camera.position.X, ypos = camera.position.Y, zpos = camera.position.Z;

        if (mPlayer->isRunning()) {
            if (FOVyExt < 9.8) {
                FOVyExt = 10.0f - (10.0f - FOVyExt) * static_cast<float>(pow(0.8, (currentTime - SpeedupAnimTimer) * 30));
                SpeedupAnimTimer = currentTime;
            } else FOVyExt = 10.0;
        } else {
            if (FOVyExt > 0.2) {
                FOVyExt *= static_cast<float>(pow(0.8, (currentTime - SpeedupAnimTimer) * 30));
                SpeedupAnimTimer = currentTime;
            } else FOVyExt = 0.0;
        }
        SpeedupAnimTimer = currentTime;

        //更新区块VBO
        World::sortChunkBuildRenderList(
            RoundInt(mPlayer->getPosition().X),
            RoundInt(mPlayer->getPosition().Y), 
            RoundInt(mPlayer->getPosition().Z)
        );
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

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        //daylight = clamp((1.0 - cos((double)gametime / gameTimeMax * 2.0 * M_PI) * 2.0) / 2.0, 0.05, 1.0);
        //Renderer::sunlightXrot = 90 * daylight;
        if (Renderer::AdvancedRender) {
            //Build shadow map
            if (!DebugShadow) ShadowMaps::BuildShadowMap(xpos, ypos, zpos, currentTime);
            else ShadowMaps::RenderShadowMap(xpos, ypos, zpos, currentTime);
        }
        glClearColor(skycolorR, skycolorG, skycolorB, 1.0);
        if (!DebugShadow) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_TEXTURE_2D);

        mFrustum.LoadIdentity();
        mFrustum.SetPerspective(FOVyNormal + FOVyExt, static_cast<float>(windowwidth) / windowheight, 0.05f,
                                           viewdistance * 16.0f);
        mFrustum.MultRotate(static_cast<float>(camera.lookUpDown), 1, 0, 0);
        mFrustum.MultRotate(360.0f - static_cast<float>(camera.heading), 0, 1, 0);
        mFrustum.update();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMultMatrixf(mFrustum.getProjMatrix());
        glMatrixMode(GL_MODELVIEW);

        World::calcVisible(xpos, ypos, zpos, mFrustum);
        auto playerChunk = mPlayer->getChunkPosition();
        WorldRenderer::ListRenderChunks(
            playerChunk.X, playerChunk.Y, playerChunk.Z,
            viewdistance,
            currentTime
        );

        MutexUnlock(Mutex);

        glBindTexture(GL_TEXTURE_2D, BlockTextures);

        // 渲染层1
        glLoadIdentity();
        glRotated(camera.lookUpDown, 1, 0, 0);
        glRotated(360.0 - camera.heading, 0, 1, 0);
        glDisable(GL_BLEND);
        Renderer::EnableShaders();
        if (!DebugShadow) WorldRenderer::RenderChunks(xpos, ypos, zpos, 0);
        Renderer::DisableShaders();
        glEnable(GL_BLEND);

        MutexLock(Mutex);

        if (mBlockDestructionProgress > 0.0) {
            auto breakingBlock = mCurrentSelection->second;
            glTranslated(breakingBlock.X - xpos, breakingBlock.Y - ypos, breakingBlock.Z - zpos);
            renderDestroy(mBlockDestructionProgress, 0, 0, 0);
            glTranslated(-breakingBlock.X + xpos, -breakingBlock.Y + ypos, -breakingBlock.Z + zpos);
        }
        glBindTexture(GL_TEXTURE_2D, BlockTextures);
        Particles::renderall(xpos, ypos, zpos);

        glDisable(GL_TEXTURE_2D);
        if (mShouldRenderGUI && mCurrentSelection) {
            auto selectingBlock = mCurrentSelection->second;
            glTranslated(selectingBlock.X - xpos, selectingBlock.Y - ypos, selectingBlock.Z - zpos);
            drawBorder(0, 0, 0);
            glTranslated(-selectingBlock.X + xpos, -selectingBlock.Y + ypos, -selectingBlock.Z + zpos);
        }

        MutexUnlock(Mutex);

        // 渲染层2&3
        glLoadIdentity();
        glRotated(camera.lookUpDown, 1, 0, 0);
        glRotated(360.0 - camera.heading, 0, 1, 0);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_CULL_FACE);
        glBindTexture(GL_TEXTURE_2D, BlockTextures);
        Renderer::EnableShaders();
        if (!DebugShadow) WorldRenderer::RenderChunks(xpos, ypos, zpos, 1);
        glDisable(GL_CULL_FACE);
        if (!DebugShadow) WorldRenderer::RenderChunks(xpos, ypos, zpos, 2);
        Renderer::DisableShaders();

        glLoadIdentity();
        glRotated(camera.lookUpDown, 1, 0, 0);
        glRotated(360.0 - camera.heading, 0, 1, 0);
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
        if (currentTime - screenshotAnimTimer <= 1.0 && !shouldGetScreenshot) {
            const auto col = 1.0f - static_cast<float>(currentTime - screenshotAnimTimer);
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
            screenshotAnimTimer = currentTime;
            auto t = time(nullptr);
            char tmp[64];
            const auto timeinfo = localtime(&t);
            strftime(tmp, sizeof(tmp), "%Y年%m月%d日%H时%M分%S秒", timeinfo);
            std::stringstream ss;
            ss << "Screenshots/" << tmp << ".bmp";
            saveScreenshot(0, 0, windowwidth, windowheight, ss.str());
        }
        if (shouldGetThumbnail) {
            shouldGetThumbnail = false;
            createThumbnail();
        }
    }

    void onRender() override {
        MutexLock(Mutex);
        gameRender();
        MutexUnlock(Mutex);
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
            ss << "X: " << mPlayer->getPosition().X << " Y: " << mPlayer->getPosition().Y << " Z: " << mPlayer->getPosition().Z << std::endl
                << "Direction:" << mPlayer->getHeading() << " Head:" << mPlayer->getLookUpDown() << std::endl
                << "Jump speed:" << mPlayer->getCurrentJumpSpeed() << std::endl
                << "Stats:";
            if (mPlayer->isFlying()) ss << " Flying";
            if (mPlayer->isOnGround()) ss << " On_ground";
            if (mPlayer->isNearWall()) ss << " Near_wall";
            if (mPlayer->isCrossWall()) ss << " Cross_Wall";
            ss << std::endl;
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
            glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        };
        mRoot->FindName<Noesis::Button>("Exit")->Click() += [this](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
            mViewModel->setGamePaused(false);
            updateThreadPaused = false;
            requestLeave();
            pushScene(Menus::startMenu());
        };
        auto hotbar = mRoot->FindName<Noesis::StackPanel>("Hotbar");
        for (auto& slot : mHotBar) {
            hotbar->GetChildren()->Add(slot = new InventorySlot());
        }
        auto inventory = mRoot->FindName<Noesis::WrapPanel>("Inventory");
        for (int row = 0; row < 4; ++row) {
            for (int i = 0; i < 10; ++i) {
                inventory->GetChildren()->Add(mInventory[row][i] = new InventorySlot());

                mInventory[row][i]->PreviewMouseUp() += [this, row, i](Noesis::BaseComponent*, const Noesis::MouseButtonEventArgs& args) {
                    auto playerInventory = mPlayer->getInventory();
                    auto& thisAmount = playerInventory[row][i].amount;
                    auto& thisItem = playerInventory[row][i].item;
                    bool rightClick = args.changedButton == Noesis::MouseButton_Right;
                    if (mInventoryMoveFrom.has_value()) { // if has already selected one
                        auto& from = mInventoryMoveFrom.value();
                        auto& fromAmount = playerInventory[from.row][from.col].amount;
                        auto& fromItem = playerInventory[from.row][from.col].item;
                        if (thisItem != fromItem && thisItem != Blocks::ENV) { // different item - swap
                            std::swap(fromAmount, thisAmount);
                            std::swap(fromItem, thisItem);
                            from.quantity = 0;
                        }
                        else { // same item or empty - stack
                            const auto moveAmount = rightClick ? 1 : std::min(255 - thisAmount, std::min(from.quantity, int(fromAmount)));
                            thisItem = fromItem;
                            fromAmount -= moveAmount;
                            thisAmount += moveAmount;
                            from.quantity -= moveAmount;
                        }
                    	if (fromAmount == 0) fromItem = Blocks::ENV;
                        if (from.quantity == 0) mInventoryMoveFrom.reset(); // done transfer
                    }
                    else if (thisAmount != 0) { // if not selected and this one is selectable
                        mInventoryMoveFrom = ItemMoveContext{
                            row,
                            i ,
                            args.changedButton == Noesis::MouseButton_Left ? thisAmount :std::max(thisAmount / 2, 1)
                        };
                    }
                };
            }
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
        mUpdateThread = std::thread([this] {gameThread(); });
        //初始化游戏状态
        InitGame();
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
        infostream << "Main loop started";
        updateThreadRun = true;
        lastUpdate = timer();
    }

    void onUpdate() override {
        mControls.Update();

        debugInfo();
        auto inventory = mPlayer->getInventory();
        for (int i = 0; i < 10; ++i) {
            mHotBar[i]->setItemStack(inventory[3][i]);
            mHotBar[i]->setSelected(i == mPlayer->getCurrentHotbarSelection());
        }
        for (int row = 0; row < 4;++row) {
            for (int i = 0; i < 10; ++i) {
                mInventory[row][i]->setItemStack(inventory[row][i]);
                mInventory[row][i]->setSelected(mInventoryMoveFrom.has_value() && 
                    row == mInventoryMoveFrom.value().row && i == mInventoryMoveFrom.value().col);
            }
        }
        mViewModel->notifyHPChanges(mPlayer);
        mViewModel->setBagOpen(mBagOpened);

        static bool wasBagOpen = false;
        if (mViewModel->getBagOpen()) {
            if (!wasBagOpen) {
                wasBagOpen = true;
                glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
        else if (wasBagOpen) {
            wasBagOpen = false;
            glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
        mUpdateThread.join();
        currentGame = nullptr;
        MutexDestroy(Mutex);
        saveGame();
        World::destroyAllChunks();
        if (!World::vbuffersShouldDelete.empty()) {
            glDeleteBuffersARB(World::vbuffersShouldDelete.size(), World::vbuffersShouldDelete.data());
            World::vbuffersShouldDelete.clear();
        }
        commands.clear();
    }
};

void pushGameView() { GUI::pushScene(std::make_unique<GameView>()); }
