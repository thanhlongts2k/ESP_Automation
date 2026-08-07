import 'package:flutter/material.dart';
import '../../services/mqtt_service.dart';

/// Dashboard Screen: Primary Control & Telemetry Overview
class DashboardScreen extends StatelessWidget {
  final MqttService mqttService;

  const DashboardScreen({super.key, required this.mqttService});

  @override
  Widget build(BuildContext context) {
    final state = mqttService.state;

    return Scaffold(
      appBar: AppBar(
        title: const Text('ESP32 Automation Cloud'),
        actions: [
          IconButton(
            icon: Icon(
              Icons.refresh,
              color: state.isMqttConnected ? Colors.greenAccent : Colors.redAccent,
            ),
            onPressed: mqttService.initMqtt,
          )
        ],
      ),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(20),
        child: Column(
          children: [
            // 1. Connection Status Badge (LWT & Local REST API)
            Container(
              padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
              decoration: BoxDecoration(
                color: state.isDeviceOnline
                    ? Colors.green.withOpacity(0.15)
                    : Colors.red.withOpacity(0.15),
                borderRadius: BorderRadius.circular(20),
                border: Border.all(
                  color: state.isDeviceOnline ? Colors.green : Colors.red,
                ),
              ),
              child: Row(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Icon(
                    Icons.circle,
                    size: 10,
                    color: state.isDeviceOnline ? Colors.green : Colors.red,
                  ),
                  const SizedBox(width: 8),
                  Text(
                    state.isDeviceOnline
                        ? (state.isLocalMode
                            ? 'ESP32 Online (Local API: ${mqttService.localIp})'
                            : 'ESP32 Online (MQTT Cloud)')
                        : (state.isLocalMode
                            ? 'Không tìm thấy ESP32 (Local API)'
                            : 'Mất kết nối MQTT'),
                    style: TextStyle(
                      color: state.isDeviceOnline
                          ? Colors.greenAccent
                          : Colors.redAccent,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                ],
              ),
            ),
            const SizedBox(height: 16),

            // 2. Wi-Fi RSSI & Uptime Summary
            if (state.isDeviceOnline)
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  const Icon(Icons.wifi, size: 16, color: Colors.cyanAccent),
                  const SizedBox(width: 4),
                  Text(
                    'Sóng: ${state.rssi} dBm',
                    style: const TextStyle(fontSize: 12, color: Colors.cyanAccent),
                  ),
                  const SizedBox(width: 16),
                  const Icon(Icons.timer, size: 16, color: Colors.amberAccent),
                  const SizedBox(width: 4),
                  Text(
                    'Uptime: ${mqttService.formatUptime(state.uptimeSeconds)}',
                    style: const TextStyle(fontSize: 12, color: Colors.amberAccent),
                  ),
                ],
              ),
            const SizedBox(height: 20),

            // 3. Sensor Cards Grid
            Row(
              children: [
                Expanded(
                  child: _buildSensorCard(
                    title: 'NHIỆT ĐỘ',
                    value: '${state.temperature.toStringAsFixed(1)} °C',
                    icon: Icons.thermostat,
                    color: Colors.orangeAccent,
                  ),
                ),
                const SizedBox(width: 8),
                Expanded(
                  child: _buildSensorCard(
                    title: 'ĐỘ ẨM',
                    value: '${state.humidity.toStringAsFixed(1)} %',
                    icon: Icons.water_drop,
                    color: Colors.lightBlueAccent,
                  ),
                ),
                const SizedBox(width: 8),
                Expanded(
                  child: _buildSensorCard(
                    title: 'ĐỘ ẨM ĐẤT',
                    value: '${state.soilHumidity.toStringAsFixed(1)} %',
                    icon: Icons.eco,
                    color: Colors.brown,
                  ),
                ),
              ],
            ),
            const SizedBox(height: 24),

            // 4. Relay 1 Control (Light)
            _buildControlCard(
              title: 'Đèn Chiếu Sáng (Relay 1)',
              icon: Icons.lightbulb,
              value: state.relay1State,
              activeColor: Colors.amber,
              onChanged: (val) => mqttService.toggleRelay(1, val),
            ),
            const SizedBox(height: 16),

            // 5. Relay 2 Control (Fan)
            _buildControlCard(
              title: 'Quạt Thông Gió (Relay 2)',
              icon: Icons.air,
              value: state.relay2State,
              activeColor: Colors.cyan,
              onChanged: (val) => mqttService.toggleRelay(2, val),
            ),

            const SizedBox(height: 20),
            Text(
              'Firmware Enterprise: v${state.version} | ID: ${mqttService.deviceId}',
              style: const TextStyle(fontSize: 11, color: Colors.grey, letterSpacing: 0.5),
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
