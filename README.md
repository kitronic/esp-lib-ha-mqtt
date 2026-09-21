# HAMQTT

> مكتبة أردوينو للربط السلس مع Home Assistant عبر MQTT Auto-Discovery

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg)](https://platformio.org/)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP8266](https://img.shields.io/badge/ESP8266-Supported-green.svg)]()
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)]()

---

## 📖 نظرة عامة

**HAMQTT** مكتبة خفيفة وقوية تبسّط عملية ربط أجهزة ESP8266 / ESP32 مع **Home Assistant** عبر **MQTT Auto-Discovery**.

بدل ما تكتب JSON يدوياً لكل entity، المكتبة تدير كل شي عنك: إنشاء الكيانات، topics، discovery، توفر الجهاز، واستقبال الأوامر.

### ✨ لماذا HAMQTT؟

- 🚀 **بسيط**: أنشئ sensor في سطر واحد
- 💾 **خفيف**: مصمم لأجهزة ذات ذاكرة محدودة (ESP8266)
- 🔄 **Auto-Discovery**: Home Assistant يكتشف الأجهزة تلقائياً
- 🎯 **مُوثّق**: كل دالة موثقة بالعربية والإنجليزية
- 🔌 **مرن**: يدعم 4 أنواع من الكيانات + قابل للتوسع

---

## ✨ المميزات

| الميزة | الوصف |
|--------|-------|
| ✅ **Auto-Discovery** | Home Assistant يكتشف الجهاز تلقائياً بدون أي إعداد يدوي |
| ✅ **Sensors** | دعم كامل للـ sensors (temperature, power, voltage...) |
| ✅ **Binary Sensors** | للتنبيهات والحالات (on/off) |
| ✅ **Buttons** | أزرار قابلة للضغط من HA |
| ✅ **Switches** | مفاتيح تحكم ثنائية |
| ✅ **Availability Tracking** | Last Will & Testament + online/offline تلقائي |
| ✅ **Command Callbacks** | استقبال أوامر HA بسهولة |
| ✅ **Multi-Platform** | يدعم ESP8266 و ESP32 |
| ✅ **Lightweight** | يستهلك أقل من 15 KB RAM |
| ✅ **Retained Messages** | قابل للتحكم لكل كيان |

---

## 📦 التثبيت

### الطريقة 1: PlatformIO (موصى به)

أضف السطر التالي إلى `platformio.ini`:

```ini
lib_deps =
    username/HAMQTT
