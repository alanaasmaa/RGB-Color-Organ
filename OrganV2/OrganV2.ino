#define SOFTPWM_OUTPUT_DELAY
#include <SoftPWM.h>                                  // Include Palatis SoftPWM libary github.com/Palatis/arduino-softpwm
SOFTPWM_DEFINE_CHANNEL(3, DDRD, PORTD, PORTD3);       // Arduino pin 3
SOFTPWM_DEFINE_CHANNEL(5, DDRD, PORTD, PORTD5);       // Arduino pin 5
SOFTPWM_DEFINE_CHANNEL(6, DDRD, PORTD, PORTD6);       // Arduino pin 6
SOFTPWM_DEFINE_CHANNEL(7, DDRD, PORTD, PORTD7);       // Arduino pin 7
SOFTPWM_DEFINE_CHANNEL(8, DDRB, PORTB, PORTB0);       // Arduino pin 8
SOFTPWM_DEFINE_CHANNEL(9, DDRB, PORTB, PORTB1);       // Arduino pin 9
SOFTPWM_DEFINE_CHANNEL(10, DDRB, PORTB, PORTB2);      // Arduino pin 10
SOFTPWM_DEFINE_CHANNEL(11, DDRB, PORTB, PORTB3);      // Arduino pin 11
SOFTPWM_DEFINE_CHANNEL(12, DDRB, PORTB, PORTB4);      // Arduino pin 12
SOFTPWM_DEFINE_CHANNEL(13, DDRB, PORTB, PORTB5);      // Arduino pin 13
SOFTPWM_DEFINE_CHANNEL(14, DDRC, PORTC, PORTC0);      // Arduino pin A0
SOFTPWM_DEFINE_CHANNEL(15, DDRC, PORTC, PORTC1);      // Arduino pin A1
SOFTPWM_DEFINE_CHANNEL(16, DDRC, PORTC, PORTC2);      // Arduino pin A2
SOFTPWM_DEFINE_CHANNEL(17, DDRC, PORTC, PORTC3);      // Arduino pin A3
SOFTPWM_DEFINE_OBJECT(14);       // Define how many pwm channels project uses and how many steps it has
SOFTPWM_DEFINE_EXTERN_OBJECT(14);
// MSGEQ7 bands = 63Hz, 160Hz, 400Hz, 1,000Hz, 2,500Hz, 6,250Hz, 16,000Hz
int ctrlReset = 2;                                    // Digital pin 2 = signal to reset MSGEQ7s
int ctrlStrobe = 4;                                   // Digital pin 4 = signal to strobe (read data from) MSGEQ7s
int channelLeft = A5;                                 // Analog pin A5 = spectrum data from left channel
int channelRight = A4;                                // Analog pin A4 = spectrum data from right channel
int spectrumLeft[7];                                  // Array to store 7 bands of spectrum data from left channel
int spectrumRight[7];                                 // Array to store 7 bands of spectrum data from right channel
int pwmLEDsLeft[] = {3, 5, 6, 7, 8, 9, 10};           // Array to store data where leds are connected.
int pwmLEDsRight[] = {11, 12, 13, 14, 15, 16, 17};    // Array to store data where leds are connected.
int brightness = 50;
String data = "ON";                                   // Mode ON = Music, 2 = Still, Anything else = off
String bluetooth;

void setup()
{
  Serial.begin(115200);
  pinMode(ctrlReset, OUTPUT);                         // Define reset as output
  pinMode(ctrlStrobe, OUTPUT);                        // Define strobe as output
  digitalWrite(ctrlReset, LOW);                       // Pull the reset signal low
  digitalWrite(ctrlStrobe, HIGH);                     // Drive the strobe signal high
  Palatis::SoftPWM.begin(60);                         // Begin with 60hz pwm frequency
  //Palatis::SoftPWM.printInterruptLoad();              // Print interrupt load for diagnostic purposes
}


void loop()
{
  while (Serial.available())
  {
    delay(10);
    char c = Serial.read();
    if (c == ':')
    {
      break;
    }
    bluetooth += c;
  }
  
  if (bluetooth.length() > 0) {
    data = bluetooth;
    bluetooth = "";
  }
  
  readMSGEQ7();
  writeLEDs();
  
  Serial.print(data);
  Serial.print("\n");
}

void readMSGEQ7()
{ // Read the seven spectrum bands from the MSGEQ7 chips
  digitalWrite(ctrlReset, HIGH);                        // Pulse the reset signal, which causes
  digitalWrite(ctrlReset, LOW);                         // the MSGEQ7s to latch the spectrum values
  delayMicroseconds(72);                                // Delay to meet minimum reset-to-strobe time
  int nF = 50;                                          // Noise filter level.
  for (int i = 0; i < 7; i++)                           // Cycle through the 7 spectrum bands
  {
    digitalWrite(ctrlStrobe, LOW);                      // Read current band (then increment to next band)
    delayMicroseconds(36);                              // Wait for outputs to settle
    spectrumLeft[i] = analogRead(channelLeft) / 4;      // Store current values from left & right channels
    spectrumRight[i] = analogRead(channelRight) / 4;    // Divide 0-1023 by 10 to give about 0-100
    //if (spectrumLeft[i] < nF) spectrumLeft[i] = 1;    // Noise filter left
    //if (spectrumRight[i] < nF) spectrumRight[i] = 1;  // Noise filter right
    digitalWrite(ctrlStrobe, HIGH);
    delayMicroseconds(36);                              // Delay to meet minimum strobe-to-strobe time
  }
}

void writeLEDs()
{
  if (data == "ON")
  {
    for (int i = 0; i < 7; i++) {
      Palatis::SoftPWM.set(pwmLEDsLeft[i], spectrumLeft[i]);
      Palatis::SoftPWM.set(pwmLEDsRight[i], spectrumRight[i]);
      Serial.print(spectrumLeft[i]);
      if (spectrumLeft[i] < 10) Serial.print(" ");
      Serial.print(" ");
      Serial.print(spectrumRight[i]);
      if (spectrumRight[i] < 10) Serial.print(" ");
      Serial.print(" | ");
    }
  }
  else 
  {
    Palatis::SoftPWM.allOff();
  }
  Serial.print("\n");
}
