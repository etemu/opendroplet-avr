/*

 Open Droplet alpha r&d
 
 shure open technologies
 etemu.com
 iilab.org  
 CC0
 
 */


#include "Arduino.h"
 
#include <Adafruit_NeoPixel.h>	// NeoPixel lib for visual feedback with RGB LEDs when debugging
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/power.h>

#define DEBUG 1 				// debug mode with verbose output over serial at 115200 bps
#define USE_EEPROM 				// read nodeID and network settings from EEPROM at bootup
#define KEEPALIVE_RATE 32768L 	// request rate of keepalive heartbeat packet in ms
#define LEDPIN 2     			// PortPin where the WS28xx LED pixels are attached/daisy chained
#define LEDPIXEL 1   			// Number of LED pixels attached
#define MICPIN A0    			// Analog input pin where the mic is attached to
#define OUTPIN 13    			// Analog output pin, LED for testing purposes

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDPIXEL, LEDPIN, NEO_GRB + NEO_KHZ800);

int mic = 0;              		// value read from the mic
int lastmic = 0;          		// previous reading from the mic
int out = 0;              		// value output to the PWM (analog out)
int showerState = 0;
byte mode = 0;					// power mode
unsigned long lastActive = 0;

// Sleep declarations
typedef enum { wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s } wdt_prescalar_e;
void setup_watchdog(uint8_t prescalar);
void do_sleep(void);
const short sleep_cycles_per_transmission = 10;
volatile short sleep_cycles_remaining = sleep_cycles_per_transmission;
void powerMode(byte _mode);


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

void active() {
  lastActive = millis();
}

// 0=16ms, 1=32ms,2=64ms,3=125ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(uint8_t prescalar)
{
  prescalar = min(9, prescalar);
  uint8_t wdtcsr = prescalar & 7;
  if ( prescalar & 8 )
    wdtcsr |= _BV(WDP3);
  MCUSR &= ~_BV(WDRF);
  WDTCSR = _BV(WDCE) | _BV(WDE);
  WDTCSR = _BV(WDCE) | wdtcsr | _BV(WDIE);
}

ISR(WDT_vect)
{
  --sleep_cycles_remaining;
}

void do_sleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  sleep_mode();                        // System sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out
}

void powerMode(byte _mode = mode) {
  switch (_mode) // check which power mode should be set active
  {
    case 0:
      break; // no change in power mode
    case 1:
      Serial.println(F("powerMode(1), MEMS, mic up"));
      mode = 0;
      break;
    case 2:
      //radio.powerUp(); // activate the radio
      Serial.println(F("powerMode(2), radio up"));
      mode = 0;
      break;
    case 5:
      Serial.println(F("powerMode(5), hibernate"));
      //radio.powerDown();
      setup_watchdog(wdt_1s); // set touch check interval
      while ( -20000 < (-10000) ) { // FIXME
        do_sleep(); // keep on sleeping
        powerMode(1); // power up MEMS, wake up every watchdog timer interval
      }
      
      powerMode(1); // power up MEMS, mic...
      powerMode(2); // power up radio
      active();
      break;
    case 6: // shutdown until next hard reset
      Serial.println(F("powerMode(6), shutdown"));
      //radio.powerDown();
      setup_watchdog(wdt_8s); // longest watchdog interval
      while(1) do_sleep(); // sleep forever
      break;      
    default: // no change in power mode
      return;
  };
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
