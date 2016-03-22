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

class NEWORLDFOUNDATIONKIT_API Form;
class NEWORLDFOUNDATIONKIT_API controls;

enum EVerticalAlignment
{
	Top, Bottom, Centre, Stretch
};

enum EHorizontalAlignment
{
	Left, Right, Centre, Stretch
};

enum SysSolidCB
{
    NWButtonGray
};

struct Rect
{
	double Left, Right, Top ,Bottom;
};

typedef Rect TMargin;
typedef void (controls::*CFC)(controls* sender, void * args);

class NEWORLDFOUNDATIONKIT_API Pen
{
private:
    double r, g, b, a, linewid;
public:
	void SerLineWidth(double Width){}
	void PaintLine(Rect Area){}
    Pen(double _r, double _g, double _b, double _a, double _lw = 1.0) :
        r(_r), g(_g), b(_b), a(_a) ,linewid(_lw){}
};

class NEWORLDFOUNDATIONKIT_API Brush
{
public:
	virtual void PaintArea(Rect Area){}
    virtual void PaintWithColorMask(double r, double g, double b, double a){}
};

class NEWORLDFOUNDATIONKIT_API SolidColorBrush
{
private:
	double r, g, b, a;
public:
	void PaintArea(Rect Area);
	void PaintBorder(Rect Area);
	SolidColorBrush(double _r, double _g, double _b, double _a) :
		r(_r), g(_g), b(_b), a(_a) {}
};

class NEWORLDFOUNDATIONKIT_API Trigger
{
public:
	CFC* HDCS = nullptr;
	int count = 0;
	Trigger &operator+=(const CFC &f){
        count++;
        CFC* swap = new CFC[count];
        for (int c = 0; c < count - 1; ++c) swap[c] = HDCS[c];
        swap[count - 1] = f;
        delete[]HDCS;
        HDCS = swap;
    }
}; 

class NEWORLDFOUNDATIONKIT_API controls
{
public:
    //Alignment and Margin
    EVerticalAlignment VerticalAlignment;
	EHorizontalAlignment HorizontalAlignment;
	TMargin Margin;
    int Width, Height;
	//Triggers
	Trigger TouchBegin, TouchUpdate, TouchEnd, Touch, DblTouch,
		EchoEnter, EchoLeave, GainFocus, LoseFocus, 
		KeyPress, KeyDown, KeyUp; 
	
    Form* parent; 
    void resize(EVerticalAlignment _Vertical, EHorizontalAlignment _Horizontal, TMargin _Margin, int _Width, int _Height);
    virtual ~controls() {}
    virtual void render() {}
};

class NEWORLDFOUNDATIONKIT_API label :public controls
{
public:
    string text;
    bool mouseon, focused;
    label() : mouseon(false), focused(false) {};
    label(string t,
          int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
    void update();
    void render();
};

class NEWORLDFOUNDATIONKIT_API button :public controls
{
public:
    string text;
    bool mouseon, focused, pressed, clicked, enabled;
    button() : mouseon(false), focused(false), pressed(false), clicked(false), enabled(false) {};
    button(string t,
           int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
    void update();
    void render();
};

class NEWORLDFOUNDATIONKIT_API trackbar :public controls
{
public:
    string text;
    int barwidth;
    int barpos;
    bool mouseon, focused, pressed, enabled;
    trackbar() : mouseon(false), focused(false), pressed(false), enabled(false) {};
    trackbar(string t, int w, int s,
             int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
    void update();
    void render();
};

class NEWORLDFOUNDATIONKIT_API textbox :public controls
{
public:
    string text;
    bool mouseon, focused, pressed, enabled;
    textbox() : mouseon(false), focused(false), pressed(false), enabled(false) {};
    textbox(string t,
            int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
    void update();
    void render();
};

class NEWORLDFOUNDATIONKIT_API vscroll :public controls
{
public:
    int barheight, barpos;
    bool mouseon, focused, pressed, enabled;
    bool defaultv, msup, msdown, psup, psdown;
    vscroll() : mouseon(false), focused(false), pressed(false), enabled(false) {};
    vscroll(int h, int s,
            int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
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
             int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
    void update();
    void render();
};

typedef void(*UIVoidF)();

class NEWORLDFOUNDATIONKIT_API Form
{
public:
    vector<controls*> children;
    bool tabp, shiftp, enterp, enterpl;
    bool upkp, downkp, upkpl, downkpl, leftkp, rightkp, leftkpl, rightkpl, backspacep, backspacepl, updated;
    int maxid, currentid, focusid, mx, my, mw, mb, mxl, myl, mwl, mbl;
    unsigned int displaylist;
    bool MouseOnTextbox;
    void Init();
    void registerControl(controls* c);
    void registerControls(int count, controls* c, ...);
    void update();
    void render();
    controls* getControlByID(int cid);
    void cleanup();
    virtual void onLoad() {}
    virtual void onUpdate() {}
    UIVoidF Background;
    virtual void onRender() {}
    virtual void onLeave() {}
    Form();
    void singleloop();
    ~Form();
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
		
        virtual Form* GetMain() { return new Form(); }
		
		Window();
		~Window();
};
}
#endif