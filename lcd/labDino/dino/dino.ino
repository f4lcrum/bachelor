#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define WHITESPACE 32
#define DINO 0
#define CACTUS 1
#define BOMB 2
#define WIDTH 16

int score = 0;
bool DINO_LOWER_POS = true; // initial position, if false he is on upper floor
const byte BUTTON = PB11;
// First parameter is address of I2C bus slave, all 3 addresses (A0, A1, A2) were set to 1. Explanation: I2C have 7-bit address, the first four bits of PFC8754A are "0111", the lower 3 bits (A0, A1, A2) are set to 1, so the whole address is "0111111" in binary, 0x3F in he   x
// PB6 SCL, PB7 SDA
LiquidCrystal_I2C lcd(0x27, 16, 2); // adresy jednotlivych i2c deviceov, kedze su vsetky nastavene na jednicku tak adresa by mala byt 0x3F (Ide o chip PFC8754A)

byte dino [8]
{
    B00000,
    B00111,
    B00101,
    B10111,
    B11100,
    B11111,
    B01101,
    B01100,
};


byte cactus [8]
{
    B00011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11111,
    B01110,
    B01110
};

byte bomb [8]
{
    B00010,
    B00100,
    B01110,
    B10001,
    B10001,
    B10001,
    B10001,
    B01110
};

byte terrain[16];
boolean started = false;
boolean player_dead = false;

void init_terrain() {
    terrain[0] = DINO;
    for (int i = 1; i <= WIDTH - 1; i++){
        terrain[i] = WHITESPACE;
    }
}

void init_game() {
    score = 0;
    lcd.clear();
    init_terrain();
}

void setup()
{
    Serial.begin(9600);
    pinMode(BUTTON, INPUT_PULLUP);
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, dino);
    lcd.createChar(1, cactus);
    lcd.createChar(2, bomb);
    init_terrain();
}

int new_block() {
    long randNum = random(100);

    if (randNum < 90) {
        return WHITESPACE;
    }
    else if (randNum == 91) {
        return BOMB;
    }
    return CACTUS;
}

void score_board() {
    char score_board[13];
    sprintf(score_board, "Score:%05d", score);
    lcd.setCursor(5, 0);
    lcd.print(score_board);
}

void graphic() {
    score_board();
    for (int i = 0; i <= WIDTH - 2; i++) {
        if (DINO_LOWER_POS && i == 0 && terrain[i + 1] == WHITESPACE) {
            continue;
        }
        terrain[i] = terrain[i + 1];
    }
    delay(300);
    terrain[15] = new_block();
    for (int i = 0; i <= WIDTH - 1; i++) {
        lcd.setCursor(i, 1);
        lcd.write(terrain[i]);
    }
}

void game_over() {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("GAME OVER");

    lcd.setCursor(0, 1);
    lcd.print("Click to start!");
    int buttonState = digitalRead(BUTTON);
    while (buttonState == HIGH) {
        buttonState = digitalRead(BUTTON);
    };
}

void set_dino_lower() {
    lcd.setCursor(0, 0);
    lcd.write(WHITESPACE);
    terrain[0] = DINO;
}

void set_dino_higher() {
    lcd.setCursor(0, 0);
    lcd.write(DINO);
    terrain[0] = WHITESPACE;
}

void dino_jump() {
    if (DINO_LOWER_POS) {
        set_dino_lower();
    } else {
        set_dino_higher();
    }
}

void play() {
    dino_jump();
    graphic();
    if (DINO_LOWER_POS) {
        if (terrain[0] == CACTUS) {
            game_over();
            delay(5000);
            init_game();
        }
        if (terrain[0] == BOMB) {
            score += 50;
        }
        score++;
    }
}

void loop() {
    int buttonState = digitalRead(BUTTON);
    if (buttonState == HIGH) {
        DINO_LOWER_POS = false;
    } else {
        DINO_LOWER_POS = true;
    }

    play();
    delay(100);
}