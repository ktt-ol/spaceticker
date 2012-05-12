
/*
clock 0 is the main clock at 666 Hz
the other clocks have different patterns that repeat
after 7 clock pulses of clock 0
*/

/*
We store the clock patterns as bits: 0 means no change,
1 means raising or falling (depending on the current clock_X_state).
A 7 pulse clock pattern consists of 14 flanks, starting from left
(most significant bit) to the right. Last (lowest) two bits are ignored.
*/
 
typedef struct {
    byte pin;
    byte state;
    unsigned int bits;
} Clock_t;


Clock_t clocks[4] = {
    {5, HIGH, 0b1111111111111111}, // puple
    {6, HIGH, 0b1010100010101011}, // gray/pink
    {7, LOW,  0b1000001000000011}, // yellow
    {8, HIGH, 0b1000101000100011}, // pink
};

byte high_pin = 9;

byte data_pin = 4; // white
byte data_state = LOW;

void setup() {
    for (int i = 0; i < 4; i++) {
        Clock_t *clock = &clocks[i];
        pinMode(clock->pin, OUTPUT);    
        digitalWrite(clock->pin, clock->state);
    }
    pinMode(data_pin, OUTPUT);
    pinMode(high_pin, OUTPUT);
    pinMode(high_pin, HIGH);
    pinMode(13, OUTPUT);
}

byte counter = 0;
void loop() {
    
    unsigned int mask = 1 << (15 - counter);

    for (int i = 0; i < 4; i++) {
        Clock_t *clock = &clocks[i];
        if (clock->bits & mask > 0) {
            clock->state = !clock->state;
            digitalWrite(clock->pin, clock->state);
        }
    }

    digitalWrite(high_pin, LOW);
    // delayMicroseconds(5);
    digitalWrite(high_pin, HIGH);

    if (counter % 2 == 0) {
        data_state = !data_state;
        digitalWrite(data_pin, data_state);
        digitalWrite(13, data_state);
    }

    delayMicroseconds(750);
}