#pragma once

#include "Definitions.h"
#include <memory>
#include <NsCore/Ptr.h>
#include <NsGui/IView.h>
#include <NsGui/Grid.h>
#include <kls/Object.h>
#include <kls/coroutine/Async.h>

namespace GUI {
    class FpsCounter {
    public:
        int getFPS() const noexcept { return mFPS; }
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
        kls::coroutine::ValueAsync<void> singleLoop();

        void requestLeave() noexcept { mShouldLeave = true; }
        bool shouldLeave() const noexcept { return mShouldLeave; }

    protected:
        virtual kls::coroutine::ValueAsync<void> onRender() { co_return; }
        virtual void onUpdate() {}
        virtual void onLoad() {}
        virtual void onViewBinding() {}

        Noesis::Ptr<Noesis::Grid> mRoot;
        Noesis::Ptr<Noesis::IView> mView;

        FpsCounter mFPS;

    private:
        kls::coroutine::ValueAsync<void> render();
        void update();
        void loadView();

        bool mShouldLeave = false;
        const char* mXamlPath;
        bool mHasCursor;
        double mEnterTimeInSec;

        std::vector<std::shared_ptr<kls::PmrBase>> mListeners;
    };

    void pushScene(std::unique_ptr<Scene> scene);

    void popScene();

    void clearScenes();

    void appStart();
}
