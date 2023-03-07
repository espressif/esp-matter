/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit DCM matrix related routines
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

#ifndef SL_IMU_H
#define SL_IMU_H

#include <stdint.h>
#include <stdbool.h>
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup imu IMU - Inertial Measurement Unit
 * @brief Inertial Measurement Unit driver
 * @{


  @n @section imu_example IMU example code

    Basic example for looping measurement of orientation data: @n @n
    @verbatim

  #include "sl_imu.h"

  int main( void )
  {

    ...

    int16_t o_vec[3];

    sl_imu_init();

    // Configure sample rate
    sl_imu_configure(10);

    // Recalibrate gyro
    sl_imu_calibrate_gyro();

    while (true) {

      sl_imu_update();

      while (!sl_imu_is_data_ready()) {
        // wait
      }

      sl_imu_get_orientation(o_vec);

      ...

    }
  } @endverbatim
  ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#define IMU_PI                     3.14159265358979323            /**< The value of the Pi                  */
#define IMU_DEG_TO_RAD_FACTOR      ( (2.0 * IMU_PI) / 360.0)      /**< Degrees to radians conversion factor */
#define IMU_RAD_TO_DEG_FACTOR      (360.0 / (2.0 * IMU_PI) )      /**< Radians to degrees conversion factor */
#define IMU_DEG_TO_RAD(ang)        (ang * IMU_DEG_TO_RAD_FACTOR)  /**< Degrees to radians converter macro   */
#define IMU_RAD_TO_DEG(ang)        (ang * IMU_RAD_TO_DEG_FACTOR)  /**< Radians to degrees converter macro   */

#define IMU_MAX_ACCEL_FOR_ANGLE    0.9848  /**< Maximum acceleration value for a given angle                */
/** @endcond */

/**************************************************************************//**
* @name State Definitions
* @{
******************************************************************************/
#define IMU_STATE_DISABLED         0x00    /**< The IMU is disabled                                         */
#define IMU_STATE_READY            0x01    /**< The IMU is fully configured and ready to take measurements  */
#define IMU_STATE_INITIALIZING     0x02    /**< The IMU is being initialized                                */
#define IMU_STATE_CALIBRATING      0x03    /**< The IMU is being calibrated                                 */
/**@}*/

/***************************************************************************//**
 * @addtogroup imu_fusion IMU Fusion
 * @brief IMU fusion driver.
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @brief
 *    Structure to store the sensor fusion data
 ******************************************************************************/
typedef struct sl_imu_sensor_fusion{
  /* Direction Cosine Matrix */
  float     dcm[3][3];           /**< Direction Cosine Matrix                       */

  /* Accelerometer filtering */
  float     aVector[3];          /**< Acceleration vector                           */
  float     aAccumulator[3];     /**< Accumulator for acceleration vector           */
  uint32_t  aAccumulatorCount;   /**< Number of vectors stored in the accumulator   */
  float     aSampleRate;         /**< Acceleration measurement sample rate          */

  /* Gyro */
  float     gVector[3];          /**< Gyro vector                                   */
  float     gSampleRate;         /**< Gyroscope measurement sample rate             */
  float     gDeltaTime;          /**< Time between gyro samples                     */
  float     gDeltaTimeScale;     /**< Rotation between gyro samples                 */

  /* Orientation */
  float     angleCorrection[3];  /**< Angle correction vector                       */
  float     orientation[3];      /**< Orientation vector                            */
} sl_imu_sensor_fusion_t;

/**@}*/

/***************************************************************************//**
 * @brief
 *    Initialize and calibrate the IMU chip.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_imu_init(void);

/***************************************************************************//**
 * @brief
 *    De-initialize the IMU chip.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
sl_status_t sl_imu_deinit(void);

/***************************************************************************//**
 * @brief
 *    Return IMU state.
 *
 * @return
 *    IMU state
 ******************************************************************************/
uint8_t sl_imu_get_state(void);

/***************************************************************************//**
 * @brief
 *    Get a new set of data from the accel and gyro sensor and updates the
 *    fusion calculation.
 ******************************************************************************/
void sl_imu_update(void);

/***************************************************************************//**
 * @brief
 *    Reset the fusion calculation.
 ******************************************************************************/
void sl_imu_reset(void);

/***************************************************************************//**
 * @brief
 *    Retrieve the processed acceleration data.
 *
 * @param[out] avec
 *    Three dimensional acceleration vector
 ******************************************************************************/
void sl_imu_get_acceleration(int16_t avec[3]);

/***************************************************************************//**
 * @brief
 *    Retrieve the processed orientation data.
 *
 * @param[out] ovec
 *    Three dimensional orientation vector
 ******************************************************************************/
void sl_imu_get_orientation(int16_t ovec[3]);

/***************************************************************************//**
 * @brief
 *    Retrieve the processed gyroscope data.
 *
 * @param[out] gvec
 *    Three dimensional gyro vector
 ******************************************************************************/
void sl_imu_get_gyro(int16_t gvec[3]);

/***************************************************************************//**
 * @brief
 *    Perform gyroscope calibration to cancel gyro bias.
 *
 * @retval SL_STATUS_OK
 *
 ******************************************************************************/
sl_status_t sl_imu_calibrate_gyro(void);

/***************************************************************************//**
 * @brief
 *    Retrieve the processed gyroscope correction angles.
 *
 * @param[out] acorr
 *    Three dimensional gyro correction angle vector
 ******************************************************************************/
void sl_imu_get_gyro_correction_angles(float acorr[3]);

/***************************************************************************//**
 * @addtogroup vector_matrix_math Vector and Matrix Math
 * @brief Inertial measurement unit fusion driver math routines.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Normalize the angle ( -PI < angle <= PI )
 *
 * @param a
 *    The angle to be normalized
 ******************************************************************************/
void sl_imu_normalize_angle(float *a);

/***************************************************************************//**
 * @brief
 *    Multiply two 3x3 matrices.
 *
 * @param[out] c
 *    The multiplication result, AB
 *
 * @param[in] a
 *    Input vector A
 *
 * @param[in] b
 *    Input vector B
 ******************************************************************************/
void sl_imu_matrix_multiply(float c[3][3], float a[3][3], float b[3][3]);

/***************************************************************************//**
 * @brief
 *    Normalize the angle of a vector.
 *
 * @param v
 *    The vector, which contains angles to be normalized
 ******************************************************************************/
void sl_imu_vector_normalize_angle(float v[3]);

/***************************************************************************//**
 * @brief
 *    Set all elements of a vector to 0.
 *
 * @param v
 *    The vector to be cleared
 ******************************************************************************/
void sl_imu_vector_zero(float v[3]);

/***************************************************************************//**
 * @brief
 *    Scale a vector by a factor.
 *
 * @param v
 *    The vector to be scaled
 *
 * @param[in] scale
 *    The scale factor
 ******************************************************************************/
void sl_imu_vector_scale(float v[3], float scale);

/***************************************************************************//**
 * @brief
 *    Multiply a vector by a scalar.
 *
 * @param[out] r
 *    The multiplied vector
 *
 * @param[in] v
 *    The vector to be multiplied
 *
 * @param[in] scale
 *    The scalar multiplicator value
 ******************************************************************************/
void sl_imu_vector_scalar_multiplication(float r[3], float v[3], float scale);

/***************************************************************************//**
 * @brief
 *    Add two vectors.
 *
 * @param[out] r
 *    The vectorial sum of the vectors, a+b
 *
 * @param[in] a
 *    The first vector
 *
 * @param[in] b
 *    The second vector
 ******************************************************************************/
void sl_imu_vector_add(float r[3], float a[3], float b[3]);

/***************************************************************************//**
 * @brief
 *    Subtract vector b from vector a.
 *
 * @param[out] r
 *    The vectorial difference, a-b
 *
 * @param[in] a
 *    Vector a
 *
 * @param[in] b
 *    Vector b
 ******************************************************************************/
void sl_imu_vector_subtract(float r[3], float a[3], float b[3]);

/***************************************************************************//**
 * @brief
 *    Calculate the dot product of two vectors.
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
float sl_imu_vector_dot_product(float a[3], float b[3]);

/***************************************************************************//**
 * @brief
 *    Calculate the cross product of two vectors.
 *
 * @param[out] r
 *    The cross product
 *
 * @param[in] a
 *    The first vector
 *
 * @param[in] b
 *    The second vector
 ******************************************************************************/
void sl_imu_vector_cross_product(float r[3], float a[3], float b[3]);
/** @} */ //Vector and matrix math

/***************************************************************************//**
 * @addtogroup direction_cosine_matrix Direction Cosine Matrix
 * @brief Unit DCM matrix related routines
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Set the elements of the DCM matrix to the corresponding elements of the
 *    identity matrix.
 *
 * @param dcm
 *    DCM matrix
 ******************************************************************************/
void sl_imu_dcm_reset(float dcm[3][3]);

/***************************************************************************//**
 * @brief
 *    DCM reset, Z direction
 *
 * @param dcm
 *    DCM matrix
 ******************************************************************************/
void sl_imu_dcm_reset_z(float dcm[3][3]);

/***************************************************************************//**
 * @brief
 *    Normalize the DCM matrix.
 *
 * @param dcm
 *    DCM matrix
 ******************************************************************************/
void sl_imu_dcm_normalize(float dcm[3][3]);

/***************************************************************************//**
 * @brief
 *    Rotate the DCM matrix by a given angle.
 *
 * @param[in, out] dcm
 *    DCM matrix
 *
 * @param[in] ang
 *    Rotation angle
 ******************************************************************************/
void sl_imu_dcm_rotate(float dcm[3][3], float ang[3]);

/***************************************************************************//**
 * @brief
 *    Calculate the Euler angles (roll, pitch, yaw) from the DCM matrix.
 *
 * @param[in] dcm
 *    DCM matrix
 *
 * @param[out] ang
 *    An array containing the Euler angles
 ******************************************************************************/
void sl_imu_dcm_get_angles(float dcm[3][3], float ang[3]);
/** @} */ //Direction cosine matrix funtions

/***************************************************************************//**
 * @brief
 *    Configure the IMU.
 *
 * @param[in] sampleRate
 *    The desired sample rate of the acceleration and gyro sensor
 ******************************************************************************/
void sl_imu_configure(float sampleRate);

/***************************************************************************//**
 * @brief
 *    Check if new accel/gyro data is available for read.
 *
 * @return
 *    True if the measurement data is ready, false otherwise
 ******************************************************************************/
bool sl_imu_is_data_ready(void);

/***************************************************************************//**
 * @brief
 *    Retrieve the raw acceleration data from the IMU.
 *
 * @param[out] avec
 *    Three dimensional raw acceleration vector
 ******************************************************************************/
void sl_imu_get_acceleration_raw_data(float avec[3]);

/***************************************************************************//**
 * @brief
 *    Retrieve the raw gyroscope data from the IMU.
 *
 * @param[out] gvec
 *    Three dimensional raw gyro vector
 ******************************************************************************/
void sl_imu_get_gyro_raw_data(float gvec[3]);

/***************************************************************************//**
 * @addtogroup imu_fusion
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Set the accelerometer sample rate in the sl_imu_sensor_fusion_t structure.
 *
 * @param[in, out] f
 *    Pointer to the sl_imu_sensor_fusion_t object
 *
 * @param[in] rate
 *    Sample rate of the accelerometer
 ******************************************************************************/
void sl_imu_fuse_accelerometer_set_sample_rate(sl_imu_sensor_fusion_t *f, float rate);

/***************************************************************************//**
 * @brief
 *    Add the current accelerometer data to the accumulator.
 *
 * @param[in, out] f
 *    Pointer to the sl_imu_sensor_fusion_t object
 *
 * @param[in] avec
 *    Accelerometer vector
 ******************************************************************************/
void sl_imu_fuse_accelerometer_update_filter(sl_imu_sensor_fusion_t *f, float avec[3]);

/***************************************************************************//**
 * @brief
 *    Set the gyro sample rate and related values in the sl_imu_sensor_fusion_t
 *    structure.
 *
 * @param[in, out] f
 *    Pointer to the sl_imu_sensor_fusion_t object
 *
 * @param[in] rate
 *    Sample rate of the gyroscope
 ******************************************************************************/
void sl_imu_fuse_gyro_set_sample_rate(sl_imu_sensor_fusion_t *f, float rate);

/***************************************************************************//**
 * @brief
 *    Update the fusion calculation with a new gyro data.
 *
 * @param[in, out] f
 *    Pointer to the sl_imu_sensor_fusion_t object
 *
 * @param[in] gvec
 *    Gyroscope vector
 ******************************************************************************/
void sl_imu_fuse_gyro_update(sl_imu_sensor_fusion_t *f, float gvec[3]);

/***************************************************************************//**
 * @brief
 *    Clear the gyro correction vector.
 *
 * @param[in, out] f
 *    Pointer to the sl_imu_sensor_fusion_t object
 ******************************************************************************/
void sl_imu_fuse_gyro_clear_correction_vector(sl_imu_sensor_fusion_t *f);

/***************************************************************************//**
 * @brief
 *    Calculate the gyro correction vector.
 *
 * @param[in, out] f
 *    Pointer to the sl_imu_sensor_fusion_t object
 *
 * @param[in] accValid
 *    True if the acceleration value is within limits
 *
 * @param[in] dirValid
 *    True if the direction value is valid
 *
 * @param[in] dirZ
 *    The direction of the Z axis
 ******************************************************************************/
void sl_imu_fuse_gyro_calculate_correction_vector(sl_imu_sensor_fusion_t *f, bool accValid, bool dirValid, float dirZ);

/***************************************************************************//**
 * @brief
 *    Initialize a new sl_imu_sensor_fusion_t structure.
 *
 * @param[in, out] f
 *    Pointer to the sl_imu_sensor_fusion_t object to be initialized
 ******************************************************************************/
void sl_imu_fuse_new(sl_imu_sensor_fusion_t *f);

/***************************************************************************//**
 * @brief
 *    Clear the values of the sensor fusion object.
 *
 * @param[in, out] f
 *    Pointer to the sl_imu_sensor_fusion_t object
 ******************************************************************************/
void sl_imu_fuse_reset(sl_imu_sensor_fusion_t *f);

/***************************************************************************//**
 * @brief
 *    Update the fusion calculation.
 *
 * @param[in, out] f
 *    Pointer to the sl_imu_sensor_fusion_t object
 ******************************************************************************/
void sl_imu_fuse_update(sl_imu_sensor_fusion_t *f);

/** @} */  // Sensor fusion functions
/** @} */ // IMU

#ifdef __cplusplus
}
#endif

#endif // SL_IMU_H
