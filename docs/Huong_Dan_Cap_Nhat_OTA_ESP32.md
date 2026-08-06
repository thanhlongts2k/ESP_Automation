# Kế Hoạch & Hướng Dẫn Chi Tiết Cập Nhật Firmware ESP32 Qua Wi-Fi (OTA - Over The Air)

Tài liệu thiết kế kiến trúc và quy trình triển khai tính năng nạp/cập nhật chương trình từ xa cho **ESP32** qua mạng Wi-Fi / Internet mà không cần cắm cáp USB, tích hợp trong **Kiến trúc C++ Modular Enterprise**.

---

## 💡 1. Tổng Quan Về OTA (Over-The-Air Update)

### OTA là gì?
**OTA (Over-The-Air)** là giải pháp cho phép truyền tải và ghi bản nạp chương trình mới (file `firmware.bin`) vào bộ nhớ Flash của ESP32 thông qua mạng không dây (Wi-Fi / Internet).

### Tại sao OTA lại bắt buộc phải có?
- Khi ESP32 được đóng vào hộp bảo vệ hoặc lắp đặt ở vị trí trên cao/ngoài ruộng, việc tháo hộp ra cắm cáp USB nạp lại code rất tốn thời gian.
- Cho phép sửa lỗi (bug fixes), nâng cấp tính năng mới từ xa thông qua ứng dụng điện thoại hoặc lệnh MQTT.

---

## 🏗️ 2. Các Phương Thức Triển Khai OTA Cho ESP32

Dự án **ESP_Automation** quy hoạch 3 phương thức OTA tương ứng với từng giai đoạn phát triển:

```text
                                       ┌──> [1. Arduino OTA] (Mạng LAN nội bộ - Dành cho DEV/Lab)
                                       │
[ Cập Nhật Firmware Qua Wi-Fi (OTA) ] ┼──> [2. WebServer OTA] (Upload file .bin qua Trang Web nhúng)
                                       │
                                       └──> [3. HTTPS Cloud OTA] (Cập nhật tự động từ xa qua MQTT & Server Nginx) ★
```

---

### 🌐 Phương Thức 1: Arduino OTA (Nạp qua mạng LAN nội bộ)
- **Mục đích:** Dùng trong quá trình lập trình thử nghiệm tại Lab mà không cần tháo dây cáp.
- **Nguyên lý:** ESP32 khởi chạy dịch vụ lắng nghe cổng nạp trong cùng mạng Wi-Fi LAN với Máy tính. Từ Arduino IDE / VS Code, chọn Port là Địa chỉ IP của ESP32 thay vì cổng COM USB và bấm Nạp.
- **Thư viện sử dụng:** `<WiFi.h>`, `<ESPmDNS.h>`, `<ArduinoOTA.h>`

---

### 💻 Phương Thức 2: Web Server OTA (Cập nhật qua trang Web nhúng)
- **Mục đích:** Cập nhật file firmware thủ công qua giao diện Web khi ở gần thiết bị.
- **Nguyên lý:**
  1. ESP32 chạy một Web Server nhúng tại địa chỉ `http://<IP_ESP32>/update`.
  2. Người dùng mở trình duyệt web trên điện thoại/laptop, chọn file `firmware.bin` vừa biên dịch.
  3. Bấm **Upload** ➔ ESP32 tự bóc tách file, ghi vào Flash và tự Reboot sang firmware mới.
- **Thư viện sử dụng:** `<WebServer.h>`, `<Update.h>`

---

### ☁️ Phương Thức 3: HTTPS Cloud OTA (Cập nhật Tự Động Từ Xa Qua Nginx & MQTT) ★ *(Khuyên Dùng Vận Hành)*
- **Mục đích:** Nâng cấp tự động hàng loạt thiết bị ngoài trường thông qua Server Nginx và App Điện thoại.
- **Luồng hoạt động (Workflow):**

```text
[ 1. Kỹ sư Biên dịch ] ──> Upload file `firmware_v1.0.2.bin` lên Nginx Server (`https://api-vending.doanhnghiep.com/firmware/`)
                                                                     │
[ 2. Mobile App / Server ] ──> Gửi lệnh MQTT đến Topic `esp32/{device_id}/system/ota_trigger`
                                Payload: {"version": "1.0.2", "url": "https://api-vending.doanhnghiep.com/firmware/firmware_v1.0.2.bin"}
                                                                     │
[ 3. ESP32 Nhận Lệnh ] ──> Kết nối HTTPS đến Nginx ➔ Tải file .bin qua SSL ➔ Ghi vào vùng nhớ OTA ➔ Reboot hoàn tất!
```

---

## ⚙️ 3. Phân Chia Vùng Nhớ Flash (Partition Table) Cho OTA

Để ESP32 có thể cập nhật OTA an toàn (không bị nạp dở chừng làm gạch/brick mạch), bộ nhớ Flash 4MB của ESP32 được chia thành 2 vùng chứa chương trình song song:

```text
┌────────────────────────────────────────────────────────────────────────┐
│                        ESP32 FLASH MEMORY (4MB)                        │
├──────────────┬──────────────┬──────────────────┬──────────────────┬────┤
│ Bootloader   │ Partition    │      app0        │      app1        │    │
│ (Mồi khởi    │ Table        │ (Firmware Hiện   │ (Firmware Mới    │ SPIFFS/
│  động)       │ (Bảng vùng)  │  tại đang chạy)  │  đang được nạp)  │ NVS│
└──────────────┴──────────────┴──────────────────┴──────────────────┴────┘
```

### Cơ chế Rollback & Anti-brick tự động:
1. Khi cập nhật OTA, ESP32 sẽ ghi firmware mới vào vùng **`app1`** (trong khi **`app0`** vẫn đang chạy bình thường).
2. Sau khi ghi thành công 100%, Bootloader sẽ đổi cờ khởi động sang **`app1`** và Reboot.
3. Khi boot sang `app1`, ESP32 kết nối tới MQTT thành công sẽ gọi `esp_ota_mark_app_valid_cancel_rollback()` để xác nhận bản nạp hợp lệ.
4. Nếu trong quá trình tải bị rớt mạng hay cúp điện, ESP32 tự động rollback giữ nguyên bản code cũ ở **`app0`** ➔ **Tuyệt đối không bao giờ bị hỏng mạch (Anti-brick)!**

---

## 🛡️ 4. Các Nguyên Tắc An Toàn Khi Cập Nhật OTA

1. **Bắt buộc dùng HTTPS (SSL/TLS):** Không tải file `.bin` qua HTTP thường để tránh bị tấn công Man-in-the-middle chèn mã độc vào ESP32.
2. **Kiểm tra dung lượng PIN/Nguồn:** Nếu nguồn cấp cho ESP32 bị sụt điện áp dưới 3.0V, hoãn quá trình OTA cho đến khi nguồn ổn định.
3. **Phân biệt phiên bản (Version Check):** Chỉ cho phép OTA khi số phiên bản mới khác với phiên bản hiện tại (`v1.0.2` > `v1.0.1`).
