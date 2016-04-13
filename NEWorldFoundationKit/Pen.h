namespace NWUIK
{

    class NEWORLDFOUNDATIONKIT_API Pen
    {
        private:
            Color4f Color; 
            double linewid;
        public:
            void SerLineWidth(double Width);
            void PaintLine(Rect Area);
            Pen(Color4f _Color, double _lw = 1.0) ;
    };

}
