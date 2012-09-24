#ifndef _CONSOLE_TICKER_H_
#define _CONSOLE_TICKER_H_

#include <stdio.h>
#include "compat.h"
#include "../font.h"
#include "../display.h"

static const byte ON = 'x';
static const byte OFF = ' ';

static const byte HEIGHT = 14;
static const byte WIDTH = 192;

static byte currentRow = 0;
static byte ticker[HEIGHT][WIDTH];
static byte *currentLed = (byte *)ticker + WIDTH - 1;
static byte status = ON;

#define delay(x) usleep(x * 1000);
#define delayMicroseconds(x) usleep(x);

// set current LED row
static void addressRow(byte row) {
    currentRow = row;
    currentLed = &ticker[row][WIDTH - 1];
}

// turn current addressed LED on/off
#define ledOff() (status = OFF)
#define ledOn() (status = ON)

// turn _all_ led on/off, current state is kept
#define show()
#define hide()

// shift current LED row to left
static inline void shiftRow() {
    memmove(&ticker[currentRow][0], &ticker[currentRow][1], WIDTH - 1);
    ticker[currentRow][WIDTH - 1] = status;
}

// shift current LED row by ``columns`` to left
static void shiftRow(byte columns) {
    for (byte i = 0; i < columns; i++) {
        shiftRow();
    }
}

// shift LEDs by ``columns`` to left
static void shift(byte columns=1) {
    for (int row = 0; row < 14; row++) {
        addressRow(row);
        for (int i = 0; i < columns; i++) {
            shiftRow();
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
        print();
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
        shiftRow(columns);
    }

    static inline void shiftCurrentRow() {
        shiftRow();
    }

    static inline void ledPowerOn() {
        ledOn();
    }

    static inline void ledPowerOff() {
        ledOff();
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
            printDelay(colDelay);
        }
        shiftBlank();
        printDelay(colDelay);
    }

    void shiftString(char *string, int colDelay=30) {
        while (*string != 0) {
            if (*string == '\t') {
                for (int i = 0; i < 50; ++i) {
                    shiftBlank();
                    printDelay(colDelay);
                }
            } else if (*string == ' ') {
                for (int i = 0; i < 5; ++i) {
                    shiftBlank();
                    printDelay(colDelay);
                }
            } else {
                shiftChar(*string, colDelay);
            }
            string++;
        }

        for (int i = 0; i < 192; ++i) {
            shiftBlank();
            printDelay(colDelay);
        }
    }

    static void printDelay(long ms) {
        print();
        delay(ms);
    }

    static void print() {
        printf("\033[2J");
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {
                putchar(ticker[row][col]);
            }
            putchar('\n');
        }
        putchar('\n');

    }

    static void clear() {
        memset(ticker, ' ', WIDTH * HEIGHT);
    }

};


void blink(Ticker *ticker) {
    ticker->allOn();
    ticker->display(50);
    ticker->allOff();
    ticker->display(100);
    ticker->allOn();
    ticker->display(50);
}

#endif
