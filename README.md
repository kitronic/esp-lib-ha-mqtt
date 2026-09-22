# HAMQTT

> مكتبة أردوينو خفيفة للربط السلس مع Home Assistant عبر MQTT Auto-Discovery — مُحسّنة خصيصاً لـ ESP8266

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange.svg)](https://platformio.org/)
[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![Version](https://img.shields.io/badge/version-2.2.0-blue.svg)](https://github.com/kitronic/esp-lib-ha-mqtt)

---

## 📖 نظرة عامة

**HAMQTT** مكتبة أردوينو احترافية تبسّط ربط أجهزة **ESP8266 / ESP32** مع **Home Assistant** عبر **MQTT Auto-Discovery**.

**الإصدار 2.2** يضيف **Device Groups** لتنظيم الكيانات في أجهزة فرعية، و **Read-Only Entities** للعرض فقط — مع الحفاظ على **~4 KB فقط RAM ثابتة** و**0 bytes heap**.

### 🎯 لماذا HAMQTT؟

- 🚀 **بسيط**: أنشئ sensor في سطر واحد
- 💾 **موفّر للذاكرة**: 4 KB فقط
- 🔄 **Auto-Discovery**: Home Assistant يكتشف الأجهزة تلقائياً
- 🧠 **Cache ذكي**: يمنع نشر القيم المتكررة
- 🎨 **6 أنواع كيانات**: sensor, binary_sensor, button, switch, select, number
- 📊 **state_class تلقائي**: دعم Energy Dashboard
- 🔌 **يعمل مع BearSSL**: يوفّر ذاكرة كافية لـ HTTPS
- 🗂️ **Device Groups**: قسّم 80+ entity على أجهزة فرعية
- 👁️ **Read-Only Entities**: اعرض القيم بدون إمكانية التغيير

---

## 📊 مقارنة الأداء

| الميزة | v1.x | v2.0 | v2.1 | v2.2 |
|--------|:----:|:----:|:----:|:----:|
| RAM ثابتة | ~11 KB | ~11 KB | ~4 KB | **~4 KB** |
| Heap usage | 0 | 0 | 0 | **0** |
| String allocations | لا | لا | لا | **لا** |
| يشتغل على ESP8266 | ⚠️ | ❌ | ✅ | ✅ |
| يدعم HTTPS | ❌ | ❌ | ✅ | ✅ |
| Cache ذكي | ❌ | ✅ | ✅ | ✅ |
| Select + Number | ❌ | ✅ | ✅ | ✅ |
| Device Groups | ❌ | ❌ | ❌ | **✅** |
| Read-Only Entities | ❌ | ❌ | ❌ | **✅** |

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
| ✅ **Device Groups** | تنظيم الأجهزة الفرعية |
| ✅ **Read-Only Entities** | للعرض فقط |

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
    ha.setDevice("my_device", "My Device", "Me", "v1", "2.2.0");
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
| `setDevice(id, name, manufacturer, model, version)` | معلومات الجهاز الرئيسي |
| `setStateTopicPrefix(prefix)` | بادئة topics الحالة |
| `setAvailabilityTopic(topic)` | topic التوفر |
| `setDiscoveryPrefix(prefix)` | بادئة discovery (افتراضي: `homeassistant`) |

### 🗂️ Device Groups (جديد في v2.2)

| الدالة | الوصف |
|--------|-------|
| `setGroup(name)` | بدء مجموعة جديدة — الكيانات التالية تنتمي لها |
| `clearGroup()` | إعادة للوضع العادي (جهاز واحد) |

**مثال:**

```cpp
// ═══ Sensors ═══
ha.setGroup("Sensors");
ha.addSensor("battery_voltage", ...);
ha.addSensor("temperature", ...);

// ═══ Controls ═══
ha.setGroup("Controls");
ha.addButton("restart", ...);
ha.addSwitch("relay", ...);

// ═══ Diagnostics ═══
ha.setGroup("Diagnostics");
ha.addSensor("wifi_ip", ...);
ha.addSensor("firmware_version", ...);
```

النتيجة في HA:

- 🏭 My Device (رئيسي)
- 📊 My Device - Sensors
- ⚙️ My Device - Controls
- 🔧 My Device - Diagnostics

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
| `addSelect(id, name, optionsJson, icon, readOnly)` | إضافة select |
| `addNumber(id, name, min, max, step, unit, icon, readOnly)` | إضافة number |

### 👁️ Read-Only Entities (جديد في v2.2)

عند تمرير `readOnly = true`، لن يضيف Home Assistant `command_topic` — يعني المستخدم يشوف القيمة لكن ما يقدر يغيّرها.

```cpp
// ═══ Select قابل للتغيير ═══
ha.addSelect("output_priority", "Output Priority",
             "[\"UtilitySolarBat\",\"SolarUtilityBat\",\"SolarBatUtility\"]");
// readOnly = false (افتراضي)

// ═══ Select للعرض فقط ═══
ha.addSelect("inverter_mode", "Inverter Mode (Read-Only)",
             "[\"Power On\",\"Standby\",\"Line\",\"Battery\"]",
             "mdi:eye",
             true);   // ← readOnly!

// ═══ Number للعرض فقط ═══
ha.addNumber("current_cutoff_display", "Current Cutoff Voltage",
             40.0, 54.0, 0.1, "V",
             "mdi:eye",
             true);   // ← readOnly!
```

### 📤 نشر الحالة

| الدالة | الوصف |
|--------|-------|
| `publishState(id, value, [decimals], [heartbeatMs])` | نشر قيمة (يدعم `const char*`, `float`, `int`, `unsigned int`, `unsigned long`) |
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

### مثال 7: جهاز كامل مع Groups (جديد)

```cpp
ha.setDevice("pip_inverter_03", "PIP Solar Inverter 3",
             "Voltronic", "Axpert/PIP/MAX2", "2.2.0");
ha.setStateTopicPrefix("solar/inverter3");
ha.setAvailabilityTopic("solar/inverter3/status");

// ═══ Sensors ═══
ha.setGroup("Sensors");
ha.addSensor("battery_voltage", "Battery Voltage", "V", "voltage", "mdi:battery", true);
ha.addSensor("battery_percent", "Battery %", "%", "battery", "mdi:battery-70", true);
ha.addSensor("load_power", "Load Power", "W", "power", "mdi:flash", true);
ha.addSensor("solar_power", "Solar Power", "W", "power", "mdi:solar-power", true);

// ═══ Controls ═══
ha.setGroup("Controls");
ha.addButton("check_updates", "Check Updates", "mdi:cloud-download");
ha.addSwitch("auto_update", "Auto Update", "mdi:update");
ha.addNumber("battery_cutoff", "Cut-off Voltage", 40.0, 54.0, 0.1, "V");

// ═══ Diagnostics ═══
ha.setGroup("Diagnostics");
ha.addSensor("wifi_ip", "WiFi IP");
ha.addSensor("firmware_version", "Firmware Version");
ha.addBinarySensor("grid_online", "Grid Online", "plug");

// ═══ Read-Only Display ═══
ha.addSelect("inverter_mode", "Inverter Mode",
             "[\"Power On\",\"Standby\",\"Line\",\"Battery\"]",
             "mdi:eye",
             true);   // ← readOnly
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

### Device Groups (v2.2)

عند استخدام `setGroup("Sensors")`، تتغيّر بنية الـ discovery:

```text
homeassistant/sensor/my_device_Sensors/temperature/config
                            ↑
                      deviceId_groupName
```

النتيجة في HA:

```text
my_device (رئيسي)
  ├── my_device_Sensors
  ├── my_device_Controls
  └── my_device_Diagnostics
```

مع `via_device` لربطهم بالرئيسي.

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
| `HA_GROUP_LEN` | 24 | أقصى طول اسم Group |

### إعدادات موصى بها حسب الجهاز

**ESP8266 (ذاكرة محدودة):**

```ini
build_flags =
    -D HA_MAX_ENTITIES=16
    -D HA_MAX_CACHE=24
    -D HA_MQTT_BUFFER_SIZE=768
```

**ESP8266 مع 80+ كيان:**

```ini
build_flags =
    -D HA_MAX_ENTITIES=80
    -D HA_MAX_CACHE=40
    -D HA_MQTT_BUFFER_SIZE=1024
```

**ESP32 (ذاكرة وفيرة):**

```ini
build_flags =
    -D HA_MAX_ENTITIES=128
    -D HA_MAX_CACHE=128
    -D HA_MQTT_BUFFER_SIZE=2048
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

### المشكلة: Entities مكررة بـ `_2`

**السبب:** تغيير `deviceId` بعد النشر الأول.

**الحل:**

1. امسح الـ retained topics القديمة من MQTT
2. أعد تشغيل HA
3. أعد تشغيل ESP

### المشكلة: Read-Only Select لا يزال قابلاً للتغيير

**السبب:** النسخة القديمة من HA مخزّنة.

**الحل:**

1. امسح retained: `homeassistant/select/<device>/<entity>/config`
2. أعد تشغيل HA

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
sizeof(HAEntity): 140 bytes
sizeof(HAStateCache): 2696 bytes
sizeof(HAMQTT): 4800 bytes
Free heap AFTER WiFi: 32500
Free heap AFTER setup: 27000
Free heap AFTER all: 25000
Max block: 19000
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
- [x] Device Groups (v2.2)
- [x] Read-Only Entities (v2.2)
- [ ] Light entities
- [ ] Cover entities
- [ ] Climate entities
- [ ] Fan entities
- [ ] Web configuration portal
- [ ] Support for ESP32-C3 / S3

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

**v2.2.0** — Groups Edition

</div>