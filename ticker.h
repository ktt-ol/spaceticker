#ifndef _TICKER_H_
#define _TICKER_H_

#include "display.h"

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
    PORTD ^= (ROW_ADDRESS_MASK & (PORTD ^ ((13 - row) << 3)));
}

// turn current addressed LED on/off
#define ledOff() PORTD &= ~(1 << 2)
#define ledOn() PORTD |= 1 << 2

// turn _all_ led on/off, current state is kept
#define show() PORTB &= ~1
#define hide() PORTB |= 1

// shift current LED row to left
static inline void shiftRow() {
    PORTD &= ~(1 << 7);
    PORTD |= 1 << 7;
}

// shift current LED row by ``columns`` to left
static void shiftRow(byte columns) {
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
        if ((data & (1 << row)) > 0) {
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


class Ticker {
    static const byte width = 192;
    static const byte height = 14;

public:
    static void initPins() {
        for (int i = 2; i <=8 ; ++i) {
            pinMode(i, OUTPUT);
        }
        allOff();
        show();
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
    static void shiftCurrentRow(byte columns) {
        for (byte i = 0; i < columns; i++) {
            PORTD &= ~(1 << 7);
            PORTD |= 1 << 7;
        }
    }

    static inline void shiftCurrentRow() {
            PORTD &= ~(1 << 7);
            PORTD |= 1 << 7;
    }

    static inline void ledPowerOn() {
        PORTD |= 1 << 2;
    }

    static inline void ledPowerOff() {
        PORTD &= ~(1 << 2);
    }

    static void shiftInColumn(unsigned int data) {
        for (int row = 0; row < 14; row++) {
            if ((data & (1 << row)) > 0) {
                pushOnLed(row);
            } else {
                pushOffLed(row);
            }
        }
    }
    static void shiftBlank(byte columns=1) {
        for (int row = 0; row < 14; row++) {
            for (int i = 0; i < columns; i++) {
                pushOffLed(row);
            }
        }
    }

    static void shiftInDisplayBuffer(const DisplayBuffer *displayBuffer) {
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

    static void shiftInDisplayBufferRaw(DisplayBuffer *displayBuffer) {
        // shift in display buffer by direct access to the buffer
        byte currentByte;
        for (int row = 0; row < height; ++row) {
            addressRow(row);
            for (int col = 0; col < width/8; ++col) {
                currentByte = displayBuffer->buf[row * (192/8) + col];

                // manual loop unrolling
                (0b10000000 & currentByte) ? ledPowerOn() : ledPowerOff();
                shiftCurrentRow();
                (0b01000000 & currentByte) ? ledPowerOn() : ledPowerOff();
                shiftCurrentRow();
                (0b00100000 & currentByte) ? ledPowerOn() : ledPowerOff();
                shiftCurrentRow();
                (0b00010000 & currentByte) ? ledPowerOn() : ledPowerOff();
                shiftCurrentRow();
                (0b00001000 & currentByte) ? ledPowerOn() : ledPowerOff();
                shiftCurrentRow();
                (0b00000100 & currentByte) ? ledPowerOn() : ledPowerOff();
                shiftCurrentRow();
                (0b00000010 & currentByte) ? ledPowerOn() : ledPowerOff();
                shiftCurrentRow();
                (0b00000001 & currentByte) ? ledPowerOn() : ledPowerOff();
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

    void shiftChar(char c, int colDelay=30) {
        font::setChar(c);
        for (int i = 0; i < font::charInfo.width; i++) {
            shiftInColumn(font::charColumn(i));
            delay(colDelay);
        }
        shiftBlank();
        delay(colDelay);
    }

    void shiftString(char *string, int colDelay=30) {
        while (*string != 0) {
            if (*string == '\t') {
                for (int i = 0; i < 50; ++i) {
                    shiftBlank();
                    delay(colDelay);
                }
            } else if (*string == ' ') {
                for (int i = 0; i < 5; ++i) {
                    shiftBlank();
                    delay(colDelay);
                }
            } else {
                shiftChar(*string, colDelay);
            }
            string++;
        }

        for (int i = 0; i < 192; ++i) {
            shiftBlank();
            delay(colDelay);
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

#endif
