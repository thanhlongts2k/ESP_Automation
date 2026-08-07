import 'package:flutter/material.dart';
import '../services/mqtt_service.dart';
import 'dashboard/dashboard_screen.dart';
import 'history/history_screen.dart';
import 'device_detail/device_detail_screen.dart';
import 'settings/settings_screen.dart';

/// Main Tab Navigator containing BottomNavigationBar & IndexedStack
class MainTabScreen extends StatefulWidget {
  final MqttService mqttService;

  const MainTabScreen({super.key, required this.mqttService});

  @override
  State<MainTabScreen> createState() => _MainTabScreenState();
}

class _MainTabScreenState extends State<MainTabScreen> {
  int _currentIndex = 0;

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: widget.mqttService,
      builder: (context, _) {
        final screens = [
          DashboardScreen(mqttService: widget.mqttService),
          HistoryScreen(mqttService: widget.mqttService),
          DeviceDetailScreen(mqttService: widget.mqttService),
          SettingsScreen(mqttService: widget.mqttService),
        ];

        return Scaffold(
          body: IndexedStack(
            index: _currentIndex,
            children: screens,
          ),
          bottomNavigationBar: BottomNavigationBar(
            currentIndex: _currentIndex,
            onTap: (index) => setState(() => _currentIndex = index),
            items: const [
              BottomNavigationBarItem(
                icon: Icon(Icons.dashboard),
                label: 'Điều khiển',
              ),
              BottomNavigationBarItem(
                icon: Icon(Icons.show_chart),
                label: 'Lịch sử',
              ),
              BottomNavigationBarItem(
                icon: Icon(Icons.memory),
                label: 'Thiết bị',
              ),
              BottomNavigationBarItem(
                icon: Icon(Icons.settings),
                label: 'Cấu hình',
              ),
            ],
          ),
        );
      },
    );
  }
}
