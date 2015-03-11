/*
 Open Droplet OEM
 iilab.org, Alex Shure
 based on Open Energy Monitor
 builds upon emontx, JeeLabs RF12 library and Arduino
 
 Dependencies:
	- Arduino
	- JeeLib		https://github.com/jcw/jeelib
	- EmonLib		https://github.com/openenergymonitor/EmonLib.git
*/

/*Recommended node ID allocation
------------------------------------------------------------------------------------------------------------
-ID-	-Node Type- 
0		- Special allocation in JeeLib RFM12 driver - reserved for OOK use
1-4     - Control nodes 
5-10	- Monitoring nodes
11-14	--Un-assigned --
15-16	- Base Station & logging nodes
17-30	- Environmental sensing nodes (temperature humidity etc.)
31		- Special allocation in JeeLib RFM12 driver - Node31 can communicate with nodes on any network group
-------------------------------------------------------------------------------------------------------------
*/

#define FILTERSETTLETIME 1023                                           //  Time (ms) to allow the filters to settle before sending data

#define DEBUG 1 				// debug mode with verbose output over serial at 115200 bps
#define USE_EEPROM 				// read nodeID and network settings from EEPROM at bootup
#define KEEPALIVE_RATE 32768L 	// request rate of keepalive heartbeat packet in ms
#define LEDPIN 2     			// PortPin where the WS28xx LED pixels are attached/daisy chained
#define LEDPIXEL 1   			// Number of LED pixels attached
#define MICPIN A0    			// Analog input pin where the mic is attached to
#define OUTPIN 9    			// Analog output pin, LED for testing purposes

unsigned int mic = 0;              		// value read from the mic
unsigned int lastmic = 0;          		// previous reading from the mic
unsigned int out = 0;              		// value output to the PWM (analog out)
unsigned int showerState = 0;
byte mode = 0;							// power mode
unsigned long lastActive = 0;
unsigned long uid = 42;

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

#define RF_freq RF12_433MHZ                                                // Frequency of RF12B module can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.
const int nodeID = 8;                                                  // emonTx RFM12B node ID
const int networkGroup = 210;                                           // emonTx RFM12B wireless network group - needs to be same as emonBase and emonGLCD                                                 

#define RF69_COMPAT 0 // set to 1 to use RFM69CW 
#include <JeeLib.h>   // make sure V12 (latest) is used if using RFM69CW
#include "EmonLib.h"

typedef struct { unsigned int uid, spl, vbat;} PayloadTX;      			// create structure - a neat way of packaging data for RF comms
PayloadTX emontx;                                                       

const int LEDpin = 9;                                                   // LED pin for status output, just a small diode, not a WS28xx RGB LED

boolean settled = false;

void setup() 
{
  Serial.begin(115200);
   //while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  
  Serial.println("Open Droplet example"); 
  Serial.println("iilab.org");
  Serial.print(F("wireless ID: ")); 
  Serial.print(nodeID); 
  Serial.print(F(" ,")); 
  if (RF_freq == RF12_433MHZ) Serial.print("433Mhz");
  if (RF_freq == RF12_868MHZ) Serial.print("868Mhz");
  if (RF_freq == RF12_915MHZ) Serial.print("915Mhz"); 
 Serial.print(" network group: "); 
  Serial.println(networkGroup);
  // }
   
  rf12_initialize(nodeID, RF_freq, networkGroup);               // initialize RFM12B
  rf12_sleep(RF12_SLEEP);										// set the radio to sleep mode for now to save energy

  pinMode(LEDpin, OUTPUT);                                      // Setup indicator LED
  digitalWrite(LEDpin, HIGH);
                                  
  //pinMode(3, OUTPUT);                                           // Give power to microphone
  //digitalWrite(3, HIGH);
                                                                                       
}

void loop() 
{ 
  Serial.begin(115200);
  unsigned long startMillis= millis(); // Start of sample window
  unsigned int peakToPeak = 0; // peak-to-peak level
   
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
   
  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {   
    sample = analogRead(MICPIN);
    if (sample < 1024) // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample; // save just the max levels
      }
    else if (sample < signalMin)
      {
        signalMin = sample; // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude
   
  Serial.println(peakToPeak);
  
  // Set LED to previous Window's sound level.
  analogWrite(LEDpin, peakToPeak/4);
  
  emontx.spl = peakToPeak;
  emontx.uid = uid; 
  
  Serial.print(" "); Serial.print(emontx.uid);
  Serial.print(" "); Serial.print(emontx.spl);
  //Serial.print(" "); Serial.print(emontx.vbat);
  Serial.println(); delay(100);

  // because millis() returns to zero after 50 days ! 
  if (!settled && millis() > FILTERSETTLETIME) settled = true;

  if (settled)                                                            // send data only after filters have settled
  { 
    send_rf_data();                                                       // *SEND RF DATA* - see emontx_lib
//    analogWrite(LEDpin, (peakToPeak/4)+20); delay(20); digitalWrite(LEDpin,  (peakToPeak/4)-20);      // flash LED
    delay(200);                                                          // delay between readings in ms
  }
}

void send_rf_data()
{
  rf12_sleep(RF12_WAKEUP);
  // if ready to send + exit loop if it gets stuck as it seems too
  int i = 0;
  while (!rf12_canSend() && i<10) {
	rf12_recvDone();
	i++;
	}
  rf12_sendStart(0, &emontx, sizeof emontx);
  // set the sync mode to 2 if the fuses are still the Arduino default
  // mode 3 (full powerdown) can only be used with 258 CK startup fuses
  rf12_sendWait(2);
  rf12_sleep(RF12_SLEEP);
}

