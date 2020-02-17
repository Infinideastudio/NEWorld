// 
// GUI: gamescene.cpp
// NEWorld: A Free Game with Similar Rules to Minecraft.
// Copyright (C) 2015-2018 NEWorld Team
// 
// NEWorld is free software: you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// NEWorld is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General 
// Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
// 

#include <atomic>
#include <chrono>
#include <mutex>
#include <Common/RPC/RPC.h>
#include <Game/SyncService/taskdispatcher.hpp>
#include "gamescene.h"
#include "Common/JsonHelper.h"
#include "neworld.h"

class PutBlockTask : public ReadWriteTask {
public:
    PutBlockTask(size_t worldID, const Vec3i& blockPosition, size_t blockID)
            :mWorldID(worldID), mBlockPosition(blockPosition), mBlockID(blockID) { }

    void task(ChunkService& cs) override {
        if (!cs.isAuthority())
            RPC::getClient().async_call("pickBlock", mWorldID, mBlockPosition);

        cs.getWorlds().getWorld(mWorldID)->setBlock(mBlockPosition, mBlockID);
    }

private:
    size_t mWorldID;
    Vec3i mBlockPosition;
    size_t mBlockID;
};

class PlayerControlTask : public ReadOnlyTask {
public:
    PlayerControlTask(Player& player)
            :mPlayer(player) { }

    void task(const ChunkService& cs) override {
        auto speed = mPlayer.getMovingSpeed();
        constexpr double SelectDistance = 5.0;
        constexpr double SelectPrecision = 200.0;

        auto& window = Window::getInstance();
        // TODO: Read keys from the configuration file
        if (window.getKeyBoardState(SDL_SCANCODE_UP)==KeyState::Hold)
            mPlayer.accelerateRotation(Vec3d(1, 0.0, 0.0));
        if (window.getKeyBoardState(SDL_SCANCODE_DOWN)==KeyState::Hold && mPlayer.getRotation().x>-90)
            mPlayer.accelerateRotation(Vec3d(-1, 0.0, 0.0));
        if (window.getKeyBoardState(SDL_SCANCODE_RIGHT)==KeyState::Hold)
            mPlayer.accelerateRotation(Vec3d(0.0, -1, 0.0));
        if (window.getKeyBoardState(SDL_SCANCODE_LEFT)==KeyState::Hold)
            mPlayer.accelerateRotation(Vec3d(0.0, 1, 0.0));
        if (window.getKeyBoardState(SDL_SCANCODE_W)==KeyState::Hold)
            mPlayer.accelerate(Vec3d(0.0, 0.0, -speed));
        if (window.getKeyBoardState(SDL_SCANCODE_S)==KeyState::Hold)
            mPlayer.accelerate(Vec3d(0.0, 0.0, speed));
        if (window.getKeyBoardState(SDL_SCANCODE_A)==KeyState::Hold)
            mPlayer.accelerate(Vec3d(-speed, 0.0, 0.0));
        if (window.getKeyBoardState(SDL_SCANCODE_D)==KeyState::Hold)
            mPlayer.accelerate(Vec3d(speed, 0.0, 0.0));
        if (window.getKeyBoardState(SDL_SCANCODE_E)==KeyState::Hold)
            mPlayer.accelerate(Vec3d(0.0, 0.0, -speed*10));
        if (window.getKeyBoardState(SDL_SCANCODE_SPACE)==KeyState::Hold) {
            if (mPlayer.isFlying())
                mPlayer.accelerate(Vec3d(0.0, 2*speed, 0.0));
            else
                mPlayer.jump();
        }

        if (window.getKeyBoardState(SDL_SCANCODE_F1)==KeyState::KeyDown)
            mPlayer.setFlying(!mPlayer.isFlying());

        if (window.getKeyBoardState(SDL_SCANCODE_F2)==KeyState::KeyDown) {
            if (Window::getInstance().isCursorLocked())
                Window::getInstance().unlockCursor();
            else
                Window::getInstance().lockCursor();
        }

#ifdef NEWORLD_TARGET_MACOSX
        if (state[SDL_SCANCODE_LGUI] || state[SDL_SCANCODE_RGUI])
#else
        if ((window.getKeyBoardState(SDL_SCANCODE_LCTRL)==KeyState::Hold
                || window.getKeyBoardState(SDL_SCANCODE_RCTRL)==KeyState::Hold) && mPlayer.isFlying())
#endif
            mPlayer.accelerate(Vec3d(0.0, -2*speed, 0.0));

        // Handle left-click events
        if (Window::getInstance().getMouseMotion().left) {
            // Selection
            const World* world = cs.getWorlds().getWorld(mPlayer.getWorldID());
            Mat4f trans(1.0f);
            const auto& position = mPlayer.getPosition();
            const auto& rotation = mPlayer.getRotation();
            trans *= Mat4f::rotation(float(rotation.y), Vec3f(0.0f, 1.0f, 0.0f));
            trans *= Mat4f::rotation(float(rotation.x), Vec3f(1.0f, 0.0f, 0.0f));
            trans *= Mat4f::rotation(float(rotation.z), Vec3f(0.0f, 0.0f, 1.0f));
            Vec3d dir = trans.transform(Vec3f(0.0f, 0.0f, -1.0f), 0.0f).first.normalize();

            for (double i = 0.0; i<SelectDistance; i += 1.0/SelectPrecision) {
                Vec3d pos = position+dir*i;
                Vec3i blockPos = Vec3i(int(std::floor(pos.x)), int(std::floor(pos.y)), int(std::floor(pos.z)));
                try {
                    if (world->getBlock(blockPos).getID()!=0) {
                        TaskDispatch::addNext(
                                std::make_unique<PutBlockTask>(mPlayer.getWorldID(), blockPos, 0));
                        break;
                    }
                }
                catch (std::out_of_range&) { break; }
            }
        }

        //    mGUIWidgets.update();
    }

    std::unique_ptr<ReadOnlyTask> clone() override { return std::make_unique<PlayerControlTask>(*this); }

private:
    static constexpr double FallingSpeed = -0.5;
    Player& mPlayer;
};

class UpsCounter : public ReadOnlyTask {
public:
    UpsCounter(size_t& updateCounter)
            :mUpdateCounter(updateCounter) { }

    void task(const ChunkService&) override { mUpdateCounter++; }

    std::unique_ptr<ReadOnlyTask> clone() override { return std::make_unique<UpsCounter>(*this); }

private:
    size_t& mUpdateCounter;
};

static bool isClient() { return Application::args()["multiplayer-client"]; }

size_t GameScene::requestWorld() {
    // TODO: change this

    if (isClient()) {
        auto& client = RPC::getClient();
        debugstream << "Connecting the server for world information...";
        auto worldIds = client.call("getAvailableWorldId").as<std::vector<uint32_t>>();
        if (worldIds.empty()) {
            errorstream << "The server didn't response with any valid worlds.";
            assert(false);
        }
        debugstream << "Worlds ids fetched from the server: " << worldIds;
        auto worldInfo = client.call("getWorldInfo", worldIds[0])
                .as<std::unordered_map<std::string, std::string>>();
        debugstream << "World info fetched from the server: "
                       "name: " << worldInfo["name"];
        chunkService->getWorlds().addWorld(worldInfo["name"]);
    }

    // It's a simple wait-until-we-have-a-world procedure now.
    // But it should be changed into get player information
    // and get the world id from it.
    while (chunkService->getWorlds().getWorld(0)==nullptr)
        std::this_thread::yield();
    return 0;
}

GameScene::GameScene(const std::string& name, const Window& window)
        :mWindow(window), mPlayer(0), mGUIWidgets(mWindow.getNkContext()),
        chunkService(&hChunkService.Get<ChunkService>()) {
    mPlayer.setPosition(Vec3d(-16.0, 48.0, 32.0));
    mPlayer.setRotation(Vec3d(-45.0, -22.5, 0.0));
    Window::getInstance().lockCursor();

    if (isClient()) {
        debugstream << "Game is running as the client of a multiplayer session.";
        chunkService->setAuthority(false);
    }
    else {
        // Initialize server
        mServer = std::make_unique<Server>(getJsonValue<unsigned short>(getSettings()["server"]["port"], 31111));
        mServer->run(getJsonValue<size_t>(getSettings()["server"]["rpc_thread_number"], 1));
    }

    // Initialize connection
    RPC::enableClient(
            getJsonValue<std::string>(getSettings()["server"]["ip"], "127.0.0.1"),
            getJsonValue<unsigned short>(getSettings()["server"]["port"], 31111));

    mCurrentWorld = chunkService->getWorlds().getWorld(requestWorld());
    mWorldRenderer = std::make_unique<WorldRenderer>(*mCurrentWorld,
            getJsonValue<size_t>(getSettings()["gui"]["render_distance"], 3));

    // Initialize plugins
    infostream << "Initializing GUI plugins...";

    // Initialize update events
    mCurrentWorld->registerChunkTasks(mPlayer);
    mWorldRenderer->registerTask(mPlayer);
    TaskDispatch::addRegular(std::make_unique<PlayerControlTask>(mPlayer));
    TaskDispatch::addRegular(std::make_unique<UpsCounter>(mUpsCounter));

    // Initialize rendering
    mTexture = BlockTextureBuilder::buildAndFlush();
    BlockRendererManager::flushTextures();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Initialize Widgets
    mGUIWidgets.addWidget(std::make_shared<WidgetCallback>(
            "Debug", nk_rect(20, 20, 300, 500),
            NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                    NK_WINDOW_CLOSABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE, [this](nk_context* ctx) {
                if (mRateCounterScheduler.isDue()) {
                    // Update FPS & UPS
                    mFpsLatest = mFpsCounter;
                    mUpsLatest = mUpsCounter;
                    mFpsCounter = 0;
                    mUpsCounter = 0;
                    mRateCounterScheduler.increaseTimer();
                }

                nk_layout_row_dynamic(ctx, 15, 1);
                nk_labelf(ctx, NK_TEXT_LEFT, "NEWorld %s (v%u)", NEWorldVersionName, NEWorldVersion);
                nk_labelf(ctx, NK_TEXT_LEFT, "FPS %zu, UPS %zu", mFpsLatest, mUpsLatest);
                nk_labelf(ctx, NK_TEXT_LEFT, "Position: x %.1f y %.1f z %.1f",
                        mPlayer.getPosition().x, mPlayer.getPosition().y, mPlayer.getPosition().z);
                nk_labelf(ctx, NK_TEXT_LEFT, "On ground: %s, flying %s",
                        mPlayer.onGround() ? "True" : "False", mPlayer.isFlying() ? "True" : "False");
                nk_labelf(ctx, NK_TEXT_LEFT, "GUI Widgets: %zu", mGUIWidgets.getSize());
                nk_labelf(ctx, NK_TEXT_LEFT, "Chunks Loaded: %zu", mCurrentWorld->getChunkCount());
                nk_labelf(ctx, NK_TEXT_LEFT, "Modules Loaded: %d", getModuleCount());
                nk_labelf(ctx, NK_TEXT_LEFT, "Update threads workload:");
                for (size_t i = 0; i<TaskDispatch::countWorkers(); ++i) {
                    auto time = std::max(static_cast<long long>(TaskDispatch::getReadTimeUsed(i)), 0ll);
                    nk_labelf(ctx, NK_TEXT_LEFT, "Thread %zu: %lld ms (%.1f)%%", i, time, time*100.0/33.3333);
                }
                const auto rwTime = TaskDispatch::getRWTimeUsed();
                nk_labelf(ctx, NK_TEXT_LEFT, "RW Tasks: %lld ms (%.1f)%%", rwTime, rwTime*100.0/33.3333);

                nk_labelf(ctx, NK_TEXT_LEFT, "Regular Tasks: read %d write %d",
                        TaskDispatch::getRegularReadTaskCount(),
                        TaskDispatch::getRegularReadWriteTaskCount());
                nk_labelf(ctx, NK_TEXT_LEFT, "All Tasks: read %d write %d",
                        TaskDispatch::getReadTaskCount(), TaskDispatch::getReadWriteTaskCount());
            }));

    TaskDispatch::boot();
    infostream << "Game initialized!";
}

void GameScene::render() {
    TaskDispatch::handleRenderTasks();

    // Camera control by mouse
    static const double mouseSensitivity =
            getJsonValue<double>(getSettings()["gui"]["mouse_sensitivity"], 0.3);
    MouseState mouse = Window::getInstance().getMouseMotion();
    if (mouse.relative) // only rotate the camera when the cursor is locked.
        mPlayer.accelerateRotation(Vec3d(-mouse.y*mouseSensitivity, -mouse.x*mouseSensitivity, 0.0));

    glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    double timeDelta = mUpdateScheduler.getDeltaTimeMs()/1000.0*UpdateFrequency;
    if (timeDelta>1.0) timeDelta = 1.0;
    Vec3d playerRenderedPosition = mPlayer.getPosition()-mPlayer.getPositionDelta()*(1.0-timeDelta);
    Vec3d playerRenderedRotation = mPlayer.getRotation()-mPlayer.getRotationDelta()*(1.0-timeDelta);

    glActiveTexture(GL_TEXTURE0);
    mTexture.bind(Texture::Texture2D);
    Renderer::clear();
    int width{}, height{};
    mWindow.getDrawableSize(width, height);
    Renderer::setViewport(0, 0, width, height);
    Renderer::restoreProj();
    Renderer::applyPerspective(70.0f, float(mWindow.getWidth())/mWindow.getHeight(), 0.1f, 3000.0f);
    Renderer::restoreView();
    Renderer::rotateView(float(-playerRenderedRotation.x), Vec3f(1.0f, 0.0f, 0.0f));
    Renderer::rotateView(float(-playerRenderedRotation.y), Vec3f(0.0f, 1.0f, 0.0f));
    Renderer::rotateView(float(-playerRenderedRotation.z), Vec3f(0.0f, 0.0f, 1.0f));
    Renderer::translateView(-playerRenderedPosition);
    Renderer::restoreScale();
    // Render
    mWorldRenderer->render(Vec3i(mPlayer.getPosition()));
    // mPlayer.render();

    glDisable(GL_DEPTH_TEST);

    mGUIWidgets.render();

    mFpsCounter++;
}
