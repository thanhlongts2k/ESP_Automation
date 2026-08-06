#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

class WiFiManager {
public:
    WiFiManager();
    void begin();
    void loop();
    bool isConnected() const;
    String getLocalIP() const;
    int getRSSI() const;

private:
    unsigned long _lastConnectAttempt;
    const unsigned long _reconnectInterval = 5000; // Thử lại sau mỗi 5s nếu mất Wi-Fi
};

#endif // WIFI_MANAGER_H
