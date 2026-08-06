#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <DHT.h>
#include "config.h"

class SensorManager {
public:
    SensorManager();
    void begin();
    void loop();

    float getTemperature() const { return _temperature; }
    float getHumidity() const { return _humidity; }
    float getSoilHumidity() const { return _soilHumidity; }

private:
    DHT _dht;
    float _temperature;
    float _humidity;
    float _soilHumidity;
    unsigned long _lastReadTime;
    const unsigned long _readInterval = 2000; // Đọc cảm biến mỗi 2s
};

#endif // SENSOR_MANAGER_H
