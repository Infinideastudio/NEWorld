namespace NWUIK
{

    class NEWORLDFOUNDATIONKIT_API Form;
    class NEWORLDFOUNDATIONKIT_API controls;

    typedef void (controls::*CFC)(controls* sender, void * args);

    class NEWORLDFOUNDATIONKIT_API Trigger
    {
        public:
            CFC* HDCS = nullptr;
            int count = 0;
            Trigger &operator+=(const CFC &f)
            {
                count++;
                CFC* swap = new CFC[count];
                for (int c = 0; c < count - 1; ++c) swap[c] = HDCS[c];
                swap[count - 1] = f;
                delete[]HDCS;
                HDCS = swap;
            }
    };

}
