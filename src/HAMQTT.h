#ifndef HAMQTT_H
#define HAMQTT_H

#include <Arduino.h>
#include <PubSubClient.h>

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#elif defined(ESP32)
    #include <WiFi.h>
#endif

#include "HAEntity.h"

typedef void (*HACommandCallback)(const char* topic, const char* payload);

class HAMQTT {
public:
    HAMQTT(Client* networkClient);

    // ═══════ الإعداد ═══════
    void setServer(const char* server, uint16_t port = 1883,
                   const char* user = nullptr, const char* pass = nullptr);
    void setDiscoveryPrefix(const char* prefix = "homeassistant");
    void setStateTopicPrefix(const char* prefix);
    void setAvailabilityTopic(const char* topic);
    void setDevice(const char* id, const char* name,
                   const char* manufacturer = "",
                   const char* model = "",
                   const char* swVersion = "");

    // ═══════ دورة الحياة ═══════
    bool begin();
    void loop();
    bool connected();
    void reconnect();

    // ═══════ إضافة الكيانات ═══════
    bool addSensor(const char* id, const char* name,
                   const char* unit = "",
                   const char* deviceClass = "",
                   const char* icon = "",
                   bool retained = true);

    bool addBinarySensor(const char* id, const char* name,
                         const char* deviceClass = "",
                         const char* icon = "");

    bool addButton(const char* id, const char* name,
                   const char* icon = "mdi:gesture-tap-button");

    bool addSwitch(const char* id, const char* name,
                   const char* icon = "mdi:toggle-switch");

    // ═══════ نشر Discovery ═══════
    bool publishDiscovery();                          // كل الكيانات
    bool publishDiscovery(const char* entityId);      // كيان واحد

    // ═══════ نشر الحالة ═══════
    bool publishState(const char* entityId, const char* value);
    bool publishState(const char* entityId, float value, uint8_t decimals = 1);
    bool publishState(const char* entityId, int value);
    bool publishBinaryState(const char* entityId, bool on);
    bool publishAttribute(const char* entityId, const char* attr, const char* value);

    // ═══════ التوفر ═══════
    void publishAvailable();
    void publishUnavailable();

    // ═══════ الأوامر ═══════
    void onCommand(HACommandCallback callback);

    // ═══════ الوصول للـ client الأصلي ═══════
    PubSubClient& client() { return _mqtt; }

private:
    PubSubClient _mqtt;
    Client* _networkClient;

    const char* _server;
    uint16_t _port;
    const char* _user;
    const char* _pass;

    char _discoveryPrefix[24];
    char _stateTopicPrefix[64];
    char _availabilityTopic[HA_TOPIC_LEN];

    char _deviceId[HA_ID_LEN];
    char _deviceName[HA_NAME_LEN];
    char _manufacturer[HA_ID_LEN];
    char _model[HA_ID_LEN];
    char _swVersion[16];

    HAEntity _entities[HA_MAX_ENTITIES];
    uint8_t _entityCount;

    unsigned long _lastReconnect;
    HACommandCallback _commandCallback;

    // ═══ Helpers داخلية ═══
    HAEntity* _findEntity(const char* id);
    bool _buildStateTopic(char* buf, size_t len, const char* entityId);
    bool _buildCommandTopic(char* buf, size_t len, const char* entityId);
    bool _buildDiscoveryTopic(char* buf, size_t len,
                              const char* component, const char* entityId);
    const char* _typeToComponent(HAEntityType type);
};

#endif