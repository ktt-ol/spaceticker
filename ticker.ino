#include <avr/pgmspace.h>
#include "fonts/font_9x15.h"
#include "fonts/font_9x15B.h"
#include "fonts/font_helvR10.h"
#include "fonts/font_helvB10.h"
#include "fonts/font_5x7.h"
#include "font.h"
#include "ticker.h"
#include "serial.h"
#include "gameoflife.h"

Ticker ticker = Ticker();
DisplayBuffer buffer1 = DisplayBuffer();
DisplayBuffer buffer2 = DisplayBuffer();
DisplayBuffer *prev, *next, *tmp;

void blink(int msDelay) {
    ticker.allOn();
    ticker.display(msDelay);
    ticker.allOff();
}

void setup() {
    Serial.begin(115200);
    Serial.write('?');
    ticker.initPins();
    pinMode(13, OUTPUT);
    font::setFont(font_helvB10);
    prev = &buffer1;
    next = &buffer2;
    prev->randomize(3);
}

char *msg = { 
    "Sommer im Quartier."
    "\tKreativitaet trifft Technik!"
    "\tDer Oldenburger Hackspace stellt sich vor."
    "\tBasteln, Loeten und Lernen!   3D-Drucker, Styroporschneider, LED-Cubes,"
    " Klackerlaken, Arduino, CNC-Fraese, Terminals"
    "\tDer Space ist offen! Kommt einfach rein und Treppe hoch."
};


int counter = 0;
void loop() {
    gameOfLive(prev, next);
    if ((counter % 20) == 0) {
        randomGlider(next);
    }
    if ((counter % 20) == 10) {
        randomFPentomino(next);
    }
    ticker.shiftInDisplayBufferRaw(next);
    tmp = next;
    next = prev;
    prev = tmp;

    counter += 1;

    // ticker.shiftString(msg, 20);
    // updateDisplayFromSerial(&buffer1);
    // ticker.shiftInDisplayBufferRaw(&buffer1);
}
