#pragma once
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

//Graphical User Interface System
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
		int barwidth;
		int barpos;
		bool mouseon, focused, pressed, enabled;
		trackbar() : mouseon(false), focused(false), pressed(false), enabled(false) {};
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
		vscroll() : mouseon(false), focused(false), pressed(false), enabled(false) {};
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
		imagebox() : imageid(0) {};
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
		Form() :maxid(0), currentid(0), focusid(-1), Background(&drawBackground)
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