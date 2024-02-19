#include "LedControl.h"

// first rotary encoder
#define CLK_ROT0 PB4
#define DT_ROT0 PB5
#define SW_ROT0 PA15
// second rotary encoder
#define CLK_ROT1 PB0
#define DT_ROT1 PB10
#define SW_ROT1 PB1
// 8X8 matrix
#define DIN_MATRIX PA0
#define CS_MATRIX PA1
#define CLK_MATRIX PA2
// Game settings
#define PADDLE_MIN 1
#define PADDLE_MAX 6

struct Paddle {
    volatile int currentStateCLK;
    volatile int lastStateCLK;
    volatile int clk;
    volatile int dt;
    volatile int x_pos;
    volatile int y_pos;
};


byte playboard[8] = {0, 0, 0, 0, 0, 0, 0, 0};

Paddle p1;
Paddle p2;
LedControl lc=LedControl(DIN_MATRIX, CLK_MATRIX, CS_MATRIX, 1);

void init_paddle(Paddle *p, int clk, int dt, int x, int y) {
    p->clk = clk;
    p->dt = dt;
    p->x_pos = x;
    p->y_pos = y;
    pinMode(p->clk, INPUT_PULLUP);
    pinMode(p->dt, INPUT_PULLUP);
    p->lastStateCLK = digitalRead(clk);
    attachInterrupt(digitalPinToInterrupt(dt), update_paddle, CHANGE);
}

void clear_playboard() {
    for (int i = 0; i < 8; i++) {
        playboard[i] = 0;
    }
}

bool paddle_range(Paddle *p, int y) {
    return y >= p->y_pos - 1 && y <= p->y_pos + 1;
}

void paddle_move(Paddle *p) {
    for (int y = 0; y < 8; y++) {
        if (paddle_range(p, y)) {
            put_val_playground(p->x_pos, y);
        }
        else {
            del_val_playground(p->x_pos, y);
        }
    }
}

void init_game() {
    init_paddle(&p1, CLK_ROT0, DT_ROT0, 0, 3);
    init_paddle(&p2, CLK_ROT1, DT_ROT1, 7, 4);
    clear_playboard();
    // update_paddle();
}

void setup() {
    init_game();
    lc.shutdown(0,false); //0.ty device zapne teda tvoj jediny matrix
    lc.setIntensity(0,8); 
    lc.clearDisplay(0);
}

void update_paddle() {
   p1.currentStateCLK = digitalRead(p1.clk);
   p2.currentStateCLK = digitalRead(p2.clk);

   if (p2.currentStateCLK != p2.lastStateCLK && p2.currentStateCLK == 1) {
        if (digitalRead(p2.dt) != HIGH) {
            if (p2.y_pos > PADDLE_MIN) {
                p2.y_pos--;
            }
        }
        else {
            if (p2.y_pos < PADDLE_MAX) {
                p2.y_pos++;
            }
        }
    }
    if (p1.currentStateCLK != p1.lastStateCLK && p1.currentStateCLK == 1) {
        if (digitalRead(p1.dt) != HIGH) {
            if (p1.y_pos > PADDLE_MIN) {
                p1.y_pos--;
            }
        }
        else {
            if (p1.y_pos < PADDLE_MAX) {
                p1.y_pos++;
            }
        }
    }
    
    paddle_move(&p2);
    p2.lastStateCLK = p2.currentStateCLK;
    paddle_move(&p1);
    p1.lastStateCLK = p1.currentStateCLK;
}

void loop() {
    print_byte(pla);
}

void put_val_playground(int x, int y) {
    playboard[y] |= 1 << x;
}

void del_val_playground(int x, int y) {
    playboard[y] &= ~(1 << x);
}

void print_byte(byte character [])
{
    int i = 0;
    for(i=0;i<8;i++)
    {
        lc.setRow(0,i,character[i]);
    }
}
