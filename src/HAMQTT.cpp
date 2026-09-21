#include "HAMQTT.h"
#include "HADiscovery.h"

HAMQTT::HAMQTT(Client* networkClient)
    : _mqtt(*networkClient), _networkClient(networkClient)
{
    _server = nullptr;
    _port   = 1883;
    _user   = nullptr;
    _pass   = nullptr;

    strcpy(_discoveryPrefix, "homeassistant");
    _stateTopicPrefix[0] = '\0';
    _availabilityTopic[0] = '\0';
    _deviceId[0]  = '\0';
    _deviceName[0] = '\0';
    _manufacturer[0] = '\0';
    _model[0] = '\0';
    _swVersion[0] = '\0';

    _entityCount = 0;
    _lastReconnect = 0;
    _commandCallback = nullptr;

    memset(_entities, 0, sizeof(_entities));
}

// ═══════════════════════════════════════════
//              Configuration
// ═══════════════════════════════════════════
void HAMQTT::setServer(const char* server, uint16_t port,
                       const char* user, const char* pass)
{
    _server = server;
    _port   = port;
    _user   = user;
    _pass   = pass;
}

void HAMQTT::setDiscoveryPrefix(const char* prefix) {
    strncpy(_discoveryPrefix, prefix, sizeof(_discoveryPrefix) - 1);
    _discoveryPrefix[sizeof(_discoveryPrefix) - 1] = '\0';
}

void HAMQTT::setStateTopicPrefix(const char* prefix) {
    strncpy(_stateTopicPrefix, prefix, sizeof(_stateTopicPrefix) - 1);
    _stateTopicPrefix[sizeof(_stateTopicPrefix) - 1] = '\0';
}

void HAMQTT::setAvailabilityTopic(const char* topic) {
    strncpy(_availabilityTopic, topic, sizeof(_availabilityTopic) - 1);
    _availabilityTopic[sizeof(_availabilityTopic) - 1] = '\0';
}

void HAMQTT::setDevice(const char* id, const char* name,
                       const char* manufacturer, const char* model,
                       const char* swVersion)
{
    strncpy(_deviceId, id, sizeof(_deviceId) - 1);
    _deviceId[sizeof(_deviceId) - 1] = '\0';

    strncpy(_deviceName, name, sizeof(_deviceName) - 1);
    _deviceName[sizeof(_deviceName) - 1] = '\0';

    strncpy(_manufacturer, manufacturer, sizeof(_manufacturer) - 1);
    _manufacturer[sizeof(_manufacturer) - 1] = '\0';

    strncpy(_model, model, sizeof(_model) - 1);
    _model[sizeof(_model) - 1] = '\0';

    strncpy(_swVersion, swVersion, sizeof(_swVersion) - 1);
    _swVersion[sizeof(_swVersion) - 1] = '\0';
}

// ═══════════════════════════════════════════
//              Lifecycle
// ═══════════════════════════════════════════
bool HAMQTT::begin() {
    if (!_server || _server[0] == '\0') return false;

    _mqtt.setServer(_server, _port);
    _mqtt.setBufferSize(512);
    _mqtt.setKeepAlive(30);

    return reconnect();
}

void HAMQTT::loop() {
    if (!_mqtt.connected()) {
        unsigned long now = millis();
        if (now - _lastReconnect > 5000) {
            _lastReconnect = now;
            reconnect();
        }
        return;
    }
    _mqtt.loop();
}

bool HAMQTT::connected() {
    return _mqtt.connected();
}

void HAMQTT::reconnect() {
    if (_mqtt.connected()) return;

    const char* willTopic = (_availabilityTopic[0] != '\0')
                            ? _availabilityTopic : nullptr;

    bool ok;
    if (_user && _user[0] != '\0') {
        ok = _mqtt.connect(_deviceId, _user, _pass,
                           willTopic, 0, true, "offline");
    } else {
        ok = _mqtt.connect(_deviceId, nullptr, nullptr,
                           willTopic, 0, true, "offline");
    }

    if (!ok) return;

    if (_availabilityTopic[0] != '\0') {
        _mqtt.publish(_availabilityTopic, "online", true);
    }

    // اشترك في command topics
    if (_commandCallback) {
        for (uint8_t i = 0; i < _entityCount; i++) {
            HAEntity& e = _entities[i];
            if (e.used && (e.type == HA_BUTTON || e.type == HA_SWITCH)) {
                char topic[HA_TOPIC_LEN];
                _buildCommandTopic(topic, sizeof(topic), e.id);
                _mqtt.subscribe(topic);
            }
        }
    }

    // تسجيل callback
    _mqtt.setCallback([this](char* topic, uint8_t* payload, unsigned int len) {
        if (_commandCallback) {
            char buf[128];
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
HAEntity* HAMQTT::_findEntity(const char* id) {
    for (uint8_t i = 0; i < _entityCount; i++) {
        if (_entities[i].used && strcmp(_entities[i].id, id) == 0) {
            return &_entities[i];
        }
    }
    return nullptr;
}

bool HAMQTT::addSensor(const char* id, const char* name,
                       const char* unit, const char* deviceClass,
                       const char* icon, bool retained)
{
    if (_entityCount >= HA_MAX_ENTITIES) return false;
    if (_findEntity(id)) return false;

    HAEntity& e = _entities[_entityCount++];
    e.type = HA_SENSOR;
    e.used = true;
    e.retained = retained;

    strncpy(e.id, id, HA_ID_LEN - 1);           e.id[HA_ID_LEN - 1] = '\0';
    strncpy(e.name, name, HA_NAME_LEN - 1);     e.name[HA_NAME_LEN - 1] = '\0';
    strncpy(e.unit, unit, HA_UNIT_LEN - 1);     e.unit[HA_UNIT_LEN - 1] = '\0';
    strncpy(e.deviceClass, deviceClass, HA_DC_LEN - 1);
    e.deviceClass[HA_DC_LEN - 1] = '\0';
    strncpy(e.icon, icon, HA_ICON_LEN - 1);     e.icon[HA_ICON_LEN - 1] = '\0';
    return true;
}

bool HAMQTT::addBinarySensor(const char* id, const char* name,
                             const char* deviceClass, const char* icon)
{
    if (_entityCount >= HA_MAX_ENTITIES) return false;
    if (_findEntity(id)) return false;

    HAEntity& e = _entities[_entityCount++];
    e.type = HA_BINARY_SENSOR;
    e.used = true;
    e.retained = true;

    strncpy(e.id, id, HA_ID_LEN - 1);           e.id[HA_ID_LEN - 1] = '\0';
    strncpy(e.name, name, HA_NAME_LEN - 1);     e.name[HA_NAME_LEN - 1] = '\0';
    strncpy(e.deviceClass, deviceClass, HA_DC_LEN - 1);
    e.deviceClass[HA_DC_LEN - 1] = '\0';
    strncpy(e.icon, icon, HA_ICON_LEN - 1);     e.icon[HA_ICON_LEN - 1] = '\0';
    e.unit[0] = '\0';
    return true;
}

bool HAMQTT::addButton(const char* id, const char* name, const char* icon) {
    if (_entityCount >= HA_MAX_ENTITIES) return false;
    if (_findEntity(id)) return false;

    HAEntity& e = _entities[_entityCount++];
    e.type = HA_BUTTON;
    e.used = true;
    e.retained = true;

    strncpy(e.id, id, HA_ID_LEN - 1);           e.id[HA_ID_LEN - 1] = '\0';
    strncpy(e.name, name, HA_NAME_LEN - 1);     e.name[HA_NAME_LEN - 1] = '\0';
    strncpy(e.icon, icon, HA_ICON_LEN - 1);     e.icon[HA_ICON_LEN - 1] = '\0';
    e.unit[0] = '\0';
    e.deviceClass[0] = '\0';
    return true;
}

bool HAMQTT::addSwitch(const char* id, const char* name, const char* icon) {
    if (_entityCount >= HA_MAX_ENTITIES) return false;
    if (_findEntity(id)) return false;

    HAEntity& e = _entities[_entityCount++];
    e.type = HA_SWITCH;
    e.used = true;
    e.retained = true;

    strncpy(e.id, id, HA_ID_LEN - 1);           e.id[HA_ID_LEN - 1] = '\0';
    strncpy(e.name, name, HA_NAME_LEN - 1);     e.name[HA_NAME_LEN - 1] = '\0';
    strncpy(e.icon, icon, HA_ICON_LEN - 1);     e.icon[HA_ICON_LEN - 1] = '\0';
    e.unit[0] = '\0';
    e.deviceClass[0] = '\0';
    return true;
}

// ═══════════════════════════════════════════
//              Topic Builders
// ═══════════════════════════════════════════
bool HAMQTT::_buildStateTopic(char* buf, size_t len, const char* entityId) {
    int n = snprintf(buf, len, "%s/%s/state", _stateTopicPrefix, entityId);
    return (n > 0 && (size_t)n < len);
}

bool HAMQTT::_buildCommandTopic(char* buf, size_t len, const char* entityId) {
    int n = snprintf(buf, len, "%s/%s/set", _stateTopicPrefix, entityId);
    return (n > 0 && (size_t)n < len);
}

// ═══════════════════════════════════════════
//              Publish Discovery
// ═══════════════════════════════════════════
bool HAMQTT::publishDiscovery(const char* entityId) {
    if (!_mqtt.connected()) return false;

    HAEntity* e = _findEntity(entityId);
    if (!e) return false;

    HADiscovery::Context ctx;
    ctx.deviceId          = _deviceId;
    ctx.deviceName        = _deviceName;
    ctx.manufacturer      = _manufacturer;
    ctx.model             = _model;
    ctx.swVersion         = _swVersion;
    ctx.stateTopicPrefix  = _stateTopicPrefix;
    ctx.availabilityTopic = _availabilityTopic;

    char topic[HA_TOPIC_LEN];
    const char* component = HADiscovery::typeToComponent(e->type);

    if (!HADiscovery::buildTopic(topic, sizeof(topic),
                                 _discoveryPrefix, component,
                                 _deviceId, e->id)) {
        return false;
    }

    char payload[512];
    int len = 0;

    switch (e->type) {
        case HA_SENSOR:
            len = HADiscovery::buildSensor(payload, sizeof(payload), *e, ctx);
            break;
        case HA_BINARY_SENSOR:
            len = HADiscovery::buildBinarySensor(payload, sizeof(payload), *e, ctx);
            break;
        case HA_BUTTON:
            len = HADiscovery::buildButton(payload, sizeof(payload), *e, ctx);
            break;
        case HA_SWITCH:
            len = HADiscovery::buildSwitch(payload, sizeof(payload), *e, ctx);
            break;
    }

    if (len <= 0 || (size_t)len >= sizeof(payload)) return false;

    return _mqtt.publish(topic, payload, true);
}

bool HAMQTT::publishDiscovery() {
    if (!_mqtt.connected()) return false;

    bool allOk = true;
    for (uint8_t i = 0; i < _entityCount; i++) {
        if (_entities[i].used) {
            if (!publishDiscovery(_entities[i].id)) allOk = false;
            delay(20);
        }
    }
    return allOk;
}

// ═══════════════════════════════════════════
//              Publish State
// ═══════════════════════════════════════════
bool HAMQTT::publishState(const char* entityId, const char* value) {
    if (!_mqtt.connected()) return false;

    HAEntity* e = _findEntity(entityId);
    if (!e) return false;

    char topic[HA_TOPIC_LEN];
    _buildStateTopic(topic, sizeof(topic), entityId);
    return _mqtt.publish(topic, value, e->retained);
}

bool HAMQTT::publishState(const char* entityId, float value, uint8_t decimals) {
    char buf[24];
    dtostrf(value, 0, decimals, buf);
    return publishState(entityId, buf);
}

bool HAMQTT::publishState(const char* entityId, int value) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", value);
    return publishState(entityId, buf);
}

bool HAMQTT::publishBinaryState(const char* entityId, bool on) {
    return publishState(entityId, on ? "ON" : "OFF");
}

bool HAMQTT::publishAttribute(const char* entityId, const char* attr,
                              const char* value) {
    if (!_mqtt.connected()) return false;

    HAEntity* e = _findEntity(entityId);
    if (!e) return false;

    char topic[HA_TOPIC_LEN];
    snprintf(topic, sizeof(topic), "%s/%s/attr/%s",
             _stateTopicPrefix, entityId, attr);
    return _mqtt.publish(topic, value, false);
}

// ═══════════════════════════════════════════
//              Availability
// ═══════════════════════════════════════════
void HAMQTT::publishAvailable() {
    if (_mqtt.connected() && _availabilityTopic[0]) {
        _mqtt.publish(_availabilityTopic, "online", true);
    }
}

void HAMQTT::publishUnavailable() {
    if (_mqtt.connected() && _availabilityTopic[0]) {
        _mqtt.publish(_availabilityTopic, "offline", true);
    }
}

// ═══════════════════════════════════════════
//              Commands
// ═══════════════════════════════════════════
void HAMQTT::onCommand(HACommandCallback callback) {
    _commandCallback = callback;

    if (_mqtt.connected()) {
        for (uint8_t i = 0; i < _entityCount; i++) {
            HAEntity& e = _entities[i];
            if (e.used && (e.type == HA_BUTTON || e.type == HA_SWITCH)) {
                char topic[HA_TOPIC_LEN];
                _buildCommandTopic(topic, sizeof(topic), e.id);
                _mqtt.subscribe(topic);
            }
        }
    }
}