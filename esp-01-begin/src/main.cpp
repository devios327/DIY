/*
#include <Arduino.h>
#include "ESP8266WiFi.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(true) {
    Serial.println("Scan wi-fi networks...");
    int numberOfNetworks = WiFi.scanNetworks();

    for(int i =0; i<numberOfNetworks; i++){
        Serial.print("Network name: ");
        Serial.println(WiFi.SSID(i));
        Serial.print("Signal strength: ");
        Serial.println(WiFi.RSSI(i));
        Serial.println("-----------------------");
    }

    delay(1000);
  }
}
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const int BUTTONS_PIN = A0;

int currentButton = 0;
int pressedButton = 0;
int period = 1000;
unsigned long time_now = 0;

const char *AC_NAME = "BtnPnlInit";
const char *AC_PASS = "password12345678";

ESP8266WebServer server;
AutoConnect Portal(server);
AutoConnectConfig Config(AC_NAME, AC_PASS);

void sendPressedButton(byte n)
{
  Serial.printf("Button %d pressed. ", n);

  char JSON[80];
  sprintf(JSON, "{\"button\": \"%d\", \"timestamp\": \"%d\"}", n, time_now);

  WiFiClient client;
  HTTPClient http;
  http.begin(client, "http://159.93.121.58:8080/button");
  http.addHeader("Content-Type", "application/json");
  byte httpResponseCode = http.POST(JSON);

  // httpResponseCode will be negative on error
  if (httpResponseCode > 0)
  {
    Serial.printf("[HTTP] POST... code: %d\n", httpResponseCode);

    if (httpResponseCode != HTTP_CODE_OK)
    {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    }
  }

  http.end();
}

void setup()
{
  pinMode(BUTTONS_PIN, INPUT);
  Serial.begin(9600);
  delay(1000);

  Serial.println("setup is loading");
  Serial.println();

  Portal.config(Config);
  if (Portal.begin())
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
}

void loop()
{
  // Portal.handleClient();

  if (millis() >= time_now + period)
  {
    Serial.println("send json");
    time_now += period;

    sendPressedButton(2);
  }
}