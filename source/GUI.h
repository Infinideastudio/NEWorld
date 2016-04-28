/*
 * NEWorld: An free game with similar rules to Minecraft.
 * Copyright (C) 2016 NEWorld Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GUI_H
#define GUI_H

#include "Definitions.h"
#include "Globalization.h"

inline string BoolYesNo(bool b)
{
    return b ? Globalization::GetStrbyKey("NEWorld.yes") : Globalization::GetStrbyKey("NEWorld.no");
}

inline string BoolEnabled(bool b)
{
    return b ? Globalization::GetStrbyKey("NEWorld.enabled") : Globalization::GetStrbyKey("NEWorld.disabled");
}

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
class control
{
public:
    virtual ~control() {}
    int id, xmin, ymin, xmax, ymax;
    Form* parent;
    bool mouse_on();
    virtual void update() {}
    virtual void render() {}
    void updatepos();
    void resize(int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
private:
    int _xmin_r, _ymin_r, _xmax_r, _ymax_r;
    double _xmin_b, _ymin_b, _xmax_b, _ymax_b;
};

class label :public control
{
public:
    //标签
    string text;
    bool focused;
    label() : focused(false) {};
    label(string t,
          int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
    void update();
    void render();
};

class button :public control
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

class trackbar :public control
{
public:
    //该控件的中文名我不造
    string text;
    int barwidth, barpos;
    bool mouseon, focused, pressed, enabled;
    trackbar() : mouseon(false), focused(false), pressed(false), enabled(false) , 
        barwidth(0), barpos(0){};
    trackbar(string t, int w, int s,
             int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
    void update();
    void render();
};

class textbox :public control
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

class vscroll :public control
{
public:
    //垂直滚动条
    int barheight, barpos;
    bool mouseon, focused, pressed, enabled;
    bool defaultv, msup, msdown, psup, psdown;
    vscroll() : mouseon(false), focused(false), pressed(false), enabled(false),
        defaultv(false), msup(false), msdown(false), psup(false), psdown(false) ,
        barheight(0), barpos(0){};
    vscroll(int h, int s,
            int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
    void update();
    void render();
};

class imagebox :public control
{
public:
    //图片框
    float txmin, txmax, tymin, tymax;
    TextureID imageid;
    imagebox() : imageid(0),
        txmin(0.0f), txmax(0.0f), tymin(0.0f), tymax(0.0f){};
    imagebox(float _txmin, float _txmax, float _tymin, float _tymax, TextureID iid,
             int xi_r, int xa_r, int yi_r, int ya_r, double xi_b, double xa_b, double yi_b, double ya_b);
    void update();
    void render();
};

typedef void(*UIVoidF)();

inline int GetMouseScroll()
{
    return mw;
}
inline int GetMouseButton()
{
    return mb;
}

// 窗体 / 容器
class Form
{
public:
    vector<control*> children;
    bool tabp, shiftp, enterp, enterpl;
    bool upkp, downkp, upkpl, downkpl, leftkp, rightkp, leftkpl, rightkpl, backspacep, backspacepl, updated;
    int maxid, currentid, focusid, mx, my, mw, mb, mxl, myl, mwl, mbl;
    unsigned int displaylist;
    bool MouseOnTextbox;
    void registerControl(control* c);
    void registerControls(int count, control* c, ...);
    void update();
    void render();
    control* getControlByID(int cid);
    virtual void onLoad() {}
    virtual void onUpdate() {}
    UIVoidF Background;
    virtual void onRender() {}
    virtual void onLeave() {}
    Form() :bool tabp(false), shiftp(false), enterp(false), enterpl(false),
        upkp(false), downkp(false), upkpl(false), downkpl(false), leftkp(false), rightkp(false),
        leftkpl(false), rightkpl(false), backspacep(false), backspacepl(false), updated(false),
        mx(0), my(0), mw(0), mb(0), mxl(0), myl(0), mwl(0), mbl(0),displaylist(0),
        MouseOnTextbox(false), maxid(0), currentid(0), focusid(-1), Background(&drawBackground)
    {
        //Transition forward
        if (transitionList) glDeleteLists(transitionList, 1);
        transitionList = lastdisplaylist;
        transitionForward = true;
        transitionTimer = timer();
    };
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

#endif
