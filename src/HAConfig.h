#ifndef HA_CONFIG_H
#define HA_CONFIG_H

// ═══════════════════════════════════════════
//   إعدادات المكتبة - عدّلها حسب احتياجك
// ═══════════════════════════════════════════
//   أقل قيمة = أقل RAM
// ═══════════════════════════════════════════

// ═══ حدود الكيانات ═══
#ifndef HA_MAX_ENTITIES
#define HA_MAX_ENTITIES      16    // كيانات
#endif

#ifndef HA_MAX_CACHE
#define HA_MAX_CACHE         24    // عناصر cache
#endif

// ═══ أحجام الحقول ═══
#define HA_ID_LEN            24    // sensor_id
#define HA_NAME_LEN          40    // "Battery Voltage"
#define HA_UNIT_LEN          10    // "V", "°C", "%"
#define HA_DC_LEN            18    // "temperature"
#define HA_ICON_LEN          28    // "mdi:battery-70"
#define HA_TOPIC_LEN         80    // "solar/inverter3/xxx/state"
#define HA_CACHE_TOPIC_LEN   72    // topic for cache
#define HA_CACHE_VALUE_LEN   28    // value for cache
#define HA_GROUP_LEN         24    // ← ✅ جديد: "Sensors" / "Controls" / "Diagnostics"

// ═══ أحجام الـ payload buffers ═══
#define HA_DISCOVERY_BUF     768   // buffer للـ discovery JSON
#define HA_STATE_BUF         24    // buffer للـ state value
#define HA_CMD_BUF           96    // buffer للـ command payload

// ═══ MQTT Buffer ═══
#ifndef HA_MQTT_BUFFER_SIZE
#define HA_MQTT_BUFFER_SIZE  768   // PubSubClient buffer
#endif

// ═══ Heartbeat الافتراضي (ms) ═══
#define HA_DEFAULT_HEARTBEAT 60000

#endif