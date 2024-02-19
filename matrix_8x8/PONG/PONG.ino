61#include "LedControl.h"

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
#define P1_HOME 0
#define P2_HOME 7
#define BOTTOM 7
#define UPPER 0
#define REFRESH_DELAY 250
#define RESTART_DELAY 1000

struct Ball {
    int x;
    int y;
    int x_velocity;
    int y_velocity;
};

struct Paddle {
    volatile int currentStateCLK;
    volatile int lastStateCLK;
    volatile int clk;
    volatile int dt;
    volatile int x_pos;
    volatile int y_pos;
};

int score_p1 = 0;
int score_p2 = 0;

byte playboard[8] = {0, 0, 0, 0, 0, 0, 0, 0};
Paddle p1;
Paddle p2;
Ball ball;
LedControl lc=LedControl(DIN_MATRIX, CLK_MATRIX, CS_MATRIX, 1);

void init_paddle(Paddle *p, int clk, int dt, int x, int y) {
    p->clk = clk;
    p->dt = dt;
    p->x_pos = x;
    p->y_pos = y;
    pinMode(p->clk, INPUT);
    pinMode(p->dt, INPUT);
    p->lastStateCLK = digitalRead(clk);
    attachInterrupt(digitalPinToInterrupt(dt), update_paddle, CHANGE);
}

void init_ball() {
    ball.x = 3;
    ball.y = 4;
    ball.x_velocity = random(2) ? -1 : 1;
    ball.y_velocity = random(2) ? -1 : 1;
}


void init_game() {
    init_paddle(&p1, CLK_ROT0, DT_ROT0, 0, 3);
    init_paddle(&p2, CLK_ROT1, DT_ROT1, 7, 4);
    init_ball();
    clear_playboard();
    update_paddle();
}

void setup() {
    init_game();
    lc.shutdown(0,false); //0.ty device zapne teda tvoj jediny matrix
    lc.setIntensity(0,8); 
    lc.clearDisplay(0);
}

void clear_playboard() {
    for (int i = 0; i < 8; i++) {
        playboard[i] = 0;
    }
}

bool paddle_range(Paddle *p, int x, int y) {
    return y >= p->y_pos - 1 && y <= p->y_pos + 1 && p->x_pos == x;
}

void paddle_move(Paddle *p) {
    for (int y = 0; y < 8; y++) {
        if (paddle_range(p, p->x_pos, y)) {
            put_val_playground(p->x_pos, y);
        }
        else {
            del_val_playground(p->x_pos, y);
        }
    }
}

void random_ball_bounce() {
    int chance = random(3);
    if (chance == 1) {
        ball.y_velocity = ball.y_velocity * -1;
    } else if (chance == 2) {
        ball.y_velocity = 0;
    }
    ball.x_velocity = ball.x_velocity * -1;
}

void adjust_score() {
    ball.x == P1_HOME ? score_p2++ : score_p1++;
    Serial.print("P1 ");
    Serial.print(score_p1);
    Serial.print(":");
    Serial.print(score_p2);
    Serial.println(" P2");
    delay(RESTART_DELAY);
    init_game();
}

void update_ball() {
	// vymazuje lopticku z displeja aby nedochadzalo k "Sandevistan" efektu, checkuje sa ale to aby neodsekol z
	// obidvoch rakiet
    if (!(paddle_range(&p1, ball.x, ball.y) || paddle_range(&p2, ball.x, ball.y))) {
        del_val_playground(ball.x, ball.y);
    }
    ball.x = ball.x + ball.x_velocity;
    ball.y = ball.y + ball.y_velocity;
    // check for ball interaction with paddles
    if (ball.x == P1_HOME || ball.x == P2_HOME){
        if (paddle_range(&p1, ball.x, ball.y) || paddle_range(&p2, ball.x, ball.y)){
            random_ball_bounce();
        }
        else {
            adjust_score();
        }
    }
    // bouncing off ceiling
    if (ball.y == BOTTOM || ball.y == UPPER) {
        ball.y_velocity = ball.y_velocity * -1;
    }
    put_val_playground(ball.x, ball.y);
    delay(REFRESH_DELAY);
}
// interrupt function 
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
    update_ball();
    print_byte(playboard);
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
