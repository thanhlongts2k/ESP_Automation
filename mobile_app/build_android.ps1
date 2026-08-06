# ==============================================================================
# SCRIPT 1-CLICK TỰ ĐỘNG BIÊN DỊCH VÀ NẠP FLUTTER ANDROID APP QUA ADB
# ==============================================================================

$OutputEncoding = [System.Text.Encoding]::UTF8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

Write-Host "==============================================================================" -ForegroundColor Cyan
Write-Host "🚀 HỆ THỐNG TỰ ĐỘNG BIÊN DỊCH VÀ CÀI ĐẶT FLUTTER ANDROID APP (1-CLICK)" -ForegroundColor Cyan
Write-Host "==============================================================================" -ForegroundColor Cyan
Write-Host ""

Set-Location $PSScriptRoot

Write-Host "📦 [1/3] Đang cập nhật gói phụ thuộc Flutter..." -ForegroundColor Yellow
flutter pub get

Write-Host ""
Write-Host "🔨 [2/3] Đang biên dịch bản Release APK..." -ForegroundColor Yellow
flutter build apk --release

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "❌ LỖI: Biên dịch APK thất bại!" -ForegroundColor Red
    Read-Host "Bấm Enter để thoát..."
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "📱 [3/3] Đang quét thiết bị điện thoại Android qua ADB..." -ForegroundColor Yellow
adb devices

Write-Host ""
Write-Host "⚡ Đang tự động nạp file APK đè vào điện thoại..." -ForegroundColor Yellow
adb install -r build\app\outputs\flutter-apk\app-release.apk

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "==============================================================================" -ForegroundColor Green
    Write-Host "🎉 THÀNH CÔNG: App đã được nạp mượt mà 100% vào điện thoại Android!" -ForegroundColor Green
    Write-Host "==============================================================================" -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "⚠️ CHÚ Ý: Chưa tìm thấy điện thoại kết nối ADB hoặc chưa bật 'Cài đặt qua USB'." -ForegroundColor Yellow
}

Write-Host ""
Read-Host "Bấm Enter để hoàn tất..."
