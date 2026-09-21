#include <HAMQTT.h>
#include "config.h"

WifiManager wifi;
MQTTClient mqtt;

// Simple temperature sensor example (replace with real sensor code)
class TempSensor : public Sensor {
public:
    const char* name() const override { return "ESP32 Temperature"; }
    const char* stateTopic() const override { return "/sensors/temp/01/state"; }
    float readValue() override {
        // Dummy value – replace with actual sensor reading
        return 25.3;
    }
};

TempSensor temp;

void setup() {
    Serial.begin(115200);
    wifi.begin(WIFI_SSID, WIFI_PASSWORD);
    mqtt.setBroker(MQTT_BROKER, MQTT_PORT, MQTT_USER, MQTT_PASS);
    mqtt.begin();

    // Publish Home Assistant discovery payload once on connect
    StaticJsonDocument<256> doc;
    doc["name"] = temp.name();
    doc["state_topic"] = temp.stateTopic();
    doc["unit_of_measurement"] = "°C";
    String payload; serializeJson(doc, payload);

    mqtt.publish("homeassistant/sensor/esp32_temp/config", payload.c_str(), true);
}

void loop() {
    wifi.loop();      // Wi‑Fi reconnection handling
    mqtt.loop();      // MQTT keep‑alive & reconnect
    temp.publish(mqtt);  // publish sensor value
    delay(10000);       // adjust interval as needed
}