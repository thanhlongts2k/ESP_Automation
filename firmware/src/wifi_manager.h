#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

/**
 * @class WiFiManager
 * @brief Lớp quản lý kết nối Wi-Fi phi bất đồng bộ cho ESP32.
 * 
 * Lớp này đảm bảo kết nối Wi-Fi tự động khôi phục ngầm khi bị ngắt sóng
 * bằng thuật toán không treo vòng lặp millis(), giúp Web Server và Relay
 * luôn duy trì hoạt động thông suốt.
 */
class WiFiManager {
public:
    WiFiManager();

    /** @brief Khởi tạo chế độ Wi-Fi Station và bắt đầu kết nối ngầm */
    void begin();

    /** @brief Vòng lặp kiểm tra và tự động tái kết nối Wi-Fi ngầm (Non-blocking) */
    void loop();

    /** @brief Kiểm tra trạng thái kết nối Wi-Fi (true: Đã kết nối, false: Mất kết nối) */
    bool isConnected() const;

    /** @brief Lấy địa chỉ IP nội bộ dạng chuỗi (Ví dụ: "192.168.1.50") */
    String getLocalIP() const;

    /** @brief Lấy cường độ sóng Wi-Fi RSSI tính theo đơn vị dBm (Ví dụ: -65 dBm) */
    int getRSSI() const;

private:
    unsigned long _lastConnectAttempt;              ///< Thống kê thời điểm thử kết nối Wi-Fi gần nhất
    const unsigned long _reconnectInterval = 5000;  ///< Chu kỳ thử kết nối lại Wi-Fi ngầm (5000ms = 5 giây)
};

#endif // WIFI_MANAGER_H
