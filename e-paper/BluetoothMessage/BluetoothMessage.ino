#include <SPI.h>
#include <epd1in54_V2.h>
#include <imagedata.h>
#include <epdpaint.h>
#include <stdio.h>

// #define COLORED     1
// #define UNCOLORED   0
#define TERMINATOR '\n'

#define COLORED     0
#define UNCOLORED   1

Epd epd;
unsigned char image[10000];
Paint paint(image, 0, 0);

int y = 0;
String input; 

void setup()
{
    Serial1.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    epd.LDirInit();
    epd.Clear();

    paint.SetWidth(200);
    paint.SetHeight(200);
    paint.Clear(COLORED);
    reload();
}

void reload() {
    epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
    epd.DisplayPartFrame();
}

void loop()
{
    if(Serial1.available() > 0)      
    {
        if (y + 16 >= paint.GetWidth()) {
            Serial1.println("No space, clearing");
            epd.Clear();
            paint.Clear(COLORED); 
            reload();
            y = 0;       
        }
        input = Serial1.readStringUntil(TERMINATOR);        
        paint.DrawStringAt(0, y, input.c_str(), &Font16, UNCOLORED);
        reload();
        y += 16;
   }
}
