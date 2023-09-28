#include "LedControl.h"

#define DIN_MATRIX PA0
#define CS_MATRIX PA1
#define CLK_MATRIX PA2

byte happy[8] = {0x8, 0x4, 0x72, 0x2, 0x2, 0x72, 0x4, 0x8};
byte sad[8] = {0x3, 0x4, 0xE8, 0x8, 0x8, 0xE8, 0x4, 0x3};
//TO-DO pridaj interrupt na rotary encoder kde pri kazdom stlaceni
// sa zmeni na SAD/HAPPY 

LedControl lc=LedControl(DIN_MATRIX, CLK_MATRIX, CS_MATRIX, 1);
void setup() {
    lc.shutdown(0, false);
    lc.setIntensity(0, 5);
    lc.clearDisplay(0);
    printByte(happy);
    delay(2000);
    lc.clearDisplay(0); 
    printByte(sad);
}

void printByte(byte character []) 
{
    for (int i=0;i<8;i++) 
    {
        lc.setRow(0,i,character[i]);
    }    
}

void loop() {
            
}
