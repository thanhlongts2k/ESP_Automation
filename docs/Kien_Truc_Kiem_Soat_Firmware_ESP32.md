# Tài Liệu Mô Tả Kiến Trúc & Chi Tiết Kỹ Thuật Firmware C++ ESP32 (ESP_Automation)

Tài liệu này phân tích chi tiết sơ đồ cấu trúc, luồng xử lý dữ liệu và thiết kế kiến trúc kỹ thuật bên trong bộ mã nguồn **Firmware ESP32** của dự án **ESP_Automation**.

---

## 🏗️ 1. CẤU TRÚC THƯ MỤC & 2 PHƯƠNG THỨC KHỞI CHẠY FIRMWARE

Mã nguồn Firmware được thiết kế linh hoạt hỗ trợ 2 môi trường phát triển:

```text
firmware/
├── ESP_Automation.ino           # [Phương án 1] File đơn dạng Sketch cho Arduino IDE (Quick Flash)
├── config.h                     # File cấu hình chung cho Arduino IDE
├── web_dashboard.h              # Giao diện Web HTML Glassmorphism trong PROGMEM
└── src/                         # [Phương án 2] Kiến trúc C++ Modular Enterprise cho PlatformIO
    ├── main.cpp                 # Entrypoint chính (Vòng lặp setup & loop không hoãn phi bất đồng bộ)
    ├── config.h                 # Cấu hình trung tâm (Biến môi trường, Pinouts, SSL & Topics)
    ├── wifi_manager.h/.cpp      # Module quản lý Wi-Fi tự khôi phục ngầm phi bất đồng bộ
    ├── sensor_manager.h/.cpp    # Module đọc & cache dữ liệu DHT22 + Độ ẩm đất Analog
    ├── relay_controller.h/.cpp  # Module điều khiển ngắt Relay & Thuật toán Hysteresis nhiệt độ
    ├── mqtt_manager.h/.cpp      # Module giao tiếp MQTT Cloud, SSL/TLS Proxy 8883, LWT & Backoff
    └── web_dashboard.h          # Giao diện Web Dashboard PROGMEM nhúng
```

---

## 🔄 2. SƠ ĐỒ TƯƠNG TÁC GIỮA CÁC MODULE (COMPONENT INTERACTION DIAGRAM)

```mermaid
graph TD
    subgraph PHẦN CỨNG (HARDWARE)
        DHT22[Cảm biến DHT22 GPIO 23] -->|Digital 1-wire| SensorMgr
        SOIL[Cảm biến đất GPIO 34] -->|Analog ADC 0-4095| SensorMgr
        RelayCtrl -->|GPIO 18 LOW Active| RELAY1[Relay 1: Đèn 💡]
        RelayCtrl -->|GPIO 19 LOW Active| RELAY2[Relay 2: Quạt 🌀]
    end

    subgraph NGUYÊN LÝ MODULE C++ (FIRMWARE CORE)
        SensorMgr[SensorManager<br>Đọc & Cache 2s] -->|Temperature, Humidity, Soil| MainLoop
        MainLoop[main.cpp<br>Loop Non-blocking] -->|Check Temp >= 32°C / <= 29°C| RelayCtrl[RelayController<br>Logic Hysteresis]
        WiFiMgr[WiFiManager<br>Tự kết nối lại 5s] -->|Check Status| MainLoop
        MainLoop -->|Push Telemetry JSON 5s| MqttMgr[MQTTManager<br>PubSubClient]
        MainLoop -->|Handle HTTP Requests| WebServer[WebServer Port 80]
    end

    subgraph KẾT NỐI BÊN NGOÀI (EXTERNAL INTERFACES)
        MqttMgr <-->|SSL/TLS 8883 / TCP 1883<br>Topics: esp32/DEVICE_ID/...| NGINX[Nginx SSL Proxy / MQTT Broker]
        WebServer <-->|HTTP GET /api/data<br>HTTP GET /api/relay1,2| App[Flutter Mobile App / Browser]
    end
```

---

## 🧩 3. PHÂN TÍCH CHI TIẾT 5 MODULE C++ CỐT LÕI (`firmware/src/`)

### 1. ⚙️ `config.h` - Cấu hình trung tâm
- **Định danh**: Định nghĩa `DEVICE_ID` (mặc định `"ESP32_Automation_01"`), `FIRMWARE_VERSION` (`"1.0.0"`), `HOSTNAME` (`"esp32-automation"`).
- **Topic Động theo `DEVICE_ID`**:
  - `getTopicSensors()` ➔ `esp32/{DEVICE_ID}/sensors`
  - `getTopicStatus()` ➔ `esp32/{DEVICE_ID}/status`
  - `getTopicRelay1Control()` ➔ `esp32/{DEVICE_ID}/control/relay1`
  - `getTopicRelay2Control()` ➔ `esp32/{DEVICE_ID}/control/relay2`
  - `getTopicOTATrigger()` ➔ `esp32/{DEVICE_ID}/system/ota_trigger`
- **Cấu hình Pinout Chân đế 30P**: GPIO 23 (DHT22), GPIO 34 (Soil Analog), GPIO 18 (Relay 1), GPIO 19 (Relay 2).
- **Mức kích Relay**: `RELAY_ACTIVE_LEVEL = LOW` (0V đóng Relay, 5V ngắt Relay).

### 2. 📶 `WiFiManager` - Quản lý kết nối Wi-Fi phi bất đồng bộ
- **Thuật toán**: Sử dụng `millis()` kiểm tra trạng thái định kỳ 5 giây, tuyệt đối không dùng `delay()` làm treo luồng chương trình.
- **Tự động khôi phục**: Khi sóng Wi-Fi bị đứt, `WiFiManager` ngầm kích hoạt `WiFi.begin()` lại mà không ảnh hưởng tới hoạt động ngắt Relay local.

### 3. 🌡️ `SensorManager` - Đọc & Cache Cảm biến
- **Cảm biến DHT22**: Đọc nhiệt độ (°C) và độ ẩm không khí (%). Kiểm tra cờ `isnan()` để lọc các giá trị lỗi rác.
- **Cảm biến độ ẩm đất Analog (GPIO 34 ADC1)**: Đọc điện áp ADC 12-bit (0 - 4095) và quy đổi tuyến tính sang phần trăm `map(raw, 4095, 1500, 0, 100)` với hàm giới hạn `constrain(0, 100)`.

### 4. ⚡ `RelayController` - Điều khiển Rơ-le & Thuật toán Hysteresis
- **Điều khiển độc lập**: `setRelay1(bool)` và `setRelay2(bool)` điều khiển chân GPIO 18 & 19.
- **Thuật toán Hysteresis (Chống nhấp nháy rơ-le)**:
  - Khi Nhiệt độ $\ge 32.0^\circ\text{C}$: Tự động BẬT Quạt (Relay 2).
  - Khi Nhiệt độ $\le 29.0^\circ\text{C}$: Tự động TẮT Quạt (Relay 2).
  - Khoảng vệt 3.0°C đệm giữa 29°C và 32°C ngăn rơ-le bị đóng/ngắt liên tục khi nhiệt độ dao động quanh ngưỡng.

### 5. 🔒 `MQTTManager` - Giao tiếp Cloud SSL/TLS & LWT Di chúc
- **Kết nối mã hóa Nginx Proxy SSL (Port 8883)**: Kích hoạt `_sslClient.setInsecure()` khi `MQTT_USE_SSL` bật, giúp ESP32 kết nối mượt mà qua Nginx SSL Stream Proxy mà không rớt handshake.
- **LWT Status (Di chúc tự động)**:
  - Khi ngắt nguồn đột ngột ➔ MQTT Broker tự bắn `{"status":"offline","device_id":"ESP32_Automation_01"}` dạng Retained.
  - Khi vừa kết nối lại ➔ ESP32 tự bắn `{"status":"online","device_id":"ESP32_Automation_01","version":"1.0.0"}` Retained.
- **Exponential Backoff**: Khi rớt kết nối MQTT, interval tự nhân đôi (2s ➔ 4s ➔ 8s ➔ max 60s) để tránh làm nghẽn hạ tầng Server.

---

## 🌐 4. LOCAL WEB SERVER & HTTP REST API (`web_dashboard.h` & `main.cpp`)

ESP32 tự chạy 1 HTTP Web Server tại **Port 80**:

1. **Trang chủ (`GET /`)**: Trả về giao diện HTML5/CSS3/JS Dark Mode Glassmorphism nhúng sẵn trong bộ nhớ flash `PROGMEM` để trình duyệt điện thoại/PC kết nối trực tiếp.
2. **REST API Data (`GET /api/data`)**: Trả về JSON 12 trường dữ liệu hoàn chỉnh:
   ```json
   {
     "device_id": "ESP32_Automation_01",
     "temperature": 28.5,
     "humidity": 65.0,
     "soil_humidity": 55.0,
     "relay1": "OFF",
     "relay2": "OFF",
     "relay1_light": "OFF",
     "relay2_fan": "OFF",
     "rssi": -62,
     "ip": "192.168.1.50",
     "version": "1.0.0",
     "uptime_s": 3400
   }
   ```
3. **REST API Control (`GET /api/relay1?state=ON|OFF`, `GET /api/relay2?state=ON|OFF`)**: Nhận lệnh bật/tắt rơ-le tức thì qua sóng Wi-Fi LAN nội bộ khi mất Internet Cloud.

---

## 🛡️ 5. CƠ CHẾ OTA VÀ CHỐNG BRICK FIRMWARE (ANTI-BRICK ROLLBACK)

Firmware tích hợp cơ chế an toàn **ESP32 Dual Partition Rollback**:
- Khi nạp bản cập nhật OTA từ xa qua HTTPS Cloud (`TOPIC_OTA_TRIGGER`), ESP32 nạp bản mới vào partition phụ và bật cờ `ESP_OTA_IMG_PENDING_VERIFY`.
- Nếu firmware mới boot lên và kết nối MQTT Cloud thành công, hàm `esp_ota_mark_app_valid_cancel_rollback()` được gọi để xác nhận bản nạp hoàn toàn hợp lệ.
- Nếu firmware mới bị lỗi crash ngắt vòng lặp, ESP32 sẽ tự động **Rollback quay về bản firmware cũ** hoạt động bình thường, không bao giờ bị biến thành "gạch" (brick).
