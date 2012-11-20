#include <avr/pgmspace.h>
#include "font_helvB10.h"
#include "font.h"
#include "ticker.h"
#include "serial.h"
#include "randomfade.h"
#include "gameoflife.h"

DisplayBuffer_t buffer1;
DisplayBuffer_t buffer2;
DisplayBuffer_t *prev = &buffer1, *next = &buffer2;

FontInfo_t font_info;
StringShift_t shift_info;
RandomFade_t fade;
GOLTask_t gol_task;

char msg[] = {
    "   Mainframe  -  "
    "Der Oldenburger Hackspace."
    "\tTaeglich ab 18:00 Uhr. Siehe: http://status.ktt-ol.de\t\t\t\t"
};

void setup() {
    Serial.begin(115200);
    Serial.write('?');
    ticker_init_pins();
    pinMode(13, OUTPUT);
    prev = &buffer1;
    next = &buffer2;
    disp_randomize_factor(prev, 5);
    font_init(&font_info, font_helvB10);
    font_string_shift_init(&shift_info, next, &font_info, msg);
}


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


uint32_t next_time = 0;
const uint8_t TICK_INTERVAL = 15;

void frame_delay_init(void) {
    next_time = millis() + TICK_INTERVAL;
}

void frame_delay(void) {
    uint32_t now = millis();
    if(next_time > now) {
        delay(next_time - now);
    }
    next_time += TICK_INTERVAL;
}

void loop() {

    disp_erase_value(next, 0);
    disp_erase_value(prev, 0);
    ticker_shift_display_buffer(next);

    font_render_string(next, &font_info, "http://ktt-ol.de", 15);
    fade_init(&fade, prev, next);
    frame_delay_init();
    while (fade_step(&fade)) {
        ticker_shift_display_buffer(prev);
        frame_delay();
    }

    delay(2000);

    disp_erase_value(prev, 0);
    fade_init(&fade, next, prev);
    frame_delay_init();
    while (fade_step(&fade)) {
        ticker_shift_display_buffer(next);
        frame_delay();
    }

    font_string_shift_init(&shift_info, next, &font_info, msg);
    frame_delay_init();
    while (font_string_shift_step(&shift_info)) {
        ticker_shift_display_buffer(next);
        frame_delay();
    }
    ticker_shift_display_buffer(next);
    frame_delay();

    font_string_shift_reset(&shift_info);

    disp_randomize_factor(prev, 5);

    fade_init(&fade, next, prev);
    frame_delay_init();
    while (fade_step(&fade)) {
        ticker_shift_display_buffer(next);
        frame_delay();
    }

    gol_task_init(&gol_task, prev, next, 100);
    while(gol_task_step(&gol_task)) {
        ticker_shift_display_buffer(gol_task.next);
    }

    disp_erase_value(prev, 0);
    fade_init(&fade, next, prev);
    frame_delay_init();
    while (fade_step(&fade)) {
        ticker_shift_display_buffer(next);
        frame_delay();
    }
}
