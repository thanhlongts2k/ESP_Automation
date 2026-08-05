/**
 * DỰ ÁN: ESP32 IoT Automation (ESP_Automation)
 * CHỨC NĂNG PHƯƠNG ÁN 3 (LOCAL WI-FI DASHBOARD + MQTT CLOUD):
 *  1. Tự chạy Web Server nội bộ (IP: http://<IP_ESP32>/ hoặc http://esp32.local)
 *  2. Giao diện Web Dark Mode Glassmorphism tuyệt đẹp tự động cập nhật Nhiệt độ/Độ ẩm Realtime.
 *  3. Nút gạt Toggle Switch điều khiển Đèn (Relay 1) & Quạt (Relay 2) trực tiếp trên Web điện thoại.
 *  4. Đọc cảm biến DHT22, tự động kết nối Wi-Fi & MQTT SSL (Port 8883) + Trọn bộ 3 Phương thức OTA.
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <HTTPUpdate.h>
#include "esp_ota_ops.h"

#include "config.h"
#include "web_dashboard.h"

// ============================================================================
// KHỞI TẠO ĐỐI TƯỢNG (OBJECTS)
// ============================================================================
DHT dht(DHT_PIN, DHT_TYPE);
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
WebServer server(80);

unsigned long lastSensorReadTime = 0;
const long sensorInterval = 2000; // Đọc cảm biến mỗi 2 giây

float currentTemperature = 0.0;
float currentHumidity = 0.0;
String relay1StateStr = "OFF";
String relay2StateStr = "OFF";

// ============================================================================
// HÀM KHỞI TẠO WEB SERVER LOCAL DASHBOARD (PHƯƠNG ÁN 3)
// ============================================================================
void setupLocalWebDashboard() {
  // 1. Trang chủ Web Dashboard
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", WEB_DASHBOARD_HTML);
  });

  // 2. API trả về JSON dữ liệu cảm biến & trạng thái Relay
  server.on("/api/data", HTTP_GET, []() {
    StaticJsonDocument<256> doc;
    doc["temperature"] = currentTemperature;
    doc["humidity"] = currentHumidity;
    doc["relay1"] = relay1StateStr;
    doc["relay2"] = relay2StateStr;
    doc["ip"] = WiFi.localIP().toString();
    doc["version"] = FIRMWARE_VERSION;

    String jsonResponse;
    serializeJson(doc, jsonResponse);
    server.send(200, "application/json", jsonResponse);
  });

  // 3. API Điều khiển Relay 1 (Đèn)
  server.on("/api/relay1", HTTP_GET, []() {
    if (server.hasArg("state")) {
      String state = server.arg("state");
      if (state == "ON") {
        digitalWrite(RELAY1_PIN, RELAY_ACTIVE_LEVEL);
        relay1StateStr = "ON";
      } else if (state == "OFF") {
        digitalWrite(RELAY1_PIN, RELAY_INACTIVE_LEVEL);
        relay1StateStr = "OFF";
      }
    }
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // 4. API Điều khiển Relay 2 (Quạt)
  server.on("/api/relay2", HTTP_GET, []() {
    if (server.hasArg("state")) {
      String state = server.arg("state");
      if (state == "ON") {
        digitalWrite(RELAY2_PIN, RELAY_ACTIVE_LEVEL);
        relay2StateStr = "ON";
      } else if (state == "OFF") {
        digitalWrite(RELAY2_PIN, RELAY_INACTIVE_LEVEL);
        relay2StateStr = "OFF";
      }
    }
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // 5. Cấu hình mDNS (Truy cập bằng http://esp32.local)
  if (MDNS.begin("esp32")) {
    Serial.println("mDNS responder started: http://esp32.local");
  }

  server.begin();
  Serial.println("🌐 Web Dashboard Local đã sẵn sàng tại: http://" + WiFi.localIP().toString());
}

// ============================================================================
// HÀM KHỞI TẠO ARDUINO OTA (PHƯƠNG THỨC 1)
// ============================================================================
void setupArduinoOTA() {
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.onStart([]() {
    Serial.println("Bắt đầu Arduino OTA nạp...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nArduino OTA Hoàn Thành!");
  });
  ArduinoOTA.begin();
}

// ============================================================================
// HÀM THỰC THI HTTPS CLOUD OTA (PHƯƠNG THỨC 3)
// ============================================================================
void executeHTTPSOTA(const char* url, const char* targetVersion) {
  Serial.printf("Bắt đầu tải bản cập nhật Firmware v%s từ HTTPS URL...\n", targetVersion);
  WiFiClientSecure otaClient;
  otaClient.setCACert(root_ca_digicert);

  httpUpdate.rebootOnUpdate(false);
  t_httpUpdate_return ret = httpUpdate.update(otaClient, url);

  if (ret == HTTP_UPDATE_OK) {
    Serial.println("✅ OTA Thành Công! Đang Reboot...");
    delay(1000);
    ESP.restart();
  }
}

// ============================================================================
// HÀM XỬ LÝ NHẬN TIN NHẮN MQTT (CALLBACK)
// ============================================================================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.printf("Nhận tin nhắn [%s]: %s\n", topic, message.c_str());

  if (String(topic) == TOPIC_CONTROL_RELAY1) {
    if (message == "ON") {
      digitalWrite(RELAY1_PIN, RELAY_ACTIVE_LEVEL);
      relay1StateStr = "ON";
    } else if (message == "OFF") {
      digitalWrite(RELAY1_PIN, RELAY_INACTIVE_LEVEL);
      relay1StateStr = "OFF";
    }
  } else if (String(topic) == TOPIC_CONTROL_RELAY2) {
    if (message == "ON") {
      digitalWrite(RELAY2_PIN, RELAY_ACTIVE_LEVEL);
      relay2StateStr = "ON";
    } else if (message == "OFF") {
      digitalWrite(RELAY2_PIN, RELAY_INACTIVE_LEVEL);
      relay2StateStr = "OFF";
    }
  } else if (String(topic) == TOPIC_OTA_TRIGGER) {
    StaticJsonDocument<384> doc;
    if (!deserializeJson(doc, message)) {
      const char* newVersion = doc["version"];
      const char* otaUrl = doc["url"];
      if (newVersion && otaUrl && String(newVersion) != String(FIRMWARE_VERSION)) {
        executeHTTPSOTA(otaUrl, newVersion);
      }
    }
  }
}

// ============================================================================
// HÀM KẾT NỐI WI-FI & MQTT
// ============================================================================
void setupWiFi() {
  delay(10);
  Serial.print("Đang kết nối Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Đã Kết Nối!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  if (!mqttClient.connected()) {
    Serial.print("Đang kết nối MQTT SSL Server (8883)...");
    if (mqttClient.connect(DEVICE_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Thành công!");
      
      const esp_partition_t *running = esp_ota_get_running_partition();
      esp_ota_img_states_t ota_state;
      if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK && ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
        esp_ota_mark_app_valid_cancel_rollback();
      }

      mqttClient.subscribe(TOPIC_CONTROL_RELAY1);
      mqttClient.subscribe(TOPIC_CONTROL_RELAY2);
      mqttClient.subscribe(TOPIC_OTA_TRIGGER);
    }
  }
}

// ============================================================================
// SETUP BAN ĐẦU
// ============================================================================
void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, RELAY_INACTIVE_LEVEL);
  digitalWrite(RELAY2_PIN, RELAY_INACTIVE_LEVEL);

  dht.begin();
  setupWiFi();

  espClient.setCACert(root_ca_digicert);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  setupArduinoOTA();
  setupLocalWebDashboard(); // Khởi chạy Web Server Local Wi-Fi Dashboard
}

// ============================================================================
// LOOP CHÍNH
// ============================================================================
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }
  
  // Duy trì MQTT (nếu có cấu hình)
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // Duy trì dịch vụ Web Local & OTA
  ArduinoOTA.handle();
  server.handleClient();

  // Định kỳ đọc DHT22
  unsigned long currentMillis = millis();
  if (currentMillis - lastSensorReadTime >= sensorInterval) {
    lastSensorReadTime = currentMillis;

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (!isnan(h) && !isnan(t)) {
      currentTemperature = t;
      currentHumidity = h;

      if (mqttClient.connected()) {
        StaticJsonDocument<200> doc;
        doc["device_id"] = DEVICE_ID;
        doc["temperature"] = currentTemperature;
        doc["humidity"] = currentHumidity;
        doc["relay1"] = relay1StateStr;
        doc["relay2"] = relay2StateStr;
        doc["version"] = FIRMWARE_VERSION;

        char jsonBuffer[200];
        serializeJson(doc, jsonBuffer);
        mqttClient.publish(TOPIC_SENSOR_DATA, jsonBuffer);
      }
    }
  }
}
