#include "relay_controller.h"

RelayController::RelayController() : _relay1State(false), _relay2State(false) {}

void RelayController::begin() {
    Serial.println("🔌 [Relay] Khởi tạo các chân GPIO Relay...");
    pinMode(RELAY1_PIN, OUTPUT);
    pinMode(RELAY2_PIN, OUTPUT);
    
    // Đặt mặc định TẮT (INACTIVE LEVEL = HIGH với LOW-active relay)
    digitalWrite(RELAY1_PIN, RELAY_INACTIVE_LEVEL);
    digitalWrite(RELAY2_PIN, RELAY_INACTIVE_LEVEL);
}

void RelayController::setRelay1(bool state) {
    _relay1State = state;
    digitalWrite(RELAY1_PIN, state ? RELAY_ACTIVE_LEVEL : RELAY_INACTIVE_LEVEL);
    Serial.printf("💡 [Relay 1 - Đèn] -> %s\n", state ? "ON" : "OFF");
}

void RelayController::setRelay2(bool state) {
    _relay2State = state;
    digitalWrite(RELAY2_PIN, state ? RELAY_ACTIVE_LEVEL : RELAY_INACTIVE_LEVEL);
    Serial.printf("🌀 [Relay 2 - Quạt] -> %s\n", state ? "ON" : "OFF");
}

void RelayController::processHysteresis(float currentTemp) {
    if (currentTemp <= 0.0f) return; // Nhiệt độ chưa hợp lệ

    // Bật quạt (Relay 2) khi nhiệt độ quá nóng >= 32.0°C
    if (currentTemp >= TEMP_HYSTERESIS_HIGH && !_relay2State) {
        Serial.printf("🔥 [Hysteresis] Nhiệt độ cao (%.1f°C >= %.1f°C) -> Tự động BẬT Quạt (Relay 2)\n", currentTemp, TEMP_HYSTERESIS_HIGH);
        setRelay2(true);
    }
    // Tắt quạt (Relay 2) khi nhiệt độ đã mát <= 29.0°C
    else if (currentTemp <= TEMP_HYSTERESIS_LOW && _relay2State) {
        Serial.printf("❄️ [Hysteresis] Nhiệt độ mát (%.1f°C <= %.1f°C) -> Tự động TẮT Quạt (Relay 2)\n", currentTemp, TEMP_HYSTERESIS_LOW);
        setRelay2(false);
    }
}
