import 'package:flutter/material.dart';
import '../../services/mqtt_service.dart';

/// History Screen: Telemetry Logs & Analytics
class HistoryScreen extends StatelessWidget {
  final MqttService mqttService;

  const HistoryScreen({super.key, required this.mqttService});

  @override
  Widget build(BuildContext context) {
    final state = mqttService.state;
    final logs = state.logs;

    return Scaffold(
      appBar: AppBar(
        title: const Text('Lịch Sử Telemetry'),
      ),
      body: logs.isEmpty
          ? const Center(
              child: Text(
                'Chưa có nhật ký dữ liệu cảm biến...',
                style: TextStyle(color: Colors.grey),
              ),
            )
          : ListView.builder(
              padding: const EdgeInsets.all(16),
              itemCount: logs.length,
              itemBuilder: (context, index) {
                final log = logs[index];
                final timeStr =
                    "${log.timestamp.hour.toString().padLeft(2, '0')}:${log.timestamp.minute.toString().padLeft(2, '0')}:${log.timestamp.second.toString().padLeft(2, '0')}";

                return Container(
                  margin: const EdgeInsets.only(bottom: 12),
                  padding: const EdgeInsets.all(14),
                  decoration: BoxDecoration(
                    color: const Color(0xFF1E293B),
                    borderRadius: BorderRadius.circular(14),
                    border: Border.all(color: Colors.white.withValues(alpha: 0.05)),
                  ),
                  child: Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Row(
                        children: [
                          const Icon(Icons.access_time,
                              size: 18, color: Colors.cyanAccent),
                          const SizedBox(width: 8),
                          Text(
                            timeStr,
                            style: const TextStyle(
                              color: Colors.white70,
                              fontWeight: FontWeight.bold,
                            ),
                          ),
                        ],
                      ),
                      Row(
                        children: [
                          _buildLogChip(
                            '${log.temperature.toStringAsFixed(1)}°C',
                            Colors.orangeAccent,
                          ),
                          const SizedBox(width: 6),
                          _buildLogChip(
                            '${log.humidity.toStringAsFixed(1)}%',
                            Colors.lightBlueAccent,
                          ),
                          const SizedBox(width: 6),
                          _buildLogChip(
                            '${log.soilHumidity.toStringAsFixed(1)}%',
                            Colors.brown,
                          ),
                        ],
                      ),
                    ],
                  ),
                );
              },
            ),
    );
  }

  Widget _buildLogChip(String label, Color color) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
      decoration: BoxDecoration(
        color: color.withValues(alpha: 0.15),
        borderRadius: BorderRadius.circular(8),
        border: Border.all(color: color.withValues(alpha: 0.3)),
      ),
      child: Text(
        label,
        style: TextStyle(
          color: color,
          fontSize: 11,
          fontWeight: FontWeight.w600,
        ),
      ),
    );
  }
}
