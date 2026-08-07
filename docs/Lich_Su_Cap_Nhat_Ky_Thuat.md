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
