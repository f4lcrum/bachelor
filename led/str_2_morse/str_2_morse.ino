// Inspired by: https://www.arduino.cc/education/morse-code-project/
// Author: Kristián Oravec

#define LED_PIN PB7  // Replace with your desired LED pin

// Delays in ms
#define DELAY_UNIT 1000
#define DELAY_DOT DELAY_UNIT
#define DELAY_DASH 3*DELAY_UNIT
#define DELAY_SPACESAMELETTER DELAY_UNIT
#define DELAY_SPACELETTER 3 * DELAY_UNIT
#define DELAY_SPACEWORD 7 * DELAY_UNIT
#define TERMINATOR '\n'

#define DOT '.'
#define DASH '-'


char* letters[] = {
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", // A-I
"*---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." // S-Z
};

char* numbers[] = {
  "-----", ".----", "..---", "...--", "....-", ".....",
"-....", "--...", "---..", "----."
};


void blink_led(int delay_ms){
    digitalWrite(LED_PIN, HIGH);
    delay(delay_ms);
    digitalWrite(LED_PIN, LOW);
    delay(DELAY_SPACESAMELETTER);
}

void parse_morse(char* morse_code)
{
    for (int i = 0; i < strlen(morse_code); i++){
        if (morse_code[i] == DOT) {
            blink_led(DELAY_DOT);
        }
        else {
            blink_led(DELAY_DASH);
        }
    }
}

void setup() {
    // Starts serial communication
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
}

void parse_string(String input){
    char c;
    for (int i = 0; i < input.length(); i++){
      c = input[i];
      if (c == ' ') {
          delay(DELAY_SPACEWORD);
      }
      if (isDigit(c)){
          parse_morse(numbers[c - '0']);
      }
      else if ( isupper(c)) {
          parse_morse(letters[c - 'A']);
      }
      else if (islower(c)) {
          parse_morse(letters[c - 'a']);
      }
      else {
          Serial.println("Other characters than alphanumeric are ignored");
      }
      delay(DELAY_SPACELETTER);
    }
}

void loop() {
    String input; // "Serial.readStringUntil" returns "String" and not "char*"
    if (Serial.available() > 0) {
        delay(10); // Give some time to byte to arrive in input buffer
        input = Serial.readStringUntil(TERMINATOR);
        Serial.println(input);
        parse_string(input);
    }
}
