# HAMQTT

> مكتبة أردوينو خفيفة للربط السلس مع Home Assistant عبر MQTT Auto-Discovery — مُحسّنة خصيصاً لـ ESP8266

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg)](https://platformio.org/)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![Version](https://img.shields.io/badge/version-2.1.0-blue.svg)](https://github.com/kitronic/esp-lib-ha-mqtt)

---

## 📖 نظرة عامة

**HAMQTT** مكتبة أردوينو احترافية تبسّط ربط أجهزة **ESP8266 / ESP32** مع **Home Assistant** عبر **MQTT Auto-Discovery**.

**الإصدار 2.1** مُحسّن بالكامل للذاكرة — يستهلك **~4 KB فقط RAM ثابتة** و**0 bytes heap**.

### 🎯 لماذا HAMQTT؟

- 🚀 **بسيط**: أنشئ sensor في سطر واحد
- 💾 **موفّر للذاكرة**: 4 KB فقط
- 🔄 **Auto-Discovery**: Home Assistant يكتشف الأجهزة تلقائياً
- 🧠 **Cache ذكي**: يمنع نشر القيم المتكررة
- 🎨 **6 أنواع كيانات**: sensor, binary_sensor, button, switch, select, number
- 📊 **state_class تلقائي**: دعم Energy Dashboard
- 🔌 **يعمل مع BearSSL**: يوفّر ذاكرة كافية لـ HTTPS

---

## 📊 مقارنة الأداء

| الميزة | v1.x | v2.0 | v2.1 |
|--------|:----:|:----:|:----:|
| RAM ثابتة | ~11 KB | ~11 KB | **~4 KB** |
| Heap usage | 0 | 0 | **0** |
| String allocations | لا | لا | **لا** |
| يشتغل على ESP8266 | ⚠️ | ❌ | ✅ |
| يدعم HTTPS | ❌ | ❌ | ✅ |
| Cache ذكي | ❌ | ✅ | ✅ |
| Select + Number | ❌ | ✅ | ✅ |

---

## ✨ المميزات

| الميزة | الوصف |
|--------|-------|
| ✅ Sensors | temperature, humidity, power, voltage |
| ✅ Binary Sensors | motion, door, plug, battery |
| ✅ Buttons | أزرار قابلة للضغط من HA |
| ✅ Switches | مفاتيح تحكم ثنائية |
| ✅ Selects | قوائم اختيار |
| ✅ Numbers | إدخال قيم رقمية |
| ✅ Auto-Discovery | كشف تلقائي في HA |
| ✅ Smart Cache | منع نشر القيم المتكررة |
| ✅ Heartbeat | تجديد retained messages |
| ✅ Availability | Last Will + online/offline |
| ✅ state_class | للـ Energy Dashboard |
| ✅ PROGMEM | strings في Flash |
| ✅ Zero Heap | لا String، لا new |
| ✅ Multi-Platform | ESP8266 + ESP32 |

---

## 📦 التثبيت

### PlatformIO

```ini
lib_deps =
    https://github.com/kitronic/esp-lib-ha-mqtt.git
    knolleary/PubSubClient@^2.8
```

### Arduino IDE

1. حمّل المستودع كملف ZIP من [GitHub](https://github.com/kitronic/esp-lib-ha-mqtt)
2. من القائمة: **Sketch → Include Library → Add .ZIP Library**
3. ثبّت مكتبة **PubSubClient** من Library Manager

---

## 🚀 مثال سريع

```cpp
#include <ESP8266WiFi.h>
#include <HAMQTT.h>

#define WIFI_SSID    "YOUR_WIFI"
#define WIFI_PASS    "YOUR_PASS"
#define MQTT_SERVER  "192.168.1.100"

WiFiClient wifiClient;
HAMQTT ha(&wifiClient);

void setup() {
    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) delay(500);

    ha.setServer(MQTT_SERVER, 1883);
    ha.setDevice("my_device", "My Device", "Me", "v1", "2.1.0");
    ha.setStateTopicPrefix("home/my_device");
    ha.setAvailabilityTopic("home/my_device/status");

    ha.addSensor("temperature", "Temperature", "°C", "temperature", "mdi:thermometer", true);
    ha.addBinarySensor("motion", "Motion", "motion");
    ha.addButton("restart", "Restart", "mdi:restart");
    ha.addSwitch("relay", "Relay", "mdi:power");
    ha.addSelect("mode", "Mode", "[\"Auto\",\"Manual\"]");
    ha.addNumber("threshold", "Threshold", 0.0, 100.0, 1.0, "%");

    ha.onCommand([](const char* topic, const char* payload) {
        Serial.printf("Command: %s => %s\n", topic, payload);
    });

    ha.begin();
    ha.publishDiscovery();
}

void loop() {
    ha.loop();

    static unsigned long last = 0;
    if (millis() - last > 5000) {
        last = millis();
        ha.publishState("temperature", 22.5f, 1);
        ha.publishBinaryState("motion", false);
    }
}
```

---

## 📖 الدوال (API Reference)

### 🔧 الإعداد الأولي

| الدالة | الوصف |
|--------|-------|
| `HAMQTT(Client* networkClient)` | Constructor - يمرّر WiFiClient |
| `setServer(server, port, user, pass)` | إعدادات MQTT Broker |
| `setDevice(id, name, manufacturer, model, version)` | معلومات الجهاز |
| `setStateTopicPrefix(prefix)` | بادئة topics الحالة |
| `setAvailabilityTopic(topic)` | topic التوفر |
| `setDiscoveryPrefix(prefix)` | بادئة discovery (افتراضي: `homeassistant`) |

### 🔄 دورة الحياة

| الدالة | الوصف |
|--------|-------|
| `begin()` | يبدأ الاتصال بـ MQTT |
| `loop()` | يُستدعى في loop الرئيسي |
| `connected()` | هل MQTT متصل؟ |
| `reconnect()` | إعادة الاتصال يدوياً |

### ➕ إضافة الكيانات

| الدالة | الوصف |
|--------|-------|
| `addSensor(id, name, unit, deviceClass, icon, stateClass)` | إضافة sensor |
| `addBinarySensor(id, name, deviceClass, icon)` | إضافة binary_sensor |
| `addButton(id, name, icon)` | إضافة button |
| `addSwitch(id, name, icon)` | إضافة switch |
| `addSelect(id, name, optionsJson, icon)` | إضافة select |
| `addNumber(id, name, min, max, step, unit, icon)` | إضافة number |

### 📤 نشر الحالة

| الدالة | الوصف |
|--------|-------|
| `publishState(id, value, [decimals], [heartbeatMs])` | نشر قيمة |
| `publishBinaryState(id, bool, [heartbeatMs])` | نشر ON/OFF |
| `publishRaw(topic, value, [retain], [heartbeatMs])` | نشر مباشر |

### 📥 استقبال الأوامر

| الدالة | الوصف |
|--------|-------|
| `onCommand(callback)` | تسجيل دالة استقبال الأوامر |

### 📡 Discovery & Availability

| الدالة | الوصف |
|--------|-------|
| `publishDiscovery()` | نشر كل الكيانات لـ HA |
| `publishDiscovery(entityId)` | نشر كيان واحد |
| `publishAvailable()` | نشر `online` |
| `publishUnavailable()` | نشر `offline` |

### 🧹 أدوات مساعدة

| الدالة | الوصف |
|--------|-------|
| `clearCache()` | تفريغ cache |
| `entityCount()` | عدد الكيانات المسجلة |
| `client()` | الوصول للـ PubSubClient الأصلي |

---

## 🎨 أمثلة مفصّلة

### مثال 1: Sensor لدرجة الحرارة

```cpp
ha.addSensor("temperature", "Room Temperature", "°C", "temperature", "mdi:thermometer", true);
ha.publishState("temperature", 24.5f, 1);
```

### مثال 2: Binary Sensor للحركة

```cpp
ha.addBinarySensor("motion", "Motion Detected", "motion");
ha.publishBinaryState("motion", true);
```

### مثال 3: زر إعادة التشغيل

```cpp
ha.addButton("restart", "Restart Device", "mdi:restart");

ha.onCommand([](const char* topic, const char* payload) {
    if (strstr(topic, "restart")) {
        ESP.restart();
    }
});
```

### مثال 4: Switch للتحكم بالريلاي

```cpp
ha.addSwitch("relay", "Main Relay", "mdi:power");

ha.onCommand([](const char* topic, const char* payload) {
    if (strstr(topic, "relay")) {
        bool on = (strcmp(payload, "ON") == 0);
        digitalWrite(RELAY_PIN, on ? HIGH : LOW);
        ha.publishBinaryState("relay", on);
    }
});
```

### مثال 5: Select للاختيار من قائمة

```cpp
ha.addSelect("inverter_mode", "Inverter Mode",
             "[\"Power On\",\"Standby\",\"Line\",\"Battery\"]",
             "mdi:cog");

ha.onCommand([](const char* topic, const char* payload) {
    if (strstr(topic, "inverter_mode")) {
        Serial.printf("Mode set to: %s\n", payload);
        ha.publishState("inverter_mode", payload);
    }
});
```

> ⚠️ **مهم:** `optionsJson` يجب أن يكون string literal.

### مثال 6: Number لضبط قيمة

```cpp
ha.addNumber("battery_cutoff", "Set Cut-off Voltage",
             40.0, 54.0, 0.1, "V", "mdi:battery-minus");

ha.onCommand([](const char* topic, const char* payload) {
    if (strstr(topic, "battery_cutoff")) {
        float v = atof(payload);
        setBatteryCutoff(v);
        ha.publishState("battery_cutoff", v, 1);
    }
});
```

### مثال 7: جهاز كامل (إنفرتر شمسي)

```cpp
ha.setDevice("pip_inverter_03", "PIP Solar Inverter 3",
             "Voltronic", "Axpert/PIP/MAX2", "2.1.0");
ha.setStateTopicPrefix("solar/inverter3");
ha.setAvailabilityTopic("solar/inverter3/status");

ha.addSensor("battery_voltage", "Battery Voltage", "V", "voltage", "mdi:battery", true);
ha.addSensor("battery_percent", "Battery %", "%", "battery", "mdi:battery-70", true);
ha.addSensor("load_power", "Load Power", "W", "power", "mdi:flash", true);
ha.addSensor("solar_power", "Solar Power", "W", "power", "mdi:solar-power", true);

ha.addBinarySensor("grid_online", "Grid Online", "plug");
ha.addBinarySensor("battery_low", "Battery Low", "battery");

ha.addButton("check_updates", "Check Updates", "mdi:cloud-download");
ha.addSwitch("auto_update", "Auto Update", "mdi:update");
ha.addSelect("inverter_mode", "Inverter Mode",
             "[\"Power On\",\"Standby\",\"Line\",\"Battery\"]");
ha.addNumber("battery_cutoff", "Cut-off Voltage", 40.0, 54.0, 0.1, "V");
```

---

## 🎨 Device Classes المدعومة

### Sensors

| الفئة | Device Classes |
|-------|----------------|
| حرارة | `temperature` |
| رطوبة | `humidity`, `moisture` |
| طاقة | `power`, `energy`, `apparent_power`, `reactive_power` |
| كهرباء | `voltage`, `current`, `battery` |
| بيئة | `pressure`, `illuminance`, `gas` |
| زمن | `duration`, `timestamp` |
| متنوع | `frequency`, `speed`, `weight`, `distance` |

### Binary Sensors

`motion`, `door`, `window`, `smoke`, `moisture`, `plug`, `battery`, `power`, `problem`, `connectivity`, `opening`, `vibration`, `sound`

---

## 🏗️ كيف يعمل Auto-Discovery؟

المكتبة تتبع [MQTT Discovery Protocol](https://www.home-assistant.io/docs/mqtt/discovery/) من Home Assistant.

### Topics الـ Discovery

```text
homeassistant/
├── sensor/
│   └── my_device/
│       └── temperature/config
├── binary_sensor/
│   └── my_device/
│       └── motion/config
├── button/
│   └── my_device/
│       └── restart/config
├── switch/
│   └── my_device/
│       └── relay/config
├── select/
│   └── my_device/
│       └── mode/config
└── number/
    └── my_device/
        └── threshold/config
```

### Topics الحالة

```text
home/my_device/
├── temperature/state
├── motion/state
├── relay/state
├── mode/state
└── threshold/state
```

### Topics الأوامر

```text
home/my_device/
├── restart/set
├── relay/set
├── mode/set
└── threshold/set
```

### Topics التوفر

```text
home/my_device/status     → online / offline
```

---

## ⚙️ إعدادات متقدمة

### تخصيص حجم الذاكرة

يمكنك تعديل حدود المكتبة من `platformio.ini`:

```ini
build_flags =
    -D HA_MAX_ENTITIES=16
    -D HA_MAX_CACHE=24
    -D HA_MQTT_BUFFER_SIZE=768
```

### الإعدادات المتاحة

| الإعداد | افتراضي | الوصف |
|---------|:-------:|-------|
| `HA_MAX_ENTITIES` | 16 | أقصى عدد كيانات |
| `HA_MAX_CACHE` | 24 | أقصى عناصر cache |
| `HA_MQTT_BUFFER_SIZE` | 768 | حجم buffer MQTT |
| `HA_DISCOVERY_BUF` | 768 | حجم buffer discovery |
| `HA_DEFAULT_HEARTBEAT` | 60000 | heartbeat افتراضي (ms) |

### إعدادات موصى بها حسب الجهاز

**ESP8266 (ذاكرة محدودة):**

```ini
build_flags =
    -D HA_MAX_ENTITIES=16
    -D HA_MAX_CACHE=24
    -D HA_MQTT_BUFFER_SIZE=768
```

**ESP32 (ذاكرة وفيرة):**

```ini
build_flags =
    -D HA_MAX_ENTITIES=48
    -D HA_MAX_CACHE=64
    -D HA_MQTT_BUFFER_SIZE=2048
```

**ESP8266 مع BearSSL (HTTPS):**

```ini
build_flags =
    -D HA_MAX_ENTITIES=12
    -D HA_MAX_CACHE=16
    -D HA_MQTT_BUFFER_SIZE=512
```

### تغيير Heartbeat

```cpp
ha.publishState("temperature", 24.5f, 1, 30000);
```

---

## 🐛 حل المشاكل

### المشكلة: الجهاز لا يظهر في Home Assistant

**الحل:**

1. تأكد أن MQTT Integration مُفعّل: **Settings → Devices & Services → MQTT**
2. تأكد من `setDiscoveryPrefix("homeassistant")`
3. افتح MQTT Explorer وافحص topic `homeassistant/sensor/...`
4. أعد تشغيل Home Assistant

### المشكلة: MQTT Connection failed

**الحل:**

1. تأكد من IP الـ broker
2. تحقق من username/password
3. في HA: **Settings → People** → أنشئ user للـ MQTT

### المشكلة: نفاد الذاكرة (Out of Memory)

**الحل:**

1. قلّل `HA_MAX_ENTITIES` و `HA_MAX_CACHE`
2. أزل الكيانات غير المستخدمة
3. راقب `ESP.getFreeHeap()` بانتظام

### المشكلة: Select crash عند الإقلاع

**السبب:** `optionsJson` مؤقت (ليس string literal)

**الحل:**

```cpp
// ✅ صح
ha.addSelect("mode", "Mode", "[\"A\",\"B\"]");

// ❌ خطأ
String s = "[\"A\",\"B\"]";
ha.addSelect("mode", "Mode", s.c_str());
```

### المشكلة: بعض الكيانات لا تنشر حالتها

**الحل:**

- Cache قد يمنع النشر إذا القيمة ما تغيّرت — هذا طبيعي
- للـ force publish: `ha.clearCache()` قبل النشر

---

## 🧪 قياس الذاكرة

أضف في `setup()`:

```cpp
Serial.printf("Free heap: %u\n", ESP.getFreeHeap());
Serial.printf("Max block: %u\n", ESP.getMaxFreeBlockSize());
Serial.printf("sizeof(HAEntity): %u\n", sizeof(HAEntity));
Serial.printf("sizeof(HAStateCache): %u\n", sizeof(HAStateCache));
Serial.printf("sizeof(HAMQTT): %u\n", sizeof(HAMQTT));
Serial.printf("Entities count: %u\n", ha.entityCount());
```

النتائج المتوقعة على ESP8266:

```text
Free heap BEFORE: 45200
sizeof(HAEntity): 116 bytes
sizeof(HAStateCache): 2696 bytes
sizeof(HAMQTT): 4200 bytes
Free heap AFTER WiFi: 32500
Free heap AFTER setup: 29800
Free heap AFTER all: 28400
Max block: 22100
```

---

## 🔬 مشاريع تستخدم HAMQTT

- 🌞 **PIP Solar Inverter** — مراقبة وتحكم بانفرتر شمسي
- 🏠 **Smart Home Hub** — مركز تحكم منزلي
- 🌡️ **Weather Station** — محطة طقس منزلية
- 💡 **Smart Lighting** — إضاءة ذكية

هل تستخدم HAMQTT؟ [افتح issue](https://github.com/kitronic/esp-lib-ha-mqtt/issues) وأخبرنا!

---

## 🤝 المساهمة

1. Fork المستودع
2. أنشئ فرع: `git checkout -b feature/amazing-feature`
3. Commit: `git commit -m "Add amazing feature"`
4. Push: `git push origin feature/amazing-feature`
5. افتح Pull Request

### قواعد المساهمة

- ✅ اتبع نمط الكود الموجود
- ✅ وثّق أي دالة جديدة
- ✅ أضف مثال إذا كانت ميزة جديدة
- ✅ اختبر على ESP8266 و ESP32
- ✅ راقب استهلاك الذاكرة

---

## 📋 خارطة الطريق (Roadmap)

- [x] Sensors
- [x] Binary Sensors
- [x] Buttons
- [x] Switches
- [x] Selects
- [x] Numbers
- [x] Smart Cache
- [x] Zero Heap
- [x] PROGMEM support
- [ ] Light entities
- [ ] Cover entities
- [ ] Climate entities
- [ ] Fan entities
- [ ] Web configuration portal
- [ ] Support for ESP32-C3 / S3
- [ ] Device Groups

---

## 📄 الترخيص

هذا المشروع مرخّص تحت **MIT License** — راجع [LICENSE](LICENSE) للتفاصيل.

---

## 👤 المؤلف

**Kitronic**

- GitHub: [@kitronic](https://github.com/kitronic)
- Email: [info@kitronic.tech](mailto:info@kitronic.tech)

---

## ⭐ دعم المشروع

إذا أفادتك المكتبة:

- ⭐ أعطِ **Star** للمستودع
- 🐛 افتح **Issue** عند وجود مشكلة
- 💬 شاركها مع مجتمع ESP و Home Assistant
- 🤝 ساهم في تطويرها

---

## 🔗 روابط مفيدة

- [Home Assistant MQTT Discovery](https://www.home-assistant.io/docs/mqtt/discovery/)
- [PubSubClient Library](https://github.com/knolleary/pubsubclient)
- [PlatformIO Registry](https://registry.platformio.org/)
- [ESP8266 Arduino Core](https://github.com/esp8266/Arduino)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)

---

<div align="center">

**صُنع بـ ❤️ لمجتمع Home Assistant العربي**

Made with ❤️ for the Home Assistant community

**v2.1.0** — Lightweight Edition

</div>