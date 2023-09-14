#define METER_PIN PA4 // ANALOG PIN on PINOUT MAP ADCX
#define LED_PIN PA10 // PWM PIN on PINOUT MAP "~"

void setup() {
}

void loop() {
    int potentiometerValue = analogRead(METER_PIN);
    int brightness =  map(potentiometerValue, 0, 1023, 0, 255);
    analogWrite(LED_PIN, brightness);
}
