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

### 📌 [2026-08-08 11:27] - Nâng Cấp Tính Năng Captive Portal AP & Quản Lý Multi Wi-Fi NVS Cho Firmware ESP32
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Nội Dung Nâng Cấp Chi Tiết:**
  1. **Tích Hợp Chế Độ Captive Portal AP (`DNSServer` + `WebServer`)**:
     - Khi không nối được Wi-Fi (hoặc rớt mạng quá 15s), ESP32 tự phát Wi-Fi AP `ESP32-Setup-AP` (IP: `192.168.4.1`) kèm DNS Redirection.
     - Trang Web Cấu hình HTML/CSS Glassmorphism nhúng PROGMEM tự động nhảy ra trên màn hình điện thoại/laptop kết nối vào.
     - Hỗ trợ nút `🔄 Quét danh sách Wi-Fi xung quanh` (API `/api/wifi-scan`) và form lưu mật khẩu (POST `/save-wifi`).
  2. **Quản Lý Multi Wi-Fi & Bộ Nhớ Flash NVS (`Preferences.h` & `WiFiMulti`)**:
     - Lưu tối đa 5 mạng Wi-Fi vào NVS namespace `"wificreds"`.
     - Tự động chọn kết nối tới mạng Wi-Fi có sóng RSSI mạnh nhất.
     - Giúp mã nguồn C++ trên Git hoàn toàn sạch sẽ 100%, không bị lộ bất kỳ mật khẩu thực tế nào.
  3. **Nút Nhấn BOOT (GPIO 0) Reset Wi-Fi**:
     - Nhấn giữ nút BOOT trên mạch ESP32 trong 3 giây để xóa sạch dữ liệu Wi-Fi trong NVS và khởi động lại về chế độ Captive Portal AP.
  4. **Cập Nhật Tài Liệu Kỹ Thuật**:
     - Cập nhật tài liệu kiến trúc `docs/Kien_Truc_Kiem_Soat_Firmware_ESP32.md` và tạo file `walkthrough.md`.
     - Đã biên dịch kiểm thử thành công bằng PlatformIO (`SUCCESS - Took 11.18s`).

---

### 📌 [2026-08-08 09:48] - Fix Lỗi Biên Dịch GCC Macro Escaping Trong `extra_script.py` Cho PlatformIO
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Nội Dung Nâng Cấp Chi Tiết:**
  1. **Khắc Phục Lỗi Macro String Escaping**:
     - Cập nhật script `extra_script.py` phân loại kiểu dữ liệu biến trong `.env`: Các chuỗi string được tự động bọc cú pháp `\\\"VAL\\\"` trước khi nạp vào `CPPDEFINES` của GCC Preprocessor.
     - Khắc phục triệt để các lỗi biên dịch C++ macro: `'ESP32_Automation_01' was not declared in this scope`, `'api' was not declared in this scope`, và `too many decimal points in number` cho `FIRMWARE_VERSION`.
  2. **Biên Dịch Thành Công 100%**:
     - PlatformIO build thành công bản nạp binary `.pio/build/esp32dev/firmware.bin` (`SUCCESS - Took 17.58s`).

---

### 📌 [2026-08-08 08:20] - Biên Soạn Tài Liệu Mô Tả Kỹ Thuật Chi Tiết Kiến Trúc Firmware ESP32
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Nội Dung Nâng Cấp Chi Tiết:**
  1. **Tạo Mới Tài Liệu Kiến Trúc Firmware (`docs/Kien_Truc_Kiem_Soat_Firmware_ESP32.md`)**:
     - Sơ đồ tương tác giữa các module C++ (Mermaid Diagram): Phần cứng ➔ `SensorManager` ➔ `main.cpp` ➔ `RelayController` (Hysteresis) ➔ `MQTTManager` ➔ `WebServer`.
     - Phân tích chi tiết 5 module C++ cốt lõi: `config.h`, `WiFiManager`, `SensorManager`, `RelayController` (nhiệt độ 32°C/29°C), `MQTTManager` (Nginx SSL Proxy 8883, LWT di chúc, Backoff).
     - Mô tả chi tiết Web Server PROGMEM HTML5 Glassmorphism & các Endpoint REST API (`/api/data`, `/api/relay1`, `/api/relay2`).
     - Mô tả cơ chế OTA Anti-brick Rollback.
  2. **Cập Nhật Index `README.md`**:
     - Thêm liên kết truy cập nhanh tài liệu kiến trúc firmware mới vào bảng Index tài liệu.

---

### 📌 [2026-08-07 16:08] - Cập Nhật Làm Rõ Tải Điện 220V AC Trong Sơ Đồ Nối Dây & Danh Sách Linh Kiện
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Nội Dung Nâng Cấp Chi Tiết:**
  1. **Làm Rõ Phương Thức Thử Nghiệm An Toàn Bằng Đèn LED Có Sẵn**:
     - Cập nhật sơ đồ ASCII và lưu ý an toàn trong `Huong_Dan_Noi_Day_Phan_Cung.md`: Làm rõ rằng tải điện 220V AC là **tùy chọn (Optional)**.
     - Người dùng **không cần mua thêm thiết bị 220V** khi thử nghiệm ban đầu; có thể thử nghiệm 100% việc Bật/Tắt Relay thông qua tiếng "tạch" (Click) và 2 Đèn LED tích hợp sẵn trên Module Relay 2 Kênh mà không cần điện cao thế nguy hiểm.
  2. **Cập Nhật Danh Sách Linh Kiện `Danh_Sach_Thiet_Bi_Da_Mua.md`**:
     - Thêm ghi chú giải thích rõ việc tận dụng bóng đèn/quạt có sẵn trong nhà khi đấu nối thực tế sau này.

---

### 📌 [2026-08-07 15:52] - Chuẩn Hóa Dynamic `${DEVICE_ID}` Topics Cho Firmware C++ & File Cấu Hình `.env`
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Nội Dung Nâng Cấp Chi Tiết:**
  1. **Đồng Bộ Bộ Hàm Topic Động Trong `firmware/config.h` & `ESP_Automation.ino`**:
     - Thêm các hàm `getTopicSensors()`, `getTopicStatus()`, `getTopicRelay1Control()`, `getTopicRelay2Control()`, `getTopicOTATrigger()` vào `firmware/config.h`.
     - Đã thay thế các chuỗi Topic hằng số tĩnh cũ (`esp32/sensors/dht22`) trong `ESP_Automation.ino` sang định dạng phân cấp động theo `DEVICE_ID`.
  2. **Bổ Sung LWT Di Chúc Status Cho Arduino IDE (`ESP_Automation.ino`)**:
     - Tự động đăng ký di chúc LWT `status` ("offline", retain = true) và tin nhắn `status` ("online", retain = true) khi kết nối MQTT Broker.
  3. **Chuẩn Hóa Mẫu Cấu Hình `.env` & `.env.example`**:
     - Cập nhật định dạng các biến Topic trong `.env` và `.env.example` theo dạng mẫu `esp32/${DEVICE_ID}/...`.

---

### 📌 [2026-08-07 15:35] - Fix Lỗi Tương Thích Ngược App Flutter Cho Các Phiên Bản Flutter SDK Cũ (v3.19 - v3.24)
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Nội Dung Nâng Cấp Chi Tiết:**
  1. **Khắc Phục Lỗi Biên Dịch `withValues`**:
     - Thay thế toàn bộ 14 vị trí `Color.withValues(alpha: x)` thành `Color.withOpacity(x)` trên tất cả các màn hình (`DashboardScreen`, `HistoryScreen`, `DeviceDetailScreen`, `SettingsScreen`).
  2. **Khắc Phục Lỗi Tham Số `activeThumbColor` Trong `Switch`**:
     - Chuyển thuộc tính `activeThumbColor` ➔ `activeColor` trong `Switch` giúp code biên dịch mượt mà trên tất cả các phiên bản Flutter SDK cũ và mới.

---

### 📌 [2026-08-07 15:25] - Nâng Cấp Firmware C++ ESP32 Đồng Bộ 100% Nginx SSL Proxy & Local REST API Data Schema
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Nội Dung Nâng Cấp Chi Tiết:**
  1. **Đồng Bộ Nginx SSL Stream Proxy Port 8883 (`WiFiClientSecure`)**:
     - Cập nhật `mqtt_manager.cpp` & `ESP_Automation.ino` sử dụng `_sslClient.setInsecure()` khi `MQTT_USE_SSL` được bật.
     - Cho phép ESP32 bắt tay mã hóa TLS mượt mà qua các Server Nginx SSL Proxy (Let's Encrypt / Dynamic Certs) trên cổng 8883 mà không bị rớt kết nối.
  2. **Bổ Sung Enriched JSON Payload Cho Endpoint `/api/data`**:
     - Cập nhật `/api/data` trong `firmware/src/main.cpp` và `ESP_Automation.ino` bổ sung đầy đủ trường dữ liệu: `device_id`, `soil_humidity`, `relay1_light`, `relay2_fan`, `rssi`, `uptime_s`.
     - Đảm bảo khi Mobile App chuyển sang chế độ Local REST API Polling qua Wi-Fi LAN, dữ liệu nhận về khớp 100% với định dạng Telemetry của MQTT Cloud.
  3. **Chú Thích Code Chi Tiết (Comprehensive Documentation & Comments)**:
     - Ghi chú rõ ràng bằng tiếng Việt giải thích trong từng module C++ và hàm callback.

---

### 📌 [2026-08-07 14:15] - Nâng Cấp Kiến Trúc Mobile App Multiple Screen, SSL/TLS Proxy, SharedPreferences & Local REST API
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Nội Dung Nâng Cấp Chi Tiết:**
  1. **Tái Cấu Trúc Mã Nguồn Mobile App Multiple Screen (`mobile_app/lib/`)**:
     - Phân rã `main.dart` thành mô hình Modular Clean Architecture: `lib/core/` (Theme, Constants), `lib/models/` (DeviceState), `lib/services/` (MqttService), `lib/screens/` (4 Màn hình chức năng).
     - Sử dụng `IndexedStack` + `BottomNavigationBar` quản lý 4 màn hình: **Dashboard (Điều khiển)**, **History (Lịch sử)**, **Device Detail (Chi tiết phần cứng/RSSI/Uptime)**, **Settings (Cấu hình hệ thống)** mà không bị ngắt luồng kết nối MQTT.
  2. **Bộ Nhớ Lưu Trữ Cấu Hình Vĩnh Viễn (`SharedPreferences`)**:
     - Tự động lưu và khôi phục các thông số **MQTT Server, Port, SSL Mode, Username, Password, Device ID, Local IP** qua bộ nhớ máy khi đóng/mở lại ứng dụng.
  3. **Hỗ Trợ Mã Hóa SSL/TLS cho Nginx SSL Stream Proxy (Port 8883 & 8084)**:
     - Tích hợp `SecurityContext` và `onBadCertificate` cho phép App Android/iOS bắt tay mã hóa TLS với Nginx SSL Proxy Port 8883 và WebSocket WSS Port 8084.
     - Bổ sung ô nhập Username / Password mã hóa trên giao diện Cấu hình.
  4. **Tự Động Polling Local REST API (`/api/data`)**:
     - Tự động phát hiện khi rỗng MQTT Domain hoặc đứt mạng Cloud ➔ Chuyển sang chế độ Local REST API.
     - Polling định kỳ mỗi 2 giây gửi HTTP GET `/api/data` để đọc chỉ số cảm biến từ ESP32 trong mạng LAN.
  5. **Tối Ưu Hóa Build Gradle Windows**:
     - Thêm `kotlin.incremental=false` trong `android/gradle.properties` khắc phục triệt để lỗi gãy đệm Kotlin Daemon khi build cross-drive (ổ C: vs ổ D:).

---

### 📌 [2026-08-07 10:30] - Quy Hoạch Toàn Bộ Bộ Công Cụ Phát Triển Về Thư Mục Dùng Chung D:\DevTools
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Nội Dung Nâng Cấp:**
  1. **Quy hoạch vị trí thư mục dùng chung trên ổ D:**
     - Android SDK ➔ `D:\DevTools\Android`
     - Flutter SDK ➔ `D:\DevTools\Flutter`
     - OpenJDK 17 ➔ `D:\DevTools\Java`
     - Gradle User Cache ➔ `D:\DevTools\.gradle`
  2. **Biến môi trường hệ thống (User Environment Variables)**:
     - `JAVA_HOME` = `D:\DevTools\Java`
     - `ANDROID_HOME` = `D:\DevTools\Android\Sdk`
     - `GRADLE_USER_HOME` = `D:\DevTools\.gradle`
     - User `PATH` ➔ Bổ sung `D:\DevTools\Flutter\bin`, `D:\DevTools\Java\bin`, `D:\DevTools\Android\Sdk\platform-tools`.
  3. **Đồng bộ hóa toàn bộ Scripts**:
     - `install_flutter_windows.bat` ➔ Tự động tải & giải nén Flutter về `D:\DevTools\Flutter`.
     - `setup_environment.bat` & `mobile_app/build_android.bat` ➔ Nhận diện ưu tiên hàng đầu tại `D:\DevTools\...`.

---

### 📌 [2026-08-07 09:50] - Tự Động Hóa 1-Click Cài Đặt Android SDK, Java 17 & Fix Sửa Lỗi Gradle/AGP Build APK
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Nội Dung Nâng Cấp:**
  1. **`install_flutter_windows.bat`**: Script 1-Click tự động tải & giải nén Flutter SDK 3.24.0 vào `C:\flutter`.
  2. **Tự Động Cài Đặt Môi Trường Build Android SDK (Command-Line Tools)**:
     - Tự động tải & giải nén OpenJDK 17 (`C:\jdk-17.0.2`) và Android SDK (`C:\Android\Sdk`).
     - Tự động tạo license hashes chấp nhận toàn bộ Android SDK licenses (`All SDK package licenses accepted`).
  3. **`mobile_app/build_android.bat`**:
     - Tự động kiểm tra trạng thái **Windows Developer Mode**, tự động mở Settings nếu chưa bật.
     - Tự động nhận diện công cụ ADB tại `C:\Android\Sdk\platform-tools\adb.exe` và sửa lỗi CMD parser `. was unexpected at this time`.
  4. **Cấu Hình Gradle & Flutter Compatibility**:
     - `gradle-wrapper.properties`: Chuẩn hóa về **Gradle 8.7-all** (khắc phục lỗi Gradle 9x làm gãy Flutter `groovy.xml.QName`).
     - `settings.gradle.kts` & `app/build.gradle.kts`: Cập nhật AGP `8.3.2`, Kotlin `1.9.24`, `kotlinOptions { jvmTarget = "17" }` và `ndkVersion = "25.1.8937393"`.
     - Build thành công bản **Release APK** 18.9MB.

---

### 📌 [2026-08-07 09:02] - Bổ Sung Script 1-Click Cài Đặt Môi Trường Tự Động Cho Máy Mới Clone Code
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `requirements.txt` -> Định nghĩa thư viện Python (`paho-mqtt`, `platformio`).
  - `setup_environment.bat` -> Script 1-Click tự động kiểm tra Python, Flutter SDK, ADB và cài đặt toàn bộ thư viện phụ thuộc (`pip`, `flutter pub get`).
  - `mobile_app/build_android.bat` -> Nâng cấp tự động kiểm tra Flutter SDK & thông báo hướng dẫn xử lý khi thiếu môi trường trên máy mới.

---

### 📌 [2026-08-06 11:46] - Tạo Script 1-Click Tự Động Build APK & Nạp Trực Tiếp Vào Android Điện Thoại
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `mobile_app/build_android.bat` -> Script 1-Click nhấp đúp chuột tự động build Release APK ➔ Quét ADB ➔ Nạp trực tiếp vào điện thoại Android.
  - `mobile_app/build_android.ps1` -> Script PowerShell 1-Click tương tự cho máy Windows.
  - `mobile_app/README.md` -> Cập nhật tài liệu hướng dẫn sử dụng bộ script 1-Click cho cả Android (.bat/.ps1) và iOS (.sh).

---

### 📌 [2026-08-06 11:25] - Nâng Cấp Toàn Diện Kiến Trúc Modular C++ Enterprise & MQTT Cloud Protocol
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Nội Dung Nâng Cấp Chi Tiết:**
  1. **Phân rã Monolithic C++ (`firmware/src/`):** `main.cpp`, `config.h`, `wifi_manager`, `sensor_manager`, `relay_controller`, `mqtt_manager`.
  2. **Nâng Cấp Script Giả Lập Python (`tools/mock_esp32.py`):** LWT Status (`esp32/{device_id}/status`), RSSI, Uptime.
  3. **Nâng Cấp Flutter Mobile App (`mobile_app/lib/main.dart`):** LWT Status Online/Offline, Wi-Fi RSSI, Uptime, Dual Fallback (TCP 1883 ➔ WS 8083).

---

## 🛠️ BẢNG TRA CỨU CHI TIẾT CÁC TÍNH NĂNG ĐÃ CÓ MÃ NGUỒN

| STT | Chức Năng / Tính Năng | Trạng Thái | Vị Trí Code (File & Hàm) | Mô Tả Kỹ Thuật & Cấu Hình |
| :---: | :--- | :---: | :--- | :--- |
| **1** | **Cài Đặt Môi Trường 1-Click Máy Mới** | 🟢 READY | `setup_environment.bat`<br>`requirements.txt` | - Tự động kiểm tra Python, Flutter SDK, ADB.<br>- Cài đặt toàn bộ thư viện `pip` và `flutter pub get` trong 1 click. |
| **2** | **Script 1-Click Build Android (Windows)** | 🟢 READY | `mobile_app/build_android.bat`<br>`mobile_app/build_android.ps1` | - Nhấp đúp chuột tự động build APK ➔ Nạp đè điện thoại Android qua ADB.<br>- Tự động kiểm tra và thông báo hướng dẫn nếu máy mới thiếu Flutter. |
| **3** | **iPhone Ảo (iOS Simulator) Trên Mac** | 🟢 READY | `mobile_app/build_ios.sh`<br>`docs/Huong_Dan_Build_iOS_Tren_Mac.md` | - Lệnh `open -a Simulator` mở chiếc iPhone ảo sắc nét trên màn hình Mac.<br>- Chạy App không cần iPhone thật hay dây cắm. |
| **4** | **Kiến Trúc Modular C++ Enterprise** | 🟢 READY | `firmware/src/main.cpp`<br>`firmware/src/` | - Tách riêng `WiFiManager`, `MQTTManager`, `SensorManager`, `RelayController`<br>- Entrypoint `main.cpp` gọn gàng chỉ ~50 dòng. |
| **5** | **MQTT LWT (Last Will & Testament)** | 🟢 READY | `firmware/src/mqtt_manager.cpp`<br>`mobile_app/lib/main.dart` | - Đăng ký di chúc LWT: Tự động báo `offline` Retained khi đứt mạng/mất điện.<br>- App Flutter nhận diện chính xác ESP32 Online/Offline. |
