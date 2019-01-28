/*
  Include Servo.h BEFORE you include PusleSensorPlayground.h
*/
#include <Servo.h>
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library.   


//  Variables
const int AnalogPin0 = 0;      // ANALOG PIN 0
const int AnalogPin1 = 1;      // ANALOG PIN 1
const int AnalogPin2 = 2;      // ANALOG PIN 2
const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value. 
                               
PulseSensorPlayground pulseSensor0;  // Create an instance of the first PulseSensorPlayground object
PulseSensorPlayground pulseSensor1;  // Create an instance of the second PulseSensorPlayground object
PulseSensorPlayground pulseSensor2;  // Create an instance of the third PulseSensorPlayground object

int personBPM0, personBPM1, personBP2 = 0; // BPM of a person when we know someone is touching the pulse sensor

// Servo variables
/*
Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to store the servo position
*/

void setup() {   

  /*
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  myservo.write(0);
  delay(1000);        // allow 1 second for the servo to get back to starting postition
  */

  Serial.begin(115200);

  // Configure the first PulseSensor object
  pulseSensor0.analogInput(AnalogPin0);   
  pulseSensor0.setThreshold(Threshold);   
  // pulseSensor0.blinkOnPulse(LED13);       // blink Arduino's LED with heartbeat
  // Configure the second PulseSensor object
  pulseSensor1.analogInput(AnalogPin1);   
  pulseSensor1.setThreshold(Threshold);  
  // Configure the third PulseSensor object
  pulseSensor2.analogInput(AnalogPin2);   
  pulseSensor2.setThreshold(Threshold);  

  // Check the first "pulseSensor" object was created and "began" seeing a signal. 
   if (pulseSensor0.begin()) {
    Serial.println("We created pulseSensor 0!");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }

  // Check the second "pulseSensor" object was created and "began" seeing a signal. 
   if (pulseSensor1.begin()) {
    Serial.println("We created pulseSensor 1!");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }

  // Check the third "pulseSensor" object was created and "began" seeing a signal. 
   if (pulseSensor2.begin()) {
    Serial.println("We created pulseSensor 2!");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }
  
}



void loop() {

 int bpm0 = pulseSensor0.getBeatsPerMinute();  // stores bpm of first pulse sensor
 int bpm1 = pulseSensor1.getBeatsPerMinute();  // stores bpm of second pulse sensor
 int bpm2 = pulseSensor2.getBeatsPerMinute();  // stores bpm of third pulse sensor

 if (pulseSensor0.sawStartOfBeat()) {            // Test to see if "a beat happened". 
   Serial.println("♥  A HeartBeat Happened A0! ");
  }
  
  
 if (pulseSensor1.sawStartOfBeat()) {            // Test to see if "a beat happened". 
   Serial.println("♥  A HeartBeat Happened A1! ");
  }
  
  
 if (pulseSensor2.sawStartOfBeat()) {            // Test to see if "a beat happened". 
   Serial.println("♥  A HeartBeat Happened A2! ");
  }

  float averageBPM = (bpm0 + bpm1 + bpm2) / 3;
  Serial.print("bpm0: ");                         
  Serial.println(bpm0);
  Serial.print("bpm1: ");                         
  Serial.println(bpm1);
  Serial.print("bpm2: ");                         
  Serial.println(bpm2);    
  Serial.print("Average BPM: ");                         
  Serial.println(averageBPM);

  if (averageBPM >= 70){
    Serial.println("Flush");
    /*
    myservo.write(180);
    delay(2000); 
    myservo.write(0);
    delay(2000); 
    */
  }
  
  delay(20);                    // considered best practice in a simple sketch.

}

  
