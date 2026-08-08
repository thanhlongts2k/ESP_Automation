#include "wifi_manager.h"
#include <ArduinoJson.h>

static const char CAPTIVE_PORTAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Wi-Fi Setup</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
        body { background: linear-gradient(135deg, #0f172a 0%, #1e293b 100%); color: #f8fafc; min-height: 100vh; display: flex; justify-content: center; align-items: center; padding: 20px; }
        .card { background: rgba(30, 41, 59, 0.7); backdrop-filter: blur(16px); border: 1px solid rgba(255, 255, 255, 0.1); border-radius: 20px; padding: 30px; width: 100%; max-width: 440px; box-shadow: 0 20px 50px rgba(0,0,0,0.5); }
        .logo { text-align: center; margin-bottom: 20px; }
        .logo h2 { color: #38bdf8; font-size: 24px; font-weight: 700; letter-spacing: 0.5px; }
        .logo p { color: #94a3b8; font-size: 13px; margin-top: 4px; }
        .form-group { margin-bottom: 18px; }
        label { display: block; font-size: 13px; color: #cbd5e1; margin-bottom: 6px; font-weight: 600; }
        select, input[type="password"], input[type="text"] { width: 100%; padding: 12px 14px; background: rgba(15, 23, 42, 0.6); border: 1px solid rgba(255, 255, 255, 0.15); border-radius: 10px; color: #fff; font-size: 14px; outline: none; transition: all 0.3s ease; }
        select:focus, input:focus { border-color: #38bdf8; box-shadow: 0 0 0 3px rgba(56, 189, 248, 0.2); }
        .btn { width: 100%; padding: 14px; background: linear-gradient(135deg, #0284c7 0%, #0369a1 100%); color: #fff; border: none; border-radius: 10px; font-size: 15px; font-weight: 600; cursor: pointer; transition: transform 0.2s, box-shadow 0.2s; box-shadow: 0 4px 15px rgba(2, 132, 199, 0.4); margin-top: 10px; }
        .btn:hover { transform: translateY(-2px); box-shadow: 0 6px 20px rgba(2, 132, 199, 0.6); }
        .btn-scan { background: rgba(51, 65, 85, 0.8); border: 1px solid rgba(255,255,255,0.1); margin-bottom: 16px; font-size: 13px; padding: 10px; }
        .status-msg { margin-top: 15px; font-size: 13px; text-align: center; color: #38bdf8; display: none; }
        .wifi-list-option { display: flex; justify-content: space-between; }
    </style>
</head>
<body>
    <div class="card">
        <div class="logo">
            <h2>🌐 ESP32 Wi-Fi Setup</h2>
            <p>Cấu hình Wi-Fi kết nối cho thiết bị Smart Home</p>
        </div>
        
        <button class="btn btn-scan" onclick="scanWifi()">🔄 Quét danh sách Wi-Fi xung quanh</button>

        <form id="wifiForm" onsubmit="saveWifi(event)">
            <div class="form-group">
                <label for="ssid">Tên Mạng Wi-Fi (SSID)</label>
                <select id="ssidSelect" onchange="document.getElementById('ssid').value=this.value">
                    <option value="">-- Đang quét Wi-Fi... --</option>
                </select>
                <input type="text" id="ssid" name="ssid" placeholder="Hoặc nhập tên Wi-Fi..." required style="margin-top: 8px;">
            </div>

            <div class="form-group">
                <label for="pass">Mật Khẩu Wi-Fi</label>
                <input type="password" id="pass" name="pass" placeholder="Nhập mật khẩu Wi-Fi..." required>
            </div>

            <button type="submit" class="btn">💾 Lưu & Kết Nối Ngay</button>
        </form>

        <div id="status" class="status-msg">⏳ Đang lưu cấu hình và khởi động lại ESP32...</div>
    </div>

    <script>
        function scanWifi() {
            const select = document.getElementById('ssidSelect');
            select.innerHTML = '<option value="">-- Đang quét Wi-Fi xung quanh... --</option>';
            fetch('/api/wifi-scan')
                .then(r => r.json())
                .then(data => {
                    select.innerHTML = '<option value="">-- Chọn Wi-Fi từ danh sách --</option>';
                    data.forEach(item => {
                        const opt = document.createElement('option');
                        opt.value = item.ssid;
                        opt.textContent = `${item.ssid} (${item.rssi} dBm)`;
                        select.appendChild(opt);
                    });
                })
                .catch(err => {
                    select.innerHTML = '<option value="">❌ Không thể scan Wi-Fi, hãy gõ tay bên dưới</option>';
                });
        }

        function saveWifi(e) {
            e.preventDefault();
            const ssid = document.getElementById('ssid').value;
            const pass = document.getElementById('pass').value;
            const status = document.getElementById('status');
            status.style.display = 'block';

            const formData = new FormData();
            formData.append('ssid', ssid);
            formData.append('pass', pass);

            fetch('/save-wifi', { method: 'POST', body: formData })
                .then(r => r.text())
                .then(msg => {
                    status.innerHTML = '✅ Đã lưu Wi-Fi thành công! ESP32 đang khởi động lại...';
                })
                .catch(err => {
                    status.innerHTML = '⚠️ Đã gửi lệnh lưu Wi-Fi! ESP32 đang khởi động lại...';
                });
        }

        window.onload = scanWifi;
    </script>
</body>
</html>
)rawliteral";

WiFiManager::WiFiManager()
    : _server(nullptr), _apMode(false), _lastConnectAttempt(0), _bootButtonPressStart(0), _bootButtonPressed(false) {}

void WiFiManager::begin(WebServer* server) {
    _server = server;
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);

    Serial.println("🌐 [WiFi] Khởi tạo hệ thống Wi-Fi & NVS Storage...");
    
    // 1. Nạp danh sách Wi-Fi từ NVS
    _loadCredentialsFromNVS();

    // 2. Thêm Wi-Fi mặc định từ .env làm dự phòng
    if (String(WIFI_SSID).length() > 0 && String(WIFI_SSID) != "Tên_WiFi_Nha_Ban") {
        _wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
        Serial.printf("📌 [WiFi NVS] Thêm Wi-Fi mặc định (.env): %s\n", WIFI_SSID);
    }

    // 3. Thử kết nối trong thời gian timeout (15s)
    WiFi.mode(WIFI_STA);
    Serial.printf("⏳ [WiFi] Đang thử kết nối tới các mạng Wi-Fi đã lưu (Timeout %d ms)...\n", WIFI_CONNECT_TIMEOUT_MS);
    
    unsigned long startMs = millis();
    bool connected = false;

    while (millis() - startMs < WIFI_CONNECT_TIMEOUT_MS) {
        if (_wifiMulti.run() == WL_CONNECTED) {
            connected = true;
            break;
        }
        delay(250);
    }

    if (connected) {
        _apMode = false;
        Serial.println("✅ [WiFi] Đã kết nối Wi-Fi thành công!");
        Serial.printf("📍 IP Address: %s (RSSI: %d dBm)\n", WiFi.localIP().toString().c_str(), WiFi.RSSI());
    } else {
        Serial.println("⚠️ [WiFi] Không thể kết nối tới mạng Wi-Fi nào! Bật Captive Portal AP...");
        _startCaptivePortal();
    }
}

void WiFiManager::_loadCredentialsFromNVS() {
    _prefs.begin("wificreds", true); // Open in read-only mode
    int count = _prefs.getInt("count", 0);
    Serial.printf("📦 [WiFi NVS] Tìm thấy %d bộ Wi-Fi lưu trong Flash NVS.\n", count);

    for (int i = 0; i < count && i < 5; i++) {
        String keySsid = "s" + String(i);
        String keyPass = "p" + String(i);
        String ssid = _prefs.getString(keySsid.c_str(), "");
        String pass = _prefs.getString(keyPass.c_str(), "");
        if (ssid.length() > 0) {
            _wifiMulti.addAP(ssid.c_str(), pass.c_str());
            Serial.printf("   + NVS WiFi [%d]: %s\n", i + 1, ssid.c_str());
        }
    }
    _prefs.end();
}

bool WiFiManager::saveCredential(const String& ssid, const String& pass) {
    if (ssid.length() == 0) return false;

    _prefs.begin("wificreds", false); // Read-write
    int count = _prefs.getInt("count", 0);
    
    // Kiểm tra xem SSID đã tồn tại chưa
    int targetIdx = count;
    for (int i = 0; i < count; i++) {
        String s = _prefs.getString(("s" + String(i)).c_str(), "");
        if (s == ssid) {
            targetIdx = i;
            break;
        }
    }

    if (targetIdx == count && count < 5) {
        count++;
        _prefs.putInt("count", count);
    }

    _prefs.putString(("s" + String(targetIdx)).c_str(), ssid);
    _prefs.putString(("p" + String(targetIdx)).c_str(), pass);
    _prefs.end();

    Serial.printf("💾 [WiFi NVS] Đã lưu thành công Wi-Fi mới: %s\n", ssid.c_str());
    return true;
}

void WiFiManager::resetCredentials() {
    Serial.println("🧹 [WiFi NVS] Đang xóa toàn bộ cấu hình Wi-Fi trong Flash NVS...");
    _prefs.begin("wificreds", false);
    _prefs.clear();
    _prefs.end();
}

void WiFiManager::_startCaptivePortal() {
    _apMode = true;
    WiFi.mode(WIFI_AP_STA);
    
    IPAddress apIP(192, 168, 4, 1);
    IPAddress netMsk(255, 255, 255, 0);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    Serial.printf("📡 [Captive Portal AP] Đã phát Wi-Fi AP: %s (IP: 192.168.4.1)\n", AP_SSID);

    // Đăng ký DNS Server chuyển hướng mọi Domain (*) về IP 192.168.4.1 (Captive Portal)
    _dnsServer.start(53, "*", apIP);

    if (_server) {
        _setupCaptivePortalRoutes();
    }
}

void WiFiManager::_setupCaptivePortalRoutes() {
    if (!_server) return;

    // 1. Trang Captive Portal HTML
    _server->on("/", HTTP_GET, [this]() {
        _server->send(200, "text/html", CAPTIVE_PORTAL_HTML);
    });

    // 2. API Scan Wi-Fi trả về JSON
    _server->on("/api/wifi-scan", HTTP_GET, [this]() {
        _server->send(200, "application/json", scanNetworksJSON());
    });

    // 3. API Lưu Wi-Fi
    _server->on("/save-wifi", HTTP_POST, [this]() {
        if (_server->hasArg("ssid") && _server->hasArg("pass")) {
            String s = _server->arg("ssid");
            String p = _server->arg("pass");
            saveCredential(s, p);
            _server->send(200, "text/html", "<h2>✅ Đã lưu Wi-Fi! Đang khởi động lại ESP32...</h2>");
            delay(1500);
            ESP.restart();
        } else {
            _server->send(400, "text/plain", "Thiếu SSID hoặc Password");
        }
    });

    // 4. Captive Portal Redirection cho Android/iOS (Generate 204 / captive.apple.com)
    _server->onNotFound([this]() {
        _server->sendHeader("Location", "http://192.168.4.1/", true);
        _server->send(302, "text/plain", "");
    });

    _server->begin();
    Serial.println("🌐 [Captive Portal Web] HTTP Server trên Port 80 đã khởi chạy!");
}

String WiFiManager::scanNetworksJSON() {
    int n = WiFi.scanNetworks();
    StaticJsonDocument<1024> doc;
    JsonArray arr = doc.to<JsonArray>();

    for (int i = 0; i < n; ++i) {
        JsonObject obj = arr.createNestedObject();
        obj["ssid"] = WiFi.SSID(i);
        obj["rssi"] = WiFi.RSSI(i);
        obj["secure"] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
    }

    String output;
    serializeJson(doc, output);
    return output;
}

void WiFiManager::loop() {
    // 1. Nếu đang ở chế độ AP Captive Portal -> Xử lý DNS Redirection
    if (_apMode) {
        _dnsServer.processNextRequest();
    } else {
        // 2. Nếu rớt mạng Wi-Fi Station -> Tự động thử lại ngầm
        if (WiFi.status() != WL_CONNECTED) {
            unsigned long now = millis();
            if (now - _lastConnectAttempt >= _reconnectInterval) {
                _lastConnectAttempt = now;
                Serial.println("⚠️ [WiFi] Mất sóng Wi-Fi! Đang tự động kết nối ngầm...");
                if (_wifiMulti.run() == WL_CONNECTED) {
                    Serial.printf("✅ [WiFi] Đã tái kết nối lại Wi-Fi: %s (IP: %s)\n", 
                                  WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
                }
            }
        }
    }

    // 3. Theo dõi nút nhấn BOOT (GPIO 0) -> Giữ 3 giây để Reset NVS Wi-Fi
    if (digitalRead(BOOT_BUTTON_PIN) == LOW) {
        if (!_bootButtonPressed) {
            _bootButtonPressed = true;
            _bootButtonPressStart = millis();
        } else {
            if (millis() - _bootButtonPressStart >= 3000) {
                Serial.println("🔘 [BOOT Button] Đã nhấn giữ 3 giây! Tiến hành xóa Wi-Fi NVS...");
                resetCredentials();
                delay(1000);
                ESP.restart();
            }
        }
    } else {
        _bootButtonPressed = false;
    }
}

bool WiFiManager::isAPMode() const {
    return _apMode;
}

bool WiFiManager::isConnected() const {
    return (WiFi.status() == WL_CONNECTED);
}

String WiFiManager::getLocalIP() const {
    if (_apMode) {
        return WiFi.softAPIP().toString();
    }
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
