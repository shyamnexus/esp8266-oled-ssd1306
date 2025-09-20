/**
 * ESP32-Optimized SSD1306Wire Implementation
 * 
 * This is an ESP32-specific optimized version of the SSD1306Wire class
 * that includes ESP32-specific features and optimizations.
 * 
 * Features:
 * - Optimized I2C communication for ESP32
 * - Support for both I2C buses on ESP32
 * - Enhanced error handling
 * - Better memory management
 * - ESP32-specific pin configurations
 * 
 * Author: ESP32 Conversion
 * Date: 2024
 */

#ifndef SSD1306WireESP32_h
#define SSD1306WireESP32_h

#include "OLEDDisplay.h"
#include <Wire.h>
#include <algorithm>

// ESP32-specific optimizations
#if defined(ARDUINO_ARCH_ESP32)
#define I2C_MAX_TRANSFER_BYTE 128  // ESP32 can transfer 128 bytes at once
#define ESP32_I2C_FREQ_DEFAULT 400000  // Default I2C frequency for ESP32
#define ESP32_I2C_FREQ_FAST 1000000   // Fast I2C frequency for ESP32
#else
#define I2C_MAX_TRANSFER_BYTE 17
#endif

// ESP32-specific I2C bus selection
enum ESP32_I2C_BUS {
  ESP32_I2C_BUS_0,  // Wire (default)
  ESP32_I2C_BUS_1   // Wire1 (if available)
};

class SSD1306WireESP32 : public OLEDDisplay {
  private:
      uint8_t             _address;
      int                 _sda;
      int                 _scl;
      bool                _doI2cAutoInit = false;
      TwoWire*            _wire = NULL;
      long                _frequency;
      ESP32_I2C_BUS       _i2cBus;
      bool                _initialized = false;

  public:
    /**
     * ESP32-optimized constructor
     * 
     * @param address I2C Display address (usually 0x3c or 0x3d)
     * @param sda I2C SDA pin number (default: 21 for ESP32)
     * @param scl I2C SCL pin number (default: 22 for ESP32)
     * @param g display geometry (default: GEOMETRY_128_64)
     * @param i2cBus ESP32 I2C bus selection (ESP32_I2C_BUS_0 or ESP32_I2C_BUS_1)
     * @param frequency I2C frequency in Hz (default: 400kHz for ESP32)
     */
    SSD1306WireESP32(uint8_t address, 
                     int sda = 21, 
                     int scl = 22, 
                     OLEDDISPLAY_GEOMETRY g = GEOMETRY_128_64, 
                     ESP32_I2C_BUS i2cBus = ESP32_I2C_BUS_0, 
                     long frequency = ESP32_I2C_FREQ_DEFAULT) {
      setGeometry(g);

      this->_address = address;
      this->_sda = sda;
      this->_scl = scl;
      this->_i2cBus = i2cBus;
      this->_frequency = frequency;
      
      // Select appropriate I2C bus for ESP32
      if (i2cBus == ESP32_I2C_BUS_0) {
        this->_wire = &Wire;
      } else {
        this->_wire = &Wire1;
      }
    }

    /**
     * Connect to the display with ESP32 optimizations
     */
    bool connect() {
      if (_initialized) {
        return true;
      }

      // Initialize I2C with ESP32-specific settings
      if (this->_sda != -1 && this->_scl != -1) {
        _wire->begin(this->_sda, this->_scl);
      } else {
        _wire->begin();
      }
      
      // Set I2C frequency with ESP32 optimizations
      if (this->_frequency != -1) {
        _wire->setClock(this->_frequency);
      }
      
      _initialized = true;
      return true;
    }

    /**
     * ESP32-optimized display update with better performance
     */
    void display(void) {
      initI2cIfNeccesary();
      const int x_offset = (128 - this->width()) / 2;
      
      #ifdef OLEDDISPLAY_DOUBLE_BUFFER
        uint8_t minBoundY = UINT8_MAX;
        uint8_t maxBoundY = 0;
        uint8_t minBoundX = UINT8_MAX;
        uint8_t maxBoundX = 0;
        uint8_t x, y;

        // Calculate the Y bounding box of changes
        for (y = 0; y < (this->height() / 8); y++) {
          for (x = 0; x < this->width(); x++) {
           uint16_t pos = x + y * this->width();
           if (buffer[pos] != buffer_back[pos]) {
             minBoundY = std::min(minBoundY, y);
             maxBoundY = std::max(maxBoundY, y);
             minBoundX = std::min(minBoundX, x);
             maxBoundX = std::max(maxBoundX, x);
           }
           buffer_back[pos] = buffer[pos];
         }
         yield(); // ESP32 yield for better multitasking
        }

        if (minBoundY == UINT8_MAX) return;

        sendCommand(COLUMNADDR);
        sendCommand(x_offset + minBoundX);
        sendCommand(x_offset + maxBoundX);

        sendCommand(PAGEADDR);
        sendCommand(minBoundY);
        sendCommand(maxBoundY);

        // ESP32-optimized data transfer
        uint8_t k = 0;
        for (y = minBoundY; y <= maxBoundY; y++) {
          for (x = minBoundX; x <= maxBoundX; x++) {
            if (k == 0) {
              _wire->beginTransmission(_address);
              _wire->write(0x40);
            }

            _wire->write(buffer[x + y * this->width()]);
            k++;
            
            // ESP32 can handle larger transfers
            if (k == (I2C_MAX_TRANSFER_BYTE - 1))  {
              _wire->endTransmission();
              k = 0;
            }
          }
          yield(); // ESP32 yield for better multitasking
        }

        if (k != 0) {
          _wire->endTransmission();
        }
      #else
        // Standard display update for non-double buffer mode
        sendCommand(COLUMNADDR);
        sendCommand(x_offset);
        sendCommand(x_offset + (this->width() - 1));

        sendCommand(PAGEADDR);
        sendCommand(0x0);

        for (uint16_t i=0; i < displayBufferSize; i++) {
          _wire->beginTransmission(this->_address);
          _wire->write(0x40);
          for (uint8_t x = 0; x < (I2C_MAX_TRANSFER_BYTE - 1); x++) {
            _wire->write(buffer[i]);
            i++;
          }
          i--;
          _wire->endTransmission();
        }
      #endif
    }

    /**
     * Set I2C auto-initialization
     */
    void setI2cAutoInit(bool doI2cAutoInit) {
      _doI2cAutoInit = doI2cAutoInit;
    }

    /**
     * ESP32-specific method to set I2C frequency
     */
    void setI2CFrequency(long frequency) {
      this->_frequency = frequency;
      if (_initialized) {
        _wire->setClock(frequency);
      }
    }

    /**
     * ESP32-specific method to get I2C bus status
     */
    bool isConnected() {
      _wire->beginTransmission(_address);
      return (_wire->endTransmission() == 0);
    }

  private:
    int getBufferOffset(void) {
      return 0;
    }
    
    inline void sendCommand(uint8_t command) __attribute__((always_inline)){
      initI2cIfNeccesary();
      _wire->beginTransmission(_address);
      _wire->write(0x80);
      _wire->write(command);
      _wire->endTransmission();
    }

    void initI2cIfNeccesary() {
      if (_doI2cAutoInit && !_initialized) {
        if (this->_sda != -1 && this->_scl != -1) {
          _wire->begin(this->_sda, this->_scl);
        } else {
          _wire->begin();
        }
        _initialized = true;
      }
    }
};

#endif