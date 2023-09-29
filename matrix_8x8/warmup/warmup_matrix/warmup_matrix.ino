#include "LedControl.h"

#define DIN_MATRIX PA0
#define CS_MATRIX PA1
#define CLK_MATRIX PA2
// rotary encoder
#define CLK_ROT0 PB4
#define DT_ROT0 PB5
#define SW_ROT0 PA15
byte happy[8] = {0x8, 0x4, 0x72, 0x2, 0x2, 0x72, 0x4, 0x8};
byte sad[8] = {0x3, 0x4, 0xE8, 0x8, 0x8, 0xE8, 0x4, 0x3};
bool is_happy = true;
LedControl lc=LedControl(DIN_MATRIX, CLK_MATRIX, CS_MATRIX, 1);
//TO-DO pridaj interrupt na rotary encoder kde pri kazdom stlaceni
// sa zmeni na SAD/HAPPY 

void change_mood() {
    lc.clearDisplay(0);
    is_happy = ! is_happy;
    if (is_happy) {
        print_byte(happy);
    }
    else {
        print_byte(sad);
    }    
}

void setup() {
    lc.shutdown(0, false);
    lc.setIntensity(0, 5);
    lc.clearDisplay(0);
    attachInterrupt(CLK_ROT0, change_mood, CHANGE);
}

void print_byte(byte character []) 
{
    for (int i=0;i<8;i++) 
    {
        lc.setRow(0,i,character[i]);
    }    
}

void loop() {    
}
