#ifndef HAMQTT_H
#define HAMQTT_H

#include <Arduino.h>
#include <PubSubClient.h>

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#elif defined(ESP32)
    #include <WiFi.h>
#endif

#include "HAConfig.h"
#include "HAEntity.h"
#include "HAStateCache.h"

typedef void (*HACommandCallback)(const char* topic, const char* payload);

class HAMQTT {
public:
    HAMQTT(Client* networkClient);

    // ═══ الإعداد ═══
    void setServer(const char* server, uint16_t port = 1883,
                   const char* user = nullptr, const char* pass = nullptr);
    void setDevice(const char* id, const char* name,
                   const char* manufacturer = "",
                   const char* model = "",
                   const char* swVersion = "");
    void setStateTopicPrefix(const char* prefix);
    void setAvailabilityTopic(const char* topic);
    void setDiscoveryPrefix(const char* prefix = "homeassistant");

    // ═══ Device Groups ═══
    void setGroup(const char* group);   // ← ✅ جديد
    void clearGroup();                  // ← ✅ جديد

    // ═══ دورة الحياة ═══
    bool begin();
    void loop();
    bool connected();
    void reconnect();

    // ═══ إضافة الكيانات ═══
    bool addSensor(const char* id, const char* name,
                   const char* unit = "",
                   const char* deviceClass = "",
                   const char* icon = "",
                   bool stateClass = false);

    bool addBinarySensor(const char* id, const char* name,
                         const char* deviceClass = "",
                         const char* icon = "");

    bool addButton(const char* id, const char* name,
                   const char* icon = "mdi:gesture-tap-button");

    bool addSwitch(const char* id, const char* name,
                   const char* icon = "mdi:toggle-switch");

    // ✅ readOnly للـ Select
    bool addSelect(const char* id, const char* name,
                   const char* optionsJson,
                   const char* icon = "mdi:format-list-bulleted",
                   bool readOnly = false);

    // ✅ readOnly للـ Number
    bool addNumber(const char* id, const char* name,
                   float minVal, float maxVal, float step,
                   const char* unit = "",
                   const char* icon = "mdi:ray-vertex",
                   bool readOnly = false);

    // ═══ Discovery ═══
    bool publishDiscovery();
    bool publishDiscovery(const char* entityId);

    // ═══ نشر الحالة ═══
    bool publishState(const char* entityId, const char* value,
                      unsigned long heartbeatMs = HA_DEFAULT_HEARTBEAT);
    bool publishState(const char* entityId, float value,
                      uint8_t decimals = 1,
                      unsigned long heartbeatMs = HA_DEFAULT_HEARTBEAT);
    bool publishState(const char* entityId, int value,
                      unsigned long heartbeatMs = HA_DEFAULT_HEARTBEAT);
    bool publishState(const char* entityId, unsigned int value,
                      unsigned long heartbeatMs = HA_DEFAULT_HEARTBEAT);
    bool publishState(const char* entityId, unsigned long value,
                      unsigned long heartbeatMs = HA_DEFAULT_HEARTBEAT);
    bool publishBinaryState(const char* entityId, bool on,
                            unsigned long heartbeatMs = HA_DEFAULT_HEARTBEAT);
    bool publishRaw(const char* topic, const char* value,
                    bool retain = false,
                    unsigned long heartbeatMs = HA_DEFAULT_HEARTBEAT);

    // ═══ Availability ═══
    void publishAvailable();
    void publishUnavailable();

    // ═══ Commands ═══
    void onCommand(HACommandCallback callback);

    // ═══ Cache ═══
    void clearCache() { _cache.clear(); }

    // ═══ Diagnostics ═══
    uint8_t entityCount() const { return _entityCount; }
    PubSubClient& client() { return _mqtt; }

private:
    PubSubClient _mqtt;
    Client* _networkClient;
    HAStateCache _cache;

    const char* _server;
    uint16_t _port;
    const char* _user;
    const char* _pass;

    char _discoveryPrefix[20];
    char _stateTopicPrefix[48];
    char _availabilityTopic[HA_TOPIC_LEN];

    char _deviceId[HA_ID_LEN];
    char _deviceName[HA_NAME_LEN];
    char _manufacturer[HA_ID_LEN];
    char _model[HA_ID_LEN];
    char _swVersion[12];

    char _currentGroup[HA_GROUP_LEN];   // ← ✅ جديد

    HAEntity _entities[HA_MAX_ENTITIES];
    uint8_t _entityCount;

    unsigned long _lastReconnect;
    HACommandCallback _commandCallback;

    HAEntity* _findEntity(const char* id);
    bool _buildStateTopic(char* buf, size_t len, const char* entityId);
    bool _buildCommandTopic(char* buf, size_t len, const char* entityId);
    bool _doPublish(const char* topic, const char* value,
                    bool retain, unsigned long heartbeatMs);
};

#endif