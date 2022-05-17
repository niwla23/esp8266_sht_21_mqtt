#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SHT2x.h>
#include <Wire.h>

#include "secrets.hpp"

// Update these with values suitable for your network.

WiFiClient espClient;
PubSubClient client(espClient);
SHT2x sht;

void setup_wifi() {
    delay(20);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), stateTopic, 1, true, "0")) {
            Serial.println("connected");
            client.publish(stateTopic, "1", true);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    sht.begin();
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }

    sht.read();

    String temperature = String(sht.getTemperature(), 2);
    String humidity = String(sht.getHumidity(), 2);

    client.publish(temperatureTopic, temperature.c_str());
    client.publish(humidityTopic, humidity.c_str());
    client.publish(stateTopic, "1", true);

    delay(5000);
}
