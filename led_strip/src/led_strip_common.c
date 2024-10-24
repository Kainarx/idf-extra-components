/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_bit_defs.h"
#include "led_strip_common.h"

static const char *TAG = "led_strip_common";

esp_err_t led_strip_set_color_order(uint8_t *led_pixel_offset, uint8_t pixel_order, uint8_t num_color_components)
{
    if (pixel_order) {
        uint8_t r_order = (pixel_order >> 0) & 0x03;
        uint8_t g_order = (pixel_order >> 2) & 0x03;
        uint8_t b_order = (pixel_order >> 4) & 0x03;
        uint8_t w_order = (pixel_order >> 6) & 0x03;
        uint8_t mask = 0;
        if (num_color_components == 3) {
            mask = BIT(r_order) | BIT(g_order) | BIT(b_order);
            // Check for invalid values
            ESP_RETURN_ON_FALSE(mask == 0x07, ESP_ERR_INVALID_ARG, TAG, "invalid order argument");
        } else {
            mask = BIT(r_order) | BIT(g_order) | BIT(b_order) | BIT(w_order);
            // Check for invalid values
            ESP_RETURN_ON_FALSE(mask == 0x0F, ESP_ERR_INVALID_ARG, TAG, "invalid order argument");
        }
        led_pixel_offset[LED_PIXEL_INDEX_RED] = r_order;
        led_pixel_offset[LED_PIXEL_INDEX_GREEN] = g_order;
        led_pixel_offset[LED_PIXEL_INDEX_BLUE] = b_order;
        led_pixel_offset[LED_PIXEL_INDEX_WHITE] = w_order;
    } else {
        // If pixel_order is not set, set default GRB(GRBW) order as fallback path
        led_pixel_offset[LED_PIXEL_INDEX_RED] = 1;
        led_pixel_offset[LED_PIXEL_INDEX_GREEN] = 0;
        led_pixel_offset[LED_PIXEL_INDEX_BLUE] = 2;
        led_pixel_offset[LED_PIXEL_INDEX_WHITE] = 3;
    }
    return ESP_OK;
}
