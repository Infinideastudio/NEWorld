#ifndef _UILIB_
#define _UILIB_

#if defined _WIN32 || defined __CYGWIN__
    #define UILIB_WIN32
#endif

#ifdef _MSC_VER
    #define UILIB_MSVC
#endif

#ifdef UILIB_WIN32
    #ifdef UILIB_MSVC
        #pragma warning(disable: 4251)
        #pragma warning(disable: 4244)
        #pragma warning(disable: 4996)
        #define UILIB_API __declspec(dllexport)
    #else
        #define UILIB_API __attribute__((dllexport))
    #endif
#else
#define UILIB_API __attribute__((visibility("default")))
#endif

//STL Headers
#include <map>
#include <unordered_map>
#include <stack>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>
#include <climits>
#include <random>
#include <initializer_list>

//Boost Headers
#include <boost/date_time.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/home/support/detail/hold_any.hpp>

//Headers from external librarys
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

std::vector<std::string> split(const std::string& src, std::string separate_character);
void trim(std::string& s);
std::string strtolower(const std::string& s);

namespace UI
{
    namespace Math
    {
        class UILIB_API Vec2
        {
        public:
            double x, y;
            Vec2() = default;
            Vec2(double _x, double _y);
            Vec2(const Vec2& from, const Vec2& to);
            bool operator == (const Vec2& v) const;
            bool operator != (const Vec2& v) const;
            Vec2 operator + (const Vec2& v) const;
            Vec2 operator - (const Vec2& v) const;
            Vec2 operator * (const double d) const;
            Vec2& set(const Vec2& to);
            Vec2& set(const double _x, const double _y);
        };

        class UILIB_API Vec3
        {
        public:
            double x, y, z;
            Vec3() = default;
            Vec3(double _x, double _y, double _z);
            Vec3(const Vec3& from, const Vec3& to);
            Vec3 operator - (const Vec3& b) const;
            Vec3 operator + (const Vec3& b) const;
            Vec3 operator * (const double b) const;
            Vec3 operator * (const Vec3& b) const; //Cross Product
            Vec3& operator -= (const Vec3& b);
            Vec3& operator += (const Vec3& b);
            Vec3& operator *= (const double b);
            Vec3& operator *= (const Vec3& b); //Cross Product
            Vec3& zero();
            Vec3& set(const Vec3& to);
            Vec3& set(const double _x, const double _y, const double _z);
            double length() const;
            Vec3 unit() const;
            double dot(const Vec3& b) const;
        };

        class UILIB_API Vec4
        {
        public:
            double x, y, z, t;
            Vec4() = default;
            Vec4(double _x, double _y, double _z, double _t);
            Vec4(const Vec4& from, const Vec4& to);
            Vec4 operator - (const Vec4& b) const;
            Vec4 operator + (const Vec4& b) const;
            Vec4 operator * (const double b) const;
            Vec4& operator -= (const Vec4& b);
            Vec4& operator += (const Vec4& b);
            Vec4& operator *= (const double b);
            Vec4& zero();
            Vec4& set(const Vec4& to);
            Vec4& set(const double _x, const double _y, const double _z, const double _t);
            double length() const;
            Vec4 unit() const;
            double dot(const Vec4& b) const;
        };
    }
    namespace Core
    {
        extern SDL_Renderer* currenderer;
    }
    namespace Base
    {
        using Color = Math::Vec4;
        UILIB_API Color blend(const Color& lhs, const Color& rhs);
        class UILIB_API Rect
        {
        public:
            double xmin, xmax, ymin, ymax;
            Rect() = default;
            Rect(Math::Vec2 p1, Math::Vec2 p2);
            Rect(double _xmin, double _xmax, double _ymin, double _ymax);
            void set(double _xmin, double _xmax, double _ymin, double _ymax);
        };

        class UILIB_API Texture
        {
        public:
            Texture() = default;
            Texture(std::string path);
            ~Texture();
        private:
            SDL_Texture * texture = nullptr;
        };

        class UILIB_API Image
        {
        public:
            Image();
            Image(Image& img);
            Image(std::string path);
//            Image Sub(Rect range);
            ~Image();
        private:
            std::shared_ptr<Texture> tex;
        };

        class UILIB_API Brush
        {
        public:
            virtual Color sample(double x, double y) = 0;
            virtual ~Brush() = default;
        };

        UILIB_API void init();
    }

    namespace Logger
    {
        enum class Level : size_t
        {
            trace, debug, info, warning, error, fatal
        };

        class UILIB_API Logger
        {
        public:
            Logger() = default;
            Logger(const std::string& path);
            Logger(const std::string& path, Level _clogLevel, Level _cerrLevel, Level _fileLevel, Level _lineLevel);

            void log(Level level, const std::string& message, const char* fileName, int lineNumber);
            void dump();

            template <typename T>
            Logger& operator<< (const T& rhs)
            {
                m_content << rhs;
                return *this;
            }
        private:
            Level clogLevel, cerrLevel, fileLevel, lineLevel;
            std::string logpath;
            std::stringstream m_content;
        };

        UILIB_API extern Logger service;
        UILIB_API void init(const char* path);

#define logdebug(x)    UI::Logger::service.log(UI::Logger::Level::debug  , x, __FUNCTION__, __LINE__)
#define loginfo(x)     UI::Logger::service.log(UI::Logger::Level::info   , x, __FUNCTION__, __LINE__)
#define logwarning(x)  UI::Logger::service.log(UI::Logger::Level::warning, x, __FUNCTION__, __LINE__)
#define logerror(x)    UI::Logger::service.log(UI::Logger::Level::error  , x, __FUNCTION__, __LINE__)
#define logfatal(x)    UI::Logger::service.log(UI::Logger::Level::fatal  , x, __FUNCTION__, __LINE__)
    }

    namespace Data
    {
        //Some Basic Convertions with String
        bool isDecimal(const std::string& str);
        bool isInteger(const std::string& str);
        bool isBoolean(const std::string& str);
        bool isString(const std::string& str);
        double getDecimal(const std::string& str);
        int getInteger(const std::string& str);
        bool getBoolean(const std::string& str);
        const std::string getString(const std::string& str);

        namespace Convert
        {
            boost::spirit::hold_any StrToAny(const std::string& str);
            std::string ToString(boost::spirit::hold_any var);
        }

        using AnyTMap = std::map<std::string, boost::spirit::hold_any>;
        enum class SimpleMapFileExceptions : size_t
        {
            IOFailureOnRead, IOFailureOnWrite
        };

        class SimpleMapFile
        {
        public:
            SimpleMapFile(const std::string& filename);
            void save();
            const AnyTMap& getMap();
            boost::spirit::hold_any get(const std::string& key) const;
            bool has(const std::string& key) const;
            void set(const std::string& key, const boost::spirit::hold_any value);
            template<class T>
            T get(const std::string& key, T defaultValue = T()) const
            {
                try
                {
                    auto result = m_settings.find(key);
                    return (result != m_settings.end()) ? result->second.cast<T>() : defaultValue;
                }
                catch(boost::spirit::bad_any_cast e)
                {
                    logerror("boost bad cast:" + std::string(e.from) + " to " + std::string(e.to));
                    UI::Logger::service.dump();
                }

                return T();
            }
        private:
            AnyTMap m_settings;
            std::string m_filename;
        };
    }

    namespace Core
    {
        enum class Exceptions : size_t
        {
            WindowCreationFailure,
            VideoInitFailure,
            AudioInitFailure
        };

        enum StatChange
        {
            Minimize, Maximize, Restore
        };

        enum FocusOp
        {
            Lose, Gain
        };

        enum ButtonAction
        {
            Release = 0, Press, Repeat
        };

        enum MouseButton
        {
            Left, Middle, Right, Preserved1, Preserved2
        };

        enum CursorOp
        {
            Leave, Enter
        };

        enum class VerticalAlignment : size_t
        {
            Top, Bottom, Center, Stretch
        };

        enum class HorizontalAlignment : size_t
        {
            Left, Right, Center, Stretch
        };

        class Grid;

        //relativeps: percentage relativepc:position
        class UILIB_API Margin
        {
        public:
            Margin() = default;
            Margin(Base::Rect _relative_ps, Base::Rect _relative_pc, HorizontalAlignment _ha, VerticalAlignment _va);
            void updateAbs(Base::Rect Parent_Rect);
            Base::Rect absrect;
            //Quick Constructors
            inline static auto TopLeft(double TopPct, double LeftPct, double TopDis, double LeftDis, double Width, double Height)
            {
                return Margin(Base::Rect(LeftPct, 0.0, TopPct, 0.0), Base::Rect(LeftDis, Width, TopDis, Height), HorizontalAlignment::Left, VerticalAlignment::Top);
            }
            inline static auto TopRight(double TopPct, double RightPct, double TopDis, double RightDis, double Width, double Height)
            {
                return Margin(Base::Rect(0.0, RightPct, TopPct, 0.0), Base::Rect(Width, RightDis, TopDis, Height), HorizontalAlignment::Right, VerticalAlignment::Top);
            }
            inline static auto TopCenter(double TopPct, double TopDis, double Height, double CenterLeft, double CenterRight)
            {
                return Margin(Base::Rect(0.0, 0.0, TopPct, 0.0), Base::Rect(CenterLeft, CenterRight, TopDis, Height), HorizontalAlignment::Center, VerticalAlignment::Top);
            }
            inline static auto TopStretch(double TopPct, double LeftPct, double RightPct, double TopDis, double Height, double LeftDis, double RightDis)
            {
                return Margin(Base::Rect(LeftPct, RightPct, TopPct, 0.0), Base::Rect(LeftDis, RightDis, TopDis, Height), HorizontalAlignment::Stretch, VerticalAlignment::Top);
            }
            inline static auto BottomLeft(double LeftPct, double BottomPct, double LeftDis, double Width, double BottomDis, double Height)
            {
                return Margin(Base::Rect(LeftPct, 0.0, 0.0, BottomPct), Base::Rect(LeftDis, Width, Height, BottomDis), HorizontalAlignment::Left, VerticalAlignment::Bottom);
            }
            inline static auto BottomRight(double RightPct, double BottomPct, double RightDis, double Width, double BottomDis, double Height)
            {
                return Margin(Base::Rect(0.0, RightPct, 0.0, BottomPct), Base::Rect(Width, RightDis, Height, BottomDis), HorizontalAlignment::Right, VerticalAlignment::Bottom);
            }
            inline static auto BottomCenter(double BottomPct, double CenterLeft, double CenterRight, double BottomDis, double Height)
            {
                return Margin(Base::Rect(0.0, 0.0, 0.0, BottomPct), Base::Rect(CenterLeft, CenterRight, Height, BottomDis), HorizontalAlignment::Center, VerticalAlignment::Bottom);
            }
            inline static auto BottomStretch(double LeftPct, double RightPct, double BottomPct, double  LeftDis, double RightDis, double BottomDis, double Height)
            {
                return Margin(Base::Rect(LeftPct, RightPct, 0.0, BottomPct), Base::Rect(LeftDis, RightDis, Height, BottomDis), HorizontalAlignment::Stretch, VerticalAlignment::Bottom);
            }
            inline static auto CenterLeft(double LeftPct, double LeftDis, double Width, double CenterTop, double CenterBottom)
            {
                return Margin(Base::Rect(LeftPct, 0.0, 0.0, 0.0), Base::Rect(LeftDis, Width, CenterTop, CenterBottom), HorizontalAlignment::Left, VerticalAlignment::Center);
            }
            inline static auto CenterRight(double RightPct, double RightDis, double Width, double CenterTop, double  CenterBottom)
            {
                return Margin(Base::Rect(0.0, RightPct, 0.0, 0.0), Base::Rect(Width, RightDis, CenterTop, CenterBottom), HorizontalAlignment::Right, VerticalAlignment::Center);
            }
            inline static auto CenterCenter(double CenterLeft, double CenterRight, double CenterTop, double CenterBottom)
            {
                return Margin(Base::Rect(0.0, 0.0, 0.0, 0.0), Base::Rect(CenterLeft, CenterRight, CenterTop, CenterBottom), HorizontalAlignment::Center, VerticalAlignment::Center);
            }
            inline static auto CenterStretch(double LeftPct, double RightPct, double LeftDis, double RightDis, double CenterTop, double CenterBottom)
            {
                return Margin(Base::Rect(LeftPct, RightPct, 0.0, 0.0), Base::Rect(LeftDis, RightDis, CenterTop, CenterBottom), HorizontalAlignment::Stretch, VerticalAlignment::Center);
            }
            inline static auto StretchLeft(double LeftPct, double TopPct, double BottomPct, double LeftDis, double Width, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(LeftPct, 0.0, TopPct, BottomPct), Base::Rect(LeftDis, Width, TopDis, BottomDis), HorizontalAlignment::Left, VerticalAlignment::Stretch);
            }
            inline static auto StretchRight(double RightPct, double TopPct, double BottomPct, double RightDis, double Width, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(0.0, RightPct, TopPct, BottomPct), Base::Rect(Width, RightDis, TopDis, BottomDis), HorizontalAlignment::Right, VerticalAlignment::Stretch);
            }
            inline static auto StretchCenter(double TopPct, double BottomPct, double CenterLeft, double CenterRight, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(0.0, 0.0, TopPct, BottomPct), Base::Rect(CenterLeft, CenterRight, TopDis, BottomDis), HorizontalAlignment::Center, VerticalAlignment::Stretch);
            }
            inline static auto StretchStretch(double LeftPct, double RightPct, double TopPct, double BottomPct, double LeftDis, double RightDis, double TopDis, double BottomDis)
            {
                return Margin(Base::Rect(LeftPct, RightPct, TopPct, BottomPct), Base::Rect(LeftDis, RightDis, TopDis, BottomDis), HorizontalAlignment::Stretch, VerticalAlignment::Stretch);
            }
        private:
            HorizontalAlignment ha;
            VerticalAlignment va;
            Base::Rect relativeps, relativepc;
        };

        class UILIB_API Control
        {
        public:
            std::string xName;  //ID
            Margin cMargin;     //Position Data
            size_t gridPosX, gridPosY;

            Control() = default; //Creator
            Control(std::string _xName, Margin _Margin);

            virtual void setSize(size_t x, size_t y);
            virtual void parentResize(Grid* parent);
            //Event Handlers
            virtual void render();
            virtual void onResize(size_t x, size_t y);
            virtual void onParentResize(Grid* parent);
            virtual void focusFunc(FocusOp Stat);
            virtual void mouseMove(int x, int y, int dx, int dy);
            virtual void mouseButtonFunc(MouseButton Button, ButtonAction Action);
            virtual void crusorEnterFunc(CursorOp Stat);
            virtual void scrollFunc(double dx, double dy);
            virtual void keyFunc(int Key, ButtonAction Action);
            virtual void charInputFunc(std::wstring Char);
            virtual void dropFunc(const char* Paths);
            virtual void touchFunc(int x, int y, ButtonAction action);
            virtual void touchmove(int x, int y, int dx, int dy);

            virtual ~Control() {}
            bool enabled, mouseOn, pressed, focused;
        };

        //CallBack Functions
        using NotifyFunc = std::function<void()>;
        using onFocus = std::function<void(FocusOp)>;
        using onMouseMove = std::function<void(int, int, int, int)>;
        using onMouseButton = std::function<void(MouseButton, ButtonAction)>;
        using onMouseEnter = std::function<void(CursorOp)>;
        using onScroll = std::function<void(double, double)>;
        using onKeyPress = std::function<void(int, ButtonAction)>;
        using onCharInput = std::function<void(wchar_t)>;
        using onFileDrop = std::function<void(const char*)>;
        using onTouch = std::function<void(int, int action)>;
        using onTouchMove = std::function<void(int, int, int, int)>;

        struct Definition
        {
            int min, max, accBegin, accEnd;
        };

        class UILIB_API Grid : public Control
        {
        public:
            std::vector<Definition> colDefinitions;
            std::vector<Definition> rowDefinitions;
            std::map<std::string, std::shared_ptr<Control>> childern;
            Grid();
            void recalGrid();
            void addChild(std::shared_ptr<Control> child);
            void render();
            void setSize(size_t x, size_t y);
            void parentResize(Grid* parent);
            void focusFunc(FocusOp Stat);
            void onResize(size_t x, size_t y);
            void mouseMove(int x, int y, int dx, int dy);
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);
            void crusorEnterFunc(CursorOp Stat);
            void scrollFunc(double dx, double dy);
            void keyFunc(int Key, ButtonAction Action);
            void charInputFunc(std::wstring Char);
            void dropFunc(const char* Paths);
            void touchFunc(int x, int y, ButtonAction action);
            void touchmove(int x, int y, int dx, int dy);
        private:
            void recal(std::vector<Definition>& definitions, bool col);
            std::vector<std::shared_ptr<Control>> mold, told;
            std::shared_ptr<Control> focused;
            std::shared_ptr<Control> pressed;
        };

        class UILIB_API Page
        {
        public:
            std::shared_ptr<Grid> content;
        };

        class UILIB_API Window
        {
        public:
            virtual ~Window()
            {
            }

            Window(const std::string& name, const int width, const int height,
                   const int _xpos, const int _ypos);
            void close();
            void resize(size_t x, size_t y);
            virtual void onShow();
            virtual void onHide();
            virtual void onClose();
            virtual void onStatChange(StatChange stat);
            virtual void onResize(size_t x, size_t y);
            //Event Handler
            void render();
            void focusFunc(FocusOp Stat);
            void mouseMove(int x, int y, int dx, int dy);
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);
            void crusorEnterFunc(CursorOp Stat);
            void scrollFunc(double dx, double dy);
            void keyFunc(int Key, ButtonAction Action);
            void charInputFunc(std::wstring Char);
            void dropFunc(const char* Paths);
            void touchFunc(int x, int y, ButtonAction action);
            void touchmove(int x, int y, int dx, int dy);

            void pushPage(std::shared_ptr<Page> page);
            void popPage();
            void clearPages();
            void pageZero();

            //SDL ONLY锛?
            Uint32 windowID();
        private:
            std::stack<std::shared_ptr<Page>> pages;
            size_t _x, _y;
            SDL_Window* window;
            SDL_GLContext context;
        };

        class UILIB_API Application
        {
        public:
            virtual ~Application()
            {
            }

            void run();
            void processMessages();
            void addWindow(std::shared_ptr<Window> win);
            void setMainWindow(std::shared_ptr<Window> win);
            void terminate();
            virtual void onTerminate();
            virtual void beforeLaunch();
            virtual void afterLaunch();
            virtual void onEnteringBackground();
            virtual void onEnteringForeground();
        private:
            std::shared_ptr<Window> mainWin, focused;
            std::map<Uint32, std::shared_ptr<Window>> windows;
            void mainLoop();
            bool winExist(int id);
            bool sigExit;
        };
        UILIB_API extern Application* application;
    }

    namespace Graphics
    {
        using namespace Math;
        using namespace Base;
        namespace Shapes
        {
            class UILIB_API Line : public Core::Control
            {
            public:
                Vec2 _p1, _p2;
                std::shared_ptr<Brush> _brush;
                void render();
                Line(const Vec2& p1, Vec2 p2, const std::shared_ptr<Brush> brush);
                static void draw(const Vec2& p1, const Vec2& p2, const std::shared_ptr<Brush> brush);
            };

            class UILIB_API Triangle : public Core::Control
            {
            public:
                Vec2 _p1, _p2, _p3;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Triangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Vec2& p1, const Vec2& p2, const Vec2& p3, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Rectangle : public Core::Control
            {
            public:
                Rect _rect;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Rectangle(const Rect& rect, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Rect& rect, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Polygon : public Core::Control
            {
            public:
                std::vector<Vec2> _pts;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Polygon(const std::initializer_list<Vec2>& pts, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const std::initializer_list<Vec2>& pts, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Circle : public Core::Control
            {
            public:
                Vec2 _center;
                double _radius;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Circle(const Vec2& center, const double radius, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Vec2& center, const double radius, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };

            class UILIB_API Ellipse : public Core::Control
            {
            public:
                Vec2 _f1, _f2;
                double l;
                std::shared_ptr<Brush> _fbrush, _bbrush;
                void render();
                Ellipse(const Vec2& f1, const Vec2& f2, const double l, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
                static void draw(const Vec2& f1, const Vec2& f2, const double l, const std::shared_ptr<Brush> fbrush, const std::shared_ptr<Brush> bbrush);
            };
        }

        namespace Brushes
        {
            class UILIB_API SolidColorBrush : public Brush
            {
            public:
                SolidColorBrush(const Color& _col);
                Color sample(double x, double y);
            private:
                Color col;
            };
#define UIMakeSolidColorBrush(x) std::shared_ptr<Base::Brush>(new Graphics::Brushes::SolidColorBrush(x))

            class UILIB_API GradientBrush : public Brush
            {
            public:
                GradientBrush(const Color& _col0, const Color& _col1, const Color& _col2, const Color& _col3);
                Color sample(double x, double y);
            private:
                Color col0, col1, col2, col3;
            };
        }
    }

    namespace Globalization
    {

        class UILIB_API Str
        {
        public:
            Str(const char* str, bool globalizationNeed = true);
            operator const char*() const;
            void flush();

        private:
            const std::string ds; //data string
            std::string gs; //globalization string
        };

        class UILIB_API Service
        {
        public:
            Service() = default;
            void attachLangFiles(std::initializer_list<const char*> filenames);
            const std::string getCurLang() const;
            void setLang(const std::string& _lang);
            const std::string getStr(const std::string& key) const;
            static inline Service& getInstance()
            {
                static Service service;
                return service;
            }
            void setBasePath(std::string _basePath);
            std::string getBasePath();
            void setSuffix(std::string _suffix);
            std::string getSuffix();
        private:
            std::unordered_map<std::string, const Data::SimpleMapFile> langs;
            std::string curLang;
            const Data::SimpleMapFile* curLangData;
            std::string basePath = "langs/", suffix = ".lang";
        };
    }

    namespace Font
    {
        using namespace Base;
        using namespace Core;
        struct UnicodeChar
        {
            unsigned int tex;
            int xpos, ypos;
            int width, height, advance;
            UnicodeChar();
        };

        class UILIB_API FontOfSize // Font with a specific size
        {
        public:
            UnicodeChar GetChar(const wchar_t);
            FontOfSize() = default;
            FontOfSize(const int _height, FT_Face* _face);
            int height;
        private:
            //int refcount;
            FT_Size size;
            FT_Face* fontface;
            FT_GlyphSlot slot;
            std::map<wchar_t, UnicodeChar> chars;
            std::vector<unsigned int> texBuffers;
            int curBufferID, curBufferPos, maxBufferX, maxBufferY;
        };

        class UILIB_API FontRenderer
        {
        public:
            Color color;
            FontRenderer(FontOfSize& _fos, Color col);
            void renderStr(int x, int y, const Rect& r, const std::wstring& str) const;
            void renderStr(int x, int y, const Rect& r, const std::string& str) const;
            double getStrWidth(const std::wstring& str) const;
            double getStrWidth(const std::string& str) const;
            FontOfSize& fos;
        };

        class UILIB_API FontBase
        {
        public:
            std::string path;
            FontBase() = default;
            FontBase(std::string _path);
            std::shared_ptr<FontRenderer> getRenderer(int height, Color col);
            void gc(); //only call when not enough physical memory
        private:
            std::map<int, FontOfSize> sizes;
            FT_Face fontface;
        };

        class UILIB_API Service
        {
        public:
            Service();
            void addSearchPaths(std::initializer_list<std::string> _apths);
            std::shared_ptr<FontRenderer> getRenderer(const std::string& font, int height, Color col);
            void gc(); //only call when not enough physical memory
        private:
            std::map<std::string, FontBase> fonts;
            std::vector<std::string> searchpaths;
        };

        UILIB_API extern Service service;
    }

    namespace Controls
    {
        using namespace Base;
        using namespace Core;
        using namespace Font;
        using namespace Globalization;
        ////////////////////////////////////////////////////////////
        //    Basic Control Definitions
        ////////////////////////////////////////////////////////////
        class UILIB_API Label : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<FontRenderer> Font;
            Str caption;
            void render();
            Label() = default;
            Label(std::string _xName, Margin _Margin, Str _caption);
        };

        class UILIB_API Button : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<Brush> backgroundHighlightBrush;
            std::shared_ptr<Brush> borderHighlightBrush;
            std::shared_ptr<Brush> backgroundOnPressBrush;
            std::shared_ptr<Brush> borderOnPressBrush;
            std::shared_ptr<FontRenderer> Font;

            NotifyFunc onClick;

            Str caption;
            void render();
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);

            Button() = default;
            Button(std::string _xName, Margin _Margin, Str _caption, NotifyFunc _onClick);
        };

        class UILIB_API TextBox : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<Brush> backgroundHighlightBrush;
            std::shared_ptr<Brush> borderHighlightBrush;
            std::shared_ptr<Brush> crusorBrush;
            std::shared_ptr<FontRenderer> Font;

            NotifyFunc onEditDone;
            std::wstring text;
            Str mask;

            void render();
            void focusFunc(FocusOp Stat);
            void charInputFunc(std::wstring Char);
            void keyFunc(int Key, ButtonAction Action);
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);

            TextBox() = default;
            TextBox(std::string _xName, Margin _Margin, std::wstring _text, Str _mask, NotifyFunc _onEditDone);
        private:
            int left;
            int cursorpxpos = 0;
            size_t cursorpos = 0;
            size_t cposrdcount = 0;
            std::wstring appendingstr;
        };

        enum CheckStat
        {
            Unchecked = 0, Checked, UnKnown
        };

        class UILIB_API CheckBox : public Control
        {
        public:
            std::shared_ptr<Brush> backgroundBrush;
            std::shared_ptr<Brush> borderBrush;
            std::shared_ptr<Brush> backgroundHighlightBrush;
            std::shared_ptr<Brush> borderHighlightBrush;
            std::shared_ptr<Brush> checkBrush;
            std::shared_ptr<Brush> checkHeightLightBrush;
            std::shared_ptr<FontRenderer> Font;

            NotifyFunc onStatChange;

            bool threeStatus;
            CheckStat stat;
            Str caption;

            void render();
            void mouseButtonFunc(MouseButton Button, ButtonAction Action);

            CheckBox() = default;
            CheckBox(std::string _xName, Margin _Margin, Str _caption, bool _threeStat, CheckStat _stat, NotifyFunc _onStatChange);
        };
    }

    namespace Theme
    {
        using namespace Base;
        using namespace Core;
        struct Theme
        {
            std::shared_ptr<Brush> ActiveBorderBrush;
            std::shared_ptr<Brush> InactiveBorderBrush;

            std::shared_ptr<Brush> ControlBrush;
            std::shared_ptr<Brush> ControlDarkBrush;
            std::shared_ptr<Brush> ControlDarkDarkBrush;
            std::shared_ptr<Brush> ControlLightBrush;
            std::shared_ptr<Brush> ControlLightLightBrush;

            std::shared_ptr<Brush> ControlHeightLightBrush;
            std::shared_ptr<Brush> ControlOnPressBrush;

            std::shared_ptr<Brush> MenuBrush;
            std::shared_ptr<Brush> MenuBarBrush;
            std::shared_ptr<Brush> MenuHighlightBrush;

            std::shared_ptr<Brush> ScrollBarBrush;
            std::shared_ptr<Brush> WindowBrush;

            std::shared_ptr<Font::FontRenderer> DefaultFont;
        };
        UILIB_API extern Theme SystemTheme;
    }
}
#endif