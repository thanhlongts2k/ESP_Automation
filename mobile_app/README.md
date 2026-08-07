# ESP_Automation Mobile App (Android & iOS)

Ứng dụng di động thông minh được xây dựng trên nền tảng **Flutter**, phục vụ điều khiển và giám sát hệ thống **ESP32 IoT Automation** thời gian thực qua giao thức **MQTT Cloud Enterprise (SSL/TLS & WebSockets)** kết hợp **Local REST API (Wi-Fi)**.

---

## 📱 1. Các Tính Năng Nổi Bật Của App

- 🌡️ **Giám sát Cảm biến Thời gian thực:** Hiển thị Nhiệt độ (°C), Độ ẩm (%) và Độ ẩm đất (%) cập nhật liên tục.
- 📶 **Vạch Sóng Wi-Fi & Uptime:** Hiển thị cường độ sóng Wi-Fi RSSI (dBm) và thời gian hoạt động liên tục Uptime.
- 🟢 **Báo Trạng Thái LWT (Last Will and Testament):** Tự động nhận diện ESP32 Online / Offline tức thì qua tin nhắn di chúc LWT.
- 💡 **Điều khiển Đèn Chiếu Sáng (Relay 1):** Công tắc gạt Toggle Switch phản hồi tức thì (< 0.1s).
- 🌀 **Điều khiển Quạt Thông Gió (Relay 2):** Công tắc gạt Toggle Switch bật/tắt quạt.
- 📡 **Chế Độ Kép Thông Minh (Hybrid Mode):**
  - **Chế độ MQTT Cloud (Dual Fallback):** Ưu tiên kết nối TCP 1883, tự động chuyển sang WebSockets 8083 khi bị 4G/tường lửa nhà mạng chặn.
  - **Chế độ Local Wi-Fi (Tự động chuyển đổi):** Khi mất mạng Internet, App tự động chuyển sang gọi REST API nội bộ (`http://192.168.1.50` hoặc `http://esp32.local`).

---

## ⚡ 2. CÁCH BUILD & NẠP APP 1-CLICK DỄ NHẤT

### 🤖 Dành cho Android (Máy Windows):
1. **Nếu chưa có Flutter SDK**: Nhấp đúp file **`install_flutter_windows.bat`** tại thư mục gốc dự án để tự động tải & giải nén Flutter SDK vào `C:\flutter` trong 1-click.
2. **Build & Nạp App**: Mở thư mục `mobile_app` và nhấp đúp file **`build_android.bat`** *(hoặc chạy `.\build_android.ps1` trong PowerShell)*.
👉 Script sẽ tự động: Kiểm tra Windows Developer Mode ➔ Build Release APK ➔ Nhận diện điện thoại qua ADB ➔ Nạp đè trực tiếp vào điện thoại chỉ trong vài giây!

### 🍏 Dành cho iPhone (Máy Mac):
Mở thư mục `mobile_app` trên Mac và gõ lệnh:
- 📄 `bash build_ios.sh`
👉 Script sẽ tự động nhận diện iPhone thật qua USB hoặc bật màn hình iPhone ảo **iOS Simulator** (`open -a Simulator`).

---

## 🛠️ 3. Môi Trường Phát Triển & Thư Viện (Tech Stack)

- **Framework:** Flutter SDK >=3.0.0
- **Ngôn ngữ:** Dart
- **Dependencies:**
  - `mqtt_client: ^10.0.0` -> Kết nối và quản lý luồng dữ liệu MQTT.
  - `http: ^1.1.0` -> Gửi REST API giao tiếp nội bộ với ESP32.
  - `cupertino_icons: ^1.0.2` & `google_fonts: ^5.1.0` -> Giao diện UI Dark Mode Glassmorphism.

---

## 📂 4. Cấu Trúc Mã Nguồn Mobile App

```text
mobile_app/
├── build_android.bat      # Script 1-Click build APK & nạp Android qua ADB
├── build_android.ps1      # Script 1-Click PowerShell cho Android
├── build_ios.sh           # Script 1-Click build & nạp iPhone / iOS Simulator trên Mac
├── android/               # Cấu hình Android (AndroidManifest.xml cấp quyền INTERNET)
├── ios/                   # Cấu hình iOS (Xcode, Runner)
├── web/                   # Cấu hình Web HTML5 PWA
├── lib/
│   └── main.dart          # Mã nguồn chính (Dashboard, MQTT Client & Local REST API)
├── pubspec.yaml           # File quản lý thư viện phụ thuộc Flutter
└── README.md              # File tài liệu hướng dẫn ứng dụng
```
