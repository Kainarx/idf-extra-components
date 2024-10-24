/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of LED strip handle
 */
typedef struct led_strip_t *led_strip_handle_t;

/**
 * @brief LED strip model
 * @note Different led model may have different timing parameters, so we need to distinguish them.
 */
typedef enum {
    LED_MODEL_WS2812, /*!< LED strip model: WS2812 */
    LED_MODEL_SK6812, /*!< LED strip model: SK6812 */
    LED_MODEL_INVALID /*!< Invalid LED strip model */
} led_model_t;

/**
 * @brief Help macro to set pixel RGB color order
 *        The default order of the three-color LED strips is GRB. If you have a different order, you can use the macro to set `pixel_order` in led_strip_config_t.
 *        The positions are counted from the least significant bit (LSB).
 * @param R The position of the red channel in the color order.
 * @param G The position of the green channel in the color order.
 * @param B The position of the blue channel in the color order.
 * @note The order starts from 0. And the user needs to make sure that all the numbers appear exactly once and are all less than the number of colors per pixel.
 */
#define LED_STRIP_SET_RGB_ORDER(R, G, B) (R << 0 | G << 2 | B << 4)

/**
 * @brief Help macro to set pixel RGBW color order
 *        The default order of the four-color LED strips is GRBW. If you have a different order, you can use the macro to set `pixel_order` in led_strip_config_t.
 *        The positions are counted from the least significant bit (LSB).
 * @param R The position of the red channel in the color order.
 * @param G The position of the green channel in the color order.
 * @param B The position of the blue channel in the color order.
 * @param W The position of the white channel in the color order.
 * @note The order starts from 0. And the user needs to make sure that all the numbers appear exactly once and are all less than the number of colors per pixel.
 */
#define LED_STRIP_SET_RGBW_ORDER(R, G, B, W) (R << 0 | G << 2 | B << 4 | W << 6)

/**
 * @brief LED Strip common configurations
 *        The common configurations are not specific to any backend peripheral.
 */
typedef struct {
    int strip_gpio_num;           /*!< GPIO number that used by LED strip */
    uint32_t max_leds;            /*!< Maximum number of LEDs that can be controlled in a single strip */
    led_model_t led_model;        /*!< Specifies the LED strip model (e.g., WS2812, SK6812) */
    uint8_t num_color_components; /*!< Number of color components per LED pixel.
                                       Use 3 for RGB (Red, Green, Blue) or 4 for RGBW (Red, Green, Blue, White).
                                       If set to 0, the driver will default to 3 (RGB). */
    uint8_t color_component_order; /*!< Specifies the order of color components in each pixel.
                                        Use helper macros LED_STRIP_SET_RGB_ORDER or LED_STRIP_SET_RGBW_ORDER to set the order.
                                        Set to 0 to use the default order. */
    /*!< LED strip extra driver flags */
    struct led_strip_extra_flags {
        uint32_t invert_out: 1; /*!< Invert output signal */
    } flags; /*!< Extra driver flags */
} led_strip_config_t;

#ifdef __cplusplus
}
#endif
