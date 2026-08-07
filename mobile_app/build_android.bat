@echo off
title Build and Install Android App - ESP Automation
color 0A

echo ==============================================================================
echo 🚀 HỆ THỐNG TỰ ĐỘNG BIÊN DỊCH VÀ CÀI ĐẶT FLUTTER ANDROID APP (1-CLICK)
echo ==============================================================================
echo.

cd /d "%~dp0"

echo [1/4] Kiem tra Flutter SDK...
where flutter >nul 2>&1
if errorlevel 1 goto FLUTTER_MISSING

echo [2/4] Dang tu dong tai va cap nhat cac thu vien Flutter App (pubspec.yaml)...
call flutter pub get
if errorlevel 1 goto PUB_ERROR

echo.
echo [3/4] Dang bien dich ban Release APK...
call flutter build apk --release
if errorlevel 1 goto BUILD_ERROR

echo.
echo [4/4] Dang quet va tu dong nap vao dien thoai Android qua ADB...
where adb >nul 2>&1
if errorlevel 1 (
    echo.
    echo ⚠️ CHU Y: File APK da build thanh cong tai:
    echo    mobile_app\build\app\outputs\flutter-apk\app-release.apk
    echo    (Chua tim thay tool ADB trong PATH de tu dong nap vao dien thoai).
    goto END
)

call adb devices
call adb install -r build\app\outputs\flutter-apk\app-release.apk
if errorlevel 1 goto INSTALL_WARNING

echo.
echo ==============================================================================
echo  THANH CONG: App da duoc nap muot ma 100%% vao dien thoai Android!
echo ==============================================================================
goto END

:FLUTTER_MISSING
echo.
echo ❌ LOI: Khong tim thay Flutter SDK trong bien moi truong (PATH)!
echo.
echo 📋 HUONG DAN KHAC PHUC CHO MAY MOI:
echo    1. Tai va giai nen Flutter SDK tai: https://docs.flutter.dev/get-started/install/windows
echo    2. Them duong dan thu muc "flutter\bin" vao System Environment Variables (PATH).
echo    3. Mo lai cua so Command Prompt / Terminal moi va chay lai file bat nay!
goto END

:PUB_ERROR
echo.
echo ❌ LOI: Khong the tai thu vien Flutter App. Vui long kiem tra ket noi Internet!
goto END

:BUILD_ERROR
echo.
echo ❌ LOI: Bien dich APK that bai! Vui long chay "flutter doctor" de kiem tra Android SDK.
goto END

:INSTALL_WARNING
echo.
echo ⚠️ CHU Y: Chua tim thay dien thoai ket noi ADB hoac chua bat USB Debugging.
echo    File APK ban Release da san sang tai:
echo    mobile_app\build\app\outputs\flutter-apk\app-release.apk
echo    Vui long cam cap / bat Wireless ADB va chay lai file nay!
goto END

:END
echo.
pause
