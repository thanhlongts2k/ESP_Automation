#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"

// Forward declaration
class RelayController;

/**
 * @brief Định nghĩa kiểu hàm callback xử lý tin nhắn lệnh MQTT từ xa
 */
typedef std::function<void(const String& topic, const String& payload)> MQTTCommandCallback;

/**
 * @class MQTTManager
 * @brief Lớp quản lý giao tiếp MQTT Cloud chuẩn Enterprise.
 * 
 * Tích hợp các tính năng công nghiệp cao cấp:
 *  - LWT (Last Will & Testament) di chúc báo Offline Retained khi thiết bị mất nguồn đột ngột.
 *  - Thuật toán Exponential Backoff tự động thử lại kết nối ngầm phi bất đồng bộ (2s, 4s, 8s... max 60s).
 *  - Quản lý cụm Topic phân cấp theo định danh `esp32/{device_id}/...`.
 *  - Đẩy dữ liệu Telemetry JSON phong phú (RSSI sóng Wi-Fi, Uptime, Soil moisture, IP, Version).
 */
class MQTTManager {
public:
    MQTTManager();

    /**
     * @brief Khởi tạo dịch vụ MQTT Client (SSL/TLS 8883 hoặc TCP 1883)
     * @param callback Hàm xử lý tin nhắn lệnh nhận được từ MQTT Broker
     */
    void begin(MQTTCommandCallback callback);

    /**
     * @brief Vòng lặp duy trì kết nối MQTT và lắng nghe tin nhắn ngầm (Non-blocking)
     * @param isWifiConnected Trạng thái sóng Wi-Fi hiện tại từ WiFiManager
     */
    void loop(bool isWifiConnected);

    /** @brief Kiểm tra xem ESP32 có đang kết nối thành công tới MQTT Broker hay không */
    bool isConnected();

    /**
     * @brief Định kỳ publish dữ liệu cảm biến & trạng thái thiết bị dạng JSON lên MQTT Cloud
     */
    void publishTelemetry(float temp, float hum, float soilHum, bool relay1, bool relay2, int rssi, const String& ip);

    /**
     * @brief Publish trạng thái ngắt Relay với cờ Retained = true để App mở lên đọc được ngay
     */
    void publishRelayStatus(int relayNum, bool state);

private:
    WiFiClient _tcpClient;                          ///< Client TCP thông thường (Port 1883)
    WiFiClientSecure _sslClient;                    ///< Client SSL/TLS bảo mật mã hóa (Port 8883)
    PubSubClient _mqttClient;                       ///< Thư viện MQTT Client PubSubClient
    MQTTCommandCallback _commandCallback;          ///< Callback xử lý lệnh điều khiển

    unsigned long _lastReconnectAttempt;           ///< Thời điểm thử kết nối lại MQTT gần nhất
    unsigned long _currentBackoffInterval;         ///< Khoảng thời gian lùi lại Exponential Backoff (2s -> 60s)
    unsigned long _lastTelemetryPublish;           ///< Thời điểm publish dữ liệu cảm biến gần nhất

    /** @brief Hàm kết nối MQTT nội bộ với LWT di chúc & Subscriptions */
    void _connect();

    /** @brief Callback tĩnh nhận tin nhắn thô từ PubSubClient C-style */
    static void _staticMqttCallback(char* topic, byte* payload, unsigned int length);
};

#endif // MQTT_MANAGER_H
