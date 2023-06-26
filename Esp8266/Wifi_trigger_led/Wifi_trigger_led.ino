#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>

#define WIFI_SSID "Jack"
#define WIFI_PASSWORD "0975732120"
#define LED_PIN D0

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  digitalWrite(LED_PIN, HIGH);
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  wifiReconnectTimer.once(2, connectToWifi);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Start Now");
  pinMode (LED_PIN , OUTPUT);
  digitalWrite(LED_PIN, LOW);
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
  connectToWifi();
}

void loop() {
  // put your main code here, to run repeatedly:

}