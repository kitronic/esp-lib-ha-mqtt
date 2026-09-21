#ifndef HA_DISCOVERY_H
#define HA_DISCOVERY_H

#include <Arduino.h>
#include "HAEntity.h"

class HADiscovery {
public:
    struct Context {
        const char* deviceId;
        const char* deviceName;
        const char* manufacturer;
        const char* model;
        const char* swVersion;
        const char* stateTopicPrefix;
        const char* availabilityTopic;
    };

    static int buildSensor(char* buf, size_t bufLen,
                           const HAEntity& e, const Context& ctx);

    static int buildBinarySensor(char* buf, size_t bufLen,
                                 const HAEntity& e, const Context& ctx);

    static int buildButton(char* buf, size_t bufLen,
                           const HAEntity& e, const Context& ctx);

    static int buildSwitch(char* buf, size_t bufLen,
                           const HAEntity& e, const Context& ctx);

    static bool buildTopic(char* buf, size_t bufLen,
                           const char* discoveryPrefix,
                           const char* component,
                           const char* deviceId,
                           const char* entityId);

    static const char* typeToComponent(HAEntityType type);

private:
    static int _appendDeviceBlock(char* buf, size_t bufLen, int n,
                                  const Context& ctx);

    static int _appendAvailabilityBlock(char* buf, size_t bufLen, int n,
                                        const Context& ctx);

    static int _appendOptionalFields(char* buf, size_t bufLen, int n,
                                     const HAEntity& e);
};

#endif