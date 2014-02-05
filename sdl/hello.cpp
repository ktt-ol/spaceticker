#include "../console/compat.h"
#include <unistd.h>
#include "SDL.h"
#include "../font.h"
#include "../font_helvB10.h"
#include "../gameoflife.h"
#include "../checkerboard.h"
#include "../randomfade.h"


DisplayBuffer_t buffer1;
DisplayBuffer_t buffer2;
DisplayBuffer_t *prev = &buffer1, *next = &buffer2, *tmp;

SDL_Surface *screen;

const byte PIXEL_WIDTH  = 2;
const byte PIXEL_HEIGTH = 2;

// GameOfLive gol = GameOfLive(&buffer1, &buffer2);
// RandomFade fade = RandomFade(&buffer1, &buffer2);

const int SCREEN_WIDTH = 192 * PIXEL_WIDTH;
const int SCREEN_HEIGHT = 14 * PIXEL_HEIGTH;

#define SCREEN_DEPTH 8
#define TICK_INTERVAL 5

static Uint32 next_time = 0;
void sdl_delay_init() {
    next_time = SDL_GetTicks() + TICK_INTERVAL;
}
void sdl_delay(void) {
    next_time += TICK_INTERVAL;
    Uint32 now = SDL_GetTicks();
    if(next_time <= now) {
        return;
    } else {
        return SDL_Delay(next_time - now);
    }
}

void sdl_poll_quit_events(void) {
    SDL_Event event;
    while ( SDL_PollEvent(&event) > 0 ) {
        switch (event.type) {
            case SDL_QUIT: {
                exit(0);
            }
            break;
            case SDL_KEYDOWN: {
                if (event.key.keysym.scancode == 12) {
                    exit(0);
                }
            }
            break;
        }
    }
}


void sdl_show_disp(DisplayBuffer_t *disp) {
    for (int y = 0; y < (disp->height * PIXEL_HEIGTH); ++y) {
        for (int x = 0; x < (disp->width * PIXEL_WIDTH); ++x) {
            Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * screen->format->BytesPerPixel;
            if (disp_get_px(disp, x/PIXEL_WIDTH, y/PIXEL_HEIGTH)) {
                *p = 0xfe;
            } else {
                *p = 0x00;
            }
        }
    }
    SDL_Flip(screen);
    sdl_poll_quit_events();
    sdl_delay();
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    sdl_delay_init();
    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SDL_SWSURFACE);

    FontInfo_t font_info;
    StringShift_t shift_info;
    RandomFade_t fade;
    char msg[] = {
        "Mainframe  -  "
        "Der Oldenburger Hackspace."
        "\tTaeglich ab 18:00 Uhr."
        "   Siehe: http://status.ktt-ol.de\t\t"
    };

    font_init(&font_info, font_helvB10);
    while (1) {
        font_render_string(next, &font_info, "http://ktt      -  ol.de", 25);
        sdl_show_disp(next);
        sleep(1);

        fade_init(&fade, next, prev);
        while (fade_step(&fade)) {
            sdl_show_disp(next);
        }

        font_string_shift_init(&shift_info, next, &font_info, msg);

        GOLTask_t gol_task = {.next = next, .prev = prev, .glider_steps = 20, .fpento_steps = 20, .max_steps = 500};

        while (font_string_shift_step(&shift_info)) {
            sdl_show_disp(next);
        }
        sdl_show_disp(next);

        font_string_shift_reset(&shift_info);

        disp_randomize_factor(prev, 5);

        fade_init(&fade, next, prev);
        while (fade_step(&fade)) {
            sdl_show_disp(next);
        }

        while(gol_task_step(&gol_task)) {
            sdl_show_disp(gol_task.next);
        }

        disp_erase_value(prev, 0);
        fade_init(&fade, next, prev);
        while (fade_step(&fade)) {
            sdl_show_disp(next);
        }
    }

    return 0;
}