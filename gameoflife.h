#include "display.h"

void gol_step(DisplayBuffer_t *prev, DisplayBuffer_t *next) {
    uint8_t lives = 0;

    for (int y = 0; y < prev->height; ++y) {
        for (int x = 0; x < prev->width; ++x) {
            lives  = disp_get_px_wrap(prev, x-1, y-1);
            lives += disp_get_px_wrap(prev, x-1, y);
            lives += disp_get_px_wrap(prev, x-1, y+1);
            lives += disp_get_px_wrap(prev, x, y-1);
            lives += disp_get_px_wrap(prev, x, y+1);
            lives += disp_get_px_wrap(prev, x+1, y-1);
            lives += disp_get_px_wrap(prev, x+1, y);
            lives += disp_get_px_wrap(prev, x+1, y+1);

            if (disp_get_px_wrap(prev, x, y)) {
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
