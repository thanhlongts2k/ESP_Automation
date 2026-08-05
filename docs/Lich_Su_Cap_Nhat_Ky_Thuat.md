# Nhật Ký Kỹ Thuật & Trạng Thái Mã Nguồn Dự Án ESP_Automation

Tài liệu này lưu trữ lịch sử cập nhật kỹ thuật, trạng thái các tính năng đã được lập trình sẵn trong dự án **ESP_Automation**. 
**Mục đích:** Giúp tra cứu nhanh trạng thái code của từng chức năng mà không cần đọc lại toàn bộ dự án (tiết kiệm token và thời gian).

---

## 📅 NGUYÊN TẮC QUẢN LÝ TRẠNG THÁI (STATUS KEY)
- 🟢 **[COMPLETED - READY]**: Đã viết xong mã nguồn C++, cấu hình hoàn chỉnh, sẵn sàng nạp.
- 🟡 **[TESTING - PENDING HW]**: Đã có code sẵn, đang chờ linh kiện về để cắm nạp thử nghiệm.
- 🔴 **[PLANNED]**: Nằm trong kế hoạch, chưa viết code.

---

## ⏱️ LỊCH SỬ CẬP NHẬT KỸ THUẬT (TIMELOGS)

### 📌 [2026-08-05 16:53] - Tạo Tài Liệu Sơ Đồ Hướng Dẫn Nối Dây Phần Cứng
- **Trạng thái:** 🟢 **[COMPLETED - READY]**
- **Các File Liên Quan:**
  - `docs/Huong_Dan_Noi_Day_Phan_Cung.md` -> Sơ đồ ASCII, bảng chân cắm Pinout chi tiết cho ESP32 30P, Relay 2K, DHT22, DS18B20, Cảm biến đất v1.2 và quy tắc đấu điện 220V AC.

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
| **1** | **Sơ Đồ Hướng Dẫn Nối Dây Phần Cứng** | 🟢 READY | `docs/Huong_Dan_Noi_Day_Phan_Cung.md` | - Sơ đồ chân cắm ESP32 30P, Relay 2K, DHT22, Cảm biến đất, DS18B20<br>- Quy tắc nối an toàn điện 220V AC. |
| **2** | **Hướng Dẫn Nạp Code & Khởi Chạy** | 🟢 READY | `docs/Huong_Dan_Nap_Code_ESP32.md` | - Hướng dẫn nạp code qua PlatformIO & Arduino IDE<br>- Cách mở Web Dashboard `http://esp32.local`. |
| **3** | **Web Dashboard Local Wi-Fi (Phương án 3)** | 🟢 READY | `firmware/web_dashboard.h`<br>`firmware/ESP_Automation.ino` | - Mở trình duyệt gõ: `http://esp32.local` hoặc IP<br>- Giao diện Dark Mode Glassmorphism tự cập nhật Nhiệt độ/Độ ẩm Realtime<br>- Toggle Switch bật/tắt Đèn & Quạt trực tiếp. |
| **4** | **mDNS Responder (`esp32.local`)** | 🟢 READY | `firmware/ESP_Automation.ino`<br>*(Hàm `setupLocalWebDashboard()`)* | - Thư viện: `ESPmDNS.h`<br>- Cho phép điện thoại/laptop chung Wi-Fi gõ `http://esp32.local` không cần nhớ IP. |
| **5** | **REST API Nội Bộ (/api/data, /api/relay)** | 🟢 READY | `firmware/ESP_Automation.ino` | - `/api/data`: Trả về JSON nhiệt độ, độ ẩm, relay state.<br>- `/api/relay1?state=ON`: Đóng ngắt Đèn.<br>- `/api/relay2?state=ON`: Đóng ngắt Quạt. |
| **6** | **Script Giả Lập ESP32 trên PC** | 🟢 READY | `tools/mock_esp32.py` | - Chạy `python tools/mock_esp32.py` để test luồng MQTT gửi sensor/nhận lệnh Relay trên PC. |
| **7** | **Đọc Cảm Biến DHT22** | 🟢 READY | `firmware/ESP_Automation.ino` | - Pin: GPIO 23<br>- Thư viện: `DHT.h`<br>- Chu kỳ đọc: Mỗi 2 giây/lần. |
| **8** | **Điều Khiển Relay 2 Kênh (Đèn & Quạt)** | 🟢 READY | `firmware/ESP_Automation.ino` | - Pin Relay 1 (Đèn): GPIO 18<br>- Pin Relay 2 (Quạt): GPIO 19<br>- Kích mức THẤP (LOW Active). |
| **9** | **Kết Nối Wi-Fi Auto-Reconnect** | 🟢 READY | `firmware/ESP_Automation.ino` | - Thư viện: `WiFi.h`<br>- Đọc SSID/PASS từ file `.env`. |
| **10** | **MQTT Client Over SSL/TLS Port 8883** | 🟢 READY | `firmware/ESP_Automation.ino` | - Mã hóa SSL với Root CA `DigiCertGlobalRootG2.pem`<br>- Thư viện: `PubSubClient.h` + `WiFiClientSecure.h`. |
| **11** | **OTA 3 Phương Thức (LAN, Web, HTTPS Cloud)** | 🟢 READY | `firmware/ESP_Automation.ino` | - Arduino OTA + Web OTA + HTTPS Cloud OTA.<br>- Có Anti-brick & Auto Rollback khi lỗi. |
| **12** | **Quản Lý Biến Môi Trường (.env)** | 🟢 READY | `platformio.ini`<br>`extra_script.py` | - Tiêm biến `.env` tự động vào C++ Compiler lúc build.<br>- Đã chặn `.env` trong `.gitignore`. |
