#pragma once

#include "Definitions.h"
#include "Globalization.h"
#include <NsCore/Ptr.h>
#include <NsGui/IView.h>
#include "NsGui/Grid.h"

namespace GUI {
    class FpsCounter
    {
    public:
        double getFPS() const noexcept { return mFPS; }
        void update() noexcept {
            if (timer() - mLastTimeInSec >= 1.0) {
                mFPS = mFPSCounter;
                mFPSCounter = 0;
                mLastTimeInSec = timer();
            }
            mFPSCounter++;
        }
    private:
        int mFPSCounter = 0;
        int mFPS = 0;
        double mLastTimeInSec = 0;
    };

    class Scene {
    public:
        Scene(const char* xaml, bool hasCursor = true) :
            mXamlPath(xaml), mHasCursor(hasCursor) {}

        virtual ~Scene();

        void load();
        void singleLoop();

        void requestLeave() noexcept { mShouldLeave = true; }
        bool shouldLeave() const noexcept { return mShouldLeave; }

    protected:
        virtual void onRender() {}
        virtual void onUpdate() {}
        virtual void onLoad() {}

        Noesis::Ptr<Noesis::Grid> mRoot;
        Noesis::Ptr<Noesis::IView> mView;

    private:
        void render();
        void update();

        bool mShouldLeave = false;
        const char* mXamlPath;
        bool mHasCursor;
        double mLastRenderTimeInSec;
        FpsCounter mFPS;
    };

    void pushScene(std::unique_ptr<Scene> scene);

    void popScene();

    void clearScenes();

    void appStart();
}
