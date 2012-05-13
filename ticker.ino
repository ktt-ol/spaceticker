// 3 purple 1
// 4 gray/pink 2
// 5 pink 4
// 6 yellow 8


byte clock_pin = 7; // red
byte data_pin = 2; // white
byte data_state = LOW;

#define ROW_ADDRESS_MASK 0b1111000

// set current LED row
static void addressRow(byte row) {
    PORTD ^= (ROW_ADDRESS_MASK & (PORTD ^ (row << 3)));
}

// shift current LED row by ``columns`` to left
static void shiftRow(byte columns=1) {
    for (int i = 0; i < columns; i++) {
        PORTD &= ~(1 << 7);
        PORTD |= 1 << 7;
    }
}

// shift LEDs by ``columns`` to left
static void shift(byte columns=1) {
    for (int row = 0; row < 14; row++) {
        addressRow(row);    
        for (int i = 0; i < columns; i++) {
            PORTD &= ~(1 << 7);
            PORTD |= 1 << 7;
        }
    }
}

// turn current addressed LED on/off
#define ledOff() PORTD &= ~(1 << 2)
#define ledOn() PORTD |= 1 << 2

// turn _all_ led on/off, current state is kept
#define show() PORTB &= ~1
#define hide() PORTB |= 1


static void rowDrawBlank(byte row, byte length) {
    addressRow(row);
    ledOff();
    shiftRow(length);
}

static void rowDraw(byte row, byte length) {
    addressRow(row);
    ledOn();
    shiftRow(length);
}

static void display(unsigned int ms) {
    show();
    delay(ms);
    hide();
}

static void displayMicroseconds(unsigned int us) {
    show();
    delayMicroseconds(us);
    hide();
}

static void clear() {
    for (int row = 0; row < 14; row++) {
        rowDrawBlank(row, 191);
    }    
}

static void allOn() {
    for (int row = 0; row < 14; row++) {
        rowDraw(row, 191);
    }    
}


void blink() {
    allOn();
    display(50);
    clear();
    display(100);
    allOn();
    display(50);
}


unsigned int icon[] = {
    0b0000000000000000,
    0b1000000101111111,
    0b1000001000001000,
    0b1000010000001000,
    0b1000100010001000,
    0b1001000010001000,
    0b1010000010001000,
    0b1100000111001000,
    0b1010000010001000,
    0b1001000010001000,
    0b1000100010001000,
    0b1000010010101000,
    0b1000001001001000,
    0b0000000000000000,   
};

static void shifInIcon(unsigned int *icon) {
    for (int i = 0; i < 16; i++) {
        for (int row = 0; row < 14; row++) {
            addressRow(row);
            if ((icon[13-row] & (1<<(15-i))) > 0) {
                ledOn();
            } else {
                ledOff();
            }
            shiftRow(1);
            delay(1);
        }
    }
    for (int i = 0; i < 192; i++) {
        shift();
        display(20);
    }
}

void setup() {
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(8, OUTPUT);
    
    
    pinMode(data_pin, OUTPUT);
    pinMode(clock_pin, OUTPUT);
    pinMode(clock_pin, HIGH);
    pinMode(13, OUTPUT);
    clear();

}

int counter = 0;
byte direction = 0;

void loop1() {
    while (1) {
        byte x = (sin(counter / 500.0) + 1) * 7;
        for (int row = 0; row < 14; row++) {
            if (row == x) {
                rowDraw(row, 1);
            } else {
                rowDrawBlank(row, 1);
            }
            displayMicroseconds(200);
            counter += 1;        
        }
    }
}

void loop() {
    for (int row = 0; row < 14; row++) {
        if (direction % 2 == 0) {
            rowDrawBlank(row, counter);
            rowDraw(row, 1);
            rowDrawBlank(row, 191-counter);            
        } else {
            rowDrawBlank(row, 191-counter);            
            rowDraw(row, 1);
            rowDrawBlank(row, counter);
        }
    }

    display(5);

    if (counter >= 191) {
        counter = 0;
        direction += 1;
    } else {
        counter += 1;
    }
}