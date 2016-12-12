// MSGEQ7 bands = 63Hz, 160Hz, 400Hz, 1,000Hz, 2,500Hz, 6,250Hz, 16,000Hz
int ctrlReset    = 2;                                 // Digital pin 2 = signal to reset MSGEQ7s
int ctrlStrobe   = 4;                                 // Digital pin 4 = signal to strobe (read data from) MSGEQ7s
int channelLeft  =  A5;                               // Analog pin A5 = spectrum data from left channel
int channelRight =  A4;                               // Analog pin A4 = spectrum data from right channel
int spectrumLeft[7];                                  // Array to store 7 bands of spectrum data from left channel
int spectrumRight[7];                                 // Array to store 7 bands of spectrum data from right channel
int pwmLEDsLeft[] = {3,5,6,7,8,9,10};                 //Array to store data where leds are connected.
int pwmLEDsRight[] = {11,12,13,A0,A1,A2,A3};          //Array to store data where leds are connected.
String data = "1";                                   // Mode ON = Music, 2 = Still, Anything else = off
String bluetooth;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ctrlReset, OUTPUT);                         // Define reset as output
  pinMode(ctrlStrobe, OUTPUT);                        // Define strobe as output
  digitalWrite(ctrlReset, LOW);                       // Pull the reset signal low
  digitalWrite(ctrlStrobe, HIGH);                     // Drive the strobe signal high
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
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
  if (data == "1")
  {
    readMSGEQ7();
    writeLEDs();
  }
  else 
  {
    for (int i = 0; i < 7; i++) {
      analogWrite(pwmLEDsLeft[i], 0);
      analogWrite(pwmLEDsRight[i], 0);
    }
  }
  
  Serial.print(data);
  Serial.print("\n");
}
void readMSGEQ7() {
  // Read the seven spectrum bands from the MSGEQ7 chips
  digitalWrite(ctrlReset, HIGH);                        // Pulse the reset signal, which causes
  digitalWrite(ctrlReset, LOW);                         // the MSGEQ7s to latch the spectrum values
  delayMicroseconds(72);                                // Delay to meet minimum reset-to-strobe time
  int nF = 50;                                          // Noise filter level.
  for (int i = 0; i < 7; i++)                           // Cycle through the 7 spectrum bands
  {
    digitalWrite(ctrlStrobe, LOW);                      // Read current band (then increment to next band)
    delayMicroseconds(36);                              // Wait for outputs to settle
    spectrumLeft[i] = analogRead(channelLeft) / 4;      // Store current values from left & right channels
    spectrumRight[i] = analogRead(channelRight) / 4;    // Divide 0-1023 by 4 to give 0-255
    if (spectrumLeft [i] < nF) spectrumLeft [i] = 0;    // Noise filter left
    if (spectrumRight [i] < nF) spectrumRight [i] = 0;  // Noise filter right
    digitalWrite(ctrlStrobe, HIGH);
    delayMicroseconds(36);                              // Delay to meet minimum strobe-to-strobe time
  }
}
void writeLEDs() {
  for (int i = 0; i < 7; i++) {
    analogWrite(pwmLEDsLeft[i], spectrumLeft[i]);
    analogWrite(pwmLEDsRight[i], spectrumRight[i]);
    Serial.print(spectrumLeft[i]);
    if (spectrumLeft[i] < 10) Serial.print(" ");
    if (spectrumLeft[i] < 100) Serial.print(" ");
    Serial.print(" ");
    Serial.print(spectrumRight[i]);
    if (spectrumRight[i] < 10) Serial.print(" ");
    if (spectrumRight[i] < 100) Serial.print(" ");
    Serial.print(" | ");
  }
}

