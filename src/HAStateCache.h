#ifndef HA_STATE_CACHE_H
#define HA_STATE_CACHE_H

#include <Arduino.h>
#include "HAConfig.h"

class HAStateCache {
public:
    HAStateCache();
    void clear();
    bool shouldPublish(const char* topic, const char* value,
                       unsigned long heartbeatMs = HA_DEFAULT_HEARTBEAT);
    void commit(const char* topic, const char* value);
    uint8_t size() const { return _count; }

private:
    struct Entry {
        char topic[HA_CACHE_TOPIC_LEN];
        char value[HA_CACHE_VALUE_LEN];
        unsigned long lastTime;
        bool used;
    };

    Entry _entries[HA_MAX_CACHE];
    uint8_t _count;

    int _find(const char* topic);
};

#endif