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

#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Display geometry
#define SSD1306_WIDTH  128
#define SSD1306_HEIGHT 64

// I2C Configuration
#define SSD1306_I2C_ADDR    0x3C
#define SSD1306_I2C_PORT    I2C_NUM_0
#define SSD1306_I2C_FREQ    400000

// Text alignment
typedef enum {
    TEXT_ALIGN_LEFT = 0,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT,
    TEXT_ALIGN_CENTER_BOTH
} text_align_t;

// Font types
typedef enum {
    FONT_ArialMT_Plain_10 = 0,
    FONT_ArialMT_Plain_16,
    FONT_ArialMT_Plain_24
} font_type_t;

// SSD1306 Display structure
typedef struct {
    i2c_config_t i2c_config;
    uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
    bool flipped;
    uint8_t contrast;
    text_align_t text_align;
    font_type_t current_font;
    uint8_t cursor_x;
    uint8_t cursor_y;
} ssd1306_t;

// Function prototypes
esp_err_t ssd1306_init(ssd1306_t *display, int sda_pin, int scl_pin);
esp_err_t ssd1306_clear(ssd1306_t *display);
esp_err_t ssd1306_display(ssd1306_t *display);
esp_err_t ssd1306_flip_screen_vertically(ssd1306_t *display, bool flip);
esp_err_t ssd1306_set_contrast(ssd1306_t *display, uint8_t contrast);
esp_err_t ssd1306_set_text_alignment(ssd1306_t *display, text_align_t alignment);
esp_err_t ssd1306_set_font(ssd1306_t *display, font_type_t font);
esp_err_t ssd1306_draw_string(ssd1306_t *display, int x, int y, const char *text);
esp_err_t ssd1306_draw_progress_bar(ssd1306_t *display, int x, int y, int width, int height, int progress);
int ssd1306_get_width(ssd1306_t *display);
int ssd1306_get_height(ssd1306_t *display);

#ifdef __cplusplus
}
#endif

#endif // SSD1306_H