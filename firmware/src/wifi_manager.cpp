#include "wifi_manager.h"

/**
 * @brief Khởi tạo đối tượng WiFiManager với thời gian kết nối ban đầu bằng 0
 */
WiFiManager::WiFiManager() : _lastConnectAttempt(0) {}

/**
 * @brief Cấu hình Wi-Fi Mode Station và kích hoạt quá trình kết nối tới Access Point
 */
void WiFiManager::begin() {
    Serial.println("🌐 [WiFi] Khởi tạo kết nối Wi-Fi...");
    WiFi.mode(WIFI_STA); // Cấu hình ESP32 thành thiết bị ngoại vi kết nối Wi-Fi (Station Mode)
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Bắt đầu gửi bắt tay Wi-Fi với Router
    _lastConnectAttempt = millis();
}

/**
 * @brief Vòng lặp tự động tái kết nối ngầm phi bất đồng bộ khi rớt mạng Wi-Fi
 */
void WiFiManager::loop() {
    // Nếu rớt mạng Wi-Fi, kiểm tra định kỳ mỗi 5 giây để thử kết nối lại
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long now = millis();
        if (now - _lastConnectAttempt >= _reconnectInterval) {
            _lastConnectAttempt = now;
            Serial.println("⚠️ [WiFi] Mất sóng Wi-Fi! Đang tự động tái kết nối ngầm...");
            WiFi.disconnect(); // Ngắt session cũ để giải phóng RAM Wi-Fi Stack
            WiFi.reconnect();  // Thử phát lệnh kết nối lại tới Router
        }
    }
}

/**
 * @brief Kiểm tra trạng thái Wi-Fi
 */
bool WiFiManager::isConnected() const {
    return (WiFi.status() == WL_CONNECTED);
}

/**
 * @brief Lấy IP local của ESP32 do Router cấp phát
 */
String WiFiManager::getLocalIP() const {
    if (isConnected()) {
        return WiFi.localIP().toString();
    }
    return "0.0.0.0";
}

/**
 * @brief Lấy cường độ tín hiệu sóng Wi-Fi RSSI (dBm)
 */
int WiFiManager::getRSSI() const {
    if (isConnected()) {
        return WiFi.RSSI();
    }
    return -100; // Trả về -100 dBm (mức sóng yếu nhất/mất sóng) nếu chưa kết nối
}
