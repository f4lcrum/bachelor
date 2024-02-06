// rotary encoder logic inspired by: https://lastminuteengineers.com/rotary-encoder-arduino-tutorial/
// Author: Kristian Oravec
#include <Adafruit_SH1106_STM32.h>

#define SPI_PORT   1 // 1:SPI1, 2:SPI2
// hardware SPI1 (the default case): SDA(MOSI)=PA7,  SCK=PA5
//          SPI2                   : SDA(MOSI)=PA15, SCK=PA13
// rotary encoder
#define CLK PA0
#define DT PC15
#define SW PC14 // this is push button of encoder
// oled pins
#define OLED_DC    PB15
#define OLED_CS    PB12
#define OLED_RESET PB14
// basic push buttons
#define BUTTON_CONFIRM PA11

Adafruit_SH1106 display(OLED_DC, OLED_RESET, OLED_CS, SPI_PORT);
int last_SW_state = LOW;
int lastButtonPress = 0;
volatile int currentStateCLK;
volatile int lastStateCLK;
volatile int encoder_pos = 0;
int _hours = 0;
int _minutes = 0;
int _seconds = 0;
boolean alarm_set = false;
char alarm_time[9] = "XX:YY:ZZ";  // this is for time saved for alarm

enum SetMode {
    HOURS,
    MINUTES,
    SECONDS
};
SetMode setTimeMode = HOURS;

void setup() {
    Serial.begin(9600);
    pinMode(BUTTON_CONFIRM, INPUT_PULLUP);
    init_oled();
    init_encoder();
    refresh_display();
}

void init_oled() {
    display.begin(SH1106_SWITCHCAPVCC);
    display.display();
    delay(2000);
    display.clearDisplay();
}

void init_encoder() {
    pinMode(CLK, INPUT);
    pinMode(DT, INPUT);
    pinMode(SW, INPUT_PULLUP);
    lastStateCLK = digitalRead(CLK);
    attachInterrupt(digitalPinToInterrupt(DT), read_encoder, CHANGE);
}

void print_settings() {
    char* mode;
    if (setTimeMode == HOURS) {
        mode = " Hours ";
    } else if (setTimeMode == MINUTES) {
        mode = "Minutes";
    } else {
        mode = "Seconds";
    }
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(24, 32);
    display.println(mode);
    display.display();
}

void print_time() {
    char time_str[9] = "XX:YY:ZZ";
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", _hours, _minutes, _seconds);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(18, 14);
    display.println(time_str);
}

void alarm_menu() {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    
    switch (setTimeMode){
        case HOURS:
            _hours = encoder_pos;
            break;
        case MINUTES:
            _minutes = encoder_pos;
            break;
        case SECONDS:
            _seconds = encoder_pos;
            break;
    }
    print_time();
    print_settings();
    display.display();
}

void read_encoder() {
    alarm_set = false;
    display.clearDisplay();
    int lower_bound;
    int upper_bound;
    if (setTimeMode == HOURS){
        lower_bound = 0;
        upper_bound = 23;
    } else {
        lower_bound = 0;
        upper_bound = 59;
    }
    currentStateCLK = digitalRead(CLK);

    if (currentStateCLK != lastStateCLK && currentStateCLK == 1) {
        if (digitalRead(DT) != HIGH) {
            if (encoder_pos > lower_bound){
                encoder_pos--;
            }
        }
        else {
            if (encoder_pos < upper_bound)
                encoder_pos++;
            }
        }
    lastStateCLK = currentStateCLK;
    alarm_menu();
}

void refresh_display() {
    display.clearDisplay();
    display.display();
    alarm_menu();
}

void read_button() {
    setTimeMode = static_cast<SetMode>((setTimeMode + 1) % 3);
    encoder_pos = setTimeMode == HOURS ? _hours : setTimeMode == MINUTES ? _minutes : _seconds ;
    refresh_display();
}


void onAlarm() {
    int btnState = digitalRead(SW);
    display.clearDisplay();
    display.display();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(28, 24);
    display.println("ALARM!!!");
    display.display();
    alarm_set = false;
    while (btnState != LOW) {
        btnState = digitalRead(SW);
        delay(50);
    }
}

void start_alarm() {
    alarm_set = true;
    refresh_display();
}

void countdown() {
    display.clearDisplay();
    display.display();  
    print_time();
    display.display();
    
    if (_seconds == 0 && _minutes != 0) {
        _minutes--;
        _seconds = 59;
    }
    else if (_minutes == 0 && _hours != 0) {
        _hours--;
        _minutes = 59;    
    }
    else {
		_seconds ? _seconds-- : _seconds;
	} 
   
    if (_seconds == 0 && _minutes == 0 && _hours == 0) {
        onAlarm();
    }
    display.display();
    delay(900);
}

void button_logic() {
    // rotary encoder logic is in interrupts
    int btnState = digitalRead(SW);
    int btnConfirmState = digitalRead(BUTTON_CONFIRM);
    if (last_SW_state == LOW && btnState == HIGH) {
        read_button();
        //delay(300);
    }
    if (btnConfirmState == HIGH) {
        start_alarm();
    }
    last_SW_state = btnState;
}

void loop() {
    if (alarm_set) {
        Serial.println("nastaveny alarm");
        countdown();   
    }
    else {
        button_logic();
        delay(70);
    }
}
