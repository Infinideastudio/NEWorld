#include "ControlAndForm.h"
namespace NWUIK
{
    void controls::resize(EVerticalAlignment _Vertical, EHorizontalAlignment _Horizontal, TMargin _Margin, int _Width, int _Height)
    {
        VerticalAlignment = _Vertical;
        HorizontalAlignment = _Horizontal;
        Margin = _Margin;
        Width = _Width;
        Height = _Height;
    }

    controls::controls(): mouseon(false), focused(false), pressed(false), enabled(true)
    {

    }

    controls::~controls()
    {

    }

    void controls::render()
    {

    }

    void Form::Init()
    {
        maxid = 0;
        currentid = 0;
        focusid = -1;
        //Transition forward
        if (transitionList != 0) glDeleteLists(transitionList, 1);
        transitionList = lastdisplaylist;
        transitionForward = true;
        transitionTimer = timer();
    }

    void Form::registerControl(controls* c)
    {
        c->id = currentid;
        c->parent = this;
        children.push_back(c);
        currentid++;
        maxid++;
    }

    void Form::registerControls(int count, controls* c, ...)
    {
        va_list arg_ptr;
        controls* cur = c;
        va_start(arg_ptr, c);
        for (int i = 0; i < count; i++)
        {
            registerControl(cur);
            cur = va_arg(arg_ptr, controls*);
        }
        va_end(arg_ptr);
    }

    void Form::update()
    {
        updated = false;
        bool lMouseOnTextbox = MouseOnTextbox;
        MouseOnTextbox = false;

        if (glfwGetKey(MainWindow, GLFW_KEY_TAB) == GLFW_PRESS)                               //TAB键切换焦点
        {
            if (glfwGetKey(MainWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(MainWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)     //Shift+Tab
            {
                updated = true;
                if (!tabp) focusid--;
                if (focusid == -2) focusid = maxid - 1;                                //到了最前一个ID
            }
            else
            {
                updated = true;
                if (!tabp) focusid++;
                if (focusid == maxid + 1) focusid = -1;                              //到了最后一个ID
            }
            tabp = true;
        }
        if (glfwGetKey(MainWindow, GLFW_KEY_TAB) != GLFW_PRESS) tabp = false;
        if (!(glfwGetKey(MainWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(MainWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)) shiftp = false;

        enterpl = enterp;
        if (glfwGetKey(MainWindow, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            updated = true;
            enterp = true;
        }
        if (!(glfwGetKey(MainWindow, GLFW_KEY_ENTER) == GLFW_PRESS)) enterp = false;

        upkpl = upkp;                                                              //方向键上
        if (glfwGetKey(MainWindow, GLFW_KEY_UP) == GLFW_PRESS)
        {
            updated = true;
            upkp = true;
        }
        if (!(glfwGetKey(MainWindow, GLFW_KEY_UP) == GLFW_PRESS)) upkp = false;

        downkpl = downkp;                                                          //方向键下
        if (glfwGetKey(MainWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            downkp = true;
        }
        if (!(glfwGetKey(MainWindow, GLFW_KEY_DOWN) == GLFW_PRESS)) downkp = false;

        leftkpl = leftkp;                                                          //方向键左
        if (glfwGetKey(MainWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            leftkp = true;
        }
        if (!(glfwGetKey(MainWindow, GLFW_KEY_LEFT) == GLFW_PRESS)) leftkp = false;
        rightkpl = rightkp;                                                        //方向键右
        if (glfwGetKey(MainWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            rightkp = true;
        }
        if (glfwGetKey(MainWindow, GLFW_KEY_RIGHT) != GLFW_PRESS) rightkp = false;

        backspacepl = backspacep;
        if (glfwGetKey(MainWindow, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
            backspacep = true;
        else
            backspacep = false;

        if (mb == 1 && mbl == 0) focusid = -1;                                   //空点击时使焦点清空

        for (size_t i = 0; i != children.size(); i++)
        {
            children[i]->updatepos();
            children[i]->update();                                               //更新子控件
        }

        if (!lMouseOnTextbox && MouseOnTextbox)
        {
            glfwDestroyCursor(MouseCursor);
            MouseCursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
            glfwSetCursor(MainWindow, MouseCursor);
        }
        if (lMouseOnTextbox && !MouseOnTextbox)
        {
            glfwDestroyCursor(MouseCursor);
            MouseCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
            glfwSetCursor(MainWindow, MouseCursor);
        }
        onUpdate();

    }

    void Form::render()
    {
        if (Background) Background();

        double TimeDelta = timer() - transitionTimer;
        float transitionAnim = (float)(1.0 - pow(0.8, TimeDelta*60.0) + pow(0.8, 0.3*60.0) / 0.3 * TimeDelta);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, windowwidth, windowheight, 0, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glDepthFunc(GL_ALWAYS);
        glLoadIdentity();
        if (GUIScreenBlur) screenBlur();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LINE_SMOOTH);

        if (TimeDelta <= 0.3 && transitionList != 0)
        {
            if (transitionForward) glTranslatef(-transitionAnim * windowwidth, 0.0f, 0.0f);
            else glTranslatef(transitionAnim * windowwidth, 0.0f, 0.0f);
            glCallList(transitionList);
            glLoadIdentity();
            if (transitionForward) glTranslatef(windowwidth - transitionAnim * windowwidth, 0.0f, 0.0f);
            else glTranslatef(transitionAnim * windowwidth- windowwidth, 0.0f, 0.0f);
        }
        else if (transitionList != 0)
        {
            glDeleteLists(transitionList, 1);
            transitionList = 0;
        }

        if (displaylist == 0) displaylist = glGenLists(1);
        glNewList(displaylist, GL_COMPILE_AND_EXECUTE);
        for (size_t i = 0; i != children.size(); i++)
        {
            children[i]->render();
        }
        onRender();
        glEndList();
        lastdisplaylist = displaylist;
    }

    void Form::onLoad()
    {
        
    }
    void oForm::nUpdate()
    {
        
    }

    void Form::onRender()
    {
        
    }
    
    void Form::onLeave()
    {
        
    }
};
