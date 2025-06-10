// Copyright 2021 Espressif Systems (Shanghai) CO LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include <color_format.h>
#include <math.h>

void hsv_to_rgb(HS_color_t HS, uint8_t brightness, RGB_color_t *RGB)
{
    uint16_t hue = HS.hue % 360;
    uint16_t hi = (hue / 60) % 6;
    uint16_t F = 100 * hue / 60 - 100 * hi;
    uint16_t P = brightness * (100 - HS.saturation) / 100;
    uint16_t Q = brightness * (10000 - F * HS.saturation) / 10000;
    uint16_t T = brightness * (10000 - HS.saturation * (100 - F)) / 10000;

    switch (hi) {
    case 0:
        RGB->red = brightness;
        RGB->green = T;
        RGB->blue = P;
        break;

    case 1:
        RGB->red = Q;
        RGB->green = brightness;
        RGB->blue = P;
        break;

    case 2:
        RGB->red = P;
        RGB->green = brightness;
        RGB->blue = T;
        break;

    case 3:
        RGB->red = P;
        RGB->green = Q;
        RGB->blue = brightness;
        break;

    case 4:
        RGB->red = T;
        RGB->green = P;
        RGB->blue = brightness;
        break;

    case 5:
        RGB->red = brightness;
        RGB->green = P;
        RGB->blue = Q;
        break;

    default:
        break;
    }

    RGB->red = RGB->red * 255 / 100;
    RGB->green = RGB->green * 255 / 100;
    RGB->blue = RGB->blue * 255 / 100;
}

// A Table from color temperature to hue and saturation.
// hue = temp_table[(temp - 600) / 100].hue
// saturation= temp_table[(temp - 600) / 100].saturation
// 600<= temp <= 10000
const HS_color_t temp_table[] = {
    {4, 100},  {8, 100},  {11, 100}, {14, 100}, {16, 100}, {18, 100}, {20, 100}, {22, 100}, {24, 100}, {25, 100},
    {27, 100}, {28, 100}, {30, 100}, {31, 100}, {31, 95},  {30, 89},  {30, 85},  {29, 80},  {29, 76},  {29, 73},
    {29, 69},  {28, 66},  {28, 63},  {28, 60},  {28, 57},  {28, 54},  {28, 52},  {27, 49},  {27, 47},  {27, 45},
    {27, 43},  {27, 41},  {27, 39},  {27, 37},  {27, 35},  {27, 33},  {27, 31},  {27, 30},  {27, 28},  {27, 26},
    {27, 25},  {27, 23},  {27, 22},  {27, 21},  {27, 19},  {27, 18},  {27, 17},  {27, 15},  {28, 14},  {28, 13},
    {28, 12},  {29, 10},  {29, 9},   {30, 8},   {31, 7},   {32, 6},   {34, 5},   {36, 4},   {41, 3},   {49, 2},
    {0, 0},    {294, 2},  {265, 3},  {251, 4},  {242, 5},  {237, 6},  {233, 7},  {231, 8},  {229, 9},  {228, 10},
    {227, 11}, {226, 11}, {226, 12}, {225, 13}, {225, 13}, {224, 14}, {224, 14}, {224, 15}, {224, 15}, {223, 16},
    {223, 16}, {223, 17}, {223, 17}, {223, 17}, {222, 18}, {222, 18}, {222, 19}, {222, 19}, {222, 19}, {222, 19},
    {222, 20}, {222, 20}, {222, 20}, {222, 21}, {222, 21}};

void temp_to_hs(uint32_t temperature, HS_color_t *HS)
{
    if (temperature < 600) {
        HS->hue = 0;
        HS->saturation = 100;
        return;
    }
    if (temperature > 10000) {
        HS->hue = 222;
        HS->saturation = 21 + (temperature - 10000) * 41 / 990000;
        return;
    }
    HS->hue = temp_table[(temperature - 600) / 100].hue;
    HS->saturation = temp_table[(temperature - 600) / 100].saturation;
}

void xy_to_rgb(XY_color_t XY, uint8_t brightness, RGB_color_t *RGB)
{
    // Convert Matter xy coordinates (0-65536) to CIE xy coordinates (0.0-1.0)
    float x = (float)XY.x / 65536.0f;
    float y = (float)XY.y / 65536.0f;
    float z = 1.0f - x - y;
    
    // Convert brightness (0-255) to Y value (0.0-1.0)
    float Y = (float)brightness / 255.0f;
    
    // Convert from xy to XYZ
    float X, Z;
    if (y > 0.0f) {
        X = (Y / y) * x;
        Z = (Y / y) * z;
    } else {
        X = 0.0f;
        Z = 0.0f;
    }
    
    // Convert XYZ to RGB using D65 white point matrix
    float r = X * 3.240479f - Y * 1.537150f - Z * 0.498535f;
    float g = -X * 0.969256f + Y * 1.875992f + Z * 0.041556f;
    float b = X * 0.055648f - Y * 0.204043f + Z * 1.057311f;
    
    // Apply reverse gamma correction
    if (r <= 0.0031308f) {
        r = 12.92f * r;
    } else {
        r = (1.0f + 0.055f) * powf(r, (1.0f / 2.4f)) - 0.055f;
    }
    
    if (g <= 0.0031308f) {
        g = 12.92f * g;
    } else {
        g = (1.0f + 0.055f) * powf(g, (1.0f / 2.4f)) - 0.055f;
    }
    
    if (b <= 0.0031308f) {
        b = 12.92f * b;
    } else {
        b = (1.0f + 0.055f) * powf(b, (1.0f / 2.4f)) - 0.055f;
    }
    
    // Clamp values to [0, 1] range
    if (r < 0.0f) r = 0.0f;
    if (r > 1.0f) r = 1.0f;
    if (g < 0.0f) g = 0.0f;
    if (g > 1.0f) g = 1.0f;
    if (b < 0.0f) b = 0.0f;
    if (b > 1.0f) b = 1.0f;
    
    // Convert to 0-255 range
    RGB->red = (uint8_t)(r * 255.0f);
    RGB->green = (uint8_t)(g * 255.0f);
    RGB->blue = (uint8_t)(b * 255.0f);
}
