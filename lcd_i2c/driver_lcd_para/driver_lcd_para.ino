#include <Wire.h> 

#define LCD_CLEAR 0x01
#define LCD_SETDDRAMADDR 0x80
#define LCD_SECONDROWADDR 0x40
#define LCD_SETTOHOME 0x02
#define LCD_FUNCTIONSET 0x20


// FunctionSet flags
#define LCD_5x10FONT 0x04 // No support for 16x2 display 
#define LCD_5x8FONT 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00 // No support for 16x2 display
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00


#define EN PB10
#define RW PB4
#define RS PB11
#define D0 PB12
#define D1 PB13
#define D2 PB14
#define D3 PB15
#define D4 PB0
#define D5 PB1
#define D6 PC13
#define D7 PC14

uint8_t data_pins[8] {D0, D1, D2, D3, D4, D5, D6, D7};

void send_to_lcd(char data, int rs) {
  digitalWrite(RS, rs);
  for (int i = 0; i < 8; i++) {
    pinMode(data_pins[i], OUTPUT);
    digitalWrite(data_pins[i], ((data>>i)&0x01));
  }
  digitalWrite(EN, LOW);
  delay(2);
  digitalWrite(EN, HIGH);
  delay(50);
}

void lcd_send_command(char cmd)
{
  send_to_lcd(cmd, 0);   
}

void lcd_send_data(char data)
{
  send_to_lcd(data, 1);  
}


void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= LCD_SETDDRAMADDR;
            break;
        case 1:
            col |= (LCD_SETDDRAMADDR | LCD_SECONDROWADDR);
            break;
    }
    lcd_send_command (col);
}

void lcd_home() {
  lcd_send_command(LCD_SETTOHOME);  
}


void lcd_init() {
  pinMode(RS, OUTPUT);
  pinMode(EN, OUTPUT);
  delay(50);
  lcd_send_command(LCD_FUNCTIONSET | LCD_8BITMODE);
  delay(5);
  lcd_send_command(LCD_FUNCTIONSET | LCD_8BITMODE);
  delay(1);
  
  lcd_send_command(LCD_FUNCTIONSET | LCD_8BITMODE);
  delay(10);
  
  lcd_send_command(LCD_FUNCTIONSET | LCD_8BITMODE | LCD_2LINE | LCD_5x8FONT); // Function Set: 8-bit mode, 2-line display, 5x8 dots
  delayMicroseconds(50);
  lcd_send_command(0x08); // Display OFF
  delayMicroseconds(50);
  lcd_send_command(LCD_CLEAR); // Clear display
  delay(3);
  lcd_send_command(0x06); // Entry Mode Set: Increment cursor, no shift
  delayMicroseconds(50);
  lcd_send_command(0x0C); // Display ON/OFF Control: Display ON, Cursor OFF, Blink OFF
  delayMicroseconds(50);
  Serial.print("End of init");
}


void lcd_send_string (char *str)
{
  while (*str) lcd_send_data(*str++); 
}

void setup()
{
   lcd_init();
   lcd_put_cur(0, 4);
   delay(2000);
   lcd_home();
   lcd_send_string("Heil Hydra !");
}

void loop() {
 
}
