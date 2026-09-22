#ifndef HA_ENTITY_H
#define HA_ENTITY_H

#include <Arduino.h>
#include "HAConfig.h"

// ═══ حد طول اسم الـ Group (احتياطي) ═══
#ifndef HA_GROUP_LEN
#define HA_GROUP_LEN 24
#endif

enum HAEntityType : uint8_t {
    HA_SENSOR,
    HA_BINARY_SENSOR,
    HA_BUTTON,
    HA_SWITCH,
    HA_SELECT,
    HA_NUMBER
};

// ═══ هيكل الكيان المحسّن ═══
struct HAEntity {
    HAEntityType type;
    bool used;
    bool retained;
    bool hasStateClass;
    bool readOnly;          // ← للعرض فقط

    char id[HA_ID_LEN];
    char name[HA_NAME_LEN];
    char unit[HA_UNIT_LEN];
    char deviceClass[HA_DC_LEN];
    char icon[HA_ICON_LEN];
    char group[HA_GROUP_LEN];   // ← ✅ جديد: اسم الجهاز الفرعي

    union {
        const char* options;
        struct {
            float min;
            float max;
            float step;
        } range;
    };
};

#endif