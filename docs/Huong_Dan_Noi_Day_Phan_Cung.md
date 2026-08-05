# Sơ Đồ & Hướng Dẫn Nối Dây Phần Cứng Chi Tiết (ESP32 IoT Kit)

Tài liệu hướng dẫn chi tiết sơ đồ chân cắm (Pinout) và cách đấu nối dây giữa **ESP32**, **Chân Đế Mở Rộng 30P**, **Module Relay 2 Kênh 5V**, **Cảm Biến DHT22** và các thiết bị tải điện.

---

## 🎨 1. SƠ ĐỒ KẾT NỐI TỔNG QUAN (ASCII DIAGRAM)

```text
               ┌──────────────────────────────────────────────┐
               │    CHÂN ĐẾ MỞ RỘNG ESP32 DEVKIT V1 30P       │
               └──────────────────┬───────────────────────────┘
                                  │
         ┌────────────────────────┼────────────────────────┐
         │ (Chân 5V, GND, GPIO23) │ (Chân 5V, GND, GPIO18,19)│
         ▼                        ▼                        ▼
┌──────────────────┐    ┌──────────────────┐    ┌──────────────────┐
│ CẢM BIẾN DHT22   │    │  MODULE RELAY    │    │ TẢI ĐIỆN         │
│ (Nhiệt độ/Độ ẩm) │    │  2 KÊNH 5V OPTO  │    │ 220V AC / DC     │
├──────────────────┤    ├──────────────────┤    ├──────────────────┤
│ VCC  ──> 5V      │    │ VCC  ──> 5V      │    │ Kênh 1 ──> Đèn 💡│
│ GND  ──> GND     │    │ GND  ──> GND     │    │ Kênh 2 ──> Quạt 🌀│
│ DATA ──> GPIO 23 │    │ IN1  ──> GPIO 18 │    └──────────────────┘
└──────────────────┘    │ IN2  ──> GPIO 19 │
                        └──────────────────┘
```

---

## 🔌 2. BẢNG CHI TIẾT NỐI DÂY GIỮA CÁC THIẾT BỊ

### 🟢 A. Đấu Nối Module Relay 2 Kênh 5V (Điều khiển Đèn & Quạt)
*Dùng 4 sợi dây cắm Cái - Cái (Female to Female):*

| Chân trên Module Relay 2K | Chân tương ứng trên Chân Đế ESP32 30P | Màu Dây Gợi Ý | Chức Năng |
| :--- | :--- | :---: | :--- |
| **VCC** | Hàng chân **5V** (VCC) | Đỏ | Cấp nguồn nuôi rơ-le 5VDC |
| **GND** | Hàng chân **GND** | Đen | Nguồn âm (Ground) |
| **IN1** | Chân **GPIO 18** | Vàng | Tín hiệu điều khiển Đèn (Relay 1) |
| **IN2** | Chân **GPIO 19** | Xanh lá | Tín hiệu điều khiển Quạt (Relay 2) |

---

### 🟡 B. Đấu Nối Cảm Biến Nhiệt Độ & Độ Ẩm Không Khí DHT22
*Dùng 3 sợi dây cắm Cái - Cái (Female to Female):*

| Chân trên Cảm biến DHT22 | Chân tương ứng trên Chân Đế ESP32 30P | Màu Dây Gợi Ý | Chức Năng |
| :--- | :--- | :---: | :--- |
| **VCC (hoặc +)** | Hàng chân **5V** (hoặc 3.3V) | Đỏ | Cấp nguồn cho cảm biến |
| **GND (hoặc -)** | Hàng chân **GND** | Đen | Nguồn âm (Ground) |
| **DATA (hoặc OUT/S)**| Chân **GPIO 23** | Xanh dương | Tín hiệu đọc dữ liệu nhiệt độ/độ ẩm |

---

### 🔵 C. Đấu Nối Cảm Biến Độ Ẩm Đất Điện Dung v1.2 (Mở Rộng Tùy Chọn)
*Dùng 3 sợi dây cắm Cái - Cái (Female to Female):*

| Chân trên Cảm biến đất v1.2 | Chân tương ứng trên Chân Đế ESP32 30P | Chức Năng |
| :--- | :--- | :--- |
| **VCC** | Hàng chân **3.3V** (hoặc 5V) | Cấp nguồn |
| **GND** | Hàng chân **GND** | Nguồn âm |
| **AOUT (Analog Out)** | Chân **GPIO 34** (ADC1_CH6) | Tín hiệu điện áp độ ẩm đất |

---

### 🟣 D. Đấu Nối Cảm Biến Nhiệt Độ Nước DS18B20 (Mở Rộng Tùy Chọn)
*Dùng 3 sợi dây cắm:*

| Chân trên DS18B20 | Chân tương ứng trên Chân Đế ESP32 30P | Ghi Chú |
| :--- | :--- | :--- |
| **VCC (Dây Đỏ)** | Hàng chân **3.3V** hoặc **5V** | Cấp nguồn |
| **GND (Dây Đen)** | Hàng chân **GND** | Nguồn âm |
| **DATA (Dây Vàng/Trắng)**| Chân **GPIO 4** | Nối thêm trở kéo 4.7k Ohm lên VCC |

---

## ⚡ 3. CÁCH ĐẤU NỐI ĐIỆN 220V AC VÀO RELAY (AN TOÀN BẢO VỆ)

Trên mỗi kênh của Module Relay 2 Kênh sẽ có 3 cổng ốc vặn:
- **COM (Common):** Cổng chung.
- **NO (Normally Open):** Thường mở (Khi ESP32 kích BẬT ➔ Relay đóng mạch ➔ Thiết bị chạy).
- **NC (Normally Closed):** Thường đóng.

### 💡 Đấu Nối Đèn 220V AC vào Kênh 1 (Relay 1):
1. Trích 1 dây của nguồn 220V AC (Dây L - Dây nóng).
2. Cắt đôi dây L: 
   - Đầu vào dây L nối vào cổng **COM** của Kênh 1.
   - Đầu dây L nối tới Đèn đấu vào cổng **NO** của Kênh 1.
3. Dây N (Dây nguội 220V) nối thẳng tới Đèn.

### 🌀 Đấu Nối Quạt 220V AC vào Kênh 2 (Relay 2):
1. Làm tương tự: Dây L nguồn 220V vào cổng **COM** Kênh 2.
2. Cổng **NO** Kênh 2 nối tới Quạt.

---

## ⚠️ 4. NGUYÊN TẮC AN TOÀN KHI THAO TÁC

1. **Rút nguồn 220V trước khi cắm dây:** Tuyệt đối không cắm/rút dây tín hiệu khi đang cắm điện 220V AC.
2. **Kiểm tra nguồn 5V:** Chân Đế ESP32 30P có cấp sẵn nguồn 5V từ cổng USB/Type-C sang chân VCC của Relay và DHT22.
3. **Mức kích Relay:** Trong code `config.h`, Relay 5V Opto được kích mức **LOW (THẤP)**. Khi chân GPIO 18/19 xuất mức `LOW` (0V), Relay sẽ bật.
