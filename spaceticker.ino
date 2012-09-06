#include <avr/pgmspace.h>
#include "font_helvB10.h"
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
    prev->randomize(5);
}

char *msg = {
    "   Kreativitaet trifft Technik!"
    "\tDer Oldenburger Hackspace."
    "\t3D-Drucker, Styroporschneider, LED-Cubes, Arduino, "
    " CNC-Fraese, Saegen, Schrauben, Loeten und Programmieren."
    "\tOffen wenn offen. Siehe: http://status.ktt-ol.de"
    "\tMindestens: Di, Do, Sa, So ab 18:00 Uhr"
    "\thttp://ktt-ol.de"
};

void blank() {
    ticker.shiftString(" ", 5);
}


int counter = 0;

void gameOfLiveLoop() {
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
}

void loop() {
    blank();
    prev->randomize(5);
    for (int counter = 0; counter < 30; ++counter) {
        gameOfLiveLoop();
    }
    blank();
    ticker.shiftString(msg, 12);
}
