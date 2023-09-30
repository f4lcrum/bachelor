#include <RTClib.h>
#include <temp_sensor_ds18b20.h>
#include <Adafruit_SH1106_STM32.h>

#define OLED_RESET -1
#define POWER_BUTTON PA0
#define UNITS_CHANGE_BUTTON PA1
#define SHOW_DATE_BUTTON PA2
#define BUTTON_DELAY 50
#define CELSIUS "C"
#define FAHRENHEIT "F"
#define TEMP_SENSOR_PIN PB3

// SCL and SDA pins both of OLED and RTC to PB6 and PB7
RTC_DS3231 rtc;
Adafruit_SH1106 display(OLED_RESET);
Temp_sensor_ds18b20 sensor(TEMP_SENSOR_PIN);
boolean american = false;
boolean print_date = true;
// all buttons states
int last_state_datebutton = LOW;
int curr_state_datebutton;
int last_state_unitsbutton = LOW;
int curr_state_unitsbutton;
int last_state_pwrbutton = LOW;
int curr_state_pwrbutton;

void init_oled() {
	display.begin(SH1106_SWITCHCAPVCC, SH1106_I2C_ADDRESS);  // initialize with the I2C addr 0x3C (for the 128x64)
  	display.display();
  	delay(2000);
  	display.clearDisplay();
}

double fahrenheit(double celsius) {
  	return (celsius * 9/5) + 32;
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
  	pinMode(UNITS_CHANGE_BUTTON, INPUT_PULLUP);
  	pinMode(SHOW_DATE_BUTTON, INPUT_PULLUP);
  	Serial.begin(9600);
  	init_oled();
  	init_rtc();
}

void set_print(int color, int text_size, int pos_x, int pos_y) {
  	display.setTextSize(text_size);
  	display.setTextColor(color);
  	display.setCursor(pos_x, pos_y);
}

void show_date() {
  	DateTime today =  rtc.now();
  	display.setTextSize(1);
  	display.setTextColor(WHITE);
  	display.setCursor(28, 36);
  	display.println("Current Date");
  	display.setTextColor(BLACK, WHITE); // 'inverted' text
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(8,48);
	char today_str[11] = "XX:YY:ZZZZ";
	snprintf(today_str, sizeof(today_str), "%02d.%02d.%02d", today.day(), today.month(), today.year());
	display.print(today_str);
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

void show_temp(double temp, char *symbol) {
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
	display.print(symbol);
}

void power_off() {
	display.display();
	display.clearDisplay();;
	int power_button_state = digitalRead(POWER_BUTTON);
	while (true) {
		int power_button_state = digitalRead(POWER_BUTTON);
		if (power_button_state == LOW) {
            Serial.println("Turning on");
			break;
		}
		delay(BUTTON_DELAY);
	}
}

void buttons_logic() {
    int power_button_state = digitalRead(POWER_BUTTON);
    int units_button_state = digitalRead(UNITS_CHANGE_BUTTON);
    int show_date_button_state = digitalRead(SHOW_DATE_BUTTON);

    if (last_state_pwrbutton == LOW && power_button_state == HIGH) {
        power_off();
    }
    if (last_state_unitsbutton == LOW && units_button_state == HIGH) {
        american = ! american;
    }
    if (last_state_datebutton == LOW && show_date_button_state == HIGH) {
        print_date = ! print_date;
    }
    if (print_date) {
        show_date();
    }
    else {
        american ? show_temp(fahrenheit(sensor.get_temp()), FAHRENHEIT) : show_temp(sensor.get_temp(), CELSIUS);
    }
    last_state_pwrbutton = power_button_state;
    last_state_unitsbutton = units_button_state;
    last_state_datebutton = show_date_button_state;
}

void loop() {
	buttons_logic();
	show_time();
	display.display();
	display.clearDisplay();;
	delay(BUTTON_DELAY);
}
