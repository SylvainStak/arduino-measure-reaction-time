#include "LedControl.h"

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
LedControl lc = LedControl(8,10,9,1);

void setup() {
  Serial.begin(9600);
  pinMode(reactionButtonPin, INPUT);
  pinMode(restartButtonPin, INPUT);
  pinMode(SRDataPin, OUTPUT);
  pinMode(SRClockPin, OUTPUT);
  pinMode(SRLatchPin, OUTPUT);
  turnOffTrafficLights();
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);
}

void loop() {
  if(restartButtonIsPushed()) {
    startTrafficLights();
  }

  if(trafficLightsEnded == true && reactionButtonIsPushed()) {
    lc.clearDisplay(0);
    reactionTime = millis() - startTime;
    trafficLightsEnded = false;
    showReactionTime();
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

int numberOfDigits(int number) {
  int digits = 0;
  while(number){
    number /= 10;
    digits++;
  }
  return digits;
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

// Show reaction time on 7 segment display
void showReactionTime() {
  int number = reactionTime;
  int digits = numberOfDigits(number);
  int arr[digits];
  boolean isDecimal = false;

  // Transform the reaction time to an integer array
  for (int i = digits-1; i >= 0; i--) {
    arr[i] = number % 10;
    number /= 10;
  }

  // Print the correct numbers with decimal points
  for (int i = digits-1; i >= 0; i--) {
    if(digits > 3) {
      isDecimal = i==3?true:false;
    } else {
      lc.setDigit(0,3,0,true);
      if(digits == 2){
        lc.setDigit(0,2,0,false);
      }
      if(digits == 1){
        lc.setDigit(0,2,0,false);
        lc.setDigit(0,1,0,false);
      }
    }
    lc.setDigit(0,i,arr[(digits-1)-i],isDecimal);
  }
}
