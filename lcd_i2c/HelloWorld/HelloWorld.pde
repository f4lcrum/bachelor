#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


// First parameter is address of I2C bus slave, all 3 addresses (A0, A1, A2) were set to 1. Explanation: I2C have 7-bit address, the first four bits of PFC8754A are "0111", the lower 3 bits (A0, A1, A2) are set to 1, so the whole address is "0111111" in binary, 0x3F in he   x  
LiquidCrystal_I2C lcd(0x3F, 16, 2); // adresy jednotlivych i2c deviceov, kedze su vsetky nastavene na jednicku tak adresa by mala byt 0x3F (Ide o chip PFC8754A)

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Hello, World!");
}


void loop()
{
}
