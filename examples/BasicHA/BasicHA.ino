#include <ESP8266WiFi.h>
#include <HAMQTT.h>

// ═══════════════════════════════════════════
//              الإعدادات
// ═══════════════════════════════════════════
#define WIFI_SSID     "YOUR_WIFI"
#define WIFI_PASS     "YOUR_PASS"
#define MQTT_SERVER   "192.168.68.100"
#define MQTT_PORT     1883
#define MQTT_USER     "mqtt_user"
#define MQTT_PASS     "mqtt_pass"

#define DEVICE_ID     "demo_device"
#define DEVICE_NAME   "HAMQTT Demo Device"
#define FW_VERSION    "2.2.0"

WiFiClient wifiClient;
HAMQTT ha(&wifiClient);

// ═══════════════════════════════════════════
//              استقبال الأوامر
// ═══════════════════════════════════════════
void onCommand(const char* topic, const char* payload) {
    Serial.printf("[CMD] %s => %s\n", topic, payload);

    // ═══ معالجة الأوامر ═══
    if (strstr(topic, "restart")) {
        Serial.println("→ Restarting...");
        ESP.restart();
    }
    else if (strstr(topic, "relay")) {
        bool on = (strcmp(payload, "ON") == 0);
        Serial.printf("→ Relay: %s\n", on ? "ON" : "OFF");
        ha.publishBinaryState("relay", on);
    }
    else if (strstr(topic, "mode")) {
        Serial.printf("→ Mode set to: %s\n", payload);
        ha.publishState("mode", payload);
    }
    else if (strstr(topic, "threshold")) {
        Serial.printf("→ Threshold: %s\n", payload);
        ha.publishState("threshold", payload);
    }
    else if (strstr(topic, "check_updates")) {
        Serial.println("→ Checking for updates...");
    }
    else if (strstr(topic, "auto_update")) {
        bool on = (strcmp(payload, "ON") == 0);
        Serial.printf("→ Auto Update: %s\n", on ? "ON" : "OFF");
        ha.publishBinaryState("auto_update", on);
    }
}

// ═══════════════════════════════════════════
//              Setup
// ═══════════════════════════════════════════
void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println(F("═══════════════════════════════════════"));
    Serial.println(F("     HAMQTT v2.2.0 Demo"));
    Serial.println(F("═══════════════════════════════════════"));

    // ═══ قياس الذاكرة ═══
    Serial.printf("Free heap BEFORE:       %u bytes\n", ESP.getFreeHeap());
    Serial.printf("sizeof(HAEntity):       %u bytes\n", sizeof(HAEntity));
    Serial.printf("sizeof(HAStateCache):   %u bytes\n", sizeof(HAStateCache));
    Serial.printf("sizeof(HAMQTT):         %u bytes\n", sizeof(HAMQTT));

    // ═══ اتصال WiFi ═══
    Serial.print(F("Connecting to WiFi"));
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.printf("WiFi connected: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Free heap AFTER WiFi:   %u bytes\n", ESP.getFreeHeap());

    // ═══════════════════════════════════════════
    //              HAMQTT Configuration
    // ═══════════════════════════════════════════
    ha.setServer(MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASS);
    ha.setDevice(DEVICE_ID, DEVICE_NAME,
                 "Kitronic", "HAMQTT-Demo", FW_VERSION);
    ha.setStateTopicPrefix("demo/device");
    ha.setAvailabilityTopic("demo/device/status");

    // ═══════════════════════════════════════════
    //  GROUP 1: Sensors (القراءات)
    // ═══════════════════════════════════════════
    ha.setGroup("Sensors");

    ha.addSensor("temperature", "Temperature",
                 "°C", "temperature", "mdi:thermometer", true);

    ha.addSensor("humidity", "Humidity",
                 "%", "humidity", "mdi:water-percent", true);

    ha.addSensor("battery_voltage", "Battery Voltage",
                 "V", "voltage", "mdi:battery", true);

    ha.addSensor("battery_percent", "Battery Level",
                 "%", "battery", "mdi:battery-70", true);

    ha.addSensor("power_usage", "Power Usage",
                 "W", "power", "mdi:flash", true);

    ha.addSensor("signal_strength", "WiFi Signal",
                 "dBm", "signal_strength", "mdi:wifi");

    // ═══════════════════════════════════════════
    //  GROUP 2: Controls (التحكم)
    // ═══════════════════════════════════════════
    ha.setGroup("Controls");

    ha.addButton("restart", "Restart Device",
                 "mdi:restart");

    ha.addButton("check_updates", "Check Updates",
                 "mdi:cloud-download");

    ha.addSwitch("relay", "Main Relay",
                 "mdi:power");

    ha.addSwitch("auto_update", "Auto Update",
                 "mdi:update");

    ha.addSelect("mode", "Operation Mode",
                 "[\"Auto\",\"Manual\",\"Eco\",\"Performance\"]",
                 "mdi:cog");

    ha.addNumber("threshold", "Threshold",
                 0.0, 100.0, 1.0, "%", "mdi:tune");

    // ═══════════════════════════════════════════
    //  GROUP 3: Diagnostics (التشخيص)
    // ═══════════════════════════════════════════
    ha.setGroup("Diagnostics");

    ha.addSensor("wifi_ip", "WiFi IP Address");
    ha.addSensor("firmware_version", "Firmware Version");
    ha.addSensor("uptime", "Uptime", "s", "duration", "mdi:clock");
    ha.addSensor("free_heap", "Free Heap", "B", "data_size", "mdi:memory");

    ha.addBinarySensor("wifi_connected", "WiFi Connected", "connectivity");
    ha.addBinarySensor("mqtt_connected", "MQTT Connected", "connectivity");

    // ═══════════════════════════════════════════
    //  GROUP 4: Read-Only Display (للعرض فقط)
    // ═══════════════════════════════════════════
    ha.setGroup("Display");

    // ═══ Select للعرض فقط ═══
    ha.addSelect("device_status", "Device Status",
                 "[\"Booting\",\"Running\",\"Idle\",\"Error\"]",
                 "mdi:eye",
                 true);   // ← ✅ readOnly

    // ═══ Number للعرض فقط ═══
    ha.addNumber("current_threshold_display", "Current Threshold",
                 0.0, 100.0, 1.0, "%",
                 "mdi:eye",
                 true);   // ← ✅ readOnly

    // ═══ رجوع للوضع العادي ═══
    ha.clearGroup();

    // ═══ معلومات ═══
    Serial.printf("Entities registered:    %u\n", ha.entityCount());
    Serial.printf("Free heap AFTER setup:  %u bytes\n", ESP.getFreeHeap());

    // ═══ تسجيل callback ═══
    ha.onCommand(onCommand);

    // ═══════════════════════════════════════════
    //              الاتصال بـ MQTT
    // ═══════════════════════════════════════════
    if (ha.begin()) {
        Serial.println(F("MQTT Connected ✓"));
        delay(500);

        Serial.print(F("Publishing Discovery..."));
        if (ha.publishDiscovery()) {
            Serial.println(F(" ✓"));
        } else {
            Serial.println(F(" ✗ (some entities failed)"));
        }
    } else {
        Serial.println(F("MQTT Connection FAILED ✗"));
    }

    Serial.printf("Free heap AFTER all:    %u bytes\n", ESP.getFreeHeap());
    Serial.printf("Max free block:         %u bytes\n", ESP.getMaxFreeBlockSize());
    Serial.println(F("═══════════════════════════════════════"));
    Serial.println();
}

// ═══════════════════════════════════════════
//              Loop
// ═══════════════════════════════════════════
void loop() {
    ha.loop();

    // ═══ نشر القراءات كل 5 ثوان ═══
    static unsigned long lastPublish = 0;
    if (millis() - lastPublish > 5000) {
        lastPublish = millis();

        // ═══ Sensors ═══
        ha.publishState("temperature", 22.5f + random(-20, 20) / 10.0f, 1);
        ha.publishState("humidity", 45 + random(-5, 5));
        ha.publishState("battery_voltage", 12.6f + random(-5, 5) / 100.0f, 2);
        ha.publishState("battery_percent", 75 + random(-3, 3));
        ha.publishState("power_usage", 850 + random(-50, 50));
        ha.publishState("signal_strength", WiFi.RSSI());

        // ═══ Diagnostics ═══
        IPAddress ip = WiFi.localIP();
        char ipBuf[16];
        snprintf(ipBuf, sizeof(ipBuf), "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
        ha.publishState("wifi_ip", ipBuf);
        ha.publishState("firmware_version", FW_VERSION);
        ha.publishState("uptime", (unsigned long)(millis() / 1000));
        ha.publishState("free_heap", ESP.getFreeHeap());

        ha.publishBinaryState("wifi_connected", WiFi.status() == WL_CONNECTED);
        ha.publishBinaryState("mqtt_connected", ha.connected());

        // ═══ Read-Only Display ═══
        // القيمة تتحدّث تلقائياً لكن المستخدم ما يقدر يغيّرها
        ha.publishState("device_status", "Running");
        ha.publishState("current_threshold_display", 50.0f, 1);

        // ═══ Debug ═══
        static unsigned long counter = 0;
        if (++counter % 12 == 0) {
            Serial.printf("[MEM] Heap: %u | MaxBlk: %u | Entities: %u\n",
                          ESP.getFreeHeap(),
                          ESP.getMaxFreeBlockSize(),
                          ha.entityCount());
        }
    }

    // ═══ محاكاة أحداث عشوائية ═══
    static unsigned long lastEvent = 0;
    if (millis() - lastEvent > 30000) {
        lastEvent = millis();
        // كل 30 ثانية، بدّل الـ relay
        static bool relayState = false;
        relayState = !relayState;
        ha.publishBinaryState("relay", relayState);
    }
}