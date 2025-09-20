# ESP32 SSD1306 OLED Display Library

This library provides ESP32-optimized support for SSD1306 and SH1106 OLED displays. It's based on the popular ESP8266 OLED SSD1306 library but includes ESP32-specific optimizations and features.

## Features

- **ESP32 Optimized**: Enhanced performance for ESP32 microcontrollers
- **Dual I2C Bus Support**: Use either I2C bus on ESP32
- **Flexible Pin Configuration**: Use any available GPIO pins
- **High-Speed Communication**: Optimized I2C and SPI communication
- **Memory Efficient**: Better memory management for ESP32
- **WiFi Integration**: Examples with WiFi connectivity
- **Multiple Display Sizes**: Support for 128x64, 128x32, 64x48, and 64x32 displays

## Hardware Connections

### I2C Connection (Recommended)

| Display Pin | ESP32 Pin | Notes |
|-------------|-----------|-------|
| VCC | 3.3V | Power supply |
| GND | GND | Ground |
| SDA | GPIO 21 | Data line (configurable) |
| SCL | GPIO 22 | Clock line (configurable) |

### SPI Connection

| Display Pin | ESP32 Pin | Notes |
|-------------|-----------|-------|
| VCC | 3.3V | Power supply |
| GND | GND | Ground |
| CLK (SCK) | GPIO 18 | SPI Clock |
| MOSI (SDA) | GPIO 23 | SPI Data |
| RES (RST) | GPIO 4 | Reset (configurable) |
| DC | GPIO 2 | Data/Command (configurable) |
| CS | GPIO 15 | Chip Select (configurable) |

## ESP32 Pin Mapping

### Default I2C Pins
- **SDA**: GPIO 21 (default)
- **SCL**: GPIO 22 (default)

### Alternative I2C Pins
You can use any available GPIO pins for I2C:
- GPIO 4, 5 (alternative I2C pins)
- GPIO 16, 17 (another alternative)
- Any other available GPIO pins

### SPI Pins
- **MOSI**: GPIO 23
- **MISO**: GPIO 19 (not used for display)
- **SCK**: GPIO 18
- **CS**: Any available GPIO (default: GPIO 15)

## Quick Start

### 1. Basic I2C Example

```cpp
#include <Wire.h>
#include "SSD1306Wire.h"

// Create display object
SSD1306Wire display(0x3c, 21, 22);  // Address, SDA, SCL

void setup() {
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
}

void loop() {
  display.clear();
  display.drawString(0, 0, "Hello ESP32!");
  display.display();
  delay(1000);
}
```

### 2. ESP32-Optimized Example

```cpp
#include <Wire.h>
#include "SSD1306WireESP32.h"

// Use ESP32-optimized version
SSD1306WireESP32 display(0x3c, 21, 22);

void setup() {
  display.init();
  display.flipScreenVertically();
}

void loop() {
  display.clear();
  display.drawString(0, 0, "ESP32 Optimized!");
  display.display();
  delay(1000);
}
```

### 3. SPI Example

```cpp
#include <SPI.h>
#include "SSD1306Spi.h"

// Create SPI display object
SSD1306Spi display(4, 2, 15);  // RES, DC, CS

void setup() {
  display.init();
  display.flipScreenVertically();
}

void loop() {
  display.clear();
  display.drawString(0, 0, "ESP32 SPI!");
  display.display();
  delay(1000);
}
```

## ESP32-Specific Features

### 1. Dual I2C Bus Support

```cpp
// Use I2C Bus 0 (default)
SSD1306WireESP32 display1(0x3c, 21, 22, GEOMETRY_128_64, ESP32_I2C_BUS_0);

// Use I2C Bus 1 (if available)
SSD1306WireESP32 display2(0x3d, 4, 5, GEOMETRY_128_64, ESP32_I2C_BUS_1);
```

### 2. Custom I2C Frequency

```cpp
SSD1306WireESP32 display(0x3c, 21, 22, GEOMETRY_128_64, ESP32_I2C_BUS_0, 1000000);
// Set to 1MHz for faster communication
```

### 3. Connection Status Check

```cpp
if (display.isConnected()) {
  Serial.println("Display connected!");
} else {
  Serial.println("Display not found!");
}
```

## Examples Included

1. **SSD1306ESP32Simple**: Basic example for beginners
2. **SSD1306ESP32Demo**: Comprehensive demo with multiple features
3. **SSD1306ESP32WiFiDemo**: WiFi-enabled example with real-time data
4. **SSD1306ESP32SPI**: SPI communication example

## PlatformIO Configuration

The library includes PlatformIO configurations for various ESP32 boards:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

[env:esp32-s3]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

[env:esp32-c3]
platform = espressif32
board = esp32-c3-devkitm-1
framework = arduino
```

## Performance Optimizations

### 1. I2C Transfer Size
- ESP32 can transfer up to 128 bytes per I2C transaction
- Automatic optimization based on ESP32 capabilities

### 2. Memory Management
- Efficient buffer management for ESP32
- Reduced memory fragmentation
- Better garbage collection

### 3. Task Scheduling
- Proper yield() calls for ESP32 FreeRTOS
- Non-blocking display updates
- Better multitasking support

## Troubleshooting

### Common Issues

1. **Display not showing anything**
   - Check wiring connections
   - Verify I2C address (usually 0x3c or 0x3d)
   - Ensure 3.3V power supply

2. **I2C communication errors**
   - Try different GPIO pins
   - Check for pull-up resistors (4.7kΩ recommended)
   - Verify I2C address with I2C scanner

3. **SPI not working**
   - Check all SPI connections
   - Verify CS pin (some displays don't have CS)
   - Ensure proper SPI initialization

### I2C Scanner Code

```cpp
#include <Wire.h>

void setup() {
  Wire.begin(21, 22);  // SDA, SCL
  Serial.begin(115200);
  Serial.println("I2C Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;
  
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  }
  
  delay(5000);
}
```

## Advanced Usage

### Custom Fonts
Create custom fonts using the online font tool: http://oleddisplay.squix.ch

### Multiple Displays
```cpp
// Two displays on different I2C addresses
SSD1306Wire display1(0x3c, 21, 22);
SSD1306Wire display2(0x3d, 21, 22);

void setup() {
  display1.init();
  display2.init();
}
```

### Display Rotation
```cpp
display.flipScreenVertically();  // Flip vertically
display.mirrorScreen();          // Mirror horizontally
```

## License

This library is based on the original ESP8266 OLED SSD1306 library and maintains the same MIT license.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## Support

For support and questions:
- Check the examples in the `examples/` folder
- Review the troubleshooting section
- Open an issue on the GitHub repository