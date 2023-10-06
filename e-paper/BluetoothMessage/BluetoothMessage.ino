#include <SPI.h>
#include <epd1in54_V2.h>
#include <imagedata.h>
#include <epdpaint.h>
#include <stdio.h>

#define COLORED     0
#define UNCOLORED   1
#define TERMINATOR '\n'

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
    paint.Clear(UNCOLORED); 
    paint.Clear(COLORED);
}

void loop()
{
    if(Serial1.available() > 0)      
    {
        if (y + 16 >= paint.GetWidth()) {
            Serial1.println("No space, clearing");
            paint.Clear(COLORED);
            paint.Clear(UNCOLORED);        
        }
        input = Serial1.readStringUntil(TERMINATOR);        
        paint.DrawStringAt(0, y, input.c_str(), &Font16, UNCOLORED);
        epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
        epd.DisplayPartFrame();
        y += 16;
   }
}
