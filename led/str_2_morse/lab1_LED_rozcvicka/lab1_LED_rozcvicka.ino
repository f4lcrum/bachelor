#define LED_PIN PB5 
#define TERMINATOR '\n'

void setup() {
    // put your setup code here, to run once:
	Serial.begin(9600);
	pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // put your main code here, to run repeatedly:
    if (Serial.available() > 0) { 
		delay(10);
		String input = Serial.readStringUntil(TERMINATOR);
		Serial.print("My input:");
		Serial.println(input);
    }
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
    Serial.println("Every 2 seconds a new line in Serial monitor will arrive!");
}
