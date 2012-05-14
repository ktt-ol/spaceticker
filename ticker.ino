#include <avr/pgmspace.h>
#include "fonts/font_9x15.h"
#include "fonts/font_9x15B.h"
#include "fonts/font_helvR10.h"
#include "fonts/font_5x7.h"
#include "font.h"

// 3 purple 1
// 4 gray/pink 2
// 5 pink 4
// 6 yellow 8


byte clock_pin = 7; // red
byte data_pin = 2; // white
byte data_state = LOW;

#define ROW_ADDRESS_MASK 0b1111000

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
    for (int i = 0; i < columns; i++) {
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

static void clear() {
    for (int row = 0; row < 14; row++) {
        rowDrawBlank(row, 191);
    }    
}

static void allOn() {
    for (int row = 0; row < 14; row++) {
        rowDraw(row, 191);
    }    
}


void blink() {
    allOn();
    display(50);
    clear();
    display(100);
    allOn();
    display(50);
}


unsigned int icon[] = {
    0b0000000000000000,
    0b1000000101111111,
    0b1000001000001000,
    0b1000010000001000,
    0b1000100010001000,
    0b1001000010001000,
    0b1010000010001000,
    0b1100000111001000,
    0b1010000010001000,
    0b1001000010001000,
    0b1000100010001000,
    0b1000010010101000,
    0b1000001001001000,
    0b0000000000000000,   
};

static void shiftInIcon(unsigned int *icon) {
    for (int i = 0; i < 16; i++) {
        for (int row = 0; row < 14; row++) {
            addressRow(row);
            if ((icon[13-row] & (1<<(15-i))) > 0) {
                ledOn();
            } else {
                ledOff();
            }
            shiftRow(1);
            delay(1);
        }
    }
    for (int i = 0; i < 192; i++) {
        shift();
        display(20);
    }
}

void setup() {
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(8, OUTPUT);
    
    
    pinMode(data_pin, OUTPUT);
    pinMode(clock_pin, OUTPUT);
    pinMode(clock_pin, HIGH);
    pinMode(13, OUTPUT);
    clear();
    setFont(font_helvR10);
}

unsigned long counter = 0;
byte direction = 0;

unsigned int columnData;

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



void sineWave() {
    for (int i = 0; i < 2000; ++i)  {
        byte x = (sin(counter / 500.0) + 1) * 7;
        for (int row = 0; row < 14; row++) {
            if (row == x) {
                rowDraw(row, 1);
            } else {
                rowDrawBlank(row, 1);
            }
            displayMicroseconds(200);
            counter += 1;        
        }
        if (counter == 500) {
            counter = 0;
        }
    }
}


void sineWave1() {
    int offset = 0;
    while (1) {
        int columnData = 0;
        byte x;
        for (int i = 0; i < 1; ++i) {
            x = round((sin(counter / 50.0 + PI/16 * i) + 1) * 6.5);
            for (int row = 0; row < 14; row++) {
                if (row == x) {
                    columnData |= 1 << (13-row);
                }
            }
        }
        if (counter % 20) {
            offset += 1;
        }
        shiftInColumn(columnData);
        displayMicroseconds(3000);
        counter += 1;
    }
}

void loop() {
    clear();
    sineWave();
    clear();
    shiftString(&string[0]);
}

void loop2() {
    for (int row = 0; row < 14; row++) {
        if (direction % 2 == 0) {
            rowDrawBlank(row, counter);
            rowDraw(row, 1);
            rowDrawBlank(row, 191-counter);            
        } else {
            rowDrawBlank(row, 191-counter);            
            rowDraw(row, 1);
            rowDrawBlank(row, counter);
        }
    }

    display(5);

    if (counter >= 191) {
        counter = 0;
        direction += 1;
    } else {
        counter += 1;
    }
}