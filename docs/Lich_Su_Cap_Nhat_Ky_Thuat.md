# Nhật Ký Kỹ Thuật & Trạng Thái Mã Nguồn Dự Án ESP_Automation

Tài liệu này lưu trữ lịch sử cập nhật kỹ thuật, trạng thái các tính năng đã được lập trình sẵn trong dự án **ESP_Automation**. 
**Mục đích:** Giúp tra cứu nhanh trạng thái code của từng chức năng mà không cần đọc lại toàn bộ dự án (tiết kiệm token và thời gian).

---

## 📅 NGUYÊN TẮC QUẢN LÝ TRẠNG THÁI (STATUS KEY)
- 🟢 **[COMPLETED - READY]**: Đã viết xong mã nguồn C++, Flutter hoặc tài liệu cấu hình hoàn chỉnh, sẵn sàng sử dụng.
- 🟡 **[TESTING - PENDING HW]**: Đã có code sẵn, đang chờ linh kiện về để cắm nạp thử nghiệm.
- 🔴 **[PLANNED]**: Nằm trong kế hoạch, chưa viết code.

---

## ⏱️ LỊCH SỬ CẬP NHẬT KỸ THUẬT (TIMELOGS)

### 📌 [2026-08-06 10:44] - Truy Vết Toàn Bộ Thay Đổi: Quy Tắc Git, Nạp .env Nâng Cao & Khắc Phục Lỗi Mạng Release APK
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Toàn Bộ File Đã Thay Đổi / Khởi Tạo (Change Traceability):**
  1. `.agents/AGENTS.md` -> [Khởi tạo mới] Thiết lập quy tắc Workspace nghiêm ngặt: Bắt buộc phải có sự xác nhận trực tiếp của User trước khi thực hiện bất kỳ lệnh `git commit` hoặc `git push` nào.
  2. `extra_script.py` -> [Cập nhật] Bổ sung bộ bóc tách `.env` hỗ trợ Tham chiếu Biến (Variable Interpolation như `MQTT_SERVER=MQTT_SERVER1` hoặc `${MQTT_SERVER1}`) khi tiêm cờ `-D` vào C++ Compiler.
  3. `tools/mock_esp32.py` -> [Cập nhật] Nạp tự động file `.env` với hỗ trợ Tham chiếu Biến, đồng bộ key JSON telemetry (`relay1`/`relay2` & `relay1_light`/`relay2_fan`) và bổ sung chỉ số giả lập `soil_humidity`.
  4. `mobile_app/android/app/src/main/AndroidManifest.xml` -> [Cập nhật] Thêm quyền `android.permission.INTERNET`, `ACCESS_NETWORK_STATE`, `ACCESS_WIFI_STATE` và `android:usesCleartextTraffic="true"` để Release APK kết nối mạng 100%.
  5. `mobile_app/lib/main.dart` -> [Cập nhật] Bóc tách JSON Null-Safe (`data['soil_humidity'] as num?`), bổ sung cơ chế Dual Fallback (TCP Port 1883 ➔ WebSockets Port 8083) xuyên tường lửa 4G/ISP, tối ưu hóa callback `onConnected`/`onDisconnected` chống lặp listener.
  6. `docs/Lich_Su_Cap_Nhat_Ky_Thuat.md` -> [Cập nhật] Tài liệu hóa toàn bộ nhật ký kỹ thuật, nguyên nhân lỗi và bảng tra cứu tính năng.
- **Ghi Chú Khắc Phục Lỗi (Troubleshooting Notes):**
  1. **Lỗi Thiếu Quyền Internet Android Release:** Khi build Release APK (`flutter build apk --release`), Android mặc định CHẶN 100% kết nối Socket Mạng (TCP & WebSocket) nếu file `AndroidManifest.xml` chưa khai báo `<uses-permission android:name="android.permission.INTERNET"/>`.
  2. **Tường Lửa Nhà Mạng 4G/ISP Chặn Port 1883:** Một số nhà mạng di động chặn cổng TCP 1883 ➔ App tự động chuyển sang WebSockets Port 8083 để xuyên tường lửa.
  3. **Null-Safety JSON Parsing:** Ép kiểu an toàn `(data['soil_humidity'] as num?)?.toDouble() ?? 0.0` để tránh crash khi bóc tách JSON thiếu trường.

---

### 📌 [2026-08-05 17:14] - Cập Nhật Lệnh Cài Đặt ADB Siêu Tốc Cho Mobile App
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `mobile_app/README.md` -> Bổ sung lệnh `adb install -r build\app\outputs\flutter-apk\app-debug.apk` đẩy APK vào điện thoại trong 2 giây & lưu ý bật quyền USB cho thiết bị Xiaomi/POCO.

---

### 📌 [2026-08-05 16:55] - Hoàn Thiện Flutter Mobile App & Hướng Dẫn Sơ Đồ Nối Dây
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `mobile_app/lib/main.dart` -> Mã nguồn Flutter App (Giao diện Dashboard, MQTT Client, Auto-switch Hybrid Local REST API Mode).
  - `mobile_app/pubspec.yaml` -> Cấu hình thư viện `mqtt_client`, `http`, `cupertino_icons`.
  - `mobile_app/README.md` -> Hướng dẫn build APK Android cho App.
  - `docs/Huong_Dan_Noi_Day_Phan_Cung.md` -> Sơ đồ chân cắm Pinout chi tiết cho ESP32 30P, Relay 2K, DHT22, DS18B20, Cảm biến đất v1.2 và đấu nối 220V AC.

---

### 📌 [2026-08-05 16:47] - Tạo Hướng Dẫn Chi Tiết Biên Dịch, Nạp Code & Khởi Chạy ESP32
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `docs/Huong_Dan_Nap_Code_ESP32.md` -> Hướng dẫn chi tiết từng bước nạp code qua PlatformIO / Arduino IDE & cách khởi chạy Web Dashboard `http://esp32.local`.

---

### 📌 [2026-08-05 16:45] - Tích Hợp Web Server Local Dashboard (Phương Án 3 - Không Cần Server/App)
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `firmware/web_dashboard.h` -> Giao diện Web Dark Mode Glassmorphism HTML/CSS/JS nhúng trong PROGMEM.
  - `firmware/ESP_Automation.ino` -> Tích hợp `WebServer.h` & `ESPmDNS.h` phục vụ Web Dashboard tại `http://esp32.local`.

---

### 📌 [2026-08-05 16:40] - Khởi Tạo Script Giả Lập ESP32 Trên PC (`mock_esp32.py`)
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `tools/mock_esp32.py` -> Script Python giả lập ESP32 đọc DHT22 & nhận lệnh Relay 1/Relay 2 trên PC.

---

### 📌 [2026-08-05 16:30] - Khởi Tạo & Hoàn Thiện Core Firmware ESP32
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `firmware/config.h` -> File cấu hình thông số, biến môi trường, chứng chỉ SSL & chân GPIO.
  - `firmware/ESP_Automation.ino` -> Chương trình C++ chính chạy trên ESP32.
  - `.env` & `.env.example` -> Quản lý tài khoản Wi-Fi & MQTT bảo mật local.
  - `platformio.ini` & `extra_script.py` -> Script tự động tiêm biến `.env` vào C++ Compiler.

---

## 🛠️ BẢNG TRA CỨU CHI TIẾT CÁC TÍNH NĂNG ĐÃ CÓ MÃ NGUỒN

| STT | Chức Năng / Tính Năng | Trạng Thái | Vị Trí Code (File & Hàm) | Mô Tả Kỹ Thuật & Cấu Hình |
| :---: | :--- | :---: | :--- | :--- |
| **1** | **Flutter Mobile App (Android & iOS)** | 🟢 READY | `mobile_app/lib/main.dart` | - Giao diện Dark Mode hiển thị Nhiệt độ/Độ ẩm Realtime<br>- Switch điều khiển Relay 1 (Đèn) & Relay 2 (Quạt)<br>- Chế độ Hybrid: Tự chuyển giữa MQTT & Local Wi-Fi REST API. |
| **2** | **Cấp Quyền Internet Android Manifest** | 🟢 READY | `mobile_app/android/app/src/main/AndroidManifest.xml` | - Khai báo `<uses-permission android:name="android.permission.INTERNET"/>`<br>- Đảm bảo Release APK kết nối mạng 100% không bị Android OS chặn. |
| **3** | **Tham Chiếu Biến .env (Variable Interpolation)** | 🟢 READY | `extra_script.py`<br>`tools/mock_esp32.py` | - Cho phép viết `MQTT_SERVER=MQTT_SERVER1` hoặc `${MQTT_SERVER1}` trong `.env`.<br>- Tự động giải quyết tham chiếu biến khi build C++ và chạy Mock Script Python. |
| **4** | **Lệnh Cài Đặt ADB Siêu Tốc 2 Giây** | 🟢 READY | `mobile_app/README.md` | - `adb install -r build\app\outputs\flutter-apk\app-release.apk`<br>- Cài đè trực tiếp APK đã build không cần rebuild. |
| **5** | **Sơ Đồ Hướng Dẫn Nối Dây Phần Cứng** | 🟢 READY | `docs/Huong_Dan_Noi_Day_Phan_Cung.md` | - Sơ đồ chân cắm ESP32 30P, Relay 2K, DHT22, Cảm biến đất, DS18B20<br>- Quy tắc nối an toàn điện 220V AC. |
| **6** | **Hướng Dẫn Nạp Code & Khởi Chạy** | 🟢 READY | `docs/Huong_Dan_Nap_Code_ESP32.md` | - Hướng dẫn nạp code qua PlatformIO & Arduino IDE<br>- Cách mở Web Dashboard `http://esp32.local`. |
| **7** | **Web Dashboard Local Wi-Fi (Phương án 3)** | 🟢 READY | `firmware/web_dashboard.h`<br>`firmware/ESP_Automation.ino` | - Mở trình duyệt gõ: `http://esp32.local` hoặc IP<br>- Giao diện Dark Mode Glassmorphism tự cập nhật Nhiệt độ/Độ ẩm Realtime<br>- Toggle Switch bật/tắt Đèn & Quạt trực tiếp. |
| **8** | **mDNS Responder (`esp32.local`)** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `setupLocalWebDashboard()`)* | - Thư viện: `ESPmDNS.h`<br>- Cho phép điện thoại/laptop chung Wi-Fi gõ `http://esp32.local` không cần nhớ IP. |
| **9** | **REST API Nội Bộ (/api/data, /api/relay)** | 🟢 READY | `firmware/ESP_Automation.ino` | - `/api/data`: Trả về JSON nhiệt độ, độ ẩm, relay state.<br>- `/api/relay1?state=ON`: Đóng ngắt Đèn.<br>- `/api/relay2?state=ON`: Đóng ngắt Quạt. |
| **10** | **Script Giả Lập ESP32 trên PC** | 🟢 READY | `tools/mock_esp32.py` | - Chạy `python tools/mock_esp32.py` để test luồng MQTT gửi sensor/nhận lệnh Relay trên PC. |
| **11** | **Đọc Cảm Biến DHT22** | 🟢 READY | `firmware/ESP_Automation.ino` | - Pin: GPIO 23<br>- Thư viện: `DHT.h`<br>- Chu kỳ đọc: Mỗi 2 giây/lần. |
| **12** | **Điều Khiển Relay 2 Kênh (Đèn & Quạt)** | 🟢 READY | `firmware/ESP_Automation.ino` | - Pin Relay 1 (Đèn): GPIO 18<br>- Pin Relay 2 (Quạt): GPIO 19<br>- Kích mức THẤP (LOW Active). |
| **13** | **Kết Nối Wi-Fi Auto-Reconnect** | 🟢 READY | `firmware/ESP_Automation.ino` | - Thư viện: `WiFi.h`<br>- Đọc SSID/PASS từ file `.env`. |
| **14** | **MQTT Client Over SSL/TLS Port 8883** | 🟢 READY | `firmware/ESP_Automation.ino` | - Mã hóa SSL với Root CA `DigiCertGlobalRootG2.pem`<br>- Thư viện: `PubSubClient.h` + `WiFiClientSecure.h`. |
| **15** | **OTA 3 Phương Thức (LAN, Web, HTTPS Cloud)** | 🟢 READY | `firmware/ESP_Automation.ino` | - Arduino OTA + Web OTA + HTTPS Cloud OTA.<br>- Có Anti-brick & Auto Rollback khi lỗi. |
