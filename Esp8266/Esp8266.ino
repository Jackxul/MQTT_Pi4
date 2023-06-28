#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>

#include <OneWire.h> 
#include <DallasTemperature.h> 

#define WIFI_SSID "*#ssid"
#define WIFI_PASSWORD "*#pswe"

const int P_resistor = A0;
#define DQ_Pin D1
#define Boot_PIN D0
int light=0;


#define MQTT_HOST IPAddress(*#ip) //MQTT BROKER IP ADDRESS
/*for example:
#define MQTT_HOST IPAddress(192, 168, 1, 106)*/
#define MQTT_PORT *#port
#define BROKER_USER "*#uid"
#define BROKER_PASS "*#pswd"

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

OneWire oneWire(DQ_Pin);
DallasTemperature sensors(&oneWire);

unsigned long previousMillis = 0;   // Stores last time a message was published
const long interval = 1000;        // Interval at which to publish values

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  digitalWrite(LED_BUILTIN, HIGH);
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  digitalWrite(LED_BUILTIN, LOW);
  wifiReconnectTimer.once(2, connectToWifi);
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  // Subscribe to topic "led" when it connects to the broker
  uint16_t packetIdSub = mqttClient.subscribe("led", 2);
  uint16_t packetIdSub2 = mqttClient.subscribe("popen", 2);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);

// Publish on the "test" topic with qos 0
  //mqttClient.publish("test", 0, true, "test 1");
  //Serial.println("Publishing at QoS 0");
// Publish on the "test" topic with qos 1
  //uint16_t packetIdPub1 = mqttClient.publish("test", 1, true, "test 2");
  //Serial.print("Publishing at QoS 1, packetId: ");
  //Serial.println(packetIdPub1);
// Publish on the "test" topic with qos 2
  //uint16_t packetIdPub2 = mqttClient.publish("test", 2, true, "test 3");
  //Serial.print("Publishing at QoS 2, packetId: ");
  //Serial.println(packetIdPub2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  // Do whatever you want when you receive a message

  // Save the message in a variable
  String receivedMessage;
  for (int i = 0; i < len; i++) {
    Serial.println((char)payload[i]);
    receivedMessage += (char)payload[i];
  }
  // Turn the LED on or off accordingly to the message content
  if (strcmp(topic, "popen") == 0) {
    if (receivedMessage == "true"){
      digitalWrite(Boot_PIN, LOW);
    }
    else {
      digitalWrite(Boot_PIN, HIGH);
    }
  }
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  pinMode (LED_BUILTIN, OUTPUT);
  pinMode(P_resistor, INPUT);
  pinMode (Boot_PIN, OUTPUT);
  digitalWrite(Boot_PIN, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  sensors.begin();
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(BROKER_USER, BROKER_PASS);


  connectToWifi();
}

void loop() {

  unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds) 
  // it publishes a new MQTT message
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    light=analogRead(P_resistor);
    Serial.println(light);
    if(light == 1024){
      digitalWrite(Boot_PIN , LOW);
      delay(500);
      digitalWrite(Boot_PIN , HIGH);
    }
    //Serial.println(sensors.getTempCByIndex(0));
  // Publish an MQTT message on topic esp/bme680/temperature
  uint16_t packetIdPub1 = mqttClient.publish("counter", 1, true, String(light).c_str());
  //uint16_t packetIdPub2 = mqttClient.publish("temp", 1, true, String(sensors.getTempCByIndex(0)).c_str());
  //Serial.printf("Publishing on topic %s at QoS 1, packetId: %i", "counter", packetIdPub1);
  }
}