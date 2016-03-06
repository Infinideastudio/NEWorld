#pragma once
#include "Definitions.h"
#include "Globalization.h"

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
namespace GUI
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

class Form;
class controls
{
public:
    //控件基类，只要是控件都得继承这个
    virtual ~controls() {}
    int id, xmin, ymin, xmax, ymax;
    Form* parent;
    virtual void update() {} //莫非这个就是传说中的虚函数？
    virtual void render() {} //貌似是的！
    virtual void destroy() {}
    void updatepos();
    void resize(int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
private:
    int _xmin_r, _ymin_r, _xmax_r, _ymax_r;
    double _xmin_b, _ymin_b, _xmax_b, _ymax_b;
};

class label :public controls
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

class button :public controls
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

class trackbar :public controls
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

class textbox :public controls
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

class vscroll :public controls
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

class imagebox :public controls
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
class Form
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
void AppStart();
void PushPage(Form* View);
void PopPage();
void BackToMain();
void ClearStack();
Form* GetMain();
}