#include <WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"
#include "soc/rtc.h"

#define BUTTON_PIN 21 

const char* ssid = "Gor Cisitu 552.4G";
const char* password = "Badminton55";
const char* mqtt_server = "127.0.0.1";
const int mqtt_port = 1883;
const char* mqtt_topic = "/node1/data1";
const float calibration_factor = 166406.0/186.0;

int lastState = HIGH;
int currentState;

WiFiClient espClient;
PubSubClient client(espClient);

HX711 scale;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  scale.begin(16,4);
  scale.set_scale(calibration_factor);
  scale.tare();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  currentState = digitalRead(BUTTON_PIN);
  float weight = scale.get_units();
  if(lastState == LOW && currentState == HIGH){
    client.publish(mqtt_topic, String(weight).c_str());
  }
  lastState = currentState;
}