// Arduino PIN  wire color  description
//      2         white      data pin   (high -> on)
//      3         purple     row address 1
//      4       gray/pink    row address 2
//      5         pink       row address 4
//      6        yellow      row address 8
//      7          red       shift pin  (low -> shift)
//      8         green     display pin (low -> show) 

#define ROW_ADDRESS_MASK ((byte)0b1111000)


// set current LED row
static void addressRow(byte row) {
    PORTD ^= (ROW_ADDRESS_MASK & (PORTD ^ (row << 3)));
}

// turn current addressed LED on/off
#define ledOff() PORTD &= ~(1 << 2)
#define ledOn() PORTD |= 1 << 2

// turn _all_ led on/off, current state is kept
#define show() PORTB &= ~1
#define hide() PORTB |= 1

// shift current LED row by ``columns`` to left
static void shiftRow(byte columns=1) {
    for (byte i = 0; i < columns; i++) {
        PORTD &= ~(1 << 7);
        PORTD |= 1 << 7;
    }
}


// shift LEDs by ``columns`` to left
static void shift(byte columns=1) {
    for (int row = 0; row < 14; row++) {
        addressRow(row);    
        for (int i = 0; i < columns; i++) {
            PORTD &= ~(1 << 7);
            PORTD |= 1 << 7;
        }
    }
}

static void shiftBlank(byte columns=1) {
    for (int row = 0; row < 14; row++) {
        addressRow(row);    
        for (int i = 0; i < columns; i++) {
            ledOff();
            shiftRow();
        }
    }
}

static void shiftInColumn(unsigned int data) {
    for (int row = 0; row < 14; row++) {
        addressRow(row);
        if ((data & (1<<(13-row))) > 0) {
            ledOn();
        } else {
            ledOff();
        }
        shiftRow();
    }
}

static void rowDrawBlank(byte row, byte length) {
    addressRow(row);
    ledOff();
    shiftRow(length);
}

static void rowDraw(byte row, byte length) {
    addressRow(row);
    ledOn();
    shiftRow(length);
}

static void display(unsigned int ms) {
    show();
    delay(ms);
    hide();
}

static void displayMicroseconds(unsigned int us) {
    show();
    delayMicroseconds(us);
    hide();
}


static void allOn() {
    for (int row = 0; row < 14; row++) {
        rowDraw(row, 191);
    }    
}

static void allOff() {
    for (int row = 0; row < 14; row++) {
        rowDrawBlank(row, 191);
    }    
}

// buf size (192 * 14) / 8 = 336

class DisplayBuffer {
public:
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
        memset(&buf[0], 159, 336);
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


class Ticker {
    static const byte width = 192;
    static const byte height = 14;

public:
    static void initPins() {
        for (int i = 2; i <=8 ; ++i) {
            pinMode(i, OUTPUT);
        }
        allOff();
    }

    static void allOff() {
        for (int row = 0; row < height; row++) {
            pushOffLed(row, width-1);
        }
    }

    static void allOn() {
        for (int row = 0; row < height; row++) {
            pushOnLed(row, width-1);
        }
    }

    static void display(unsigned int msDelay) {
        show();
        delay(msDelay);
        hide();
    }

    static void displayMicroseconds(unsigned int usDelay) {
        show();
        delayMicroseconds(usDelay);
        hide();
    }

    static void pushOnLed(byte row, byte num=1) {
        addressRow(row);
        ledPowerOn();
        shiftCurrentRow(num);
    }

    static void pushOffLed(byte row, byte num=1) {
        addressRow(row);
        ledPowerOff();
        shiftCurrentRow(num);
    }

    // shift current LED row by ``columns`` to left
    static void shiftCurrentRow(byte columns=1) {
        for (byte i = 0; i < columns; i++) {
            PORTD &= ~(1 << 7);
            PORTD |= 1 << 7;
        }
    }

    static inline void ledPowerOn() {
        PORTD |= 1 << 2;
    }

    static inline void ledPowerOff() {
        PORTD &= ~(1 << 2);
    }

    void shiftInDisplayBuffer(DisplayBuffer *displayBuffer) {
        for (int row = 0; row < height; ++row) {
            addressRow(row);
            for (int col = 0; col < width; ++col) {
                if (displayBuffer->getPixel(col, row)) {
                    ledPowerOn();
                } else {
                    ledPowerOff();
                }
                shiftCurrentRow();
            }
        }
    }

    void shiftInRandom() {
        for (int row = 0; row < height; ++row) {
            addressRow(row);
            for (int col = 0; col < width; ++col) {
                if ((col % 2) == (row % 2)) {
                    ledPowerOn();
                } else {
                    ledPowerOff();
                }
                shiftCurrentRow();
            }
        }
    }

};


void blink(const Ticker *ticker) {
    ticker->allOn();
    ticker->display(50);
    ticker->allOff();
    ticker->display(100);
    ticker->allOn();
    ticker->display(50);
}

void shiftChar(char c, int colDelay=30) {
    setChar(c);
    for (int i = 0; i < charInfo.width; i++) {
        shiftInColumn(charColumn(i));
        display(colDelay);
    }
    shiftBlank();
    display(colDelay);
}

char string[] = "Kreativitaet trifft Technik!";

void shiftString(char *string) {
    int len = strlen(string);
    while (*string != 0) {
        shiftChar(*string, 30);
        string++;
    }

    for (int i = 0; i < (192 - len); ++i) {
        shiftBlank();
        display(30);
    }
}


