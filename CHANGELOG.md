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

## [2.2.1] - 2026-09-28

### 🩹 Patch Release — إصلاحات ما بعد Groups Edition

### Fixed
- 🐛 رفع `HA_TOPIC_LEN` من 80 إلى 128 — كانت topics الـ discovery الطويلة تُقطع بصمت
- 🐛 إضافة `publishState(const char*, long)` لمنع ambiguous overload مع `int` و `unsigned int` و `unsigned long`
- 🐛 `setDevice()` الآن يستخدم `HA_NAME_LEN` لحقول `manufacturer` و `model` بدل `HA_ID_LEN` (كانت تُقصّ عند 24 حرف)
- 🐛 فحص نتيجة `_mqtt.setBufferSize()` في `begin()` — عند فشل التخصيص تُرجع `false` بدل المتابعة بصمت
- 🐛 تثبيت MQTT callback مرة واحدة في `begin()` بدل `reconnect()` — يمنع فقدانه بعد إعادة اتصال خارجية
- 🐛 إصلاح LRU في `HAStateCache::commit()` — كان يستبدل العنصر الأول، الآن يستبدل الأقدم زمنياً فعلاً
- 🐛 توحيد discovery topic مع `device.identifiers` في وضع Groups
- 🐛 إعادة كتابة `simple_sensor.ino` بالكامل — كان يستخدم API من مكتبة أخرى ولن يُترجم
- 🐛 `BasicHA.ino` يستخدم `config.h` بدل التعريفات المحلية المكررة
- 🐛 `random(-20, 20)` → `random(-20, 21)` لتوزيع متماثل بدون انحياز

### Changed
- 🔧 حذف عضو `_networkClient` غير المستخدم من `HAMQTT`
- 🔧 `HA_VERSION_LEN` ثابت جديد لـ `sw_version` (بدل الرقم السحري `12`)
- 🔧 `HA_GROUP_LEN` معرّف فقط في `HAConfig.h` (حُذف التعريف المكرر من `HAEntity.h`)
- 🔧 إضافة `static_assert` في `HAConfig.h` للتحقق من أحجام buffers عند الترجمة
- 🔧 استخراج `_subscribeAll()` و `_installCallback()` و `_makeTopicDeviceId()` كدوال خاصة في `HAMQTT`

### Migration Notes
- ⚠️ إذا كنت تستخدم v2.2.0 مع Groups، احذف retained topics القديمة قبل التحديث
- ⚠️ `sizeof(HAMQTT)` زاد قليلاً (~80 bytes) بسبب `_manufacturer` و `_model` الأكبر

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

    sizeof(HAEntity):      140 bytes  (v2.1: 116)
    sizeof(HAStateCache): 2696 bytes  (v2.1: 2696)
    sizeof(HAMQTT):       4800 bytes  (v2.1: 4200)
    ────────────────────────────────
    Total:                ~7.5 KB     (v2.1: ~7 KB)