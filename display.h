#ifndef _DISPLAY_H_
#define _DISPLAY_H_

class DisplayBuffer {
public:
    // buf size (192 * 14) / 8 = 336
    static const size_t size = 336;
    byte buf[size];

    inline int pixelOffset(byte col, byte row) const {
        return row * 192 + col;
    }

    bool getPixel(byte col, byte row) {
        int offset = pixelOffset(col, row);
        return (buf[offset / 8] & (1 << (offset % 8))) > 0;
    }

    void setPixelOn(byte col, byte row) {
        int offset = pixelOffset(col, row);
        buf[offset / 8] |= 1 << (offset % 8);
    }

    void setPixelOff(byte col, byte row) {
        int offset = pixelOffset(col, row);
        buf[offset / 8] &= ~(1 << (offset % 8));
    }

    void setColumn(byte row, unsigned int columnData) {
        for (int i = 0; i < 14; ++i) {
            if ((columnData & (1 << i)) > 0) {
                setPixelOn(i, row);                
            } else {
                setPixelOff(i, row);
            }
        }
    }
    
    void erase() {
        memset(&buf[0], 0, 336);
    }

    void randomize() {
        for (int row = 0; row < 14; ++row) {
            for (int col = 0; col < 192; ++col) {
                if (random(5) == 0) {
                    setPixelOn(col, row);
                } else {
                    setPixelOff(col, row);
                }
            }
        }
    }
};

#endif