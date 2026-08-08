# 🚀 ESP32 IoT Automation Enterprise Platform

Hệ thống giám sát cảm biến và điều khiển tự động hóa thiết bị qua **ESP32**, **Flutter Mobile App (Android & iOS)**, **MQTT Cloud Enterprise (Nginx SSL/TLS Stream Proxy)** và **Local REST API Fallback (Wi-Fi LAN)**.

---

## 📚 1. Danh Sách Tài Liệu Hướng Dẫn Kỹ Thuật (Documentation Directory)

Toàn bộ tài liệu thiết kế, nối dây phần cứng, hướng dẫn cài đặt và lịch sử nâng cấp được lưu trữ trong thư mục [`docs/`](file:///d:/Sources/ESP_Automation/docs):

| STT | Tài Liệu Hướng Dẫn | Mô Tả Chi Tiết |
| :---: | :--- | :--- |
| 1 | 🛠️ [Huong_Dan_Noi_Day_Phan_Cung.md](file:///d:/Sources/ESP_Automation/docs/Huong_Dan_Noi_Day_Phan_Cung.md) | Sơ đồ cắm chân ESP32 DevKit 30P, Module Relay 2 Kênh, Cảm biến DHT22. |
| 2 | 🛒 [Danh_Sach_Thiet_Bi_Da_Mua.md](file:///d:/Sources/ESP_Automation/docs/Danh_Sach_Thiet_Bi_Da_Mua.md) | Danh sách linh kiện phần cứng, chân đế mở rộng và thông số kỹ thuật. |
| 3 | 🔌 [Huong_Dan_Nap_Code_ESP32.md](file:///d:/Sources/ESP_Automation/docs/Huong_Dan_Nap_Code_ESP32.md) | Hướng dẫn nạp firmware C++ cho ESP32 qua PlatformIO / VS Code hoặc Arduino IDE. |
| 4 | 🔄 [Huong_Dan_Cap_Nhat_OTA_ESP32.md](file:///d:/Sources/ESP_Automation/docs/Huong_Dan_Cap_Nhat_OTA_ESP32.md) | Quy trình nạp code từ xa qua mạng Wi-Fi (ArduinoOTA & HTTP OTA Update). |
| 5 | 🍏 [Huong_Dan_Build_iOS_Tren_Mac.md](file:///d:/Sources/ESP_Automation/docs/Huong_Dan_Build_iOS_Tren_Mac.md) | Hướng dẫn build và chạy App Flutter trên iPhone thật & iPhone ảo (iOS Simulator). |
| 6 | 📅 [Ke_Hoach_Trien_Khai_Du_An.md](file:///d:/Sources/ESP_Automation/docs/Ke_Hoach_Trien_Khai_Du_An.md) | Lộ trình thiết kế kiến trúc, tính năng nâng cao và các mốc triển khai. |
| 7 | 📐 [Kien_Truc_Kiem_Soat_Firmware_ESP32.md](file:///d:/Sources/ESP_Automation/docs/Kien_Truc_Kiem_Soat_Firmware_ESP32.md) | Tài liệu kiến trúc sơ đồ các module C++, luồng dữ liệu, Hysteresis & SSL/TLS. |
| 8 | ⏱️ [Lich_Su_Cap_Nhat_Ky_Thuat.md](file:///d:/Sources/ESP_Automation/docs/Lich_Su_Cap_Nhat_Ky_Thuat.md) | Nhật ký nâng cấp kỹ thuật, trạng thái các module code và nhật ký thay đổi. |

---

## 🏗️ 2. Cấu Trúc Tổng Quan Dự Án

```text
ESP_Automation/
├── firmware/                      # Mã nguồn C++ Modular cho ESP32 (PlatformIO)
│   ├── platformio.ini             # Cấu hình PlatformIO & nạp thư viện
│   ├── config.h                   # File cấu hình biến định danh, Wi-Fi & MQTT Topic
│   └── src/                       # Thư mục mã nguồn C++ (main.cpp, web_dashboard.h,...)
├── mobile_app/                    # Mã nguồn Flutter Cross-Platform (Android & iOS)
│   ├── build_android.bat          # Script 1-Click build APK & nạp trực tiếp qua ADB (Windows)
│   ├── build_android.ps1          # Script 1-Click PowerShell cho Android
│   ├── build_ios.sh               # Script 1-Click build & mở iPhone ảo Simulator (Mac)
│   ├── pubspec.yaml               # Thư viện Flutter (mqtt_client, shared_preferences,...)
│   └── lib/                       # Mã nguồn Flutter (Multiple Screen Clean Architecture)
│       ├── main.dart              # Entrypoint ứng dụng
│       ├── core/                  # Theme Dark Slate Glassmorphism & App Constants
│       ├── models/                # DeviceState & Telemetry Log History
│       ├── services/              # MqttService (MQTT TCP/SSL & Local HTTP Fallback)
│       └── screens/               # 4 Màn hình (Dashboard, History, Device Detail, Settings)
├── tools/                         # Bộ công cụ kiểm thử & mô phỏng
│   └── mock_esp32.py              # Script Python giả lập 100% ESP32 Cloud trên PC
├── docs/                          # Thư mục tài liệu hướng dẫn kỹ thuật chi tiết
├── setup_environment.bat          # Script 1-Click cài đặt tự động môi trường cho máy mới
├── install_flutter_windows.bat    # Script 1-Click tải & giải nén Flutter SDK
├── .env.example                   # Template file mẫu biến môi trường bảo mật
└── README.md                      # File tài liệu hướng dẫn tổng quan dự án
```

---

## ⚡ 3. Cài Đặt Nhanh 1-Click Cho Máy Mới (Getting Started)

### Step 1: Chuẩn bị môi trường tự động
1. Tải dự án về máy.
2. Nhấp đúp file **`setup_environment.bat`** tại thư mục gốc. Script sẽ tự động kiểm tra Python, Flutter, ADB và cài đặt toàn bộ thư viện cần thiết (`paho-mqtt`, `flutter pub get`).

### Step 2: Chạy Giả lập ESP32 trên PC (Không cần bo mạch thật)
```bash
python tools/mock_esp32.py
```

### Step 3: Build & Nạp App lên Điện thoại Android
Vào thư mục `mobile_app` và nhấp đúp file **`build_android.bat`**. Script sẽ tự động build APK và nạp trực tiếp vào điện thoại cắm nối qua USB/Wi-Fi ADB.

---

## 🛡️ 4. Bảo Mật & Quy Tắc Mã Nguồn
- File bảo mật cấu hình **`.env`** chứa mật khẩu Wi-Fi và tài khoản MQTT được cách ly hoàn toàn khỏi Git revision control (`.gitignore`).
- Dự án áp dụng quy tắc bảo vệ lịch sử Git commit theo hướng dẫn trong `.agents/AGENTS.md`.
