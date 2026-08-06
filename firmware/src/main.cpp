/**
 * DỰ ÁN: ESP32 IoT Automation (ESP_Automation)
 * KIẾN TRÚC: MODULAR ENTERPRISE C++ (CỤM TOPICS ESP32/{DEVICE_ID}/..., LWT, RETAINED, EXPONENTIAL BACKOFF)
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

// Khởi tạo các Module Đối tượng
WiFiManager wifiManager;
SensorManager sensorManager;
RelayController relayController;
MQTTManager mqttManager;
WebServer server(80);

unsigned long lastSensorPublishTime = 0;
const unsigned long publishInterval = 5000; // Publish cảm biến mỗi 5s

// ============================================================================
// HÀM XỬ LÝ HTTPS CLOUD OTA (PHƯƠNG THỨC 3)
// ============================================================================
void executeHTTPSOTA(const char* url, const char* targetVersion) {
    Serial.printf("☁️ [OTA] Bắt đầu tải bản cập nhật Firmware v%s từ HTTPS URL...\n", targetVersion);
    WiFiClientSecure otaClient;
    if (MQTT_USE_SSL) {
        otaClient.setCACert(ROOT_CA_DIGICERT);
    } else {
        otaClient.setInsecure();
    }

    httpUpdate.rebootOnUpdate(false);
    t_httpUpdate_return ret = httpUpdate.update(otaClient, url);

    if (ret == HTTP_UPDATE_OK) {
        Serial.println("✅ [OTA] Cập nhật thành công! Đang Reboot...");
        delay(1000);
        ESP.restart();
    }
}

// ============================================================================
// HÀM XỬ LÝ LỆNH TỪ MQTT (CALLBACK)
// ============================================================================
void handleMQTTCommand(const String& topic, const String& payload) {
    Serial.printf("📩 [MQTT Command] [%s]: %s\n", topic.c_str(), payload.c_str());

    if (topic == getTopicRelay1Control()) {
        bool state = (payload == "ON");
        relayController.setRelay1(state);
        mqttManager.publishRelayStatus(1, state);
    } 
    else if (topic == getTopicRelay2Control()) {
        bool state = (payload == "ON");
        relayController.setRelay2(state);
        mqttManager.publishRelayStatus(2, state);
    } 
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
// HÀM KHỞI TẠO WEB SERVER LOCAL & ARDUINO OTA
// ============================================================================
void setupLocalWebDashboard() {
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", WEB_DASHBOARD_HTML);
    });

    server.on("/api/data", HTTP_GET, []() {
        StaticJsonDocument<256> doc;
        doc["temperature"] = sensorManager.getTemperature();
        doc["humidity"] = sensorManager.getHumidity();
        doc["soil_humidity"] = sensorManager.getSoilHumidity();
        doc["relay1"] = relayController.getRelay1StateStr();
        doc["relay2"] = relayController.getRelay2StateStr();
        doc["ip"] = wifiManager.getLocalIP();
        doc["version"] = FIRMWARE_VERSION;

        String jsonResponse;
        serializeJson(doc, jsonResponse);
        server.send(200, "application/json", jsonResponse);
    });

    server.on("/api/relay1", HTTP_GET, []() {
        if (server.hasArg("state")) {
            String state = server.arg("state");
            bool isOn = (state == "ON");
            relayController.setRelay1(isOn);
            mqttManager.publishRelayStatus(1, isOn);
        }
        server.send(200, "application/json", "{\"status\":\"ok\"}");
    });

    server.on("/api/relay2", HTTP_GET, []() {
        if (server.hasArg("state")) {
            String state = server.arg("state");
            bool isOn = (state == "ON");
            relayController.setRelay2(isOn);
            mqttManager.publishRelayStatus(2, isOn);
        }
        server.send(200, "application/json", "{\"status\":\"ok\"}");
    });

    if (MDNS.begin("esp32")) {
        Serial.println("🌐 [mDNS] http://esp32.local");
    }

    server.begin();
}

void setupArduinoOTA() {
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.onStart([]() { Serial.println("🌐 [Arduino OTA] Bắt đầu..."); });
    ArduinoOTA.onEnd([]() { Serial.println("\n🌐 [Arduino OTA] Hoàn thành!"); });
    ArduinoOTA.begin();
}

// ============================================================================
// CHƯƠNG TRÌNH CHÍNH (SETUP & LOOP)
// ============================================================================
void setup() {
    Serial.begin(115200);
    Serial.println("\n==================================================");
    Serial.println("🚀 ESP32 IoT Automation Enterprise Firmware Booting...");
    Serial.printf("🆔 Device ID: %s | Version: %s\n", DEVICE_ID, FIRMWARE_VERSION);
    Serial.println("==================================================\n");

    relayController.begin();
    sensorManager.begin();
    wifiManager.begin();
    mqttManager.begin(handleMQTTCommand);

    setupArduinoOTA();
    setupLocalWebDashboard();
}

void loop() {
    wifiManager.loop();
    sensorManager.loop();
    relayController.processHysteresis(sensorManager.getTemperature());

    bool isWifiOk = wifiManager.isConnected();
    mqttManager.loop(isWifiOk);

    ArduinoOTA.handle();
    server.handleClient();

    // Định kỳ publish dữ liệu telemetry lên MQTT
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
