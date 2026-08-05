---
name: build-release-git
description: Automates building ESP32 firmware binaries, Flutter APK release, tagging, and pushing git releases publicly when triggered by release commands.
---

# Build & Release ESP32 Firmware & Mobile APK via Git

Use this skill when the user requests "build release", "build-release git", or asks to build/release a new version for the **ESP_Automation** project.

## Execution Steps

1. **Check Version**: Read version from project configuration files (`version.h`, `pubspec.yaml`, or `package.json`).
2. **Validate & Compile Artifacts**:
   - **ESP32 Firmware**: Compile binary firmware (`.bin`) using Arduino CLI or PlatformIO (`pio run`).
   - **Flutter Mobile App**: Build release APK (`flutter build apk --release`).
3. **Commit & Tag**:
   - Stage working tree: `git add .`
   - Create commit: `git commit -m "release: v<VERSION>"` (if changes exist)
   - Create version tag: `git tag v<VERSION>`
4. **Push to GitHub**:
   - Push main branch and release tags: `git push origin main --tags`
5. **Release Artifacts Verification**:
   - Confirm generated ESP32 firmware binary path (e.g., `.pio/build/esp32/firmware.bin`).
   - Confirm generated Flutter APK path (`build/app/outputs/flutter-apk/app-release.apk`).
6. **Report Status**:
   - Confirm tag push to trigger GitHub Actions CI/CD release.
   - Summarize generated binary & APK paths for deployment.
