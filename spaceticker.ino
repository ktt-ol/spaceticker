#include <avr/pgmspace.h>
#include "font_helvB10.h"
#include "font.h"
#include "ticker.h"
#include "serial.h"
#include "gameoflife.h"

DisplayBuffer_t buffer1;
DisplayBuffer_t buffer2;
DisplayBuffer_t *prev = &buffer1, *next = &buffer2;

void setup() {
    Serial.begin(115200);
    Serial.write('?');
    ticker_init_pins();
    pinMode(13, OUTPUT);
    font::setFont(font_helvB10);
    prev = &buffer1;
    next = &buffer2;
    disp_randomize_factor(prev, 5);
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

int counter = 0;

void gameOfLiveLoop() {
    gol_step(prev, next);
    if ((counter % 20) == 0) {
        gol_random_glider(next);
    }
    if ((counter % 20) == 10) {
        gol_random_fpentomino(next);
    }
    ticker_shift_display_buffer(next);
    disp_swap(&prev, &next);
}

void loop() {
    disp_randomize_factor(prev, 5);
    for (int counter = 0; counter < 30; ++counter) {
        gameOfLiveLoop();
    }
}
