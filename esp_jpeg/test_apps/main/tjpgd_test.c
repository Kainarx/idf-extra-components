/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"


#include "jpeg_decoder.h"
#include "test_logo_jpg.h"
#include "test_logo_rgb888.h"

#define TESTW 46
#define TESTH 46

TEST_CASE("Test JPEG decompression library", "[esp_jpeg]")
{
    char aapix[] = " .:;+=xX$$";
    unsigned char *decoded, *p;
    const unsigned char *o;
    int x, y, v;
    int decoded_outsize = TESTW * TESTH * 3;

    decoded = malloc(decoded_outsize);
    for (x = 0; x < decoded_outsize; x += 2) {
        decoded[x] = 0;
        decoded[x + 1] = 0xff;
    }

    /* JPEG decode */
    esp_jpeg_image_cfg_t jpeg_cfg = {
        .indata = (uint8_t *)logo_jpg,
        .indata_size = logo_jpg_len,
        .outbuf = decoded,
        .outbuf_size = decoded_outsize,
        .out_format = JPEG_IMAGE_FORMAT_RGB888,
        .out_scale = JPEG_IMAGE_SCALE_0,
        .flags = {
            .swap_color_bytes = 0,
        }
    };
    esp_jpeg_image_output_t outimg;
    esp_err_t err = esp_jpeg_decode(&jpeg_cfg, &outimg);
    TEST_ASSERT_EQUAL(err, ESP_OK);

    /* Decoded image size */
    TEST_ASSERT_EQUAL(outimg.width, TESTW);
    TEST_ASSERT_EQUAL(outimg.height, TESTH);

    p = decoded;
    o = logo_rgb888;
    for (x = 0; x < outimg.width * outimg.height; x++) {
        /* The color can be +- 2 */
        TEST_ASSERT_UINT8_WITHIN(2, o[0], p[0]);
        TEST_ASSERT_UINT8_WITHIN(2, o[1], p[1]);
        TEST_ASSERT_UINT8_WITHIN(2, o[2], p[2]);

        p += 3;
        o += 3;
    }

    p = decoded + 2;
    for (y = 0; y < outimg.width; y++) {
        for (x = 0; x < outimg.height; x++) {
            v = ((*p) * (sizeof(aapix) - 2) * 2) / 256;
            printf("%c%c", aapix[v / 2], aapix[(v + 1) / 2]);
            p += 3;
        }
        printf("%c%c", ' ', '\n');
    }


    free(decoded);
}

#if CONFIG_JD_DEFAULT_HUFFMAN
#include "test_usb_camera_jpg.h"
#include "test_usb_camera_rgb888.h"

/**
 * @brief Test for JPEG decompression without Huffman tables
 *
 * This test case verifies the functionality of the JPEG decompression library
 * when decoding an image that lacks Huffman tables, such as a USB frame
 * from a Logitech C270 USB camera. The image was reconstructed from raw USB data
 * (using `hex_to_jpg.py`) and then converted into an RGB888 C-style array
 * (using `jpg_to_rgb888_hex.py`).
 *
 * Due to the unique structure of the JPEG data (double block size, 16x8 pixels)
 * and absence of Huffman tables, this test assesses whether the decompression
 * library correctly decodes the image and outputs RGB888 pixel data within
 * an acceptable tolerance range.
 *
 * The test performs the following steps:
 * - Allocates a buffer for the decoded image.
 * - Configures and runs the JPEG decoder with the RGB888 output format.
 * - Checks that the decoded image dimensions match expected values.
 * - Compares the decompressed image data against the reference RGB888 data,
 *   allowing a tolerance of ±16 in each color component due to potential
 *   differences in Huffman tables or decompression accuracy.
 *
 * @note This test allows a margin of error in pixel values due to potential
 *       differences in how color data is interpreted across different decoders.
 *
 * @param None
 *
 * @return None
 *
 * @test Requirements:
 * - JPEG decompression library support for images without Huffman tables.
 * - JPEG decompression accuracy within acceptable error margins.
 */
TEST_CASE("Test JPEG decompression library: No Huffman tables", "[esp_jpeg]")
{
    unsigned char *decoded, *p;
    const unsigned int *o;
    int decoded_outsize = 160 * 120 * 3;

    decoded = malloc(decoded_outsize);

    /* JPEG decode */
    esp_jpeg_image_cfg_t jpeg_cfg = {
        .indata = (uint8_t *)jpeg_no_huffman,
        .indata_size = jpeg_no_huffman_len,
        .outbuf = decoded,
        .outbuf_size = decoded_outsize,
        .out_format = JPEG_IMAGE_FORMAT_RGB888,
        .out_scale = JPEG_IMAGE_SCALE_0,
        .flags = {
            .swap_color_bytes = 0,
        }
    };
    esp_jpeg_image_output_t outimg;
    esp_err_t err = esp_jpeg_decode(&jpeg_cfg, &outimg);
    TEST_ASSERT_EQUAL(err, ESP_OK);

    /* Decoded image size */
    TEST_ASSERT_EQUAL(outimg.width, 160);
    TEST_ASSERT_EQUAL(outimg.height, 120);

    p = decoded;
    o = jpeg_no_huffman_rgb888;
    for (int x = 0; x < outimg.width * outimg.height; x++) {
        /* The color can be +- 16 */
        // Here we allow bigger decoding error
        // It might be that the Windows decoder used slightly different Huffman tables
        TEST_ASSERT_UINT8_WITHIN(16, (*o) & 0xff, p[0]);
        TEST_ASSERT_UINT8_WITHIN(16, (*o >> 8) & 0xff, p[1]);
        TEST_ASSERT_UINT8_WITHIN(16, (*o >> 16) & 0xff, p[2]);

        p += 3; // this is uint8_t
        o ++;   // this is unt32_t
    }

    free(decoded);
}

#endif