#ifndef CONFIG_H
#define CONFIG_H

// ═══════════════════════════════════════════
//              Wi-Fi
// ═══════════════════════════════════════════
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASS"

// ═══════════════════════════════════════════
//              MQTT Broker
// ═══════════════════════════════════════════
#define MQTT_BROKER "mqtt.example.com"
#define MQTT_PORT 1883
#define MQTT_USER "user" // اتركها فاضية "" إذا مو مطلوب
#define MQTT_PASS "pass"

// ═══════════════════════════════════════════
//              Device Identity
// ═══════════════════════════════════════════
#define DEVICE_ID "my_device"
#define DEVICE_NAME "My Device"
#define DEVICE_MANUF "Kitronic"
#define DEVICE_MODEL "ESP8266"
#define FW_VERSION "2.2.1"

// ═══════════════════════════════════════════
//              Topics
// ═══════════════════════════════════════════
#define STATE_PREFIX "home/my_device"
#define AVAIL_TOPIC "home/my_device/status"

#endif // CONFIG_H