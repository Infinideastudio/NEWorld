#include <deque>
#include "GUI.h"
#include <NsRender/GLFactory.h>
#include <NoesisPCH.h>

#include "Noesis.h"
#include "System/MessageBus.h"
#include "Common/Logger.h"

namespace GUI {
    static Noesis::Key mapKey(int glfwKey) {
        using namespace Noesis;
        static std::unordered_map<int, Noesis::Key> keyTable = {
			{GLFW_KEY_SPACE ,Key_Space},
			{GLFW_KEY_MINUS ,Key_Subtract},
            {GLFW_KEY_0, Key_D0},
            {GLFW_KEY_1, Key_D1},
            {GLFW_KEY_2, Key_D2},
            {GLFW_KEY_3, Key_D3},
            {GLFW_KEY_4, Key_D4},
            {GLFW_KEY_5, Key_D5},
            {GLFW_KEY_6, Key_D6},
            {GLFW_KEY_7, Key_D7},
            {GLFW_KEY_8, Key_D8},
            {GLFW_KEY_9, Key_D9},
            {GLFW_KEY_A, Key_A},
            {GLFW_KEY_B, Key_B},
            {GLFW_KEY_C, Key_C},
            {GLFW_KEY_D, Key_D},
            {GLFW_KEY_E, Key_E},
            {GLFW_KEY_F, Key_F},
            {GLFW_KEY_G, Key_G},
            {GLFW_KEY_H, Key_H},
            {GLFW_KEY_I, Key_I},
            {GLFW_KEY_J, Key_J},
            {GLFW_KEY_K, Key_K},
            {GLFW_KEY_L, Key_L},
            {GLFW_KEY_M, Key_M},
            {GLFW_KEY_N, Key_N},
            {GLFW_KEY_O, Key_O},
            {GLFW_KEY_P, Key_P},
            {GLFW_KEY_Q, Key_Q},
            {GLFW_KEY_R, Key_R},
            {GLFW_KEY_S, Key_S},
            {GLFW_KEY_T, Key_T},
            {GLFW_KEY_U, Key_U},
            {GLFW_KEY_V, Key_V},
            {GLFW_KEY_W, Key_W},
            {GLFW_KEY_X, Key_X},
            {GLFW_KEY_Y, Key_Y},
            {GLFW_KEY_Z, Key_Z}
        };
        return keyTable[glfwKey];
    }

    void Scene::update() {
        // TODO: change to use glfw callback + message bus?
        if (mView) {
            static bool pressed = false;
            static double lastPressedTime = 0;
            double xpos, ypos;
            glfwGetCursorPos(MainWindow, &xpos, &ypos);
            mView->MouseMove(xpos, ypos);
            mView->SetSize(windowwidth, windowheight);
            int state = glfwGetMouseButton(MainWindow, GLFW_MOUSE_BUTTON_LEFT);
            if (state == GLFW_PRESS && !pressed) {
                mView->MouseButtonDown(xpos, ypos, Noesis::MouseButton_Left);
                auto curTime = timer();
                if (curTime - lastPressedTime < 0.25) {
                    mView->MouseDoubleClick(xpos, ypos, Noesis::MouseButton_Left);
                }
                pressed = true;
                lastPressedTime = curTime;
            }
            else if (state != GLFW_PRESS && pressed) {
                pressed = false;
                mView->MouseButtonUp(xpos, ypos, Noesis::MouseButton_Left);
            }
        }

        onUpdate();
    }

    void Scene::render() {
        mFPS.update();

        if (mView) {
            // Update view (layout, animations, ...)
            mView->Update(timer() - mEnterTimeInSec);

            glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT);
            // Offscreen rendering phase populates textures needed by the on-screen rendering
            mView->GetRenderer()->UpdateRenderTree();
            mView->GetRenderer()->RenderOffscreen();
            glPopAttrib();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, windowwidth, windowheight);

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClearStencil(0);
            glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }
        
        onRender();

        if (mView) {
            glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT);
            // Rendering is done in the active framebuffer
            mView->GetRenderer()->Render();
            glPopAttrib();
        }
    }

    Scene::~Scene() {
        if (mView) {
            mView->GetRenderer()->Shutdown();
            mView.Reset();
        }
    }

    void Scene::loadView() {
        mRoot = Noesis::GUI::LoadXaml<Noesis::Grid>(mXamlPath);
        if (!mRoot) {
            errorstream << "UI failed to load!";
            return;
        }
        onViewBinding();
        mView = Noesis::GUI::CreateView(mRoot);
        mView->SetFlags(Noesis::RenderFlags_PPAA | Noesis::RenderFlags_LCD);
        mView->GetRenderer()->Init(GUI::renderDevice);
    }

    void Scene::singleLoop() {
        update();
        render();
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();
    }

    void Scene::load() {
    	loadView();

        glfwSetInputMode(MainWindow, GLFW_CURSOR, mHasCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
        if (mXamlPath) {
            mListeners.push_back(MessageBus::Default().Get<std::pair<int, int>>("KeyEvents")->Listen([this](void*, std::pair<int, int> keyAndAction) {
                auto [key, action] = keyAndAction;
                if (action == GLFW_PRESS) mView->KeyDown(mapKey(key));
                else if (action == GLFW_RELEASE) mView->KeyUp(mapKey(key));

                if (key == GLFW_KEY_F5 && action == GLFW_PRESS) {
                    // reload view. might leak memory but it's for debug only
                    infostream << "Reloading View";
                    loadView();
                }
            }));
            mListeners.push_back(MessageBus::Default().Get<int>("InputEvents")->Listen([this](void*, int k) {
                mView->Char(k);
            }));
        }

        onLoad();
    }

    std::deque<std::unique_ptr<Scene>> scenes;
    
    void pushScene(std::unique_ptr<Scene> scene) {
        scene->load();
        scenes.emplace_back(std::move(scene));
    }

    void popScene() {
        scenes.pop_back();
    }

    void clearScenes() {
        while (!scenes.empty()) popScene();
    }

    void appStart() {
        glfwSetInputMode(MainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glDisable(GL_CULL_FACE);

        while (!scenes.empty()) {
            auto& currentScene = scenes.back();
            currentScene->singleLoop();
            if (currentScene->shouldLeave()) GUI::popScene();
            if (glfwWindowShouldClose(MainWindow)) {
                clearScenes();
            }
        }
        AppCleanUp();
    }
}
