#ifndef WEB_DASHBOARD_H
#define WEB_DASHBOARD_H

#include <Arduino.h>

// ============================================================================
// GIAO DIỆN WEB DASHBOARD HIỆN ĐẠI (DARK MODE - GLASSMORPHISM)
// ĐƯỢC LƯU TRỰC TIẾP TRÊN BỘ NHỚ PROGMEM CỦA ESP32
// ============================================================================
const char WEB_DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Automation Dashboard</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; }
    body { background: linear-gradient(135deg, #0f172a 0%, #1e293b 100%); color: #f8fafc; min-height: 100vh; display: flex; justify-content: center; align-items: center; padding: 20px; }
    .container { width: 100%; max-width: 480px; background: rgba(30, 41, 59, 0.7); backdrop-filter: blur(12px); border-radius: 24px; padding: 28px; border: 1px solid rgba(255, 255, 255, 0.1); box-shadow: 0 20px 40px rgba(0,0,0,0.4); }
    .header { text-align: center; margin-bottom: 24px; }
    .header h1 { font-size: 1.6rem; color: #38bdf8; margin-bottom: 6px; }
    .header p { font-size: 0.85rem; color: #94a3b8; }
    .card-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 16px; margin-bottom: 24px; }
    .card { background: rgba(15, 23, 42, 0.6); border-radius: 18px; padding: 18px; text-align: center; border: 1px solid rgba(255, 255, 255, 0.05); }
    .card-icon { font-size: 2rem; margin-bottom: 8px; }
    .card-value { font-size: 1.8rem; font-weight: 700; color: #f1f5f9; }
    .card-label { font-size: 0.8rem; color: #94a3b8; margin-top: 4px; text-transform: uppercase; letter-spacing: 1px; }
    .control-group { background: rgba(15, 23, 42, 0.6); border-radius: 18px; padding: 16px 20px; margin-bottom: 16px; display: flex; justify-content: space-between; align-items: center; border: 1px solid rgba(255, 255, 255, 0.05); }
    .control-info { display: flex; align-items: center; gap: 12px; }
    .control-icon { font-size: 1.5rem; }
    .control-name { font-size: 1rem; font-weight: 600; }
    
    /* Toggle Switch Component */
    .switch { position: relative; display: inline-block; width: 56px; height: 30px; }
    .switch input { opacity: 0; width: 0; height: 0; }
    .slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #475569; transition: .3s; border-radius: 30px; }
    .slider:before { position: absolute; content: ""; height: 22px; width: 22px; left: 4px; bottom: 4px; background-color: white; transition: .3s; border-radius: 50%; }
    input:checked + .slider { background-color: #38bdf8; }
    input:checked + .slider:before { transform: translateX(26px); }

    .status-bar { text-align: center; font-size: 0.8rem; color: #64748b; margin-top: 20px; }
    .status-online { color: #4ade80; font-weight: bold; }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <h1>ESP32 Local Control</h1>
      <p>Điều khiển Wi-Fi Nội bộ - IP: <span id="ip-addr">--</span></p>
    </div>

    <!-- Hàng hiển thị chỉ số cảm biến -->
    <div class="card-grid">
      <div class="card">
        <div class="card-icon">🌡️</div>
        <div class="card-value" id="temp">--</div>
        <div class="card-label">Nhiệt Độ (°C)</div>
      </div>
      <div class="card">
        <div class="card-icon">💧</div>
        <div class="card-value" id="hum">--</div>
        <div class="card-label">Độ Ẩm (%)</div>
      </div>
    </div>

    <!-- Hàng điều khiển thiết bị Relay -->
    <div class="control-group">
      <div class="control-info">
        <div class="control-icon">💡</div>
        <div class="control-name">Đèn (Relay 1)</div>
      </div>
      <label class="switch">
        <input type="checkbox" id="relay1-switch" onchange="toggleRelay(1, this.checked)">
        <span class="slider"></span>
      </label>
    </div>

    <div class="control-group">
      <div class="control-info">
        <div class="control-icon">🌀</div>
        <div class="control-name">Quạt (Relay 2)</div>
      </div>
      <label class="switch">
        <input type="checkbox" id="relay2-switch" onchange="toggleRelay(2, this.checked)">
        <span class="slider"></span>
      </label>
    </div>

    <div class="status-bar">
      Trạng thái: <span class="status-online">● Online (Local Wi-Fi)</span> | Hạn: <span id="ver">v1.0.0</span>
    </div>
  </div>

  <script>
    // Hàm gửi lệnh điều khiển Relay tới ESP32
    function toggleRelay(relayNum, isChecked) {
      const state = isChecked ? "ON" : "OFF";
      fetch(`/api/relay${relayNum}?state=${state}`)
        .then(res => res.json())
        .then(data => console.log("Relay status updated:", data))
        .catch(err => console.error("Error toggling relay:", err));
    }

    // Hàm cập nhật dữ liệu Cảm biến Realtime mỗi 2 giây
    function fetchSensorData() {
      fetch('/api/data')
        .then(res => res.json())
        .then(data => {
          document.getElementById('temp').innerText = data.temperature.toFixed(1);
          document.getElementById('hum').innerText = data.humidity.toFixed(1);
          document.getElementById('relay1-switch').checked = (data.relay1 === "ON");
          document.getElementById('relay2-switch').checked = (data.relay2 === "ON");
          document.getElementById('ip-addr').innerText = data.ip;
          document.getElementById('ver').innerText = data.version;
        })
        .catch(err => console.error("Error fetching data:", err));
    }

    // Tự động gọi đọc dữ liệu mỗi 2 giây
    setInterval(fetchSensorData, 2000);
    fetchSensorData();
  </script>
</body>
</html>
)rawliteral";

#endif // WEB_DASHBOARD_H
