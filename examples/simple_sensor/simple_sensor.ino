// ═══════════════════════════════════════════
//   HAMQTT v2.2.1 — Simple Sensor (cross-platform)
//   يعمل على ESP8266 و ESP32
// ═══════════════════════════════════════════

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include <HAMQTT.h>

// ═══════════════════════════════════════════
//              الإعدادات
// ═══════════════════════════════════════════
#define WIFI_SSID "YOUR_WIFI"
#define WIFI_PASS "YOUR_PASS"
#define MQTT_SERVER "192.168.1.100"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASS ""

#define DEVICE_ID "esp_temp"
#define DEVICE_NAME "ESP Temperature"
#define FW_VERSION "2.2.1"

#define STATE_PREFIX "sensors/temp01"
#define AVAIL_TOPIC "sensors/temp01/status"

// ═══════════════════════════════════════════
WiFiClient wifiClient;
HAMQTT ha(&wifiClient);

// ═══════════════════════════════════════════
void setup()
{
    Serial.begin(115200);
    delay(200);

    Serial.println(F("\n[HAMQTT] Simple Sensor Example"));

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print(F("Connecting to WiFi"));
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print('.');
    }
    Serial.println();
    Serial.printf("Connected: %s\n", WiFi.localIP().toString().c_str());

    ha.setServer(MQTT_SERVER, MQTT_PORT,
                 (MQTT_USER[0] ? MQTT_USER : nullptr),
                 (MQTT_PASS[0] ? MQTT_PASS : nullptr));

    ha.setDevice(DEVICE_ID, DEVICE_NAME,
                 "Kitronic", "ESP-Temp", FW_VERSION);

    ha.setStateTopicPrefix(STATE_PREFIX);
    ha.setAvailabilityTopic(AVAIL_TOPIC);

    ha.addSensor("temperature", "Temperature",
                 "°C", "temperature", "mdi:thermometer", true);

    if (ha.begin())
    {
        Serial.println(F("[HAMQTT] MQTT connected ✓"));
        ha.publishDiscovery();
    }
    else
    {
        Serial.println(F("[HAMQTT] MQTT connection FAILED ✗"));
    }
}

void loop()
{
    ha.loop();

    static unsigned long last = 0;
    if (millis() - last > 10000)
    {
        last = millis();

        // استبدل هذا بقراءة الحساس الحقيقي
        float temperature = 25.3f;

        ha.publishState("temperature", temperature, 1);
        Serial.printf("[HAMQTT] temp = %.1f °C\n", temperature);
    }
}