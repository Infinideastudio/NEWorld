#ifndef BLOCKDATA_H_
#define BLOCKDATA_H_

class BlockData
{
private:
    union
    {
        struct
        {
            unsigned int id : 12; // Block ID
            unsigned int brightness : 4; // Brightness
            unsigned int state : 16; // Block state
        };

        unsigned int data; // Collection of all above
    };

public:

    bool operator=(const BlockData& rhs) { return id == rhs.id; }

    int getID() const { return id; }
    int getBrightness() const { return brightness; }
    int getState() const { return state; }
    void setID(int id_) { id = id_; }
    void setBrightness(int brightness_) { brightness = brightness_; }
    void setState(int state_) { state = state_; }

};

#endif // !BLOCKDATA_H_