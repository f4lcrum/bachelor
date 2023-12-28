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


Adafruit_SH1106 display(OLED_DC, OLED_RESET, OLED_CS, SPI_PORT);
int last_SW_state = LOW;
int last_btn_confirm_state = LOW;
int currentStateCLK;
int lastStateCLK;
int encoder_pos = 0;
int _hours = 0;
int _minutes = 0;
int _seconds = 0;
enum SetMode {
    HOURS,
    MINUTES,
    SECONDS
};
SetMode setTimeMode = HOURS;

void setup() {
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

void refresh_display() {
    display.clearDisplay();
    display.display();
    alarm_menu();
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
}

void init_encoder() {
    pinMode(CLK, INPUT);
    pinMode(DT, INPUT);
    pinMode(SW, INPUT_PULLUP);
    lastStateCLK = digitalRead(CLK);
    attachInterrupt(digitalPinToInterrupt(CLK), read_encoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(DT), read_encoder, CHANGE);
}

void read_button() {
    setTimeMode = static_cast<SetMode>((setTimeMode + 1) % 3);
    encoder_pos = setTimeMode == HOURS ? _hours : setTimeMode == MINUTES ? _minutes : _seconds ;
    refresh_display();
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

void button_logic() {
    // rotary encoder logic is in interrupts
    int btnState = digitalRead(SW);
    if (last_SW_state == LOW && btnState == HIGH) {
        read_button();
    }
    last_SW_state = btnState;  
}

void loop() {
    button_logic();
    refresh_display();
    delay(70);
}
