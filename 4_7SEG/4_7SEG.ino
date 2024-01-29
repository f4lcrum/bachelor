#define D1 PA9
#define D2 PA8
#define D3 PB15
#define D4 PB11
#define pinA PB8
#define pinB PA5
#define pinC PA0
#define pinD PC13
#define pinE PC14
#define pinF PA6
#define pinG PA1
#define pinDP PC15 // decimal point
#define BUTTON PB9
#define DIGITS 4
#define SEGMENTS 8
#define TIMER_DURATION 9999 // between 0 and 9999 pls

int dPins[DIGITS] = {D4, D3, D2, D1};
int time_point = millis();
int timer = 0;
const int clear_seg = 0xff;
const int pins[SEGMENTS] = {pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP};
const int numbers[10] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};
int last_state = LOW;
int current_state;

void setup() {
    for (int i = 0; i < SEGMENTS; i++){
        pinMode(pins[i], OUTPUT);
    }
    for (int i = 0; i < DIGITS; i++) {
        pinMode(dPins[i], OUTPUT);    
    }
    pinMode(BUTTON, INPUT_PULLUP);
    set_timer();
}

void print_number(int num) {
    if (num < 0) {
        num = 0;
    }
    if (num > 9999) {
        num = 9999;
    }
    display_number(0, num / 1000);
    num = num % 1000;

    display_number(1, num / 100);
    num = num % 100;

    display_number(2, num / 10);
    num = num % 10;

    display_number(3, num / 1);
    num = num % 1;
}

void loop() {
    current_state = digitalRead(BUTTON);
    if (last_state == LOW && current_state == HIGH) {
        set_timer();
    }
    print_number(timer);
    if ((millis() - time_point) > 1000) {
        time_point = millis();
        if (timer > 0) 
        {
            timer--;
        }
    }
    last_state = current_state;
}

void display_number(int id, unsigned char num)
{
    cipher_enable(id);
    for (int i = 0; i < SEGMENTS; i++) {
        digitalWrite(pins[i], (numbers[num]>>i)&0x01);
    }
    delay(5);
}

void set_timer() {
    if (0 > TIMER_DURATION  || TIMER_DURATION > 9999) {
        return;
    }
    timer = TIMER_DURATION;
}

void clear_segments() {
    for (int i = 0; i < SEGMENTS; i++) {
        digitalWrite(pins[i], HIGH);
    }
}

void cipher_enable(int id) {
    for (int j = 0; j < 4; j++) digitalWrite(dPins[j], LOW);
    digitalWrite(dPins[id], HIGH);
    clear_segments();
}

void print_decimal() // writing the decimal point
{
    digitalWrite(pinDP, HIGH);
    delay(1);
}
