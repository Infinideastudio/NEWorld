#ifndef BLOCKDATA_H_
#define BLOCKDATA_H_

class BlockData
{
    private:
        union {
            unsigned int data;
            struct {
                unsigned int id : 12; // Block ID
                unsigned int brightness : 4; // Brightness
                unsigned int state : 16; // Block state
            };
        };

    public:
        int getID() const;
        int getBrightness() const;
        int getState() const;
        void setID(int id);
        void setBrightness(int brightness);
        void setState(int state);

};

#endif // !BLOCKDATA_H_
