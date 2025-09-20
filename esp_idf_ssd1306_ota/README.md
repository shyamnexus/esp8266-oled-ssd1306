# SSD1306 OTA Demo for ESP-IDF

This project converts the Arduino SSD1306 OTA demo to run with ESP-IDF framework.

## Features

- SSD1306 OLED display support via I2C
- WiFi connectivity
- OTA (Over-The-Air) update functionality
- Progress display on OLED screen

## Hardware Requirements

- ESP32 development board
- SSD1306 OLED display (128x64)
- I2C connections:
  - SDA: GPIO 21 (configurable)
  - SCL: GPIO 22 (configurable)
  - VCC: 3.3V
  - GND: Ground

## Configuration

Before building, you need to configure:

1. **WiFi Credentials**: Edit `main/main.c` and update:
   ```c
   #define WIFI_SSID      "Your_SSID"
   #define WIFI_PASS      "Your_Password"
   ```

2. **OTA Server URL**: Update the OTA server URL:
   ```c
   #define OTA_SERVER_URL "http://your-server.com/firmware.bin"
   ```

3. **Display Pins**: If using different pins, update:
   ```c
   #define SDA_PIN        21
   #define SCL_PIN        22
   ```

## Building and Flashing

1. **Set up ESP-IDF environment**:
   ```bash
   . $HOME/esp/esp-idf/export.sh
   ```

2. **Configure the project**:
   ```bash
   cd esp_idf_ssd1306_ota
   idf.py menuconfig
   ```

3. **Build the project**:
   ```bash
   idf.py build
   ```

4. **Flash the firmware**:
   ```bash
   idf.py -p /dev/ttyUSB0 flash monitor
   ```

## Usage

1. Power on the ESP32
2. The device will connect to your WiFi network
3. The OLED display will show "Ready for OTA: [IP_ADDRESS]"
4. The device will attempt to download and install firmware from the configured OTA server
5. During OTA update, the display will show "OTA Update" and a progress bar
6. After successful update, the device will restart with the new firmware

## OTA Server Setup

You need to host a binary firmware file on an HTTP server. The firmware should be built using the same partition table and target the same hardware.

### Simple HTTP Server Example

```python
# simple_ota_server.py
import http.server
import socketserver

PORT = 8000

class MyHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/firmware.bin':
            self.send_response(200)
            self.send_header('Content-type', 'application/octet-stream')
            self.end_headers()
            with open('firmware.bin', 'rb') as f:
                self.wfile.write(f.read())
        else:
            super().do_GET()

with socketserver.TCPServer(("", PORT), MyHTTPRequestHandler) as httpd:
    print(f"Server running at http://localhost:{PORT}")
    httpd.serve_forever()
```

## Project Structure

```
esp_idf_ssd1306_ota/
├── main/
│   ├── main.c              # Main application code
│   └── CMakeLists.txt      # Main component CMake
├── components/
│   └── ssd1306/
│       ├── ssd1306.h       # SSD1306 driver header
│       ├── ssd1306.c       # SSD1306 driver implementation
│       └── CMakeLists.txt  # SSD1306 component CMake
├── CMakeLists.txt          # Root CMake
├── partitions.csv          # Partition table for OTA
├── sdkconfig.defaults     # Default configuration
└── README.md              # This file
```

## Differences from Arduino Version

- Uses ESP-IDF framework instead of Arduino
- Native ESP32 WiFi and HTTP client APIs
- Custom SSD1306 driver implementation
- FreeRTOS task-based architecture
- ESP-IDF OTA APIs instead of ArduinoOTA

## Troubleshooting

1. **Display not working**: Check I2C connections and pin configuration
2. **WiFi connection failed**: Verify SSID and password
3. **OTA update failed**: Check server URL and network connectivity
4. **Build errors**: Ensure ESP-IDF environment is properly set up

## License

MIT License - see source files for details.