@echo off
title Auto Download and Install Flutter SDK to C:\flutter
color 0B

echo ==============================================================================
echo    HE THONG TU DONG TAI VA GIAI NEN FLUTTER SDK VAO C:\flutter (1-CLICK)
echo ==============================================================================
echo:

if exist "C:\flutter\bin\flutter.bat" (
    echo [OK] DA TIM THAY FLUTTER SDK TAI C:\flutter!
    echo    Ban khong can tai lai. Vui long chay mobile_app\build_android.bat
    pause
    exit /b 0
)

echo [INFO] Dang tu dong tai Flutter SDK (khoang 1GB) tu server Google...
echo    (Vui long cho trong 1-3 phut tuy toc do mang)...
echo:

powershell -Command "$ProgressPreference = 'SilentlyContinue'; Invoke-WebRequest -Uri 'https://storage.googleapis.com/flutter_infra_release/releases/stable/windows/flutter_windows_3.24.0-stable.zip' -OutFile \"$env:TEMP\flutter.zip\""

if errorlevel 1 (
    echo [LOI] Khong the tai Flutter Zip. Vui long kiem tra ket noi Internet!
    pause
    exit /b 1
)

echo:
echo [INFO] Dang tu dong giai nen Flutter vao thu muc C:\flutter ...
powershell -Command "$ProgressPreference = 'SilentlyContinue'; Expand-Archive -Path \"$env:TEMP\flutter.zip\" -DestinationPath 'C:\' -Force"

if exist "%TEMP%\flutter.zip" del /f /q "%TEMP%\flutter.zip"

echo:
echo ==============================================================================
echo    TAI VA GIAI NEN FLUTTER SDK VAO C:\flutter HOAN TAT!
echo    Bay gio ban co the nhap dup mobile_app\build_android.bat de build App!
echo ==============================================================================
echo:
pause
