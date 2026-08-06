import 'dart:async';
import 'dart:convert';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

void main() {
  runApp(const EspAutomationApp());
}

class EspAutomationApp extends StatelessWidget {
  const EspAutomationApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ESP32 Automation',
      debugShowCheckedModeBanner: false,
      theme: ThemeData.dark().copyWith(
        scaffoldBackgroundColor: const Color(0xFF0F172A),
        colorScheme: const ColorScheme.dark(
          primary: Color(0xFF38BDF8),
          surface: Color(0xFF1E293B),
        ),
      ),
      home: const DashboardScreen(),
    );
  }
}

class DashboardScreen extends StatefulWidget {
  const DashboardScreen({super.key});

  @override
  State<DashboardScreen> createState() => _DashboardScreenState();
}

class _DashboardScreenState extends State<DashboardScreen> {
  // Cấu hình MQTT & Local IP
  final String mqttServer = "broker.emqx.io"; // Hoặc "api-vending.doanhnghiep.com"
  final int mqttPort = 1883;
  final String localIp = "192.168.1.50"; // Hoặc esp32.local

  final String topicSensor = "esp32/sensors/dht22";
  final String topicRelay1 = "esp32/control/relay1";
  final String topicRelay2 = "esp32/control/relay2";

  MqttServerClient? mqttClient;
  bool isConnected = false;
  bool isLocalMode = false;

  // Data Cảm biến
  double temperature = 0.0;
  double humidity = 0.0;
  bool relay1State = false; // Đèn
  bool relay2State = false; // Quạt

  Timer? _localTimer;

  @override
  void initState() {
    super.initState();
    _initMqtt();
  }

  @override
  void dispose() {
    _localTimer?.cancel();
    mqttClient?.disconnect();
    super.dispose();
  }

  // Khởi tạo kết nối MQTT
  Future<void> _initMqtt() async {
    mqttClient = MqttServerClient(mqttServer, 'flutter_app_${DateTime.now().millisecondsSinceEpoch}');
    mqttClient!.port = mqttPort;
    mqttClient!.logging(on: false);
    mqttClient!.keepAlivePeriod = 20;
    mqttClient!.onDisconnected = _onDisconnected;
    mqttClient!.onConnected = _onConnected;

    final connMess = MqttConnectMessage()
        .withClientIdentifier('flutter_app_${DateTime.now().millisecondsSinceEpoch}')
        .startClean();
    mqttClient!.connectionMessage = connMess;

    try {
      await mqttClient!.connect();
    } catch (e) {
      debugPrint('Lỗi kết nối MQTT: $e');
      _switchToLocalMode();
    }
  }

  void _onConnected() {
    setState(() {
      isConnected = true;
      isLocalMode = false;
    });
    mqttClient!.subscribe(topicSensor, MqttQos.atMostOnce);
    mqttClient!.updates!.listen((List<MqttReceivedMessage<MqttMessage?>>? c) {
      final recMess = c![0].payload as MqttPublishMessage;
      final pt = MqttPublishPayload.bytesToStringAsString(recMess.payload.message);
      
      try {
        final data = jsonDecode(pt);
        setState(() {
          temperature = (data['temperature'] as num).toDouble();
          humidity = (data['humidity'] as num).toDouble();
          relay1State = data['relay1'] == "ON";
          relay2State = data['relay2'] == "ON";
        });
      } catch (e) {
        debugPrint('Lỗi bóc tách JSON MQTT: $e');
      }
    });
  }

  void _onDisconnected() {
    setState(() {
      isConnected = false;
    });
    _switchToLocalMode();
  }

  // Tự chuyển sang chế độ Local Wi-Fi REST API nếu rớt MQTT
  void _switchToLocalMode() {
    setState(() {
      isLocalMode = true;
    });
    _localTimer?.cancel();
    _localTimer = Timer.periodic(const Duration(seconds: 2), (timer) async {
      try {
        final response = await http.get(Uri.parse('http://$localIp/api/data')).timeout(const Duration(seconds: 2));
        if (response.statusCode == 200) {
          final data = jsonDecode(response.body);
          setState(() {
            temperature = (data['temperature'] as num).toDouble();
            humidity = (data['humidity'] as num).toDouble();
            relay1State = data['relay1'] == "ON";
            relay2State = data['relay2'] == "ON";
            isConnected = true;
          });
        }
      } catch (_) {
        setState(() {
          isConnected = false;
        });
      }
    });
  }

  // Điều khiển Relay (Đèn / Quạt)
  Future<void> _toggleRelay(int relayNum, bool value) async {
    final stateStr = value ? "ON" : "OFF";

    // 1. Gửi qua MQTT (nếu đang online)
    if (!isLocalMode && isConnected && mqttClient != null) {
      final builder = MqttClientPayloadBuilder();
      builder.addString(stateStr);
      final topic = relayNum == 1 ? topicRelay1 : topicRelay2;
      mqttClient!.publishMessage(topic, MqttQos.atMostOnce, builder.payload!);
    }

    // 2. Gửi qua HTTP REST API Local (nếu ở chế độ Local)
    else {
      try {
        await http.get(Uri.parse('http://$localIp/api/relay$relayNum?state=$stateStr'));
      } catch (e) {
        debugPrint('Lỗi gửi HTTP Local: $e');
      }
    }

    setState(() {
      if (relayNum == 1) relay1State = value;
      if (relayNum == 2) relay2State = value;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('ESP32 Automation'),
        centerTitle: true,
        elevation: 0,
        backgroundColor: Colors.transparent,
        actions: [
          IconButton(
            icon: Icon(Icons.refresh, color: isConnected ? Colors.greenAccent : Colors.redAccent),
            onPressed: _initMqtt,
          )
        ],
      ),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(20),
        child: Column(
          children: [
            // Trạng thái kết nối Badge
            Container(
              padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
              decoration: BoxDecoration(
                color: isConnected ? Colors.green.withOpacity(0.15) : Colors.red.withOpacity(0.15),
                borderRadius: BorderRadius.circular(20),
                border: Border.all(color: isConnected ? Colors.green : Colors.red),
              ),
              child: Row(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Icon(Icons.circle, size: 10, color: isConnected ? Colors.green : Colors.red),
                  const SizedBox(width: 8),
                  Text(
                    isConnected
                        ? (isLocalMode ? 'Local Wi-Fi Mode (192.168.1.50)' : 'MQTT Online ($mqttServer)')
                        : 'Mất kết nối',
                    style: TextStyle(
                      color: isConnected ? Colors.greenAccent : Colors.redAccent,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                ],
              ),
            ),
            const SizedBox(height: 24),

            // Hàng hiển thị Cảm biến
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
                const SizedBox(width: 16),
                Expanded(
                  child: _buildSensorCard(
                    title: 'ĐỘ ẨM',
                    value: '${humidity.toStringAsFixed(1)} %',
                    icon: Icons.water_drop,
                    color: Colors.lightBlueAccent,
                  ),
                ),
              ],
            ),
            const SizedBox(height: 24),

            // Card Điều khiển Relay 1 (Đèn)
            _buildControlCard(
              title: 'Đèn Chiếu Sáng (Relay 1)',
              icon: Icons.lightbulb,
              value: relay1State,
              activeColor: Colors.amber,
              onChanged: (val) => _toggleRelay(1, val),
            ),
            const SizedBox(height: 16),

            // Card Điều khiển Relay 2 (Quạt)
            _buildControlCard(
              title: 'Quạt Thông Gió (Relay 2)',
              icon: Icons.air,
              value: relay2State,
              activeColor: Colors.cyan,
              onChanged: (val) => _toggleRelay(2, val),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildSensorCard({
    required String title,
    required String value,
    required IconData icon,
    required Color color,
  }) {
    return Container(
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        color: const Color(0xFF1E293B),
        borderRadius: BorderRadius.circular(20),
        border: Border.all(color: Colors.white.withOpacity(0.05)),
      ),
      child: Column(
        children: [
          Icon(icon, size: 36, color: color),
          const SizedBox(height: 12),
          Text(
            value,
            style: const TextStyle(fontSize: 24, fontWeight: FontWeight.bold, color: Colors.white),
          ),
          const SizedBox(height: 4),
          Text(
            title,
            style: const TextStyle(fontSize: 12, color: Colors.grey, letterSpacing: 1),
          ),
        ],
      ),
    );
  }

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
