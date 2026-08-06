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

### 📌 [2026-08-06 11:46] - Tạo Script 1-Click Tự Động Build APK & Nạp Trực Tiếp Vào Android Điện Thoại
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `mobile_app/build_android.bat` -> Script 1-Click nhấp đúp chuột tự động build Release APK ➔ Quét ADB ➔ Nạp trực tiếp vào điện thoại Android.
  - `mobile_app/build_android.ps1` -> Script PowerShell 1-Click tương tự cho máy Windows.
  - `mobile_app/README.md` -> Cập nhật tài liệu hướng dẫn sử dụng bộ script 1-Click cho cả Android (.bat/.ps1) và iOS (.sh).

---

### 📌 [2026-08-06 11:44] - Hỗ Trợ Khởi Chạy Trên iPhone Ảo (iOS Simulator) Trên Mac
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `mobile_app/build_ios.sh` -> Script kịch bản 1-Click tự động kiểm tra thiết bị và cài App vào iPhone trên Mac.
  - `docs/Huong_Dan_Build_iOS_Tren_Mac.md` -> Hướng dẫn lệnh `open -a Simulator` bật iPhone ảo trên Mac thử nghiệm giao diện 1-Click.

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
| **1** | **Script 1-Click Build Android (Windows)** | 🟢 READY | `mobile_app/build_android.bat`<br>`mobile_app/build_android.ps1` | - Nhấp đúp chuột tự động build APK ➔ Nạp đè điện thoại Android qua ADB.<br>- Không cần nhớ bất kỳ lệnh Terminal nào. |
| **2** | **iPhone Ảo (iOS Simulator) Trên Mac** | 🟢 READY | `mobile_app/build_ios.sh`<br>`docs/Huong_Dan_Build_iOS_Tren_Mac.md` | - Lệnh `open -a Simulator` mở chiếc iPhone ảo sắc nét trên màn hình Mac.<br>- Chạy App không cần iPhone thật hay dây cắm. |
| **3** | **Kiến Trúc Modular C++ Enterprise** | 🟢 READY | `firmware/src/main.cpp`<br>`firmware/src/` | - Tách riêng `WiFiManager`, `MQTTManager`, `SensorManager`, `RelayController`<br>- Entrypoint `main.cpp` gọn gàng chỉ ~50 dòng. |
| **4** | **Cụm Topic Phân Cấp theo Device ID** | 🟢 READY | `firmware/src/config.h`<br>`tools/mock_esp32.py` | - Telemetry: `esp32/{device_id}/sensors`<br>- LWT Status: `esp32/{device_id}/status`<br>- Control: `esp32/{device_id}/control/relay1` & `relay2`. |
| **5** | **MQTT LWT (Last Will & Testament)** | 🟢 READY | `firmware/src/mqtt_manager.cpp`<br>`mobile_app/lib/main.dart` | - Đăng ký di chúc LWT: Tự động báo `offline` Retained khi đứt mạng/mất điện.<br>- App Flutter nhận diện chính xác ESP32 Online/Offline. |
| **6** | **Async Reconnect Exponential Backoff** | 🟢 READY | `firmware/src/mqtt_manager.cpp`<br>`firmware/src/wifi_manager.cpp` | - Tự động thử lại kết nối ngầm với thuật toán 2s ➔ 4s ➔ 8s ➔ max 60s.<br>- Tuyệt đối không treo vòng lặp `loop()`, Web Local & Relay vẫn chạy 100%. |
| **7** | **Flutter Mobile App Hybrid Mode** | 🟢 READY | `mobile_app/lib/main.dart` | - Giao diện Dark Mode Glassmorphism Realtime.<br>- Chế độ kép: Dual Fallback MQTT (TCP 1883 ➔ WS 8083) + Local Wi-Fi API. |
| **8** | **Cấp Quyền Internet Android Manifest** | 🟢 READY | `mobile_app/android/app/src/main/AndroidManifest.xml` | - Quyền `INTERNET`, `ACCESS_NETWORK_STATE`, `usesCleartextTraffic="true"`. |
