#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "display.h"

static const byte MAGIC_BYTES[4] = {159, 170, 85, 241};

#define SERIAL_WAIT_AVAILABLE() while (Serial.available() < 1) {}

bool seekToFrameStart() {
    uint32_t timeout = millis() + 1000;
    while (millis() < timeout) {
        Serial.print('\n');
        SERIAL_WAIT_AVAILABLE();
        // read till first magic byte
        while (Serial.read() != MAGIC_BYTES[0]) {
            SERIAL_WAIT_AVAILABLE();
        }
        // start again (continue) if other three bytes do not match
        SERIAL_WAIT_AVAILABLE();
        if (Serial.read() != MAGIC_BYTES[1]) { continue; }
        SERIAL_WAIT_AVAILABLE();
        if (Serial.read() != MAGIC_BYTES[2]) { continue; }
        SERIAL_WAIT_AVAILABLE();
        if (Serial.read() != MAGIC_BYTES[3]) { continue; }
        SERIAL_WAIT_AVAILABLE();
        return true;
    }
    return false;
}

bool updateDisplayFromSerial(DisplayBuffer_t *display) {
    if (!seekToFrameStart()) {
        return false;
    }
    int bytesRead = 0;
    while (bytesRead <= display->size) {
        SERIAL_WAIT_AVAILABLE();
        display->buf[bytesRead] = Serial.read();
        bytesRead += 1;
    }
    return true;
}

#endif