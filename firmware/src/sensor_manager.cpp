#include "sensor_manager.h"

SensorManager::SensorManager() 
    : _dht(DHT_PIN, DHT_TYPE), _temperature(0.0f), _humidity(0.0f), _soilHumidity(0.0f), _lastReadTime(0) {}

void SensorManager::begin() {
    Serial.println("🌡️ [Sensor] Khởi tạo Cảm biến DHT22...");
    _dht.begin();
    pinMode(SOIL_PIN, INPUT);
}

void SensorManager::loop() {
    unsigned long now = millis();
    if (now - _lastReadTime >= _readInterval) {
        _lastReadTime = now;

        float h = _dht.readHumidity();
        float t = _dht.readTemperature();

        if (!isnan(t)) {
            _temperature = t;
        }
        if (!isnan(h)) {
            _humidity = h;
        }

        // Đọc giá trị Analog Cảm biến độ ẩm đất (nếu có cắm chân SOIL_PIN 34)
        int rawAnalog = analogRead(SOIL_PIN);
        // Chuyển đổi ADC 0-4095 sang tỷ lệ 0-100% (Khô 4095 -> 0%, Ẩm 1500 -> 100%)
        float soilPct = map(rawAnalog, 4095, 1500, 0, 100);
        _soilHumidity = constrain(soilPct, 0.0f, 100.0f);
    }
}
