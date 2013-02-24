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

int space_status;

char header[] = {
    "   Mainframe  -  "
    "Der Oldenburger Hackspace.\t"
};
char open[] = {
    "Der Space ist offen!\t"
};
char closed[] = {
    "Taeglich ab 18:00 Uhr. Siehe: http://status.ktt-ol.de\t"
};
char msg[MAX_MSG_SIZE];

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

bool in_image_mode = false;

/** Read commands from serial.
*
* @return true if command changed (i.e. from image mode to text)
*/
bool read_commands() {
    if (Serial.available() > 0) {
        int type = Serial.read();
        if (type == 'm') {
            uint16_t bytes_read = Serial.readBytesUntil('\n', msg, MAX_MSG_SIZE);
            terminate_message(bytes_read);
            write_msg_to_eeprom();
        } else if (type == 's') {
            while (Serial.available() <= 0) { }
            space_status = Serial.read();
        } else if (type == 'i') {
            if (in_image_mode) {
                return false;
            }

            in_image_mode = true;
            return true;
        }
        if (in_image_mode) {
            in_image_mode = false;
            return true;
        }
        return false;
    }
    return false;
}

uint32_t next_time = 0;
const uint8_t TICK_INTERVAL = 12;

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


enum LoopReturn {
    NORMAL,
    COMMAND_CHANGE
};
#define READ_COMMAND() if(read_commands()) { return COMMAND_CHANGE; }
#define PASS_COMMAND_CHANGE(x) if (x == COMMAND_CHANGE) { return COMMAND_CHANGE; }

LoopReturn gol_loop() {
    // fade in random
    disp_randomize_factor(prev, 5);
    fade_init(&fade, next, prev);
    frame_delay_init();
    while (fade_step(&fade)) {
        ticker_shift_display_buffer(next);
        READ_COMMAND();
        frame_delay();
    }

    // game of life with current random display
    gol_task_init(&gol_task, prev, next, 50);
    while(gol_task_step(&gol_task)) {
        READ_COMMAND();
        ticker_shift_display_buffer(gol_task.next);
    }

    // fade out game of life
    disp_erase_value(prev, 0);
    fade_init(&fade, next, prev);
    frame_delay_init();
    while (fade_step(&fade)) {
        ticker_shift_display_buffer(next);
        READ_COMMAND();
        frame_delay();
    }
    return NORMAL;
}


void setup() {
    // Serial.begin(250000);
    Serial.begin(115200);
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

LoopReturn image_loop() {
    if (updateDisplayFromSerial(next)) {
        ticker_shift_display_buffer(next);
    }
    while (true) {
        READ_COMMAND();
    }
}

LoopReturn shift_msg(char *msg) {
    font_string_shift_init(&shift_info, next, &font_info, msg);
    frame_delay_init();
    while (font_string_shift_step(&shift_info)) {
        READ_COMMAND();
        ticker_shift_display_buffer(next);
        frame_delay();
    }
    ticker_shift_display_buffer(next);
    font_string_shift_reset(&shift_info);
    return NORMAL;
}


/** Normal text/anim loop.
*
* Displays header text, open/close status, custom msg,
* Game Of Live loop and URL.
*/
LoopReturn text_loop() {
    PASS_COMMAND_CHANGE(shift_msg(header));
    if (space_status == 'o') {
        PASS_COMMAND_CHANGE(shift_msg(open));
    } else {
        PASS_COMMAND_CHANGE(shift_msg(closed));
    }
    PASS_COMMAND_CHANGE(shift_msg(msg));
    PASS_COMMAND_CHANGE(shift_msg(" \thttp://mainframe.io"));
    PASS_COMMAND_CHANGE(shift_msg(" \t\t\t\t"));
}


void loop() {
    while (true) {
        if (in_image_mode) {
            image_loop();
        } else {
            text_loop();
            gol_loop();
        }
    }
}


void fade_text_in_out() {
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
}
