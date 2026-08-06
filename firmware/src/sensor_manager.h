#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <DHT.h>
#include "config.h"

/**
 * @class SensorManager
 * @brief Lớp quản lý thu thập và cache dữ liệu các cảm biến phần cứng (DHT22 & Độ ẩm đất Analog).
 * 
 * Lớp này thực hiện đọc dữ liệu định kỳ 2 giây/lần theo cơ chế phi bất đồng bộ (Non-blocking),
 * giúp tránh việc gọi thư viện quá nhiều gây treo hệ thống hoặc sai lệch số liệu.
 */
class SensorManager {
public:
    SensorManager();

    /** @brief Khởi tạo cảm biến DHT22 và chân đọc Analog Cảm biến đất */
    void begin();

    /** @brief Vòng lặp đọc dữ liệu cảm biến định kỳ (phi bất đồng bộ) */
    void loop();

    /** @brief Lấy giá trị nhiệt độ (°C) được cache gần nhất */
    float getTemperature() const { return _temperature; }

    /** @brief Lấy giá trị độ ẩm không khí (%) được cache gần nhất */
    float getHumidity() const { return _humidity; }

    /** @brief Lấy giá trị độ ẩm đất (%) được tính toán từ ADC 12-bit */
    float getSoilHumidity() const { return _soilHumidity; }

private:
    DHT _dht;                                       ///< Đối tượng thư viện điều khiển cảm biến DHT22
    float _temperature;                             ///< Cache Nhiệt độ (°C)
    float _humidity;                                ///< Cache Độ ẩm không khí (%)
    float _soilHumidity;                            ///< Cache Độ ẩm đất (%)
    unsigned long _lastReadTime;                    ///< Mốc thời gian lần đọc cảm biến gần nhất
    const unsigned long _readInterval = 2000;       ///< Chu kỳ đọc cảm biến (2000ms = 2 giây)
};

#endif // SENSOR_MANAGER_H
