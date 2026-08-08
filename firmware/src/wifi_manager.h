#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <Preferences.h>
#include "config.h"

/**
 * @class WiFiManager
 * @brief Lớp quản lý kết nối Wi-Fi phi bất đồng bộ & Captive Portal cho ESP32.
 */
class WiFiManager {
public:
    WiFiManager();

    /** @brief Khởi tạo hệ thống Wi-Fi: NVS, WiFiMulti & Captive Portal */
    void begin(WebServer* server = nullptr);

    /** @brief Vòng lặp kiểm tra kết nối Wi-Fi, DNS Captive Portal & Nút nhấn BOOT */
    void loop();

    /** @brief Kiểm tra xem ESP32 có đang ở chế độ Captive Portal AP hay không */
    bool isAPMode() const;

    /** @brief Kiểm tra trạng thái kết nối Wi-Fi Station */
    bool isConnected() const;

    /** @brief Lấy địa chỉ IP (dù ở Station hay AP Mode) */
    String getLocalIP() const;

    /** @brief Lấy cường độ sóng Wi-Fi RSSI (dBm) */
    int getRSSI() const;

    /** @brief Xóa toàn bộ Wi-Fi trong bộ nhớ Flash NVS và Reboot về lại AP Mode */
    void resetCredentials();

    /** @brief Lưu một bộ Wi-Fi (SSID, Password) mới vào NVS Flash */
    bool saveCredential(const String& ssid, const String& pass);

    /** @brief Xây dựng chuỗi JSON chứa danh sách các mạng Wi-Fi dặt xung quanh (Wi-Fi Scan) */
    String scanNetworksJSON();

private:
    WiFiMulti _wifiMulti;
    DNSServer _dnsServer;
    Preferences _prefs;
    WebServer* _server;

    bool _apMode;
    unsigned long _lastConnectAttempt;
    const unsigned long _reconnectInterval = 5000;
    
    unsigned long _bootButtonPressStart;
    bool _bootButtonPressed;

    void _loadCredentialsFromNVS();
    void _startCaptivePortal();
    void _setupCaptivePortalRoutes();
};

#endif // WIFI_MANAGER_H
