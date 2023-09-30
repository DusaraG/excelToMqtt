#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *SSID = "OnePlus 7T";              // SSID of your WiFi
const char *PASSWORD = "qwertasdf";           // Password of your WiFi
const char *mqttBroker = "broker.mqtt.cool";  // alternate hosts: test.mosquitto.org, broker.hivemq.com
const int mqttPort = 1883;
const char *mqttClientID = "esp32";
const char *Topic = "P1T1";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char *potRange(int value) {
  if (value >= 70) {
    return "High";
  } else if (value < 70 && value >= 30) {
    return "Medium";
  } else {
    return "Low";
  }
}

const char *ldrRange(int value) {
  if (value >= 20) {
    return "High";
  } else if (value < 20 && value >= 5) {
    return "Medium";
  } else {
    return "Low";
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);
  Serial.println(doc.as<String>());
  Serial.print(ldrRange(doc["LDR_value"]));
  const char *ldr_value = ldrRange(doc["LDR_value"]);
  const char *pot_value = potRange(doc["POT_value"]);

  Serial.println(ldr_value);
  Serial.println(pot_value);

  if ((strcmp(ldr_value, "Low") == 0 && strcmp(pot_value, "Medium") == 0) ||
      (strcmp(ldr_value, "Low") == 0 && strcmp(pot_value, "Low") == 0) ||
      (strcmp(ldr_value, "Medium") == 0 && strcmp(pot_value, "Low") == 0)) {
    Serial.print("inside the loop!");
    for (int i = 0; i < 5; i++) {
      digitalWrite(D0, HIGH);
      digitalWrite(D1, HIGH);
      digitalWrite(D2, HIGH);
      delay(500);
      digitalWrite(D0, LOW);
      digitalWrite(D1, LOW);
      digitalWrite(D2, LOW);
      delay(500);
    }
  }
  if ((strcmp(ldr_value, "Medium") == 0 && strcmp(pot_value, "Medium") == 0) ||
      (strcmp(ldr_value, "Low") == 0 && strcmp(pot_value, "High") == 0) ||
      (strcmp(ldr_value, "High") == 0 && strcmp(pot_value, "Low") == 0)) {
    digitalWrite(D0, LOW);
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
  }
  if ((strcmp(ldr_value, "Medium") == 0 && strcmp(pot_value, "High") == 0) ||
      (strcmp(ldr_value, "High") == 0 && strcmp(pot_value, "Medium") == 0) ||
      (strcmp(ldr_value, "High") == 0 && strcmp(pot_value, "High") == 0)) {
    digitalWrite(D0, HIGH);
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
  }
}

void mqttInit() {
  mqttClient.setServer(mqttBroker, mqttPort);
  mqttClient.setCallback(mqttCallback);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttLoop() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect(mqttClientID)) {
      Serial.println("connected");
      mqttClient.subscribe(Topic);  // Subscribe to LEDTopic
      Serial.println("MQTT subscribed");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
  mqttClient.loop();
}

void pinInit() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
}

void setup() {
  Serial.begin(115200);
  Serial.println(Topic);
  mqttInit();
  pinInit();
}

void loop() {
  mqttLoop();
}
