#ifndef _DISPLAY_H_
#define _DISPLAY_H_

class DisplayBuffer {
public:
    // buf size (192 * 14) / 8 = 336
    static const byte width = 192;
    static const byte height = 14;
    static const size_t size = width * height / 8;
    byte buf[size];

    inline int pixelOffset(byte col, byte row) const {
        return row * 192 + col;
    }

    inline bool getPixel(byte col, byte row) const {
        int offset = pixelOffset(col, row);
        return (buf[offset / 8] & (0b10000000 >> (offset % 8))) > 0;
    }

    inline bool getPixelWrap(int col, int row) const {
        // wrap around at edges
        if (col < 0) {
            col = width - 1;
        } else if (col >= width) {
            col = 0;
        }
        if (row < 0) {
            row = height - 1;
        } else if (row >= height) {
            row = 0;
        }
        int offset = pixelOffset(col, row);
        return (buf[offset / 8] & (0b10000000 >> (offset % 8))) > 0;
    }

    void setPixelOn(byte col, byte row) {
        int offset = pixelOffset(col, row);
        buf[offset / 8] |= (0b10000000 >> (offset % 8));
    }

    void setPixelOff(byte col, byte row) {
        int offset = pixelOffset(col, row);
        buf[offset / 8] &= ~(0b10000000 >> (offset % 8));
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

    void randomize(byte factor=5) {
        for (int row = 0; row < 14; ++row) {
            for (int col = 0; col < 192; ++col) {
                if (random(factor) == 0) {
                    setPixelOn(col, row);
                } else {
                    setPixelOff(col, row);
                }
            }
        }
    }
};

#endif