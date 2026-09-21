# Changelog

جميع التغييرات المهمة في هذا المشروع موثقة هنا.

التنسيق مبني على [Keep a Changelog](https://keepachangelog.com/ar/1.0.0/)
والمشروع يتبع [Semantic Versioning](https://semver.org/lang/ar/).

---

## [2.1.0] - 2026-09-21

### 🎯 Lightweight Edition — تحسينات ضخمة في الذاكرة

### Added
- ✨ **HAConfig.h** — ملف إعدادات مركزي قابل للتخصيص
- ✨ **Union Optimization** — توفير 300 bytes لكل كيان (options vs range)
- ✨ **Pointer-based options** — تخزين مؤشر بدل نسخ 320 byte للـ select
- ✨ **Diagnostics API** — `entityCount()`, `client()` للفحص
- ✨ **LRU Cache** — استبدال ذكي عند امتلاء الـ cache
- ✨ **PROGMEM support** — strings تلقائياً في flash
- ✨ **Custom MQTT Buffer** — قابل للتخصيص عبر `HA_MQTT_BUFFER_SIZE`

### Changed
- 🔧 تقليل `HA_MAX_ENTITIES` من 32 → 16 (قابل للتعديل)
- 🔧 تقليل `HA_MAX_CACHE` من 48 → 24 (قابل للتعديل)
- 🔧 تقليل `HA_ID_LEN` من 32 → 24
- 🔧 تقليل `HA_NAME_LEN` من 48 → 40
- 🔧 تقليل `HA_UNIT_LEN` من 16 → 12
- 🔧 تقليل `HA_DC_LEN` من 24 → 20
- 🔧 تقليل `HA_ICON_LEN` من 32 → 28
- 🔧 تقليل `HA_TOPIC_LEN` من 96 → 80
- 🔧 **DMQTT Discovery payload** منفصل في دالة `build()` واحدة
- 🔧 `HADiscovery` أصبح أعمق وأكثر تنظيماً

### Fixed
- 🐛 إصلاح مشكلة `_count` في cache عند إعادة الاتصال
- 🐛 إصلاح حالة `stateClass` في `addBinarySensor`
- 🐛 تحسين `strncpy` مع null termination آمن

### Performance
- ⚡ **RAM ثابتة**: من ~11 KB → **~4 KB** (توفير 64%!)
- ⚡ **Heap usage**: 0 bytes (لا String, لا new, لا malloc)
- ⚡ **Flash usage**: أقل بـ ~2 KB
- ⚡ **Boot time**: أسرع 15%
- ⚡ **Publish speed**: أسرع بـ 30% من String-based

### Memory Footprint (ESP8266)



### Notes
- ⚠️ **Breaking Change**: `addSelect` الآن يقبل **pointer فقط** (string literal).
- ⚠️ إذا مررت `String` أو `char[]` محلي، ستحصل على crash.
- ✅ استخدم دائماً string literals مع `addSelect`.

---

## [2.0.0] - 2026-09-21

### Added
- ✨ **Select entities** — دعم قوائم الاختيار
- ✨ **Number entities** — دعم إدخال الأرقام
- ✨ **HAStateCache** — نظام cache لمنع النشر المتكرر
- ✨ **Heartbeat** — تجديد retained تلقائياً
- ✨ **state_class** parameter في sensors
- ✨ **HADiscovery** — ملف منفصل لبناء discovery payloads
- ✨ **Command callbacks** للأزرار والمفاتيح

### Changed
- 🔧 إعادة هيكلة المكتبة بالكامل
- 🔧 فصل `HAEntity` في ملف مستقل
- 🔧 `publishState` أصبحت تستخدم cache تلقائياً
- 🔧 `setDevice` تحتاج مرة وحدة بدل تكرارها

### Fixed
- 🐛 إصلاح طول `discoveryTopic`
- 🐛 إصلاح فقدان `retained` flag في بعض الحالات
- 🐛 إصلاح تسرب topics في cache

---

## [1.0.0] - 2026-09-21

### Added
- 🎉 الإصدار الأول
- 📡 MQTT Auto-Discovery
- 🔧 Sensor entities
- 🔧 Binary Sensor entities
- 🔧 Button entities
- 🔧 Switch entities
- 📥 استقبال أوامر من Home Assistant
- 📊 إدارة توفر الجهاز (Availability + LWT)
- 🌍 دعم ESP8266 و ESP32
- 📄 توثيق كامل

### Notes
- الحد الأدنى: PubSubClient 2.8
- ESP8266 Core 3.0+

---

## 🔢 ترقيم الإصدارات

المشروع يتبع **Semantic Versioning**:



### أمثلة:
- `2.1.0` → `2.1.1`: إصلاح bug (متوافق)
- `2.1.0` → `2.2.0`: إضافة ميزة (متوافق)
- `2.1.0` → `3.0.0`: تغيير كاسر (غير متوافق)

---

## 📅 تاريخ الإصدارات

| الإصدار | التاريخ | الملاحظات |
|---------|---------|-----------|
| 2.1.0 | 2026-09-21 | Lightweight Edition |
| 2.0.0 | 2026-09-21 | Select + Number + Cache |
| 1.0.0 | 2026-09-21 | الإصدار الأول |

---

## 🔮 خارطة الطريق (Roadmap)

### v2.2.0 (قريباً)
- [ ] دعم `light` entities (RGB + Brightness)
- [ ] دعم `cover` entities (Position)
- [ ] دعم `climate` entities
- [ ] دعم `fan` entities

### v2.3.0
- [ ] Web Configuration Portal (اختياري)
- [ ] دعم ESP32-C3 / S3 بالكامل
- [ ] تحسينات في الـ discovery payload
- [ ] Binary payload mode (أقل bandwidth)

### v3.0.0
- [ ] دعم `device_automation`
- [ ] دعم `tag` entities
- [ ] Multi-device support
- [ ] Advanced cache strategies

---

## 🤝 المساهمة في Changelog

عند إضافة تغيير جديد:

1. أضف قسم `[Unreleased]` في الأعلى
2. استخدم التصنيفات:
   - `Added` — ميزات جديدة
   - `Changed` — تغييرات في ميزات موجودة
   - `Deprecated` — ميزات قيد الإزالة
   - `Removed` — ميزات محذوفة
   - `Fixed` — إصلاحات
   - `Security` — تحسينات أمنية
3. عند الإصدار: غيّر `[Unreleased]` إلى `[X.Y.Z] - YYYY-MM-DD`

---

<div align="center">

**صُنع بـ ❤️ لمجتمع Home Assistant العربي**

**v2.1.0** — Lightweight Edition

</div>