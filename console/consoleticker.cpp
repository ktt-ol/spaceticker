#include "consoleticker.h"
#include "../font_helvB10.h"
#include "../gameoflife.h"

Ticker t = Ticker();
DisplayBuffer buffer1 = DisplayBuffer();
DisplayBuffer buffer2 = DisplayBuffer();
DisplayBuffer *prev, *next, *tmp;

bool hitEnd() {
    for (int row = 0; row < HEIGHT; ++row) {
        if (ticker[row][0] == OFF) {
            return false;
        }
    }
    return true;
}


void gameOfLiveLoop(int counter) {
    gameOfLive(prev, next);
    if ((counter % 20) == 0) {
        randomGlider(next);
    }
    if ((counter % 20) == 10) {
        randomFPentomino(next);
    }
    t.shiftInDisplayBuffer(next);
    t.printDelay(50);
    tmp = next;
    next = prev;
    prev = tmp;
}

char msg[] = "Hello";
int main(int argc, char **argv) {
    t.clear();
    prev = &buffer1;
    next = &buffer2;
    prev->randomize(5);
    t.shiftInDisplayBuffer(prev);
    t.printDelay(500);

    font::setFont(font_helvB10);

    prev->randomize(5);
    for (int counter = 0; counter < 100; ++counter) {
        gameOfLiveLoop(counter);
    }

    t.shiftString(msg, 12);

    return 0;
}

// int main(int argc, char **argv) {

//     srandomdev();
//     t.clear();
//     while (!hitEnd()) {
//         for (int j = 0; j < 14; ++j) {
//             t.pushOnLed(random(14), random(2, 8));
//         }
//         t.display(50);
//     }
//     return 0;
// }