/*

 Open Droplet alpha r&d
 
 iilab ltd.
 iilab.org
 
 shure open technologies
 etemu.com
  
 CC0
 
 */

const unsigned int micPin = A0;    // Analog input pin where the mic is attached to
const unsigned int outPin = 13;    // Analog output pin, LED for testing purposes

int mic = 0;              // value read from the mic
int lastmic = 0;          // previous reading from the mic
int out = 0;              // value output to the PWM (analog out)

void setup() {
  // initialize serial communications at 115200 bps:
  Serial.begin(115200);
}

void loop() {
  // read the mic volume:
  mic = analogRead(micPin);
  // map it to the range of the analog out:
  out = map(mic, 0, 1023, 255, 0);
  // change the analog out value:
  analogWrite(outPin, out);
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
  delay(4);
}
