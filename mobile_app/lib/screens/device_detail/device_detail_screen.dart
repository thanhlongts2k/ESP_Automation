import 'package:flutter/material.dart';
import '../../services/mqtt_service.dart';

/// Device Detail Screen: Hardware, RSSI meter, Uptime & Connection Diagnostics
class DeviceDetailScreen extends StatelessWidget {
  final MqttService mqttService;

  const DeviceDetailScreen({super.key, required this.mqttService});

  @override
  Widget build(BuildContext context) {
    final state = mqttService.state;

    return Scaffold(
      appBar: AppBar(
        title: const Text('Thông Tin Phần Cứng & Kết Nối'),
      ),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(20),
        child: Column(
          children: [
            // Status Card
            Container(
              width: double.infinity,
              padding: const EdgeInsets.all(20),
              decoration: BoxDecoration(
                color: const Color(0xFF1E293B),
                borderRadius: BorderRadius.circular(20),
                border: Border.all(color: Colors.white.withOpacity(0.05)),
              ),
              child: Column(
                children: [
                  const Icon(Icons.memory, size: 48, color: Color(0xFF38BDF8)),
                  const SizedBox(height: 12),
                  Text(
                    mqttService.deviceId,
                    style: const TextStyle(
                      fontSize: 20,
                      fontWeight: FontWeight.bold,
                      color: Colors.white,
                    ),
                  ),
                  const SizedBox(height: 4),
                  Text(
                    'Firmware Version v${state.version}',
                    style: const TextStyle(color: Colors.grey, fontSize: 13),
                  ),
                ],
              ),
            ),
            const SizedBox(height: 20),

            // Diagnostic Tiles
            _buildDetailTile(
              icon: Icons.wifi,
              iconColor: Colors.cyanAccent,
              title: 'Cường độ tín hiệu (RSSI)',
              subtitle: '${state.rssi} dBm',
            ),
            const SizedBox(height: 12),
            _buildDetailTile(
              icon: Icons.timer,
              iconColor: Colors.amberAccent,
              title: 'Thời gian hoạt động (Uptime)',
              subtitle: mqttService.formatUptime(state.uptimeSeconds),
            ),
            const SizedBox(height: 12),
            _buildDetailTile(
              icon: Icons.hub,
              iconColor: Colors.greenAccent,
              title: 'Giao thức kết nối',
              subtitle: state.activeProtocol,
            ),
            const SizedBox(height: 12),
            _buildDetailTile(
              icon: Icons.router,
              iconColor: Colors.purpleAccent,
              title: 'Địa chỉ Local REST API IP',
              subtitle: 'http://${mqttService.localIp}',
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildDetailTile({
    required IconData icon,
    required Color iconColor,
    required String title,
    required String subtitle,
  }) {
    return Container(
      padding: const EdgeInsets.all(16),
      decoration: BoxDecoration(
        color: const Color(0xFF1E293B),
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: Colors.white.withOpacity(0.05)),
      ),
      child: Row(
        children: [
          Container(
            padding: const EdgeInsets.all(10),
            decoration: BoxDecoration(
              color: iconColor.withOpacity(0.15),
              borderRadius: BorderRadius.circular(12),
            ),
            child: Icon(icon, color: iconColor, size: 24),
          ),
          const SizedBox(width: 16),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  title,
                  style: const TextStyle(color: Colors.grey, fontSize: 12),
                ),
                const SizedBox(height: 2),
                Text(
                  subtitle,
                  style: const TextStyle(
                    color: Colors.white,
                    fontSize: 15,
                    fontWeight: FontWeight.bold,
                  ),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
