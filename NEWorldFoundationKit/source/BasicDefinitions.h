namespace NWUIK
{

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
    
    struct Color4f
    {
        double r, g, b, a;
    }

    typedef Rect TMargin;

}
