#include "DigitLedDisplay.h"

const int SRDataPin = 2; 
const int SRLatchPin = 3;
const int SRClockPin = 4;
const int restartButtonPin = 5;
const int reactionButtonPin = 6;
const int ledSegmentsDIN = 8;
const int ledSegmentsCS = 9;
const int ledSegmentsCLK = 10;
const int speakerPin = 12;
int waitTime = 0;
int startTime = 0;
int reactionTime = 0;
byte SRData = 0; // Shift Register 8-bit storage
boolean trafficLightsEnded = false;

// Set up led display library object with correct pin numbers
DigitLedDisplay ld = DigitLedDisplay(ledSegmentsDIN, ledSegmentsCS, ledSegmentsCLK);

void setup() {
  Serial.begin(9600);
  ld.setBright(6);
  ld.setDigitLimit(8);
  pinMode(reactionButtonPin, INPUT);
  pinMode(restartButtonPin, INPUT);
  pinMode(SRDataPin, OUTPUT);
  pinMode(SRClockPin, OUTPUT);
  pinMode(SRLatchPin, OUTPUT);
  turnOffTrafficLights();
}

void loop() {
  if(restartButtonIsPushed()) {
    startTrafficLights();
  }

  if(trafficLightsEnded == true && reactionButtonIsPushed()) {
    ld.clear();
    reactionTime = millis() - startTime;
    trafficLightsEnded = false;
    ld.printDigit(reactionTime);
    Serial.print(reactionTime);
    Serial.print('\n');
  }
}

boolean restartButtonIsPushed() {
  return digitalRead(restartButtonPin)==HIGH?true:false;
}

boolean reactionButtonIsPushed() {
  return digitalRead(reactionButtonPin)==HIGH?true:false;
}

void turnOffTrafficLights() {
  SRWrite(3, LOW);
  SRWrite(4, LOW);
  SRWrite(5, LOW);
  SRWrite(6, LOW);
  SRWrite(7, LOW);
}

void startTrafficLights() {
  // Calculate random wait time between 1 and 3 seconds 
  waitTime = random(1000, 3000);
  
  // Traffic lights pattern with 5 LEDS from pins 3 to 7 on the shift register
  for(int ledIndex=3; ledIndex<=7; ledIndex++) {
    delay(1000);
    noTone(speakerPin);
    SRWrite(ledIndex, HIGH);
    tone(speakerPin, 659, 100);
  }
  
  delay(waitTime);
  turnOffTrafficLights();
  tone(speakerPin, 1319, 100);
  trafficLightsEnded = true;  
  startTime = millis();
}

// Write to the shift register memory
void SRWrite(int SRPin, boolean SRPinState){
  // Set the pin state in the data variable
  bitWrite(SRData,SRPin,SRPinState); 
  // Shift the storage to apply the changes
  shiftOut(SRDataPin, SRClockPin, MSBFIRST, SRData); 
  // Activate the latch to let the data pass from the storage to the shift register pins
  digitalWrite(SRLatchPin, HIGH);
  digitalWrite(SRLatchPin, LOW);
}
