import 'package:flutter/material.dart';

/// Glassmorphism & Dark Slate Theme configuration for ESP32 Automation App
class AppTheme {
  static const Color darkSlate = Color(0xFF0F172A);
  static const Color cardSurface = Color(0xFF1E293B);
  static const Color primaryCyan = Color(0xFF38BDF8);
  static const Color accentGreen = Colors.greenAccent;
  static const Color accentOrange = Colors.orangeAccent;
  static const Color accentRed = Colors.redAccent;
  static const Color accentAmber = Colors.amberAccent;

  static ThemeData get darkTheme {
    return ThemeData.dark().copyWith(
      scaffoldBackgroundColor: darkSlate,
      colorScheme: const ColorScheme.dark(
        primary: primaryCyan,
        surface: cardSurface,
      ),
      appBarTheme: const AppBarTheme(
        backgroundColor: Colors.transparent,
        elevation: 0,
        centerTitle: true,
        titleTextStyle: TextStyle(
          color: Colors.white,
          fontSize: 18,
          fontWeight: FontWeight.bold,
        ),
      ),
      bottomNavigationBarTheme: const BottomNavigationBarThemeData(
        backgroundColor: cardSurface,
        selectedItemColor: primaryCyan,
        unselectedItemColor: Colors.grey,
        type: BottomNavigationBarType.fixed,
        elevation: 8,
      ),
    );
  }
}
