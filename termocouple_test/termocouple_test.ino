#include "MAX31855.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <Messenger.h>
#include <PID_v1.h>
#include "ProgramProfile.h"

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

void updateSetpoint(uint16_t sp);
void programDone();

LiquidCrystal_I2C lcd(0x27,20,4);
MAX31855 tc(clPin, csPin, doPin);
Encoder encoder(encode1, encode2);
Messenger message = Messenger();
ProgramProfile program(updateSetpoint, programDone);

#define refreshPeriod 500
unsigned long timeToSend = 0;

double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint,40,0,0, DIRECT);
int WindowSize = 2000;

unsigned long windowStartTime;
unsigned long refreshTime;
unsigned int power=0;
float temp;
float ambient;

enum modes {
    IDLE,
    POWER,
    POINT,
    PROGR
} mode = IDLE;

char mode_str[4][6] = {"IDLE ", "POWER", "POINT", "PROGR"};

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  tc.begin();
  pinMode(ssr, OUTPUT);
  digitalWrite(ssr, LOW);
  pinMode(ledr, OUTPUT);
  pinMode(ledg, OUTPUT);
  pinMode(buttEnc, INPUT);
  pinMode(buttRed, INPUT);
  message.attach(message_ready);
  windowStartTime = millis();
  refreshTime = millis();
  Setpoint = 15;
  myPID.SetOutputLimits(0, WindowSize);
  myPID.SetSampleTime(1000);
  myPID.SetMode(MANUAL);  //turn the PID on
}

void resetFunc(){
  digitalWrite(ssr, LOW);
  asm("jmp 0x3800");
}

void printStatus(){
  Serial.print(millis());
  Serial.print(",");
  Serial.print(Output);
  Serial.print(",");
  Serial.print(Setpoint);
  Serial.print(",");
//  Serial.print(mode);
//  Serial.print(",");
  Serial.print(ambient);
  Serial.print(",");
  Serial.print(temp);
  Serial.println();
}

char buffer[30];

void message_ready() {
    while (message.available()) {
      if (message.checkString("RESET") ) {
        resetFunc();
      }else if (message.checkString("point") ) {
        Setpoint = message.readInt();
        Serial.print("Setpoint :");
        Serial.println(Setpoint);
      }else if (message.checkString("power") ) {
        power = message.readInt();
        Serial.print("Power :");
        Serial.println(power);
      }else if (message.checkString("mode") ) {
        chmod(message.readInt());
        Serial.print(mode_str[mode]);
        Serial.println(mode);
      }else if (message.checkString("temp") ) {
        Serial.print(temp);
      }else if (message.checkString("status") ) {
        printStatus();
      }else if (message.checkString("help") ) {
        Serial.println("RESET");
        Serial.println("power <pwr>");
        Serial.println("mode <mode>");
        Serial.println("temp");
        Serial.println("status");
        Serial.println("help");
      }else{
        message.copyString(buffer, 30);
        Serial.print("bullshit");
        Serial.println(buffer);
      }
    }
}


long oldPosition  = 0;
bool lastButtonStateRed = HIGH;
bool lastButtonStateEnc = HIGH;
bool newButtonStateRed;
bool newButtonStateEnc;

void chmod(modes newMode){
  switch(newMode){
    case IDLE :
      myPID.SetMode(MANUAL);
      pinMode(ssr, INPUT);
    break;
    case POWER :
      myPID.SetMode(MANUAL);
      pinMode(ssr, OUTPUT);
    break;
    case POINT :
    //
      pinMode(ssr, OUTPUT);
      myPID.SetMode(AUTOMATIC);
    break;
    case PROGR :
    //do program loading
      pinMode(ssr, OUTPUT);
      program.start(temp);
      myPID.SetMode(AUTOMATIC);
    break;
  }
  mode = newMode;
}

void refreshScr() {
  printStatus();
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print(temp);
  lcd.setCursor(10,0);
  lcd.print(mode_str[mode]);
  lcd.setCursor(0,1);
  if (mode == IDLE){
    lcd.print(buffer);
  }
  if (mode == POINT){
    lcd.print("Set ");
    lcd.print(Setpoint +(float) oldPosition /16);
  }
  if (mode == PROGR){
    lcd.print("Set ");
    lcd.print(Setpoint);
  }
  if (mode == POWER){
    lcd.print("Pwr ");
    lcd.print(power);    
  }

}

static byte checkbutton(){
  return ((LOW == newButtonStateRed) && (HIGH == lastButtonStateRed));
}

void loop() {
  newButtonStateRed = digitalRead(buttRed);
  newButtonStateEnc = digitalRead(buttEnc);
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
    //digitalWrite(ledg, !digitalRead(ledg));
    Setpoint+= (float) oldPosition /16;
    encoder.write(0);
  }
  lastButtonStateRed = newButtonStateRed;
  lastButtonStateEnc = newButtonStateEnc;
  Input=temp;

  
  myPID.Compute();
  if (POWER == mode){
    Output = power;
  }

  long now = millis();
  if(now - windowStartTime > WindowSize) {
    windowStartTime = now;
  }

  if(now - refreshTime > refreshPeriod) {
    refreshScr();
    refreshTime  = now;
  }

//TODO: use only descrete output 10ms intervals
  if(Output < now - windowStartTime) {
    digitalWrite(ssr,LOW);
  } else {
    digitalWrite(ssr,HIGH);
  }

  while (Serial.available()) {
    message.process(Serial.read());
  }

  program.compute(temp);
}

void updateSetpoint(uint16_t sp) {
    Setpoint = sp;
}

void programDone() {
  chmod(IDLE);
}

