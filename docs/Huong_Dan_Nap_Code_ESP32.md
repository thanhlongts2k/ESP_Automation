# Hướng Dẫn Chi Tiết Biên Dịch, Nạp Code & Khởi Chạy ESP32 Enterprise Firmware

Tài liệu hướng dẫn chi tiết từng bước cách biên dịch chương trình **Kiến Trúc Modular C++**, nạp firmware vào bo mạch **ESP32 Type-C** và khởi chạy Web Server Dashboard nội bộ (Phương án 3) kết hợp MQTT Cloud Enterprise.

---

## 🏗️ NGUYÊN TẮC CẤU TRÚC MÃ NGUỒN MODULAR C++

Dự án **ESP_Automation** đã được nâng cấp lên **Kiến trúc C++ Modular Enterprise chuẩn công nghiệp** với toàn bộ mã nguồn nằm gọn gàng trong `firmware/src/`:

```text
firmware/
├── platformio.ini               ← Cấu hình biên dịch PlatformIO (src_dir = firmware/src)
├── extra_script.py              ← Script tự động tiêm biến .env vào GCC Compiler
└── src/
    ├── main.cpp                 ← Entrypoint tinh gọn (~50 dòng code)
    ├── config.h                 ← Cấu hình thông số & Định nghĩa Topic chuẩn esp32/{device_id}/...
    ├── wifi_manager.h / .cpp    ← Quản lý Wi-Fi tự khôi phục phi bất đồng bộ (Non-blocking)
    ├── sensor_manager.h / .cpp  ← Đọc DHT22 & Cảm biến độ ẩm đất Analog ADC
    ├── relay_controller.h / .cpp← Điều khiển Đèn, Quạt & Logic Hysteresis tự động 32/29°C
    ├── mqtt_manager.h / .cpp    ← Dynamic Topics, LWT Status, Retained States & Exponential Backoff
    └── web_dashboard.h          ← Giao diện Web Local Glassmorphism (PROGMEM)
```

---

## 🛠️ PHƯƠNG PHÁP 1: NẠP CODE BẰNG VS CODE + PLATFORMIO (KHUYÊN DÙNG ★)

PlatformIO đã được quy hoạch tự động đọc biến môi trường từ file `.env` lúc biên dịch.

### 📋 Hướng Dẫn Từng Bước Chi Tiết:

#### 1️⃣ Bước 1: Điền thông tin Wi-Fi và MQTT trong `.env`
- Mở file **[.env](file:///d:/Sources/ESP_Automation/.env)** ở thư mục gốc dự án:
  ```env
  WIFI_SSID="Tên_WiFi_Nhà_Bạn"
  WIFI_PASSWORD="Mật_Khẩu_WiFi_Nhà_Bạn"

  MQTT_SERVER1="broker.emqx.io"
  MQTT_PORT1=1883

  MQTT_SERVER=MQTT_SERVER1
  MQTT_PORT=MQTT_PORT1
  DEVICE_ID="ESP32_Automation_01"
  ```

#### 2️⃣ Bước 2: Cắm bo mạch ESP32 vào máy tính
- Dùng cáp Type-C cắm từ con ESP32 vào cổng USB máy tính.
- *(Nếu máy tính chưa nhận cổng COM, tải driver CH340 ở phần Xử lý sự cố cuối bài)*.

#### 3️⃣ Bước 3: Cài Extension PlatformIO trên VS Code
- Bấm tổ hợp phím **`Ctrl + Shift + X`**.
- Tìm kiếm từ khóa: **`PlatformIO IDE`** *(Có biểu tượng con kiến màu cam 🐜)* và bấm **Install**.

#### 4️⃣ Bước 4: Thực hiện Build & Upload (Nạp Code)
- Nhấp vào **Biểu tượng Con Kiến 🐜 (PlatformIO)** ở thanh bên trái VS Code.
- Tại mục **Project Tasks ➔ esp32dev**:
  - 🔨 Bấm nút **`Build`** (để kiểm tra xem biên dịch Modular C++ có báo lỗi không).
  - ⬆️ Bấm nút **`Upload`** (để đẩy mã nguồn vào bộ nhớ ESP32 qua cổng USB).
  - 🖥️ Bấm nút **`Monitor`** (để mở màn hình xem log Serial baud rate 115200).

---

## 🌐 CÁCH TRUY CẬP & KHỞI CHẠY WEB DASHBOARD (PHƯƠNG ÁN 3)

Sau khi nạp code thành công và ESP32 đã kết nối Wi-Fi:

1. Đảm bảo Điện thoại hoặc Laptop của bạn đang kết nối **chung mạng Wi-Fi** với ESP32.
2. Mở trình duyệt Web (Chrome, Safari, Edge...), gõ địa chỉ:
   - Cách 1 (Dễ nhất): **`http://esp32.local`**
   - Cách 2 (Dùng địa chỉ IP): **`http://192.168.1.50`** *(Thay bằng IP thực tế in ra trên Serial Monitor)*.
3. Giao diện **ESP32 Local Control Dashboard** hiện lên ngay lập tức!
   - Xem Nhiệt độ & Độ ẩm cập nhật tự động mỗi 2 giây.
   - Bật/Tắt công tắc Switch để điều khiển Đèn & Quạt trực tiếp.

---

## ❓ XỬ LÝ SỰ CỐ THƯỜNG GẶP (TROUBLESHOOTING)

- **Máy tính không nhận cổng COM khi cắm ESP32:**
  - Tải và cài đặt **Driver CH340** cho Windows ([Tải tại đây](http://www.wch-ic.com/downloads/CH341SER_EXE.html)).
- **Trình duyệt gõ `esp32.local` không ra:**
  - Một số máy Android cũ không hỗ trợ mDNS. Hãy xem địa chỉ IP in ra trên Serial Monitor và gõ trực tiếp IP đó vào trình duyệt.
