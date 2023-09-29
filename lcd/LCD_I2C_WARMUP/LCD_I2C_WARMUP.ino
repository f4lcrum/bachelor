#include <LiquidCrystal_I2C.h>

#define DINO 0
#define BUTTON PB11

int score = 0;
bool DINO_LOWER_POS = false;
LiquidCrystal_I2C lcd(0x27, 16, 2);
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

void setup() {
    Serial.begin(9600);
    pinMode(BUTTON, INPUT_PULLUP);
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, dino);
    lcd.setCursor(0, 0);
    lcd.print("Warm up!");
    delay(2000);
}

void score_board() {
    char score_board[13];
    sprintf(score_board, "Score:%05d", score);
    lcd.setCursor(5, 0);
    lcd.print(score_board);
}

void loop() {
    lcd.clear();
    int buttonState = digitalRead(BUTTON);
    if (buttonState == HIGH) {
        DINO_LOWER_POS = false;
        lcd.setCursor(0, 0);
        lcd.write(DINO);
    } else {
        DINO_LOWER_POS = true;
        lcd.setCursor(0, 1);
        lcd.write(DINO);
        score++;
    }
    score_board();
    delay(100);
}
