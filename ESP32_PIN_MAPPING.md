# ESP32 Pin Mapping Guide for SSD1306 OLED Displays

This guide provides comprehensive pin mapping information for using SSD1306 OLED displays with ESP32 microcontrollers.

## ESP32 Pin Overview

### GPIO Pins Available for I2C
| GPIO | Function | Notes |
|------|----------|-------|
| 21 | SDA (default) | Primary I2C data pin |
| 22 | SCL (default) | Primary I2C clock pin |
| 4 | SDA (alt) | Alternative I2C data pin |
| 5 | SCL (alt) | Alternative I2C clock pin |
| 16 | SDA (alt) | Alternative I2C data pin |
| 17 | SCL (alt) | Alternative I2C clock pin |
| 18 | SDA (alt) | Alternative I2C data pin |
| 19 | SCL (alt) | Alternative I2C clock pin |

### GPIO Pins Available for SPI
| GPIO | Function | Notes |
|------|----------|-------|
| 18 | SCK | SPI Clock |
| 19 | MISO | SPI MISO (not used for display) |
| 23 | MOSI | SPI Data Out |
| 5 | CS | Chip Select (configurable) |
| 4 | RST | Reset (configurable) |
| 2 | DC | Data/Command (configurable) |

## Popular ESP32 Development Boards

### ESP32 DevKit V1
```
I2C Pins: 21 (SDA), 22 (SCL)
SPI Pins: 18 (SCK), 23 (MOSI), 19 (MISO), 5 (CS)
```

### ESP32-WROOM-32
```
I2C Pins: 21 (SDA), 22 (SCL)
SPI Pins: 18 (SCK), 23 (MOSI), 19 (MISO), 5 (CS)
```

### ESP32-S3
```
I2C Pins: 21 (SDA), 22 (SCL)
SPI Pins: 18 (SCK), 23 (MOSI), 19 (MISO), 5 (CS)
```

### ESP32-C3
```
I2C Pins: 4 (SDA), 5 (SCL)
SPI Pins: 2 (SCK), 3 (MOSI), 4 (MISO), 5 (CS)
```

## Wiring Diagrams

### I2C Connection (Recommended)
```
ESP32          SSD1306 Display
-----          --------------
3.3V    -----> VCC
GND     -----> GND
GPIO 21 -----> SDA
GPIO 22 -----> SCL
```

### SPI Connection
```
ESP32          SSD1306 Display
-----          --------------
3.3V    -----> VCC
GND     -----> GND
GPIO 18 -----> CLK (SCK)
GPIO 23 -----> MOSI
GPIO 4  -----> RES (RST)
GPIO 2  -----> DC
GPIO 15 -----> CS
```

## Code Examples for Different Pin Configurations

### Standard I2C (GPIO 21, 22)
```cpp
#include <Wire.h>
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, 21, 22);
```

### Alternative I2C Pins (GPIO 4, 5)
```cpp
#include <Wire.h>
#include "SSD1306Wire.h"

SSD1306Wire display(0x3c, 4, 5);
```

### Custom I2C Pins
```cpp
#include <Wire.h>
#include "SSD1306Wire.h"

// Use any available GPIO pins
SSD1306Wire display(0x3c, 16, 17);
```

### SPI with Custom Pins
```cpp
#include <SPI.h>
#include "SSD1306Spi.h"

// RES, DC, CS pins
SSD1306Spi display(4, 2, 15);
```

## ESP32-Specific Considerations

### 1. Pull-up Resistors
- I2C requires pull-up resistors (4.7kΩ recommended)
- Some ESP32 boards have built-in pull-ups
- External pull-ups may be needed for reliable communication

### 2. Voltage Levels
- ESP32 GPIO pins are 3.3V
- SSD1306 displays work with 3.3V
- No level shifting required

### 3. I2C Frequency
- Default: 400kHz (recommended)
- Maximum: 1MHz (for fast communication)
- Lower frequencies for longer wires

### 4. SPI Frequency
- Default: 10MHz
- Can be increased for faster updates
- Lower frequencies for longer wires

## Troubleshooting Pin Issues

### Common Problems

1. **Wrong Pin Numbers**
   - Double-check GPIO numbers
   - Some boards have different pin layouts
   - Use pinout diagrams for your specific board

2. **I2C Communication Errors**
   - Check SDA/SCL connections
   - Verify pull-up resistors
   - Try different GPIO pins

3. **SPI Not Working**
   - Verify all SPI connections
   - Check CS pin (some displays don't use CS)
   - Ensure proper SPI initialization

### Pin Testing Code

```cpp
// Test I2C pins
void testI2CPins(int sda, int scl) {
  Wire.begin(sda, scl);
  Wire.setClock(100000); // 100kHz for testing
  
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("Device found at 0x");
      Serial.println(addr, HEX);
    }
  }
}

// Test specific pins
void setup() {
  Serial.begin(115200);
  testI2CPins(21, 22); // Test default pins
  testI2CPins(4, 5);    // Test alternative pins
}
```

## Board-Specific Pin Configurations

### ESP32 DevKit V1
```cpp
// I2C
SSD1306Wire display(0x3c, 21, 22);

// SPI
SSD1306Spi display(4, 2, 15);
```

### ESP32-S3 DevKit
```cpp
// I2C
SSD1306Wire display(0x3c, 21, 22);

// SPI
SSD1306Spi display(4, 2, 15);
```

### ESP32-C3 DevKit
```cpp
// I2C (different default pins)
SSD1306Wire display(0x3c, 4, 5);

// SPI
SSD1306Spi display(2, 3, 5);
```

## Advanced Pin Configurations

### Multiple Displays
```cpp
// Two displays on different I2C addresses
SSD1306Wire display1(0x3c, 21, 22);
SSD1306Wire display2(0x3d, 21, 22);

// Two displays on different I2C buses
SSD1306WireESP32 display1(0x3c, 21, 22, GEOMETRY_128_64, ESP32_I2C_BUS_0);
SSD1306WireESP32 display2(0x3d, 4, 5, GEOMETRY_128_64, ESP32_I2C_BUS_1);
```

### Custom I2C Configuration
```cpp
// Custom frequency and pins
SSD1306WireESP32 display(0x3c, 16, 17, GEOMETRY_128_64, ESP32_I2C_BUS_0, 1000000);
```

## Best Practices

1. **Use Default Pins When Possible**
   - GPIO 21/22 for I2C
   - Standard SPI pins for SPI

2. **Avoid Conflicting Pins**
   - Don't use pins reserved for other functions
   - Check board documentation for pin restrictions

3. **Test Your Configuration**
   - Use I2C scanner to verify connections
   - Test with simple examples first

4. **Consider Wire Length**
   - Shorter wires for higher frequencies
   - Use appropriate pull-up resistors

5. **Power Supply**
   - Use stable 3.3V supply
   - Add decoupling capacitors if needed