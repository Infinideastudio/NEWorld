#include "Menus.h"
#include "TextRenderer.h"
#include "../GUI.h"
#include "NsRender/GLFactory.h"
#include "NsGui/Grid.h"
#include "NsGui/Button.h"
#include "GameView.h"
#include "Frustum.h"
#include "NsApp/NotifyPropertyChangedBase.h"
#include "Renderer.h"
#include "NsGui/ObservableCollection.h"
#include <filesystem>
#include "Universe/World/Chunk.h"
#include "NsGui/TextBox.h"
#include "NsGui/ListView.h"

namespace Menus {
    class WorldModel : public NoesisApp::NotifyPropertyChangedBase
    {
    public:
        WorldModel(std::string name) : mName(std::move(name)) {}
        const char* name() const { return mName.c_str(); }

    private:
        std::string mName;

        NS_IMPLEMENT_INLINE_REFLECTION(WorldModel, NotifyPropertyChangedBase) {
            NsProp("Name", &WorldModel::name);
        }
    };

    class GameMenuViewModel : public NoesisApp::NotifyPropertyChangedBase {
    public:
        GameMenuViewModel() {
            for (auto&& x : std::filesystem::directory_iterator("./Worlds/")) {
                if (is_directory(x)) {
                    mWorlds.Add(Noesis::MakePtr<WorldModel>(x.path().filename().string()));
                }
            }
        }
        enum class State { MAIN_MENU, SETTINGS, SELECT_WORLD };
        const char* getState() const {
            switch (mState) {
            case State::MAIN_MENU: return "MainMenu";
            case State::SETTINGS: return "Settings";
            case State::SELECT_WORLD: return "SelectWorld";
            default: assert(false);
            }
            return nullptr;
        }
        void setState(State state) noexcept {
            if(mState!=state) {
                mState = state;
                OnPropertyChanged("State");
            }
        }

#define SETTING_ITEM(TYPE, MEMBER_NAME, PROP_NAME) \
        TYPE get##PROP_NAME() const noexcept { return MEMBER_NAME; }\
        void set##PROP_NAME(TYPE value) noexcept {\
            if (value == MEMBER_NAME) return;\
            MEMBER_NAME = value;\
            OnPropertyChanged(#PROP_NAME);\
        }

        SETTING_ITEM(int, mFOV, FOV);
        SETTING_ITEM(float, mMouseSensitivity, MouseSensitivity);
        SETTING_ITEM(bool, mNiceGrass, NiceGrass);
        SETTING_ITEM(bool, mSmoothLighting, SmoothLighting);
        SETTING_ITEM(bool, mVSync, VSync);
        SETTING_ITEM(bool, mShadows, Shadows);

#undef SETTING_ITEM

        int getRenderDistance() const { return mRenderDistance; }
        int getRenderDistanceTick() const noexcept { return mRenderDistanceTick; }
        void setRenderDistanceTick(int renderDistanceTick) noexcept {
            if (mRenderDistanceTick != renderDistanceTick) {
                mRenderDistanceTick = renderDistanceTick;
                mRenderDistance = 1 << renderDistanceTick; // 2^renderDistanceTick
                OnPropertyChanged("RenderDistanceTick");
                OnPropertyChanged("RenderDistance");
            }
        }

        const Noesis::ObservableCollection<WorldModel>* getWorlds() const {
            return &mWorlds;
        }

    private:
        State mState = State::MAIN_MENU;
        float& mFOV = FOVyNormal;
        float& mMouseSensitivity = mousemove;
        int& mRenderDistance = viewdistance;
        int mRenderDistanceTick = int(std::log2(viewdistance));
        bool& mNiceGrass = NiceGrass;
        bool& mVSync = vsync;
        bool& mSmoothLighting = SmoothLighting;
        bool& mShadows  = Renderer::AdvancedRender;

        Noesis::ObservableCollection<WorldModel> mWorlds;

        NS_IMPLEMENT_INLINE_REFLECTION(GameMenuViewModel, NotifyPropertyChangedBase) {
            NsProp("State", &GameMenuViewModel::getState);
            // settings 
            NsProp("FOV", &GameMenuViewModel::getFOV, &GameMenuViewModel::setFOV);
            NsProp("RenderDistanceTick", &GameMenuViewModel::getRenderDistanceTick, &GameMenuViewModel::setRenderDistanceTick);
            NsProp("RenderDistance", &GameMenuViewModel::getRenderDistance);
            NsProp("MouseSensitivity", &GameMenuViewModel::getMouseSensitivity, &GameMenuViewModel::setMouseSensitivity);
            NsProp("NiceGrass", &GameMenuViewModel::getNiceGrass, &GameMenuViewModel::setNiceGrass);
            NsProp("VSync", &GameMenuViewModel::getVSync, &GameMenuViewModel::setVSync);
            NsProp("SmoothLighting", &GameMenuViewModel::getSmoothLighting, &GameMenuViewModel::setSmoothLighting);
            NsProp("Shadows", &GameMenuViewModel::getShadows, &GameMenuViewModel::setShadows);
            // Select World
            NsProp("Worlds", &GameMenuViewModel::getWorlds);
        }
    };

    class MainMenu : public GUI::Scene {
    public:
        MainMenu() : Scene("MainMenu.xaml"){}
    private:
        Noesis::Ptr<GameMenuViewModel> mViewModel;

        void onViewBinding() override {
            mViewModel = Noesis::MakePtr<GameMenuViewModel>();
            mRoot->SetDataContext(mViewModel);
            mRoot->FindName<Noesis::Button>("startGame")->Click() += [this](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
                mViewModel->setState(GameMenuViewModel::State::SELECT_WORLD);
            };
            mRoot->FindName<Noesis::Button>("settings")->Click() += [this](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
                mViewModel->setState(GameMenuViewModel::State::SETTINGS);
            };
            mRoot->FindName<Noesis::Button>("Save")->Click() += [this](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
                mViewModel->setState(GameMenuViewModel::State::MAIN_MENU);
            };
            mRoot->FindName<Noesis::Button>("Back")->Click() += [this](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
                mViewModel->setState(GameMenuViewModel::State::MAIN_MENU);
            };
            mRoot->FindName<Noesis::Button>("Create")->Click() += [this](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
                World::worldname = mRoot->FindName<Noesis::TextBox>("NewWorldNameTextBox")->GetText();
                pushGameView();
            };
            mRoot->FindName<Noesis::Button>("Play")->Click() += [this](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
                World::worldname = static_cast<WorldModel*>(
                    mRoot->FindName<Noesis::ListView>("WorldList")->GetSelectedItem()
                )->name();
                pushGameView();
            };
            mRoot->FindName<Noesis::Button>("exit")->Click() += [this](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
                requestLeave();
            };

        }

        void onRender() override {
            drawBackground();
        }

        void drawBackground() {
            static Frustum frus;
            static auto startTimer = timer();
            const auto elapsed = timer() - startTimer;
            frus.LoadIdentity();
            frus.SetPerspective(90.0f, static_cast<float>(windowwidth) / windowheight, 0.1f, 10.0f);

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glMultMatrixf(frus.getProjMatrix());
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glRotated(elapsed * 4.0, 0.1, 1.0, 0.1);
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDepthFunc(GL_LEQUAL);
            glDisable(GL_CULL_FACE);
            glEnable(GL_TEXTURE_2D);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

            //Begin to draw a cube
            glBindTexture(GL_TEXTURE_2D, tex_mainmenu[0]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-1.0f, 1.0f, -1.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, -1.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(1.0f, -1.0f, -1.0f);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-1.0f, -1.0f, -1.0f);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, tex_mainmenu[1]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, -1.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, 1.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(1.0f, -1.0f, 1.0f);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(1.0f, -1.0f, -1.0f);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, tex_mainmenu[2]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, 1.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(-1.0f, 1.0f, 1.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(-1.0f, -1.0f, 1.0f);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(1.0f, -1.0f, 1.0f);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, tex_mainmenu[3]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-1.0f, 1.0f, 1.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(-1.0f, 1.0f, -1.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(-1.0f, -1.0f, -1.0f);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-1.0f, -1.0f, 1.0f);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, tex_mainmenu[4]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-1.0f, -1.0f, 1.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(-1.0f, -1.0f, -1.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(1.0f, -1.0f, -1.0f);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(1.0f, -1.0f, 1.0f);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, tex_mainmenu[5]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, 1.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, -1.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(-1.0f, 1.0f, -1.0f);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-1.0f, 1.0f, 1.0f);
            glEnd();
        }
    };

    std::unique_ptr<GUI::Scene> startMenu() {
        return std::make_unique<MainMenu>();
    }
}
