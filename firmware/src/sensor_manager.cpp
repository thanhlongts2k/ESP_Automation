#include "sensor_manager.h"

/**
 * @brief Khởi tạo các thông số cảm biến ban đầu
 */
SensorManager::SensorManager() 
    : _dht(DHT_PIN, DHT_TYPE), _temperature(0.0f), _humidity(0.0f), _soilHumidity(0.0f), _lastReadTime(0) {}

/**
 * @brief Bắt đầu cấu hình phần cứng chân GPIO cảm biến
 */
void SensorManager::begin() {
    Serial.println("🌡️ [Sensor] Khởi tạo Cảm biến DHT22 & Độ ẩm đất Analog...");
    _dht.begin();
    pinMode(SOIL_PIN, INPUT); // Cấu hình chân GPIO 34 dạng Input đọc tín hiệu Analog từ cảm biến đất
}

/**
 * @brief Vòng lặp định kỳ 2 giây đọc tín hiệu cảm biến và lưu vào cache
 */
void SensorManager::loop() {
    unsigned long now = millis();
    if (now - _lastReadTime >= _readInterval) {
        _lastReadTime = now;

        // 1. Đọc chỉ số Nhiệt độ & Độ ẩm từ cảm biến DHT22
        float h = _dht.readHumidity();
        float t = _dht.readTemperature();

        // Chỉ cập nhật giá trị nếu đọc dữ liệu thành công (không bị lỗi isnan - NaN)
        if (!isnan(t)) {
            _temperature = t;
        }
        if (!isnan(h)) {
            _humidity = h;
        }

        // 2. Đọc giá trị Analog Cảm biến độ ẩm đất (nếu có cắm chân SOIL_PIN 34)
        int rawAnalog = analogRead(SOIL_PIN);
        
        // Chuyển đổi tín hiệu ADC 12-bit (0-4095) sang phần trăm độ ẩm 0-100%
        // (Đất khô giá trị ADC cao ~4095 -> 0%, Đất ngập nước giá trị ADC thấp ~1500 -> 100%)
        float soilPct = map(rawAnalog, 4095, 1500, 0, 100);
        _soilHumidity = constrain(soilPct, 0.0f, 100.0f); // Giới hạn giá trị nằm trong khoảng [0%, 100%]
    }
}
