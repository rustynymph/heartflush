#include <Servo.h>                   // Include Servo.h BEFORE you include PusleSensorPlayground.h
#define USE_ARDUINO_INTERRUPTS true  // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>   // Includes the PulseSensorPlayground Library.   

/* Pulse sensor variables */
const int PULSE_SENSOR_COUNT = 3;
const int PULSE_INPUT0 = 0;      // ANALOG PIN 0
const int PULSE_INPUT1 = A1;      // ANALOG PIN 1
const int PULSE_INPUT2 = A2;      // ANALOG PIN 2
const int THRESHOLD = 590;           // Determine which Signal to "count as a beat" and which to ignore.                          
int bpm0, bpm1, bpm2 = -1;
float bpm0Avg, bpm1Avg, bpm2Avg = -1.0;
int bpm0Vals[3] = {-1.0, -1.0, -1.0};
int bpm1Vals[3] = {-1.0, -1.0, -1.0};
int bpm2Vals[3] = {-1.0, -1.0, -1.0};
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
  
  Serial.begin(250000);

  pulseSensor.setSerial(Serial);
  //pulseSensor.setOutputType(OUTPUT_TYPE);
  //lseSensor.setThreshold(THRESHOLD);

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

  delay(20);

  // write the latest sample to Serial.
  //pulseSensor.outputSample();

  /*
  for (int i = 0; i < PULSE_SENSOR_COUNT; ++i) {
    if (pulseSensor.sawStartOfBeat(i)) {
      pulseSensor.outputBeat(i);
    }
  }
  */


 /* Update BPM values when a heartbeat is detected */
 if (pulseSensor.sawStartOfBeat(0)) {
  int currentBPM = pulseSensor.getBeatsPerMinute(0);
  if (currentBPM <= 150) {
    bpm0 = currentBPM;
    timeLastDetected0 = millis();
    /* update the array containing the 3 most recent bpm0 readings */
    bpm0Vals[2] = bpm0Vals[1];
    bpm0Vals[1] = bpm0Vals[0];    
    bpm0Vals[0] = bpm0;
  }
 }
 if (pulseSensor.sawStartOfBeat(1)) {
  int currentBPM = pulseSensor.getBeatsPerMinute(1);
  if (currentBPM <= 150) {
    bpm1 = currentBPM;
    timeLastDetected1 = millis();
    /* update the array containing the 3 most recent bpm1 readings */
    bpm1Vals[2] = bpm1Vals[1];
    bpm1Vals[1] = bpm1Vals[0];    
    bpm1Vals[0] = bpm1;
  }
 }
 if (pulseSensor.sawStartOfBeat(2)) {
  int currentBPM = pulseSensor.getBeatsPerMinute(2);  
  if (currentBPM <= 150) {
    bpm2 = currentBPM;
    timeLastDetected2 = millis();
    /* update the array containing the 3 most recent bpm2 readings */
    bpm2Vals[2] = bpm2Vals[1];
    bpm2Vals[1] = bpm2Vals[0];    
    bpm2Vals[0] = bpm2;
  }
 }

  // calculate average of the last 3 bpm readings of each sensor
  bpm0Avg = (bpm0Vals[0] + bpm0Vals[1] + bpm0Vals[2]) / 3;
  bpm1Avg = (bpm1Vals[0] + bpm1Vals[1] + bpm1Vals[2]) / 3;
  bpm2Avg = (bpm2Vals[0] + bpm2Vals[1] + bpm2Vals[2]) / 3;

  /* Reset BPM to -1 if enough time has passed since reading */
  if (abs(timeLastDetected0 - millis()) >= 3000) {
    bpm0 = -1;
    bpm0Vals[0] = bpm0Vals[1] = bpm0Vals[2] = -1;
  }
  if (abs(timeLastDetected1 - millis()) >= 3000) {
    bpm1 = -1;
    bpm1Vals[0] = bpm1Vals[1] = bpm1Vals[2] = -1;   
  }  
  if (abs(timeLastDetected2 - millis()) >= 3000) {
    bpm2 = -1;
    bpm2Vals[0] = bpm2Vals[1] = bpm2Vals[2] = -1;   
  }  


  totalAverageBPM = (bpm0Avg + bpm1Avg + bpm2Avg) / 3;
  // now takes the average of any of the active pulse sensors, not just all 3
  /*totalAverageBPM = 0;
  int avgCount = 0;
  if (bpm0 != -1){
    totalAverageBPM += bpm0Avg;
    avgCount++;
  }
  if (bpm1 != -1){
    totalAverageBPM += bpm1Avg;
    avgCount++;
  }
  if (bpm2 != -1){
    totalAverageBPM += bpm2Avg;
    avgCount++;
  }    
  if (avgCount != 0) {
    totalAverageBPM = totalAverageBPM / avgCount;  
  }*/

  printBPMData(); // prints BPM data to serial monitor

  if (totalAverageBPM >= 75) { // average BPM that needs to be met to trigger flush
    if (!flushing) { // only flush if it's not already flushing
      flushing = true;
      flush();
      flushing = false; // finished flushing
    }
  }
 
  
}

void flush() {
  Serial.println("Flushing");
  printBPMData();
  
  myservo.attach(SERVO_PIN);
  myservo.write(180);
  delay(2500);
  myservo.detach();
  delay(500);
  myservo.attach(SERVO_PIN);
  myservo.write(0);
  delay(500);
  myservo.detach();
  delay(30000); // delay also needs to account for water to refill tank, adjust later
}

void printBPMData() {
  Serial.print("bpm0: ");                         
  Serial.print(bpm0Avg);
  Serial.print(", bpm1: ");                         
  Serial.print(bpm1Avg);
  Serial.print(", bpm2: ");                         
  Serial.println(bpm2Avg);
  Serial.print("Average BPM: ");                       
  Serial.println(totalAverageBPM);
}


  
