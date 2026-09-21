#include <ESP8266WiFi.h>
#include <HAMQTT.h>

#define WIFI_SSID     "YOUR_WIFI"
#define WIFI_PASS     "YOUR_PASS"
#define MQTT_SERVER   "192.168.68.100"
#define MQTT_USER     "mqtt_user"
#define MQTT_PASS     "mqtt_pass"

WiFiClient wifiClient;
HAMQTT ha(&wifiClient);

void onCommand(const char* topic, const char* payload) {
    Serial.printf("CMD: %s => %s\n", topic, payload);
}

void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.printf("\n=== HAMQTT v2.1 Lightweight ===\n");
    Serial.printf("Free heap BEFORE: %u\n", ESP.getFreeHeap());
    Serial.printf("sizeof(HAEntity): %u bytes\n", sizeof(HAEntity));
    Serial.printf("sizeof(HAStateCache): %u bytes\n", sizeof(HAStateCache));
    Serial.printf("sizeof(HAMQTT): %u bytes\n", sizeof(HAMQTT));

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.printf("WiFi: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Free heap AFTER WiFi: %u\n", ESP.getFreeHeap());

    // ═══ Setup ═══
    ha.setServer(MQTT_SERVER, 1883, MQTT_USER, MQTT_PASS);
    ha.setDevice("pip_inverter_03", "PIP Solar Inverter 3",
                 "Voltronic", "Axpert/PIP/MAX2", "2.1.0");
    ha.setStateTopicPrefix("solar/inverter3");
    ha.setAvailabilityTopic("solar/inverter3/status");

    // ═══ Sensors (stateClass=true لـ statistics) ═══
    ha.addSensor("battery_voltage", "Battery Voltage", "V", "voltage", "mdi:battery", true);
    ha.addSensor("battery_percent", "Battery %", "%", "battery", "mdi:battery-70", true);
    ha.addSensor("load_power", "Load Power", "W", "power", "mdi:flash", true);
    ha.addSensor("solar_power", "Solar Power", "W", "power", "mdi:solar-power", true);
    ha.addSensor("grid_voltage", "Grid Voltage", "V", "voltage", "mdi:transmission-tower", true);
    ha.addSensor("temperature", "Inverter Temp", "°C", "temperature", "mdi:thermometer", true);

    // ═══ Binary Sensors ═══
    ha.addBinarySensor("grid_online", "Grid Online", "plug");
    ha.addBinarySensor("battery_low", "Battery Low", "battery");

    // ═══ Button ═══
    ha.addButton("check_updates", "Check Updates", "mdi:cloud-download");

    // ═══ Switch ═══
    ha.addSwitch("auto_update", "Auto Update", "mdi:update");

    // ═══ Select (options في flash) ═══
    ha.addSelect("inverter_mode", "Inverter Mode",
                 "[\"Power On\",\"Standby\",\"Line\",\"Battery\"]",
                 "mdi:cog");

    // ═══ Number ═══
    ha.addNumber("battery_cutoff", "Cut-off Voltage",
                 40.0, 54.0, 0.1, "V", "mdi:battery-minus");

    Serial.printf("Entities: %u\n", ha.entityCount());
    Serial.printf("Free heap AFTER setup: %u\n", ESP.getFreeHeap());

    // ═══ Command callback ═══
    ha.onCommand(onCommand);

    if (ha.begin()) {
        Serial.println("MQTT OK");
        delay(500);
        ha.publishDiscovery();
        Serial.println("Discovery published");
    }

    Serial.printf("Free heap AFTER all: %u\n", ESP.getFreeHeap());
    Serial.printf("Max free block: %u\n", ESP.getMaxFreeBlockSize());
}

void loop() {
    ha.loop();

    static unsigned long last = 0;
    if (millis() - last > 5000) {
        last = millis();

        ha.publishState("battery_voltage", 48.5f, 2);
        ha.publishState("battery_percent", 87);
        ha.publishState("load_power", 1250);
        ha.publishState("solar_power", 2100);
        ha.publishState("grid_voltage", 230.1f, 1);
        ha.publishState("temperature", 42.3f, 1);

        ha.publishBinaryState("grid_online", true);
        ha.publishBinaryState("battery_low", false);

        // القيم الثابتة - cache يمنع التكرار
        ha.publishState("battery_cutoff", 44.0f, 1);
    }
}