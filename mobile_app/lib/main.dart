import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

/// ============================================================================
/// DỰ ÁN: ESP32 IoT Automation Mobile App
/// FRAMEWORK: FLUTTER SDK (CROSS-PLATFORM ANDROID & IOS)
/// CHỨC NĂNG: GIÁM SÁT SENSOR REALTIME, LWT STATUS, RSSI, UPTIME & DUAL FALLBACK MQTT
/// ============================================================================

void main() {
  runApp(const EspAutomationApp());
}

/// Widget gốc cấu hình Theme Dark Mode Glassmorphism toàn ứng dụng
class EspAutomationApp extends StatelessWidget {
  const EspAutomationApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ESP32 Automation',
      debugShowCheckedModeBanner: false,
      theme: ThemeData.dark().copyWith(
        scaffoldBackgroundColor: const Color(0xFF0F172A), // Nền xanh đen Dark Slate
        colorScheme: const ColorScheme.dark(
          primary: Color(0xFF38BDF8), // Màu xanh Cyan nổi bật
          surface: Color(0xFF1E293B), // Màu bề mặt Card
        ),
      ),
      home: const DashboardScreen(),
    );
  }
}

/// Màn hình Dashboard điều khiển & hiển thị dữ liệu thời gian thực
class DashboardScreen extends StatefulWidget {
  const DashboardScreen({super.key});

  @override
  State<DashboardScreen> createState() => _DashboardScreenState();
}

class _DashboardScreenState extends State<DashboardScreen> {
  // ==========================================================================
  // CẤU HÌNH THÔNG SỐ MQTT CLOUD & DIỆN RỘNG NỘI BỘ (LOCAL IP)
  // ==========================================================================
  final String mqttServer = "broker.emqx.io";
  final int mqttPort = 1883;
  final String localIp = "192.168.1.50";
  final String deviceId = "ESP32_Automation_01";

  // Cụm Topics phân cấp theo device_id chuẩn Enterprise
  late final String topicSensor = "esp32/$deviceId/sensors";
  late final String topicStatus = "esp32/$deviceId/status";
  late final String topicRelay1 = "esp32/$deviceId/control/relay1";
  late final String topicRelay2 = "esp32/$deviceId/control/relay2";

  MqttServerClient? mqttClient;
  StreamSubscription? _mqttSubscription;

  // Quản lý trạng thái kết nối & LWT (Last Will and Testament)
  bool isMqttConnected = false;
  bool isDeviceOnline = false; // Trạng thái sống/chết của thiết bị ESP32
  bool isLocalMode = false;     // Chế độ dự phòng Local Wi-Fi REST API

  String version = "1.0.0";
  int rssi = -60;              // Cường độ tín hiệu Wi-Fi dBm
  int uptimeSeconds = 0;       // Thời gian ESP32 chạy liên tục (Uptime)

  // Lưu trữ chỉ số Cảm biến & Trạng thái Relay
  double temperature = 0.0;    // Nhiệt độ (°C)
  double humidity = 0.0;       // Độ ẩm không khí (%)
  double soilHumidity = 0.0;   // Độ ẩm đất (%)
  bool relay1State = false;    // Công tắc Đèn Chiếu Sáng (Relay 1)
  bool relay2State = false;    // Công tắc Quạt Thông Gió (Relay 2)

  Timer? _localTimer;

  @override
  void initState() {
    super.initState();
    _initMqtt(); // Khởi tạo kết nối MQTT ngay khi mở App
  }

  @override
  void dispose() {
    _localTimer?.cancel();
    _mqttSubscription?.cancel();
    try {
      mqttClient?.onDisconnected = null;
      mqttClient?.disconnect();
    } catch (_) {}
    super.dispose();
  }

  // ==========================================================================
  // THUẬT TOÁN DUAL FALLBACK MQTT (THỬ TCP 1883 ➔ CHUYỂN WEBSOCKET 8083 NẾU BỊ CHẶN)
  // ==========================================================================
  Future<void> _initMqtt() async {
    _localTimer?.cancel();
    _mqttSubscription?.cancel();

    if (mqttClient != null) {
      mqttClient!.onDisconnected = null;
      mqttClient!.onConnected = null;
      try {
        mqttClient!.disconnect();
      } catch (_) {}
    }

    final clientId = 'flutter_app_${DateTime.now().millisecondsSinceEpoch}';

    // 1. Thử nghiệm kết nối qua giao thức TCP Port 1883 tiêu chuẩn
    mqttClient = MqttServerClient.withPort(mqttServer, clientId, mqttPort);
    mqttClient!.logging(on: false);
    mqttClient!.keepAlivePeriod = 60;
    mqttClient!.autoReconnect = true;
    mqttClient!.onDisconnected = _onDisconnected;
    mqttClient!.onConnected = _onConnected;

    final connMess = MqttConnectMessage()
        .withClientIdentifier(clientId)
        .startClean();
    mqttClient!.connectionMessage = connMess;

    try {
      debugPrint('--> [MQTT] Đang mở kết nối TCP tới $mqttServer:$mqttPort...');
      final status = await mqttClient!.connect();
      if (status?.state != MqttConnectionState.connected) {
        _tryWebSocketConnect(clientId);
      }
    } catch (e) {
      debugPrint('--> [MQTT] TCP 1883 thất bại: $e. Đang tự động chuyển sang WebSocket Port 8083...');
      _tryWebSocketConnect(clientId);
    }
  }

  // Phương thức kết nối dự phòng WebSocket Port 8083 (xuyên qua tường lửa 4G/5G)
  Future<void> _tryWebSocketConnect(String clientId) async {
    try {
      if (mqttClient != null) {
        mqttClient!.onDisconnected = null;
        mqttClient!.onConnected = null;
        try { mqttClient!.disconnect(); } catch (_) {}
      }

      final wsClientId = '${clientId}_ws';
      mqttClient = MqttServerClient.withPort(mqttServer, wsClientId, 8083);
      mqttClient!.useWebSocket = true;
      mqttClient!.websocketProtocols = MqttClientConstants.protocolsSingleDefault;
      mqttClient!.logging(on: false);
      mqttClient!.keepAlivePeriod = 60;
      mqttClient!.autoReconnect = true;
      mqttClient!.onDisconnected = _onDisconnected;
      mqttClient!.onConnected = _onConnected;

      final connMessWS = MqttConnectMessage()
          .withClientIdentifier(wsClientId)
          .startClean();
      mqttClient!.connectionMessage = connMessWS;

      await mqttClient!.connect();
    } catch (eWS) {
      debugPrint('--> [MQTT] WebSocket Port 8083 thất bại: $eWS');
      _onDisconnected();
    }
  }

  // Callback khi kết nối MQTT Broker thành công
  void _onConnected() {
    debugPrint('✅ [MQTT] Đã kết nối thành công tới Broker Cloud!');
    if (!mounted) return;

    setState(() {
      isMqttConnected = true;
      isLocalMode = false;
    });

    // Subscribe nhận dữ liệu từ cụm Topics phân cấp theo Device ID
    mqttClient!.subscribe(topicSensor, MqttQos.atMostOnce);
    mqttClient!.subscribe(topicStatus, MqttQos.atMostOnce);
    mqttClient!.subscribe("esp32/sensors/dht22", MqttQos.atMostOnce); // Dự phòng topic cũ

    _mqttSubscription?.cancel();
    _mqttSubscription = mqttClient!.updates?.listen((List<MqttReceivedMessage<MqttMessage?>>? c) {
      if (c == null || c.isEmpty) return;
      final recMess = c[0].payload as MqttPublishMessage;
      final pt = MqttPublishPayload.bytesToStringAsString(recMess.payload.message);
      final topic = c[0].topic;

      try {
        final data = jsonDecode(pt);
        if (mounted) {
          setState(() {
            // 1. Nhận diện trạng thái LWT (Online / Offline)
            if (topic == topicStatus || data.containsKey('status')) {
              final statusStr = data['status']?.toString().toLowerCase();
              if (statusStr == 'online') {
                isDeviceOnline = true;
              } else if (statusStr == 'offline') {
                isDeviceOnline = false;
              }
            }

            // 2. Nhận diện và cập nhật chỉ số Cảm biến Telemetry
            if (topic == topicSensor || topic == "esp32/sensors/dht22" || data.containsKey('temperature')) {
              isDeviceOnline = true; // Nhận được telemetry chứng tỏ thiết bị đang sống
              version = data['version']?.toString() ?? version;
              temperature = (data['temperature'] as num?)?.toDouble() ?? temperature;
              humidity = (data['humidity'] as num?)?.toDouble() ?? humidity;
              soilHumidity = (data['soil_humidity'] as num?)?.toDouble() ?? 0.0;
              rssi = (data['rssi'] as num?)?.toInt() ?? -60;
              uptimeSeconds = (data['uptime_s'] as num?)?.toInt() ?? uptimeSeconds;

              final r1Val = data['relay1'] ?? data['relay1_light'];
              final r2Val = data['relay2'] ?? data['relay2_fan'];

              if (r1Val != null) relay1State = (r1Val == "ON");
              if (r2Val != null) relay2State = (r2Val == "ON");
            }
            isMqttConnected = true;
          });
        }
      } catch (e) {
        debugPrint('Lỗi bóc tách chuỗi JSON MQTT: $e');
      }
    });
  }

  // Callback khi ngắt kết nối MQTT
  void _onDisconnected() {
    debugPrint('❌ [MQTT] Bị ngắt kết nối!');
    if (mounted) {
      setState(() {
        isMqttConnected = false;
        isDeviceOnline = false;
      });
    }
  }

  // Gửi lệnh Bật/Tắt Relay (Đèn / Quạt) qua MQTT hoặc Local REST API
  Future<void> _toggleRelay(int relayNum, bool value) async {
    final stateStr = value ? "ON" : "OFF";

    if (!isLocalMode && isMqttConnected && mqttClient != null) {
      // 1. Gửi lệnh qua MQTT Cloud
      final builder = MqttClientPayloadBuilder();
      builder.addString(stateStr);
      final topic = relayNum == 1 ? topicRelay1 : topicRelay2;
      mqttClient!.publishMessage(topic, MqttQos.atMostOnce, builder.payload!);
    } else {
      // 2. Gửi lệnh qua REST API Wi-Fi nội bộ
      try {
        await http.get(Uri.parse('http://$localIp/api/relay$relayNum?state=$stateStr'));
      } catch (e) {
        debugPrint('Lỗi gửi HTTP Local API: $e');
      }
    }

    setState(() {
      if (relayNum == 1) relay1State = value;
      if (relayNum == 2) relay2State = value;
    });
  }

  // Định dạng thời gian Uptime giây ➔ Giờ Phút Giây
  String _formatUptime(int seconds) {
    int hrs = seconds ~/ 3600;
    int mins = (seconds % 3600) ~/ 60;
    int secs = seconds % 60;
    return '${hrs}h ${mins}m ${secs}s';
  }

  // ==========================================================================
  // XÂY DỰNG GIAO DIỆN NGƯỜI DÙNG (UI DASHBOARD)
  // ==========================================================================
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('ESP32 Automation Cloud'),
        centerTitle: true,
        elevation: 0,
        backgroundColor: Colors.transparent,
        actions: [
          IconButton(
            icon: Icon(Icons.refresh, color: isMqttConnected ? Colors.greenAccent : Colors.redAccent),
            onPressed: _initMqtt,
          )
        ],
      ),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(20),
        child: Column(
          children: [
            // 1. Trạng thái kết nối Badge LWT
            Container(
              padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
              decoration: BoxDecoration(
                color: isMqttConnected
                    ? (isDeviceOnline ? Colors.green.withOpacity(0.15) : Colors.orange.withOpacity(0.15))
                    : Colors.red.withOpacity(0.15),
                borderRadius: BorderRadius.circular(20),
                border: Border.all(
                  color: isMqttConnected
                      ? (isDeviceOnline ? Colors.green : Colors.orange)
                      : Colors.red,
                ),
              ),
              child: Row(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Icon(
                    Icons.circle,
                    size: 10,
                    color: isMqttConnected
                        ? (isDeviceOnline ? Colors.green : Colors.orange)
                        : Colors.red,
                  ),
                  const SizedBox(width: 8),
                  Text(
                    isMqttConnected
                        ? (isDeviceOnline
                            ? 'ESP32 Online ($deviceId)'
                            : 'ESP32 Offline (LWT Status)')
                        : 'Mất kết nối MQTT',
                    style: TextStyle(
                      color: isMqttConnected
                          ? (isDeviceOnline ? Colors.greenAccent : Colors.orangeAccent)
                          : Colors.redAccent,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                ],
              ),
            ),
            const SizedBox(height: 16),

            // 2. Hiển thị Cường độ sóng Wi-Fi RSSI & Thời gian chạy Uptime
            if (isMqttConnected && isDeviceOnline)
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  const Icon(Icons.wifi, size: 16, color: Colors.cyanAccent),
                  const SizedBox(width: 4),
                  Text(
                    'Sóng: $rssi dBm',
                    style: const TextStyle(fontSize: 12, color: Colors.cyanAccent),
                  ),
                  const SizedBox(width: 16),
                  const Icon(Icons.timer, size: 16, color: Colors.amberAccent),
                  const SizedBox(width: 4),
                  Text(
                    'Uptime: ${_formatUptime(uptimeSeconds)}',
                    style: const TextStyle(fontSize: 12, color: Colors.amberAccent),
                  ),
                ],
              ),
            const SizedBox(height: 20),

            // 3. Cụm Card hiển thị thông số Cảm biến (Nhiệt độ, Độ ẩm, Độ ẩm đất)
            Row(
              children: [
                Expanded(
                  child: _buildSensorCard(
                    title: 'NHIỆT ĐỘ',
                    value: '${temperature.toStringAsFixed(1)} °C',
                    icon: Icons.thermostat,
                    color: Colors.orangeAccent,
                  ),
                ),
                const SizedBox(width: 8),
                Expanded(
                  child: _buildSensorCard(
                    title: 'ĐỘ ẨM',
                    value: '${humidity.toStringAsFixed(1)} %',
                    icon: Icons.water_drop,
                    color: Colors.lightBlueAccent,
                  ),
                ),
                const SizedBox(width: 8),
                Expanded(
                  child: _buildSensorCard(
                    title: 'ĐỘ ẨM ĐẤT',
                    value: '${soilHumidity.toStringAsFixed(1)} %',
                    icon: Icons.eco,
                    color: Colors.brown,
                  ),
                ),
              ],
            ),
            const SizedBox(height: 24),

            // 4. Card Điều khiển Relay 1 (Đèn Chiếu Sáng)
            _buildControlCard(
              title: 'Đèn Chiếu Sáng (Relay 1)',
              icon: Icons.lightbulb,
              value: relay1State,
              activeColor: Colors.amber,
              onChanged: (val) => _toggleRelay(1, val),
            ),
            const SizedBox(height: 16),

            // 5. Card Điều khiển Relay 2 (Quạt Thông Gió)
            _buildControlCard(
              title: 'Quạt Thông Gió (Relay 2)',
              icon: Icons.air,
              value: relay2State,
              activeColor: Colors.cyan,
              onChanged: (val) => _toggleRelay(2, val),
            ),

            const SizedBox(height: 20),
            Text(
              'Firmware Enterprise: v$version | ID: $deviceId',
              style: const TextStyle(fontSize: 11, color: Colors.grey, letterSpacing: 0.5),
            ),
          ],
        ),
      ),
    );
  }

  // Component tạo Card Cảm biến bo tròn góc mờ kính
  Widget _buildSensorCard({
    required String title,
    required String value,
    required IconData icon,
    required Color color,
  }) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 16),
      decoration: BoxDecoration(
        color: const Color(0xFF1E293B),
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: Colors.white.withOpacity(0.05)),
      ),
      child: Column(
        children: [
          Icon(icon, size: 26, color: color),
          const SizedBox(height: 8),
          Text(
            value,
            style: const TextStyle(fontSize: 15, fontWeight: FontWeight.bold, color: Colors.white),
          ),
          const SizedBox(height: 4),
          Text(
            title,
            style: const TextStyle(fontSize: 9, color: Colors.grey, letterSpacing: 0.5),
          ),
        ],
      ),
    );
  }

  // Component tạo Card Điều khiển Công tắc Switch
  Widget _buildControlCard({
    required String title,
    required IconData icon,
    required bool value,
    required Color activeColor,
    required ValueChanged<bool> onChanged,
  }) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 16),
      decoration: BoxDecoration(
        color: const Color(0xFF1E293B),
        borderRadius: BorderRadius.circular(20),
        border: Border.all(color: Colors.white.withOpacity(0.05)),
      ),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Row(
            children: [
              Icon(icon, size: 28, color: value ? activeColor : Colors.grey),
              const SizedBox(width: 16),
              Text(
                title,
                style: const TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white),
              ),
            ],
          ),
          Switch(
            value: value,
            activeTrackColor: activeColor.withOpacity(0.5),
            activeColor: activeColor,
            onChanged: onChanged,
          ),
        ],
      ),
    );
  }
}
