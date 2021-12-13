#include <deque>
#include "GUI.h"
#include <NsRender/GLFactory.h>
#include <NoesisPCH.h>
#include "Shader.h"
#include "System/MessageBus.h"
#include "Common/Logger.h"

namespace GUI {
    static Noesis::Key mapKey(int glfwKey) {
        using namespace Noesis;
        static Noesis::Key keyTable[255] = {};
        static bool keysLoaded = false;
        if (!keysLoaded) {
            keyTable['0'] = Key_D0;
            keyTable['1'] = Key_D1;
            keyTable['2'] = Key_D2;
            keyTable['3'] = Key_D3;
            keyTable['4'] = Key_D4;
            keyTable['5'] = Key_D5;
            keyTable['6'] = Key_D6;
            keyTable['7'] = Key_D7;
            keyTable['8'] = Key_D8;
            keyTable['9'] = Key_D9;
            keyTable['A'] = Key_A;
            keyTable['B'] = Key_B;
            keyTable['C'] = Key_C;
            keyTable['D'] = Key_D;
            keyTable['E'] = Key_E;
            keyTable['F'] = Key_F;
            keyTable['G'] = Key_G;
            keyTable['H'] = Key_H;
            keyTable['I'] = Key_I;
            keyTable['J'] = Key_J;
            keyTable['K'] = Key_K;
            keyTable['L'] = Key_L;
            keyTable['M'] = Key_M;
            keyTable['N'] = Key_N;
            keyTable['O'] = Key_O;
            keyTable['P'] = Key_P;
            keyTable['Q'] = Key_Q;
            keyTable['R'] = Key_R;
            keyTable['S'] = Key_S;
            keyTable['T'] = Key_T;
            keyTable['U'] = Key_U;
            keyTable['V'] = Key_V;
            keyTable['W'] = Key_W;
            keyTable['X'] = Key_X;
            keyTable['Y'] = Key_Y;
            keyTable['Z'] = Key_Z;
            keysLoaded = true;
        }
        if (glfwKey >= 'a' && glfwKey <= 'z') glfwKey = toupper(glfwKey);
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
                if (curTime - lastPressedTime < 0.5) {
                    mView->MouseDoubleClick(xpos, ypos, Noesis::MouseButton_Left);
                }
                pressed = true;
                lastPressedTime = curTime;
            }
            else if(state != GLFW_PRESS && pressed) {
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

        //Disable shader
        Shader::unbind();
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

    void Scene::singleLoop() {
        update();
        render();
        glfwSwapBuffers(MainWindow);
        glfwPollEvents();
    }

    void Scene::load() {
        static Noesis::Ptr<Noesis::RenderDevice> renderDevice = nullptr;
        if (!renderDevice) {
            glPushAttrib(GL_ALL_ATTRIB_BITS);
            renderDevice = NoesisApp::GLFactory::CreateDevice(false);
            glPopAttrib();
        }

        glfwSetInputMode(MainWindow, GLFW_CURSOR, mHasCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
        if (mXamlPath) {
            mRoot = Noesis::GUI::LoadXaml<Noesis::Grid>(mXamlPath);
            onViewBinding();
            mView = Noesis::GUI::CreateView(mRoot);
            mView->SetFlags(Noesis::RenderFlags_PPAA | Noesis::RenderFlags_LCD);
            mView->GetRenderer()->Init(renderDevice);

            mListeners.push_back(MessageBus::Default().Get<int>("KeyEvents")->Listen([this](void*, int k) {
                mView->KeyDown(mapKey(k));
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
