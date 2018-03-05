#include "config.h"
#include <Arduino.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
const int pin_SIM800_Rx = 2;
const int pin_SIM800_Tx = 3;
const int pin_SIM800_Boot = 5;
const int pin_Relay_Cold = 6;
const int pin_Relay_Hot = 7;
const int pin_LED = 8;

SoftwareSerial SerialAT(pin_SIM800_Rx, pin_SIM800_Tx); // RX, TX
TinyGsm modem(SerialAT);
TinyGsmClient gsm(modem);
PubSubClient client(gsm);


bool OverTemperature = false;
bool UnderTemperature = false;
int SetPointTemp_C = 20; // default
int MinTempDifference_C = 0.5; // default

unsigned long MinTimeDelay_ms = 30000; // default
unsigned long LastTriggeredCold_ms = 0;
unsigned long LastTriggeredHot_ms = 0;

bool ColdPassedTimeDelay = false;
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

void pwr() {
  modem.poweroff();
  delay (1000);
  Serial.println("booting modem ..");
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  delay(5000);
  digitalWrite(5, HIGH);
  delay(2000);
}

void GSM_init() {
  pwr();

  Serial.println("Initializing modem...");
  modem.restart();


  DBG("Waiting for network...");
  if (!modem.waitForNetwork()) {
    Serial.println(" fail");
    while (true);
  }
  Serial.println(" OK");

  // GSM_network();

}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  Serial.println();
}

void setup() {
    Serial.begin(9600);
    delay(10);
    SerialAT.begin(9600);

    pinMode(pin_Relay_Cold, OUTPUT);
    pinMode(pin_Relay_Hot, OUTPUT);
    digitalWrite(pin_Relay_Cold, LOW);
    digitalWrite(pin_Relay_Hot, LOW);

    GSM_init(); // off, boot, restart, connect to service network, register apn, etc

    client.setServer(broker, 1883);
    client.setCallback(mqttCallback);
}

void loop() {
    // put your main code here, to run repeatedly:
    SetStateCold();
    SetStateHot();
    //
    // if (!client.connected()) {
    //     reconnect();
    // }
    //
    //
    // client.loop();
}
