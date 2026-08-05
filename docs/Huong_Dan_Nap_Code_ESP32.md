# Hướng Dẫn Chi Tiết Biên Dịch, Nạp Code & Khởi Chạy ESP32

Tài liệu hướng dẫn từng bước cách nạp chương trình vào bo mạch **ESP32 Type-C** và khởi chạy Web Server Dashboard nội bộ (Phương án 3).

---

## 🛠️ PHƯƠNG PHÁP 1: NẠP CODE BẰNG VS CODE + PLATFORMIO (KHUYÊN DÙNG ★)

Project **ESP_Automation** đã được cấu hình sẵn môi trường PlatformIO tự động hóa 100%.

### Các Bước Thực Hiện:

1. **Điền thông tin Wi-Fi của bạn:**
   - Mở file `.env` ở thư mục gốc dự án:
     ```env
     WIFI_SSID="Tên_WiFi_Nhà_Bạn"
     WIFI_PASSWORD="Mật_Khẩu_WiFi_Nhà_Bạn"
     ```
2. **Cắm mạch ESP32 vào Máy tính:**
   - Dùng cáp Type-C cắm từ ESP32 vào cổng USB máy tính.
3. **Mở Project trong VS Code:**
   - Cài Extension **PlatformIO IDE** trong VS Code (nếu chưa có).
   - Chọn `File -> Open Folder` ➔ Trỏ tới thư mục `D:\Sources\ESP_Automation`.
4. **Bấm Nạp Code (Upload):**
   - Nhấp vào biểu tượng **PlatformIO** (Hình con kiến ở thanh bên trái).
   - Bấm nút **Build** (để kiểm tra lỗi) ➔ Bấm nút **Upload** (để nạp code sang ESP32).
   - *(Hoặc mở Terminal trong VS Code gõ lệnh: `pio run -t upload`)*.
5. **Mở Serial Monitor xem log:**
   - Bấm nút **Serial Monitor** (115200 baud).
   - Màn hình sẽ in ra IP của ESP32 (Ví dụ: `IP Address: 192.168.1.50`).

---

## 🛠️ PHƯƠNG PHÁP 2: NẠP CODE BẰNG ARDUINO IDE

Nếu bạn thích dùng phần mềm **Arduino IDE**:

1. **Cài đặt Board ESP32 trong Arduino IDE:**
   - Vào `File -> Preferences` ➔ Tại ô *Additional Board Manager URLs*, dán link:
     `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Vào `Tools -> Board -> Boards Manager` ➔ Tìm `esp32` và bấm **Install**.
2. **Cài đặt các Thư viện cần thiết:**
   - Vào `Tools -> Manage Libraries`, tìm và cài các thư viện sau:
     - `DHT sensor library` (bởi Adafruit)
     - `PubSubClient` (bởi Nick O'Leary)
     - `ArduinoJson` (bởi Benoit Blanchon)
3. **Mở File Code & Chỉnh thông tin Wi-Fi:**
   - Mở file `firmware/config.h`.
   - Điền tên Wi-Fi và Mật khẩu vào dòng `WIFI_SSID` và `WIFI_PASSWORD`.
4. **Chọn Board & Nạp Code:**
   - Chọn `Tools -> Board -> esp32 -> ESP32 Dev Module`.
   - Chọn `Tools -> Port -> COMx` (Cổng COM của ESP32).
   - Bấm nút **Upload** (Mũi tên hướng sang phải) để nạp code.

---

## 🌐 CÁCH TRUY CẬP & KHỞI CHẠY WEB DASHBOARD (PHƯƠNG ÁN 3)

Sau khi nạp code thành công và ESP32 đã kết nối Wi-Fi:

1. Đảm bảo Điện thoại hoặc Laptop của bạn đang kết nối **chung mạng Wi-Fi** với ESP32.
2. Mở trình duyệt Web (Chrome, Safari, Edge...), gõ một trong hai địa chỉ sau:
   - Cách 1 (Dễ nhất): **`http://esp32.local`**
   - Cách 2 (Dùng địa chỉ IP): **`http://192.168.1.50`** *(Thay 192.168.1.50 bằng IP in ra trên Serial Monitor)*.
3. Giao diện **ESP32 Local Control Dashboard** hiện lên ngay lập tức!
   - Xem Nhiệt độ & Độ ẩm cập nhật tự động mỗi 2 giây.
   - Bật/Tắt công tắc Switch để điều khiển Đèn & Quạt trực tiếp.

---

## ❓ XỬ LÝ SỰ CỐ THƯỜNG GẶP (TROUBLESHOOTING)

- **Máy tính không nhận cổng COM khi cắm ESP32:**
  - Tải và cài đặt **Driver CH340** cho Windows ([Tải tại đây](http://www.wch-ic.com/downloads/CH341SER_EXE.html)).
- **Trình duyệt gõ `esp32.local` không ra:**
  - Một số máy Android cũ không hỗ trợ mDNS. Hãy xem địa chỉ IP in ra trên Serial Monitor (ví dụ `192.168.1.50`) và gõ trực tiếp IP đó vào trình duyệt.
