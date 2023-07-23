#define D4 PB11 
#define pinG PA1
#define pinC PA0
#define pinDP PC15 // decimal point
#define pinD PC14
#define pinE PC13

#define D1 PA9
#define pinA PB8
#define pinF PB7
#define D2 PA8
#define D3 PB15
#define pinB PB6
#define SEGMENTS 8

const int pins[SEGMENTS] = {pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP};
const int number_1[SEGMENTS] = {HIGH, LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH};
const int number_2[SEGMENTS] = {LOW, LOW, HIGH, LOW, LOW, HIGH, LOW, HIGH}; 
const int number_3[SEGMENTS] = {LOW, LOW, LOW, LOW, HIGH, HIGH, LOW, HIGH};
const int number_4[SEGMENTS] = {HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW, HIGH};
const int number_5[SEGMENTS] = {LOW, HIGH, LOW, LOW, HIGH, LOW, LOW, HIGH};
const int number_6[SEGMENTS] = {LOW, HIGH, LOW, LOW, LOW, LOW, LOW, HIGH};
const int number_7[SEGMENTS] = {LOW, LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH};
const int number_8[SEGMENTS] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, HIGH};
const int number_9[SEGMENTS] = {LOW, LOW, LOW, LOW, HIGH, LOW, LOW, HIGH};
const int number_0[SEGMENTS] = {LOW, LOW, LOW, LOW, LOW, LOW, HIGH, HIGH};


void setup() {
  // put your setup code here, to run once:
  pinMode(pinA, OUTPUT);     
  pinMode(pinB, OUTPUT);     
  pinMode(pinC, OUTPUT);     
  pinMode(pinD, OUTPUT);     
  pinMode(pinE, OUTPUT);     
  pinMode(pinF, OUTPUT);     
  pinMode(pinG, OUTPUT);   
  pinMode(pinDP, OUTPUT);  
  pinMode(D1, OUTPUT);  
  pinMode(D2, OUTPUT);  
  pinMode(D3, OUTPUT);  
  pinMode(D4, OUTPUT);  
}

void loop() {
  // put your main code here, to run repeatedly:
  write_D4(); //function call to turn on D1 to write characters for the first digit
  segment(number_0); // function call to display 3
  print_decimal(); // function call to display the decimal point
  delay(1000);
  segment(number_1);
  delay(1000);
  segment(number_2);
  delay(1000);
  segment(number_3);
  delay(1000);
  segment(number_4);
  delay(1000);
  segment(number_5);
  delay(1000);
  segment(number_6);
  delay(1000);
  segment(number_7);
  delay(1000);
  segment(number_8);
  delay(1000);
  segment(number_9);
  delay(1000);
  
}


void write_D1()
{
  digitalWrite(D1, HIGH);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW); 
}

void write_D2()
{
  digitalWrite(D1, LOW);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, LOW);
  digitalWrite(D4, LOW); 
}

void write_D3()
{
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, HIGH);
  digitalWrite(D4, LOW); 
}

void write_D4()
{
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  digitalWrite(D4, HIGH); 
}



void print_decimal() // writing the decimal point
{
  digitalWrite(pinDP, HIGH);
}

void segment(const int num[]){
  for (int i = 0; i < SEGMENTS; i++) {
    digitalWrite(pins[i], num[i]);
  } 
}

