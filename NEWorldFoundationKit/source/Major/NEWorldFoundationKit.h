#ifdef NEWORLDFOUNDATIONKIT_EXPORTS
#define NEWORLDFOUNDATIONKIT_API __declspec(dllexport)
#else
#define NEWORLDFOUNDATIONKIT_API __declspec(dllimport)
#endif

#include "../Definitions.h"
#include "../Globalization.h"

// �����Ǵ� NEWorldFoundationKit.dll ������
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

//图形界面系统。。。正宗OOP！！！
namespace NWUIK
{
extern float linewidth;
extern float linealpha;
extern float FgR;
extern float FgG;
extern float FgB;
extern float FgA;
extern float BgR;
extern float BgG;
extern float BgB;
extern float BgA;

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

struct Rect
{
	double Left, Right, Top ,Bottom;
};

class NEWORLDFOUNDATIONKIT_API Brush
{
public:
	virtual void PaintArea(Rect Area){}
}

class NEWORLDFOUNDATIONKIT_API Pen
{
public:
	virtual void SerLineWidth(int Width){}
	virtual void PaintLine(Rect Area){}
}

class NEWORLDFOUNDATIONKIT_API SolidColorBrush
{
private:
	double r, g, b, a;
public:
	void PaintArea(Rect Area);
	void PaintBorder(Rect Area);
	SolidColorBrush(double _r, double _g, double _b, double _a) :
		r(_r), g(_g), b(_b), a(_a) {}
}

typedef Rect TMargin;
typedef void (controls::CFC*)(controls* sender, void * args);

class NEWORLDFOUNDATIONKIT_API Trigger
{
public:
	CFC* HDCS = nullptr;
	int count = 0;
	Trigger &operator+=(const CFC &f){
         v += a;
         return *this;
    }
}; 

class NEWORLDFOUNDATIONKIT_API controls
{
public:
    //Alignment and Margin
    EVerticalAlignment VerticalAlignment;
	EHorizontalAlignment HorizontalAlignment;
	TMargin Margin;
	
	//Triggers
	Trigger TouchBegin, TouchUpdate, TouchEnd, Touch, DblTouch,
		EchoEnter, EchoLeave, GainFocus, LoseFocus, 
		KeyPress, KeyDown, KeyUp; 
	
    virtual ~controls() {}
    Form* parent;
    virtual void render() {} //貌似是的！
    virtual void destroy() {}
    void updatepos();
    void resize(int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
};

class NEWORLDFOUNDATIONKIT_API label :public controls
{
public:
    //标签
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
    //按钮
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
    //该控件的中文名我不造
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
    //文本框
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
    //垂直滚动条
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
    //图片框
    float txmin, txmax, tymin, tymax;
    TextureID imageid;
    imagebox() : imageid(0) {};
    imagebox(float _txmin, float _txmax, float _tymin, float _tymax, TextureID iid,
             int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
    void update();
    void render();
};

typedef void(*UIVoidF)();

// 窗体 / 容器
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
		
		virtual GetMain(){return Form()}
		
		Window();
		~Window();
};

}
