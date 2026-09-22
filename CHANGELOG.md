# Changelog

جميع التغييرات المهمة في هذا المشروع موثقة هنا.

التنسيق مبني على [Keep a Changelog](https://keepachangelog.com/ar/1.0.0/)
والمشروع يتبع [Semantic Versioning](https://semver.org/lang/ar/).

---

## [Unreleased]

### Planned
- دعم `light` entities (RGB + Brightness)
- دعم `cover` entities (Position)
- دعم `climate` entities
- دعم `fan` entities
- Web Configuration Portal
- دعم ESP32-C3 / S3

---

## [2.2.0] - 2026-09-22

### 🗂️ Groups Edition — تنظيم الأجهزة والكيانات

### Added
- ✨ **Device Groups** — دالة `setGroup()` لتنظيم الكيانات في أجهزة فرعية
- ✨ **clearGroup()** — العودة للوضع العادي (جهاز واحد)
- ✨ **Read-Only Entities** — parameter `readOnly` في `addSelect` و `addNumber`
- ✨ **via_device** — ربط الأجهزة الفرعية بالجهاز الرئيسي في HA
- ✨ **HA_GROUP_LEN** — ثابت جديد في `HAConfig.h` (افتراضي: 24)
- ✨ **group field** في `struct HAEntity`
- ✨ **group في Context** في `HADiscovery`

### Changed
- 🔧 `addSelect()` الآن يقبل parameter `readOnly = false`
- 🔧 `addNumber()` الآن يقبل parameter `readOnly = false`
- 🔧 `appendDevice()` في `HADiscovery.cpp` يدعم Groups
- 🔧 `publishDiscovery()` يمرّر `e->group` للـ Context

### Fixed
- 🐛 إصلاح `begin()` — ترجع `bool` بدل `void`
- 🐛 إصلاح ambiguous overload لـ `uint32_t` في `publishState`
- 🐛 إضافة `publishState(const char*, unsigned int)` و `publishState(const char*, unsigned long)`
- 🐛 إضافة تعريف `publishState(const char*, const char*)` الأساسي
- 🐛 إضافة تعريف `publishState(const char*, float, uint8_t)`

### Performance
- ⚡ RAM ثابتة: **~4.8 KB** (زيادة ~800 bytes بسبب `group` field)
- ⚡ Heap usage: **0 bytes**
- ⚡ Read-Only entities لا تستهلك `command_topic` → توفير RAM على ESP

### Memory Footprint (ESP8266)

```text
sizeof(HAEntity):      140 bytes  (v2.1: 116)
sizeof(HAStateCache): 2696 bytes  (v2.1: 2696)
sizeof(HAMQTT):       4800 bytes  (v2.1: 4200)
────────────────────────────────
Total:                ~7.5 KB     (v2.1: ~7 KB)

