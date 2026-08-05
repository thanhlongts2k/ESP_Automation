# Kế Hoạch Triển Khai Chi Tiết Dự Án ESP_Automation (Từ 0 Đến Hoàn Thành)

Tài liệu hướng dẫn lộ trình từng bước thiết kế, lập trình và vận hành hệ thống IoT điều khiển & thu thập dữ liệu giao tiếp giữa **ESP32** và **Ứng dụng Điện thoại (Mobile App)** qua giao thức **MQTT SSL/TLS**.

---

## 🏗️ 1. Tổng Quan Kiến Trúc Hệ Thống

```text
[ Cảm Biến DHT22 ] ──(Đọc Nhiệt Độ/Độ Ẩm)──┐
                                             ├──> [ ESP32 + Chân Đế 30P ] <──(Wi-Fi / MQTT TLS 8883)──> [ Nginx SSL Proxy ] <──> [ MQTT Broker ]
[ Relay 2 Kênh 5V ] <──(Đóng/Ngắt Đèn/Quạt)─┘                                                                  ▲
                                                                                                               │ (MQTT SSL / WebSockets)
                                                                                                               ▼
                                                                                                    [ Ứng Dụng Điện Thoại ]
                                                                                                   (Flutter / Android / iOS)
```

### Phương Thức Giao Tiếp:
1. **Giao tiếp Từ Xa (Qua Internet - Chính):**
   - **ESP32 ➔ Mobile App:** Định kỳ gửi dữ liệu JSON (Nhiệt độ, Độ ẩm) lên MQTT Broker (`esp32/sensors/dht22`).
   - **Mobile App ➔ ESP32:** Gửi lệnh JSON đóng/mở Relay để bật/tắt Đèn (`esp32/control/relay1`) và Quạt (`esp32/control/relay2`).
   - **Bảo mật:** Toàn bộ luồng dữ liệu chạy qua Nginx Stream SSL/TLS Port 8883 (Mã hóa SSL 100%).

2. **Giao tiếp Trực Tiếp (Không cần Internet - Cấu hình ban đầu):**
   - Sử dụng **Bluetooth BLE (Bluetooth Low Energy)** trên ESP32 để điện thoại kết nối và cài đặt tên Wi-Fi / Password cho ESP32 khi lần đầu khởi chạy (Wi-Fi Provisioning).

---

## 🗓️ 2. Lộ Trình Triển Khai Chi Tiết (5 Giai Đoạn)

### 📌 Giai Đoạn 1: Chuẩn Bị Môi Trường & Kiểm Tra Phần Cứng (Ngày 1 - 2)
*Mục tiêu: Đảm bảo bo mạch phần cứng hoạt động tốt và cài đặt xong công cụ lập trình.*

- [ ] **Bước 1.1:** Cài đặt Driver **CH340** trên Windows để máy tính nhận diện cổng COM khi cắm ESP32 Type-C.
- [ ] **Bước 1.2:** Cài đặt môi trường lập trình **Arduino IDE** (hoặc VS Code + Extension PlatformIO).
- [ ] **Bước 1.3:** Thêm bo mạch ESP32 vào Arduino IDE (`ESP32 Board Manager` bởi Espressif).
- [ ] **Bước 1.4:** Lắp ráp phần cứng:
  - Cắm ESP32 30P vào Chân Đế Mở Rộng.
  - Cắm dây test nối ESP32 với Relay 2 Kênh (Chân GPIO 18, 19) và Cảm biến DHT22 (Chân GPIO 23).
- [ ] **Bước 1.5:** Nạp thử chương trình mẫu **Blink LED** kiểm tra nạp code thông suốt.

---

### 📌 Giai Đoạn 2: Lập Trình Firmware ESP32 Cơ Bản (Ngày 3 - 4)
*Mục tiêu: Đọc được cảm biến và điều khiển Relay cục bộ.*

- [ ] **Bước 2.1:** Cài đặt thư viện `DHT sensor library` bởi Adafruit trong Arduino IDE.
- [ ] **Bước 2.2:** Viết code đọc chỉ số Nhiệt độ (°C) và Độ ẩm (%) từ DHT22, in ra màn hình Serial Monitor.
- [ ] **Bước 2.3:** Viết code kích chân GPIO để đóng/ngắt Relay 2 Kênh (Bật/tắt thử nghiệm Đèn & Quạt).
- [ ] **Bước 2.4:** Viết chương trình tích hợp Bluetooth BLE (dùng thư viện `BLEDevice.h`) để Điện thoại có thể dò tìm và quét được ESP32 qua Bluetooth.

---

### 📌 Giai Đoạn 3: Tích Hợp MQTT SSL/TLS & Cloud Server (Ngày 5 - 6)
*Mục tiêu: ESP32 kết nối Internet, gửi/nhận dữ liệu an toàn qua Server Nginx.*

- [ ] **Bước 3.1:** Cài đặt thư viện `PubSubClient` (MQTT Client) và `WiFiClientSecure`.
- [ ] **Bước 3.2:** Khai báo chứng chỉ Root CA (`DigiCertGlobalRootG2.pem`) vào firmware ESP32 để xác thực SSL với Nginx.
- [ ] **Bước 3.3:** Cấu hình kết nối Wi-Fi & MQTT Client trỏ về Domain Server: `api-vending.doanhnghiep.com:8883`.
- [ ] **Bước 3.4:** Lập trình **Publish Data:** Định kỳ 5 giây tạo chuỗi JSON `{"temperature": 28.5, "humidity": 65.0}` gửi lên topic `esp32/sensors/dht22`.
- [ ] **Bước 3.5:** Lập trình **Subscribe Control:** Lắng nghe lệnh từ topic `esp32/control/relay1` (Bật/Tắt Đèn) và `esp32/control/relay2` (Bật/Tắt Quạt).

---

### 📌 Giai Đoạn 4: Xây Dựng Ứng Dụng Điện Thoại (Mobile App) (Ngày 7 - 10)
*Mục tiêu: Tạo App trên điện thoại giao tiếp với ESP32 qua MQTT SSL & Bluetooth.*

- [ ] **Bước 4.1:** Lựa chọn Framework phát triển Mobile App:
  - **Khuyên dùng: Flutter** (Viết 1 lần chạy cả trên Android & iOS, giao diện đẹp, thư viện MQTT mạnh).
- [ ] **Bước 4.2:** Thiết kế Giao diện Người dùng (UI/UX App):
  - **Màn hình chính (Dashboard):**
    - Đồng hồ hiển thị chỉ số Nhiệt độ (°C) và Độ ẩm (%) cập nhật Realtime.
    - Công tắc Toggle Switch bật/tắt **Đèn** và **Quạt**.
    - Đèn báo trạng thái kết nối (Online/Offline) của thiết bị ESP32.
  - **Màn hình Cấu hình (Settings):** Cấu hình Wi-Fi cho ESP32 qua Bluetooth BLE.
- [ ] **Bước 4.3:** Lập trình tích hợp thư viện `mqtt_client` trên Flutter:
  - Bật kết nối WSS (WebSocket Secure) hoặc TLS 8883 về MQTT Broker.
  - Nhận chuỗi JSON từ ESP32 bóc tách dữ liệu hiển thị lên giao diện.
  - Bấm công tắc ➔ Gửi gói tin MQTT điều khiển đóng/mở Relay tới ESP32.

---

### 📌 Giai Đoạn 5: Tối Ưu, Chống Mất Kết Nối & Vận Hành Ổn Định (Ngày 11 - 12)
*Mục tiêu: Đảm bảo hệ thống chạy bền bỉ 24/7 không rỗi mạch hay treo App.*

- [ ] **Bước 5.1:** Lập trình tính năng **Auto Reconnect** trên ESP32:
  - Nếu mất Wi-Fi hoặc ngắt kết nối MQTT, ESP32 tự động quét và thử kết nối lại sau mỗi 5 giây mà không bị treo mạch.
- [ ] **Bước 5.2:** Lưu trạng thái vào bộ nhớ NVS (Non-Volatile Storage) của ESP32:
  - Khi mất điện và có điện trở lại, ESP32 tự khôi phục đúng trạng thái Bật/Tắt cũ của Đèn & Quạt.
- [ ] **Bước 5.3:** Build file ứng dụng điện thoại (.apk cho Android) để cài đặt và nghiệm thu dự án.
- [ ] **Bước 5.4:** Chạy kiểm thử liên tục trong 48 giờ để đánh giá độ ổn định.

---

## 📑 3. Bảng Phân Công Công Việc & Sản Phẩm Đầu Ra

| Giai Đoạn | Công Việc Chính | Sản Phẩm Đầu Ra (Deliverables) |
| :---: | :--- | :--- |
| **Giai đoạn 1** | Cài môi trường & Lắp ráp phần cứng | Phần cứng lắp hoàn chỉnh, ESP32 nạp thành công Blink LED |
| **Giai đoạn 2** | Code đọc DHT22 & Điều khiển Relay | Code C++ đọc đúng chỉ số sensor và nhảy rơ-le |
| **Giai đoạn 3** | Tích hợp MQTT SSL 8883 over TLS | ESP32 bắn dữ liệu lên Broker và nhận lệnh bật/tắt qua MQTT |
| **Giai đoạn 4** | Lập trình Mobile App (Flutter) | File cài đặt App (.apk) hiển thị nhiệt độ & nút bật/tắt quạt/đèn |
| **Giai đoạn 5** | Test Auto-reconnect & Đóng gói | Hệ thống vận hành ổn định 24/7, có tính năng tự phục hồi |
