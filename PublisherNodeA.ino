#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

char out[128];
const int potPin = 33;
int potValue = 0;
const int ldrPin = 34;
int ldrValue = 0;

const char* ssid = "AndroidAPA4DD";
const char* password = "nuths11011";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

const char* mqttTopic = "EnigmaTaskA";
const char* subscribeTopic = "yourname/ledValue";

WiFiClient espClient;
PubSubClient client(espClient); //tell pubsub lib use this wifi to make pub sub

void setup() {
  // put your setup code here, to run once:
  
  pinMode(potPin,INPUT);
  Serial.begin(115200);

  WiFi.begin(ssid,password); //initialize WiFi object

  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED){  //make sure wifi connection is establish, if not until connection establishes loop executes
    delay (1000);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());     //gives the ip assgined to the device from the wifi network

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()){  //make sure mqtt connection is establish, if not until connection establishes loop executes
    Serial.print("Connecting to MQTT..");
    if (client.connect("ESP8266ClientNuthya")){   //try to make unique since connecting to a global broker
      Serial.print("Connected to MQTT");
      client.subscribe(subscribeTopic); //for subscription
    }else{
      Serial.print("MQTT Failed to connect");
      delay(5000);
    };
  }
}

void loop() {
  client.loop(); //everytime checks for the subscription

  StaticJsonDocument<256> doc;
  int potValue=analogRead(potPin);
  int ldrValue=analogRead(ldrPin);
 
  doc["PotValue"] = potValue;
  doc["LDRValue"] = ldrValue;  
  doc["PotRange"] = range(potValue);
  doc["LDRRange"] = range(ldrValue);
  serializeJson(doc, out);

  // convert integer port value to string, all data should be string in mqtt
  char valueStr[5];
  snprintf(valueStr, sizeof(valueStr),"%d", potValue);

  if(client.connected()){
    client.publish(mqttTopic, out); //publish the value(string) valueStr to the topic mqttTopic
    Serial.println("Published to MQTT: "+ String(out));
    delay(5000);
  }

  Serial.print("Pot Value: ");
  Serial.println(potValue);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

}


//to subscribe callback
void callback(char* topic,byte* payload, unsigned int length){
  Serial.print("Message received from topic: ");
  Serial.print(topic);

  //data come as a byte
  char payloadStr[length+1];
  memcpy(payloadStr, payload, length); 
  payloadStr[length] = '\0'; //make sure string is properly completed

}

String range (int Value){
  if (Value>=2730){
    return "High";
  }
  else if (Value < 2730 && Value >= 1365 ){
    return "Medium";
  }
  else {
    return "Low" ;
  }
}

