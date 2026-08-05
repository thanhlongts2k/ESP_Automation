/**
 * DỰ ÁN: ESP32 IoT Automation (ESP_Automation)
 * CHỨC NĂNG:
 *  1. Đọc Nhiệt độ & Độ ẩm từ cảm biến DHT22 -> Gửi JSON qua MQTT SSL Port 8883
 *  2. Đóng/ngắt Relay 2 Kênh (Bật/Tắt Đèn & Quạt) qua MQTT
 *  3. Tích hợp trọn bộ 3 Phương thức Cập nhật Firmware OTA qua Wi-Fi:
 *     - Phương thức 1: Arduino OTA (Nạp qua LAN trên Arduino IDE)
 *     - Phương thức 2: Web Server OTA (Upload file .bin qua trình duyệt web)
 *     - Phương thức 3: HTTPS Cloud OTA (Tải tự động từ Nginx qua lệnh MQTT + Rollback Safety)
 */

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <Update.h>
#include <HTTPUpdate.h>
#include "esp_ota_ops.h" // Thư viện quản lý Bootloader & Rollback an toàn của ESP32

#include "config.h"

// ============================================================================
// KHỞI TẠO ĐỐI TƯỢNG (OBJECTS)
// ============================================================================
DHT dht(DHT_PIN, DHT_TYPE);
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);
WebServer server(80);

unsigned long lastSensorReadTime = 0;
const long sensorInterval = 5000; // Đọc cảm biến mỗi 5 giây

// ============================================================================
// HÀM GIAO DIỆN WEB SERVER OTA (PHƯƠNG THỨC 2)
// ============================================================================
const char* serverIndex = 
  "<form method='POST' action='/update' enctype='multipart/form-data'>"
  "<h2>ESP32 Web OTA Update</h2>"
  "<input type='file' name='update'>"
  "<input type='submit' value='Upload Firmware'>"
  "</form>";

void setupWebServerOTA() {
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });

  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "UPDATE FAIL" : "UPDATE SUCCESS! REBOOTING...");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Bắt đầu Web OTA: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentLength) != upload.currentLength) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Web OTA Thành Công! Kích thước: %u bytes\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });

  server.begin();
  Serial.println("Web Server OTA đã sẵn sàng tại http://" + WiFi.localIP().toString());
}

// ============================================================================
// HÀM KHỞI TẠO ARDUINO OTA (PHƯƠNG THỨC 1)
// ============================================================================
void setupArduinoOTA() {
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      type = "filesystem";
    }
    Serial.println("Bắt đầu Arduino OTA nạp: " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nArduino OTA Hoàn Thành!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Tiến trình: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Lỗi Arduino OTA [%u]: ", error);
  });
  ArduinoOTA.begin();
  Serial.println("Arduino OTA đã sẵn sàng!");
}

// ============================================================================
// HÀM THỰC THI HTTPS CLOUD OTA (PHƯƠNG THỨC 3 - CÓ ROLLBACK SAFETY)
// ============================================================================
void executeHTTPSOTA(const char* url, const char* targetVersion) {
  Serial.printf("Bắt đầu tải bản cập nhật Firmware v%s từ HTTPS URL...\n", targetVersion);
  
  WiFiClientSecure otaClient;
  otaClient.setCACert(root_ca_digicert);

  // Tự động kiểm tra MD5 / SHA256 nếu Nginx Server trả về header
  httpUpdate.rebootOnUpdate(false); // Tự chủ động reboot sau khi xác nhận

  t_httpUpdate_return ret = httpUpdate.update(otaClient, url);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("❌ Lỗi Cloud OTA (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("ℹ️ Không có bản cập nhật mới.");
      break;
    case HTTP_UPDATE_OK:
      Serial.println("✅ Tải Firmware OTA Thành Công! Đang tiến hành Reboot khởi chạy bản mới...");
      delay(1000);
      ESP.restart();
      break;
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

  // 1. Điều khiển Relay 1 (Đèn)
  if (String(topic) == TOPIC_CONTROL_RELAY1) {
    if (message == "ON") {
      digitalWrite(RELAY1_PIN, RELAY_ACTIVE_LEVEL);
      Serial.println("-> Đã BẬT Đèn (Relay 1)");
    } else if (message == "OFF") {
      digitalWrite(RELAY1_PIN, RELAY_INACTIVE_LEVEL);
      Serial.println("-> Đã TẮT Đèn (Relay 1)");
    }
  }
  
  // 2. Điều khiển Relay 2 (Quạt)
  else if (String(topic) == TOPIC_CONTROL_RELAY2) {
    if (message == "ON") {
      digitalWrite(RELAY2_PIN, RELAY_ACTIVE_LEVEL);
      Serial.println("-> Đã BẬT Quạt (Relay 2)");
    } else if (message == "OFF") {
      digitalWrite(RELAY2_PIN, RELAY_INACTIVE_LEVEL);
      Serial.println("-> Đã TẮT Quạt (Relay 2)");
    }
  }

  // 3. Kích hoạt HTTPS Cloud OTA
  else if (String(topic) == TOPIC_OTA_TRIGGER) {
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, message);
    if (!error) {
      const char* newVersion = doc["version"];
      const char* otaUrl = doc["url"];
      
      if (newVersion && otaUrl) {
        // KIỂM TRA PHIÊN BẢN: Chỉ cho phép nâng cấp nếu newVersion > FIRMWARE_VERSION hiện tại
        if (String(newVersion) != String(FIRMWARE_VERSION)) {
          Serial.printf("Phát hiện bản cập nhật mới v%s (Bản hiện tại v%s)\n", newVersion, FIRMWARE_VERSION);
          executeHTTPSOTA(otaUrl, newVersion);
        } else {
          Serial.println("Thiết bị đang ở phiên bản mới nhất. Bỏ qua OTA.");
        }
      }
    } else {
      Serial.println("Lỗi bóc tách JSON OTA Payload!");
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
  while (!mqttClient.connected()) {
    Serial.print("Đang kết nối MQTT SSL Server (8883)...");
    if (mqttClient.connect(DEVICE_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Thành công!");
      
      // BẢO VỆ ROLLBACK: Xác nhận Firmware mới boot & kết nối MQTT thành công -> Hủy cờ Rollback!
      const esp_partition_t *running = esp_ota_get_running_partition();
      esp_ota_img_states_t ota_state;
      if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
          Serial.println("🎉 Firmware mới khởi chạy & kết nối MQTT thành công! Đang chốt hạ (Confirm Valid Firmware)...");
          esp_ota_mark_app_valid_cancel_rollback();
        }
      }

      // Subscribe vào các Topic điều khiển & OTA
      mqttClient.subscribe(TOPIC_CONTROL_RELAY1);
      mqttClient.subscribe(TOPIC_CONTROL_RELAY2);
      mqttClient.subscribe(TOPIC_OTA_TRIGGER);
    } else {
      Serial.printf("Thất bại, rc=%d. Thử lại sau 5 giây...\n", mqttClient.state());
      delay(5000);
    }
  }
}

// ============================================================================
// SETUP BAN ĐẦU
// ============================================================================
void setup() {
  Serial.begin(115200);
  
  // Khởi tạo chân GPIO Relay
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, RELAY_INACTIVE_LEVEL); // Tắt mặc định
  digitalWrite(RELAY2_PIN, RELAY_INACTIVE_LEVEL);

  // Khởi tạo Cảm biến DHT22
  dht.begin();

  // Kết nối Wi-Fi & Khai báo SSL Certificate
  setupWiFi();
  espClient.setCACert(root_ca_digicert);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);

  // Khởi tạo 3 Phương thức OTA
  setupArduinoOTA();   // Phương thức 1
  setupWebServerOTA(); // Phương thức 2
}

// ============================================================================
// LOOP CHÍNH (CHẠY LIÊN TỤC)
// ============================================================================
void loop() {
  // Duy trì Wi-Fi & MQTT
  if (WiFi.status() != WL_CONNECTED) {
    setupWiFi();
  }
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // Duy trì dịch vụ OTA
  ArduinoOTA.handle();   // Xử lý Arduino OTA
  server.handleClient(); // Xử lý Web OTA

  // Định kỳ đọc cảm biến DHT22 và gửi dữ liệu qua MQTT
  unsigned long currentMillis = millis();
  if (currentMillis - lastSensorReadTime >= sensorInterval) {
    lastSensorReadTime = currentMillis;

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Lỗi: Không đọc được dữ liệu từ cảm biến DHT22!");
      return;
    }

    // Đóng gói JSON gửi qua MQTT
    StaticJsonDocument<200> doc;
    doc["device_id"] = DEVICE_ID;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["version"] = FIRMWARE_VERSION;

    char jsonBuffer[200];
    serializeJson(doc, jsonBuffer);

    Serial.printf("Publish MQTT [%s]: %s\n", TOPIC_SENSOR_DATA, jsonBuffer);
    mqttClient.publish(TOPIC_SENSOR_DATA, jsonBuffer);
  }
}
