#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// #define BUTTON_STATE HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)
const byte BUTTON = PB11;
// First parameter is address of I2C bus slave, all 3 addresses (A0, A1, A2) were set to 1. Explanation: I2C have 7-bit address, the first four bits of PFC8754A are "0111", the lower 3 bits (A0, A1, A2) are set to 1, so the whole address is "0111111" in binary, 0x3F in he   x  
LiquidCrystal_I2C lcd(0x3F, 16, 2); // adresy jednotlivych i2c deviceov, kedze su vsetky nastavene na jednicku tak adresa by mala byt 0x3F (Ide o chip PFC8754A)

#define WHITESPACE 32
#define WIDTH 16

byte monster[] = {
  B10001,
  B11011,
  B00100,
  B00100,
  B01110,
  B01110,
  B01110,
  B11111
};

byte tree[] = {
  B10101,
  B01110,
  B10101,
  B01110,
  B10101,
  B00100,
  B00100,
  B00100
};


byte terrain[16];
boolean started = false;
boolean player_dead = false;


void init_terrain() {
  for (int i = 0; i <= WIDTH - 1; i++){
    terrain[i] = WHITESPACE;  
  }
}


void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, monster);
  lcd.createChar(1, tree);
  // Init playground 
  init_terrain();
}


int new_block() {
  long randNum = random(100);
  if (randNum <= 8) {
    return 0;  
  }
  else if (randNum <= 90) {
    return WHITESPACE;
  }
  return 1;
}


void graphic() {
  for (int i = 0; i <= WIDTH - 2; i++) {
    terrain[i] = terrain[i + 1];
  }
  delay(200);
  terrain[15] = new_block();
  for (int i = 0; i <= WIDTH - 1; i++) {
     lcd.setCursor(i, 1);
     lcd.write(terrain[i]);
  }
}


void play() {
  graphic();
}

void loop() {
  byte buttonState = digitalRead(BUTTON);
  
 
  play();
}


