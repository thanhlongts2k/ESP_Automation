@echo off
title Build and Install Android App - ESP Automation
color 0A

echo ==============================================================================
echo  HE THONG TU DONG BIEN DICH VA CAI DAT FLUTTER ANDROID APP (1-CLICK)
echo ==============================================================================
echo.

cd /d "%~dp0"

echo [1/4] Kiem tra va tu dong tim kiem Flutter SDK...

:: Tu dong tim Flutter trong cac thu muc pho bien tren Windows neu chua co trong PATH
where flutter >nul 2>&1
if errorlevel 1 (
    if exist "C:\flutter\bin\flutter.bat" set "PATH=%PATH%;C:\flutter\bin"
    if exist "C:\src\flutter\bin\flutter.bat" set "PATH=%PATH%;C:\src\flutter\bin"
    if exist "D:\flutter\bin\flutter.bat" set "PATH=%PATH%;D:\flutter\bin"
    if exist "D:\src\flutter\bin\flutter.bat" set "PATH=%PATH%;D:\src\flutter\bin"
    if exist "%USERPROFILE%\flutter\bin\flutter.bat" set "PATH=%PATH%;%USERPROFILE%\flutter\bin"
    if exist "%USERPROFILE%\src\flutter\bin\flutter.bat" set "PATH=%PATH%;%USERPROFILE%\src\flutter\bin"
    if exist "%LOCALAPPDATA%\flutter\bin\flutter.bat" set "PATH=%PATH%;%LOCALAPPDATA%\flutter\bin"
)

where flutter >nul 2>&1
if errorlevel 1 goto FLUTTER_MISSING

echo [OK] Da tim thay Flutter SDK!

echo.
echo [2/4] Dang tu dong tai va cap nhat cac thu vien Flutter App (pubspec.yaml)...
call flutter pub get
if errorlevel 1 goto PUB_ERROR

echo.
echo [3/4] Dang bien dich ban Release APK...
call flutter build apk --release --android-skip-build-dependency-validation
if errorlevel 1 goto BUILD_ERROR

echo.
echo [4/4] Dang quet va tu dong nap vao dien thoai Android qua ADB...
where adb >nul 2>&1
if errorlevel 1 (
    if exist "%ANDROID_HOME%\platform-tools\adb.exe" set "PATH=%PATH%;%ANDROID_HOME%\platform-tools"
    if exist "%LOCALAPPDATA%\Android\Sdk\platform-tools\adb.exe" set "PATH=%PATH%;%LOCALAPPDATA%\Android\Sdk\platform-tools"
    if exist "C:\Android\platform-tools\adb.exe" set "PATH=%PATH%;C:\Android\platform-tools"
)

where adb >nul 2>&1
if errorlevel 1 (
    echo.
    echo [CHU Y] File APK da build thanh cong tai:
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
echo [LOI] Tren may moi nay CHUA CAI DAT FLUTTER SDK (hoac chua giai nen vao C:\flutter)!
echo.
echo  CAP NHAT BAN 1-CLICK TU DONG DL FLUTTER CHO MAY MOI:
echo    - Anh chi can ra thu muc goc du an va NHAP DUP FILE "install_flutter_windows.bat"
echo    - File bat do se TU DONG TAI VA GIAI NEN Flutter vao C:\flutter cho anh!
echo.
echo    Hoac anh tu giai nen file Zip Flutter vao thu muc "C:\flutter" (Script se tu dong NTIEN).
goto END

:PUB_ERROR
echo.
echo [LOI] Khong the tai thu vien Flutter App. Vui long kiem tra ket noi Internet!
goto END

:BUILD_ERROR
echo.
echo [LOI] Bien dich APK that bai! Vui long chay "flutter doctor" de kiem tra Android SDK.
goto END

:INSTALL_WARNING
echo.
echo [CHU Y] Chua tim thay dien thoai ket noi ADB hoac chua bat USB Debugging.
echo    File APK ban Release da san sang tai:
echo    mobile_app\build\app\outputs\flutter-apk\app-release.apk
echo    Vui long cam cap / bat Wireless ADB va chay lai file nay!
goto END

:END
echo.
pause
