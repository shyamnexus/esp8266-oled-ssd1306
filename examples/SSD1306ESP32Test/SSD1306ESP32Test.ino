/**
 * ESP32 SSD1306 OLED Test Example
 * 
 * This is a simple test example to verify that the ESP32 conversion
 * is working correctly. It performs basic display operations and
 * shows system information.
 * 
 * Hardware Connections:
 * - VCC -> 3.3V
 * - GND -> GND
 * - SDA -> GPIO 21
 * - SCL -> GPIO 22
 * 
 * Author: ESP32 Conversion
 * Date: 2024
 */

#include <Wire.h>
#include "SSD1306Wire.h"

// Create display object
SSD1306Wire display(0x3c, 21, 22);  // ADDRESS, SDA, SCL

// Test variables
int testStep = 0;
unsigned long lastUpdate = 0;
const unsigned long STEP_INTERVAL = 2000; // 2 seconds per test

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 SSD1306 Test Starting...");
  
  // Initialize display
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  
  Serial.println("Display initialized successfully!");
  Serial.println("Running tests...");
}

void loop() {
  // Clear display
  display.clear();
  
  // Run current test step
  switch(testStep) {
    case 0: testBasicText(); break;
    case 1: testSystemInfo(); break;
    case 2: testGraphics(); break;
    case 3: testAnimation(); break;
    case 4: testComplete(); break;
  }
  
  // Show test step indicator
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 54, String(testStep + 1) + "/5");
  
  // Update display
  display.display();
  
  // Move to next test step
  if (millis() - lastUpdate > STEP_INTERVAL) {
    testStep = (testStep + 1) % 5;
    lastUpdate = millis();
    
    // Print test step to serial
    Serial.println("Test step " + String(testStep + 1) + " completed");
  }
  
  delay(100);
}

void testBasicText() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "ESP32 Test");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 25, "Basic text display");
  display.drawString(0, 40, "working correctly!");
  
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 55, "Step 1/5");
}

void testSystemInfo() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "System Info");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  // Show ESP32 information
  display.drawString(0, 20, "Chip: " + String(ESP.getChipModel()));
  display.drawString(0, 35, "CPU: " + String(ESP.getCpuFreqMHz()) + " MHz");
  display.drawString(0, 50, "Heap: " + String(ESP.getFreeHeap()) + " bytes");
  
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 55, "Step 2/5");
}

void testGraphics() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Graphics Test");
  
  // Draw some basic shapes
  display.drawRect(10, 20, 30, 20);
  display.fillRect(50, 20, 30, 20);
  display.drawCircle(25, 50, 8);
  display.fillCircle(75, 50, 8);
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 55, "Step 3/5");
}

void testAnimation() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Animation");
  
  // Simple animation
  int centerX = 64;
  int centerY = 40;
  int radius = 15 + (sin(millis() / 500.0) * 5);
  
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
  display.drawString(64, 55, "Step 4/5");
}

void testComplete() {
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Test Complete!");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 25, "All tests passed");
  display.drawString(64, 40, "ESP32 conversion");
  display.drawString(64, 55, "working correctly!");
}