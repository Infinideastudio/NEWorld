#include "blockdata.h"

int BlockData::getID() const
{
    return id;
}

int BlockData::getBrightness() const
{
    return brightness;
}

int BlockData::getState() const
{
    return state;
}

void BlockData::setID(int id_)
{
    id = id_;
}

void BlockData::setBrightness(int brightness_)
{
    brightness = brightness_;
}

void BlockData::setState(int state_)
{
    state = state_;
}
