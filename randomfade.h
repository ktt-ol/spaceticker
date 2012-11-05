#include "display.h"

/*
Lehmer random number generator
RNG_N (the cycle of the RNG) is prime and one larger
then the total pixel number
*/
const int RNG_G = 2670;
const int RNG_N = 2689;
const int RNG_X0 = 2688;

class RandomFade {
private:
    DisplayBuffer *prev;
    DisplayBuffer *next;
    int stepCounter;
    int rnd;

public:
    RandomFade(DisplayBuffer *disp1, DisplayBuffer *disp2):
        prev(disp1), next(disp2), stepCounter(0), rnd(RNG_X0)
     {}

    bool step() {
        for (int i = 0; i < 50; ++i) {
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

