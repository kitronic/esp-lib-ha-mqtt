#include "HAMQTT.h"
#include "HADiscovery.h"
#include <string.h>
#include <stdio.h>

// ═══════════════════════════════════════════
HAMQTT::HAMQTT(Client *networkClient)
    : _mqtt(*networkClient), _networkClient(networkClient)
{
    _server = nullptr;
    _port = 1883;
    _user = nullptr;
    _pass = nullptr;

    strcpy(_discoveryPrefix, "homeassistant");
    _stateTopicPrefix[0] = '\0';
    _availabilityTopic[0] = '\0';
    _deviceId[0] = '\0';
    _deviceName[0] = '\0';
    _manufacturer[0] = '\0';
    _model[0] = '\0';
    _swVersion[0] = '\0';
    _currentGroup[0] = '\0';   // ← ✅ جديد

    _entityCount = 0;
    _lastReconnect = 0;
    _commandCallback = nullptr;

    memset(_entities, 0, sizeof(_entities));
}

// ═══════════════════════════════════════════
//              Configuration
// ═══════════════════════════════════════════
void HAMQTT::setServer(const char *server, uint16_t port,
                       const char *user, const char *pass)
{
    _server = server;
    _port = port;
    _user = user;
    _pass = pass;
}

void HAMQTT::setDevice(const char *id, const char *name,
                       const char *manufacturer, const char *model,
                       const char *swVersion)
{
    strncpy(_deviceId, id, HA_ID_LEN - 1);
    _deviceId[HA_ID_LEN - 1] = '\0';
    strncpy(_deviceName, name, HA_NAME_LEN - 1);
    _deviceName[HA_NAME_LEN - 1] = '\0';
    strncpy(_manufacturer, manufacturer, HA_ID_LEN - 1);
    _manufacturer[HA_ID_LEN - 1] = '\0';
    strncpy(_model, model, HA_ID_LEN - 1);
    _model[HA_ID_LEN - 1] = '\0';
    strncpy(_swVersion, swVersion, 11);
    _swVersion[11] = '\0';
}

void HAMQTT::setStateTopicPrefix(const char *prefix)
{
    strncpy(_stateTopicPrefix, prefix, sizeof(_stateTopicPrefix) - 1);
    _stateTopicPrefix[sizeof(_stateTopicPrefix) - 1] = '\0';
}

void HAMQTT::setAvailabilityTopic(const char *topic)
{
    strncpy(_availabilityTopic, topic, HA_TOPIC_LEN - 1);
    _availabilityTopic[HA_TOPIC_LEN - 1] = '\0';
}

void HAMQTT::setDiscoveryPrefix(const char *prefix)
{
    strncpy(_discoveryPrefix, prefix, sizeof(_discoveryPrefix) - 1);
    _discoveryPrefix[sizeof(_discoveryPrefix) - 1] = '\0';
}

// ═══════════════════════════════════════════
//              Device Groups
// ═══════════════════════════════════════════
void HAMQTT::setGroup(const char *group)   // ← ✅ جديد
{
    if (group == nullptr) {
        _currentGroup[0] = '\0';
        return;
    }
    strncpy(_currentGroup, group, HA_GROUP_LEN - 1);
    _currentGroup[HA_GROUP_LEN - 1] = '\0';
}

void HAMQTT::clearGroup()                  // ← ✅ جديد
{
    _currentGroup[0] = '\0';
}

// ═══════════════════════════════════════════
//              Lifecycle
// ═══════════════════════════════════════════
bool HAMQTT::begin()
{
    if (!_server || !_server[0])
        return false;

    _mqtt.setServer(_server, _port);
    _mqtt.setBufferSize(HA_MQTT_BUFFER_SIZE);
    _mqtt.setKeepAlive(60);
    reconnect();
    return _mqtt.connected();
}

void HAMQTT::loop()
{
    if (!_mqtt.connected())
    {
        unsigned long now = millis();
        if (now - _lastReconnect > 5000)
        {
            _lastReconnect = now;
            reconnect();
        }
        return;
    }
    _mqtt.loop();
}

bool HAMQTT::connected() { return _mqtt.connected(); }

void HAMQTT::reconnect()
{
    if (_mqtt.connected()) return;

    const char* willTopic = _availabilityTopic[0] ? _availabilityTopic : nullptr;

    bool ok;
    if (_user && _user[0]) {
        ok = _mqtt.connect(_deviceId, _user, _pass, willTopic, 1, true, "offline");
    } else {
        ok = _mqtt.connect(_deviceId, nullptr, nullptr, willTopic, 1, true, "offline");
    }

    if (!ok) return;

    _cache.clear();

    if (_availabilityTopic[0])
        _mqtt.publish(_availabilityTopic, "online", true);

    // اشترك فقط في command topics (تخطى readOnly)
    for (uint8_t i = 0; i < _entityCount; i++) {
        HAEntity& e = _entities[i];
        if (!e.used) continue;
        if (e.readOnly) continue;

        if (e.type == HA_BUTTON || e.type == HA_SWITCH ||
            e.type == HA_SELECT || e.type == HA_NUMBER) {
            char topic[HA_TOPIC_LEN];
            _buildCommandTopic(topic, sizeof(topic), e.id);
            _mqtt.subscribe(topic);
        }
    }

    _mqtt.setCallback([this](char* topic, uint8_t* payload, unsigned int len) {
        if (_commandCallback) {
            char buf[HA_CMD_BUF];
            unsigned int n = (len < sizeof(buf) - 1) ? len : sizeof(buf) - 1;
            memcpy(buf, payload, n);
            buf[n] = '\0';
            _commandCallback(topic, buf);
        }
    });
}

// ═══════════════════════════════════════════
//              Add Entities
// ═══════════════════════════════════════════
HAEntity *HAMQTT::_findEntity(const char *id)
{
    for (uint8_t i = 0; i < _entityCount; i++)
    {
        if (_entities[i].used && strcmp(_entities[i].id, id) == 0)
            return &_entities[i];
    }
    return nullptr;
}

bool HAMQTT::addSensor(const char *id, const char *name,
                       const char *unit, const char *deviceClass,
                       const char *icon, bool stateClass)
{
    if (_entityCount >= HA_MAX_ENTITIES)
        return false;
    if (_findEntity(id))
        return false;

    HAEntity &e = _entities[_entityCount++];
    memset(&e, 0, sizeof(e));
    e.type = HA_SENSOR;
    e.used = true;
    e.retained = true;
    e.hasStateClass = stateClass;

    strncpy(e.id, id, HA_ID_LEN - 1);
    strncpy(e.name, name, HA_NAME_LEN - 1);
    strncpy(e.unit, unit, HA_UNIT_LEN - 1);
    strncpy(e.deviceClass, deviceClass, HA_DC_LEN - 1);
    strncpy(e.icon, icon, HA_ICON_LEN - 1);
    strncpy(e.group, _currentGroup, HA_GROUP_LEN - 1);   // ← ✅ جديد
    e.group[HA_GROUP_LEN - 1] = '\0';
    return true;
}

bool HAMQTT::addBinarySensor(const char *id, const char *name,
                             const char *deviceClass, const char *icon)
{
    if (_entityCount >= HA_MAX_ENTITIES)
        return false;
    if (_findEntity(id))
        return false;

    HAEntity &e = _entities[_entityCount++];
    memset(&e, 0, sizeof(e));
    e.type = HA_BINARY_SENSOR;
    e.used = true;
    e.retained = true;

    strncpy(e.id, id, HA_ID_LEN - 1);
    strncpy(e.name, name, HA_NAME_LEN - 1);
    strncpy(e.deviceClass, deviceClass, HA_DC_LEN - 1);
    strncpy(e.icon, icon, HA_ICON_LEN - 1);
    strncpy(e.group, _currentGroup, HA_GROUP_LEN - 1);   // ← ✅ جديد
    e.group[HA_GROUP_LEN - 1] = '\0';
    return true;
}

bool HAMQTT::addButton(const char *id, const char *name, const char *icon)
{
    if (_entityCount >= HA_MAX_ENTITIES)
        return false;
    if (_findEntity(id))
        return false;

    HAEntity &e = _entities[_entityCount++];
    memset(&e, 0, sizeof(e));
    e.type = HA_BUTTON;
    e.used = true;
    e.retained = true;

    strncpy(e.id, id, HA_ID_LEN - 1);
    strncpy(e.name, name, HA_NAME_LEN - 1);
    strncpy(e.icon, icon, HA_ICON_LEN - 1);
    strncpy(e.group, _currentGroup, HA_GROUP_LEN - 1);   // ← ✅ جديد
    e.group[HA_GROUP_LEN - 1] = '\0';
    return true;
}

bool HAMQTT::addSwitch(const char *id, const char *name, const char *icon)
{
    if (_entityCount >= HA_MAX_ENTITIES)
        return false;
    if (_findEntity(id))
        return false;

    HAEntity &e = _entities[_entityCount++];
    memset(&e, 0, sizeof(e));
    e.type = HA_SWITCH;
    e.used = true;
    e.retained = true;

    strncpy(e.id, id, HA_ID_LEN - 1);
    strncpy(e.name, name, HA_NAME_LEN - 1);
    strncpy(e.icon, icon, HA_ICON_LEN - 1);
    strncpy(e.group, _currentGroup, HA_GROUP_LEN - 1);   // ← ✅ جديد
    e.group[HA_GROUP_LEN - 1] = '\0';
    return true;
}

// ⚠️ مهم: optionsJson لازم يكون string literal (flash)
bool HAMQTT::addSelect(const char* id, const char* name,
                       const char* optionsJson, const char* icon,
                       bool readOnly)
{
    if (_entityCount >= HA_MAX_ENTITIES) return false;
    if (_findEntity(id)) return false;

    HAEntity& e = _entities[_entityCount++];
    memset(&e, 0, sizeof(e));
    e.type = HA_SELECT;
    e.used = true;
    e.retained = true;
    e.readOnly = readOnly;

    strncpy(e.id, id, HA_ID_LEN - 1);
    strncpy(e.name, name, HA_NAME_LEN - 1);
    strncpy(e.icon, icon, HA_ICON_LEN - 1);
    strncpy(e.group, _currentGroup, HA_GROUP_LEN - 1);   // ← ✅ جديد
    e.group[HA_GROUP_LEN - 1] = '\0';

    e.options = optionsJson;

    return true;
}

bool HAMQTT::addNumber(const char* id, const char* name,
                       float minVal, float maxVal, float step,
                       const char* unit, const char* icon,
                       bool readOnly)
{
    if (_entityCount >= HA_MAX_ENTITIES) return false;
    if (_findEntity(id)) return false;

    HAEntity& e = _entities[_entityCount++];
    memset(&e, 0, sizeof(e));
    e.type = HA_NUMBER;
    e.used = true;
    e.retained = true;
    e.readOnly = readOnly;

    strncpy(e.id, id, HA_ID_LEN - 1);
    strncpy(e.name, name, HA_NAME_LEN - 1);
    strncpy(e.unit, unit, HA_UNIT_LEN - 1);
    strncpy(e.icon, icon, HA_ICON_LEN - 1);
    strncpy(e.group, _currentGroup, HA_GROUP_LEN - 1);   // ← ✅ جديد
    e.group[HA_GROUP_LEN - 1] = '\0';

    e.range.min = minVal;
    e.range.max = maxVal;
    e.range.step = step;
    return true;
}

// ═══════════════════════════════════════════
//              Topic Helpers
// ═══════════════════════════════════════════
bool HAMQTT::_buildStateTopic(char *buf, size_t len, const char *entityId)
{
    int n = snprintf(buf, len, "%s/%s/state", _stateTopicPrefix, entityId);
    return (n > 0 && (size_t)n < len);
}

bool HAMQTT::_buildCommandTopic(char *buf, size_t len, const char *entityId)
{
    int n = snprintf(buf, len, "%s/%s/set", _stateTopicPrefix, entityId);
    return (n > 0 && (size_t)n < len);
}

// ═══════════════════════════════════════════
//              Discovery
// ═══════════════════════════════════════════
bool HAMQTT::publishDiscovery(const char *entityId)
{
    if (!_mqtt.connected())
        return false;

    HAEntity *e = _findEntity(entityId);
    if (!e)
        return false;

    HADiscovery::Context ctx;
    ctx.deviceId = _deviceId;
    ctx.deviceName = _deviceName;
    ctx.manufacturer = _manufacturer;
    ctx.model = _model;
    ctx.swVersion = _swVersion;
    ctx.stateTopicPrefix = _stateTopicPrefix;
    ctx.availabilityTopic = _availabilityTopic;
    ctx.group = e->group;   // ← ✅ جديد

    char topic[HA_TOPIC_LEN];
    const char *component = HADiscovery::typeToComponent(e->type);

    if (!HADiscovery::buildTopic(topic, sizeof(topic),
                                 _discoveryPrefix, component,
                                 _deviceId, e->id))
        return false;

    char payload[HA_DISCOVERY_BUF];
    int len = HADiscovery::build(payload, sizeof(payload), *e, ctx);

    if (len <= 0 || (size_t)len >= sizeof(payload))
        return false;

    return _mqtt.publish(topic, payload, true);
}

bool HAMQTT::publishDiscovery()
{
    if (!_mqtt.connected())
        return false;

    bool allOk = true;
    for (uint8_t i = 0; i < _entityCount; i++)
    {
        if (_entities[i].used)
        {
            if (!publishDiscovery(_entities[i].id))
                allOk = false;
            delay(20);
        }
    }
    return allOk;
}

// ═══════════════════════════════════════════
//              Publish State (مع cache)
// ═══════════════════════════════════════════
bool HAMQTT::_doPublish(const char *topic, const char *value,
                        bool retain, unsigned long heartbeatMs)
{
    if (!_mqtt.connected())
        return false;
    if (!_cache.shouldPublish(topic, value, heartbeatMs))
        return true;

    if (_mqtt.publish(topic, value, retain))
    {
        _cache.commit(topic, value);
        return true;
    }
    return false;
}

bool HAMQTT::publishRaw(const char *topic, const char *value,
                        bool retain, unsigned long heartbeatMs)
{
    return _doPublish(topic, value, retain, heartbeatMs);
}

// ═══════════════════════════════════════════
//   publishState overloads (الترتيب مهم!)
// ═══════════════════════════════════════════

// 1️⃣ الأساسي - const char*
bool HAMQTT::publishState(const char *entityId, const char *value,
                          unsigned long heartbeatMs)
{
    HAEntity *e = _findEntity(entityId);
    if (!e)
        return false;

    char topic[HA_TOPIC_LEN];
    if (!_buildStateTopic(topic, sizeof(topic), entityId))
        return false;

    return _doPublish(topic, value, e->retained, heartbeatMs);
}

// 2️⃣ float
bool HAMQTT::publishState(const char *entityId, float value,
                          uint8_t decimals, unsigned long heartbeatMs)
{
    char buf[HA_STATE_BUF];
    dtostrf(value, 0, decimals, buf);
    return publishState(entityId, buf, heartbeatMs);
}

// 3️⃣ int
bool HAMQTT::publishState(const char *entityId, int value,
                          unsigned long heartbeatMs)
{
    char buf[HA_STATE_BUF];
    snprintf(buf, sizeof(buf), "%d", value);
    return publishState(entityId, buf, heartbeatMs);
}

// 4️⃣ unsigned int
bool HAMQTT::publishState(const char *entityId, unsigned int value,
                          unsigned long heartbeatMs)
{
    char buf[HA_STATE_BUF];
    snprintf(buf, sizeof(buf), "%u", value);
    return publishState(entityId, buf, heartbeatMs);
}

// 5️⃣ unsigned long
bool HAMQTT::publishState(const char *entityId, unsigned long value,
                          unsigned long heartbeatMs)
{
    char buf[HA_STATE_BUF];
    snprintf(buf, sizeof(buf), "%lu", value);
    return publishState(entityId, buf, heartbeatMs);
}

// 6️⃣ binary
bool HAMQTT::publishBinaryState(const char *entityId, bool on,
                                unsigned long heartbeatMs)
{
    return publishState(entityId, on ? "true" : "false", heartbeatMs);
}

// ═══════════════════════════════════════════
//              Availability
// ═══════════════════════════════════════════
void HAMQTT::publishAvailable()
{
    if (_mqtt.connected() && _availabilityTopic[0])
        _mqtt.publish(_availabilityTopic, "online", true);
}

void HAMQTT::publishUnavailable()
{
    if (_mqtt.connected() && _availabilityTopic[0])
        _mqtt.publish(_availabilityTopic, "offline", true);
}

// ═══════════════════════════════════════════
//              Commands
// ═══════════════════════════════════════════
void HAMQTT::onCommand(HACommandCallback callback)
{
    _commandCallback = callback;

    if (_mqtt.connected()) {
        for (uint8_t i = 0; i < _entityCount; i++) {
            HAEntity& e = _entities[i];
            if (!e.used) continue;
            if (e.readOnly) continue;

            if (e.type == HA_BUTTON || e.type == HA_SWITCH ||
                e.type == HA_SELECT || e.type == HA_NUMBER) {
                char topic[HA_TOPIC_LEN];
                _buildCommandTopic(topic, sizeof(topic), e.id);
                _mqtt.subscribe(topic);
            }
        }
    }
}