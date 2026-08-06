# Kế Hoạch Triển Khai Chi Tiết Dự Án ESP_Automation (Kiến Trúc Enterprise)

Tài liệu hướng dẫn lộ trình từng bước thiết kế, lập trình và vận hành hệ thống IoT điều khiển & thu thập dữ liệu giao tiếp giữa **ESP32** và **Ứng dụng Điện thoại (Mobile App)** qua giao thức **MQTT Cloud Enterprise (SSL/TLS & WebSockets)**.

---

## 🏗️ 1. Tổng Quan Kiến Trúc Hệ Thống

```text
[ Cảm Biến DHT22 ] ──(Đọc Nhiệt Độ/Độ Ẩm)──┐
                                             ├──> [ ESP32 (Modular C++) ] <──(Wi-Fi / MQTT TLS 8883)──> [ Nginx SSL Proxy ] <──> [ MQTT Broker ]
[ Relay 2 Kênh 5V ] <──(Đóng/Ngắt Đèn/Quạt)─┘                                                                  ▲
                                                                                                               │ (MQTT SSL / WebSockets)
                                                                                                               ▼
                                                                                                    [ Ứng Dụng Điện Thoại ]
                                                                                                   (Flutter / Android / iOS)
```

### Phương Thức Giao Tiếp Chuẩn Enterprise:
1. **Giao tiếp Từ Xa (Qua Internet - Chính):**
   - **ESP32 ➔ Mobile App:** Định kỳ gửi dữ liệu JSON (Nhiệt độ, Độ ẩm, RSSI, Uptime) lên MQTT Broker (`esp32/{device_id}/sensors`).
   - **LWT Status (Last Will & Testament):** Đăng ký tin nhắn di chúc `esp32/{device_id}/status` -> `offline` (Retained=True). Khi đứt mạng hay mất điện, Broker tự động báo cho App.
   - **Mobile App ➔ ESP32:** Gửi lệnh JSON đóng/mở Relay để bật/tắt Đèn (`esp32/{device_id}/control/relay1`) và Quạt (`esp32/{device_id}/control/relay2`).
   - **Bảo mật:** Luồng dữ liệu chạy qua Nginx Stream SSL/TLS Port 8883 (Mã hóa SSL 100%) hoặc WebSockets Port 8083.

2. **Giao tiếp Trực Tiếp (Local Wi-Fi REST API - Phương án 3):**
   - Chạy Web Server nội bộ tại `http://esp32.local` nhúng trong bộ nhớ PROGMEM, hỗ trợ điều khiển ngắt rơ-le và xem chỉ số trực tiếp không cần mạng Internet.

---

## 🗓️ 2. Lộ Trình Triển Khai Chi Tiết (5 Giai Đoạn)

### 📌 Giai Đoạn 1: Chuẩn Bị Môi Trường & Kiểm Tra Phần Cứng (Ngày 1 - 2)
*Mục tiêu: Đảm bảo bo mạch phần cứng hoạt động tốt và cài đặt xong công cụ lập trình.*

- [x] **Bước 1.1:** Cài đặt Driver **CH340** trên Windows để máy tính nhận diện cổng COM khi cắm ESP32 Type-C.
- [x] **Bước 1.2:** Cài đặt môi trường lập trình **VS Code + Extension PlatformIO**.
- [x] **Bước 1.3:** Thêm bo mạch ESP32 vào PlatformIO (`board = esp32dev`).
- [x] **Bước 1.4:** Lắp ráp phần cứng:
  - Cắm ESP32 30P vào Chân Đế Mở Rộng.
  - Cắm dây test nối ESP32 với Relay 2 Kênh (GPIO 18, 19) và Cảm biến DHT22 (GPIO 23).
- [x] **Bước 1.5:** Nạp thử chương trình mẫu **Blink LED** kiểm tra nạp code thông suốt.

---

### 📌 Giai Đoạn 2: Lập Trình Firmware ESP32 Modular C++ (Ngày 3 - 4)
*Mục tiêu: Đọc cảm biến, quản lý Relay & Hysteresis tự động.*

- [x] **Bước 2.1:** Phân rã mã nguồn C++ thành các module: `WiFiManager`, `SensorManager`, `RelayController`, `MQTTManager`.
- [x] **Bước 2.2:** Đọc chỉ số Nhiệt độ (°C), Độ ẩm (%) từ DHT22 và Cảm biến độ ẩm đất Analog.
- [x] **Bước 2.3:** Lập trình Logic Hysteresis tự động bật quạt khi Nhiệt độ >= 32°C và tắt quạt khi <= 29°C.

---

### 📌 Giai Đoạn 3: Tích Hợp MQTT SSL/TLS & Cloud Server (Ngày 5 - 6)
*Mục tiêu: Kết nối MQTT Cloud an toàn với LWT, Retained States & Backoff.*

- [x] **Bước 3.1:** Cấu hình chứng chỉ Root CA (`DigiCertGlobalRootG2.pem`) vào firmware ESP32.
- [x] **Bước 3.2:** Khai báo cấu hình kết nối Wi-Fi & MQTT Client trỏ về `.env` linh hoạt.
- [x] **Bước 3.3:** Đăng ký LWT Di chúc `esp32/{device_id}/status` và cờ Retained = True.
- [x] **Bước 3.4:** Lập trình thuật toán **Exponential Backoff Reconnect** ngầm phi bất đồng bộ (2s ➔ 4s ➔ 8s... max 60s).

---

### 📌 Giai Đoạn 4: Xây Dựng Ứng Dụng Điện Thoại (Mobile App) (Ngày 7 - 10)
*Mục tiêu: Tạo App trên điện thoại giao tiếp với ESP32 qua MQTT Cloud & Local API.*

- [x] **Bước 4.1:** Phát triển Mobile App trên nền tảng **Flutter SDK**.
- [x] **Bước 4.2:** Giao diện Dashboard Dark Mode Glassmorphism hiển thị Nhiệt độ, Độ ẩm, Độ ẩm đất, RSSI Wi-Fi và Uptime.
- [x] **Bước 4.3:** Lập trình Dual Fallback MQTT (TCP 1883 ➔ WebSockets 8083) xuyên tường lửa 4G/ISP.
- [x] **Bước 4.4:** Khai báo đầy đủ quyền mạng `INTERNET` trong `AndroidManifest.xml`.

---

### 📌 Giai Đoạn 5: Tối Ưu, Chống Mất Kết Nối & Vận Hành Ổn Định (Ngày 11 - 12)
*Mục tiêu: Đảm bảo hệ thống chạy bền bỉ 24/7.*

- [x] **Bước 5.1:** Lập trình tính năng **Auto Reconnect** không treo vòng lặp `loop()`.
- [x] **Bước 5.2:** Build file APK Release và nạp siêu tốc qua lệnh `adb install -r`.
- [x] **Bước 5.3:** Chạy kiểm thử giả lập `mock_esp32.py` và phần cứng thực tế.

---

## 📑 3. Bảng Phân Công Công Việc & Sản Phẩm Đầu Ra

| Giai Đoạn | Công Việc Chính | Sản Phẩm Đầu Ra (Deliverables) |
| :---: | :--- | :--- |
| **Giai đoạn 1** | Cài môi trường & Lắp ráp phần cứng | Phần cứng lắp hoàn chỉnh, ESP32 nạp thành công Blink LED |
| **Giai đoạn 2** | Code Modular C++ (Sensor, Relay, Hysteresis) | Các file `src/wifi_manager`, `src/sensor_manager`, `src/relay_controller` |
| **Giai đoạn 3** | Tích hợp MQTT Cloud LWT & Exponential Backoff | `src/mqtt_manager` hỗ trợ LWT, Retained, Dynamic Topics `esp32/{device_id}/...` |
| **Giai đoạn 4** | Lập trình Mobile App (Flutter) | File cài đặt App (`app-release.apk`) hiển thị Realtime LWT, RSSI, Uptime |
| **Giai đoạn 5** | Test Auto-reconnect & Đóng gói | Hệ thống Enterprise vận hành ổn định 24/7 |
