#include <avr/pgmspace.h>
#include <EEPROM.h>
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

const uint16_t MAX_MSG_SIZE = 512;
const char MAGIC_EEPROM_BYTE = 211;
// char msg[MAX_MSG_SIZE] = {
//     "   Mainframe  -  "
//     "Der Oldenburger Hackspace."
//     "\tTaeglich ab 18:00 Uhr. Siehe: http://status.ktt-ol.de\t\t\t\t"
// };

char msg[MAX_MSG_SIZE] = {
    "123456789012345678901234567890"
};

void read_msg_from_eeprom() {
    uint16_t pos = 0;
    char *m = msg;

    *m = EEPROM.read(pos++);
    while (*m != '\0' && pos < MAX_MSG_SIZE - 1) {
        m++;
        *m = EEPROM.read(pos++);
    }
    *m = '\0';
}

void write_msg_to_eeprom() {
    uint16_t pos = 0;
    char *m = msg;

    while (*m != '\0' && *m != '\n' && pos < MAX_MSG_SIZE - 1) {
        if (*m != '\r') {
            EEPROM.write(pos++, *m);
        }
        m++;
    }
    EEPROM.write(pos, '\0');
}

// void blink() {
//     for (int i = 0; i < 10; ++i) {
//         digitalWrite(13, !digitalRead(13));
//         delay(50);
//     }
// }

void terminate_message(uint16_t num_bytes) {
    msg[num_bytes >= MAX_MSG_SIZE ? MAX_MSG_SIZE - 1 : num_bytes] = '\0';
    for (int i = 0; i < MAX_MSG_SIZE; ++i) {
        switch (msg[i]) {
            case '\r':
            case '\n':
                msg[i] = '\0';
            case '\0':
                return;
        }
    }
}

bool cli_loop() {
    if (Serial.available() > 0) {
        int type = Serial.read();
        if (type == 'm') {
            Serial.println("reading msg");
            uint16_t bytes_read = Serial.readBytesUntil('\n', msg, MAX_MSG_SIZE);
            terminate_message(bytes_read);
            Serial.print("msg: ");
            Serial.println(msg);
            delay(50);
            write_msg_to_eeprom();
            return true;
        }
    }
    return false;
    // delay(500);
    // Serial.println(msg);
    // delay(500);
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
    next_time = millis() + TICK_INTERVAL;
}

void setup() {
    Serial.begin(250000);
    Serial.write('?');
    ticker_init_pins();
    pinMode(13, OUTPUT);
    prev = &buffer1;
    next = &buffer2;
    disp_randomize_factor(prev, 5);
    font_init(&font_info, font_helvB10);
    font_string_shift_init(&shift_info, next, &font_info, msg);
    read_msg_from_eeprom();
}


void loop1() {
    font_string_shift_init(&shift_info, next, &font_info, msg);
    frame_delay_init();
    while (font_string_shift_step(&shift_info)) {
        ticker_shift_display_buffer(next);
        frame_delay();
        if (cli_loop()) {
            break;
        }
    }
    // font_string_shift_step returns immediately when msg
    // is empty, call cli_loop again
    cli_loop();
}

void image_loop() {
    updateDisplayFromSerial(next);
    ticker_shift_display_buffer(next);
}

void loop() {
    updateDisplayFromSerial(next);
    ticker_shift_display_buffer(next);
}

void foo() {
    disp_erase_value(next, 0);
    disp_erase_value(prev, 0);
    ticker_shift_display_buffer(next);

    font_render_string(next, &font_info, "http://ktt----------ol.de", 15);
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

    gol_task_init(&gol_task, prev, next, 50);
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
