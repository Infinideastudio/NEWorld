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
#include "gamescene.h"
#include "window.h"
#include "renderer/blockrenderer.h"
#include "sync_service/taskdispatcher.hpp"
#include "Common/Json/JsonHelper.h"
#include "Common/Logger.h"
#include "Common/Modules.h"

class PutBlockTask : public ReadWriteTask {
public:
    PutBlockTask(size_t worldID, const Vec3i& blockPosition, size_t blockID)
        : mWorldID(worldID), mBlockPosition(blockPosition), mBlockID(blockID) {}

    void task(ChunkService& cs) override {
        // TODO: let the server handle this.
        cs.getWorlds().getWorld(mWorldID)->setBlock(mBlockPosition, mBlockID);
    }

private:
    size_t mWorldID;
    Vec3i mBlockPosition;
    size_t mBlockID;
};

class PlayerControlTask : public ReadOnlyTask {
public:
    PlayerControlTask(Player& player) : mPlayer(player) {}

    void task(const ChunkService& cs) override {

        constexpr double speed = 0.1;
        constexpr double SelectDistance = 5.0;
        constexpr double SelectPrecision = 200.0;

        // TODO: Read keys from the configuration file
        auto state = Window::getKeyBoardState();
        if (state[SDL_SCANCODE_UP])
            mPlayer.accelerateRotation(Vec3d(1, 0.0, 0.0));
        if (state[SDL_SCANCODE_DOWN] && mPlayer.getRotation().x > -90)
            mPlayer.accelerateRotation(Vec3d(-1, 0.0, 0.0));
        if (state[SDL_SCANCODE_RIGHT])
            mPlayer.accelerateRotation(Vec3d(0.0, -1, 0.0));
        if (state[SDL_SCANCODE_LEFT])
            mPlayer.accelerateRotation(Vec3d(0.0, 1, 0.0));
        if (state[SDL_SCANCODE_W])
            mPlayer.accelerate(Vec3d(0.0, 0.0, -speed));
        if (state[SDL_SCANCODE_S])
            mPlayer.accelerate(Vec3d(0.0, 0.0, speed));
        if (state[SDL_SCANCODE_A])
            mPlayer.accelerate(Vec3d(-speed, 0.0, 0.0));
        if (state[SDL_SCANCODE_D])
            mPlayer.accelerate(Vec3d(speed, 0.0, 0.0));
        if (state[SDL_SCANCODE_E])
            mPlayer.accelerate(Vec3d(0.0, 0.0, -speed * 10));
        if (state[SDL_SCANCODE_SPACE])
            mPlayer.accelerate(Vec3d(0.0, 2 * speed, 0.0));
#ifdef NEWORLD_TARGET_MACOSX
        if (state[SDL_SCANCODE_LGUI] || state[SDL_SCANCODE_RGUI])
#else
        if (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL])
#endif
            mPlayer.accelerate(Vec3d(0.0, -2 * speed, 0.0));

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

            for (double i = 0.0; i < SelectDistance; i += 1.0 / SelectPrecision) {
                Vec3d pos = position + dir * i;
                Vec3i blockPos = Vec3i(int(std::floor(pos.x)), int(std::floor(pos.y)), int(std::floor(pos.z)));
                try {
                    if (world->getBlock(blockPos).getID() != 0) {
                        chunkService.getTaskDispatcher().addReadWriteTask(
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
    Player& mPlayer;
};

class UpsCounter : public ReadOnlyTask {
public:
    UpsCounter(size_t& updateCounter) : mUpdateCounter(updateCounter) {}

    void task(const ChunkService&) override { mUpdateCounter++; }

    std::unique_ptr<ReadOnlyTask> clone() override { return std::make_unique<UpsCounter>(*this); }

private:
    size_t& mUpdateCounter;
};

static bool isClient() { return context.args["multiplayer-client"]; }

size_t GameScene::requestWorld() {
    // TODO: change this

    if (isClient()) {
        auto& client = context.rpc.getClient();
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
        chunkService.getWorlds().addWorld(worldInfo["name"]);
    }

    // It's a simple wait-until-we-have-a-world procedure now.
    // But it should be changed into get player information
    // and get the world id from it.
    while (chunkService.getWorlds().getWorld(0) == nullptr)
        std::this_thread::yield();
    return 0;
}

GameScene::GameScene(const std::string& name, const Window& window):
    mWindow(window), mPlayer(0), mGUIWidgets(mWindow.getNkContext()) {

    mPlayer.setPosition(Vec3d(-16.0, 48.0, 32.0));
    mPlayer.setRotation(Vec3d(-45.0, -22.5, 0.0));
    Window::lockCursor();

    if (isClient()) {
        debugstream << "Game is running as the client of a multiplayer session.";
        chunkService.setAuthority(false);
    }
    else {
        // Initialize server
        mServer = std::make_unique<Server>(getJsonValue<unsigned short>(getSettings()["server"]["port"], 31111));
        mServer->run(getJsonValue<size_t>(getSettings()["server"]["rpc_thread_number"], 1));
    }

    // Initialize connection
    context.rpc.enableClient(
        getJsonValue<std::string>(getSettings()["server"]["ip"], "127.0.0.1"),
        getJsonValue<unsigned short>(getSettings()["server"]["port"], 31111));

    mCurrentWorld = chunkService.getWorlds().getWorld(requestWorld());
    mWorldRenderer = std::make_unique<WorldRenderer>(*mCurrentWorld,
                                                     getJsonValue<size_t>(getSettings()["gui"]["render_distance"], 3));

    // Initialize plugins
    infostream << "Initializing GUI plugins...";

    // Initialize update events
    mCurrentWorld->registerChunkTasks(chunkService, mPlayer);
    mWorldRenderer->registerTask(chunkService, mPlayer);
    chunkService.getTaskDispatcher().addRegularReadOnlyTask(
        std::make_unique<PlayerControlTask>(mPlayer));
    chunkService.getTaskDispatcher().addRegularReadOnlyTask(
        std::make_unique<UpsCounter>(mUpsCounter));

    // Initialize rendering
    mTexture = BlockTextureBuilder::buildAndFlush();
    BlockRendererManager::flushTextures();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Initialize Widgets
    mGUIWidgets.addWidget(std::make_shared<WidgetCallback>(
        "Debug", nk_rect(20, 20, 300, 300),
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
            nk_labelf(ctx, NK_TEXT_LEFT, "GUI Widgets: %zu", mGUIWidgets.getSize());
            nk_labelf(ctx, NK_TEXT_LEFT, "Chunks Loaded: %zu", mCurrentWorld->getChunkCount());
            nk_labelf(ctx, NK_TEXT_LEFT, "Modules Loaded: %zu", getModuleCount());
            nk_labelf(ctx, NK_TEXT_LEFT, "Update threads workload:");
            auto& dispatcher = chunkService.getTaskDispatcher();
            for (size_t i = 0; i < dispatcher.getTimeUsed().size(); ++i) {
                auto time = std::max(static_cast<long long>(dispatcher.getTimeUsed()[i]), 0ll);
                nk_labelf(ctx, NK_TEXT_LEFT, "Thread %zu: %lld ms (%.1f)%", i, time, time / 33.3333);
            }
            nk_labelf(ctx, NK_TEXT_LEFT, "Regular Tasks: read %zu write %zu",
                      dispatcher.getRegularReadOnlyTaskCount(),
                      dispatcher.getRegularReadWriteTaskCount());
        }));

    chunkService.getTaskDispatcher().start();
    infostream << "Game initialized!";
}

GameScene::~GameScene() { chunkService.getTaskDispatcher().stop(); }

void GameScene::render() {
    chunkService.getTaskDispatcher().processRenderTasks();

    // Camera control by mouse
    static const double mouseSensitivity =
        getJsonValue<double>(getSettings()["gui"]["mouse_sensitivity"], 0.3);
    MouseState mouse = Window::getInstance().getMouseMotion();
    mPlayer.accelerateRotation(Vec3d(-mouse.y * mouseSensitivity, -mouse.x * mouseSensitivity, 0.0));

    glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    double timeDelta = mUpdateScheduler.getDeltaTimeMs() / 1000.0 * UpdateFrequency;
    if (timeDelta > 1.0) timeDelta = 1.0;
    Vec3d playerRenderedPosition = mPlayer.getPosition() - mPlayer.getPositionDelta() * (1.0 - timeDelta);
    Vec3d playerRenderedRotation = mPlayer.getRotation() - mPlayer.getRotationDelta() * (1.0 - timeDelta);

    mTexture.bind(Texture::Texture2D);
    Renderer::clear();
    Renderer::setViewport(0, 0, mWindow.getWidth(), mWindow.getHeight());
    Renderer::restoreProj();
    Renderer::applyPerspective(70.0f, float(mWindow.getWidth()) / mWindow.getHeight(), 0.1f, 3000.0f);
    Renderer::restoreScale();
    Renderer::rotate(float(-playerRenderedRotation.x), Vec3f(1.0f, 0.0f, 0.0f));
    Renderer::rotate(float(-playerRenderedRotation.y), Vec3f(0.0f, 1.0f, 0.0f));
    Renderer::rotate(float(-playerRenderedRotation.z), Vec3f(0.0f, 0.0f, 1.0f));
    Renderer::translate(-playerRenderedPosition);

    // Render

    mWorldRenderer->render(Vec3i(mPlayer.getPosition()));
    // mPlayer.render();

    glDisable(GL_DEPTH_TEST);

    mGUIWidgets.render();

    mFpsCounter++;
}
