#include <Arduino.h>

const int pin_SIM800_Tx = 3;
const int pin_SIM800_Rx = 2;
const int pin_SIM800_Boot = 5;
const int pin_Relay_Cold = 6;
const int pin_Relay_Hot = 7;
const int pin_LED = 8;

bool OverTemperature = false;
bool UnderTemperature = false;
int SetPointTemp_C = 20; // default
int MinTempDifference_C = 0.5; // default

unsigned long MinTimeDelay_ms = 30000; // default
unsigned long LastTriggeredCold_ms = 0;
unsigned long LastTriggeredHot_ms = 0;

bool ColdPassedTimeDelay false;
bool HotPassedTimeDelay = false;

void CheckTemperature()
{
    int currentTemp_C = 15; //get from sensor
    OverTemperature = currentTemp_C > SetPointTemp_C + MinTempDifference_C;
    UnderTemperature = currentTemp_C < SetPointTemp_C - MinTempDifference_C;
}

void CheckTimeDelays()
{
    ColdPassedTimeDelay = millis() - LastTriggeredCold_ms > MinTimeDelay_ms;
    HotPassedTimeDelay = millis() - LastTriggeredHot_ms > MinTimeDelay_ms;
}

void SetStateCold()
{
    CheckTemperature();
    CheckTimeDelays();
    if(OverTemperature && ColdPassedTimeDelay)
    {
        digitalWrite(pin_Relay_Cold, HIGH);
    }
    else
    {
        digitalWrite(pin_Relay_Cold, LOW);
    }
}

void SetStateHot()
{
    CheckTemperature();
    CheckTimeDelays();
    if(UnderTemperature && HotPassedTimeDelay)
    {
        digitalWrite(pin_Relay_Hot, HIGH);
    }
    else
    {
        digitalWrite(pin_Relay_Hot, LOW);
    }
}

void setup() {
    // put your setup code here, to run once:
}

void loop() {
    // put your main code here, to run repeatedly:
    SetStateCold();
    SetStateHot();
}
