#include <bluepill_ws2812.h> // https://github.com/FearlessNight/bluepill_ws2812
#include <temp_sensor_ds18b20.h>

bluepill_neopixel PIX;
Temp_sensor_ds18b20 sensor(PB3);
#define NUM_PIXELS 8
#define string_port GPIOA
#define string_pin  7

pixel string[NUM_PIXELS];

void setup() {
    Serial.begin(9600);
    PIX.begin(string_port, string_pin); // set pin to output 
    clear_pixels(); 
}

void set_pixel( pixel *p, int r, int g, int b) { // this is intentional (grb)
    p->rgb.r = g;
    p->rgb.g = r;
    p->rgb.b = b;
}

void clear_pixels() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        set_pixel(&string[i], 0, 0, 0);
    }
    PIX.paint( string[0].bytes, NUM_PIXELS, string_port, string_pin );
}

void loop() {
    for (int i = 0; i < NUM_PIXELS; i++) { 
         set_pixel(&string[i], 0, 255, 0);
    }  
    PIX.paint( string[0].bytes, NUM_PIXELS, string_port, string_pin);
    delay(1000);
    double temperature = sensor.get_temp();
    Serial.print("Temperature: ");
    Serial.println(temperature);    
}
