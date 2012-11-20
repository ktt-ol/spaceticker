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

    bool step() {
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

void gol_step(DisplayBuffer_t *prev, DisplayBuffer_t *next) {
    uint8_t lives = 0;

    for (int y = 0; y < prev->height; ++y) {
        for (int x = 0; x < prev->width; ++x) {
            lives  = disp_get_px(prev, x-1, y-1);
            lives += disp_get_px(prev, x-1, y);
            lives += disp_get_px(prev, x-1, y+1);
            lives += disp_get_px(prev, x, y-1);
            lives += disp_get_px(prev, x, y+1);
            lives += disp_get_px(prev, x+1, y-1);
            lives += disp_get_px(prev, x+1, y);
            lives += disp_get_px(prev, x+1, y+1);

            if (disp_get_px(prev, x, y)) {
                if (lives < 2) {
                    disp_set_px_off(next, x, y);
                } else if (lives == 2 || lives == 3) {
                    disp_set_px_on(next, x, y);
                } else {
                    disp_set_px_off(next, x, y);
                }
            } else if (lives == 3) {
                disp_set_px_on(next, x, y);
            } else {
                disp_set_px_off(next, x, y);
            }
        }
    }
}

void gol_random_glider(DisplayBuffer_t *disp) {
    int x = random(disp->width-2);
    int y = random(disp->height-2);

    disp_set_px_on(disp, x,   y);
    disp_set_px_on(disp, x+1, y);
    disp_set_px_on(disp, x+2, y);
    disp_set_px_on(disp, x+2, y+1);
    disp_set_px_on(disp, x+1, y+2);
}

void gol_random_fpentomino(DisplayBuffer_t *disp) {
    int x = random(disp->width-2);
    int y = random(disp->height-2);

    disp_set_px_on(disp, x+1, y);
    disp_set_px_on(disp, x,   y+1);
    disp_set_px_on(disp, x+1, y+1);
    disp_set_px_on(disp, x+1, y+2);
    disp_set_px_on(disp, x+2, y+2);
}

typedef struct GOLTask_ {
    DisplayBuffer_t *next;
    DisplayBuffer_t *prev;
    uint16_t glider_steps;
    uint16_t fpento_steps;
    uint32_t step_counter;
    uint32_t max_steps;
} GOLTask_t;

void gol_task_init(GOLTask_t *task, DisplayBuffer_t *next, DisplayBuffer_t *prev, uint32_t max_steps) {
    task->next = next;
    task->prev = prev;
    task->max_steps = max_steps;
    task->step_counter = 0;
}

bool gol_task_step(GOLTask_t *task) {
    disp_swap(&(task->prev), &(task->next));
    gol_step(task->prev, task->next);
    task->step_counter += 1;

    if (task->max_steps && task->step_counter >= task->max_steps) {
        return false;
    }
    if (task->glider_steps && task->step_counter % task->glider_steps == 0) {
        gol_random_glider(task->next);
    }
    if (task->fpento_steps && task->step_counter % task->fpento_steps == 0) {
        gol_random_fpentomino(task->next);
    }
    return true;
}
