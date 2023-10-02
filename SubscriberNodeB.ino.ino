#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "Dulini";
const char* password = "dulini123";
const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_topic = "enigma";

WiFiClient espClient;
PubSubClient client(espClient);

int redLED = 32;
int orangeLED = 33;
int greenLED = 34;
const char* potValue = "Low";  // Initialize with a default value
const char* ldrValue = "Low";  // Initialize with a default value

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.print("Connecting to MQTT..");
    if (client.connect("ESP8266ClientNuthyaa")) {
      Serial.print("Connected to MQTT");
      client.subscribe("enigma");
    } else {
      Serial.print("MQTT Failed to connect");
      delay(5000);
    }
  }

  pinMode(redLED, OUTPUT);
  pinMode(orangeLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
}

void turnOffAllLEDs() {
  digitalWrite(redLED, LOW);
  digitalWrite(orangeLED, LOW);
  digitalWrite(greenLED, LOW);
}

void blink(int r, int o, int g) {
  digitalWrite(r, HIGH);
  delay(1000);
  digitalWrite(r, LOW);
  delay(1000);

  digitalWrite(o, HIGH);
  delay(1000);
  digitalWrite(o, LOW);
  delay(1000);

  digitalWrite(g, HIGH);
  delay(1000);
  digitalWrite(g, LOW);
  delay(1000);
}


void reconnect() {
  while (!client.connected()) {
    if (client.connect("Node_B")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  char payloadStr[length + 1];
  memcpy(payloadStr, payload, length);
  payloadStr[length] = '\0';

  StaticJsonDocument<32> doc;
  DeserializationError error = deserializeJson(doc, payloadStr);

  if (error) {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  if (doc.containsKey("potValue") && doc.containsKey("ldrValue")) {
    potValue = doc["potValue"];
    ldrValue = doc["ldrValue"];
    Serial.print("Pot Value: ");
    Serial.println(potValue);
    Serial.print("LDR Value: ");
    Serial.println(ldrValue);
  } else {
    Serial.println("JSON object is missing keys.");
  }
}

void loop() {
  client.loop();

  if (strcmp(potValue, "High") == 0 && strcmp(ldrValue, "High") == 0) {
    // Turn on the red LED
    digitalWrite(redLED, HIGH);
    digitalWrite(orangeLED, HIGH);
    digitalWrite(greenLED, HIGH);
  } else if (strcmp(potValue, "Medium") == 0 && strcmp(ldrValue, "High") == 0) {
    // Turn on the orange LED
    digitalWrite(redLED, HIGH);
    digitalWrite(orangeLED, HIGH);
    digitalWrite(greenLED, HIGH);
  } else if (strcmp(potValue, "High") == 0 && strcmp(ldrValue, "Medium") == 0) {
    // Turn on the green LED
    digitalWrite(redLED, HIGH);
    digitalWrite(orangeLED, HIGH);
    digitalWrite(greenLED, HIGH);
  } else if (strcmp(potValue, "Low") == 0 && strcmp(ldrValue, "High") == 0) {
    digitalWrite(redLED, LOW);
    digitalWrite(orangeLED, HIGH);
    digitalWrite(greenLED, HIGH);
  } else if (strcmp(potValue, "High") == 0 && strcmp(ldrValue, "Low") == 0) {
    digitalWrite(redLED, LOW);
    digitalWrite(orangeLED, HIGH);
    digitalWrite(greenLED, HIGH);
  } else if (strcmp(potValue, "Medium") == 0 && strcmp(ldrValue, "Medium") == 0) {
    digitalWrite(redLED, LOW);
    digitalWrite(orangeLED, HIGH);
    digitalWrite(greenLED, HIGH);
  } else if (strcmp(potValue, "Medium") == 0 && strcmp(ldrValue, "Low") == 0) {
    blink(redLED, orangeLED, greenLED);

  } else if (strcmp(potValue, "Low") == 0 && strcmp(ldrValue, "Medium") == 0) {
    blink(redLED, orangeLED, greenLED);

  } else {
    blink(redLED, orangeLED, greenLED);
  }
  }
  