/**
 * ESP32 SSD1306 OLED SPI Example
 * 
 * This example demonstrates how to use the SSD1306 OLED display
 * with ESP32 using SPI communication instead of I2C.
 * 
 * Hardware Connections for SPI:
 * - VCC -> 3.3V
 * - GND -> GND
 * - CLK (SCK) -> GPIO 18
 * - MOSI (SDA) -> GPIO 23
 * - RES (RST) -> GPIO 4
 * - DC (Data/Command) -> GPIO 2
 * - CS (Chip Select) -> GPIO 15
 * 
 * Note: Some displays may not have a CS pin (hard-wired to ground)
 * In that case, use -1 for the CS parameter.
 * 
 * Author: ESP32 Conversion
 * Date: 2024
 */

#include <SPI.h>
#include "SSD1306Spi.h"

// Create display object for SPI
// Parameters: RES pin, DC pin, CS pin
SSD1306Spi display(4, 2, 15);  // RES, DC, CS

// If your display doesn't have a CS pin, use:
// SSD1306Spi display(4, 2);  // RES, DC (CS hard-wired to ground)

void setup() {
  // Start serial communication
  Serial.begin(115200);
  Serial.println("ESP32 SSD1306 SPI Demo");
  
  // Initialize SPI (ESP32 handles this automatically)
  // You can also manually configure SPI if needed:
  // SPI.begin(18, 19, 23, 15); // SCK, MISO, MOSI, SS
  
  // Initialize the display
  display.init();
  
  // Optional: Flip the display vertically
  display.flipScreenVertically();
  
  // Set font and alignment
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  
  Serial.println("SPI Display initialized successfully!");
}

void loop() {
  // Clear the display buffer
  display.clear();
  
  // Draw title
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, "ESP32 SPI Demo");
  
  // Draw some information
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 25, "Communication: SPI");
  display.drawString(0, 40, "Speed: Fast!");
  
  // Draw a simple animation
  int centerX = 64;
  int centerY = 50;
  int radius = 8 + (sin(millis() / 500.0) * 3);
  
  display.drawCircle(centerX, centerY, radius);
  
  // Show current time
  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.drawString(128, 55, String(millis() / 1000) + "s");
  
  // Update the display
  display.display();
  
  // Wait a bit
  delay(50);
}