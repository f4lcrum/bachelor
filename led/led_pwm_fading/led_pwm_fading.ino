#define METER_PIN PA4 // ANALOG PIN on PINOUT MAP ADCX
#define LED_PIN PA10 // PWM PIN on PINOUT MAP "~"
#define FADE_STEP 5

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(METER_PIN, OUTPUT);
}

void led_fade() {
    int i = 0;
    while ( i <= 255 ){
        analogWrite( LED_PIN, i );
        delay( 50 );
        i = i + FADE_STEP;
    }

    i = 255;
    while ( i >= 0 ){
        analogWrite( LED_PIN, i );
        delay( 50 );
        i = i - FADE_STEP;
    }
}

void loop() {
    int potentiometerValue = analogRead(METER_PIN);
    int brightness =  map(potentiometerValue, 0, 1023, 0, 255);
    analogWrite(LED_PIN, brightness);
}
