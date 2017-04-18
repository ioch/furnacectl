#include "MAX31855.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <Messenger.h>
#include <PID_v1.h>

static const byte doPin = 12;
static const byte csPin = 10;
static const byte clPin = 13;
static const byte encode1 = 2;
static const byte encode2 = 3;
static const byte buttEnc = 9;
static const byte buttRed = 8;
static const byte ledr = 7;
static const byte ledg = 6;
static const byte ssr = A2;

LiquidCrystal_I2C lcd(0x27,20,4);
MAX31855 tc(clPin, csPin, doPin);
Encoder encoder(encode1, encode2);
Messenger message = Messenger();

unsigned long timeToSend = 0;
double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint,500,1,100, DIRECT);
int WindowSize = 2000;
unsigned long windowStartTime;

void setup()
{
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  tc.begin();
  pinMode(ssr, OUTPUT);
  digitalWrite(ssr, HIGH);
  pinMode(ledr, OUTPUT);
  pinMode(ledg, OUTPUT);
  pinMode(buttEnc, INPUT);
  pinMode(buttRed, INPUT);
  message.attach(message_ready);
  windowStartTime = millis();
  Setpoint = 25;
  myPID.SetOutputLimits(0, WindowSize);
  myPID.SetSampleTime(1000);
  myPID.SetMode(AUTOMATIC);  //turn the PID on
}

void resetFunc(){
  asm("jmp 0x3800");
}

char buffer[30];

void message_ready() {
    while (message.available()) {
      if ( message.checkString("RESET") ) {
        resetFunc();
      }
      else{
        message.copyString(buffer, 30);
        Serial.println(buffer);
      }
    }
}


long oldPosition  = -999;
bool lastButtonStateRed = HIGH;
bool lastButtonStateEnc = HIGH;
float temp;
float ambient;

void refreshScr() {
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print(temp);
  lcd.setCursor(10,0);
  lcd.print(ambient);
  lcd.setCursor(2,1);
  lcd.print(oldPosition>>2);
}

void loop()
{
  bool newButtonStateRed = digitalRead(buttRed);
  bool newButtonStateEnc = digitalRead(buttEnc);
  int status = tc.read();
  if (status != 0) 
  {
    Serial.print("stat:\t\t");
    Serial.println(status);
  }

  temp = tc.getTemperature();
  ambient = tc.getInternal();
//  Serial.println(temp);
  

  long newPosition = encoder.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;

  }
  if((LOW == newButtonStateRed) && (HIGH == lastButtonStateRed)){
    digitalWrite(ledr, !digitalRead(ledr));
  }
  if((LOW == newButtonStateEnc) && (HIGH == lastButtonStateEnc)){
    digitalWrite(ledg, !digitalRead(ledg));
  }
  lastButtonStateRed = newButtonStateRed;
  lastButtonStateEnc = newButtonStateEnc;
  Input=temp;
  myPID.Compute();
  long now = millis();
  if(now - windowStartTime>WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
    refreshScr();
  }
  if(Output < now - windowStartTime) digitalWrite(ssr,LOW);
  else digitalWrite(ssr,HIGH);

  while (Serial.available()) {
    message.process(Serial.read());
  }
}
