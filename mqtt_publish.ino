/*
 * MQTT and AskSensors IoT Platform
 * Description: ESP32 publishes PIR motion data to AskSensors over MQTT
 *  Author: https://asksensors.com, 2019
 *  github: https://github.com/asksensors
 */
 
#include <WiFi.h>
#include <PubSubClient.h>

//TODO: ESP32 MQTT user config
const char* ssid = ".................."; // Wifi SSID
const char* password = ".................."; // Wifi Password
const char* username = "................."; // my AskSensors username
const char* pubTopic = "................../.................."; // username/apiKeyIn
const unsigned int writeInterval = 25000;   // write interval (in ms)
//AskSensors MQTT config
const char* mqtt_server = "mqtt.asksensors.com";
unsigned int mqtt_port = 1883;
int PIR_data = 22;

WiFiClient askClient;
PubSubClient client(askClient);

void setup() {
  Serial.begin(115200);
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : ESP32 publishes PIR Motion data to AskSensors over MQTT");
  Serial.print("********** connecting to WIFI : ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("->WiFi connected");
  Serial.println("->IP address: ");
  Serial.println(WiFi.localIP());
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  pinMode(PIR_data,INPUT); // pir data as input
  
}

void loop() {

  if (!client.connected()) 
    reconnect();
  client.loop();
  
  bool PIR_status = digitalRead(PIR_data);
 
  if(PIR_status){
    Serial.println("PIR motion detected");
  } else {
    Serial.println("NO PIR motion detected"); 
  }
  
  Serial.println("********** Publish MQTT data to ASKSENSORS");
  char mqtt_payload[30] = "";
  snprintf (mqtt_payload, 30, "m1=%ld", PIR_status);
  Serial.print("Publish message: ");
  Serial.println(mqtt_payload);
  client.publish(pubTopic, mqtt_payload);
  Serial.println("> MQTT data published");
  Serial.println("********** End ");
  Serial.println("*****************************************************");
  
 delay(writeInterval);// delay
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("********** Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", username, "")) {  
      Serial.println("-> MQTT client connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("-> try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
