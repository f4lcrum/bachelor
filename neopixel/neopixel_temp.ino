#include <bluepill_ws2812.h> // https://github.com/FearlessNight/bluepill_ws2812
#include <temp_sensor_ds18b20.h>

bluepill_neopixel PIX;
#define NUM_PIXELS 8
#define string_port GPIOA
#define string_pin  7

pixel string[NUM_PIXELS];

typedef struct {
    double temp;
    int r;
    int g;
    int b;
} TemperatureRange;

TemperatureRange ranges[] = {
    {17.5,   0,   0, 255},
    {20,     0, 162, 255},
    {22.5,   0, 255, 154},
    {25,     0, 255,  51},
    {27.5, 255, 230,   0},
    {30,   255, 230,   0},
    {32.5, 255, 128,   0},
    {9999, 255,   0,   0} // Upper bound placeholder to cover anything above 32.5
};

Temp_sensor_ds18b20 sensor(PB3);

void setup() {
    // put your setup code here, to run once:
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

void set_color(double temp) {
    for (int i = 0; i < sizeof(ranges) / sizeof(ranges[0]); i++) {
        if (temp >= ranges[i].temp) {
            set_pixel(& string[i], ranges[i].r, ranges[i].g, ranges[i].b);
        }
        else {
            set_pixel(& string[i], 0, 0, 0);
        }
    }
    PIX.paint( string[0].bytes, NUM_PIXELS, string_port, string_pin );
}

void console_write_temp(double temp) {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" °C");
    Serial.println("");
}

void loop()
{
    double temp = sensor.get_temp();
    set_color(temp);
    console_write_temp(temp);
    delay(2000);
}