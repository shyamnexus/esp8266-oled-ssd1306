/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 by ThingPulse, Daniel Eichhorn
 * Copyright (c) 2018 by Fabrice Weinberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ThingPulse invests considerable time and money to develop these open source libraries.
 * Please support us by buying our products (and not the clones) from
 * https://thingpulse.com
 *
 */

#include "ssd1306.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "SSD1306";

// SSD1306 Commands
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D

// Simple font data (5x7 font)
static const uint8_t font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // space
    {0x00, 0x00, 0x5f, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7f, 0x14, 0x7f, 0x14}, // #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1c, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1c, 0x00}, // )
    {0x08, 0x2a, 0x1c, 0x2a, 0x08}, // *
    {0x08, 0x08, 0x3e, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3e, 0x51, 0x49, 0x45, 0x3e}, // 0
    {0x00, 0x42, 0x7f, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4b, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7f, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3c, 0x4a, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1e}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x00, 0x08, 0x14, 0x22, 0x41}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x41, 0x22, 0x14, 0x08, 0x00}, // >
    {0x00, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3e}, // @
    {0x7e, 0x11, 0x11, 0x11, 0x7e}, // A
    {0x7f, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3e, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7f, 0x41, 0x41, 0x22, 0x1c}, // D
    {0x7f, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7f, 0x09, 0x09, 0x01, 0x01}, // F
    {0x3e, 0x41, 0x41, 0x51, 0x32}, // G
    {0x7f, 0x08, 0x08, 0x08, 0x7f}, // H
    {0x00, 0x41, 0x7f, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3f, 0x01}, // J
    {0x7f, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7f, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7f, 0x02, 0x04, 0x02, 0x7f}, // M
    {0x7f, 0x04, 0x08, 0x10, 0x7f}, // N
    {0x3e, 0x41, 0x41, 0x41, 0x3e}, // O
    {0x7f, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3e, 0x41, 0x51, 0x21, 0x5e}, // Q
    {0x7f, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7f, 0x01, 0x01}, // T
    {0x3f, 0x40, 0x40, 0x40, 0x3f}, // U
    {0x1f, 0x20, 0x40, 0x20, 0x1f}, // V
    {0x7f, 0x20, 0x18, 0x20, 0x7f}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x03, 0x04, 0x78, 0x04, 0x03}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
};

static esp_err_t send_command(ssd1306_t *display, uint8_t command) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1306_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x80, true); // Command mode
    i2c_master_write_byte(cmd, command, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(SSD1306_I2C_PORT, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static esp_err_t send_data(ssd1306_t *display, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1306_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x40, true); // Data mode
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(SSD1306_I2C_PORT, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t ssd1306_init(ssd1306_t *display, int sda_pin, int scl_pin) {
    if (!display) {
        return ESP_ERR_INVALID_ARG;
    }

    // Initialize I2C
    display->i2c_config.mode = I2C_MODE_MASTER;
    display->i2c_config.sda_io_num = sda_pin;
    display->i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    display->i2c_config.scl_io_num = scl_pin;
    display->i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    display->i2c_config.master.clk_speed = SSD1306_I2C_FREQ;
    
    esp_err_t ret = i2c_param_config(SSD1306_I2C_PORT, &display->i2c_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C param config failed");
        return ret;
    }
    
    ret = i2c_driver_install(SSD1306_I2C_PORT, display->i2c_config.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed");
        return ret;
    }

    // Initialize display
    send_command(display, SSD1306_DISPLAYOFF);
    send_command(display, SSD1306_SETDISPLAYCLOCKDIV);
    send_command(display, 0x80);
    send_command(display, SSD1306_SETMULTIPLEX);
    send_command(display, SSD1306_HEIGHT - 1);
    send_command(display, SSD1306_SETDISPLAYOFFSET);
    send_command(display, 0x0);
    send_command(display, SSD1306_SETSTARTLINE | 0x0);
    send_command(display, SSD1306_CHARGEPUMP);
    send_command(display, 0x14);
    send_command(display, SSD1306_MEMORYMODE);
    send_command(display, 0x00);
    send_command(display, SSD1306_SEGREMAP | 0x1);
    send_command(display, SSD1306_COMSCANDEC);
    send_command(display, SSD1306_SETCOMPINS);
    send_command(display, 0x12);
    send_command(display, SSD1306_SETCONTRAST);
    send_command(display, 0xCF);
    send_command(display, SSD1306_SETPRECHARGE);
    send_command(display, 0xF1);
    send_command(display, SSD1306_SETVCOMDETECT);
    send_command(display, 0x40);
    send_command(display, SSD1306_DISPLAYALLON_RESUME);
    send_command(display, SSD1306_NORMALDISPLAY);
    send_command(display, SSD1306_DISPLAYON);

    // Initialize display structure
    memset(display->buffer, 0, sizeof(display->buffer));
    display->flipped = false;
    display->contrast = 255;
    display->text_align = TEXT_ALIGN_LEFT;
    display->current_font = FONT_ArialMT_Plain_10;
    display->cursor_x = 0;
    display->cursor_y = 0;

    ESP_LOGI(TAG, "SSD1306 initialized successfully");
    return ESP_OK;
}

esp_err_t ssd1306_clear(ssd1306_t *display) {
    if (!display) {
        return ESP_ERR_INVALID_ARG;
    }
    memset(display->buffer, 0, sizeof(display->buffer));
    return ESP_OK;
}

esp_err_t ssd1306_display(ssd1306_t *display) {
    if (!display) {
        return ESP_ERR_INVALID_ARG;
    }

    send_command(display, SSD1306_COLUMNADDR);
    send_command(display, 0);
    send_command(display, SSD1306_WIDTH - 1);
    send_command(display, SSD1306_PAGEADDR);
    send_command(display, 0);
    send_command(display, (SSD1306_HEIGHT / 8) - 1);

    // Send buffer data
    for (int i = 0; i < sizeof(display->buffer); i += 16) {
        size_t chunk_size = (i + 16 > sizeof(display->buffer)) ? (sizeof(display->buffer) - i) : 16;
        send_data(display, &display->buffer[i], chunk_size);
    }

    return ESP_OK;
}

esp_err_t ssd1306_flip_screen_vertically(ssd1306_t *display, bool flip) {
    if (!display) {
        return ESP_ERR_INVALID_ARG;
    }
    display->flipped = flip;
    return ESP_OK;
}

esp_err_t ssd1306_set_contrast(ssd1306_t *display, uint8_t contrast) {
    if (!display) {
        return ESP_ERR_INVALID_ARG;
    }
    display->contrast = contrast;
    send_command(display, SSD1306_SETCONTRAST);
    send_command(display, contrast);
    return ESP_OK;
}

esp_err_t ssd1306_set_text_alignment(ssd1306_t *display, text_align_t alignment) {
    if (!display) {
        return ESP_ERR_INVALID_ARG;
    }
    display->text_align = alignment;
    return ESP_OK;
}

esp_err_t ssd1306_set_font(ssd1306_t *display, font_type_t font) {
    if (!display) {
        return ESP_ERR_INVALID_ARG;
    }
    display->current_font = font;
    return ESP_OK;
}

esp_err_t ssd1306_draw_string(ssd1306_t *display, int x, int y, const char *text) {
    if (!display || !text) {
        return ESP_ERR_INVALID_ARG;
    }

    int pos_x = x;
    int pos_y = y;
    const char *c = text;

    while (*c) {
        if (*c == '\n') {
            pos_x = x;
            pos_y += 8; // Line height
        } else if (*c >= 32 && *c <= 126) {
            // Draw character
            const uint8_t *char_data = font5x7[*c - 32];
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 7; j++) {
                    if (char_data[i] & (1 << j)) {
                        int pixel_x = pos_x + i;
                        int pixel_y = pos_y + j;
                        if (pixel_x >= 0 && pixel_x < SSD1306_WIDTH && pixel_y >= 0 && pixel_y < SSD1306_HEIGHT) {
                            int byte_index = pixel_x + (pixel_y / 8) * SSD1306_WIDTH;
                            int bit_index = pixel_y % 8;
                            display->buffer[byte_index] |= (1 << bit_index);
                        }
                    }
                }
            }
            pos_x += 6; // Character width + spacing
        }
        c++;
    }

    return ESP_OK;
}

esp_err_t ssd1306_draw_progress_bar(ssd1306_t *display, int x, int y, int width, int height, int progress) {
    if (!display) {
        return ESP_ERR_INVALID_ARG;
    }

    // Draw border
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (i == 0 || i == width - 1 || j == 0 || j == height - 1) {
                int pixel_x = x + i;
                int pixel_y = y + j;
                if (pixel_x >= 0 && pixel_x < SSD1306_WIDTH && pixel_y >= 0 && pixel_y < SSD1306_HEIGHT) {
                    int byte_index = pixel_x + (pixel_y / 8) * SSD1306_WIDTH;
                    int bit_index = pixel_y % 8;
                    display->buffer[byte_index] |= (1 << bit_index);
                }
            }
        }
    }

    // Fill progress
    int fill_width = (width - 2) * progress / 100;
    for (int i = 1; i <= fill_width; i++) {
        for (int j = 1; j < height - 1; j++) {
            int pixel_x = x + i;
            int pixel_y = y + j;
            if (pixel_x >= 0 && pixel_x < SSD1306_WIDTH && pixel_y >= 0 && pixel_y < SSD1306_HEIGHT) {
                int byte_index = pixel_x + (pixel_y / 8) * SSD1306_WIDTH;
                int bit_index = pixel_y % 8;
                display->buffer[byte_index] |= (1 << bit_index);
            }
        }
    }

    return ESP_OK;
}

int ssd1306_get_width(ssd1306_t *display) {
    return SSD1306_WIDTH;
}

int ssd1306_get_height(ssd1306_t *display) {
    return SSD1306_HEIGHT;
}