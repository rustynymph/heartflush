/* Flushes the toilet about 1 second after the Arduino is powered on
 * Author: Annie Kelly
 * Date: 01/28/2019
*/

#include <Servo.h>

/* Servo variables */
Servo myservo;           // create servo object to control a servo
const int SERVO_PIN = 10; // PWM digital pin 6

void setup() {   
  myservo.attach(SERVO_PIN); // attaches the servo on pin 6 to the servo object
  myservo.write(0);          // move servo to 0 degrees
  delay(500);                // allow 250 ms for the servo to get to starting postition
  myservo.detach();          // detaches the servo to prevent vibrating and twitching when idle
  
  Serial.begin(115200);
}

void loop() {
  Serial.println("Flushing");
  flush();
}

void flush() {
  myservo.attach(SERVO_PIN);
  myservo.write(180);
  delay(1000);
  myservo.detach();
  delay(500);
  myservo.attach(SERVO_PIN);
  myservo.write(0);
  delay(1000);
  myservo.detach();
  delay(5000); // delay also needs to account for water to refill tank, adjust later
}



  
