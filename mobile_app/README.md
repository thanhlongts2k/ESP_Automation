# ESP_Automation Mobile App (Android & iOS)

Ứng dụng di động thông minh được xây dựng trên nền tảng **Flutter**, phục vụ điều khiển và giám sát hệ thống **ESP32 IoT Automation** thời gian thực qua giao thức **MQTT (SSL/TLS & WebSockets)** kết hợp **Local REST API (Wi-Fi)**.

---

## 📱 1. Các Tính Năng Nổi Bật Của App

- 🌡️ **Giám sát Cảm biến Thời gian thực:** Hiển thị đồng hồ nhiệt độ (°C) và độ ẩm (%) từ cảm biến DHT22 cập nhật liên tục.
- 💡 **Điều khiển Đèn Chiếu Sáng (Relay 1):** Công tắc gạt Toggle Switch phản hồi tức thì (< 0.1s).
- 🌀 **Điều khiển Quạt Thông Gió (Relay 2):** Công tắc gạt Toggle Switch bật/tắt quạt.
- 📡 **Chế Độ Kép Thông Minh (Hybrid Mode):**
  - **Chế độ MQTT (Internet):** Kết nối về MQTT Broker (`broker.emqx.io:1883` hoặc Server Nginx SSL `8883`) giúp điều khiển & giám sát từ bất kỳ đâu trên thế giới.
  - **Chế độ Local Wi-Fi (Tự động chuyển đổi):** Khi mất mạng Internet hoặc đứt kết nối MQTT, App tự động chuyển sang gọi REST API nội bộ (`http://192.168.1.50` hoặc `http://esp32.local`) giúp hệ thống **không bao giờ bị gián đoạn hoạt động**.

---

## 🛠️ 2. Môi Trường Phát Triển & Thư Viện (Tech Stack)

- **Framework:** Flutter SDK >=3.0.0
- **Ngôn ngữ:** Dart
- **Dependencies:**
  - `mqtt_client: ^10.0.0` -> Kết nối và quản lý luồng dữ liệu MQTT.
  - `http: ^1.1.0` -> Gửi REST API giao tiếp nội bộ với ESP32.
  - `cupertino_icons: ^1.0.2` & `google_fonts: ^5.1.0` -> Giao diện UI Dark Mode Glassmorphism.

---

## 🚀 3. Hướng Dẫn Biên Dịch & Chạy Ứng Dụng

### Bước 1: Tải phụ thuộc Flutter
```bash
cd mobile_app
flutter pub get
```

### Bước 2: Biên dịch ứng dụng (Build)
- **Chạy trực tiếp thử nghiệm (Debug Mode):**
  ```bash
  flutter run
  ```
- **Build file APK Debug / Release:**
  ```bash
  flutter build apk --release
  ```

---

## ⚡ 4. LỆNH CÀI ĐẶT ADB SIÊU TỐC VÀO ĐIỆN THOẠI (KHÔNG CẦN REBUILD)

Khi file `.apk` đã được build sẵn, bạn có thể đẩy thẳng vào điện thoại nối dây/Wi-Fi ADB chỉ trong **2 giây** bằng lệnh:

```bash
# Cài đặt file APK Debug vào điện thoại qua ADB
adb install -r build\app\outputs\flutter-apk\app-debug.apk

# Hoặc cài đặt file APK Release
adb install -r build\app\outputs\flutter-apk\app-release.apk
```

> **📌 LƯU Ý KHI DÙNG VỚI ĐIỆN THOẠI XIAOMI / REDMI / POCO:**
> - Nếu gặp lỗi `[INSTALL_FAILED_USER_RESTRICTED]`, hãy vào **Cài đặt (Settings) -> Tùy chọn nhà phát triển (Developer Options)**.
> - Bật công tắc **Cài đặt qua USB (Install via USB)** và **Gỡ lỗi USB (USB Debugging)**.
> - Khi gõ lệnh `adb install`, nhìn màn hình điện thoại và bấm **Cho phép / Install**.

---

## 📂 5. Cấu Trúc Mã Nguồn Mobile App

```text
mobile_app/
├── android/               # Cấu hình nền tảng Android (Gradle, Manifest, Permissions)
├── ios/                   # Cấu hình nền tảng iOS (Xcode, Runner)
├── web/                   # Cấu hình nền tảng Web HTML5 PWA
├── lib/
│   └── main.dart          # Mã nguồn chính (Giao diện Dashboard, MQTT Client & Local REST API)
├── pubspec.yaml           # File quản lý thư viện phụ thuộc Flutter
└── README.md              # File tài liệu hướng dẫn ứng dụng
```
