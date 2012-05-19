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
    Serial.println("Hello!");
    ticker.initPins();
    pinMode(13, OUTPUT);
    setFont(font_helvR10);
    blink(1000);
}

void fillBufferRandom(DisplayBuffer *buffer) {
    int bytesRead = 0;
    while (bytesRead < buffer->size) {
        if (random(5) == 0) {
            buffer->buf[bytesRead] = 170;                
        } else {
            buffer->buf[bytesRead] = 0;
        }
        bytesRead += 1;                
    }
}

void loop() {
    updateDisplayFromSerial(&buffer);
    ticker.shiftInDisplayBuffer(&buffer);
    // delay(1000);
}
