#ifndef NEWORLDFOUNDATIONKIT_H
#define NEWORLFFOUNDATIONKIT_H

#include "Definitions.h"
#include "Globalization.h"

#ifdef NEWORLDFOUNDATIONKIT_EXPORTS
#define NEWORLDFOUNDATIONKIT_API __declspec(dllexport)
#else
#define NEWORLDFOUNDATIONKIT_API __declspec(dllimport)
#endif


extern int getMouseButton();
extern int getMouseScroll();
inline string BoolYesNo(bool b)
{
    return b ? Globalization::GetStrbyKey("NEWorld.yes") : Globalization::GetStrbyKey("NEWorld.no");
}
inline string BoolEnabled(bool b)
{
    return b ? Globalization::GetStrbyKey("NEWorld.enabled") : Globalization::GetStrbyKey("NEWorld.disabled");
}
template<typename T>
inline string strWithVar(string str, T var)
{
    std::stringstream ss;
    ss << str << var;
    return ss.str();
}
template<typename T>
inline string Var2Str(T var)
{
    std::stringstream ss;
    ss << var;
    return ss.str();
}

#include "BasicDefinitions.h"
#incldue "Brush.h"
#include "Pen.h"
#include "Trigger.h"

namespace NWUIK
{
    extern int nScreenWidth;
    extern int nScreenHeight;
    extern unsigned int transitionList;
    extern unsigned int lastdisplaylist;
    extern double transitionTimer;
    extern bool transitionForward;

    void clearTransition();
    void screenBlur();
    void drawBackground();
    void InitStretch();
    void EndStretch();


    class NEWORLDFOUNDATIONKIT_API label :public controls
    {
        public:
            string text;
            label() {};
            label(string t,
                  int EVerticalAlignment _Vertical, EHorizontalAlignment _Horizontal, TMargin _Margin, int _Width, int _Height);
            void update();
            void render();
    };

    class NEWORLDFOUNDATIONKIT_API button :public controls
    {
        public:
            string text;
            button() {};
            button(string t,
                   int EVerticalAlignment _Vertical, EHorizontalAlignment _Horizontal, TMargin _Margin, int _Width, int _Height);
            void update();
            void render();
    };

    class NEWORLDFOUNDATIONKIT_API trackbar :public controls
    {
        public:
            string text;
            int barwidth;
            int barpos;
            trackbar() {};
            trackbar(string t, int w, int s,
                     int EVerticalAlignment _Vertical, EHorizontalAlignment _Horizontal, TMargin _Margin, int _Width, int _Height);
            void update();
            void render();
    };

    class NEWORLDFOUNDATIONKIT_API textbox :public controls
    {
        public:
            string text;
            textbox() {};
            textbox(string t,
                    int EVerticalAlignment _Vertical, EHorizontalAlignment _Horizontal, TMargin _Margin, int _Width, int _Height);
            void update();
            void render();
    };

    class NEWORLDFOUNDATIONKIT_API vscroll :public controls
    {
        public:
            int barheight, barpos;
            bool defaultv, msup, msdown, psup, psdown;
            vscroll() {};
            vscroll(int h, int s,
                    int EVerticalAlignment _Vertical, EHorizontalAlignment _Horizontal, TMargin _Margin, int _Width, int _Height);
            void update();
            void render();
    };

    class NEWORLDFOUNDATIONKIT_API imagebox :public controls
    {
        public:
            float txmin, txmax, tymin, tymax;
            TextureID imageid;
            imagebox() : imageid(0) {};
            imagebox(float _txmin, float _txmax, float _tymin, float _tymax, TextureID iid,
                     int EVerticalAlignment _Vertical, EHorizontalAlignment _Horizontal, TMargin _Margin, int _Width, int _Height);
            void update();
            void render();
    };

    struct PageOpRq
    {
        int Op; //1 is push ,2 is pop ,3 is back to main;
        Form* Page;
    };

    class Window
    {
        private:
            std::deque<Form*> ViewStack;
            std::deque<PageOpRq> ViewOps = {};
            bool HaveRequest = false;
        public:
            void AppStart();
            void PushPage(Form* View);
            void PopPage();
            void BackToMain();
            void ClearStack();

            virtual Form* GetMain()
            {
                return new Form();
            }

            Window();
            ~Window();
    };
}
#endif
