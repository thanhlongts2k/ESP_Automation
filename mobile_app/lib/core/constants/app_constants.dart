/// Constants & Configuration Defaults for MQTT & App Settings
class AppConstants {
  static const String defaultMqttServer = "broker.emqx.io";
  static const int defaultMqttTcpPort = 1883;
  static const int defaultMqttWsPort = 8083;
  static const String defaultDeviceId = "ESP32_Automation_01";
  static const String defaultLocalIp = "192.168.1.50";

  // Topics
  static String getSensorTopic(String deviceId) => "esp32/$deviceId/sensors";
  static String getStatusTopic(String deviceId) => "esp32/$deviceId/status";
  static String getRelay1Topic(String deviceId) => "esp32/$deviceId/control/relay1";
  static String getRelay2Topic(String deviceId) => "esp32/$deviceId/control/relay2";
}
