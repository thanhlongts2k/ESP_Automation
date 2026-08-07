@echo off
title Setup Environment - ESP Automation (1-Click Install)
color 0B

echo ==============================================================================
echo  HE THONG TU DONG KIEM TRA VA CAI DAT THU VIEN MOI TRUONG DU AN (1-CLICK)
echo ==============================================================================
echo.

cd /d "%~dp0"

echo [1/4] Kiem tra moi truong Python...
where python >nul 2>&1
if errorlevel 1 goto PYTHON_MISSING

echo [OK] Da tim thay Python! Dang tu dong cai dat thu vien (paho-mqtt, platformio)...
call python -m pip install -r requirements.txt
goto CHECK_FLUTTER

:PYTHON_MISSING
echo [CHU Y] Khong tim thay Python tren may.
echo    Neu muon chay Script gia lap tools/mock_esp32.py, vui long tai Python tai: https://www.python.org/

:CHECK_FLUTTER
echo.
echo [2/4] Kiem tra Flutter SDK...
where flutter >nul 2>&1
if errorlevel 1 goto FLUTTER_MISSING

echo [OK] Da tim thay Flutter SDK!
echo.
echo [3/4] Tu dong tai va cai dat cac thu vien Flutter App (pubspec.yaml)...
cd /d "%~dp0mobile_app"
call flutter pub get
cd /d "%~dp0"
goto CHECK_ADB

:FLUTTER_MISSING
echo [LOI] Khong tim thay Flutter SDK trong bien moi truong (PATH)!
echo    De build Android App, vui long tai va cai dat Flutter tai: https://docs.flutter.dev/get-started/install/windows
echo    Sau khi cai xong, hay them duong dan "flutter\bin" vao System Environment Variables (PATH).

:CHECK_ADB
echo.
echo [4/4] Kiem tra cong cu Android ADB...
where adb >nul 2>&1
if errorlevel 1 goto ADB_MISSING

echo [OK] Da tim thay cong cu ADB!
goto END

:ADB_MISSING
echo [CHU Y] Khong tim thay tool ADB (Android Debug Bridge).
echo    Tool ADB di kem voi Android Studio hoac Platform-Tools.
echo    May van co the build file APK nhung can bat ADB de tu dong nap vao dien thoai.

:END
echo.
echo ==============================================================================
echo  KIEM TRA VA CAI DAT MOI TRUONG HOAN TAT!
echo    - De build va nap App Android: Nhap dup mobile_app\build_android.bat
echo    - De chay Gia lap ESP32 tren PC: Chay lenh "python tools\mock_esp32.py"
echo ==============================================================================
echo.
pause
