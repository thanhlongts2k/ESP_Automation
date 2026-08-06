#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"

// Forward declaration
class RelayController;

typedef std::function<void(const String& topic, const String& payload)> MQTTCommandCallback;

class MQTTManager {
public:
    MQTTManager();
    void begin(MQTTCommandCallback callback);
    void loop(bool isWifiConnected);

    bool isConnected();
    void publishTelemetry(float temp, float hum, float soilHum, bool relay1, bool relay2, int rssi, const String& ip);
    void publishRelayStatus(int relayNum, bool state);

private:
    WiFiClient _tcpClient;
    WiFiClientSecure _sslClient;
    PubSubClient _mqttClient;
    MQTTCommandCallback _commandCallback;

    unsigned long _lastReconnectAttempt;
    unsigned long _currentBackoffInterval; // Exponential Backoff (2s, 4s, 8s... max 60s)
    unsigned long _lastTelemetryPublish;

    void _connect();
    static void _staticMqttCallback(char* topic, byte* payload, unsigned int length);
};

#endif // MQTT_MANAGER_H
