#include "mqtt_manager.h"
#include "esp_ota_ops.h"

// Con trỏ toàn cục dùng cho tĩnh callback PubSubClient
static MQTTManager* instance = nullptr;

/**
 * @brief Khởi tạo các mốc thời gian Backoff ban đầu
 */
MQTTManager::MQTTManager()
    : _lastReconnectAttempt(0), _currentBackoffInterval(2000), _lastTelemetryPublish(0) {
    instance = this;
}

/**
 * @brief Cấu hình Client (SSL 8883 hoặc TCP 1883), gán Callback và tăng kích thước Buffer
 */
void MQTTManager::begin(MQTTCommandCallback callback) {
    _commandCallback = callback;

    if (MQTT_USE_SSL) {
        Serial.println("🔒 [MQTT] Khởi tạo kết nối SSL/TLS Port 8883 (Root CA DigiCert)...");
        _sslClient.setCACert(ROOT_CA_DIGICERT);
        _mqttClient.setClient(_sslClient);
    } else {
        Serial.printf("📡 [MQTT] Khởi tạo kết nối TCP Port %d...\n", MQTT_PORT);
        _mqttClient.setClient(_tcpClient);
    }

    _mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    _mqttClient.setCallback(_staticMqttCallback);
    _mqttClient.setBufferSize(512); // Mở rộng buffer bộ nhớ lên 512 bytes chứa đủ gói JSON telemetry lớn
}

/**
 * @brief Callback tĩnh nhận tín hiệu tin nhắn từ thư viện PubSubClient C-style
 */
void MQTTManager::_staticMqttCallback(char* topic, byte* payload, unsigned int length) {
    if (instance && instance->_commandCallback) {
        String message = "";
        for (unsigned int i = 0; i < length; i++) {
            message += (char)payload[i];
        }
        instance->_commandCallback(String(topic), message);
    }
}

/**
 * @brief Vòng lặp kiểm tra trạng thái và duy trì kết nối ngầm (Exponential Backoff Non-blocking)
 */
void MQTTManager::loop(bool isWifiConnected) {
    if (!isWifiConnected) return; // Nếu chưa có Wi-Fi thì bỏ qua không thử MQTT

    if (!_mqttClient.connected()) {
        unsigned long now = millis();
        // Áp dụng thuật toán lùi thời gian ngầm Exponential Backoff (2s, 4s, 8s... max 60s)
        if (now - _lastReconnectAttempt >= _currentBackoffInterval) {
            _lastReconnectAttempt = now;
            _connect();
        }
    } else {
        _mqttClient.loop(); // Duy trì vòng lặp giữ kết nối Ping/Pong với MQTT Broker
    }
}

/**
 * @brief Hàm thực thi kết nối MQTT Cloud với LWT Di chúc & Cụm Topic phân cấp theo Device ID
 */
void MQTTManager::_connect() {
    Serial.printf("📡 [MQTT] Đang thử kết nối tới %s:%d (Backoff %lums)...\n", MQTT_SERVER, MQTT_PORT, _currentBackoffInterval);

    // 1. Chuẩn bị thông điệp di chúc LWT (Last Will and Testament)
    // Nếu ESP32 bị sụt nguồn đột ngột hay rớt mạng, Broker sẽ tự động phát gói này cho các Client
    String lwtTopic = getTopicStatus();
    StaticJsonDocument<128> lwtDoc;
    lwtDoc["status"] = "offline";
    lwtDoc["device_id"] = DEVICE_ID;
    String lwtPayload;
    serializeJson(lwtDoc, lwtPayload);

    // 2. Thực hiện kết nối với LWT Topic status, Payload offline, QoS 1, Retain true
    bool connected = false;
    if (String(MQTT_USER).length() > 0) {
        connected = _mqttClient.connect(DEVICE_ID, MQTT_USER, MQTT_PASSWORD, lwtTopic.c_str(), 1, true, lwtPayload.c_str());
    } else {
        connected = _mqttClient.connect(DEVICE_ID, lwtTopic.c_str(), 1, true, lwtPayload.c_str());
    }

    if (connected) {
        Serial.println("✅ [MQTT] ĐÃ KẾT NỐI THÀNH CÔNG TỚI BROKER CLOUD!");
        _currentBackoffInterval = 2000; // Reset thời gian lùi lại về mức 2s khi kết nối lại thành công

        // 3. Kiểm tra Anti-brick OTA Rollback nếu vừa nâng cấp firmware mới
        const esp_partition_t *running = esp_ota_get_running_partition();
        esp_ota_img_states_t ota_state;
        if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK && ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            esp_ota_mark_app_valid_cancel_rollback();
            Serial.println("🛡️ [OTA Anti-brick] Đã xác nhận Firmware mới hoạt động tốt!");
        }

        // 4. Publish thông điệp trạng thái Online (Retained = true)
        StaticJsonDocument<128> onlineDoc;
        onlineDoc["status"] = "online";
        onlineDoc["device_id"] = DEVICE_ID;
        onlineDoc["version"] = FIRMWARE_VERSION;
        String onlinePayload;
        serializeJson(onlineDoc, onlinePayload);
        _mqttClient.publish(lwtTopic.c_str(), onlinePayload.c_str(), true);

        // 5. Subscribe các Topic điều khiển phân cấp theo DEVICE_ID
        String tRelay1 = getTopicRelay1Control();
        String tRelay2 = getTopicRelay2Control();
        String tOTA = getTopicOTATrigger();

        _mqttClient.subscribe(tRelay1.c_str(), 1);
        _mqttClient.subscribe(tRelay2.c_str(), 1);
        _mqttClient.subscribe(tOTA.c_str(), 1);

        Serial.println("📥 Đã Subscribe Topics:");
        Serial.printf("   - %s\n", tRelay1.c_str());
        Serial.printf("   - %s\n", tRelay2.c_str());
        Serial.printf("   - %s\n", tOTA.c_str());
    } else {
        Serial.printf("❌ [MQTT] Kết nối thất bại rc=%d. Thử lại sau %lums...\n", _mqttClient.state(), _currentBackoffInterval);
        // Tăng thời gian Exponential Backoff lên gấp đôi (Tối đa 60 giây)
        _currentBackoffInterval = min(_currentBackoffInterval * 2, 60000UL);
    }
}

/**
 * @brief Trạng thái kết nối MQTT
 */
bool MQTTManager::isConnected() {
    return _mqttClient.connected();
}

/**
 * @brief Gửi dữ liệu Telemetry JSON đầy đủ các chỉ số cảm biến và trạng thái hệ thống
 */
void MQTTManager::publishTelemetry(float temp, float hum, float soilHum, bool relay1, bool relay2, int rssi, const String& ip) {
    if (!isConnected()) return;

    StaticJsonDocument<384> doc;
    doc["device_id"] = DEVICE_ID;
    doc["temperature"] = temp;
    doc["humidity"] = hum;
    doc["soil_humidity"] = soilHum;
    doc["relay1"] = relay1 ? "ON" : "OFF";
    doc["relay2"] = relay2 ? "ON" : "OFF";
    doc["relay1_light"] = relay1 ? "ON" : "OFF";
    doc["relay2_fan"] = relay2 ? "ON" : "OFF";
    doc["rssi"] = rssi;
    doc["ip"] = ip;
    doc["version"] = FIRMWARE_VERSION;
    doc["uptime_s"] = millis() / 1000;

    char buffer[384];
    serializeJson(doc, buffer);

    String sensorTopic = getTopicSensors();
    _mqttClient.publish(sensorTopic.c_str(), buffer, false);
    Serial.printf("📊 [MQTT Telemetry] [%s] -> %s\n", sensorTopic.c_str(), buffer);
}

/**
 * @brief Gửi thông điệp cập nhật trạng thái Relay với cờ Retained = true
 */
void MQTTManager::publishRelayStatus(int relayNum, bool state) {
    if (!isConnected()) return;

    String topic = (relayNum == 1) ? getTopicRelay1Control() : getTopicRelay2Control();
    String stateStr = state ? "ON" : "OFF";
    
    // Publish trạng thái Relay với Retained = true để App mở lên đọc được ngay
    _mqttClient.publish(topic.c_str(), stateStr.c_str(), true);
}
