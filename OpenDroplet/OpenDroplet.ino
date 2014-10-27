/*

 Open Droplet alpha r&d
 
 shure open technologies
 etemu.com
 iilab.org  
 CC0
 
 */

#include <Adafruit_NeoPixel.h>	// NeoPixel lib for visual feedback with RGB LEDs when debugging
 
#define DEBUG 1 				// debug mode with verbose output over serial at 115200 bps
#define USE_EEPROM 				// read nodeID and network settings from EEPROM at bootup
#define KEEPALIVE_RATE 32768L 	// request rate of sendKeepalive in ms
#define LEDPIN 2     			// PortPin where the LED pixels are attached/daisy chained
#define LEDPIXEL 1   			// Number of LED pixels attached
#define MICPIN A0    			// Analog input pin where the mic is attached to
#define OUTPIN 13    			// Analog output pin, LED for testing purposes

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDPIXEL, LEDPIN, NEO_GRB + NEO_KHZ800);

int mic = 0;              		// value read from the mic
int lastmic = 0;          		// previous reading from the mic
int out = 0;              		// value output to the PWM (analog out)
int showerState = 0;
void setup() {
  Serial.begin(115200);			// initialize serial communications at 115200 bps:
  pinMode(LEDPIN,OUTPUT);
  pinMode(MICPIN,INPUT);
  pinMode(OUTPIN,OUTPUT);

  strip.setPixelColor(0, 255,255,255);
  delay(250);
  strip.setPixelColor(0,32,32,32);
  //ledoff();
}

void led(uint8_t _mode=0){
  uint16_t r,g,b = 0;
  for(uint16_t _l = 0; _l < LEDPIXEL; _l++) {
      strip.setPixelColor(_l, r, g, b);
    }
}

void ledoff(){
    for(uint16_t _l = 0; _l < LEDPIXEL; _l++) {
      strip.setPixelColor(_l, 0, 0, 0);
    }
}

void loop() {
  
  mic = analogRead(MICPIN); 			// read the mic volume
  out = map(mic, 0, 1023, 255, 0); 		// map it to the range of the analog out
  analogWrite(OUTPIN, out);				// change the analog out value
  if (lastmic+10 < mic) { showerState=1; }
  Serial.print("mic: " );				// print the results to the serial monitor:
  Serial.print(mic);
  Serial.print("\t out: ");
  Serial.println(out);
  lastmic = mic;
  // wait before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(20);
}
