#include "HADiscovery.h"
#include <stdio.h>
#include <string.h>

const char* HADiscovery::typeToComponent(HAEntityType type) {
    switch (type) {
        case HA_SENSOR:        return "sensor";
        case HA_BINARY_SENSOR: return "binary_sensor";
        case HA_BUTTON:        return "button";
        case HA_SWITCH:        return "switch";
    }
    return "sensor";
}

bool HADiscovery::buildTopic(char* buf, size_t bufLen,
                             const char* discoveryPrefix,
                             const char* component,
                             const char* deviceId,
                             const char* entityId)
{
    int n = snprintf(buf, bufLen,
                     "%s/%s/%s/%s/config",
                     discoveryPrefix, component, deviceId, entityId);
    return (n > 0 && (size_t)n < bufLen);
}

int HADiscovery::_appendDeviceBlock(char* buf, size_t bufLen, int n,
                                    const Context& ctx)
{
    n += snprintf(buf + n, bufLen - n,
                  "\"device\":{\"identifiers\":[\"%s\"],\"name\":\"%s\"",
                  ctx.deviceId, ctx.deviceName);

    if (ctx.manufacturer && ctx.manufacturer[0])
        n += snprintf(buf + n, bufLen - n,
                      ",\"manufacturer\":\"%s\"", ctx.manufacturer);

    if (ctx.model && ctx.model[0])
        n += snprintf(buf + n, bufLen - n,
                      ",\"model\":\"%s\"", ctx.model);

    if (ctx.swVersion && ctx.swVersion[0])
        n += snprintf(buf + n, bufLen - n,
                      ",\"sw_version\":\"%s\"", ctx.swVersion);

    n += snprintf(buf + n, bufLen - n, "}");
    return n;
}

int HADiscovery::_appendAvailabilityBlock(char* buf, size_t bufLen, int n,
                                          const Context& ctx)
{
    if (!ctx.availabilityTopic || ctx.availabilityTopic[0] == '\0')
        return n;

    n += snprintf(buf + n, bufLen - n,
                  ",\"availability_topic\":\"%s\","
                  "\"payload_available\":\"online\","
                  "\"payload_not_available\":\"offline\"",
                  ctx.availabilityTopic);
    return n;
}

int HADiscovery::_appendOptionalFields(char* buf, size_t bufLen, int n,
                                       const HAEntity& e)
{
    if (e.icon[0])
        n += snprintf(buf + n, bufLen - n,
                      ",\"icon\":\"%s\"", e.icon);
    return n;
}

int HADiscovery::buildSensor(char* buf, size_t bufLen,
                             const HAEntity& e, const Context& ctx)
{
    char stateTopic[HA_TOPIC_LEN];
    snprintf(stateTopic, sizeof(stateTopic),
             "%s/%s/state", ctx.stateTopicPrefix, e.id);

    int n = 0;

    n += snprintf(buf + n, bufLen - n,
                  "{\"name\":\"%s\","
                  "\"unique_id\":\"%s_%s\","
                  "\"state_topic\":\"%s\",",
                  e.name, ctx.deviceId, e.id, stateTopic);

    n = _appendDeviceBlock(buf, bufLen, n, ctx);

    if (e.unit[0])
        n += snprintf(buf + n, bufLen - n,
                      ",\"unit_of_measurement\":\"%s\"", e.unit);

    if (e.deviceClass[0])
        n += snprintf(buf + n, bufLen - n,
                      ",\"device_class\":\"%s\"", e.deviceClass);

    n = _appendOptionalFields(buf, bufLen, n, e);
    n = _appendAvailabilityBlock(buf, bufLen, n, ctx);

    n += snprintf(buf + n, bufLen - n, "}");
    return n;
}

int HADiscovery::buildBinarySensor(char* buf, size_t bufLen,
                                   const HAEntity& e, const Context& ctx)
{
    char stateTopic[HA_TOPIC_LEN];
    snprintf(stateTopic, sizeof(stateTopic),
             "%s/%s/state", ctx.stateTopicPrefix, e.id);

    int n = 0;

    n += snprintf(buf + n, bufLen - n,
                  "{\"name\":\"%s\","
                  "\"unique_id\":\"%s_%s\","
                  "\"state_topic\":\"%s\","
                  "\"payload_on\":\"ON\","
                  "\"payload_off\":\"OFF\",",
                  e.name, ctx.deviceId, e.id, stateTopic);

    n = _appendDeviceBlock(buf, bufLen, n, ctx);

    if (e.deviceClass[0])
        n += snprintf(buf + n, bufLen - n,
                      ",\"device_class\":\"%s\"", e.deviceClass);

    n = _appendOptionalFields(buf, bufLen, n, e);
    n = _appendAvailabilityBlock(buf, bufLen, n, ctx);

    n += snprintf(buf + n, bufLen - n, "}");
    return n;
}

int HADiscovery::buildButton(char* buf, size_t bufLen,
                             const HAEntity& e, const Context& ctx)
{
    char cmdTopic[HA_TOPIC_LEN];
    snprintf(cmdTopic, sizeof(cmdTopic),
             "%s/%s/set", ctx.stateTopicPrefix, e.id);

    int n = 0;

    n += snprintf(buf + n, bufLen - n,
                  "{\"name\":\"%s\","
                  "\"unique_id\":\"%s_%s\","
                  "\"command_topic\":\"%s\","
                  "\"payload_press\":\"PRESS\",",
                  e.name, ctx.deviceId, e.id, cmdTopic);

    n = _appendDeviceBlock(buf, bufLen, n, ctx);
    n = _appendOptionalFields(buf, bufLen, n, e);
    n = _appendAvailabilityBlock(buf, bufLen, n, ctx);

    n += snprintf(buf + n, bufLen - n, "}");
    return n;
}

int HADiscovery::buildSwitch(char* buf, size_t bufLen,
                             const HAEntity& e, const Context& ctx)
{
    char cmdTopic[HA_TOPIC_LEN];
    char stateTopic[HA_TOPIC_LEN];

    snprintf(cmdTopic, sizeof(cmdTopic),
             "%s/%s/set", ctx.stateTopicPrefix, e.id);
    snprintf(stateTopic, sizeof(stateTopic),
             "%s/%s/state", ctx.stateTopicPrefix, e.id);

    int n = 0;

    n += snprintf(buf + n, bufLen - n,
                  "{\"name\":\"%s\","
                  "\"unique_id\":\"%s_%s\","
                  "\"command_topic\":\"%s\","
                  "\"state_topic\":\"%s\","
                  "\"payload_on\":\"ON\","
                  "\"payload_off\":\"OFF\","
                  "\"state_on\":\"ON\","
                  "\"state_off\":\"OFF\",",
                  e.name, ctx.deviceId, e.id, cmdTopic, stateTopic);

    n = _appendDeviceBlock(buf, bufLen, n, ctx);
    n = _appendOptionalFields(buf, bufLen, n, e);
    n = _appendAvailabilityBlock(buf, bufLen, n, ctx);

    n += snprintf(buf + n, bufLen - n, "}");
    return n;
}