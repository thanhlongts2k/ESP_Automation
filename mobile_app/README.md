# ESP_Automation Mobile App (Android & iOS)

Ứng dụng di động điều khiển và giám sát hệ thống **ESP_Automation** thời gian thực qua giao thức **MQTT (SSL/TLS & WebSockets)** và **Local REST API (Wi-Fi)**.

---

## 📱 1. Các Tính Năng Chính Của App

- 🌡️ **Giám sát thời gian thực:** Hiển thị chỉ số Nhiệt độ (°C) và Độ ẩm (%) từ cảm biến DHT22.
- 💡 **Điều khiển Đèn (Relay 1):** Công tắc gạt Toggle Switch bật/tắt Đèn.
- 🌀 **Điều khiển Quạt (Relay 2):** Công tắc gạt Toggle Switch bật/tắt Quạt.
- 📡 **Chế độ Kép (Hybrid Mode):**
  - **Chế độ MQTT (Internet):** Điều khiển & xem chỉ số từ bất kỳ đâu trên thế giới.
  - **Chế độ Local Wi-Fi:** Tự động gửi REST API trực tiếp tới `http://esp32.local` khi chung Wi-Fi.

---

## 🛠️ 2. Môi Trường Phát Triển & Thư Viện (Stack)

- **Framework:** Flutter (Dart) 3.x
- **MQTT Client:** `mqtt_client: ^10.0.0`
- **HTTP Client:** `http: ^1.1.0`
- **UI Components:** `flutter_spinkit`, `google_fonts`

---

## 🚀 3. Hướng Dẫn Biên Dịch & Build APK Android

### Cài đặt phụ kiện:
```bash
cd mobile_app
flutter pub get
```

### Chạy ứng dụng thử nghiệm (Debug):
```bash
flutter run
```

### Build file cài đặt Android APK Release:
```bash
flutter build apk --release
```
*(File APK xuất ra nằm tại `build/app/outputs/flutter-apk/app-release.apk`)*.
