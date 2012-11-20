/*! \file */

#ifdef ARDUINO
#include "Arduino.h"
#endif

#include "display.h"

typedef struct CharInfo_ {
    word pos;
    byte width;
    byte pre;
    byte post;
    byte total_width;
} CharInfo_t;

typedef struct FontInfo_ {
    byte height, width, first, count;
    prog_uint8_t* image;
    prog_uint8_t* widths;
    prog_uint8_t* overflow;
    CharInfo_t current_char;
} FontInfo_t;


/** Initialize font.
*
* @param font_info initialize this FontInfo_t struct
* @param font_data pointer to the raw font data in progmem
*/
void font_init(FontInfo_t *font_info, prog_uint8_t* font_data) {
    prog_uint8_t* fp = (prog_uint8_t*) font_data;
    font_info->height = pgm_read_byte(fp++);
    font_info->width = pgm_read_byte(fp++);
    font_info->image = fp;
    fp += font_info->height * font_info->width;
    font_info->first = pgm_read_byte(fp++);
    font_info->count = pgm_read_byte(fp++);
    font_info->widths = fp;
    if (pgm_read_byte(fp) == 0) {
        fp += font_info->count + font_info->count + 1;
        font_info->overflow = fp;
    } else
        font_info->overflow = 0;
}

void font_set_current_char(FontInfo_t *font_info, char c) {
    if (c >= font_info->first) {
        c -= font_info->first;
        if (c < font_info->count) {
            byte pix = pgm_read_byte(font_info->widths);
            byte gaps = pgm_read_byte(font_info->widths+1);
            word pos = 0;
            if (pix == 0) {
                // adjust for offset overflows past 255
                for (byte i = 0; ; ++i) {
                    byte o = pgm_read_byte(font_info->overflow + i);
                    if (c <= o)
                        break;
                    pos += 256;
                }
                // extract offset and gap info for this particular char index
                prog_uint8_t* wp = font_info->widths + 2 * c;
                byte off = pgm_read_byte(wp++);
                gaps = pgm_read_byte(wp++);
                byte next = pgm_read_byte(wp);
                // horizontal bitmap position and char width in pixels
                pos += off;
                pix = next - off;
            } else
                pos = c * pix; // mono-spaced fonts
            char pre = (gaps & 0x0F) - 4;
            char post = (gaps >> 4) - 4;

            font_info->current_char.pos = pos;
            font_info->current_char.width = pix;
            font_info->current_char.pre = pre;
            font_info->current_char.post = post;
            font_info->current_char.total_width = pre + pix + post;
        }
    }
}

#define MIN(a,b) (((a)<(b))?(a):(b))

/** Render char in DisplayBuffer_t.
* @param disp display buffer for rendering
* @param font_info font to use
* @param c char to render
* @param x x pixel offset
*/
uint8_t font_render_char(DisplayBuffer_t *disp, FontInfo_t *font_info, const char c, uint8_t x) {
    font_set_current_char(font_info, c);
    prog_uint8_t *bits;
    CharInfo_t *char_info = &(font_info->current_char);

    uint8_t max_width = MIN(char_info->width, disp->width);
    uint8_t max_height = MIN(font_info->height, disp->height);

    for (int i = 0; i < max_width; i++) {
        bits = font_info->image;
        for (byte j = 0; j < max_height; ++j) {
            if (pgm_read_byte((prog_uint8_t*) bits + (char_info->pos+i) / 8) & bit((char_info->pos+i) % 8)) {
                disp_set_px_on(disp, x + i, j);
            } else {
                disp_set_px_off(disp, x + i, j);
            }
            bits += font_info->width;
        }
    }
    return char_info->width;
}


/** Render string in DisplayBuffer_t.
* @param disp display buffer for rendering
* @param font_info font to use
* @param c text to render
* @param x x pixel offset
*/
uint8_t font_render_string(DisplayBuffer_t *disp, FontInfo_t *font_info, const char *c, uint8_t x) {
    while (*c != '\0') {
        x += font_render_char(disp, font_info, *c, x);
        x += 2;
        c++;
    }
    return x;
}


/** Shift in a single column of a char in to a DisplayBuffer_t.
*
* Shifts the \a disp one pixel to the left and insert \a column of \a char_info
* at the right. Use this function to shift in a single char step-by-step.
*
* @param disp display buffer for rendering
* @param char_info the char to shift in
* @param column
*/
void font_shift_char_column(DisplayBuffer_t *disp, FontInfo_t *font_info, uint8_t column) {
    disp_shift_left(disp);
    CharInfo_t *char_info = &(font_info->current_char);

    prog_uint8_t *bits = font_info->image;
    // skip first row, empty on most fonts?
    bits += font_info->width;
    uint8_t max_height = MIN(font_info->height - 1, disp->height);

    for (byte j = 0; j < max_height; ++j) {
        if (pgm_read_byte((prog_uint8_t*) bits + (char_info->pos+column) / 8) & bit((char_info->pos+column) % 8)) {
            disp_set_px_on(disp, disp->width - 1, j);
        } else {
            disp_set_px_off(disp, disp->width - 1, j);
        }
        bits += font_info->width;
    }
}

typedef struct StringShift_ {
    char *msg;
    char *current_char;
    FontInfo_t *font_info;
    DisplayBuffer_t * disp;
    uint8_t current_char_column;
    uint8_t interchar_columns; /**< number of pixels between each char */
    uint8_t interchar_pos; /**< internal counter for interchar_columns */
    uint8_t tab_width; /**< whitespace pixels for \t */
} StringShift_t;


void font_string_shift_init(StringShift_t *shift, DisplayBuffer_t *disp, FontInfo_t *font_info, char *msg) {
    shift->disp = disp;
    shift->msg = msg;
    shift->current_char = msg;
    shift->font_info = font_info;
    font_set_current_char(font_info, *shift->current_char);
    shift->current_char_column = 0;
    shift->interchar_columns = 2;
    shift->interchar_pos = 0;
    shift->tab_width = 50;
}

void font_string_shift_reset(StringShift_t *shift) {
    shift->current_char = shift->msg;
    font_set_current_char(shift->font_info, *shift->current_char);
    shift->current_char_column = 0;
    shift->interchar_pos = 0;
}

void font_string_shift_set_message(StringShift_t *shift, char *msg) {
    shift->msg = msg;
    font_string_shift_reset(shift);
}

bool font_string_shift_step(StringShift_t *shift) {
    if (shift->interchar_pos) {
        disp_shift_left(shift->disp);
        shift->interchar_pos -= 1;
        return true;
    }

    if (*(shift->current_char) == '\0') {
        return false;
    }

    if (shift->current_char_column == shift->font_info->current_char.width) {
        (shift->current_char)++;
        shift->interchar_pos = shift->interchar_columns;
        if (*(shift->current_char) != '\0') {
            if (*(shift->current_char) == '\t') {
                shift->interchar_pos = shift->tab_width;
            } else {
                shift->current_char_column = 0;
                font_set_current_char(shift->font_info, *(shift->current_char));
            }
            disp_shift_left(shift->disp);
            shift->interchar_pos -= 1;
        }
        return true;
    }

    font_shift_char_column(shift->disp, shift->font_info, shift->current_char_column);

    shift->current_char_column += 1;
    return true;
}
