# Danh Sách Thiết Bị ESP32 IoT - Đã Mua (ESP_Automation)

Tài liệu lưu trữ danh sách linh kiện, thông số kỹ thuật và tổng chi phí mua sắm bộ Kit thử nghiệm ESP32 IoT Automation.

---

## 🛒 1. Danh Sách Chi Tiết Linh Kiện Đã Đặt Hàng

| STT | Tên Linh Kiện / Thiết Bị | Thông Số Kỹ Thuật Chính | Số Lượng | Đơn Giá Tham Khảo | Mục Đích Sử Dụng |
| :---: | :--- | :--- | :---: | :---: | :--- |
| **1** | **Kit ESP32 WiFi + Bluetooth CH340 Type-C** | - Chip Tensilica Xtensa Dual-Core 240MHz<br>- RAM 520KB, Flash 4MB<br>- Cổng nạp Type-C, Chip nạp CH340<br>- Form factor: DIP 30-pin | 1 | ~70.282đ | Bo mạch vi xử lý trung tâm (CPU + WiFi + Bluetooth), thu thập dữ liệu và gửi/nhận lệnh MQTT over SSL. |
| **2** | **Chân Đế Mở Rộng ESP32 (30P)** | - Dành riêng cho ESP32 DevKit V1 30 chân<br>- Mở rộng hàng chân đực (VCC/GND/Signal)<br>- Có Jack nguồn tròn DC (6-12V) + Cổng Type-C | 1 | ~28.598đ | Bo cắm mở rộng giúp cắm dây nối cảm biến và relay cực kỳ tiện lợi mà không cần hàn mạch. |
| **3** | **Module Relay 2 Kênh 5V (Có Opto Cách Ly)** | - Điện áp nuôi: 5VDC<br>- Có Opto cách ly bảo vệ an toàn<br>- Đóng ngắt tải: 10A / 250VAC (tới 2200W) | 1 | ~36.064đ | Công tắc thông minh giúp ESP32 đóng/ngắt các thiết bị điện 220V AC (Đèn, Quạt, Máy bơm...). |
| **4** | **Cảm Biến Nhiệt Độ & Độ Ẩm DHT22 (AM2302)** | - Đo nhiệt độ: -40°C đến 80°C (Sai số ±0.5°C)<br>- Đo độ ẩm: 0% đến 100% (Sai số ±2%)<br>- Chuẩn giao tiếp: Digital 1-wire | 1 | ~45.000đ | Thu thập dữ liệu nhiệt độ và độ ẩm không khí môi trường để đẩy lên Server qua MQTT. |
| **5** | **Dây Cắm Testboard 30cm (Cái - Cái)** | - Loại đầu nối: Female to Female (Cái - Cái)<br>- Số lượng: Tép 40 sợi dính liền | 1 | ~33.000đ | Dây dẫn nối tín hiệu chính từ Chân Đế ESP32 (Đực) sang Relay (Đực) và Cảm biến (Đực). |
| **6** | **Dây Cắm Testboard 30cm (Đực - Cái)** | - Loại đầu nối: Male to Female (Đực - Cái)<br>- Số lượng: Tép 10 sợi | 1 | ~12.000đ | Dây dẫn nối dự phòng cho các cảm biến mở rộng khác. |

---

## 💰 2. Tổng Kết Chi Phí Mua Sắm

- **Tổng số đơn hàng:** 6 đơn
- **Tổng chi phí trọn bộ:** **~217.000 VNĐ**

---

## 🚀 3. Kế Hoạch Triển Khai Tiếp Theo (Khi Nhận Hàng)

1. **Lắp ráp phần cứng:**
   - Cắm Kit ESP32 Type-C vào Chân Đế Mở Rộng 30P.
   - Nối dây tín hiệu từ Chân Đế ➔ Module Relay 2 Kênh (Kênh 1: Đèn, Kênh 2: Quạt).
   - Nối dây tín hiệu từ Chân Đế ➔ Cảm biến DHT22 (Chân VCC, GND, DATA).

2. **Lập trình & Nạp Code (Arduino IDE / ESP-IDF):**
   - Cài đặt Driver CH340 trên Windows.
   - Cấu hình thư viện `WiFi.h`, `PubSubClient.h` (MQTT), `DHT.h`.
   - Kết nối Wi-Fi ➔ Kết nối **MQTT SSL/TLS (Port 8883)** qua Server Nginx (`api-vending.doanhnghiep.com`).

3. **Chức năng thực thi:**
   - **Publish:** Định kỳ đẩy dữ liệu JSON (Nhiệt độ, Độ ẩm) từ DHT22 lên MQTT Broker.
   - **Subscribe:** Nhận lệnh đóng/mở từ Server/App để kích hoạt Relay bật/tắt Đèn & Quạt.

> 💡 **LƯU Ý VỀ TẢI ĐIỆN (BÓNG ĐÈN / QUẠT 220V AC):**
> Bạn **KHÔNG CẦN mua thêm** thiết bị điện 220V AC. Khi thử nghiệm giai đoạn đầu, Module Relay 2 Kênh đã có sẵn **2 Đèn LED trạng thái màu Đỏ/Xanh** và cuộn hút cơ tạo ra tiếng **"tạch" (Click)** đóng ngắt tiếp điểm. Bạn có thể test trực quan 100% việc Bật/Tắt qua App Flutter hay Web Dashboard dựa vào tiếng "tạch" và Đèn LED trên Relay mà không cần cắm điện lưới 220V. Khi triển khai thực tế vào nhà cửa, bạn chỉ cần trích dây nóng (L) của đèn/quạt có sẵn cắm vào cổng COM & NO của Relay.
