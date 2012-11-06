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
//      8         green     display pin (low -> power on)

#define ROW_ADDRESS_MASK ((byte)0b1111000)


// set current LED row
static inline void ticker_address_row(byte row) {
    PORTD ^= (ROW_ADDRESS_MASK & (PORTD ^ ((13 - row) << 3)));
}

// turn current addressed LED on/off
static inline void ticker_led_on() { PORTD &= ~(1 << 2); }
static inline void ticker_led_off() { PORTD |= 1 << 2; }

// turn _all_ led on/off, current state is kept
static inline void ticker_power_on() { PORTB &= ~1; }
static inline void ticker_power_off() { PORTB |= 1; }

// shift current LED row to left
static inline void ticker_shift_row() {
    PORTD &= ~(1 << 7);
    PORTD |= 1 << 7;
}

static void ticker_init_pins() {
    for (int i = 2; i <= 8 ; ++i) {
        pinMode(i, OUTPUT);
    }
    ticker_power_on();
}

static void ticker_shift_display_buffer(DisplayBuffer_t *disp) {
    // shift in display buffer by direct access to the buffer
    uint8_t current_byte;
    for (uint8_t row = 0; row < disp->height; ++row) {
        ticker_address_row(row);
        for (uint8_t col = 0; col < disp->width/8; ++col) {
            current_byte = disp->buf[row * (disp->width/8) + col];

            // manual loop unrolling
            (0b10000000 & current_byte) ? ticker_led_on() : ticker_led_off();
            ticker_shift_row();
            (0b01000000 & current_byte) ? ticker_led_on() : ticker_led_off();
            ticker_shift_row();
            (0b00100000 & current_byte) ? ticker_led_on() : ticker_led_off();
            ticker_shift_row();
            (0b00010000 & current_byte) ? ticker_led_on() : ticker_led_off();
            ticker_shift_row();
            (0b00001000 & current_byte) ? ticker_led_on() : ticker_led_off();
            ticker_shift_row();
            (0b00000100 & current_byte) ? ticker_led_on() : ticker_led_off();
            ticker_shift_row();
            (0b00000010 & current_byte) ? ticker_led_on() : ticker_led_off();
            ticker_shift_row();
            (0b00000001 & current_byte) ? ticker_led_on() : ticker_led_off();
            ticker_shift_row();
        }
    }
}


// // shift current LED row by ``columns`` to left
// static void ticker_shift_row_n(byte columns) {
//     for (byte i = 0; i < columns; i++) {
//         PORTD &= ~(1 << 7);
//         PORTD |= 1 << 7;
//     }
// }


// // shift LEDs by ``columns`` to left
// static void shift(byte columns=1) {
//     for (int row = 0; row < 14; row++) {
//         ticker_address_row(row);
//         for (int i = 0; i < columns; i++) {
//             PORTD &= ~(1 << 7);
//             PORTD |= 1 << 7;
//         }
//     }
// }

// static void shiftBlank(byte columns=1) {
//     ticker_led_off();
//     for (int row = 0; row < 14; row++) {
//         ticker_address_row(row);
//         for (int i = 0; i < columns; i++) {
//             ticker_shift_row();
//         }
//     }
// }

// static void shiftInColumn(unsigned int data) {
//     for (int row = 0; row < 14; row++) {
//         ticker_address_row(row);
//         if ((data & (1 << row)) > 0) {
//             ticker_led_on();
//         } else {
//             ticker_led_off();
//         }
//         ticker_shift_row();
//     }
// }

// static void rowDrawBlank(byte row, byte length) {
//     ticker_address_row(row);
//     ticker_led_off();
//     ticker_shift_row_n(length);
// }

// static void rowDraw(byte row, byte length) {
//     ticker_address_row(row);
//     ticker_led_on();
//     ticker_shift_row_n(length);
// }

// static void display(unsigned int ms) {
//     ticker_power_on();
//     delay(ms);
//     ticker_power_off();
// }

// static void displayMicroseconds(unsigned int us) {
//     ticker_power_on();
//     delayMicroseconds(us);
//     ticker_power_off();
// }


// static void allOn() {
//     for (int row = 0; row < 14; row++) {
//         rowDraw(row, 191);
//     }
// }

// static void allOff() {
//     for (int row = 0; row < 14; row++) {
//         rowDrawBlank(row, 191);
//     }
// }


// class Ticker {
//     static const byte width = 192;
//     static const byte height = 14;

// public:
//     static void initPins() {
//         for (int i = 2; i <=8 ; ++i) {
//             pinMode(i, OUTPUT);
//         }
//         allOff();
//         ticker_power_on();
//     }

//     static void allOff() {
//         for (int row = 0; row < height; row++) {
//             pushOffLed(row, width-1);
//         }
//     }

//     static void allOn() {
//         for (int row = 0; row < height; row++) {
//             pushOnLed(row, width-1);
//         }
//     }

//     static void display(unsigned int msDelay) {
//         ticker_power_on();
//         delay(msDelay);
//         ticker_power_off();
//     }

//     static void displayMicroseconds(unsigned int usDelay) {
//         ticker_power_on();
//         delayMicroseconds(usDelay);
//         ticker_power_off();
//     }

//     static void pushOnLed(byte row, byte num=1) {
//         ticker_address_row(row);
//         ledPowerOn();
//         shiftCurrentRow(num);
//     }

//     static void pushOffLed(byte row, byte num=1) {
//         ticker_address_row(row);
//         ledPowerOff();
//         shiftCurrentRow(num);
//     }

//     // shift current LED row by ``columns`` to left
//     static void shiftCurrentRow(byte columns) {
//         for (byte i = 0; i < columns; i++) {
//             PORTD &= ~(1 << 7);
//             PORTD |= 1 << 7;
//         }
//     }

//     static inline void shiftCurrentRow() {
//             PORTD &= ~(1 << 7);
//             PORTD |= 1 << 7;
//     }

//     static inline void ledPowerOn() {
//         PORTD |= 1 << 2;
//     }

//     static inline void ledPowerOff() {
//         PORTD &= ~(1 << 2);
//     }

//     static void shiftInColumn(unsigned int data) {
//         for (int row = 0; row < 14; row++) {
//             if ((data & (1 << row)) > 0) {
//                 pushOnLed(row);
//             } else {
//                 pushOffLed(row);
//             }
//         }
//     }
//     static void shiftBlank(byte columns=1) {
//         for (int row = 0; row < 14; row++) {
//             for (int i = 0; i < columns; i++) {
//                 pushOffLed(row);
//             }
//         }
//     }

//     static void shiftInDisplayBuffer(const DisplayBuffer *displayBuffer) {
//         for (int row = 0; row < height; ++row) {
//             ticker_address_row(row);
//             for (int col = 0; col < width; ++col) {
//                 if (displayBuffer->getPixel(col, row)) {
//                     ledPowerOn();
//                 } else {
//                     ledPowerOff();
//                 }
//                 shiftCurrentRow();
//             }
//         }
//     }

//     static void shiftInDisplayBufferRaw(DisplayBuffer_t *disp) {
//         // shift in display buffer by direct access to the buffer
//         byte currentByte;
//         for (int row = 0; row < disp->height; ++row) {
//             ticker_address_row(row);
//             for (int col = 0; col < disp->width/8; ++col) {
//                 currentByte = disp->buf[row * (192/8) + col];

//                 // manual loop unrolling
//                 (0b10000000 & currentByte) ? ledPowerOn() : ledPowerOff();
//                 shiftCurrentRow();
//                 (0b01000000 & currentByte) ? ledPowerOn() : ledPowerOff();
//                 shiftCurrentRow();
//                 (0b00100000 & currentByte) ? ledPowerOn() : ledPowerOff();
//                 shiftCurrentRow();
//                 (0b00010000 & currentByte) ? ledPowerOn() : ledPowerOff();
//                 shiftCurrentRow();
//                 (0b00001000 & currentByte) ? ledPowerOn() : ledPowerOff();
//                 shiftCurrentRow();
//                 (0b00000100 & currentByte) ? ledPowerOn() : ledPowerOff();
//                 shiftCurrentRow();
//                 (0b00000010 & currentByte) ? ledPowerOn() : ledPowerOff();
//                 shiftCurrentRow();
//                 (0b00000001 & currentByte) ? ledPowerOn() : ledPowerOff();
//                 shiftCurrentRow();
//             }
//         }
//     }

//     void shiftInRandom() {
//         for (int row = 0; row < height; ++row) {
//             ticker_address_row(row);
//             for (int col = 0; col < width; ++col) {
//                 if ((col % 2) == (row % 2)) {
//                     ledPowerOn();
//                 } else {
//                     ledPowerOff();
//                 }
//                 shiftCurrentRow();
//             }
//         }
//     }

//     void shiftChar(char c, int colDelay=30) {
//         font::setChar(c);
//         for (int i = 0; i < font::charInfo.width; i++) {
//             shiftInColumn(font::charColumn(i));
//             delay(colDelay);
//         }
//         shiftBlank();
//         delay(colDelay);
//     }

//     void shiftString(char *string, int colDelay=30) {
//         while (*string != 0) {
//             if (*string == '\t') {
//                 for (int i = 0; i < 50; ++i) {
//                     shiftBlank();
//                     delay(colDelay);
//                 }
//             } else if (*string == ' ') {
//                 for (int i = 0; i < 5; ++i) {
//                     shiftBlank();
//                     delay(colDelay);
//                 }
//             } else {
//                 shiftChar(*string, colDelay);
//             }
//             string++;
//         }

//         for (int i = 0; i < 192; ++i) {
//             shiftBlank();
//             delay(colDelay);
//         }
//     }

// };


// void blink(const Ticker *ticker) {
//     ticker->allOn();
//     ticker->display(50);
//     ticker->allOff();
//     ticker->display(100);
//     ticker->allOn();
//     ticker->display(50);
// }


#endif
