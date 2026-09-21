#ifndef HA_ENTITY_H
#define HA_ENTITY_H

#include <Arduino.h>
#include "HAConfig.h"

enum HAEntityType : uint8_t {
    HA_SENSOR,
    HA_BINARY_SENSOR,
    HA_BUTTON,
    HA_SWITCH,
    HA_SELECT,
    HA_NUMBER
};

// ═══ هيكل الكيان المحسّن (union لتوفير RAM) ═══
struct HAEntity {
    HAEntityType type;
    bool used;
    bool retained;
    bool hasStateClass;

    char id[HA_ID_LEN];
    char name[HA_NAME_LEN];
    char unit[HA_UNIT_LEN];
    char deviceClass[HA_DC_LEN];
    char icon[HA_ICON_LEN];

    // ═══ Union: إما options (SELECT) أو range (NUMBER) ═══
    union {
        const char* options;    // SELECT - pointer لـ flash string
        struct {
            float min;
            float max;
            float step;
        } range;                // NUMBER
    };
};

#endif