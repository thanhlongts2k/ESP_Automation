import 'package:flutter/material.dart';
import 'core/theme/app_theme.dart';
import 'services/mqtt_service.dart';
import 'screens/main_tab_screen.dart';

/// ============================================================================
/// DỰ ÁN: ESP32 IoT Automation Mobile App (Multi-Screen Modular Architecture)
/// FRAMEWORK: FLUTTER SDK (CROSS-PLATFORM ANDROID & IOS)
/// CHỨC NĂNG: MULTIPLE SCREENS, SENSOR MONITORING, DUAL FALLBACK MQTT & HTTP
/// ============================================================================

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(const EspAutomationApp());
}

/// Root Application Widget with Theme & Navigation Configuration
class EspAutomationApp extends StatefulWidget {
  const EspAutomationApp({super.key});

  @override
  State<EspAutomationApp> createState() => _EspAutomationAppState();
}

class _EspAutomationAppState extends State<EspAutomationApp> {
  late final MqttService _mqttService;

  @override
  void initState() {
    super.initState();
    _mqttService = MqttService();
  }

  @override
  void dispose() {
    _mqttService.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ESP32 Automation',
      debugShowCheckedModeBanner: false,
      theme: AppTheme.darkTheme,
      home: MainTabScreen(mqttService: _mqttService),
    );
  }
}
