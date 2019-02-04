#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include "arduino_secrets.h" 

#define USE_ARDUINO_INTERRUPTS true  // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>   // Includes the PulseSensorPlayground Library.   

/* Wifi variables */
// please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP)
int status  = WL_IDLE_STATUS;     // the Wifi radio's status
unsigned int localPort = 9000;    // local port to listen on
String remoteHostName = ""; // needs to be the IP of the Arduino
int remotePortNum = 9000; // needs to be the IP of the Arduino
WiFiUDP Udp;


/* Pulse sensor variables */
const int PULSE_SENSOR_COUNT = 3;
const int PULSE_INPUT0 = A0;      // ANALOG PIN 0
const int PULSE_INPUT1 = A1;      // ANALOG PIN 1
const int PULSE_INPUT2 = A2;      // ANALOG PIN 2
const int THRESHOLD = 550;           // Determine which Signal to "count as a beat" and which to ignore.                          
int bpm0, bpm1, bpm2 = -1;
float bpm0Avg, bpm1Avg, bpm2Avg = -1.0;
int bpm0Vals[3] = {-1.0, -1.0, -1.0};
int bpm1Vals[3] = {-1.0, -1.0, -1.0};
int bpm2Vals[3] = {-1.0, -1.0, -1.0};
float totalAverageBPM = -1.0;
float timeLastDetected0, timeLastDetected1, timeLastDetected2;
PulseSensorPlayground pulseSensor(PULSE_SENSOR_COUNT);

void setup() {
    wifiSetup();

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
  /* update the array containing the 3 most recent bpm0 readings */
  bpm0Vals[2] = bpm0Vals[1];
  bpm0Vals[1] = bpm0Vals[0];    
  bpm0Vals[0] = bpm0;
 }
 if (pulseSensor.sawStartOfBeat(1)) {
  bpm1 = pulseSensor.getBeatsPerMinute(1);
  timeLastDetected1 = millis();
  /* update the array containing the 3 most recent bpm1 readings */
  bpm1Vals[2] = bpm1Vals[1];
  bpm1Vals[1] = bpm1Vals[0];    
  bpm1Vals[0] = bpm1;
 }
 if (pulseSensor.sawStartOfBeat(2)) {
  bpm2 = pulseSensor.getBeatsPerMinute(2);
  timeLastDetected2 = millis();
  /* update the array containing the 3 most recent bpm2 readings */
  bpm2Vals[2] = bpm2Vals[1];
  bpm2Vals[1] = bpm2Vals[0];    
  bpm2Vals[0] = bpm2;
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

  //totalAverageBPM = (bpm0 + bpm1 + bpm2) / 3;
  totalAverageBPM = (bpm0Avg + bpm1Avg + bpm2Avg) / 3;
  
  printBPMData(); // prints BPM data to serial monitor

  if (totalAverageBPM >= 90) { // average BPM that needs to be met to trigger flush
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.beginPacket(remoteHostName, remotePortNum);
    Udp.write("1");
    Udp.endPacket();
  }

}

void wifiSetup() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

    Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);

}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
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
