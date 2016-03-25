#incldue "Trigger.h"
namespace NWUIK
{

    Trigger & Trigger::operator+=(const CFC &f)
    {
        count++;
        CFC* swap = new CFC[count];
        for (int c = 0; c < count - 1; ++c) swap[c] = HDCS[c];
        swap[count - 1] = f;
        delete[]HDCS;
        HDCS = swap;
    }

}
