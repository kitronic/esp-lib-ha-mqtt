#ifndef HA_ENTITY_H
#define HA_ENTITY_H

#include <Arduino.h>

// ═══ أنواع الكيانات المدعومة ═══
enum HAEntityType : uint8_t {
    HA_SENSOR,
    HA_BINARY_SENSOR,
    HA_BUTTON,
    HA_SWITCH
};

// ═══ حدود الذاكرة (قابلة للتعديل من library.json) ═══
#ifndef HA_MAX_ENTITIES
#define HA_MAX_ENTITIES      32
#endif
#define HA_ID_LEN            32
#define HA_NAME_LEN          48
#define HA_UNIT_LEN          16
#define HA_DC_LEN            24
#define HA_ICON_LEN          32
#define HA_TOPIC_LEN         96

// ═══ هيكل الكيان ═══
struct HAEntity {
    HAEntityType type;
    char id[HA_ID_LEN];
    char name[HA_NAME_LEN];
    char unit[HA_UNIT_LEN];
    char deviceClass[HA_DC_LEN];
    char icon[HA_ICON_LEN];
    bool retained;
    bool used;
};

#endif