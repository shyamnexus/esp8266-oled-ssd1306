/**
 * ESP32 SSD1306 OLED WiFi Demo
 * 
 * This example demonstrates advanced ESP32 features including:
 * - WiFi connectivity
 * - HTTP requests
 * - Real-time data display
 * - Multiple display modes
 * 
 * Hardware Connections:
 * - VCC -> 3.3V
 * - GND -> GND
 * - SDA -> GPIO 21
 * - SCL -> GPIO 22
 * 
 * WiFi Configuration:
 * Update the WiFi credentials below for your network.
 * 
 * Author: ESP32 Conversion
 * Date: 2024
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "SSD1306Wire.h"

// Display configuration
SSD1306Wire display(0x3c, 21, 22);  // ADDRESS, SDA, SCL

// WiFi credentials - UPDATE THESE FOR YOUR NETWORK
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Demo variables
int currentMode = 0;
const int MAX_MODES = 4;
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 5000; // 5 seconds per mode

// Weather data (simulated)
struct WeatherData {
  float temperature;
  float humidity;
  String description;
  String city;
} weather;

// System info
struct SystemInfo {
  unsigned long uptime;
  size_t freeHeap;
  uint8_t cpuFreq;
  String chipModel;
} sysInfo;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 WiFi OLED Demo Starting...");
  
  // Initialize display
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  
  // Show startup screen
  displayStartupScreen();
  
  // Initialize WiFi
  initWiFi();
  
  // Initialize system info
  updateSystemInfo();
  
  Serial.println("Setup complete!");
}

void loop() {
  // Update system info
  updateSystemInfo();
  
  // Clear display
  display.clear();
  
  // Show current mode
  switch(currentMode) {
    case 0: showSystemInfo(); break;
    case 1: showWiFiInfo(); break;
    case 2: showWeatherInfo(); break;
    case 3: showNetworkInfo(); break;
  }
  
  // Show mode indicator
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 0, String(currentMode + 1) + "/" + String(MAX_MODES));
  
  // Update display
  display.display();
  
  // Switch modes
  if (millis() - lastUpdate > UPDATE_INTERVAL) {
    currentMode = (currentMode + 1) % MAX_MODES;
    lastUpdate = millis();
    
    // Update weather data when switching to weather mode
    if (currentMode == 2) {
      updateWeatherData();
    }
  }
  
  delay(100);
}

void displayStartupScreen() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 10, "ESP32 WiFi");
  display.drawString(64, 30, "OLED Demo");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 50, "Initializing...");
  display.display();
  delay(2000);
}

void initWiFi() {
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Connecting to WiFi");
  display.drawString(64, 20, ssid);
  display.display();
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
    display.drawString(64, 40, "Attempt " + String(attempts));
    display.display();
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    display.clear();
    display.drawString(64, 20, "WiFi Connected!");
    display.drawString(64, 40, WiFi.localIP().toString());
    display.display();
    delay(2000);
  } else {
    display.clear();
    display.drawString(64, 20, "WiFi Failed!");
    display.drawString(64, 40, "Demo mode only");
    display.display();
    delay(2000);
  }
}

void updateSystemInfo() {
  sysInfo.uptime = millis() / 1000;
  sysInfo.freeHeap = ESP.getFreeHeap();
  sysInfo.cpuFreq = ESP.getCpuFreqMHz();
  sysInfo.chipModel = ESP.getChipModel();
}

void updateWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    // Simulate weather data (in real implementation, you'd call a weather API)
    weather.temperature = 20.0 + (sin(millis() / 10000.0) * 10.0);
    weather.humidity = 50.0 + (cos(millis() / 15000.0) * 20.0);
    weather.description = "Partly Cloudy";
    weather.city = "Demo City";
  } else {
    weather.temperature = 0.0;
    weather.humidity = 0.0;
    weather.description = "No Data";
    weather.city = "Offline";
  }
}

void showSystemInfo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "System Info");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  // Uptime
  unsigned long hours = sysInfo.uptime / 3600;
  unsigned long minutes = (sysInfo.uptime % 3600) / 60;
  unsigned long seconds = sysInfo.uptime % 60;
  display.drawString(0, 20, "Uptime: " + String(hours) + "h " + String(minutes) + "m " + String(seconds) + "s");
  
  // Free heap
  display.drawString(0, 35, "Free Heap: " + String(sysInfo.freeHeap) + " bytes");
  
  // CPU frequency
  display.drawString(0, 50, "CPU: " + String(sysInfo.cpuFreq) + " MHz");
}

void showWiFiInfo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "WiFi Status");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  if (WiFi.status() == WL_CONNECTED) {
    display.drawString(0, 20, "Status: Connected");
    display.drawString(0, 35, "SSID: " + WiFi.SSID());
    display.drawString(0, 50, "IP: " + WiFi.localIP().toString());
  } else {
    display.drawString(0, 20, "Status: Disconnected");
    display.drawString(0, 35, "Check credentials");
    display.drawString(0, 50, "in code");
  }
}

void showWeatherInfo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Weather");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  display.drawString(0, 20, "City: " + weather.city);
  display.drawString(0, 35, "Temp: " + String(weather.temperature, 1) + "°C");
  display.drawString(0, 50, "Humidity: " + String(weather.humidity, 1) + "%");
}

void showNetworkInfo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Network");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  if (WiFi.status() == WL_CONNECTED) {
    display.drawString(0, 20, "Gateway: " + WiFi.gatewayIP().toString());
    display.drawString(0, 35, "DNS: " + WiFi.dnsIP().toString());
    display.drawString(0, 50, "RSSI: " + String(WiFi.RSSI()) + " dBm");
  } else {
    display.drawString(0, 20, "No network");
    display.drawString(0, 35, "connection");
    display.drawString(0, 50, "available");
  }
}