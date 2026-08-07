/// Represents a log entry for sensor history telemetry
class TelemetryLog {
  final DateTime timestamp;
  final double temperature;
  final double humidity;
  final double soilHumidity;

  TelemetryLog({
    required this.timestamp,
    required this.temperature,
    required this.humidity,
    required this.soilHumidity,
  });
}

/// Holds all real-time device telemetry and connection state
class DeviceState {
  String version;
  int rssi;
  int uptimeSeconds;
  double temperature;
  double humidity;
  double soilHumidity;
  bool relay1State;
  bool relay2State;
  bool isMqttConnected;
  bool isDeviceOnline;
  bool isLocalMode;
  String activeProtocol; // 'TCP 1883' or 'WebSocket 8083' or 'Local HTTP'
  List<TelemetryLog> logs;

  DeviceState({
    this.version = "1.0.0",
    this.rssi = -60,
    this.uptimeSeconds = 0,
    this.temperature = 0.0,
    this.humidity = 0.0,
    this.soilHumidity = 0.0,
    this.relay1State = false,
    this.relay2State = false,
    this.isMqttConnected = false,
    this.isDeviceOnline = false,
    this.isLocalMode = false,
    this.activeProtocol = "Disconnected",
    List<TelemetryLog>? logs,
  }) : logs = logs ?? [];

  void addLog(double temp, double hum, double soil) {
    logs.insert(
      0,
      TelemetryLog(
        timestamp: DateTime.now(),
        temperature: temp,
        humidity: hum,
        soilHumidity: soil,
      ),
    );
    // Keep last 50 logs for performance
    if (logs.length > 50) {
      logs.removeLast();
    }
  }
}
