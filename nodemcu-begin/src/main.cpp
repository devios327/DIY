#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const byte ROTATOR_PIN = A0;
int ROTATOR_VAL, ROTATOR_VAL_PREV = 0;

int period = 1000;
unsigned long time_now = 0;

const char *AC_NAME = "BtnPnlInit";
const char *AC_PASS = "password12345678";

ESP8266WebServer server;
AutoConnect Portal(server);
AutoConnectConfig Config(AC_NAME, AC_PASS);

//Send value from rotator to server
//Value 0-100
void sendRotatorVal(int n)
{
  //Serial.printf("Rotator has new value %d", n);

  char JSON[30];
  sprintf(JSON, "{\"rotatorValue\": \"%d\"}", n);

  WiFiClient client;
  HTTPClient http;
  http.begin(client, "http://159.93.121.58:8080/rotatorValue");
  http.addHeader("Content-Type", "application/json");
  byte httpResponseCode = http.POST(JSON);

  if (httpResponseCode > 0)
  {
    //Serial.printf("[HTTP] POST... code: %d\n", httpResponseCode);

    if (httpResponseCode != HTTP_CODE_OK)
    {
      //Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    }
  }

  http.end();
}

void getRotatorValue()
{
  ROTATOR_VAL = map(analogRead(ROTATOR_PIN), 0, 1024, 0, 360);
  Serial.println(ROTATOR_VAL);
}

void setup()
{
  //pinMode(ROTATOR_PIN, INPUT);
  pinMode(ROTATOR_PIN,INPUT_PULLUP);
  
  Serial.begin(9600);
  delay(1000);

  Serial.println("setup is loading");
  Serial.println();

  Portal.config(Config);
  if (Portal.begin())
    Serial.println("WiFi connected: " + WiFi.localIP().toString());

  getRotatorValue();

  ROTATOR_VAL_PREV = ROTATOR_VAL;

  sendRotatorVal(ROTATOR_VAL);
}

void loop()
{
  getRotatorValue();

  if (ROTATOR_VAL != ROTATOR_VAL_PREV)
  {
    //Serial.print("Rotator changed: ");
    //Serial.println(ROTATOR_VAL);
    ROTATOR_VAL_PREV = ROTATOR_VAL;

    sendRotatorVal(ROTATOR_VAL);
  }

  delay(10);
}