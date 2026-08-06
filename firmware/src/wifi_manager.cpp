#include "wifi_manager.h"

WiFiManager::WiFiManager() : _lastConnectAttempt(0) {}

void WiFiManager::begin() {
    Serial.println("🌐 [WiFi] Khởi tạo kết nối Wi-Fi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    _lastConnectAttempt = millis();
}

void WiFiManager::loop() {
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long now = millis();
        if (now - _lastConnectAttempt >= _reconnectInterval) {
            _lastConnectAttempt = now;
            Serial.println("⚠️ [WiFi] Mất Wi-Fi! Đang tự động kết nối lại ngầm...");
            WiFi.disconnect();
            WiFi.reconnect();
        }
    }
}

bool WiFiManager::isConnected() const {
    return (WiFi.status() == WL_CONNECTED);
}

String WiFiManager::getLocalIP() const {
    if (isConnected()) {
        return WiFi.localIP().toString();
    }
    return "0.0.0.0";
}

int WiFiManager::getRSSI() const {
    if (isConnected()) {
        return WiFi.RSSI();
    }
    return -100;
}
