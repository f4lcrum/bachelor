#include <RTClib.h>
#include <temp_sensor_ds18b20.h>
#include <Adafruit_SH1106_STM32.h>

#define OLED_RESET -1
#define TEMP_SENSOR_PIN PB3
#define CELSIUS "C"
#define FAHRENHEIT "F"
// SCL and SDA pins both of OLED and RTC to PB6 and PB7
RTC_DS3231 rtc;
Adafruit_SH1106 display(OLED_RESET);
Temp_sensor_ds18b20 sensor(TEMP_SENSOR_PIN);

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

void show_time() {
    char curr_time[10] = "hh:mm:ss";
    rtc.now().toString(curr_time);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(28, 0);
    display.println("Current Time");
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(18,12);
    display.println(curr_time);
}

void show_temp(double temperature, char *symbol) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(28, 36);
    display.println("Current Temp");
    display.setTextColor(BLACK, WHITE); // 'inverted' text
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(24,48);
    display.print(temperature);
    display.cp437(true);
    display.write(167);
    display.print(symbol);
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    init_oled();
    init_rtc();
}

void loop() {
    show_time();
    show_temp(sensor.get_temp(), CELSIUS);
    display.display();
    display.clearDisplay();
    delay(200);
}
