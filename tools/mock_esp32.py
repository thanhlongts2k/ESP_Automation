#!/usr/bin/env python3
"""
==============================================================================
 DỰ ÁN: ESP_Automation
 CHỨC NĂNG: SCRIPT GIẢ LẬP VÀ XÁC THỰC HOẠT ĐỘNG CỦA ESP32 TRÊN MÁY TÍNH (PC/LAPTOP)
==============================================================================
 Script này chạy trực tiếp trên Python 3 (dùng thư viện paho-mqtt).
 Đóng vai trò là 1 con ESP32 thực tế:
  1. Tự động sinh dữ liệu giả lập Nhiệt độ & Độ ẩm DHT22 & Độ ẩm đất -> Gửi JSON qua MQTT.
  2. Lắng nghe lệnh Bật/Tắt Đèn (Relay 1) & Quạt (Relay 2) và in trạng thái ra màn hình.
  3. Hỗ trợ kết nối cả Broker Public miễn phí (broker.emqx.io:1883) hoặc Nginx SSL (8883).
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

DEVICE_ID = _env.get("DEVICE_ID", "ESP32_Mock_PC_01")
FIRMWARE_VERSION = _env.get("FIRMWARE_VERSION", "1.0.0")

TOPIC_SENSOR_DATA = _env.get("TOPIC_SENSOR_DATA", "esp32/sensors/dht22")
TOPIC_CONTROL_RELAY1 = _env.get("TOPIC_CONTROL_RELAY1", "esp32/control/relay1")
TOPIC_CONTROL_RELAY2 = _env.get("TOPIC_CONTROL_RELAY2", "esp32/control/relay2")
TOPIC_OTA_TRIGGER = _env.get("TOPIC_OTA_TRIGGER", "esp32/system/ota_trigger")

# Trạng thái giả lập của Relay
relay1_state = "OFF" # Đèn
relay2_state = "OFF" # Quạt

# ============================================================================
# CÁC HÀM XỬ LÝ SỰ KIỆN MQTT (CALLBACKS)
# ============================================================================
def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print("\n==================================================")
        print("✅ [MOCK ESP32] ĐÃ KẾT NỐI THÀNH CÔNG TỚI MQTT BROKER!")
        print(f"📡 Server: {MQTT_SERVER}:{MQTT_PORT}")
        print(f"🆔 Device ID: {DEVICE_ID}")
        print(f"📦 Firmware Version: {FIRMWARE_VERSION}")
        print("==================================================\n")
        
        # Subscribe vào các topic điều khiển
        client.subscribe(TOPIC_CONTROL_RELAY1)
        client.subscribe(TOPIC_CONTROL_RELAY2)
        client.subscribe(TOPIC_OTA_TRIGGER)
        
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
    print("🚀 Đang khởi chạy Script Giả lập ESP32 trên Máy tính...")
    
    # Tương thích với cả paho-mqtt v1 và v2
    try:
        client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, client_id=DEVICE_ID)
    except AttributeError:
        client = mqtt.Client(client_id=DEVICE_ID)

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
    use_soil_humidity = False
    
    try:
        while True:
            # Biến động nhiệt độ/độ ẩm ngẫu nhiên nhẹ cho giống thực tế
            current_temp += random.uniform(-1.0, 1.0)
            current_hum += random.uniform(-1.5, 1.5)
            soil_hum += random.uniform(-1.0, 1.0)

            current_temp = round(max(20.0, min(40.0, current_temp)), 1)
            current_hum = round(max(40.0, min(95.0, current_hum)), 1)
            soil_hum = round(max(20.0, min(95.0, soil_hum)), 1)
            
            if use_soil_humidity == False:
                soil_hum = 0
            
            # Đóng gói JSON Payload (Gửi cả soil_humidity)
            payload = {
                "device_id": DEVICE_ID,
                "temperature": current_temp,
                "humidity": current_hum,
                "soil_humidity": soil_hum,
                "relay1": relay1_state,
                "relay2": relay2_state,
                "relay1_light": relay1_state,
                "relay2_fan": relay2_state,
                "version": FIRMWARE_VERSION,
                "timestamp": int(time.time())
            }

            json_str = json.dumps(payload)
            print(f"📊 [PUBLISH SENSOR] [{TOPIC_SENSOR_DATA}] ➔ {json_str}")
            client.publish(TOPIC_SENSOR_DATA, json_str)
            
            time.sleep(5) # Đọc và gửi mỗi 5 giây
            
    except KeyboardInterrupt:
        payload = {
            "device_id": DEVICE_ID,
            "temperature": 0.0,
            "humidity": 0.0,
            "soil_humidity": 0.0,
            "relay1": "OFF",
            "relay2": "OFF",
            "relay1_light": "OFF",
            "relay2_fan": "OFF",
            "version": FIRMWARE_VERSION,
            "timestamp": int(time.time())
        }
        json_str = json.dumps(payload)
        client.publish(TOPIC_SENSOR_DATA, json_str)

        print("\n🛑 Đã dừng Script Giả Lập ESP32.")
        client.loop_stop()
        client.disconnect()

if __name__ == "__main__":
    main()
