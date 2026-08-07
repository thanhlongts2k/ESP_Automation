import 'dart:async';
import 'dart:convert';
import 'dart:io';
import 'package:flutter/foundation.dart';
import 'package:http/http.dart' as http;
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:shared_preferences/shared_preferences.dart';
import '../core/constants/app_constants.dart';
import '../models/device_state.dart';

class MqttService extends ChangeNotifier {
  String mqttServer = AppConstants.defaultMqttServer;
  int mqttPort = AppConstants.defaultMqttTcpPort;
  String deviceId = AppConstants.defaultDeviceId;
  String localIp = AppConstants.defaultLocalIp;
  bool isSecure = false;
  String mqttUser = "";
  String mqttPassword = "";

  MqttServerClient? _mqttClient;
  StreamSubscription? _mqttSubscription;
  Timer? _localTimer;

  final DeviceState _state = DeviceState();
  DeviceState get state => _state;

  String get topicSensor => AppConstants.getSensorTopic(deviceId);
  String get topicStatus => AppConstants.getStatusTopic(deviceId);
  String get topicRelay1 => AppConstants.getRelay1Topic(deviceId);
  String get topicRelay2 => AppConstants.getRelay2Topic(deviceId);

  MqttService() {
    _loadAndConnect();
  }

  Future<void> _loadAndConnect() async {
    await loadSavedConfig();
    await initMqtt();
  }

  Future<void> loadSavedConfig() async {
    try {
      final prefs = await SharedPreferences.getInstance();
      mqttServer = prefs.getString('mqttServer') ?? AppConstants.defaultMqttServer;
      mqttPort = prefs.getInt('mqttPort') ?? AppConstants.defaultMqttTcpPort;
      deviceId = prefs.getString('deviceId') ?? AppConstants.defaultDeviceId;
      localIp = prefs.getString('localIp') ?? AppConstants.defaultLocalIp;
      isSecure = prefs.getBool('isSecure') ?? (mqttPort == 8883);
      mqttUser = prefs.getString('mqttUser') ?? "";
      mqttPassword = prefs.getString('mqttPassword') ?? "";
      notifyListeners();
    } catch (e) {
      debugPrint('Error loading saved config: $e');
    }
  }

  Future<void> _saveConfig() async {
    try {
      final prefs = await SharedPreferences.getInstance();
      await prefs.setString('mqttServer', mqttServer);
      await prefs.setInt('mqttPort', mqttPort);
      await prefs.setString('deviceId', deviceId);
      await prefs.setString('localIp', localIp);
      await prefs.setBool('isSecure', isSecure);
      await prefs.setString('mqttUser', mqttUser);
      await prefs.setString('mqttPassword', mqttPassword);
    } catch (e) {
      debugPrint('Error saving config: $e');
    }
  }

  void updateConfig({
    required String newServer,
    required int newPort,
    required String newDeviceId,
    required String newLocalIp,
    bool? newIsSecure,
    String? newMqttUser,
    String? newMqttPassword,
  }) {
    mqttServer = newServer;
    mqttPort = newPort;
    deviceId = newDeviceId;
    localIp = newLocalIp;
    isSecure = newIsSecure ?? (newPort == 8883);
    mqttUser = newMqttUser ?? mqttUser;
    mqttPassword = newMqttPassword ?? mqttPassword;
    _saveConfig();
    notifyListeners();
    initMqtt();
  }

  Future<void> initMqtt() async {
    _localTimer?.cancel();
    _mqttSubscription?.cancel();

    if (mqttServer.trim().isEmpty) {
      debugPrint('--> [MQTT] Server Domain empty. Switching to Local REST API mode...');
      _activateLocalMode();
      return;
    }

    if (_mqttClient != null) {
      _mqttClient!.onDisconnected = null;
      _mqttClient!.onConnected = null;
      try {
        _mqttClient!.disconnect();
      } catch (_) {}
    }

    final clientId = 'flutter_app_${DateTime.now().millisecondsSinceEpoch}';

    _mqttClient = MqttServerClient.withPort(mqttServer, clientId, mqttPort);
    _mqttClient!.logging(on: false);
    _mqttClient!.keepAlivePeriod = 60;
    _mqttClient!.autoReconnect = true;
    _mqttClient!.onDisconnected = _onDisconnected;
    _mqttClient!.onConnected = _onConnected;

    final useSsl = (mqttPort == 8883 || isSecure);
    if (useSsl) {
      _mqttClient!.secure = true;
      _mqttClient!.securityContext = SecurityContext.defaultContext;
      _mqttClient!.onBadCertificate = (dynamic cert) => true; 
      _state.activeProtocol = "TCP SSL/TLS (Port $mqttPort)";
    } else {
      _state.activeProtocol = "TCP (Port $mqttPort)";
    }

    final connMess = MqttConnectMessage().withClientIdentifier(clientId);
    if (mqttUser.isNotEmpty) {
      connMess.authenticateAs(mqttUser, mqttPassword);
    }
    connMess.startClean();
    _mqttClient!.connectionMessage = connMess;

    try {
      debugPrint('--> [MQTT] Connecting TCP (SSL: $useSsl) $mqttServer:$mqttPort...');
      final status = await _mqttClient!.connect();
      if (status?.state != MqttConnectionState.connected) {
        await _tryWebSocketConnect(clientId);
      }
    } catch (e) {
      debugPrint('--> [MQTT] TCP $mqttPort failed: $e. Retrying WebSocket...');
      await _tryWebSocketConnect(clientId);
    }
  }

  Future<void> _tryWebSocketConnect(String clientId) async {
    try {
      if (_mqttClient != null) {
        _mqttClient!.onDisconnected = null;
        _mqttClient!.onConnected = null;
        try {
          _mqttClient!.disconnect();
        } catch (_) {}
      }

      final useSsl = (mqttPort == 8883 || isSecure);
      final wsPort = useSsl ? 8084 : AppConstants.defaultMqttWsPort;
      final wsClientId = '${clientId}_ws';

      _mqttClient = MqttServerClient.withPort(
        mqttServer,
        wsClientId,
        wsPort,
      );
      _mqttClient!.useWebSocket = true;
      _mqttClient!.websocketProtocols =
          MqttClientConstants.protocolsSingleDefault;
      _mqttClient!.logging(on: false);
      _mqttClient!.keepAlivePeriod = 60;
      _mqttClient!.autoReconnect = true;
      _mqttClient!.onDisconnected = _onDisconnected;

      if (useSsl) {
        _mqttClient!.secure = true;
        _mqttClient!.securityContext = SecurityContext.defaultContext;
        _mqttClient!.onBadCertificate = (dynamic cert) => true;
      }

      _mqttClient!.onConnected = () {
        _state.activeProtocol = useSsl
            ? "WebSocket WSS (Port $wsPort)"
            : "WebSocket (Port $wsPort)";
        _onConnected();
      };

      final connMessWS = MqttConnectMessage().withClientIdentifier(wsClientId);
      if (mqttUser.isNotEmpty) {
        connMessWS.authenticateAs(mqttUser, mqttPassword);
      }
      connMessWS.startClean();
      _mqttClient!.connectionMessage = connMessWS;

      await _mqttClient!.connect();
    } catch (eWS) {
      debugPrint('--> [MQTT] WebSocket failed: $eWS. Activating Local REST API mode...');
      _activateLocalMode();
    }
  }

  void _activateLocalMode() {
    _state.isMqttConnected = false;
    _state.isLocalMode = true;
    _state.activeProtocol = "Local REST API ($localIp)";
    notifyListeners();

    _startLocalPolling();
  }

  void _startLocalPolling() {
    _localTimer?.cancel();
    _fetchLocalData();
    _localTimer = Timer.periodic(const Duration(seconds: 2), (_) {
      _fetchLocalData();
    });
  }

  Future<void> _fetchLocalData() async {
    if (localIp.trim().isEmpty) {
      _state.isDeviceOnline = false;
      notifyListeners();
      return;
    }

    final endpoints = ['/api/data', '/api/sensors', '/api/status'];

    for (final endpoint in endpoints) {
      try {
        final response = await http
            .get(Uri.parse('http://$localIp$endpoint'))
            .timeout(const Duration(seconds: 2));

        if (response.statusCode == 200) {
          final data = jsonDecode(response.body);
          _state.isDeviceOnline = true;
          _state.isLocalMode = true;
          _state.activeProtocol = "Local REST API ($localIp)";

          _state.version = data['version']?.toString() ?? _state.version;
          _state.temperature =
              (data['temperature'] as num?)?.toDouble() ?? _state.temperature;
          _state.humidity =
              (data['humidity'] as num?)?.toDouble() ?? _state.humidity;
          _state.soilHumidity =
              (data['soil_humidity'] as num?)?.toDouble() ?? _state.soilHumidity;
          _state.rssi = (data['rssi'] as num?)?.toInt() ?? _state.rssi;
          _state.uptimeSeconds =
              (data['uptime_s'] as num?)?.toInt() ?? _state.uptimeSeconds;

          final r1Val = data['relay1'] ?? data['relay1_light'];
          final r2Val = data['relay2'] ?? data['relay2_fan'];

          if (r1Val != null) {
            _state.relay1State = (r1Val == "ON" || r1Val == true || r1Val == 1);
          }
          if (r2Val != null) {
            _state.relay2State = (r2Val == "ON" || r2Val == true || r2Val == 1);
          }

          _state.addLog(_state.temperature, _state.humidity, _state.soilHumidity);
          notifyListeners();
          return;
        }
      } catch (_) {
        // Try next endpoint
      }
    }

    _state.isDeviceOnline = false;
    notifyListeners();
  }

  void _onConnected() {
    debugPrint('✅ [MQTT] Connected to Broker Cloud!');
    _state.isMqttConnected = true;
    _state.isLocalMode = false;
    if (_state.activeProtocol == "Disconnected") {
      _state.activeProtocol = "TCP (Port $mqttPort)";
    }

    _mqttClient!.subscribe(topicSensor, MqttQos.atMostOnce);
    _mqttClient!.subscribe(topicStatus, MqttQos.atMostOnce);
    _mqttClient!.subscribe("esp32/sensors/dht22", MqttQos.atMostOnce);

    _mqttSubscription?.cancel();
    _mqttSubscription = _mqttClient!.updates?.listen(
      (List<MqttReceivedMessage<MqttMessage?>>? c) {
        if (c == null || c.isEmpty) return;
        final recMess = c[0].payload as MqttPublishMessage;
        final pt = MqttPublishPayload.bytesToStringAsString(
          recMess.payload.message,
        );
        final topic = c[0].topic;

        try {
          final data = jsonDecode(pt);

          // 1. LWT Status
          if (topic == topicStatus || data.containsKey('status')) {
            final statusStr = data['status']?.toString().toLowerCase();
            if (statusStr == 'online') {
              _state.isDeviceOnline = true;
            } else if (statusStr == 'offline') {
              _state.isDeviceOnline = false;
            }
          }

          // 2. Sensor Telemetry
          if (topic == topicSensor ||
              topic == "esp32/sensors/dht22" ||
              data.containsKey('temperature')) {
            _state.isDeviceOnline = true;
            _state.version = data['version']?.toString() ?? _state.version;
            _state.temperature =
                (data['temperature'] as num?)?.toDouble() ?? _state.temperature;
            _state.humidity =
                (data['humidity'] as num?)?.toDouble() ?? _state.humidity;
            _state.soilHumidity =
                (data['soil_humidity'] as num?)?.toDouble() ?? 0.0;
            _state.rssi = (data['rssi'] as num?)?.toInt() ?? -60;
            _state.uptimeSeconds =
                (data['uptime_s'] as num?)?.toInt() ?? _state.uptimeSeconds;

            final r1Val = data['relay1'] ?? data['relay1_light'];
            final r2Val = data['relay2'] ?? data['relay2_fan'];

            if (r1Val != null) _state.relay1State = (r1Val == "ON");
            if (r2Val != null) _state.relay2State = (r2Val == "ON");

            _state.addLog(
              _state.temperature,
              _state.humidity,
              _state.soilHumidity,
            );
          }

          _state.isMqttConnected = true;
          notifyListeners();
        } catch (e) {
          debugPrint('JSON parse error: $e');
        }
      },
    );

    notifyListeners();
  }

  void _onDisconnected() {
    debugPrint('❌ [MQTT] Disconnected!');
    _state.isMqttConnected = false;
    _state.isDeviceOnline = false;
    _state.activeProtocol = "Disconnected";
    notifyListeners();
  }

  Future<void> toggleRelay(int relayNum, bool value) async {
    final stateStr = value ? "ON" : "OFF";

    if (!_state.isLocalMode && _state.isMqttConnected && _mqttClient != null) {
      final builder = MqttClientPayloadBuilder();
      builder.addString(stateStr);
      final topic = relayNum == 1 ? topicRelay1 : topicRelay2;
      _mqttClient!.publishMessage(topic, MqttQos.atMostOnce, builder.payload!);
    } else {
      try {
        await http.get(
          Uri.parse('http://$localIp/api/relay$relayNum?state=$stateStr'),
        );
      } catch (e) {
        debugPrint('HTTP Local API error: $e');
      }
    }

    if (relayNum == 1) _state.relay1State = value;
    if (relayNum == 2) _state.relay2State = value;
    notifyListeners();
  }

  String formatUptime(int seconds) {
    int hrs = seconds ~/ 3600;
    int mins = (seconds % 3600) ~/ 60;
    int secs = seconds % 60;
    return '${hrs}h ${mins}m ${secs}s';
  }

  @override
  void dispose() {
    _localTimer?.cancel();
    _mqttSubscription?.cancel();
    try {
      _mqttClient?.onDisconnected = null;
      _mqttClient?.disconnect();
    } catch (_) {}
    super.dispose();
  }
}
