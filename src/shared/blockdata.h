#ifndef BLOCKDATA_H_
#define BLOCKDATA_H_

class BlockData
{
private:
    union
    {
        unsigned int data;
        struct
        {
            unsigned int id : 12; // Block ID
            unsigned int brightness : 4; // Brightness
            unsigned int state : 16; // Block state
        };
    };

public:

    BlockData() :data(0) {}
    BlockData(int id_, int brightness_, int state_) :id(id_), brightness(brightness_), state(state_) {}
    BlockData(int data_) :data(data_) {}
    BlockData(const BlockData& rhs) :data(rhs.data) {}

    void operator= (const BlockData& rhs) { data = rhs.data; }
    bool operator== (const BlockData& rhs) const { return id == rhs.id; }

    int getID() const { return id; }
    int getBrightness() const { return brightness; }
    int getState() const { return state; }
    void setID(int id_) { id = id_; }
    void setBrightness(int brightness_) { brightness = brightness_; }
    void setState(int state_) { state = state_; }

};

#endif // !BLOCKDATA_H_