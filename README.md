# HAMQTT

> مكتبة أردوينو خفيفة للربط السلس مع Home Assistant عبر MQTT Auto-Discovery — مُحسّنة خصيصاً لـ ESP8266

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg)](https://platformio.org/)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP8266](https://img.shields.io/badge/ESP8266-Supported-green.svg)]()
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)]()
[![Version](https://img.shields.io/badge/version-2.1.0-blue.svg)]()

---

## 📖 نظرة عامة

**HAMQTT** مكتبة أردوينو احترافية تبسّط ربط أجهزة **ESP8266 / ESP32** مع **Home Assistant** عبر **MQTT Auto-Discovery**.

**الإصدار 2.1** مُحسّن بالكامل للذاكرة — يستهلك **~4 KB فقط RAM ثابتة** و**0 bytes heap**، مما يجعله مثالياً للأجهزة ذات الذاكرة المحدودة.

### 🎯 لماذا HAMQTT؟

- 🚀 **بسيط**: أنشئ sensor في سطر واحد
- 💾 **موفّر للذاكرة**: 4 KB فقط (مقابل 11 KB في الإصدارات السابقة)
- 🔄 **Auto-Discovery**: Home Assistant يكتشف الأجهزة تلقائياً
- 🧠 **Cache ذكي**: يمنع نشر القيم المتكررة
- 🎨 **6 أنواع كيانات**: sensor, binary_sensor, button, switch, select, number
- 📊 **state_class تلقائي**: دعم Energy Dashboard
- 🔌 **يعمل مع BearSSL**: يوفّر ذاكرة كافية لـ HTTPS

---

## 📊 مقارنة الأداء

| الميزة | v1.x | v2.0 | **v2.1** |
|--------|------|------|----------|
| RAM ثابتة | ~11 KB | ~11 KB | **~4 KB** |
| Heap usage | 0 | 0 | **0** |
| String allocations | لا | لا | **لا** |
| يشتغل على ESP8266 (15 KB) | ⚠️ | ❌ | ✅ |
| يدعم HTTPS (مع BearSSL) | ❌ | ❌ | ✅ |
| Cache ذكي | ❌ | ✅ | ✅ |
| Select + Number | ❌ | ✅ | ✅ |

---

## ✨ المميزات

| الميزة | الوصف |
|--------|-------|
| ✅ **Sensors** | temperature, humidity, power, voltage, energy, battery... |
| ✅ **Binary Sensors** | motion, door, plug, battery, problem... |
| ✅ **Buttons** | أزرار قابلة للضغط من HA |
| ✅ **Switches** | مفاتيح تحكم ثنائية |
| ✅ **Selects** | قوائم اختيار |
| ✅ **Numbers** | إدخال قيم رقمية مع min/max/step |
| ✅ **Auto-Discovery** | كشف تلقائي في Home Assistant |
| ✅ **Smart Cache** | منع نشر القيم المتكررة (توفير bandwidth) |
| ✅ **Heartbeat** | تجديد retained messages تلقائياً |
| ✅ **Availability** | Last Will + online/offline تلقائي |
| ✅ **state_class** | للـ Energy Dashboard و Statistics |
| ✅ **PROGMEM** | تخزين strings في Flash تلقائياً |
| ✅ **Zero Heap** | لا String، لا new، لا malloc |
| ✅ **Multi-Platform** | ESP8266 + ESP32 |

---

## 📦 التثبيت

### الطريقة 1: PlatformIO (موصى به)

أضف إلى `platformio.ini`:

```ini
lib_deps =
    kitronic/esp-lib-ha-mqtt@^2.1.0
    knolleary/PubSubClient@^2.8

lib_deps =
    https://github.com/kitronic/esp-lib-ha-mqtt.git
    knolleary/PubSubClient@^2.8
