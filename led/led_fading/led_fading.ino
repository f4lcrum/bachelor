#define METER_PIN PA4 // ANALOG PIN on PINOUT MAP ADCX
#define LED_PIN PA10 // PWM PIN on PINOUT MAP "~"
#define FADE_STEP 1

void setup() {
}

void loop() {
    int i = 1023;
    while ( i >= 0 ){
        analogWrite( LED_PIN, i );
        delay( 10 );
        i = i - FADE_STEP;
    }
    
    i = 0;
    while ( i <= 1023){
        analogWrite( LED_PIN, i );
        delay( 10 );
        i = i + FADE_STEP;
    }
}
