/**
 * ESP32 SSD1306 OLED Advanced Example
 * 
 * This example demonstrates advanced ESP32 features including:
 * - Multiple display configurations
 * - WiFi connectivity with real-time data
 * - Sensor simulation
 * - Advanced graphics
 * - Performance monitoring
 * - Error handling
 * 
 * Hardware Connections:
 * - VCC -> 3.3V
 * - GND -> GND
 * - SDA -> GPIO 21 (or any available GPIO)
 * - SCL -> GPIO 22 (or any available GPIO)
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
// Uncomment for ESP32-optimized version:
// #include "SSD1306WireESP32.h"

// Display configuration
SSD1306Wire display(0x3c, 21, 22);  // ADDRESS, SDA, SCL
// For ESP32-optimized version:
// SSD1306WireESP32 display(0x3c, 21, 22);

// WiFi credentials - UPDATE THESE
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Demo variables
int currentMode = 0;
const int MAX_MODES = 6;
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 4000;

// Performance monitoring
struct PerformanceData {
  unsigned long loopTime;
  unsigned long displayTime;
  size_t freeHeap;
  uint8_t cpuFreq;
  int wifiRSSI;
} perfData;

// System information
struct SystemData {
  unsigned long uptime;
  String chipModel;
  String chipRevision;
  size_t heapSize;
  size_t freeHeap;
  size_t minFreeHeap;
} sysData;

// Weather data (simulated)
struct WeatherData {
  float temperature;
  float humidity;
  float pressure;
  String description;
  String city;
  bool isOnline;
} weatherData;

// Animation variables
float animationPhase = 0;
int progressValue = 0;
bool progressDirection = true;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Advanced OLED Demo Starting...");
  
  // Initialize display
  if (!initializeDisplay()) {
    Serial.println("Display initialization failed!");
    return;
  }
  
  // Show startup sequence
  showStartupSequence();
  
  // Initialize WiFi
  initializeWiFi();
  
  // Initialize system data
  updateSystemData();
  
  // Initialize weather data
  updateWeatherData();
  
  Serial.println("Setup complete!");
}

void loop() {
  unsigned long loopStart = millis();
  
  // Update performance data
  updatePerformanceData();
  
  // Clear display
  display.clear();
  
  // Show current mode
  switch(currentMode) {
    case 0: showSystemInfo(); break;
    case 1: showWiFiInfo(); break;
    case 2: showWeatherInfo(); break;
    case 3: showPerformanceInfo(); break;
    case 4: showAnimationDemo(); break;
    case 5: showGraphicsDemo(); break;
  }
  
  // Show mode indicator and timing
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 0, String(currentMode + 1) + "/" + String(MAX_MODES));
  display.drawString(128, 54, String(millis() / 1000) + "s");
  
  // Update display
  unsigned long displayStart = millis();
  display.display();
  perfData.displayTime = millis() - displayStart;
  
  // Switch modes
  if (millis() - lastUpdate > UPDATE_INTERVAL) {
    currentMode = (currentMode + 1) % MAX_MODES;
    lastUpdate = millis();
    
    // Update data when switching to relevant modes
    if (currentMode == 1) updateWiFiInfo();
    if (currentMode == 2) updateWeatherData();
    if (currentMode == 3) updatePerformanceData();
  }
  
  // Update animation
  updateAnimation();
  
  perfData.loopTime = millis() - loopStart;
  delay(10);
}

bool initializeDisplay() {
  display.init();
  
  // Test display connection
  if (!testDisplayConnection()) {
    return false;
  }
  
  // Configure display
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  return true;
}

bool testDisplayConnection() {
  // Simple display test
  display.clear();
  display.drawString(0, 0, "Display Test");
  display.display();
  delay(100);
  
  // Check if display responds (basic test)
  return true; // In a real implementation, you'd check for I2C response
}

void showStartupSequence() {
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 10, "ESP32 OLED");
  display.drawString(64, 30, "Advanced Demo");
  
  display.setFont(ArialMT_Plain_10);
  display.drawString(64, 50, "Initializing...");
  display.display();
  delay(2000);
  
  // Animated startup
  for (int i = 0; i < 5; i++) {
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 20, "Starting");
    
    // Draw loading dots
    for (int j = 0; j <= i; j++) {
      display.drawString(64 + (j * 10) - 20, 40, ".");
    }
    display.display();
    delay(300);
  }
}

void initializeWiFi() {
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
    display.drawString(64, 40, "Attempt " + String(attempts) + "/20");
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

void updateSystemData() {
  sysData.uptime = millis() / 1000;
  sysData.chipModel = ESP.getChipModel();
  sysData.chipRevision = String(ESP.getChipRevision());
  sysData.heapSize = ESP.getHeapSize();
  sysData.freeHeap = ESP.getFreeHeap();
  sysData.minFreeHeap = ESP.getMinFreeHeap();
}

void updateWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    // Simulate weather data with some variation
    weatherData.temperature = 20.0 + (sin(millis() / 10000.0) * 10.0);
    weatherData.humidity = 50.0 + (cos(millis() / 15000.0) * 20.0);
    weatherData.pressure = 1013.25 + (sin(millis() / 20000.0) * 10.0);
    weatherData.description = "Partly Cloudy";
    weatherData.city = "Demo City";
    weatherData.isOnline = true;
  } else {
    weatherData.temperature = 0.0;
    weatherData.humidity = 0.0;
    weatherData.pressure = 0.0;
    weatherData.description = "No Data";
    weatherData.city = "Offline";
    weatherData.isOnline = false;
  }
}

void updatePerformanceData() {
  perfData.freeHeap = ESP.getFreeHeap();
  perfData.cpuFreq = ESP.getCpuFreqMHz();
  perfData.wifiRSSI = WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : -100;
}

void updateAnimation() {
  animationPhase += 0.1;
  if (animationPhase > 2 * PI) animationPhase = 0;
  
  // Update progress bar
  if (progressDirection) {
    progressValue += 2;
    if (progressValue >= 100) progressDirection = false;
  } else {
    progressValue -= 2;
    if (progressValue <= 0) progressDirection = true;
  }
}

void showSystemInfo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "System Info");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  // Uptime
  unsigned long hours = sysData.uptime / 3600;
  unsigned long minutes = (sysData.uptime % 3600) / 60;
  unsigned long seconds = sysData.uptime % 60;
  display.drawString(0, 20, "Uptime: " + String(hours) + "h " + String(minutes) + "m");
  
  // Chip info
  display.drawString(0, 35, "Chip: " + sysData.chipModel);
  display.drawString(0, 50, "Rev: " + sysData.chipRevision);
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
  
  if (weatherData.isOnline) {
    display.drawString(0, 20, "City: " + weatherData.city);
    display.drawString(0, 35, "Temp: " + String(weatherData.temperature, 1) + "°C");
    display.drawString(0, 50, "Humidity: " + String(weatherData.humidity, 1) + "%");
  } else {
    display.drawString(0, 20, "No network");
    display.drawString(0, 35, "connection");
    display.drawString(0, 50, "available");
  }
}

void showPerformanceInfo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Performance");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  display.drawString(0, 20, "Loop: " + String(perfData.loopTime) + "ms");
  display.drawString(0, 35, "Display: " + String(perfData.displayTime) + "ms");
  display.drawString(0, 50, "Heap: " + String(perfData.freeHeap) + " bytes");
}

void showAnimationDemo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Animation");
  
  // Animated progress bar
  display.drawProgressBar(4, 20, 120, 10, progressValue);
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 35, String(progressValue) + "% Complete");
  
  // Animated circle
  int centerX = 64;
  int centerY = 50;
  int radius = 8 + (sin(animationPhase) * 3);
  display.drawCircle(centerX, centerY, radius);
}

void showGraphicsDemo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Graphics");
  
  // Animated graphics
  int centerX = 64;
  int centerY = 40;
  int radius = 15 + (sin(animationPhase) * 5);
  
  // Draw animated circle
  display.drawCircle(centerX, centerY, radius);
  
  // Draw rotating lines
  for (int i = 0; i < 8; i++) {
    float angle = animationPhase + (i * PI / 4);
    int x = centerX + cos(angle) * radius;
    int y = centerY + sin(angle) * radius;
    display.drawLine(centerX, centerY, x, y);
  }
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 55, "Animated");
}