// ═══════════════════════════════════════════
//   مثال بسيط: Sensor واحد لدرجة الحرارة
// ═══════════════════════════════════════════
#include <ESP8266WiFi.h>
#include <HAMQTT.h>
#include "config.h"

WiFiClient wifiClient;
HAMQTT ha(&wifiClient);

void setup()
{
    Serial.begin(115200);
    delay(200);

    Serial.println(F("\n[HAMQTT] Simple Sensor Example"));

    // ═══ WiFi ═══
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(F("Connecting to WiFi"));
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print('.');
    }
    Serial.printf("\nConnected: %s\n", WiFi.localIP().toString().c_str());

    // ═══ HAMQTT ═══
    ha.setServer(MQTT_BROKER, MQTT_PORT,
                 (MQTT_USER[0] ? MQTT_USER : nullptr),
                 (MQTT_PASS[0] ? MQTT_PASS : nullptr));

    ha.setDevice(DEVICE_ID, DEVICE_NAME,
                 DEVICE_MANUF, DEVICE_MODEL, FW_VERSION);

    ha.setStateTopicPrefix(STATE_PREFIX);
    ha.setAvailabilityTopic(AVAIL_TOPIC);

    // ═══ إضافة sensor واحد ═══
    ha.addSensor("temperature", "Temperature",
                 "°C", "temperature", "mdi:thermometer", true);

    // ═══ بدء الاتصال ونشر الـ discovery ═══
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

        // ═══ استبدل هذا بقراءة الحساس الحقيقي ═══
        float temperature = 25.3f;

        ha.publishState("temperature", temperature, 1);
        Serial.printf("[HAMQTT] temp = %.1f °C\n", temperature);
    }
}