# Nhật Ký Kỹ Thuật & Trạng Thái Mã Nguồn Dự Án ESP_Automation

Tài liệu này lưu trữ lịch sử cập nhật kỹ thuật, trạng thái các tính năng đã được lập trình sẵn trong dự án **ESP_Automation**. 
**Mục đích:** Giúp tra cứu nhanh trạng thái code của từng chức năng mà không cần đọc lại toàn bộ dự án (tiết kiệm token và thời gian).

---

## 📅 NGUYÊN TẮC QUẢN LÝ TRẠNG THÁI (STATUS KEY)
- 🟢 **[COMPLETED - READY]**: Đã viết xong mã nguồn C++, cấu hình hoàn chỉnh, sẵn sàng nạp.
- 🟡 **[TESTING - PENDING HW]**: Đã có code sẵn, đang chờ linh kiện về để cắm nạp thử nghiệm.
- 🔴 **[PLANNED]**: Nằm trong kế hoạch, chưa viết code.

---

## ⏱️ LỊCH SỬ CẬP NHẬT KỸ THUẬT (TIMELOGS)

### 📌 [2026-08-05 16:30] - Khởi Tạo & Hoàn Thiện Core Firmware ESP32
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `firmware/config.h` -> File cấu hình thông số, biến môi trường, chứng chỉ SSL & chân GPIO.
  - `firmware/ESP_Automation.ino` -> Chương trình C++ chính chạy trên ESP32.
  - `.env` & `.env.example` -> Quản lý tài khoản Wi-Fi & MQTT bảo mật local.
  - `platformio.ini` & `extra_script.py` -> Script tự động tiêm biến `.env` vào C++ Compiler.

#### 🛠️ Chi Tiết Các Chức Năng Đã Có Mã Nguồn:

| STT | Chức Năng / Tính Năng | Trạng Thái | Vị Trí Code (File & Hàm) | Mô Tả Kỹ Thuật & Cấu Hình |
| :---: | :--- | :---: | :--- | :--- |
| **1** | **Đọc Cảm Biến DHT22** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `loop()`)* | - Pin: GPIO 23<br>- Thư viện: `DHT.h`<br>- Chu kỳ đọc: Mỗi 5 giây/lần<br>- Báo lỗi nếu cảm biến hỏng/rút dây. |
| **2** | **Điều Khiển Relay 2 Kênh (Đèn & Quạt)** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `mqttCallback()`)* | - Pin Relay 1 (Đèn): GPIO 18<br>- Pin Relay 2 (Quạt): GPIO 19<br>- Kích mức THẤP (LOW Active)<br>- Nhận lệnh: `"ON"` / `"OFF"`. |
| **3** | **Kết Nối Wi-Fi Auto-Reconnect** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `setupWiFi()`)* | - Thư viện: `WiFi.h`<br>- Đọc SSID/PASS từ file `.env`<br>- Tự kết nối lại khi mất Wi-Fi. |
| **4** | **MQTT Client Over SSL/TLS Port 8883** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `reconnectMQTT()`)* | - Server: `api-vending.doanhnghiep.com:8883`<br>- Mã hóa SSL với Root CA: `DigiCertGlobalRootG2.pem`<br>- Thư viện: `PubSubClient.h` + `WiFiClientSecure.h`. |
| **5** | **Publish dữ liệu JSON Cảm Biến** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `loop()`)* | - Topic: `esp32/sensors/dht22`<br>- Payload JSON: `{"device_id":"...", "temperature":28.5, "humidity":65.0, "version":"1.0.0"}`. |
| **6** | **Subscribe Lệnh Điều Khiển Relay** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `mqttCallback()`)* | - Topic Đèn: `esp32/control/relay1`<br>- Topic Quạt: `esp32/control/relay2`. |
| **7** | **OTA Phương Thức 1: Arduino OTA (LAN)** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `setupArduinoOTA()`)* | - Thư viện: `ArduinoOTA.h`<br>- Cho phép nạp code qua mạng LAN Wi-Fi từ Arduino IDE. |
| **8** | **OTA Phương Thức 2: Web Server OTA** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `setupWebServerOTA()`)* | - URL: `http://<IP_ESP32>/update`<br>- Giao diện HTML chọn file `.bin` nâng cấp qua trình duyệt. |
| **9** | **OTA Phương Thức 3: HTTPS Cloud OTA (MQTT)** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `executeHTTPSOTA()`)* | - Topic kích hoạt: `esp32/system/ota_trigger`<br>- Payload: `{"version":"1.0.1", "url":"https://..."}`<br>- Tải qua HTTPS Nginx SSL `DigiCertGlobalRootG2`. |
| **10** | **Chống Chết Mạch (Anti-Brick Dual Partition)** | 🟢 READY | `firmware/ESP_Automation.ino` | - Phân vùng `app0` & `app1`<br>- Nạp vào vùng tạm, chỉ chuyển boot khi ghi xong 100%. |
| **11** | **Tự Động Rollback Khi Code Mới Lỗi** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `reconnectMQTT()`)* | - Thư viện: `esp_ota_ops.h`<br>- Chỉ chốt hạ (`esp_ota_mark_app_valid_cancel_rollback()`) khi kết nối MQTT thành công. Nếu lỗi crash loop -> Tự lùi về bản code cũ. |
| **12** | **Quản Lý Biến Môi Trường (.env)** | 🟢 READY | `platformio.ini`<br>`extra_script.py` | - Tiêm biến `.env` tự động vào C++ Compiler lúc build.<br>- Đã chặn `.env` trong `.gitignore`. |

---

## 📌 BẢNG TRA CỨU NHANH TRẠNG THÁI CÁC TOPIC MQTT

| Topic MQTT | Hướng Truyền | Định Dạng Dữ Liệu | Trạng Thái Code |
| :--- | :---: | :--- | :---: |
| `esp32/sensors/dht22` | ESP32 ➔ Broker | `{"device_id":"ESP32_Automation_01","temperature":28.5,"humidity":65.0,"version":"1.0.0"}` | 🟢 ĐÃ CÓ CODE |
| `esp32/control/relay1` | Broker ➔ ESP32 | Chuỗi văn bản thuần: `"ON"` hoặc `"OFF"` (Điều khiển Đèn) | 🟢 ĐÃ CÓ CODE |
| `esp32/control/relay2` | Broker ➔ ESP32 | Chuỗi văn bản thuần: `"ON"` hoặc `"OFF"` (Điều khiển Quạt) | 🟢 ĐÃ CÓ CODE |
| `esp32/system/ota_trigger` | Broker ➔ ESP32 | `{"version":"1.0.1", "url":"https://api-vending.doanhnghiep.com/firmware/v1.0.1.bin"}` | 🟢 ĐÃ CÓ CODE |

---

## 🚀 CÁC TÍNH NĂNG TIẾP THEO SẼ LÀM (BACKLOG)

- 🔴 **[PLANNED]**: Lập trình App Điện thoại bằng Flutter (Giao diện hiển thị Nhiệt độ/Độ ẩm Realtime + Nút gạt bật/tắt Quạt/Đèn).
- 🔴 **[PLANNED]**: Tích hợp Bluetooth BLE cho ESP32 để cài đặt tên Wi-Fi từ Điện thoại (Wi-Fi Provisioning).
- 🔴 **[PLANNED]**: Đọc cảm biến độ ẩm đất điện dung v1.2 và nhiệt độ nước DS18B20.
