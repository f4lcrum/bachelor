#include <SPI.h>
#include <epd1in54_V2.h>
#include <imagedata.h>
#include <epdpaint.h>
#include <stdio.h>

#define COLORED     1
#define UNCOLORED   0
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

    paint.Clear(COLORED);
    paint.Clear(UNCOLORED);
}

void loop()
{
    if(Serial1.available() > 0)      
    {
        paint.Clear(COLORED);
        input = Serial1.readStringUntil(TERMINATOR);        
        paint.DrawStringAt(0, y, input.c_str(), &Font16, UNCOLORED); // ak biele pozadie tak colored inak uncolored 
        epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight()); // 0, 0 znamena co vsetko updatnut (teda vsetko v tomto pripade)
        epd.DisplayPartFrame();
   }
}
