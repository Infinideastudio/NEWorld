#pragma once

#include "Definitions.h"
#include <NsCore/Ptr.h>
#include <NsGui/IView.h>
#include "NsGui/Grid.h"
#include "System/PmrBase.h"

namespace GUI {
    class FpsCounter
    {
    public:
        double getFPS() const noexcept { return mFPS; }
        void update() noexcept {
            check();
            frame();
        }
        void check() noexcept {
            if (timer() - mLastTimeInSec >= 1.0) {
                mFPS = mFPSCounter;
                mFPSCounter = 0;
                mLastTimeInSec = timer();
            }
        }
        void frame() noexcept { mFPSCounter++; }
    private:
        int mFPSCounter = 0;
        int mFPS = 0;
        double mLastTimeInSec = 0;
    };

    class Scene {
    public:
        Scene(const char* xaml, bool hasCursor = true) :
            mXamlPath(xaml), mHasCursor(hasCursor), mEnterTimeInSec(timer()) {}

        virtual ~Scene();

        void load();
        void singleLoop();

        void requestLeave() noexcept { mShouldLeave = true; }
        bool shouldLeave() const noexcept { return mShouldLeave; }

    protected:
        virtual void onRender() {}
        virtual void onUpdate() {}
        virtual void onLoad() {}
        virtual void onViewBinding() {}

        Noesis::Ptr<Noesis::Grid> mRoot;
        Noesis::Ptr<Noesis::IView> mView;

        FpsCounter mFPS;

    private:
        void render();
        void update();
        void loadView();

        bool mShouldLeave = false;
        const char* mXamlPath;
        bool mHasCursor;
        double mEnterTimeInSec;

        std::vector<std::shared_ptr<PmrBase>> mListeners;
    };

    void pushScene(std::unique_ptr<Scene> scene);

    void popScene();

    void clearScenes();

    void appStart();
}
