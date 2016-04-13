namespace NWUIK
{

    enum SysSolidCB
    {
        NWButtonGray
    };

    class NEWORLDFOUNDATIONKIT_API Brush
    {
        public:
            virtual void PaintArea(Rect Area);
            virtual void PaintWithColorMask(Color4f Color);
    };

    class NEWORLDFOUNDATIONKIT_API SolidColorBrush
    {
        private:
            Color4f Color;
        public:
            void PaintArea(Rect Area);
            void PaintBorder(Rect Area);
            SolidColorBrush(Color4f _Color);
    };
}

