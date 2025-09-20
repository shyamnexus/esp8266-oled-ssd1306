/**
 * ESP32 SSD1306 OLED Simple Example
 * 
 * This is a basic example showing how to use the SSD1306 OLED display
 * with ESP32. Perfect for beginners!
 * 
 * Hardware Connections:
 * - VCC -> 3.3V
 * - GND -> GND
 * - SDA -> GPIO 21 (or any available GPIO)
 * - SCL -> GPIO 22 (or any available GPIO)
 * 
 * Author: ESP32 Conversion
 * Date: 2024
 */

#include <Wire.h>
#include "SSD1306Wire.h"

// Create display object
// Parameters: I2C address, SDA pin, SCL pin
SSD1306Wire display(0x3c, 21, 22);

void setup() {
  // Start serial communication
  Serial.begin(115200);
  Serial.println("ESP32 SSD1306 Simple Demo");
  
  // Initialize the display
  display.init();
  
  // Optional: Flip the display vertically
  display.flipScreenVertically();
  
  // Set font and alignment
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  Serial.println("Display initialized successfully!");
}

void loop() {
  // Clear the display buffer
  display.clear();
  
  // Draw some text
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "Hello ESP32!");
  
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 25, "This is a simple");
  display.drawString(0, 40, "OLED display demo");
  
  // Show current time
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 55, String(millis() / 1000) + "s");
  
  // Update the display
  display.display();
  
  // Wait a bit
  delay(100);
}