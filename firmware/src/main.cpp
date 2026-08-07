/**
 * ==============================================================================
 * DỰ ÁN: ESP32 IoT Automation (ESP_Automation)
 * CHỨC NĂNG: ENTRYPOINT CHÍNH CỦA FIRMWARE C++ MODULAR ENTERPRISE
 * ==============================================================================
 *  Mã nguồn đã được phân rã thành các lớp module C++ riêng biệt:
 *   1. WiFiManager: Quản lý Wi-Fi tự khôi phục ngầm phi bất đồng bộ.
 *   2. SensorManager: Đọc & cache dữ liệu DHT22 & Độ ẩm đất Analog.
 *   3. RelayController: Điều khiển ngắt GPIO Đèn/Quạt & Logic Hysteresis 32/29°C.
 *   4. MQTTManager: Cụm Topics esp32/{device_id}/..., LWT Status, Retained States, Backoff.
 *   5. WebServer & ArduinoOTA: Giao diện Web Dashboard Local PROGMEM & Nạp code qua Wi-Fi.
 * ==============================================================================
 */

#include <Arduino.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <HTTPUpdate.h>

#include "config.h"
#include "web_dashboard.h"
#include "wifi_manager.h"
#include "sensor_manager.h"
#include "relay_controller.h"
#include "mqtt_manager.h"

// ============================================================================
// KHỞI TẠO CÁC ĐỐI TƯỢNG MODULE C++ (GLOBAL OBJECTS)
// ============================================================================
WiFiManager wifiManager;        ///< Khởi tạo đối tượng quản lý Wi-Fi
SensorManager sensorManager;    ///< Khởi tạo đối tượng đọc cảm biến
RelayController relayController;///< Khởi tạo đối tượng điều khiển Relay & Hysteresis
MQTTManager mqttManager;        ///< Khởi tạo đối tượng giao tiếp MQTT Cloud
WebServer server(80);           ///< Khởi tạo HTTP Web Server nội bộ tại Cổng 80

unsigned long lastSensorPublishTime = 0;
const unsigned long publishInterval = 5000; // Chu kỳ đẩy dữ liệu Telemetry lên MQTT Cloud (5 giây)

// ============================================================================
// HÀM XỬ LÝ HTTPS CLOUD OTA (PHƯƠNG THỨC 3 - CẬP NHẬT TỰ ĐỘNG TỪ XA)
// ============================================================================
void executeHTTPSOTA(const char* url, const char* targetVersion) {
    Serial.printf("☁️ [OTA] Bắt đầu tải bản cập nhật Firmware v%s từ HTTPS URL...\n", targetVersion);
    WiFiClientSecure otaClient;
    if (MQTT_USE_SSL) {
        otaClient.setCACert(ROOT_CA_DIGICERT);
    } else {
        otaClient.setInsecure(); // Bỏ qua kiểm tra CA nếu dùng môi trường thử nghiệm không mã hóa
    }

    httpUpdate.rebootOnUpdate(false); // Không tự động reboot ngay để kiểm tra lỗi
    t_httpUpdate_return ret = httpUpdate.update(otaClient, url);

    if (ret == HTTP_UPDATE_OK) {
        Serial.println("✅ [OTA] Cập nhật thành công! Đang Reboot khởi động bản nạp mới...");
        delay(1000);
        ESP.restart();
    } else {
        Serial.printf("❌ [OTA] Cập nhật thất bại lỗi: %s\n", httpUpdate.getLastErrorString().c_str());
    }
}

// ============================================================================
// HÀM XỬ LÝ LỆNH ĐIỀU KHIỂN NHẬN TỪ MQTT CLOUD (CALLBACK)
// ============================================================================
void handleMQTTCommand(const String& topic, const String& payload) {
    Serial.printf("📩 [MQTT Command] [%s]: %s\n", topic.c_str(), payload.c_str());

    // 1. Điều khiển Đèn (Relay 1) qua topic esp32/{device_id}/control/relay1
    if (topic == getTopicRelay1Control()) {
        bool state = (payload == "ON");
        relayController.setRelay1(state);
        mqttManager.publishRelayStatus(1, state);
    } 
    // 2. Điều khiển Quạt (Relay 2) qua topic esp32/{device_id}/control/relay2
    else if (topic == getTopicRelay2Control()) {
        bool state = (payload == "ON");
        relayController.setRelay2(state);
        mqttManager.publishRelayStatus(2, state);
    } 
    // 3. Kích hoạt cập nhật OTA qua topic esp32/{device_id}/system/ota_trigger
    else if (topic == getTopicOTATrigger()) {
        StaticJsonDocument<384> doc;
        if (!deserializeJson(doc, payload)) {
            const char* newVer = doc["version"];
            const char* otaUrl = doc["url"];
            if (newVer && otaUrl && String(newVer) != String(FIRMWARE_VERSION)) {
                executeHTTPSOTA(otaUrl, newVer);
            }
        }
    }
}

// ============================================================================
// HÀM KHỞI TẠO LOCAL WEB DASHBOARD (PHƯƠNG ÁN 3) & ARDUINO OTA
// ============================================================================
void setupLocalWebDashboard() {
    // Trang chủ hiển thị Giao diện HTML/CSS/JS nhúng trong PROGMEM
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", WEB_DASHBOARD_HTML);
    });

    // Endpoint REST API lấy dữ liệu JSON cảm biến và trạng thái hệ thống
    server.on("/api/data", HTTP_GET, []() {
        StaticJsonDocument<384> doc;
        doc["device_id"] = DEVICE_ID;
        doc["temperature"] = sensorManager.getTemperature();
        doc["humidity"] = sensorManager.getHumidity();
        doc["soil_humidity"] = sensorManager.getSoilHumidity();
        doc["relay1"] = relayController.getRelay1StateStr();
        doc["relay2"] = relayController.getRelay2StateStr();
        doc["relay1_light"] = relayController.getRelay1StateStr();
        doc["relay2_fan"] = relayController.getRelay2StateStr();
        doc["rssi"] = wifiManager.getRSSI();
        doc["ip"] = wifiManager.getLocalIP();
        doc["version"] = FIRMWARE_VERSION;
        doc["uptime_s"] = millis() / 1000;

        String jsonResponse;
        serializeJson(doc, jsonResponse);
        server.send(200, "application/json", jsonResponse);
    });

    // Endpoint REST API điều khiển Relay 1 (Đèn)
    server.on("/api/relay1", HTTP_GET, []() {
        if (server.hasArg("state")) {
            String state = server.arg("state");
            bool isOn = (state == "ON");
            relayController.setRelay1(isOn);
            mqttManager.publishRelayStatus(1, isOn);
        }
        server.send(200, "application/json", "{\"status\":\"ok\"}");
    });

    // Endpoint REST API điều khiển Relay 2 (Quạt)
    server.on("/api/relay2", HTTP_GET, []() {
        if (server.hasArg("state")) {
            String state = server.arg("state");
            bool isOn = (state == "ON");
            relayController.setRelay2(isOn);
            mqttManager.publishRelayStatus(2, isOn);
        }
        server.send(200, "application/json", "{\"status\":\"ok\"}");
    });

    // Bắt đầu dịch vụ mDNS Responder (Truy cập bằng địa chỉ http://esp32.local)
    if (MDNS.begin("esp32")) {
        Serial.println("🌐 [mDNS] Responder đã sẵn sàng: http://esp32.local");
    }

    server.begin();
}

void setupArduinoOTA() {
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.onStart([]() { Serial.println("🌐 [Arduino OTA] Bắt đầu nạp qua mạng LAN..."); });
    ArduinoOTA.onEnd([]() { Serial.println("\n🌐 [Arduino OTA] Nạp hoàn tất!"); });
    ArduinoOTA.begin();
}

// ============================================================================
// CHƯƠNG TRÌNH KHỞI TẠO BAN ĐẦU (SETUP)
// ============================================================================
void setup() {
    Serial.begin(115200);
    Serial.println("\n==================================================");
    Serial.println("🚀 ESP32 IoT Automation Enterprise Firmware Booting...");
    Serial.printf("🆔 Device ID: %s | Version: %s\n", DEVICE_ID, FIRMWARE_VERSION);
    Serial.println("==================================================\n");

    // 1. Khởi tạo các module phần cứng
    relayController.begin();
    sensorManager.begin();

    // 2. Khởi tạo dịch vụ kết nối Wi-Fi & MQTT Client
    wifiManager.begin();
    mqttManager.begin(handleMQTTCommand);

    // 3. Khởi chạy các dịch vụ mạng nội bộ
    setupArduinoOTA();
    setupLocalWebDashboard();
}

// ============================================================================
// VÒNG LẶP CHÍNH THỰC THI LIÊN TỤC (LOOP - NON-BLOCKING)
// ============================================================================
void loop() {
    // 1. Duy trì các module mạng và cảm biến ngầm
    wifiManager.loop();
    sensorManager.loop();

    // 2. Xử lý thuật toán Hysteresis tự động điều khiển quạt theo nhiệt độ
    relayController.processHysteresis(sensorManager.getTemperature());

    // 3. Duy trì kết nối MQTT Cloud với thuật toán Exponential Backoff
    bool isWifiOk = wifiManager.isConnected();
    mqttManager.loop(isWifiOk);

    // 4. Lắng nghe các dịch vụ Web Dashboard & Arduino OTA
    ArduinoOTA.handle();
    server.handleClient();

    // 5. Định kỳ publish dữ liệu Telemetry JSON đầy đủ chỉ số lên MQTT Cloud mỗi 5 giây
    unsigned long now = millis();
    if (now - lastSensorPublishTime >= publishInterval) {
        lastSensorPublishTime = now;
        if (mqttManager.isConnected()) {
            mqttManager.publishTelemetry(
                sensorManager.getTemperature(),
                sensorManager.getHumidity(),
                sensorManager.getSoilHumidity(),
                relayController.getRelay1State(),
                relayController.getRelay2State(),
                wifiManager.getRSSI(),
                wifiManager.getLocalIP()
            );
        }
    }
}
