#include <RTClib.h>
#include <temp_sensor_ds18b20.h>
#include <Adafruit_SH1106_STM32.h>
RTC_DS3231 rtc;
#define OLED_RESET -1
#define POWER_BUTTON PA0
Adafruit_SH1106 display(OLED_RESET);
Temp_sensor_ds18b20 sensor(PB3);

void init_oled() {
  display.begin(SH1106_SWITCHCAPVCC, SH1106_I2C_ADDRESS);  // initialize with the I2C addr 0x3C (for the 128x64)
  display.display();
  delay(2000);
  display.clearDisplay();
}

void init_rtc() {
  rtc.begin();
  if(rtc.lostPower()) {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   }

  //we don't need the 32K Pin, so disable it
  rtc.disable32K();  
}

void setup() {
  pinMode(POWER_BUTTON, INPUT_PULLUP);
  Serial.begin(9600);
  init_oled();
  init_rtc(); 
}

void set_print(int color, int text_size, int pos_x, int pos_y) {
  display.setTextSize(text_size);
  display.setTextColor(color);
  display.setCursor(pos_x, pos_y);
}

void show_time() {
  char date[10] = "hh:mm:ss";
  rtc.now().toString(date);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(28, 0);
  display.println("Current Time");
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(18,12);
  display.println(date);
}

void show_temp() {
  double temp = sensor.get_temp();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(28, 36);
  display.println("Current Temp");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(24,48);
  display.print(temp);
  display.cp437(true);
  display.write(167);
  display.print("C");
}

void power_off() {
  display.display();
  display.clearDisplay();;
  int buttonState = digitalRead(POWER_BUTTON);
  while (true) {
    int buttonState = digitalRead(POWER_BUTTON);
    if (buttonState == LOW) {
      break;
    } 
    delay(50);
  }
}

void loop() {
  int buttonState = digitalRead(POWER_BUTTON);
  if (buttonState == LOW) {
    power_off();
  }
  show_time();
  show_temp();
  display.display();
  display.clearDisplay();;
  delay(50);
}
