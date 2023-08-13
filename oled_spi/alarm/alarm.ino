// rotary encoder logic inspired by: https://lastminuteengineers.com/rotary-encoder-arduino-tutorial/
// Author: Kristian Oravec

#include <Adafruit_SH1106_STM32.h>
#include <RTClib.h>

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

//RTC SQW
#define CLOCK_INTERRUPT_PIN PB3


RTC_DS3231 rtc;
Adafruit_SH1106 display(OLED_DC, OLED_RESET, OLED_CS, SPI_PORT);

int currentStateCLK;
int lastStateCLK;
int encoder_pos = 0;

int hour_color = WHITE;
int minute_color = WHITE;
int second_color = WHITE;
boolean alarm_set = false;
char alarm_time[9] = "XX:YY:ZZ";  // this is for time saved for alarm

void setup() {
    pinMode(BUTTON_CONFIRM, INPUT_PULLUP);
    init_oled();
    init_rtc();
    init_encoder();
}

int _hours = 0;
int _minutes = 0;
int _seconds = 0;

enum SetMode {
    HOURS,
    MINUTES,
    SECONDS  
};

SetMode setTimeMode = HOURS;

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
    attachInterrupt(digitalPinToInterrupt(CLK), read_encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(DT), read_encoder, CHANGE);
}

void init_rtc() {
    rtc.begin();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    rtc.disable32K(); 
    pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), onAlarm, FALLING);

    // set alarm 1, 2 flag to false (so alarm 1, 2 didn't happen so far)
    // if not done, this easily leads to problems, as both register aren't reset on reboot/recompile
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);

    // stop oscillating signals at SQW Pin
    // otherwise setAlarm1 will fail
    rtc.writeSqwPinMode(DS3231_OFF);

    // turn off alarm 2 (in case it isn't off already)
    // again, this isn't done at reboot, so a previously set alarm could easily go overlooked
    rtc.disableAlarm(2); 
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

void print_clock_now() {
    char curr_time[10] = "hh:mm:ss";
    rtc.now().toString(curr_time);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(40,54);
    display.println(curr_time);
    
}

void alarm_menu() {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Alarm: ");
    alarm_set ? display.print(alarm_time) : display.print("NOT SET");
  
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
    print_clock_now();
    display.display();
}

void read_encoder() {
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
        if (digitalRead(DT) != currentStateCLK) {
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
    encoder_pos = 0; 
    refresh_display(); 
}

void start_alarm() {
    alarm_set = true;
    snprintf(alarm_time, sizeof(alarm_time), "%02d:%02d:%02d", _hours, _minutes, _seconds);   
    refresh_display();

    DateTime now = rtc.now();
    DateTime alarmTime = DateTime(now.year(), now.month(), now.day(), _hours, _minutes, _seconds);
    rtc.setAlarm1(alarmTime, DS3231_A1_Date );
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
        btnState = digitalRead(SW);\
        delay(50);  
    }
    rtc.clearAlarm(1);
}

void loop() {
    int btnState = digitalRead(SW);
    int btnConfirmState = digitalRead(BUTTON_CONFIRM);
    if (btnState == LOW) {
        read_button(); 
        delay(300);       
    }
    if (btnConfirmState == LOW) {
        start_alarm();
        delay(50);            
    }  
}
