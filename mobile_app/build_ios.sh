#!/bin/bash
# ==============================================================================
# SCRIPT TỰ ĐỘNG BIÊN DỊCH VÀ NẠP FLUTTER APP LÊN IPHONE (THẬT & GIẢ LẬP SIMULATOR) TRÊN MAC
# ==============================================================================

echo "🚀 Đang tự động kiểm tra môi trường và cài đặt phụ thuộc..."
flutter pub get

echo ""
echo "📲 Đang kiểm tra thiết bị iPhone thật kết nối USB và iPhone Simulator ảo..."
flutter devices

echo ""
echo "⚡ Đang biên dịch và khởi chạy App trên iPhone / iOS Simulator..."
flutter run

echo "✅ Hoàn tất!"
