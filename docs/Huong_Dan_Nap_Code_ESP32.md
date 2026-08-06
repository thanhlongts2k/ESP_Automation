# Hướng Dẫn Chi Tiết Biên Dịch, Nạp Code & Khởi Chạy ESP32

Tài liệu hướng dẫn chi tiết từng bước cách biên dịch chương trình, nạp firmware vào bo mạch **ESP32 Type-C** và khởi chạy Web Server Dashboard nội bộ (Phương án 3).

---

## 🛠️ PHƯƠNG PHÁP 1: NẠP CODE BẰNG VS CODE + PLATFORMIO (KHUYÊN DÙNG ★)

Dự án **ESP_Automation** đã được cấu hình sẵn môi trường PlatformIO tự động đọc biến từ file `.env` lúc biên dịch.

### 📋 Hướng Dẫn Từng Bước Chi Tiết:

#### 1️⃣ Bước 1: Điền thông tin Wi-Fi nhà bạn
- Mở file **[.env](file:///d:/Sources/ESP_Automation/.env)** ở thư mục gốc dự án:
  ```env
  WIFI_SSID="Tên_WiFi_Nhà_Bạn"
  WIFI_PASSWORD="Mật_Khẩu_WiFi_Nhà_Bạn"
  ```

#### 2️⃣ Bước 2: Cắm bo mạch ESP32 vào máy tính
- Dùng cáp Type-C cắm từ con ESP32 vào cổng USB máy tính.
- *(Nếu máy tính chưa nhận cổng COM, tải driver CH340 ở phần Xử lý sự cố cuối bài)*.

#### 3️⃣ Bước 3: Cài Extension PlatformIO trên VS Code
- Bấm tổ hợp phím **`Ctrl + Shift + X`** (hoặc nhấp vào biểu tượng 4 ô vuông ở thanh công cụ góc trái VS Code).
- Tìm kiếm từ khóa: **`PlatformIO IDE`** *(Có biểu tượng con kiến màu cam 🐜)*.
- Bấm nút **Install** và chờ 1-2 phút cho VS Code tải hoàn tất.

#### 4️⃣ Bước 4: Thực hiện Build & Upload (Nạp Code)
- Nhấp vào **Biểu tượng Con Kiến 🐜 (PlatformIO)** ở thanh bên trái VS Code.
- Tại mục **Project Tasks ➔ esp32dev**:
  - 🔨 Bấm nút **`Build`** (để kiểm tra xem biên dịch C++ có báo lỗi không).
  - ⬆️ Bấm nút **`Upload`** (để đẩy mã nguồn vào bộ nhớ ESP32 qua cổng USB).
  - 🖥️ Bấm nút **`Monitor`** (để mở màn hình xem log Serial baud rate 115200).

---

## 🛠️ PHƯƠNG PHÁP 2: NẠP CODE BẰNG ARDUINO IDE

Nếu bạn muốn sử dụng phần mềm **Arduino IDE**:

1. **Cài đặt Board ESP32 trong Arduino IDE:**
   - Vào `File -> Preferences` ➔ Tại ô *Additional Board Manager URLs*, dán link:
     `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Vào `Tools -> Board -> Boards Manager` ➔ Tìm `esp32` và bấm **Install**.
2. **Cài đặt 3 Thư viện phụ thuộc:**
   - Vào `Tools -> Manage Libraries`, tìm và cài các thư viện sau:
     - `DHT sensor library` (bởi Adafruit)
     - `PubSubClient` (bởi Nick O'Leary)
     - `ArduinoJson` (bởi Benoit Blanchon)
3. **Mở File Code & Điền Wi-Fi:**
   - Mở file `firmware/config.h`.
   - Điền tên Wi-Fi và Mật khẩu vào dòng `WIFI_SSID` và `WIFI_PASSWORD`.
4. **Chọn Board & Nạp Code:**
   - Vào `Tools -> Board -> esp32 -> ESP32 Dev Module`.
   - Vào `Tools -> Port -> COMx` (Chọn đúng cổng COM của ESP32).
   - Bấm nút **Upload (Mũi tên hướng sang phải ➔)** để nạp code.

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
  - Một số máy Android cũ không hỗ trợ mDNS. Hãy xem địa chỉ IP in ra trên Serial Monitor (ví dụ `192.168.1.50`) và gõ trực tiếp IP đó vào trình duyệt.
