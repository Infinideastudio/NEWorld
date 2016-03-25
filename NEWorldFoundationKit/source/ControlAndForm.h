namespace NWUIK
{

    class NEWORLDFOUNDATIONKIT_API controls
    {
        public:
            bool mouseon, focused, pressed, enabled;
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
            controls();
            virtual ~controls();
            virtual void render();
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


}
