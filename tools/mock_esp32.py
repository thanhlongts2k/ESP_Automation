#!/usr/bin/env python3
"""
==============================================================================
 DỰ ÁN: ESP_Automation
 CHỨC NĂNG: SCRIPT GIẢ LẬP ESP32 CLOUD ENTERPRISE TRÊN MÁY TÍNH (PC/LAPTOP)
==============================================================================
 Script này giả lập 100% hành vi của Firmware C++ Modular Enterprise:
  1. Đăng ký LWT (Last Will & Testament) di chúc -> Tự động báo Offline khi crash/mất mạng.
  2. Bắn thông điệp Online + Retained States chuẩn phân cấp Topic esp32/{device_id}/...
  3. Giả lập Nhiệt độ, Độ ẩm DHT22, Độ ẩm đất & RSSI Wi-Fi -> Gửi JSON qua MQTT.
  4. Lắng nghe lệnh Bật/Tắt Relay 1/2 và hỗ trợ tham chiếu biến từ file .env tự động.
==============================================================================
"""

import time
import json
import random
import os
import sys

# Bắt buộc UTF-8 cho Windows Terminal
if sys.platform == "win32":
    try:
        sys.stdout.reconfigure(encoding='utf-8')
    except Exception:
        pass

# Kiểm tra thư viện paho-mqtt
try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("[ERROR] Thư viện 'paho-mqtt' chưa được cài đặt!")
    print("-> Hãy chạy lệnh sau trên Terminal/CMD để cài đặt: pip install paho-mqtt")
    sys.exit(1)

# ============================================================================
# HÀM ĐỌC BẢO MẬT TỪ FILE .ENV (HỖ TRỢ THAM CHIẾU BIẾN TỰ ĐỘNG)
# ============================================================================
def load_env_config():
    config = {}
    base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    env_path = os.path.join(base_dir, ".env")
    if not os.path.exists(env_path):
        env_path = os.path.join(os.getcwd(), ".env")
    if os.path.exists(env_path):
        with open(env_path, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith("#") and "=" in line:
                    key, val = line.split("=", 1)
                    config[key.strip()] = val.strip().strip('"').strip("'")
                    
        # Giải quyết biến tham chiếu (Ví dụ: MQTT_SERVER=MQTT_SERVER1 hoặc ${MQTT_SERVER1})
        for k, v in list(config.items()):
            for ref_k, ref_v in config.items():
                v = v.replace(f"${{{ref_k}}}", ref_v).replace(f"${ref_k}", ref_v)
            if v in config:
                v = config[v]
            config[k] = v

    return config

_env = load_env_config()

# ============================================================================
# CẤU HÌNH THÔNG SỐ GIẢ LẬP (ĐỌC TỪ .ENV HOẶC DÙNG MẶC ĐỊNH)
# ============================================================================
MQTT_SERVER = _env.get("MQTT_SERVER", "broker.emqx.io")
MQTT_PORT = int(_env.get("MQTT_PORT", 1883))
MQTT_USER = _env.get("MQTT_USER", "")
MQTT_PASSWORD = _env.get("MQTT_PASSWORD", "")

DEVICE_ID = _env.get("DEVICE_ID", "ESP32_Automation_01")
FIRMWARE_VERSION = _env.get("FIRMWARE_VERSION", "1.0.0")

# Topic chuẩn phân cấp theo DEVICE_ID
TOPIC_SENSOR_DATA = f"esp32/{DEVICE_ID}/sensors"
TOPIC_STATUS = f"esp32/{DEVICE_ID}/status"
TOPIC_CONTROL_RELAY1 = f"esp32/{DEVICE_ID}/control/relay1"
TOPIC_CONTROL_RELAY2 = f"esp32/{DEVICE_ID}/control/relay2"
TOPIC_OTA_TRIGGER = f"esp32/{DEVICE_ID}/system/ota_trigger"

# Trạng thái giả lập của Relay
relay1_state = "OFF" # Đèn
relay2_state = "OFF" # Quạt
start_time = time.time()

# ============================================================================
# CÁC HÀM XỬ LÝ SỰ KIỆN MQTT (CALLBACKS)
# ============================================================================
def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print("\n==================================================")
        print("✅ [MOCK ESP32 CLOUD] KẾT NỐI THÀNH CÔNG TỚI BROKER!")
        print(f"📡 Server: {MQTT_SERVER}:{MQTT_PORT}")
        print(f"🆔 Device ID: {DEVICE_ID}")
        print(f"📦 Firmware Version: v{FIRMWARE_VERSION}")
        print("==================================================\n")
        
        # 1. Publish trạng thái Online (Retained = True)
        online_payload = json.dumps({
            "status": "online",
            "device_id": DEVICE_ID,
            "version": FIRMWARE_VERSION,
            "timestamp": int(time.time())
        })
        client.publish(TOPIC_STATUS, online_payload, qos=1, retain=True)
        print(f"📢 [LWT STATUS] [{TOPIC_STATUS}] -> ONLINE (Retained=True)")
        
        # 2. Subscribe vào các topic điều khiển
        client.subscribe(TOPIC_CONTROL_RELAY1, qos=1)
        client.subscribe(TOPIC_CONTROL_RELAY2, qos=1)
        client.subscribe(TOPIC_OTA_TRIGGER, qos=1)
        
        print(f"📥 Đã Subscribe Topic Relay 1 (Đèn): {TOPIC_CONTROL_RELAY1}")
        print(f"📥 Đã Subscribe Topic Relay 2 (Quạt): {TOPIC_CONTROL_RELAY2}")
        print(f"📥 Đã Subscribe Topic OTA Trigger:    {TOPIC_OTA_TRIGGER}")
        print("--------------------------------------------------\n")
    else:
        print(f"❌ [MOCK ESP32] Kết nối thất bại! Mã lỗi rc={rc}")

def on_message(client, userdata, msg):
    global relay1_state, relay2_state
    topic = msg.topic
    payload = msg.payload.decode("utf-8")
    
    print(f"\n📩 [NHẬN LỆNH FROM MQTT] Topic: {topic} | Payload: {payload}")
    
    # 1. Điều khiển Relay 1 (Đèn)
    if topic == TOPIC_CONTROL_RELAY1:
        if payload.upper() == "ON":
            relay1_state = "ON"
            print("  💡 ➔ [THỰC THI] ĐÃ BẬT ĐÈN (Relay 1: ON)")
        elif payload.upper() == "OFF":
            relay1_state = "OFF"
            print("  💡 ➔ [THỰC THI] ĐÃ TẮT ĐÈN (Relay 1: OFF)")
            
    # 2. Điều khiển Relay 2 (Quạt)
    elif topic == TOPIC_CONTROL_RELAY2:
        if payload.upper() == "ON":
            relay2_state = "ON"
            print("  🌀 ➔ [THỰC THI] ĐÃ BẬT QUẠT (Relay 2: ON)")
        elif payload.upper() == "OFF":
            relay2_state = "OFF"
            print("  🌀 ➔ [THỰC THI] ĐÃ TẮT QUẠT (Relay 2: OFF)")
            
    # 3. Giả lập OTA Trigger
    elif topic == TOPIC_OTA_TRIGGER:
        print(f"  ☁️ ➔ [THỰC THI OTA] Giả lập nhận lệnh nạp code OTA từ xa: {payload}")

# ============================================================================
# CHƯƠNG TRÌNH CHÍNH (MAIN LOOP)
# ============================================================================
def main():
    print("🚀 Đang khởi chạy Script Giả lập ESP32 Enterprise trên Máy tính...")
    
    # Tương thích với cả paho-mqtt v1 và v2
    try:
        client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=DEVICE_ID)
    except AttributeError:
        client = mqtt.Client(client_id=DEVICE_ID)

    # Đăng ký thông điệp di chúc LWT (Last Will and Testament): Khi crash/mất mạng -> Báo offline
    lwt_payload = json.dumps({"status": "offline", "device_id": DEVICE_ID})
    client.will_set(TOPIC_STATUS, payload=lwt_payload, qos=1, retain=True)

    client.on_connect = on_connect
    client.on_message = on_message
    
    if MQTT_USER and MQTT_PASSWORD:
        client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
        
    if MQTT_PORT == 8883:
        client.tls_set() # Bật SSL nếu dùng port 8883
        
    try:
        client.connect(MQTT_SERVER, MQTT_PORT, keepalive=60)
    except Exception as e:
        print(f"❌ Không thể kết nối tới Broker {MQTT_SERVER}:{MQTT_PORT}. Lỗi: {e}")
        return

    client.loop_start()
    
    print("🔄 Đang bắt đầu vòng lặp định kỳ đọc Cảm biến & Publish dữ liệu...\n")
    
    # Giả lập nhiệt độ độ ẩm ban đầu
    current_temp = 28.5
    current_hum = 65.0
    soil_hum = 55.0
    
    try:
        while True:
            current_temp += random.uniform(-0.5, 0.5)
            current_hum += random.uniform(-0.8, 0.8)
            soil_hum += random.uniform(-0.5, 0.5)
            
            current_temp = round(max(20.0, min(40.0, current_temp)), 1)
            current_hum = round(max(40.0, min(95.0, current_hum)), 1)
            soil_hum = round(max(20.0, min(95.0, soil_hum)), 1)
            
            # Đóng gói JSON Payload Enterprise
            payload = {
                "device_id": DEVICE_ID,
                "temperature": current_temp,
                "humidity": current_hum,
                "soil_humidity": soil_hum,
                "relay1": relay1_state,
                "relay2": relay2_state,
                "relay1_light": relay1_state,
                "relay2_fan": relay2_state,
                "rssi": random.randint(-68, -52), # Giả lập cường độ sóng Wi-Fi dBm
                "ip": "192.168.1.50",
                "version": FIRMWARE_VERSION,
                "uptime_s": int(time.time() - start_time)
            }
            
            json_str = json.dumps(payload)
            print(f"📊 [PUBLISH SENSOR] [{TOPIC_SENSOR_DATA}] ➔ {json_str}")
            client.publish(TOPIC_SENSOR_DATA, json_str)
            
            time.sleep(5) # Đọc và gửi mỗi 5 giây
            
    except KeyboardInterrupt:
        # Khi dừng script -> Publish thông điệp offline
        offline_payload = json.dumps({
            "status": "offline",
            "device_id": DEVICE_ID,
            "timestamp": int(time.time())
        })
        client.publish(TOPIC_STATUS, offline_payload, qos=1, retain=True)
        print(f"\n📢 [LWT STATUS] [{TOPIC_STATUS}] -> OFFLINE (Retained=True)")
        print("🛑 Đã dừng Script Giả Lập ESP32.")
        client.loop_stop()
        client.disconnect()

if __name__ == "__main__":
    main()
