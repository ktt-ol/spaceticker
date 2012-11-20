#include "display.h"

/*
Lehmer random number generator
RNG_N (the cycle of the RNG) is prime and one larger
then the total pixel number
*/
const uint32_t RNG_G = 2670;
const uint32_t RNG_N = 2689;
const uint32_t RNG_X0 = 2688;

class RandomFade {
private:
    DisplayBuffer *prev;
    DisplayBuffer *next;
    uint32_t stepCounter;
    uint32_t rnd;

public:
    RandomFade(DisplayBuffer *disp1, DisplayBuffer *disp2):
        prev(disp1), next(disp2), stepCounter(0), rnd(RNG_X0)
     {}

    bool step() {
        for (uint32_t i = 0; i < 50; ++i) {
            singleStep();
            rnd = (RNG_G * rnd) % RNG_N;
        }
        if (stepCounter >= RNG_X0) {
            stepCounter = 0;
            return false;
        }
        return true;
    }

    DisplayBuffer *getDisplayBuffer() {
        return prev;
    }
    void swapDisplays() {
        DisplayBuffer *temp = prev;
        prev = next;
        next = temp;
    }

private:
    void singleStep() {
        byte x = rnd % next->width;
        byte y = (rnd / next->width) % next->height;
        if (next->getPixel(x, y)) {
            prev->setPixelOn(x, y);
        } else {
            prev->setPixelOff(x, y);
        }
        stepCounter += 1;
    }

};


typedef struct {
    DisplayBuffer_t *prev;
    DisplayBuffer_t *next;
    uint32_t stepCounter;
    uint32_t rnd;
    uint16_t speed;
} RandomFade_t;


void fade_init(RandomFade_t *fade, DisplayBuffer_t *prev, DisplayBuffer_t *next) {
    fade->prev = prev;
    fade->next = next;
    fade->stepCounter = 0;
    fade->rnd = RNG_X0;
    fade->speed = 75;
}

bool fade_step(RandomFade_t *fade) {
    uint8_t x, y;
    for (uint8_t i = 0; i < fade->speed; ++i) {
        x = fade->rnd % fade->next->width;
        y = (fade->rnd / fade->next->width) % fade->next->height;
        if (disp_get_px(fade->next, x, y)) {
            disp_set_px_on(fade->prev, x, y);
        } else {
            disp_set_px_off(fade->prev, x, y);
        }
        fade->stepCounter += 1;

        fade->rnd = (RNG_G * fade->rnd) % RNG_N;
    }
    if (fade->stepCounter >= RNG_X0) {
        fade->stepCounter = 0;
        return false;
    }
    return true;
}
