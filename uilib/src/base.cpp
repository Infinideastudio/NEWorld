#include <uilib.h>
#include <SDL2/SDL_image.h>

namespace UI
{
    namespace Base
    {
        UILIB_API Color blend(const Color & lhs, const Color & rhs)
        {
            return Color(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z, lhs.t*rhs.t);
        }

        UILIB_API void init()
        {
            int initted = IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
            if (!initted)
            {
                printf("IMG_Init: Failed to init required jpg and png support!\n");
                printf("IMG_Init: %s\n", IMG_GetError());
            }
        }

        Rect::Rect(double _xmin, double _xmax, double _ymin, double _ymax) :xmin(_xmin), xmax(_xmax), ymin(_ymin), ymax(_ymax)
        {
        }

        Rect::Rect(Math::Vec2 p1, Math::Vec2 p2) :
            xmin(p1.x), ymin(p1.y), xmax(p2.x), ymax(p2.y)
        {
        }

        void Rect::set(double _xmin, double _xmax, double _ymin, double _ymax)
        {
            xmin = _xmin;
            xmax = _xmax;
            ymin = _ymin;
            ymax = _ymax;
        }

        inline Texture::Texture(std::string path)
        {

            SDL_Surface *image = IMG_Load(path.c_str());
            if (!image) printf("IMG_Load: %s\n", IMG_GetError());
            SDL_CreateTextureFromSurface(UI::Core::currenderer, image);
            SDL_FreeSurface(image);
        }

        Texture::~Texture()
        {
        }

        Image::Image()
        {
        }
        Image::Image(Image & img)
        {
        }
        Image::Image(std::string path)
        {
        }
        /*Image Image::Sub(Rect range)
        {
            return std::move(Image());
        }*/
        Image::~Image()
        {
        }
    }
}
