#include <avr/pgmspace.h>
#include "fonts/font_9x15.h"
#include "fonts/font_9x15B.h"
#include "fonts/font_helvR10.h"
#include "fonts/font_5x7.h"
#include "font.h"
#include "ticker.h"
#include "serial.h"

Ticker ticker = Ticker();
DisplayBuffer buffer = DisplayBuffer();

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
    font::setFont(font_helvR10);
}

char *msg = { 
    "Sommer im Quartier."
    "\tKreativitaet trifft Technik!"
    "\tDer Oldenburger Hackspace stellt sich vor."
    "\tBasteln, loeten und diskutieren; 3D-Drucker, Styrophorschneider, LED-Cubes,"
    " Klackerlaken, Arduino, CNC-Fraese, Terminals"
    "\tDer Space ist offen! Kommt einfach rein und Treppe hoch."
};

void loop() {
    ticker.shiftString(msg, 20);
    // updateDisplayFromSerial(&buffer);
    // ticker.shiftInDisplayBufferRaw(&buffer);
}
