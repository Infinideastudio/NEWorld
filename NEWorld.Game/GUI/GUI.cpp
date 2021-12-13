#include <deque>
#include "GUI.h"
#include "TextRenderer.h"
#include <NsRender/GLFactory.h>
#include <NoesisPCH.h>
#include "Shader.h"

namespace GUI {

    void Scene::update() {
        // TODO: change to use glfw callback + message bus?
        if (mView) {
            double xpos, ypos;
            glfwGetCursorPos(MainWindow, &xpos, &ypos);
            mView->MouseMove(xpos, ypos);
            mView->SetSize(windowwidth, windowheight);
            int state = glfwGetMouseButton(MainWindow, GLFW_MOUSE_BUTTON_LEFT);
            if (state == GLFW_PRESS) {
                mView->MouseButtonDown(xpos, ypos, Noesis::MouseButton_Left);
            }
            else {
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
            // Offscreen rendering phase populates textures needed by the on-screen rendering
            mView->GetRenderer()->UpdateRenderTree();
            mView->GetRenderer()->RenderOffscreen();
            // Rendering is done in the active framebuffer
            mView->GetRenderer()->Render();
            glPopAttrib();
        }
    }

    Scene::~Scene() {

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
        TextRenderer::setFontColor(1.0, 1.0, 1.0, 1.0);

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
