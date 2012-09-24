#ifndef _COMPAT_H_
#define _COMPAT_H_

#define ARDUINO_COMPAT

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

typedef uint8_t byte;
typedef uint16_t word;
typedef uint8_t prog_uint8_t;

#define PROGMEM

uint8_t pgm_read_byte(uint8_t *p) {
    return *p;
}

uint8_t bit(uint8_t b) {
    return 1 << b;
}

int random(int max) {
    return random() % max;
}

int random(int min, int max) {
    return random() % (max - min) + min;
}

#endif