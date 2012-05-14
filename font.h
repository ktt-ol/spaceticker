struct FontInfo {
    byte height, width, first, count;
    prog_uint8_t* image;
    prog_uint8_t* widths;
    prog_uint8_t* overflow;
} fontInfo;

struct CharInfo {
    word pos;
    byte width;
    byte pre;
    byte post;
    byte totalWidth;
} charInfo;

void setFont(const byte* font) {
    prog_uint8_t* fp = (prog_uint8_t*) font;
    fontInfo.height = pgm_read_byte(fp++);
    fontInfo.width = pgm_read_byte(fp++);
    fontInfo.image = fp;
    fp += fontInfo.height * fontInfo.width;
    fontInfo.first = pgm_read_byte(fp++);
    fontInfo.count = pgm_read_byte(fp++);
    fontInfo.widths = fp;
    if (pgm_read_byte(fp) == 0) {
        fp += fontInfo.count + fontInfo.count + 1;
        fontInfo.overflow = fp;
    } else
        fontInfo.overflow = 0;
}

void setChar(char c) {
    const struct FontInfo& fi = fontInfo;
    if (c >= fi.first) {
        c -= fi.first;
        if (c < fi.count) {
            byte pix = pgm_read_byte(fi.widths);
            byte gaps = pgm_read_byte(fi.widths+1);
            word pos = 0;
            if (pix == 0) {
                // adjust for offset overflows past 255
                for (byte i = 0; ; ++i) {
                    byte o = pgm_read_byte(fi.overflow + i);
                    if (c <= o)
                        break;
                    pos += 256;
                } 
                // extract offset and gap info for this particular char index
                prog_uint8_t* wp = fi.widths + 2 * c;
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
            
            charInfo.pos = pos;
            charInfo.width = pix;
            charInfo.pre = pre;
            charInfo.post = post;
            charInfo.totalWidth = pre + pix + post;
        }
    }
}

unsigned int charColumn(byte column) {
    unsigned int result = 0;
    
    byte *bits = fontInfo.image;

    for (byte j = 0; j < fontInfo.height; ++j) {
        if (pgm_read_byte((prog_uint8_t*) bits + (charInfo.pos+column) / 8) & bit((charInfo.pos+column) % 8)) {
            result |= 1 << j;
        }
        bits += fontInfo.width;
    }
    return result;
}