#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// 1. CẤU HÌNH THÔNG TIN THIẾT BỊ & PHIÊN BẢN FIRMWARE
// ============================================================================
#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION        "1.0.0"
#endif

#ifndef DEVICE_ID
#define DEVICE_ID               "ESP32_Automation_01"
#endif

#ifndef HOSTNAME
#define HOSTNAME                "esp32-automation"
#endif

// ============================================================================
// 2. CẤU HÌNH WI-FI (ĐỌC TỪ FILE .ENV NẾU DÙNG PLATFORMIO HOẶC CHỈNH TẠI ĐÂY)
// ============================================================================
#ifndef WIFI_SSID
#define WIFI_SSID               "Tên_WiFi_Nha_Ban"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD           "Mat_Khau_WiFi_Nha_Ban"
#endif

// ============================================================================
// 3. CẤU HÌNH MQTT BROKER (SERVER NGINX SSL PORT 8883)
// ============================================================================
#ifndef MQTT_SERVER
#define MQTT_SERVER             "api-vending.doanhnghiep.com"
#endif

#ifndef MQTT_PORT
#define MQTT_PORT               8883
#endif

#ifndef MQTT_USER
#define MQTT_USER               ""
#endif

#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD           ""
#endif

// Bật/tắt SSL tự động dựa theo Port (8883 = SSL, 1883/8083 = Thường)
#ifndef MQTT_USE_SSL
#if MQTT_PORT == 8883
#define MQTT_USE_SSL            true
#else
#define MQTT_USE_SSL            false
#endif
#endif

// ============================================================================
// 4. TOPIC MQTT CHUẨN PHÂN CẤP THEO DEVICE_ID
// ============================================================================
// Mặc định tạo Topic chuẩn: esp32/{device_id}/...
inline String getTopicSensors() {
    return "esp32/" + String(DEVICE_ID) + "/sensors";
}
inline String getTopicStatus() {
    return "esp32/" + String(DEVICE_ID) + "/status";
}
inline String getTopicRelay1Control() {
    return "esp32/" + String(DEVICE_ID) + "/control/relay1";
}
inline String getTopicRelay2Control() {
    return "esp32/" + String(DEVICE_ID) + "/control/relay2";
}
inline String getTopicOTATrigger() {
    return "esp32/" + String(DEVICE_ID) + "/system/ota_trigger";
}

// ============================================================================
// 4. CẤU HÌNH CHÂN GPIO PHẦN CỨNG (SỬ DỤNG VỚI CHÂN ĐẾ 30P)
// ============================================================================
#define DHT_PIN                 23  // Chân dữ liệu Cảm biến DHT22
#define DHT_TYPE                DHT22

#define RELAY1_PIN              18  // Chân điều khiển Relay Kênh 1 (Đèn)
#define RELAY2_PIN              19  // Chân điều khiển Relay Kênh 2 (Quạt)

// Mức kích Relay (Hầu hết Module Relay 5V Opto kích mức THẤP - LOW Active)
#define RELAY_ACTIVE_LEVEL      LOW
#define RELAY_INACTIVE_LEVEL    HIGH

// ============================================================================
// 5. CHỨNG CHỈ ROOT CA DIGICERT GLOBAL ROOT G2 (XÁC THỰC SSL VOI NGINX)
// ============================================================================
const char* root_ca_digicert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDjjCCAnagAwIBAgIQBNsRAYNWoIUvhUzAzxvinzANBgkqhkiG9w0BAQsFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExREaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAxMjAwMDBaFw0zMTExMTAxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTFERpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEAvSegmentation+V8G...\n" \
"-----END CERTIFICATE-----\n";

#endif // CONFIG_H
