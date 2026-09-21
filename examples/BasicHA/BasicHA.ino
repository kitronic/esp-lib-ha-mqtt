#include <ESP8266WiFi.h>
#include <HAMQTT.h>

// ═══ الإعدادات ═══
#define WIFI_SSID     "YOUR_WIFI"
#define WIFI_PASS     "YOUR_PASS"
#define MQTT_SERVER   "192.168.68.100"
#define MQTT_PORT     1883
#define MQTT_USER     "mqtt_user"
#define MQTT_PASS     "mqtt_pass"

WiFiClient wifiClient;
HAMQTT ha(&wifiClient);

// ═══ استقبال الأوامر ═══
void onCommandReceived(const char* topic, const char* payload) {
    Serial.printf("📥 Command: %s => %s\n", topic, payload);
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n\n=== HAMQTT Basic Example ===");

    // ═══ WiFi ═══ 
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\n✅ WiFi OK - IP: %s\n", WiFi.localIP().toString().c_str());

    // ═══ HAMQTT Setup ═══
    ha.setServer(MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASS);
    ha.setDevice("pip_inverter_03", "PIP Solar Inverter 3",
                 "Voltronic", "Axpert/PIP/MAX2", "1.0.0");
    ha.setStateTopicPrefix("solar/inverter3");
    ha.setAvailabilityTopic("solar/inverter3/status");

    // ═══ إضافة الكيانات ═══
    ha.addSensor("battery_voltage", "Battery Voltage", "V", "voltage", "mdi:battery");
    ha.addSensor("battery_percent", "Battery Level", "%", "battery", "mdi:battery-70");
    ha.addSensor("load_power", "Load Power", "W", "power", "mdi:flash");
    ha.addSensor("solar_power", "Solar Power", "W", "power", "mdi:solar-power");
    ha.addSensor("grid_voltage", "Grid Voltage", "V", "voltage", "mdi:transmission-tower");
    ha.addSensor("temperature", "Inverter Temperature", "°C", "temperature", "mdi:thermometer");

    ha.addBinarySensor("grid_online", "Grid Online", "plug");
    ha.addBinarySensor("battery_low", "Battery Low", "battery");

    ha.addButton("check_updates", "Check Updates", "mdi:cloud-download");
    ha.addSwitch("charger_enable", "Charger Enable", "mdi:power-plug");

    // ═══ Callback ═══
    ha.onCommand(onCommandReceived);

    // ═══ بدء الاتصال ═══
    if (ha.begin()) {
        Serial.println("✅ MQTT Connected");
        delay(1000);
        ha.publishDiscovery();
        Serial.println("✅ Discovery published");
    } else {
        Serial.println("❌ MQTT Connection failed");
    }
}

void loop() {
    ha.loop();

    // ═══ نشر الحالة كل 5 ثوان ═══
    static unsigned long lastPub = 0;
    if (millis() - lastPub > 5000) {
        lastPub = millis();

        if (ha.connected()) {
            ha.publishState("battery_voltage", 48.5f, 2);
            ha.publishState("battery_percent", 87);
            ha.publishState("load_power", 1250);
            ha.publishState("solar_power", 2100);
            ha.publishState("grid_voltage", 230.1f, 1);
            ha.publishState("temperature", 42.3f, 1);
ha.pu
            ha.publishBinaryState("grid_online", true);
            ha.publishBinaryState("battery_low", false);
        }
    }
}