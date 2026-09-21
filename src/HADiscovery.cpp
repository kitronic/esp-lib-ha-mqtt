#include "HADiscovery.h"
#include <stdio.h>
#include <string.h>

const char* HADiscovery::typeToComponent(HAEntityType type) {
    switch (type) {
        case HA_SENSOR:        return "sensor";
        case HA_BINARY_SENSOR: return "binary_sensor";
        case HA_BUTTON:        return "button";
        case HA_SWITCH:        return "switch";
        case HA_SELECT:        return "select";
        case HA_NUMBER:        return "number";
    }
    return "sensor";
}

bool HADiscovery::buildTopic(char* buf, size_t bufLen,
                             const char* discoveryPrefix,
                             const char* component,
                             const char* deviceId,
                             const char* entityId)
{
    int n = snprintf(buf, bufLen, "%s/%s/%s/%s/config",
                     discoveryPrefix, component, deviceId, entityId);
    return (n > 0 && (size_t)n < bufLen);
}

// ═══ Device block مشترك ═══
static int appendDevice(char* buf, size_t bufLen, int n,
                        const HADiscovery::Context& ctx)
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

// ═══ Availability block مشترك ═══
static int appendAvail(char* buf, size_t bufLen, int n,
                       const HADiscovery::Context& ctx)
{
    if (!ctx.availabilityTopic || !ctx.availabilityTopic[0]) return n;
    n += snprintf(buf + n, bufLen - n,
                  ",\"availability_topic\":\"%s\","
                  "\"payload_available\":\"online\","
                  "\"payload_not_available\":\"offline\"",
                  ctx.availabilityTopic);
    return n;
}

int HADiscovery::build(char* buf, size_t bufLen,
                       const HAEntity& e, const Context& ctx)
{
    int n = 0;

    // ═══ الاسم + unique_id ═══
    n += snprintf(buf + n, bufLen - n,
                  "{\"name\":\"%s\","
                  "\"unique_id\":\"%s_%s\",",
                  e.name, ctx.deviceId, e.id);

    // ═══ حسب النوع ═══
    switch (e.type) {
        case HA_SENSOR:
            n += snprintf(buf + n, bufLen - n,
                          "\"state_topic\":\"%s/%s/state\",",
                          ctx.stateTopicPrefix, e.id);
            break;

        case HA_BINARY_SENSOR:
            n += snprintf(buf + n, bufLen - n,
                          "\"state_topic\":\"%s/%s/state\","
                          "\"payload_on\":\"true\","
                          "\"payload_off\":\"false\",",
                          ctx.stateTopicPrefix, e.id);
            break;

        case HA_BUTTON:
            n += snprintf(buf + n, bufLen - n,
                          "\"command_topic\":\"%s/%s/set\","
                          "\"payload_press\":\"PRESS\",",
                          ctx.stateTopicPrefix, e.id);
            break;

        case HA_SWITCH:
            n += snprintf(buf + n, bufLen - n,
                          "\"command_topic\":\"%s/%s/set\","
                          "\"state_topic\":\"%s/%s/state\","
                          "\"payload_on\":\"ON\","
                          "\"payload_off\":\"OFF\","
                          "\"state_on\":\"ON\","
                          "\"state_off\":\"OFF\",",
                          ctx.stateTopicPrefix, e.id,
                          ctx.stateTopicPrefix, e.id);
            break;

        case HA_SELECT:
            n += snprintf(buf + n, bufLen - n,
                          "\"command_topic\":\"%s/%s/set\","
                          "\"state_topic\":\"%s/%s/state\","
                          "\"options\":%s,",
                          ctx.stateTopicPrefix, e.id,
                          ctx.stateTopicPrefix, e.id,
                          e.options);
            break;

        case HA_NUMBER:
            n += snprintf(buf + n, bufLen - n,
                          "\"command_topic\":\"%s/%s/set\","
                          "\"state_topic\":\"%s/%s/state\","
                          "\"min\":%.2f,"
                          "\"max\":%.2f,"
                          "\"step\":%.2f,"
                          "\"mode\":\"box\",",
                          ctx.stateTopicPrefix, e.id,
                          ctx.stateTopicPrefix, e.id,
                          e.range.min, e.range.max, e.range.step);
            break;
    }

    // ═══ Device info ═══
    n = appendDevice(buf, bufLen, n, ctx);

    // ═══ Unit + DeviceClass ═══
    if (e.unit[0])
        n += snprintf(buf + n, bufLen - n,
                      ",\"unit_of_measurement\":\"%s\"", e.unit);

    if (e.deviceClass[0])
        n += snprintf(buf + n, bufLen - n,
                      ",\"device_class\":\"%s\"", e.deviceClass);

    // ═══ state_class للـ statistics ═══
    if (e.hasStateClass)
        n += snprintf(buf + n, bufLen - n,
                      ",\"state_class\":\"measurement\"");

    // ═══ Icon ═══
    if (e.icon[0])
        n += snprintf(buf + n, bufLen - n,
                      ",\"icon\":\"%s\"", e.icon);

    // ═══ Availability ═══
    n = appendAvail(buf, bufLen, n, ctx);

    // ═══ Close ═══
    n += snprintf(buf + n, bufLen - n, "}");
    return n;
}