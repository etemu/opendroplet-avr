/*

 Open Droplet alpha r&d
 
 iilab ltd.
 iilab.org
 
 shure open technologies
 etemu.com
  
 CC0
 
 */
 
#include <Adafruit_NeoPixel.h>

#define LEDPIN 2
#define LEDPIXEL 1
#define MICPIN A0    // Analog input pin where the mic is attached to
#define OUTPIN 13    // Analog output pin, LED for testing purposes

// Parameters: number of LEDs attached, pin number, pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel neoled = Adafruit_NeoPixel(LEDPIXEL, LEDPIN, NEO_GRB + NEO_KHZ800);

int mic = 0;              // value read from the mic
int lastmic = 0;          // previous reading from the mic
int out = 0;              // value output to the PWM (analog out)
int showerState = 0;
void setup() {
  // initialize serial communications at 115200 bps:
  Serial.begin(115200);
  pinMode(LEDPIN,OUTPUT);
  pinMode(MICPIN,INPUT);
  pinMode(OUTPIN,OUTPUT);

  neoled.setPixelColor(0, 255,255,255);
  delay(250);
  neoled.setPixelColor(0,32,32,32);
  //ledoff();
}

void led(uint8_t _mode=0){
  uint16_t r,g,b = 0;
  for(uint16_t l = 0; l < LEDPIXEL; l++) {
      neoled.setPixelColor(l, r, g, b);
    }
}

void ledoff(){
    for(uint16_t l = 0; l < LEDPIXEL; l++) {
      neoled.setPixelColor(l, 0, 0, 0);
    }
}

void loop() {
  // read the mic volume:
  mic = analogRead(MICPIN);
  // map it to the range of the analog out:
  out = map(mic, 0, 1023, 255, 0);
  // change the analog out value:
  analogWrite(OUTPIN, out);
  if (lastmic+10 < mic) { showerState=1; }

  // print the results to the serial monitor:
  Serial.print("mic: " );
  Serial.print(mic);
  Serial.print("\t out: ");
  Serial.println(out);
  lastmic = mic;
  // wait before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(20);
}
