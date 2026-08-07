import 'package:flutter/material.dart';
import '../../services/mqtt_service.dart';

/// Settings Screen: Customization of MQTT Broker, Device ID, and Local IP
class SettingsScreen extends StatefulWidget {
  final MqttService mqttService;

  const SettingsScreen({super.key, required this.mqttService});

  @override
  State<SettingsScreen> createState() => _SettingsScreenState();
}

class _SettingsScreenState extends State<SettingsScreen> {
  late TextEditingController _serverController;
  late TextEditingController _portController;
  late TextEditingController _deviceIdController;
  late TextEditingController _localIpController;
  late TextEditingController _userController;
  late TextEditingController _passwordController;
  late bool _isSecure;

  @override
  void initState() {
    super.initState();
    _serverController =
        TextEditingController(text: widget.mqttService.mqttServer);
    _portController =
        TextEditingController(text: widget.mqttService.mqttPort.toString());
    _deviceIdController =
        TextEditingController(text: widget.mqttService.deviceId);
    _localIpController =
        TextEditingController(text: widget.mqttService.localIp);
    _userController =
        TextEditingController(text: widget.mqttService.mqttUser);
    _passwordController =
        TextEditingController(text: widget.mqttService.mqttPassword);
    _isSecure = widget.mqttService.isSecure;

    widget.mqttService.addListener(_onMqttServiceChanged);
  }

  void _onMqttServiceChanged() {
    if (!mounted) return;
    if (_serverController.text != widget.mqttService.mqttServer) {
      _serverController.text = widget.mqttService.mqttServer;
    }
    if (_portController.text != widget.mqttService.mqttPort.toString()) {
      _portController.text = widget.mqttService.mqttPort.toString();
    }
    if (_deviceIdController.text != widget.mqttService.deviceId) {
      _deviceIdController.text = widget.mqttService.deviceId;
    }
    if (_localIpController.text != widget.mqttService.localIp) {
      _localIpController.text = widget.mqttService.localIp;
    }
    if (_userController.text != widget.mqttService.mqttUser) {
      _userController.text = widget.mqttService.mqttUser;
    }
    if (_passwordController.text != widget.mqttService.mqttPassword) {
      _passwordController.text = widget.mqttService.mqttPassword;
    }
    if (_isSecure != widget.mqttService.isSecure) {
      setState(() => _isSecure = widget.mqttService.isSecure);
    }
  }

  @override
  void dispose() {
    widget.mqttService.removeListener(_onMqttServiceChanged);
    _serverController.dispose();
    _portController.dispose();
    _deviceIdController.dispose();
    _localIpController.dispose();
    _userController.dispose();
    _passwordController.dispose();
    super.dispose();
  }

  void _saveSettings() {
    final port = int.tryParse(_portController.text) ?? widget.mqttService.mqttPort;

    widget.mqttService.updateConfig(
      newServer: _serverController.text.trim(),
      newPort: port,
      newDeviceId: _deviceIdController.text.trim(),
      newLocalIp: _localIpController.text.trim(),
      newIsSecure: _isSecure,
      newMqttUser: _userController.text.trim(),
      newMqttPassword: _passwordController.text,
    );

    ScaffoldMessenger.of(context).showSnackBar(
      const SnackBar(
        content: Text('Đã lưu và tái kết nối MQTT!'),
        backgroundColor: Colors.green,
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Cấu Hình Hệ Thống'),
      ),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(20),
        child: Column(
          children: [
            _buildTextField(
              controller: _serverController,
              label: 'MQTT Broker Domain/IP',
              icon: Icons.cloud_queue,
            ),
            const SizedBox(height: 16),
            _buildTextField(
              controller: _portController,
              label: 'MQTT TCP Port (Mặc định 1883 hoặc SSL 8883)',
              icon: Icons.numbers,
              keyboardType: TextInputType.number,
            ),
            const SizedBox(height: 16),
            Container(
              padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
              decoration: BoxDecoration(
                color: const Color(0xFF1E293B),
                borderRadius: BorderRadius.circular(16),
                border: Border.all(color: Colors.white.withOpacity(0.05)),
              ),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  const Row(
                    children: [
                      Icon(Icons.lock, color: Color(0xFF38BDF8)),
                      SizedBox(width: 12),
                      Text(
                        'Bật kết nối Bảo mật SSL/TLS',
                        style: TextStyle(color: Colors.white, fontSize: 15),
                      ),
                    ],
                  ),
                  Switch(
                    value: _isSecure,
                    activeTrackColor: const Color(0xFF38BDF8).withOpacity(0.5),
                    activeColor: const Color(0xFF38BDF8),
                    onChanged: (val) => setState(() => _isSecure = val),
                  ),
                ],
              ),
            ),
            const SizedBox(height: 16),
            _buildTextField(
              controller: _userController,
              label: 'MQTT Username (Tùy chọn)',
              icon: Icons.person,
            ),
            const SizedBox(height: 16),
            _buildTextField(
              controller: _passwordController,
              label: 'MQTT Password (Tùy chọn)',
              icon: Icons.key,
              obscureText: true,
            ),
            const SizedBox(height: 16),
            _buildTextField(
              controller: _deviceIdController,
              label: 'Device ID',
              icon: Icons.developer_board,
            ),
            const SizedBox(height: 16),
            _buildTextField(
              controller: _localIpController,
              label: 'Local REST API IP (Fallback)',
              icon: Icons.lan,
            ),
            const SizedBox(height: 30),
            ElevatedButton.icon(
              style: ElevatedButton.styleFrom(
                backgroundColor: const Color(0xFF38BDF8),
                foregroundColor: Colors.black,
                minimumSize: const Size.fromHeight(50),
                shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(16),
                ),
              ),
              onPressed: _saveSettings,
              icon: const Icon(Icons.save),
              label: const Text(
                'Lưu Cấu Hình & Tái Kết Nối',
                style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildTextField({
    required TextEditingController controller,
    required String label,
    required IconData icon,
    TextInputType keyboardType = TextInputType.text,
    bool obscureText = false,
  }) {
    return TextField(
      controller: controller,
      keyboardType: keyboardType,
      obscureText: obscureText,
      style: const TextStyle(color: Colors.white),
      decoration: InputDecoration(
        labelText: label,
        labelStyle: const TextStyle(color: Colors.grey),
        prefixIcon: Icon(icon, color: const Color(0xFF38BDF8)),
        filled: true,
        fillColor: const Color(0xFF1E293B),
        enabledBorder: OutlineInputBorder(
          borderRadius: BorderRadius.circular(16),
          borderSide: BorderSide(color: Colors.white.withOpacity(0.05)),
        ),
        focusedBorder: OutlineInputBorder(
          borderRadius: BorderRadius.circular(16),
          borderSide: const BorderSide(color: Color(0xFF38BDF8)),
        ),
      ),
    );
  }
}
