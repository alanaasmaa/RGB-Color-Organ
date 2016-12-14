// MSGEQ7 bands = 63Hz, 160Hz, 400Hz, 1,000Hz, 2,500Hz, 6,250Hz, 16,000Hz
int ctrlReset    = 2;                                 // Digital pin 2 = signal to reset MSGEQ7s
int ctrlStrobe   = 4;                                 // Digital pin 4 = signal to strobe (read data from) MSGEQ7s
int channelLeft  =  A5;                               // Analog pin A5 = spectrum data from left channel
int channelRight =  A4;                               // Analog pin A4 = spectrum data from right channel
int spectrumLeft[7];                                  // Array to store 7 bands of spectrum data from left channel
int spectrumRight[7];                                 // Array to store 7 bands of spectrum data from right channel
int pwmLEDsLeft[] = {3,5,6,7,8,9,10};                 //Array to store data where leds are connected.
int pwmLEDsRight[] = {11,12,13,A0,A1,A2,A3};          //Array to store data where leds are connected.
String data = "Music";                                // Mode ON = Music, 2 = Still, Anything else = off
String bluetooth;
float brightness = 1;

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
    if ((bluetooth.toInt() >=1)&&(bluetooth.toInt() <=100))
    {
      brightness = bluetooth.toFloat() / 100;
    }
    else
    {
      data = bluetooth;
      data.trim();
    }
    bluetooth = "";
  }
  if (data == "Music")
  {
    readMSGEQ7();
    musicMode();
  }
  if (data == "Still")
  {
    stillMode();
  }
  if (data == "Random")
  {
    randomMode();
  }
  if (data == "Off")
  {
    ledsOff();
  }
  if (data == "Debug")
  {
    readMSGEQ7();
    musicMode();
    Serial.print(data);
    Serial.print(":");
    Serial.print(brightness);
    Serial.print("| ");
  }
}
void readMSGEQ7() {
  // Read the seven spectrum bands from the MSGEQ7 chips
  digitalWrite(ctrlReset, HIGH);                        // Pulse the reset signal, which causes
  digitalWrite(ctrlReset, LOW);                         // the MSGEQ7s to latch the spectrum values
  delayMicroseconds(72);                                // Delay to meet minimum reset-to-strobe time
  int nF = 50 * brightness;                             // Noise filter level.
  for (int i = 0; i < 7; i++)                           // Cycle through the 7 spectrum bands
  {
    digitalWrite(ctrlStrobe, LOW);                      // Read current band (then increment to next band)
    delayMicroseconds(36);                              // Wait for outputs to settle
    spectrumLeft[i] = analogRead(channelLeft) / 4;      // Store current values from left & right channels
    spectrumRight[i] = analogRead(channelRight) / 4;    // Divide 0-1023 by 4 to give 0-255
    //if (spectrumLeft [i] < nF) spectrumLeft [i] = 0;    // Noise filter left
    //if (spectrumRight [i] < nF) spectrumRight [i] = 0;  // Noise filter right
    digitalWrite(ctrlStrobe, HIGH);
    delayMicroseconds(36);                              // Delay to meet minimum strobe-to-strobe time
  }
}
void musicMode() {
  for (int i = 0; i < 7; i++) {
    analogWrite(pwmLEDsLeft[i], spectrumLeft[i] * brightness);
    analogWrite(pwmLEDsRight[i], spectrumRight[i] * brightness);
    if (data == "Debug") {
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
  if (data == "Debug") {
    Serial.print("\n");
  }
}

void stillMode() {
  for (int i = 0; i < 7; i++) {
    analogWrite(pwmLEDsLeft[i], 255 * brightness);
    analogWrite(pwmLEDsRight[i], 255 * brightness);
  }
}

void randomMode() {
  for (int i = 0; i < 7; i++) {
    analogWrite(pwmLEDsLeft[i], random(255) * brightness);
    analogWrite(pwmLEDsRight[i], random(255) * brightness);
  }
}

void ledsOff() {
  for (int i = 0; i < 7; i++) {
    analogWrite(pwmLEDsLeft[i], 0);
    analogWrite(pwmLEDsRight[i], 0);
  }
}

