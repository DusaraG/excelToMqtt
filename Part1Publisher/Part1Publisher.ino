#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <cstring>
#include <ArduinoJson.h>

char out[128];
const int potPin = A3;
const int ldrPin = A0;

const char* ssid = "OnePlus 7T";
const char* password = "qwertasdf";
// const char* ssid = "Galaxy Tab S7 FE0DBD";
// const char* password = "dulangithmi";

const char* mqttServer = "broker.mqtt.cool";
const int mqttPort = 1883;

const char* mqttTopic = "P1T1";
const char* subscribeTopic = "P1T1"; //For Subscription

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(potPin,INPUT);
  pinMode(ldrPin,INPUT);
  WiFi.begin(ssid, password);  //Initialize WiFi Object

  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address : ");
  Serial.print(WiFi.localIP());

  client.setServer(mqttServer, mqttPort);
  // client.setCallback(callback); // For Subscription

  while (!client.connected()) {
    Serial.println("Connecting to MQTT..");
    if (client.connect("ESP8266ClientDulan")) {
      Serial.print("Connected to MQTT");
      // client.subscribe(subscribeTopic); // For Subscription
    } else {
      Serial.println("MQTT Failed to connect");
      delay(2000);
    }
  }
}

String potRange (int value){
  if (value>=70){
    return "High";
  }
  else if (value < 70 && value >= 30 ){
    return "Medium";
  }
  else {
    return("Low");
  }
}

String ldrRange (int value){
  if (value>=20){
    return "High";
  }
  else if (value < 20 && value >= 5 ){
    return "Medium";
  }
  else {
    return("Low");
  }
}

void loop() {
  client.loop(); // For Subscription
  // put your main code here, to run repeatedly:
  int potValue = analogRead(potPin);
  int mappedPot = map(potValue, 0, 4096, 0, 100);
  //Serial.print("Pot Value : ");
  //Serial.println(mappedPot);

  int ldrValue = analogRead(ldrPin);
  int mappedldr = map(ldrValue, 0, 4096, 0, 100);
  Serial.print("LDR Reading: ");
  Serial.println(mappedldr);
  Serial.print("POT Reading: ");
  Serial.println(mappedPot);

  //Converte the Integer Value to a String
  // char valueStr[5];
  // snprintf(valueStr, sizeof(valueStr), "%d", mappedPot);

  // Create a JSON string
  // String jsonPayload = "{\"ldr\": " + String(ldrValue) + ", \"pot\": " + String(potValue) + "}";

  // Json::Value sendMsg;
  // sendMsg["LDR_reading"] = ldrValue;
  // sendMsg["POt_reading"] = potValue;

  // Json::StreamWriterBuilder writer;
  // std::string jsonPayload = Json::writeString(writer, sendMsg);

  DynamicJsonDocument doc(1024);

  // doc["LDR_value"] = ldrRange(mappedldr);
  // doc["POT_value"] = potRange(mappedPot);
  doc["LDR_value"] = mappedldr;
  doc["POT_value"] = mappedPot;

  String jasonString;
  serializeJson(doc, jasonString);
  Serial.println(jasonString);
// This prints:
// {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}

  if (client.connected()) {
    //client.publish(mqttTopic, jsonPayload.c_str());
    client.publish(mqttTopic, jasonString.c_str());
    Serial.println("Published to MQTT: "+ String(out));
    //delay(2000);
    //Serial.println("Published to MQTT: " + String(valueStr));
  }

  delay(5000);
}

// For Subscription
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  char payloadStr[length + 1];
  memcpy(payloadStr, payload, length);
  payloadStr[length] = '\0';

  Serial.print("Payload: ");
  Serial.println(payloadStr);
}
