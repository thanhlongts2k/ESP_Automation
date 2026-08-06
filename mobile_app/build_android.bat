@echo off
title Build and Install Android App - ESP Automation
color 0A

echo ==============================================================================
echo  HE THONG TU DONG BIEN DICH VA CAI DAT FLUTTER ANDROID APP (1-CLICK)
echo ==============================================================================
echo.

cd /d "%~dp0"

echo [1/3] Dang cap nhat goi phu thuoc Flutter...
call flutter pub get

echo.
echo [2/3] Dang bien dich ban Release APK...
call flutter build apk --release
if errorlevel 1 goto BUILD_ERROR

echo.
echo [3/3] Dang quet thiet bi dien thoai Android qua ADB...
call adb devices

echo.
echo Dang tu dong nap file APK de vao dien thoai...
call adb install -r build\app\outputs\flutter-apk\app-release.apk
if errorlevel 1 goto INSTALL_WARNING

echo.
echo ==============================================================================
echo  THANH CONG: App da duoc nap muot ma 100%% vao dien thoai Android!
echo ==============================================================================
goto END

:BUILD_ERROR
echo.
echo [LOI] Bien dich APK that bai! Vui long kiem tra lai Flutter SDK.
goto END

:INSTALL_WARNING
echo.
echo [CHU Y] Chua tim thay dien thoai ket noi ADB hoac chua bat USB Debugging.
echo Vui long cam cap / bat Wireless ADB va chay lai file nay!
goto END

:END
echo.
pause
