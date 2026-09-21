#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi credentials
const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASS";

// MQTT broker details
const char* MQTT_BROKER = "mqtt.example.com";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_USER = "user"; // optional
const char* MQTT_PASS = "pass"; // optional

#endif // CONFIG_H