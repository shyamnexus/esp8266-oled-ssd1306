/**
 * ESP32 SSD1306 OLED Display Demo
 * 
 * This example demonstrates how to use the SSD1306 OLED display library
 * with ESP32 microcontrollers. It includes examples for both I2C and SPI
 * communication methods.
 * 
 * Hardware Connections:
 * 
 * For I2C (recommended):
 * - VCC -> 3.3V
 * - GND -> GND
 * - SDA -> GPIO 21 (default) or GPIO 4
 * - SCL -> GPIO 22 (default) or GPIO 5
 * 
 * For SPI:
 * - VCC -> 3.3V
 * - GND -> GND
 * - CLK -> GPIO 18 (SCK)
 * - MOSI -> GPIO 23 (MOSI)
 * - RES -> GPIO 4 (RST)
 * - DC -> GPIO 2 (DC)
 * - CS -> GPIO 15 (CS)
 * 
 * Author: ESP32 Conversion
 * Date: 2024
 */

#include <Wire.h>
#include "SSD1306Wire.h"
// Uncomment for SPI version:
// #include <SPI.h>
// #include "SSD1306Spi.h"

// I2C Configuration for ESP32
// Default I2C pins for ESP32: SDA=21, SCL=22
// You can use custom pins: SSD1306Wire display(0x3c, 4, 5); // SDA=4, SCL=5
SSD1306Wire display(0x3c, 21, 22);  // ADDRESS, SDA, SCL

// For SPI configuration (uncomment to use):
// SSD1306Spi display(4, 2, 15);  // RES, DC, CS

// Demo variables
int demoMode = 0;
int counter = 1;
unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 3000; // 3 seconds per demo

// Demo functions
void drawSystemInfo();
void drawWiFiInfo();
void drawSensorData();
void drawProgressDemo();
void drawGraphicsDemo();
void drawTextDemo();

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 SSD1306 OLED Demo Starting...");
  
  // Initialize the display
  display.init();
  
  // Configure display settings
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  // Clear and show initial message
  display.clear();
  display.drawString(0, 0, "ESP32 OLED Demo");
  display.drawString(0, 20, "Initializing...");
  display.display();
  
  delay(2000);
  
  Serial.println("Display initialized successfully!");
}

void loop() {
  // Clear the display
  display.clear();
  
  // Run current demo
  switch(demoMode) {
    case 0: drawSystemInfo(); break;
    case 1: drawWiFiInfo(); break;
    case 2: drawSensorData(); break;
    case 3: drawProgressDemo(); break;
    case 4: drawGraphicsDemo(); break;
    case 5: drawTextDemo(); break;
  }
  
  // Show demo counter and timing
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 54, String(millis() / 1000) + "s");
  
  // Update display
  display.display();
  
  // Switch demo every 3 seconds
  if (millis() - lastUpdate > UPDATE_INTERVAL) {
    demoMode = (demoMode + 1) % 6;
    lastUpdate = millis();
    counter++;
  }
  
  delay(10);
}

void drawSystemInfo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "ESP32 System");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  // CPU frequency
  display.drawString(0, 20, "CPU Freq: " + String(ESP.getCpuFreqMHz()) + " MHz");
  
  // Free heap
  display.drawString(0, 35, "Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  
  // Chip model
  display.drawString(0, 50, "Chip: " + String(ESP.getChipModel()));
}

void drawWiFiInfo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "WiFi Status");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  // WiFi status
  wl_status_t status = WiFi.status();
  String statusText = "Disconnected";
  if (status == WL_CONNECTED) {
    statusText = "Connected";
  } else if (status == WL_CONNECT_FAILED) {
    statusText = "Failed";
  } else if (status == WL_CONNECTION_LOST) {
    statusText = "Lost";
  }
  
  display.drawString(0, 20, "Status: " + statusText);
  
  if (status == WL_CONNECTED) {
    display.drawString(0, 35, "SSID: " + WiFi.SSID());
    display.drawString(0, 50, "IP: " + WiFi.localIP().toString());
  } else {
    display.drawString(0, 35, "Not connected to WiFi");
    display.drawString(0, 50, "Demo mode only");
  }
}

void drawSensorData() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Sensor Data");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  // Simulated sensor data
  float temperature = 25.0 + (sin(millis() / 1000.0) * 5.0);
  float humidity = 60.0 + (cos(millis() / 2000.0) * 10.0);
  
  display.drawString(0, 20, "Temperature:");
  display.drawString(0, 35, String(temperature, 1) + " °C");
  
  display.drawString(0, 50, "Humidity: " + String(humidity, 1) + "%");
}

void drawProgressDemo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Progress Demo");
  
  // Animated progress bar
  int progress = (millis() / 50) % 101;
  display.drawProgressBar(4, 20, 120, 10, progress);
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 35, String(progress) + "% Complete");
  
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 50, "Loading...");
}

void drawGraphicsDemo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Graphics Demo");
  
  // Animated graphics
  int centerX = 64;
  int centerY = 40;
  int radius = 15 + (sin(millis() / 500.0) * 5);
  
  // Draw animated circle
  display.drawCircle(centerX, centerY, radius);
  
  // Draw some lines
  for (int i = 0; i < 8; i++) {
    float angle = (millis() / 100.0) + (i * PI / 4);
    int x = centerX + cos(angle) * radius;
    int y = centerY + sin(angle) * radius;
    display.drawLine(centerX, centerY, x, y);
  }
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 55, "Animated");
}

void drawTextDemo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Text Demo");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  // Different text alignments
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 20, "Left aligned");
  
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 35, "Center aligned");
  
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 50, "Right aligned");
  
  // Font sizes
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 55, "Font: 10pt");
}