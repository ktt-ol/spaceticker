#ifndef _DISPLAY_H_
#define _DISPLAY_H_

typedef struct  {
    static const byte width = 192;
    static const byte height = 14;
    static const size_t size = width * height / 8;
    byte buf[size];
} DisplayBuffer_t;

static inline uint16_t disp_px_offset(const DisplayBuffer_t *disp, const uint8_t col, const uint8_t row) {
        return row * (disp->width) + col;
}

inline bool disp_get_px(const DisplayBuffer_t *disp, const uint8_t col, const uint8_t row) {
    uint16_t offset = disp_px_offset(disp, col, row);
    return (disp->buf[offset / 8] & (0b10000000 >> (offset % 8))) > 0;
}

inline bool disp_get_px_wrap(const DisplayBuffer_t *disp, uint8_t col, uint8_t row) {
    // wrap around at edges
    if (col < 0) {
        col = disp->width - 1;
    } else if (col >= disp->width) {
        col = 0;
    }
    if (row < 0) {
        row = disp->height - 1;
    } else if (row >= disp->height) {
        row = 0;
    }
    return disp_get_px(disp, col, row);
}

inline void disp_set_px_on(DisplayBuffer_t *disp, uint8_t col, uint8_t row) {
    uint16_t offset = disp_px_offset(disp, col, row);
    disp->buf[offset / 8] |= (0b10000000 >> (offset % 8));
}

inline void disp_set_px_off(DisplayBuffer_t *disp, uint8_t col, uint8_t row) {
    uint16_t offset = disp_px_offset(disp, col, row);
    disp->buf[offset / 8] &= ~(0b10000000 >> (offset % 8));
}

inline void disp_set_column(DisplayBuffer_t *disp, uint8_t row, uint16_t column_data) {
    for (uint8_t i = 0; i < 14; ++i) {
        if ((column_data & (1 << i)) > 0) {
            disp_set_px_on(disp, i, row);
        } else {
            disp_set_px_off(disp, i, row);
        }
    }
}

void disp_erase(DisplayBuffer_t *disp) {
    memset(&(disp->buf)[0], 0, disp->size);
}

void disp_erase_value(DisplayBuffer_t *disp, uint8_t value) {
    memset(&(disp->buf)[0], value, disp->size);
}

void disp_randomize_factor(DisplayBuffer_t *disp, uint8_t factor) {
    for (uint8_t row = 0; row < disp->height; ++row) {
        for (uint8_t col = 0; col < disp->width; ++col) {
            if (random(factor) == 0) {
                disp_set_px_on(disp, col, row);
            } else {
                disp_set_px_off(disp, col, row);
            }
        }
    }
}

void disp_randomize(DisplayBuffer_t *disp) {
    disp_randomize_factor(disp, 2);
}

void disp_invert(DisplayBuffer_t *disp) {
    for (int i = 0; i < disp->size; ++i) {
        disp->buf[i] ^= 0xff;
    }
}

void disp_swap(DisplayBuffer_t **one, DisplayBuffer_t **two) {
    DisplayBuffer_t *temp = *two;
    *two = *one;
    *one = temp;
}

void disp_shift_left(DisplayBuffer_t *disp) {
    for (uint16_t i = 0; i < disp->size - 1; ++i) {
        disp->buf[i] = (disp->buf[i] << 1) | (disp->buf[i+1] >> 7);
    }
    disp->buf[disp->size - 1] = (disp->buf[disp->size - 1] << 1);

    for (int row = 0; row < disp->height; ++row) {
        disp_set_px_off(disp, disp->width - 1, row);
    }
}

#endif