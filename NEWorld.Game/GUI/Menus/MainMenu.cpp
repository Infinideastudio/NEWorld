#include "Menus.h"
#include "TextRenderer.h"
#include "../GUI.h"
#include "NsRender/GLFactory.h"
#include "NsGui/Grid.h"
#include "NsGui/Button.h"
#include "GameView.h"
#include "Frustum.h"
#include "NsApp/NotifyPropertyChangedBase.h"

namespace Menus {

    class GameMenuViewModel : public NoesisApp::NotifyPropertyChangedBase {
    public:
        enum class State { MAIN_MENU, SETTINGS };

        const char* getState() const {
            switch (mState) {
            case State::MAIN_MENU: return "MainMenu";
            case State::SETTINGS: return "Settings";
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

    private:
        State mState = State::MAIN_MENU;

        NS_IMPLEMENT_INLINE_REFLECTION(GameMenuViewModel, NotifyPropertyChangedBase) {
            NsProp("State", &GameMenuViewModel::getState);
        }
    };

    class MainMenu : public GUI::Scene {
    public:
        MainMenu() : GUI::Scene("MainMenu.xaml"){}
    private:
        Noesis::Ptr<GameMenuViewModel> mViewModel;

        void onViewBinding() override {
            mViewModel = Noesis::MakePtr<GameMenuViewModel>();
            mRoot->SetDataContext(mViewModel);
            mRoot->FindName<Noesis::Button>("startGame")->Click() += [](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
                pushGameView();
            };
            mRoot->FindName<Noesis::Button>("settings")->Click() += [this](Noesis::BaseComponent*, const Noesis::RoutedEventArgs&) {
                mViewModel->setState(GameMenuViewModel::State::SETTINGS);
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
        return std::make_unique<Menus::MainMenu>();
    }
}
