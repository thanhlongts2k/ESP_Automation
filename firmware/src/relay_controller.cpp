#include "relay_controller.h"

/**
 * @brief Khởi tạo trạng thái mặc định của các rơ-le là TẮT (false)
 */
RelayController::RelayController() : _relay1State(false), _relay2State(false) {}

/**
 * @brief Cấu hình xuất tín hiệu GPIO và đặt trạng thái mặc định ban đầu
 */
void RelayController::begin() {
    Serial.println("🔌 [Relay] Khởi tạo các chân GPIO điều khiển Relay...");
    pinMode(RELAY1_PIN, OUTPUT);
    pinMode(RELAY2_PIN, OUTPUT);
    
    // Đặt trạng thái ban đầu là TẮT (Mức INACTIVE LEVEL = HIGH với module rơ-le kích mức THẤP LOW Active)
    digitalWrite(RELAY1_PIN, RELAY_INACTIVE_LEVEL);
    digitalWrite(RELAY2_PIN, RELAY_INACTIVE_LEVEL);
}

/**
 * @brief Điều khiển đóng ngắt Relay 1 (Đèn)
 * @param state true: BẬT Đèn (xuất LOW), false: TẮT Đèn (xuất HIGH)
 */
void RelayController::setRelay1(bool state) {
    _relay1State = state;
    digitalWrite(RELAY1_PIN, state ? RELAY_ACTIVE_LEVEL : RELAY_INACTIVE_LEVEL);
    Serial.printf("💡 [Relay 1 - Đèn] -> %s\n", state ? "ON" : "OFF");
}

/**
 * @brief Điều khiển đóng ngắt Relay 2 (Quạt)
 * @param state true: BẬT Quạt (xuất LOW), false: TẮT Quạt (xuất HIGH)
 */
void RelayController::setRelay2(bool state) {
    _relay2State = state;
    digitalWrite(RELAY2_PIN, state ? RELAY_ACTIVE_LEVEL : RELAY_INACTIVE_LEVEL);
    Serial.printf("🌀 [Relay 2 - Quạt] -> %s\n", state ? "ON" : "OFF");
}

/**
 * @brief Thuật toán Hysteresis bảo vệ thiết bị, tự động đóng/ngắt Quạt theo nhiệt độ
 * @details
 *  - Ngưỡng TRÊN (TEMP_HYSTERESIS_HIGH = 32.0°C): Tự động BẬT quạt làm mát.
 *  - Ngưỡng DƯỚI (TEMP_HYSTERESIS_LOW = 29.0°C): Tự động TẮT quạt tiết kiệm điện.
 *  - Khoảng đệm 3.0°C giúp rơ-le không bao giờ bị bật/tắt liên tục làm hỏng cuộn dây rơ-le.
 */
void RelayController::processHysteresis(float currentTemp) {
    if (currentTemp <= 0.0f) return; // Bỏ qua nếu nhiệt độ chưa hợp lệ (lỗi đọc cảm biến)

    // Bật quạt (Relay 2) khi nhiệt độ quá nóng >= 32.0°C
    if (currentTemp >= TEMP_HYSTERESIS_HIGH && !_relay2State) {
        Serial.printf("🔥 [Hysteresis Auto] Nhiệt độ cao (%.1f°C >= %.1f°C) -> Tự động BẬT Quạt (Relay 2)\n", currentTemp, TEMP_HYSTERESIS_HIGH);
        setRelay2(true);
    }
    // Tắt quạt (Relay 2) khi nhiệt độ đã mát <= 29.0°C
    else if (currentTemp <= TEMP_HYSTERESIS_LOW && _relay2State) {
        Serial.printf("❄️ [Hysteresis Auto] Nhiệt độ mát (%.1f°C <= %.1f°C) -> Tự động TẮT Quạt (Relay 2)\n", currentTemp, TEMP_HYSTERESIS_LOW);
        setRelay2(false);
    }
}
