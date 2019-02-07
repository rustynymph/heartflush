/******************************************************************************
Create a voltage divider circuit combining an FSR (force sensitive resistor) with a 3.3k resistor.
- The resistor should connect from A0 to GND.
- The FSR should connect from A0 to 3.3V
As the resistance of the FSR decreases (meaning an increase in pressure), the
voltage at A0 should increase.
******************************************************************************/
#include <Servo.h>

/* Pressure sensor variables */
const int FSR_PIN0 = A0; // Pin connected to FSR/resistor divider
const int FSR_PIN1 = A1; // Pin connected to FSR/resistor divider
const int FSR_PIN2 = A2; // Pin connected to FSR/resistor divider
float fsrV0, fsrV1, fsrV2 = 0.0;
float fsrR0, fsrR1, fsrR2 = 0.0;
float fsrG0, fsrG1, fsrG2 = 0.0;
// Measure the voltage at 5V and resistance of your 3.3k resistor, and enter
// their value's below:
const float VCC   = 4.98;   // Measured voltage of Ardunio 5V line
const float R_DIV = 3230.0; // Measured resistance of 3.3k resistor
const float THRESHOLD = 350.00;

/* Servo variables */
Servo myservo;            // create servo object to control a servo
const int SERVO_PIN = 10; // PWM digital pin 6

void setup() {
  Serial.begin(9600);
  
  pinMode(FSR_PIN0, INPUT);
  pinMode(FSR_PIN1, INPUT);
  pinMode(FSR_PIN2, INPUT);    

  myservo.attach(SERVO_PIN); // attaches the servo on pin 6 to the servo object
  myservo.write(0);          // move servo to 0 degrees
  delay(500);                // allow 250 ms for the servo to get to starting postition
  myservo.detach();          // detaches the servo to prevent vibrating and twitching when idle
}

void loop() {
  int fsrADC0 = analogRead(FSR_PIN0);
  int fsrADC1 = analogRead(FSR_PIN1);
  int fsrADC2 = analogRead(FSR_PIN2);    

  Serial.print("A0: ");
  Serial.print(analogRead(fsrADC0));
  Serial.print(" A1: ");  
  Serial.print(analogRead(fsrADC1));
  Serial.print(" A2: ");   
  Serial.println(analogRead(fsrADC2));
  
  // If the FSR has no pressure, the resistance will be
  // near infinite. So the voltage should be near 0.
  if (fsrADC0 != 0) {
    fsrV0 = calculateVoltage(fsrADC0);
    fsrR0 = calculateResistance(fsrV0);
    Serial.println("Voltage FSR 0: " + String(fsrV0) + " V");
    Serial.println("Resistance FSR 0: " + String(fsrR0) + " ohms");
  }
  if (fsrADC1 != 0) {
    fsrV1 = calculateVoltage(fsrADC1);
    fsrR1 = calculateResistance(fsrV1);
    Serial.println("Voltage FSR 1: " + String(fsrV1) + " V");
    Serial.println("Resistance FSR 1: " + String(fsrR1) + " ohms");
  }
  if (fsrADC2 != 0) {
    fsrV2 = calculateVoltage(fsrADC2);
    fsrR2 = calculateResistance(fsrV2);
    Serial.println("Voltage FSR 2: " + String(fsrV2) + " V");
    Serial.println("Resistance FSR 2: " + String(fsrR2) + " ohms");
  }
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
  //delay(25000); // delay also needs to account for water to refill tank, adjust later
}

float calculateVoltage(float fsrADC) {
  // Use ADC reading to calculate voltage
  return fsrADC * VCC / 1023.0;
}

float calculateResistance(float fsrV) {
    // Use voltage and static resistor value to 
    // calculate FSR resistance
    return R_DIV * (VCC / fsrV - 1.0);  
}

float calculateForce(float fsrR) {
    // Guesstimate force based on slopes in figure 3 of
    // FSR datasheet  
    float fsrG = 1.0 / fsrR; // Calculate conductance
    // Break parabolic curve down into two linear slopes:
    if (fsrR <= 600) 
      return (fsrG - 0.00075) / 0.00000032639;
    else
      return fsrG / 0.000000642857; 
}
