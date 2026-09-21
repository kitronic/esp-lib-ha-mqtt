#include "HAStateCache.h"
#include <string.h>

HAStateCache::HAStateCache() : _count(0) {
    memset(_entries, 0, sizeof(_entries));
}

void HAStateCache::clear() {
    for (uint8_t i = 0; i < HA_MAX_CACHE; i++) {
        _entries[i].used = false;
    }
    _count = 0;
}

int HAStateCache::_find(const char* topic) {
    for (uint8_t i = 0; i < _count; i++) {
        if (_entries[i].used && strcmp(_entries[i].topic, topic) == 0)
            return i;
    }
    return -1;
}

bool HAStateCache::shouldPublish(const char* topic, const char* value,
                                 unsigned long heartbeatMs)
{
    int idx = _find(topic);
    if (idx < 0) return true;
    if (strcmp(_entries[idx].value, value) != 0) return true;
    if ((millis() - _entries[idx].lastTime) >= heartbeatMs) return true;
    return false;
}

void HAStateCache::commit(const char* topic, const char* value) {
    int idx = _find(topic);

    if (idx < 0) {
        if (_count >= HA_MAX_CACHE) {
            // استبدل الأقدم (LRU بسيط: نستبدل الأول)
            idx = 0;
        } else {
            idx = _count++;
        }
        _entries[idx].used = true;
        strncpy(_entries[idx].topic, topic, HA_CACHE_TOPIC_LEN - 1);
        _entries[idx].topic[HA_CACHE_TOPIC_LEN - 1] = '\0';
    }

    strncpy(_entries[idx].value, value, HA_CACHE_VALUE_LEN - 1);
    _entries[idx].value[HA_CACHE_VALUE_LEN - 1] = '\0';
    _entries[idx].lastTime = millis();
}