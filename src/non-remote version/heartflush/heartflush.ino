#include <Servo.h>
                // Include Servo.h BEFORE you include PusleSensorPlayground.h
#define USE_ARDUINO_INTERRUPTS true  // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>   // Includes the PulseSensorPlayground Library.   

/* Pulse sensor variables */
const int PULSE_SENSOR_COUNT = 3;
const int PULSE_INPUT0 = A0;      // ANALOG PIN 0
const int PULSE_INPUT1 = A1;      // ANALOG PIN 1
const int PULSE_INPUT2 = A2;      // ANALOG PIN 2
const int THRESHOLD = 550;           // Determine which Signal to "count as a beat" and which to ignore.                          
int bpm0, bpm1, bpm2 = -1;
float totalAverageBPM = -1.0;
float timeLastDetected0, timeLastDetected1, timeLastDetected2;
PulseSensorPlayground pulseSensor(PULSE_SENSOR_COUNT);

/* Servo variables */
Servo myservo;           // create servo object to control a servo
const int SERVO_PIN = 10; // PWM digital pin 6

bool flushing = false;   // keeps track of whether or not the toilet is currently flushing
                         // and/or in the middle of refilling

void setup() {   
  myservo.attach(SERVO_PIN); // attaches the servo on pin 6 to the servo object
  myservo.write(0);          // move servo to 0 degrees
  delay(250);                // allow 250 ms for the servo to get to starting postition
  myservo.detach();          // detaches the servo to prevent vibrating and twitching when idle
  
  Serial.begin(115200);

  pulseSensor.analogInput(PULSE_INPUT0, 0);
  pulseSensor.analogInput(PULSE_INPUT1, 1);
  pulseSensor.analogInput(PULSE_INPUT2, 2);
  pulseSensor.setThreshold(THRESHOLD); 

  // Check the "pulseSensor" object was created and "began" seeing a signal. 
   if (pulseSensor.begin()) {
    Serial.println("We created the pulseSensor!");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  } 
}

void loop() {
 /* Update BPM values when a heartbeat is detected */
 if (pulseSensor.sawStartOfBeat(0)) {
  bpm0 = pulseSensor.getBeatsPerMinute(0);
  timeLastDetected0 = millis();
 }
 if (pulseSensor.sawStartOfBeat(1)) {
  bpm1 = pulseSensor.getBeatsPerMinute(1);
  timeLastDetected1 = millis(); 
 }
 if (pulseSensor.sawStartOfBeat(2)) {
  bpm2 = pulseSensor.getBeatsPerMinute(2);
  timeLastDetected2 = millis(); 
 }

  /* Reset BPM to -1 if enough time has passed since reading */
  if (abs(timeLastDetected0 - millis()) >= 3000) {
    bpm0 = -1;
  }
  if (abs(timeLastDetected1 - millis()) >= 3000) {
    bpm1 = -1;  
  }  
  if (abs(timeLastDetected2 - millis()) >= 3000) {
    bpm2 = -1;  
  }  

  totalAverageBPM = (bpm0 + bpm1 + bpm2) / 3;
  
  printBPMData(); // prints BPM data to serial monitor

  if (totalAverageBPM >= 800) { // average BPM that needs to be met to trigger flush
    if (!flushing) { // only flush if it's not already flushing
      Serial.println("Flushing");
      flushing = true;
      flush();
      flushing = false; // finished flushing
    }
  }
  
  delay(20);
}

void flush() {
  myservo.attach(SERVO_PIN);
  myservo.write(180);
  delay(1000);
  myservo.detach();
  delay(500);
  myservo.attach(SERVO_PIN);
  myservo.write(0);
  delay(500);
  myservo.detach();
  delay(5000); // delay also needs to account for water to refill tank, adjust later
}

void printBPMData() {
  Serial.print("bpm0: ");                         
  Serial.print(bpm0);
  Serial.print(", bpm1: ");                         
  Serial.print(bpm1);
  Serial.print(", bpm2: ");                         
  Serial.println(bpm2);
  Serial.print("Average BPM: ");                         
  Serial.println(totalAverageBPM);
}


  
