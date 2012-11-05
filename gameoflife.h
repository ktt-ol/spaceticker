#include "display.h"

class GameOfLive {
private:
    DisplayBuffer *prev;
    DisplayBuffer *next;
    int stepCounter;
    int maxSteps;

public:
    GameOfLive(DisplayBuffer *disp1, DisplayBuffer *disp2):
        prev(disp1), next(disp2), stepCounter(0), maxSteps(0)
     {}

    void step() {
        byte lives = 0;

        for (int y = 0; y < prev->height; ++y) {
            for (int x = 0; x < prev->width; ++x) {
                lives  = prev->getPixelWrap(x-1, y-1);
                lives += prev->getPixelWrap(x-1, y);
                lives += prev->getPixelWrap(x-1, y+1);
                lives += prev->getPixelWrap(x, y-1);
                lives += prev->getPixelWrap(x, y+1);
                lives += prev->getPixelWrap(x+1, y-1);
                lives += prev->getPixelWrap(x+1, y);
                lives += prev->getPixelWrap(x+1, y+1);

                if (prev->getPixelWrap(x, y)) {
                    if (lives < 2) {
                        next->setPixelOff(x, y);
                    } else if (lives == 2 || lives == 3) {
                        next->setPixelOn(x, y);
                    } else {
                        next->setPixelOff(x, y);
                    }
                } else if (lives == 3) {
                    next->setPixelOn(x, y);
                } else {
                    next->setPixelOff(x, y);
                }
            }
        }
        stepCounter += 1;

        addRandomStuff();
        swapDisplays();

        if (maxSteps && stepCounter >= maxSteps) {
            return false;
        }
        return true;
    }

    void setMaxSteps(int steps) {
        maxSteps = steps;
    }

    DisplayBuffer *getDisplayBuffer() {
        return next;
    }

    void addRandomStuff() {
        if ((stepCounter % 20) == 0) {
            randomGlider();
        }
        if ((stepCounter % 20) == 10) {
            randomFPentomino();
        }
    }


    void randomGlider() {
        int x = random(next->width-2);
        int y = random(next->height-2);

        next->setPixelOn(x,   y);
        next->setPixelOn(x+1, y);
        next->setPixelOn(x+2, y);
        next->setPixelOn(x+2, y+1);
        next->setPixelOn(x+1, y+2);
    }

    void randomFPentomino() {
        int x = random(next->width-2);
        int y = random(next->height-2);

        next->setPixelOn(x+1, y);
        next->setPixelOn(x,   y+1);
        next->setPixelOn(x+1, y+1);
        next->setPixelOn(x+1, y+2);
        next->setPixelOn(x+2, y+2);
    }

private:
    void swapDisplays() {
        DisplayBuffer *temp = prev;
        prev = next;
        next = temp;
    }

};

void gameOfLive(DisplayBuffer *prev, DisplayBuffer *next) {
    byte lives = 0;

    for (int y = 0; y < prev->height; ++y) {
        for (int x = 0; x < prev->width; ++x) {
            lives  = prev->getPixelWrap(x-1, y-1);
            lives += prev->getPixelWrap(x-1, y);
            lives += prev->getPixelWrap(x-1, y+1);
            lives += prev->getPixelWrap(x, y-1);
            lives += prev->getPixelWrap(x, y+1);
            lives += prev->getPixelWrap(x+1, y-1);
            lives += prev->getPixelWrap(x+1, y);
            lives += prev->getPixelWrap(x+1, y+1);

            if (prev->getPixelWrap(x, y)) {
                if (lives < 2) {
                    next->setPixelOff(x, y);
                } else if (lives == 2 || lives == 3) {
                    next->setPixelOn(x, y);
                } else {
                    next->setPixelOff(x, y);
                }
            } else if (lives == 3) {
                next->setPixelOn(x, y);
            } else {
                next->setPixelOff(x, y);
            }
        }
    }
}

void randomGlider(DisplayBuffer *display) {
    int x = random(display->width-2);
    int y = random(display->height-2);

    display->setPixelOn(x,   y);
    display->setPixelOn(x+1, y);
    display->setPixelOn(x+2, y);
    display->setPixelOn(x+2, y+1);
    display->setPixelOn(x+1, y+2);
}

void randomFPentomino(DisplayBuffer *display) {
    int x = random(display->width-2);
    int y = random(display->height-2);

    display->setPixelOn(x+1, y);
    display->setPixelOn(x,   y+1);
    display->setPixelOn(x+1, y+1);
    display->setPixelOn(x+1, y+2);
    display->setPixelOn(x+2, y+2);
}
