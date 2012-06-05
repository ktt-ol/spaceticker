#include "display.h"

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
