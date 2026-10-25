// Copyright 2021-2026 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <math.h>

#include <led_driver_utils.h>

static const HS_color_t temp_table[] = {
    {4, 100},  {8, 100},  {11, 100}, {14, 100}, {16, 100}, {18, 100}, {20, 100}, {22, 100}, {24, 100}, {25, 100},
    {27, 100}, {28, 100}, {30, 100}, {31, 100}, {31, 95},  {30, 89},  {30, 85},  {29, 80},  {29, 76},  {29, 73},
    {29, 69},  {28, 66},  {28, 63},  {28, 60},  {28, 57},  {28, 54},  {28, 52},  {27, 49},  {27, 47},  {27, 45},
    {27, 43},  {27, 41},  {27, 39},  {27, 37},  {27, 35},  {27, 33},  {27, 31},  {27, 30},  {27, 28},  {27, 26},
    {27, 25},  {27, 23},  {27, 22},  {27, 21},  {27, 19},  {27, 18},  {27, 17},  {27, 15},  {28, 14},  {28, 13},
    {28, 12},  {29, 10},  {29, 9},   {30, 8},   {31, 7},   {32, 6},   {34, 5},   {36, 4},   {41, 3},   {49, 2},
    {0, 0},    {294, 2},  {265, 3},  {251, 4},  {242, 5},  {237, 6},  {233, 7},  {231, 8},  {229, 9},  {228, 10},
    {227, 11}, {226, 11}, {226, 12}, {225, 13}, {225, 13}, {224, 14}, {224, 14}, {224, 15}, {224, 15}, {223, 16},
    {223, 16}, {223, 17}, {223, 17}, {223, 17}, {222, 18}, {222, 18}, {222, 19}, {222, 19}, {222, 19}, {222, 19},
    {222, 20}, {222, 20}, {222, 20}, {222, 21}, {222, 21},
};

static uint8_t rgb_channel(float channel)
{
    if (channel <= 0.0f) {
        return 0;
    }
    if (channel >= 1.0f) {
        return UINT8_MAX;
    }
    return (uint8_t)(channel * UINT8_MAX);
}

static float reverse_gamma(float channel)
{
    if (channel <= 0.0031308f) {
        return 12.92f * channel;
    }
    return 1.055f * powf(channel, 1.0f / 2.4f) - 0.055f;
}

static HS_color_t rgb_to_hs(RGB_color_t rgb)
{
    const uint8_t minimum =
        rgb.red < rgb.green ? (rgb.red < rgb.blue ? rgb.red : rgb.blue) : (rgb.green < rgb.blue ? rgb.green : rgb.blue);
    const uint8_t maximum =
        rgb.red > rgb.green ? (rgb.red > rgb.blue ? rgb.red : rgb.blue) : (rgb.green > rgb.blue ? rgb.green : rgb.blue);
    const uint8_t delta = maximum - minimum;
    if (delta == 0) {
        return (HS_color_t) {
            0, 0
        };
    }

    int32_t hue;
    if (maximum == rgb.red) {
        hue = 60 * ((int32_t)rgb.green - rgb.blue) / delta;
    } else if (maximum == rgb.green) {
        hue = 60 * ((int32_t)rgb.blue - rgb.red) / delta + 120;
    } else {
        hue = 60 * ((int32_t)rgb.red - rgb.green) / delta + 240;
    }
    if (hue < 0) {
        hue += 360;
    }

    return (HS_color_t) {
        (uint16_t)hue, (uint8_t)((uint32_t)delta * UINT8_MAX / maximum)
    };
}

HS_color_t temp_to_hs(uint32_t temperature)
{
    HS_color_t color;
    if (temperature < 600) {
        color = (HS_color_t) {
            0, 100
        };
    } else if (temperature > 10000) {
        if (temperature > 1000000) {
            temperature = 1000000;
        }
        color = (HS_color_t) {
            222, 21 + (temperature - 10000) * 41 / 990000
        };
    } else {
        color = temp_table[(temperature - 600) / 100];
    }
    color.saturation = (uint8_t)((uint32_t)color.saturation * UINT8_MAX / 100);
    return color;
}

HS_color_t xy_to_hs(uint16_t matter_x, uint16_t matter_y)
{
    const float x = (float)matter_x / 65536.0f;
    const float y = (float)matter_y / 65536.0f;
    const float z = 1.0f - x - y;
    float X = 0.0f;
    float Z = 0.0f;
    if (y > 0.0f) {
        X = x / y;
        Z = z / y;
    }

    const RGB_color_t rgb = {
        .red = rgb_channel(reverse_gamma(X * 3.240479f - 1.537150f - Z * 0.498535f)),
        .green = rgb_channel(reverse_gamma(-X * 0.969256f + 1.875992f + Z * 0.041556f)),
        .blue = rgb_channel(reverse_gamma(X * 0.055648f - 0.204043f + Z * 1.057311f)),
    };
    return rgb_to_hs(rgb);
}
