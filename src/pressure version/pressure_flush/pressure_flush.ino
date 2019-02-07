#include <Servo.h>

/* Forse Sensitive Resistor (FSR) variables */
const int FSR_PIN0    = A0;     // Pin connected to FSR/resistor divider
const int FSR_PIN1    = A1;     // Pin connected to FSR/resistor divider
const int FSR_PIN2    = A2;     // Pin connected to FSR/resistor divider
const float THRESHOLD = 350.00; // ADC value all 3 FSR sensors need to surpass to flush the toilet

/* Servo variables */
Servo myservo;            // create servo object to control a servo
const int SERVO_PIN = 10; // PWM digital pin 6

void setup() {
  Serial.begin(9600);
  
  pinMode(FSR_PIN0, INPUT);
  pinMode(FSR_PIN1, INPUT);
  pinMode(FSR_PIN2, INPUT);    

  myservo.attach(SERVO_PIN); // attaches the servo on pin 10 to the myservo object
  myservo.write(0);          // move servo to starting position of 0 degrees
  delay(500);                // allow 500 ms for the servo to get to starting postition
  myservo.detach();          // detaches the servo to prevent vibrating and twitching when idle
}

void loop() {
  int fsrADC0 = analogRead(FSR_PIN0); // read value of FSR on pin A0
  int fsrADC1 = analogRead(FSR_PIN1); // read value of FSR on pin A1
  int fsrADC2 = analogRead(FSR_PIN2); // read value of FSR on pin A2   

  Serial.print("A0: ");
  Serial.print(analogRead(fsrADC0));
  Serial.print(" A1: ");  
  Serial.print(analogRead(fsrADC1));
  Serial.print(" A2: ");   
  Serial.println(analogRead(fsrADC2));

  // flushes the toilet if all 3 FSR sensors are over the threshold value
  if (fsrADC0 >= THRESHOLD && fsrADC1 >= THRESHOLD && fsrADC2 >= THRESHOLD) {
    Serial.println("Flushing!");
    flush();
  }
}

void flush() {
  myservo.attach(SERVO_PIN);
  myservo.write(180);
  delay(3250);
  myservo.detach();
  delay(500);
  myservo.attach(SERVO_PIN);
  myservo.write(0);
  delay(1000);
  myservo.detach();
  //delay(25000); // delay that can account for time for water to refill tank
}
