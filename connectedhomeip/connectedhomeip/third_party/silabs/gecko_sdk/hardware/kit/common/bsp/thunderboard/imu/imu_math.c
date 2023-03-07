/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit Fusion driver math routines
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "thunderboard/imu/imu.h"

/***************************************************************************//**
 * @addtogroup IMU
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Normalizes the angle ( -PI < angle <= PI )
 *
 * @param a
 *    The angle to be normalized
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_normalizeAngle(float *a)
{
  while ( *a >= IMU_PI ) {
    *a -= 2 * IMU_PI;
  }

  while ( *a < -IMU_PI ) {
    *a += 2 * IMU_PI;
  }

  return;
}

/***************************************************************************//**
 * @brief
 *    Multiplies two 3x3 matrices
 *
 * @param[out] c
 *    The multiplication result, AB
 *
 * @param[in] a
 *    Input vector A
 *
 * @param[in] b
 *    Input vector B
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_matrixMultiply(float c[3][3], float a[3][3], float b[3][3])
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

  return;
}

/***************************************************************************//**
 * @brief
 *    Normalizes the angle of a vector
 *
 * @param v
 *    The vector, which contains angles to be normalized
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_vectorNormalizeAngle(float v[3])
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    IMU_normalizeAngle(&v[n]);
  }

  return;
}

/***************************************************************************//**
 * @brief
 *    Sets all elements of a vector to 0
 *
 * @param v
 *    The vector to be cleared
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_vectorZero(float v[3])
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    v[n] = 0.0f;
  }

  return;
}

/***************************************************************************//**
 * @brief
 *    Multiplies a vector by a scalar
 *
 * @param[out] r
 *    The multiplied vector
 *
 * @param[in] v
 *    The vector to be multiplied
 *
 * @param[in] scale
 *    The scalar multiplicator value
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_vectorScalarMultiplication(float r[3], float v[3], float scale)
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    r[n] = v[n] * scale;
  }

  return;
}

/***************************************************************************//**
 * @brief
 *    Scales a vector by a factor
 *
 * @param v
 *    The vector to be scaled
 *
 * @param[in] scale
 *    The scale factor
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_vectorScale(float v[3], float scale)
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    v[n] *= scale;
  }

  return;
}

/***************************************************************************//**
 * @brief
 *    Adds two vectors
 *
 * @param[out] r
 *    The vectorial sum of the vectors, a+b
 *
 * @param[in] a
 *    The first vector
 *
 * @param[in] b
 *    The second vector
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_vectorAdd(float r[3], float a[3], float b[3])
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    r[n] = a[n] + b[n];
  }

  return;
}

/***************************************************************************//**
 * @brief
 *    Subtracts vector b from vector a
 *
 * @param[out] r
 *    The vectorial difference, a-b
 *
 * @param[in] a
 *    Vector a
 *
 * @param[in] b
 *    Vector b
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_vectorSubtract(float r[3], float a[3], float b[3])
{
  int n;

  for ( n = 0; n < 3; n++ ) {
    r[n] = a[n] - b[n];
  }

  return;
}

/***************************************************************************//**
 * @brief
 *    Calculates the dot product of two vectors
 *
 * @param[in] a
 *    The first vector
 *
 * @param[in] b
 *    The second vector
 *
 * @return
 *    The dot product
 ******************************************************************************/
float IMU_vectorDotProduct(float a[3], float b[3])
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
 * @brief
 *    Calculates the cross product of two vectors
 *
 * @param[out] r
 *    The cross product
 *
 * @param[in] a
 *    The first vector
 *
 * @param[in] b
 *    The second vector
 *
 * @return
 *    None
 ******************************************************************************/
void IMU_vectorCrossProduct(float r[3], float a[3], float b[3])
{
  r[0] = a[1] * b[2] - a[2] * b[1];
  r[1] = a[2] * b[0] - a[0] * b[2];
  r[2] = a[0] * b[1] - a[1] * b[0];

  return;
}

/** @} */
