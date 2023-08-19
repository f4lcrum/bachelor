// VSS -> GND
// VDD -> 5V
// V0 -> GND
// A -> Backlight Anode
// K -> Backlight Cathode
//The RS (data/instruction select) bit tells the LCD whether we are going to send an instruction or a piece of data.

// The R/W (read/write) bit tells the LCD whether we wish to read from or to write to RAM.
// The E (enable) bit tells the LCD when it should read the data lines.
#define LCD_CLEAR 0x01
#define LCD_SETDDRAMADDR 0x80
#define LCD_SECONDROWADDR 0x40
#define LCD_SETTOHOME 0x02
#define LCD_FUNCTIONSET 0x20
#define LCD_DISPLAYCONTROL 0x08
#define LCD_ENTRYMODESET 0x04


// EntryMode flags
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// FunctionSet flags
#define LCD_5x8FONT 0x00
#define LCD_2LINE 0x08
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00


// DisplayControl flags
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// RS bit
#define LCD_CMD 0
#define LCD_DATA 1

#define NIBBLE_MASK 0x0f

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


uint8_t data_pins[8] {D0, D1, D2, D3, D4, D5, D6, D7}; // 8bit
// uint8_t data_pins[4] {D4, D5, D6, D7}; // 4bit

bool four_bit_mode;

void set_rs(int rs) {
    digitalWrite(RS, rs);
}

void send_to_lcd(char data, int rs) {
    set_rs(rs);
    four_bit_mode ? send_nibble(data) : send_byte(data);

    digitalWrite(EN, LOW);
    delay(2);
    digitalWrite(EN, HIGH);
    delay(50);
}

int get_nth_bit(char data, int n) {
    return ((data>>n)&0x01);
}

void send_byte(char data) {
    for (int i = 0; i < 8; i++) {
        digitalWrite(data_pins[i], get_nth_bit(data, i));
    }
}

void init_pins() {
    pinMode(RW, OUTPUT);
    digitalWrite(RW, LOW); // Set to write mode
    pinMode(RS, OUTPUT);
    pinMode(EN, OUTPUT);
    int pin_count;
    four_bit_mode ? pin_count = 4 : pin_count = 8;
    for (int i = 0; i < pin_count; i++) {
        pinMode(data_pins[i], OUTPUT);
    }
    set_rs(LCD_CMD);
}

void send_nibble(char data) {
    for (int i = 0; i < 4; i++) {
        digitalWrite(data_pins[i], get_nth_bit(data, i));
    }
}

void lcd_send_command(char cmd)
{
    if (four_bit_mode){
        send_to_lcd(get_upper_nibble(cmd), LCD_CMD);
        send_to_lcd(get_lower_nibble(cmd), LCD_CMD);
    } else {
        send_to_lcd(cmd, LCD_CMD);
    }
}

char get_lower_nibble(char data) {
    return data & NIBBLE_MASK; // Example: 1110 0100 & 0000 1111 = 0000 0100
}

char get_upper_nibble(char data) {
    return (data>>4) & NIBBLE_MASK; // Example: 1110 0100 >> 4 = 0000 1110; 0000 1110 & 0000 1111 = 0000 1110
}

void lcd_send_data(char data)
{
    if (four_bit_mode){
        send_to_lcd(get_upper_nibble(data), LCD_DATA);
        send_to_lcd(get_lower_nibble(data), LCD_DATA);
    } else {
        send_to_lcd(data, LCD_DATA);
    }
}

void lcd_put_cur(int row, int col)
{
    if (row == 0) {
        col |= LCD_SETDDRAMADDR;
    }
    else if (row == 1) {
        col |= (LCD_SETDDRAMADDR | LCD_SECONDROWADDR);
    }
    lcd_send_command (col);
}

void lcd_home() {
    lcd_send_command(LCD_SETTOHOME);
}

void lcd_init(bool _4bit_mode) {
    init_pins();
    delay(50);
    four_bit_mode = _4bit_mode;
    delay(10);

    if (_4bit_mode){
        lcd_send_command(get_upper_nibble(LCD_FUNCTIONSET | LCD_8BITMODE));
        delayMicroseconds(4500); // wait min 4.1ms

        lcd_send_command(get_upper_nibble(LCD_FUNCTIONSET | LCD_8BITMODE));
        delayMicroseconds(4500); // wait min 4.1ms

        lcd_send_command(get_upper_nibble(LCD_FUNCTIONSET | LCD_8BITMODE));
        delayMicroseconds(150);

        lcd_send_command(get_upper_nibble(LCD_FUNCTIONSET | LCD_4BITMODE));
        lcd_send_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8FONT);
    } else {
        lcd_send_command(LCD_FUNCTIONSET | LCD_8BITMODE);
        delay(5);

        lcd_send_command(LCD_FUNCTIONSET | LCD_8BITMODE);
        delay(1);

        lcd_send_command(LCD_FUNCTIONSET | LCD_8BITMODE);
        lcd_send_command(LCD_FUNCTIONSET | LCD_8BITMODE | LCD_2LINE | LCD_5x8FONT);
    }
    delayMicroseconds(50);
    lcd_send_command(LCD_CLEAR);
    delay(3);
    lcd_send_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
    delayMicroseconds(50);
    lcd_send_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON);
    delayMicroseconds(50);

}

void lcd_send_string (char *str)
{
    while (*str != '\0') {
        lcd_send_data(*str);
        str++; // Move to the next character in the string
    }
}

void setup()
{
    Serial.begin(9600);
    lcd_init(false);
    lcd_home();
    lcd_put_cur(0,4);

    delay(2000);

    lcd_send_string("IoT je zabava!");
    lcd_put_cur(1, 14);
    lcd_send_string("H");
}

void loop() {

}
