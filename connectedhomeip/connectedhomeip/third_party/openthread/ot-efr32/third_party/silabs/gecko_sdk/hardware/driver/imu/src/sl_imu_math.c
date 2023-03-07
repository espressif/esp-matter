/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit Fusion driver math routines
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "sl_imu.h"

/***************************************************************************//**
 *    Normalizes the angle ( -PI < angle <= PI )
 ******************************************************************************/
void sl_imu_normalize_angle(float *a)
{
  while ( *a >= IMU_PI ) {
    *a -= 2 * IMU_PI;
  }

  while ( *a < -IMU_PI ) {
    *a += 2 * IMU_PI;
  }
}

/***************************************************************************//**
 *    Multiplies two 3x3 matrices
 ******************************************************************************/
void sl_imu_matrix_multiply(float c[3][3], float a[3][3], float b[3][3])
{
  int x, y, w;
  float op[3];

  for ( x = 0; x < 3; x++ ) {
    for ( y = 0; y < 3; y++ ) {
      for ( w = 0; w < 3; w++ ) {
        op[w] = a[x][w] * b[w][y];
      }
      c[x][y] = op[0] + op[1] + op[2];
    }
  }
}

/***************************************************************************//**
 *    Normalizes the angle of a vector
 ******************************************************************************/
void sl_imu_vector_normalize_angle(float v[3])
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    sl_imu_normalize_angle(&v[n]);
  }
}

/***************************************************************************//**
 *    Sets all elements of a vector to 0
 ******************************************************************************/
void sl_imu_vector_zero(float v[3])
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    v[n] = 0.0f;
  }
}

/***************************************************************************//**
 *    Multiplies a vector by a scalar
 ******************************************************************************/
void sl_imu_vector_scalar_multiplication(float r[3], float v[3], float scale)
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    r[n] = v[n] * scale;
  }
}

/***************************************************************************//**
 *    Scales a vector by a factor
 ******************************************************************************/
void sl_imu_vector_scale(float v[3], float scale)
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    v[n] *= scale;
  }
}

/***************************************************************************//**
 *    Adds two vectors
 ******************************************************************************/
void sl_imu_vector_add(float r[3], float a[3], float b[3])
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    r[n] = a[n] + b[n];
  }
}

/***************************************************************************//**
 *    Subtracts vector b from vector a
 ******************************************************************************/
void sl_imu_vector_subtract(float r[3], float a[3], float b[3])
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    r[n] = a[n] - b[n];
  }
}

/***************************************************************************//**
 *    Calculates the dot product of two vectors
 ******************************************************************************/
float sl_imu_vector_dot_product(float a[3], float b[3])
{
  float r;
  int n;

  r = 0.0f;
  for ( n = 0; n < 3; n++ ) {
    r += a[n] * b[n];
  }

  return r;
}

/***************************************************************************//**
 *    Calculates the cross product of two vectors
 ******************************************************************************/
void sl_imu_vector_cross_product(float r[3], float a[3], float b[3])
{
  r[0] = a[1] * b[2] - a[2] * b[1];
  r[1] = a[2] * b[0] - a[0] * b[2];
  r[2] = a[0] * b[1] - a[1] * b[0];
}
