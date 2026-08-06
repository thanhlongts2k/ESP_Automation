# Hướng Dẫn Nạp Flutter App Lên iPhone (Thật & Giả Lập iOS Simulator) Trên Mac

Tài liệu hướng dẫn cách chạy ứng dụng **ESP_Automation Mobile App** trên chiếc **iPhone ảo (iOS Simulator)** có sẵn trên máy Mac hoặc iPhone thật qua cổng USB.

---

## 📱 CÁCH 1: CHẠY TRÊN IPHONE ẢO (IOS SIMULATOR - KHÔNG CẦN IPHONE THẬT)

Trên máy Mac có sẵn phần mềm iPhone ảo cực kỳ tiện lợi:

1. Mở cửa sổ **Terminal** trên Mac (`Command + Space` ➔ Gõ `Terminal`).
2. Mở iPhone ảo lên bằng cách gõ câu lệnh:
   ```bash
   open -a Simulator
   ```
   *(Ngay lập tức một chiếc iPhone ảo Retina sắc nét sẽ hiện ra trên màn hình Mac)*.
3. Kéo thả thư mục `mobile_app` vào Terminal (`cd ` + kéo thả thư mục).
4. Gõ lệnh:
   ```bash
   flutter run
   ```
   *(Flutter sẽ tự động nạp ứng dụng lên chiếc iPhone ảo trên màn hình cho bạn trải nghiệm và test giao diện mượt mà 100%)*.

---

## 📱 CÁCH 2: CHẠY TRÊN IPHONE THẬT CẮM CÁP USB

1. Cắm iPhone thật vào máy Mac (Chọn **Tin tưởng/Trust** trên màn hình iPhone).
2. Mở Terminal, chuyển tới thư mục `mobile_app`.
3. Dán lệnh:
   ```bash
   bash build_ios.sh
   ```

---

## 💡 MẸO CHỌN MẪU IPHONE ẢO (IPHONE 15 PRO / IPHONE 16 PRO MAX)
Khi màn hình iPhone ảo đang mở trên Mac:
- Vào menu **File ➔ Open Device ➔ iOSxx ➔ iPhone 15 Pro** để đổi mẫu iPhone khác theo ý thích!
