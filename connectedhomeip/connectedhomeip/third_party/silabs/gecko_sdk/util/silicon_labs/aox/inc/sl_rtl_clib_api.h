/***************************************************************************//**
 *  @brief Silicon Labs Real-Time Locationing library for AoA/D and locationing
 *******************************************************************************
 * # License
 * <b>Copyright 2019-2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/**************************************************************************//**
*
*  Information on open-source software used with the library can be found in
*  the included license.txt file.
*
******************************************************************************/

/**************************************************************************//**
 *  @file
 *  This is the API for the Silicon Labs Real-Time Locationing library.
 *  It provides an interface for estimating arrival and departure angles of
 *  signals and the positions of AoA/AoD signal transmitters. AoA stands for
 *  Angle-of-Arrival and AoD for Angle-of-Departure. AoX is used when
 *  referring to both techniques.
 *
 *  Estimators can be created individually for each locator node using the API.
 *  Even multiple estimators with different parameters can be created for a
 *  single node. An instance of the estimator is created as a libitem and the
 *  estimator is initialized. Next, the estimation parameters such as antenna
 *  array type, number of antennas, estimation mode, and so on can be set
 *  using the function calls described below.
 *
 *  One snapshot means a set of IQ-samples with exactly one sample per
 *  each antenna.
 *
 *****************************************************************************/

#ifndef SL_RTL_CLIB_API_H
#define SL_RTL_CLIB_API_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @addtogroup sl_rtl_error Error Codes
 * @{
 *
 * @brief RTL library Error Codes
 *
 */

/// RTL error code
enum sl_rtl_error_code{
  SL_RTL_ERROR_SUCCESS = 0, ///< Successful execution / estimation complete
  SL_RTL_ERROR_ARGUMENT, ///< Invalid argument
  SL_RTL_ERROR_OUT_OF_MEMORY, ///< Memory / allocation failure
  SL_RTL_ERROR_ESTIMATION_IN_PROGRESS, ///< Estimation not yet finished
  SL_RTL_ERROR_NUMBER_OF_SNAPHOTS_DO_NOT_MATCH, ///< Initialized and calculated number of snapshots do not match
  SL_RTL_ERROR_ESTIMATOR_NOT_CREATED, ///< Estimator not yet created
  SL_RTL_ERROR_NOT_INITIALIZED, ///< Library item not yet initialized
  SL_RTL_ERROR_INTERNAL, ///< An internal error occurred
  SL_RTL_ERROR_IQ_SAMPLE_QA, ///< IQ sample quality analysis failed
  SL_RTL_ERROR_FEATURE_NOT_SUPPORTED, ///< The requested feature is not supported by the library
  SL_RTL_ERROR_INCORRECT_MEASUREMENT, ///< The error of the last measurement for this locator was too large

  SL_RTL_ERROR_LAST ///< Number of error codes
};
/** @} */ // end addtogroup sl_rtl_error

/**
 * @addtogroup sl_rtl_aox Angle of Arrival / Departure
 * @{
 *
 * @brief Angle of Arrival / Departure
 *
 * These functions are related to the calculation of the Angle of Arrival
 * and Angle of Departure from I/Q samples. The angles can be calculated
 * following these steps:
 *   1. Initialize a sl_rtl_aox_libitem instance.
 *   2. Set up the antenna array and angle calculation parameters.
 *   3. Create the estimator.
 *   4. Set the antenna switching pattern.
 *   5. Input the I/Q data into the libitem.
 *   6. Process the I/Q data into an angle.
 */

/// AoX antenna array type
enum sl_rtl_aox_array_type{
  SL_RTL_AOX_ARRAY_TYPE_4x4_URA = 0, ///< Silicon Labs Ref. 4x4 Uniform Rectangular Array
  SL_RTL_AOX_ARRAY_TYPE_3x3_URA,     ///< Silicon Labs Ref. 3x3 Uniform Rectangular Array
  SL_RTL_AOX_ARRAY_TYPE_1x4_ULA,     ///< Silicon Labs Ref. 1x4 Uniform Linear Array
  SL_RTL_AOX_ARRAY_TYPE_4x4_DP_URA,  ///< Silicon Labs Ref. 4x4 Uniform Dual Polarized Rectangular Array
  SL_RTL_AOX_ARRAY_TYPE_COREHW_15x15_DP,    ///< CoreHw Ref. 150 mm x 150 mm, 8 Element Dual Polarized Array
  SL_RTL_AOX_ARRAY_TYPE_COREHW_12x12_DP,    ///< CoreHw Ref. 120 mm x 120 mm, 8 Element Dual Polarized Array

  SL_RTL_AOX_ARRAY_TYPE_LAST ///< Placeholder
};

enum sl_rtl_aox_switch_pattern_array{
  SL_RTL_AOX_SWITCH_PATTERN_ARRAY_4x4_CP = 0,
  SL_RTL_AOX_SWITCH_PATTERN_ARRAY_4x4_DP,

  SL_RTL_AOX_SWITCH_PATTERN_ARRAY_LAST
};

enum sl_rtl_aox_switch_pattern_options{
  SL_RTL_AOX_SWITCH_PATTERN_OPTIONS_DUAL_POLARIZED = 0x1,
  SL_RTL_AOX_SWITCH_PATTERN_OPTIONS_EXTRA_REFERENCE = 0x2
};

/// AoA, AoD (and AoX for backward compatibility) estimator modes
enum sl_rtl_aox_mode {
  SL_RTL_AOA_MODE_ONE_SHOT_BASIC = 3, ///< Medium filtering, medium response. Returns 2D angle, requires 10 rounds. Most suitable for single shot measurement.
  SL_RTL_AOA_MODE_ONE_SHOT_BASIC_LIGHTWEIGHT, ///< Medium filtering, medium response, low CPU cost & low elevation resolution. 2D angle, req. 10 rounds. Most suitable for single shot measurement.
  SL_RTL_AOA_MODE_ONE_SHOT_FAST_RESPONSE, ///< Low filtering, fast response, low CPU cost & low elevation resolution. 2D angle, requires 2 rounds. Most suitable for single shot measurement.
  SL_RTL_AOA_MODE_ONE_SHOT_HIGH_ACCURACY, ///< High filtering, slow response. 2D angle, requires 20 rounds. Most suitable for single shot measurement.

  SL_RTL_AOA_MODE_ONE_SHOT_BASIC_AZIMUTH_ONLY, ///< Equivalent to ONE_SHOT_BASIC with low CPU cost and returns 1D angle. Most suitable for single shot measurement.
  SL_RTL_AOA_MODE_ONE_SHOT_FAST_RESPONSE_AZIMUTH_ONLY, ///< Equivalent to ONE_SHOT_FAST_RESPONSE with low CPU cost, 1D angle. Most suitable for single shot measurement.
  SL_RTL_AOA_MODE_ONE_SHOT_HIGH_ACCURACY_AZIMUTH_ONLY, ///< Equivalent to ONE_SHOT_HIGH_ACCURACY with low CPU cost, 1D angle. Most suitable for single shot measurement.

  SL_RTL_AOA_MODE_REAL_TIME_FAST_RESPONSE, ///< Low filtering, fast response, lowest CPU cost, 2D angle, Most suitable for real-time tracking.
  SL_RTL_AOA_MODE_REAL_TIME_BASIC, ///< Medium filtering, medium response, medium CPU cost, 2D angle, Most suitable for real-time tracking.
  SL_RTL_AOA_MODE_REAL_TIME_HIGH_ACCURACY, ///< High filtering, slow response, highest CPU cost, 2D angle, Most suitable for real-time tracking.

  SL_RTL_AOA_MODE_LAST, ///< Placeholder

  /***************************************************************************/

  SL_RTL_AOD_MODE_ONE_SHOT_BASIC = 3 | 0x20, ///< Medium filtering, medium response. Returns 2D angle, requires 10 rounds. Most suitable for single shot measurement.
  SL_RTL_AOD_MODE_ONE_SHOT_BASIC_LIGHTWEIGHT, ///< Medium filtering, medium response, low CPU cost & low elevation resolution. 2D angle, req. 10 rounds. Most suitable for single shot measurement.
  SL_RTL_AOD_MODE_ONE_SHOT_FAST_RESPONSE, ///< Low filtering, fast response, low CPU cost & low elevation resolution. 2D angle, requires 2 rounds. Most suitable for single shot measurement.
  SL_RTL_AOD_MODE_ONE_SHOT_HIGH_ACCURACY, ///< High filtering, slow response. 2D angle, requires 20 rounds. Most suitable for single shot measurement.

  SL_RTL_AOD_MODE_ONE_SHOT_BASIC_AZIMUTH_ONLY, ///< Equivalent to ONE_SHOT_BASIC with low CPU cost and returns 1D angle. Most suitable for single shot measurement.
  SL_RTL_AOD_MODE_ONE_SHOT_FAST_RESPONSE_AZIMUTH_ONLY, ///< Equivalent to ONE_SHOT_FAST_RESPONSE with low CPU cost, 1D angle. Most suitable for single shot measurement.
  SL_RTL_AOD_MODE_ONE_SHOT_HIGH_ACCURACY_AZIMUTH_ONLY, ///< Equivalent to ONE_SHOT_HIGH_ACCURACY with low CPU cost, 1D angle. Most suitable for single shot measurement.

  SL_RTL_AOD_MODE_REAL_TIME_FAST_RESPONSE, ///< Low filtering, fast response, lowest CPU cost, 2D angle, Most suitable for real-time tracking.
  SL_RTL_AOD_MODE_REAL_TIME_BASIC, ///< Medium filtering, medium response, medium CPU cost, 2D angle, Most suitable for real-time tracking.
  SL_RTL_AOD_MODE_REAL_TIME_HIGH_ACCURACY, ///< High filtering, slow response, highest CPU cost, 2D angle, Most suitable for real-time tracking.

  SL_RTL_AOD_MODE_LAST, ///< Placeholder

  /***************************************************************************/

  SL_RTL_AOX_MODE_ONE_SHOT_BASIC = 3 | 0x40, ///< Medium filtering, medium response. Returns 2D angle, requires 10 rounds. Most suitable for single shot measurement.
  SL_RTL_AOX_MODE_ONE_SHOT_BASIC_LIGHTWEIGHT, ///< Medium filtering, medium response, low CPU cost & low elevation resolution. 2D angle, req. 10 rounds. Most suitable for single shot measurement.
  SL_RTL_AOX_MODE_ONE_SHOT_FAST_RESPONSE, ///< Low filtering, fast response, low CPU cost & low elevation resolution. 2D angle, requires 2 rounds. Most suitable for single shot measurement.
  SL_RTL_AOX_MODE_ONE_SHOT_HIGH_ACCURACY, ///< High filtering, slow response. 2D angle, requires 20 rounds. Most suitable for single shot measurement.

  SL_RTL_AOX_MODE_ONE_SHOT_BASIC_AZIMUTH_ONLY, ///< Equivalent to ONE_SHOT_BASIC with low CPU cost and returns 1D angle. Most suitable for single shot measurement.
  SL_RTL_AOX_MODE_ONE_SHOT_FAST_RESPONSE_AZIMUTH_ONLY, ///< Equivalent to ONE_SHOT_FAST_RESPONSE with low CPU cost, 1D angle. Most suitable for single shot measurement.
  SL_RTL_AOX_MODE_ONE_SHOT_HIGH_ACCURACY_AZIMUTH_ONLY, ///< Equivalent to ONE_SHOT_HIGH_ACCURACY with low CPU cost, 1D angle. Most suitable for single shot measurement.

  SL_RTL_AOX_MODE_REAL_TIME_FAST_RESPONSE, ///< Low filtering, fast response, lowest CPU cost, 2D angle, Most suitable for real-time tracking.
  SL_RTL_AOX_MODE_REAL_TIME_BASIC, ///< Medium filtering, medium response, medium CPU cost, 2D angle, Most suitable for real-time tracking.
  SL_RTL_AOX_MODE_REAL_TIME_HIGH_ACCURACY, ///< High filtering, slow response, highest CPU cost, 2D angle, Most suitable for real-time tracking.

  SL_RTL_AOX_MODE_LAST ///< Placeholder
};

// AoX constraint types
enum sl_rtl_aox_constraint_type{
  SL_RTL_AOX_CONSTRAINT_TYPE_AZIMUTH = 0, ///< Azimuth constraint in degrees
  SL_RTL_AOX_CONSTRAINT_TYPE_ELEVATION, ///< Elevation constraint in degrees
};

// AoX switch pattern modes
enum sl_rtl_aox_switch_pattern_mode{
  SL_RTL_AOX_SWITCH_PATTERN_MODE_DEFAULT = 0, ///< Internally defined switch pattern: 0, 1, 2, ..., N-1, where N is the number of antennas
  SL_RTL_AOX_SWITCH_PATTERN_MODE_RANDOM, ///< Internally defined random switch pattern
  SL_RTL_AOX_SWITCH_PATTERN_MODE_EXTERNAL, ///< Switch pattern set externally by the user
  SL_RTL_AOX_SWITCH_PATTERN_MODE_EXTRA_REFERENCE, ///< Switch pattern set externally by the user with extra reference antenna as a first
};

/// Angle of Arrival / Departure library item
typedef void* sl_rtl_aox_libitem;

// Structure for IQ sample quality calculation antenna-specific results
typedef struct {
  float level;             ///< Antenna signal level, in decibels
  float snr;               ///< Antenna level signal to noise ratio, in decibels
  float phase_value;       ///< Antenna's average unrotated phase value in the packet, in radians
  float phase_jitter;      ///< Phase variation of snapshots' data of an antenna in packet, in radians
} sl_rtl_clib_iq_sample_qa_antenna_data_t;

// Structure for IQ sample quality calculation results
typedef struct {
  bool data_available;     ///< If false, all the antenna values are undefined
  uint32_t curr_channel;   ///< Radio channel for the last packet
  float ref_freq;          ///< Apparent supplemental tone frequency
  float ref_sndr;          ///< Reference period signal to noise and distortion ratio

  // available only when raw samples are used
  float switching_jitter;  ///< Estimated antenna switching clock jitter
} sl_rtl_clib_iq_sample_qa_dataset_t;

// return codes from the individual IQ sample QA tests
#define  SL_RTL_AOX_IQ_SAMPLE_QA_ALL_OK         0
#define SL_RTL_AOX_IQ_SAMPLE_QA_FAILURE         0xffffffff
typedef enum {
  SL_RTL_AOX_IQ_SAMPLE_QA_INVAL_REF            = 0,  ///< Invalid reference period data
  SL_RTL_AOX_IQ_SAMPLE_QA_DCOFFSET             = 2,  ///< DC offset too large
  SL_RTL_AOX_IQ_SAMPLE_QA_SNDR                 = 3,  ///< Reference period SNDR too large
  SL_RTL_AOX_IQ_SAMPLE_QA_ROTATING_ERROR       = 4,  ///< Rotation error too large
  SL_RTL_AOX_IQ_SAMPLE_QA_REF_ANT_PHASE_VALUE  = 5,  ///< Reference antenna phase value too big
  SL_RTL_AOX_IQ_SAMPLE_QA_REF_ANT_PHASE_JITTER = 6,  ///< Reference antenna phase jitter too large
  SL_RTL_AOX_IQ_SAMPLE_QA_ANT_X_PHASE_JITTER   = 7,  ///< Antenna X phase jitter too large
  SL_RTL_AOX_IQ_SAMPLE_QA_ALL_SAME_PHASE       = 8,  ///< All the antennas seen in the same phase
  SL_RTL_AOX_IQ_SAMPLE_QA_SWICHING_JITTER      = 9,  ///< Switching jitter too large
} sl_rtl_slib_iq_sample_qa_result_t;

#define SL_RTL_AOX_IQ_SAMPLE_QA_CLEAR_BIT(code, bit) \
  (code &= ~(1UL << bit))
#define SL_RTL_AOX_IQ_SAMPLE_QA_SET_BIT(code, bit) \
  (code |= (1UL << bit))
#define SL_RTL_AOX_IQ_SAMPLE_QA_IS_SET(code, bit) \
  (code & (1UL << bit))

/**************************************************************************//**
 * Initialize the AoX libitem instance.
 *
 * @param[in] item Pointer to the libitem to be initialized.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_init(sl_rtl_aox_libitem* item);

/**************************************************************************//**
 * Deinitialize a libitem instance of the AoX estimator.
 *
 * @param[in] item Pointer to the libitem to be deinitialized.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_deinit(sl_rtl_aox_libitem* item);

/* NARROWBAND AOA ESTIMATION */

/**************************************************************************//**
 * Set the number of signal snapshots to be used in the angle estimation.
 *
 * @param[in] item Pointer to the initialized AoX libitem.
 * @param[in] num_snapshots Number of snapshots as positive integer value.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_set_num_snapshots(sl_rtl_aox_libitem* item, uint32_t num_snapshots);

/**************************************************************************//**
 * Set the array type used with the estimator.
 *
 * @param[in] item Pointer to the initialized AoX libitem.
 * @param[in] array_type Array type as ::sl_rtl_aox_array_type.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * Set the array type used with the estimator.
 * For example, the array type should be set to ::SL_RTL_AOX_ARRAY_TYPE_4x4_URA
 * when using the reference 4x4 URA board.
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_set_array_type(sl_rtl_aox_libitem* item, enum sl_rtl_aox_array_type array_type);

/**************************************************************************//**
 * Set the estimation mode.
 *
 * @param[in] item Pointer to the initialized AoX libitem.
 * @param[in] mode Estimator mode as ::sl_rtl_aox_mode.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * Set the estimation mode. For example, ::SL_RTL_AOX_MODE_BASIC sets medium filtering
 * and estimates both azimuth and elevation. For further description of the modes,
 * see the documentation of ::sl_rtl_aox_mode.
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_set_mode(sl_rtl_aox_libitem* item, enum sl_rtl_aox_mode mode);

/**************************************************************************//**
 * Estimate the I/Q-sample phase rotation error in degrees.
 *
 * @param[in] item Pointer to the initialized AoX libitem.
 * @param[in] iq_data_downsampling_factor Ratio between reference period IQ-data sampling rate.
 * and actual IQ-data (i.e., antenna array data) sampling rate.
 *            For example, 1e6 / 500e3 = 2.0
 * @param[in] i_samples Float-array of the reference period I samples.
 * @param[in] q_samples Float-array of the reference period Q samples.
 * @param[in] num_samples Number of samples, or size of the I or Q sample array.
 * @param[out] phase_rotation_out Returned phase rotation value as float in degrees.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * Estimate the I/Q-sample phase rotation error caused by switching
 * and CTE frequency error based on I/Q samples from the reference period.
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_calculate_iq_sample_phase_rotation(sl_rtl_aox_libitem* item, float iq_data_downsampling_factor, float* i_samples, float* q_samples, uint32_t num_samples, float* phase_rotation_out);

/**************************************************************************//**
 * Set a constant value of I/Q sample phase rotation in degrees.
 *
 * @param[in] item Pointer to the initialized AoX libitem.
 * @param[in] phase_rotation Float value of I/Q sample phase rotation in degrees.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * Set a constant value of I/Q sample phase rotation in degrees.
 * The sample rotation can be used to correct the switching and CTE frequency error.
 * @note: This function should be called only after the estimator is
 * created.
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_set_iq_sample_phase_rotation(sl_rtl_aox_libitem* item, float phase_rotation);

/**************************************************************************//**
 * Add constraints for the estimator. Call before sl_rtl_aox_create_estimator.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] constraint_type Select which constraints should be added.
 * @param[in] min_value Starting (minimum) value of the range including the min_value.
 * @param[in] max_value Ending (maximum) value of the range including the max_value.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * For example, constraint for SL_RTL_AOX_CONSTRAINT_TYPE_AZIMUTH with min_value = 0
 * and max_value = 90 means that the angular range 0 to 90 degrees is excluded
 * from the estimators internal processing range and angle estimated between that range
 * are not considered as valid results. Setting constraints will help ruling
 * out false multipath-detections when the locator is installed nearby a wall or
 * a RF-reflective surface.
 * @note: This function must called before the estimator is created.
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_add_constraint(sl_rtl_aox_libitem* item, enum sl_rtl_aox_constraint_type constraint_type, float min_value, float max_value);

/**************************************************************************//**
 * Configure the IQ sampling sample rate for the library.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] sampleRate The new sample rate.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_set_sample_rate(sl_rtl_aox_libitem* item, float sampleRate);

/**************************************************************************//**
 * Configure the number of radio channels.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] channels The new number of radio channels.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_set_num_radio_channels(sl_rtl_aox_libitem* item, uint32_t channels);

/**************************************************************************//**
 * Configure the IQ sample quality analysis.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * This function turns the IQ sample quality analysis,
 * which is not done by default.
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_iq_sample_qa_configure(sl_rtl_aox_libitem* item);

/**************************************************************************//**
 * Get the IQ sample quality analysis overall results.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @return bitmask of found problems, zero indicates that everything is OK
 *****************************************************************************/
uint32_t sl_rtl_aox_iq_sample_qa_get_results(sl_rtl_aox_libitem* item);

/**************************************************************************//**
 * Get the IQ sample quality analysis detailed results for the latest packet.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[out] results The data structure with data related to the latest data packet.
 * @param[out] antenna_data The array of antenna-specific results.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * Fetch the results for the latest packet.
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_iq_sample_qa_get_details(sl_rtl_aox_libitem* item, sl_rtl_clib_iq_sample_qa_dataset_t *results, sl_rtl_clib_iq_sample_qa_antenna_data_t *antenna_data);

/**************************************************************************//**
 * Get the IQ sample quality analysis detailed results for the requested radio channel.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] channel Radio channel to show results for.
 * @param[out] results The data structure with data related to the last data packet using the requested channel.
 * @param[out] antenna_data The array of antenna-specific results
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * Fetch the latest results for the given radio channel, which may be other than the latest
 * packet received.
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_iq_sample_qa_get_channel_details(sl_rtl_aox_libitem* item, uint8_t channel, sl_rtl_clib_iq_sample_qa_dataset_t *results, sl_rtl_clib_iq_sample_qa_antenna_data_t *antenna_data);

/**************************************************************************//**
 * Create the estimator after initializing the libitem and setting parameters.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_create_estimator(sl_rtl_aox_libitem* item);

/**************************************************************************//**
 * Convert data in RAW IQ-data buffers to downsampled IQ-data buffers.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] start_offset The start offset in the buffer (for example, for skipping a reference period).
 * @param[in] iq_data_downsampling_factor Ratio between chip IQ-data sampling rate and downsampled rate.
 *            For example, 4.8e6 / 500e3 = 9.6
 * @param[in] raw_i_samples_in I-part sample buffer input
 * @param[in] raw_q_samples_in Q-part sample buffer input
 * @param[in] num_raw_samples_in Number of IQ-sample pairs in the input buffers.
 * @param[out] i_samples_out Buffer for the processed I-samples. Must be allocated by the user.
 *             Indexing: i_samples_out[snapshot][antenna]
 * @param[out] q_samples_out Buffer for the processed Q-samples. Must be allocated by the user.
 *             Indexing: q_samples_out[snapshot][antenna]
 * @param[in] num_snapshots_out Number of snaphots allocated in the output buffers. This function
 *            checks if the given number of snaphots and calculated snapshots based on the length of RAW-data match
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_convert_raw_samples(sl_rtl_aox_libitem* item, uint32_t start_offset, float iq_data_downsampling_factor, float* raw_i_samples_in, float* raw_q_samples_in, uint32_t num_raw_samples_in, float** i_samples_out, float** q_samples_out, uint32_t num_snapshots_out);

/**************************************************************************//**
 * Calculate the number of downsampled snaphots in a RAW IQ-data buffer. Use this function to get the number
 * of snaphots to allocate the i_samples and q_samples buffers for the process-function.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] num_raw_samples_in Total number of RAW IQ-sample pairs.
 * @param[in] start_offset The start offset in the buffer, for example, for skipping a reference period.
 * @param[in] iq_data_downsampling_factor Ratio between chip IQ-data sampling rate and downsampled rate.
 *            For example, 4.8e6 / 500e3 = 9.6.
 * @param[in] num_channels Number of channels in the RAW data
 * @param[out] num_snapshots_out Calculated number of snaphots based on the input.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_calculate_number_of_snapshots(sl_rtl_aox_libitem* item, uint32_t num_raw_samples_in, uint32_t start_offset, float iq_data_downsampling_factor, uint32_t num_channels, uint32_t* num_snapshots_out);

/**************************************************************************//**
 * Set switch pattern mode. Sets internal mode used by the library. See enum sl_rtl_aox_switch_pattern_mode
 * for detailed description of the modes. If this function isn't called, a default switch pattern
 * of: 0, 1, 2, 3, ..., N-1 will be used, where N is the number of antennas.
 * Call this function at run-time after initializing and creating the estimator.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] mode Required mode.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_set_switch_pattern_mode(sl_rtl_aox_libitem* item, enum sl_rtl_aox_switch_pattern_mode mode);

/**************************************************************************//**
 * Update switch pattern, which is used by the estimator algorithm.
 * This function must be called before calling the function sl_rtl_aox_process.
 * Call this function at run-time after initializing and creating the estimator.
 *
 * When using a custom pattern in SL_RTL_AOX_SWITCH_PATTERN_MODE_EXTERNAL mode,
 * the pattern in switch_pattern_in must not contain duplicate elements and the
 * antenna indices must be within the range 0 to (number_of_antennas - 1).
 * Number_of_antennas is the amount of antennas in the set array type, for example
 * for the 4x4 URA this is 16 and for the 1x4 ULA this is 4.
 * The length of the switching pattern must be equal to number_of_antennas.
 *
 * Note: The RF switch pattern indices may be different to the indices required by
 * the RTL algorithm. For example, a 1x4 ULA might use RF controls [12, 13, 14, 15]
 * for switching the antennas 1-4, but the RTL algorithm requires the switch pattern
 * [0, 1, 2, 3]. If the RF controls are switched as [14, 15, 13, 12],
 * the RTL algorithm requires the pattern [2, 3, 1, 0].
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] switch_pattern_in Pointer to the switch pattern array. The pointer must point to a valid switch pattern when
 * SL_RTL_AOX_SWITCH_PATTERN_MODE_EXTERNAL is used.
 * @param[out] switch_pattern_out Pointer to the user defined uint32_t* variable. This pointer must be non-zero when using
 * internally defined switch pattern modes, otherwise can be zero.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_update_switch_pattern(sl_rtl_aox_libitem* item, uint32_t* switch_pattern_in, uint32_t** switch_pattern_out);

/**************************************************************************//**
 * Set the random seed for the switch pattern, which is used for the random pattern mode.
 * Call this function at run-time after initializing and creating the estimator.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] seed_value The seed value to be used.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_set_switch_pattern_seed(sl_rtl_aox_libitem* item, int32_t seed_value);

/**************************************************************************//**
 * Convert switching pattern from antenna indices to antenna switching pin control logic.
 * Call this function at run-time after initializing and creating the estimator.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] array_id Array ID, see ::sl_rtl_aox_switch_pattern_array, for example SL_RTL_AOX_SWITCH_PATTERN_ARRAY_4x4_DP
 * @param[in] options Switch pattern options, see ::sl_rtl_aox_switch_pattern_options.
 *            Can be used to set for example dual polarization (default is single polarization).
 * @param[in] switch_pattern_size_in The length of the input switching pattern.
 * @param[in] switch_pattern_in The input switching pattern.
 * @param[out] switch_pattern_size_out The outputted length of the converted switching pattern.
 * @param[out] switch_pattern_out The outputted switching pattern.
 * @return ::SL_RTL_ERROR_SUCCESS if successful

 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_convert_switch_pattern(sl_rtl_aox_libitem* item, uint32_t array_id, uint32_t options, uint32_t switch_pattern_size_in, uint32_t* switch_pattern_in, uint32_t* switch_pattern_size_out, uint32_t** switch_pattern_out);

/**************************************************************************//**
 * Reset estimator state. Calling this function causes the selected aox-mode to start from its initial state.
 * Call this function at run-time after initializing and creating the estimator.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_reset_estimator(sl_rtl_aox_libitem* item);

/**************************************************************************//**
 * Calculate the angle estimate.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] i_samples Two-dimensional float-array of captured I samples
 *  as i_samples[snapshot][antenna].
 * @param[in] q_samples Two-dimensional float-array of captured Q samples
 * as q_samples[snapshot][antenna], corresponding to the I samples array.
 * @param[in] tone_frequency The frequency of the signal from which the
 *  I/Q data was captured from as float (e.g., 2.46e9f).
 * @param[out] az_out Output azimuth angle.
 * @param[out] el_out Output elevation angle.
 * @return ::SL_RTL_ERROR_SUCCESS if successful and
 *   ::SL_RTL_ERROR_ESTIMATION_IN_PROGRESS if estimate is not yet final
 *   and more I/Q data needs to be processed.
 *
 * Calculate the angle estimate from the given I/Q samples captured at the
 * given frequency. Call this function with new I/Q data as many times as
 * indicated by the ::sl_rtl_aox_mode used by the estimator before the final
 * estimate is output.
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_process(sl_rtl_aox_libitem* item, float** i_samples, float** q_samples, float tone_frequency, float* az_out, float* el_out);

/**************************************************************************//**
 * Get the standard deviation for the latest AoA/AoD-estimate.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[out] az_std_dev Pointer for getting standard deviation of the
 *                         latest azimuth estimate.
 * @param[out] el_std_dev Pointer for getting standard deviation of the
 *                         latest elevation estimate.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * Call this function after sl_rtl_aox_process to fetch standard
 * deviation for the latest AoA/AoD-estimate. Positive standard deviations
 * indicate line-of-sight detection and negative values indicate likely
 * non-line-of-sight detection.
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_get_latest_aox_standard_deviation(sl_rtl_aox_libitem* item, float* az_std_dev, float* el_std_dev);

/**************************************************************************//**
 * Kept for backward compatibility
 *****************************************************************************/
static inline enum sl_rtl_error_code sl_rtl_aox_get_latest_aoa_standard_deviation(sl_rtl_aox_libitem* item, float* az_std_dev, float* el_std_dev)
{
  return sl_rtl_aox_get_latest_aox_standard_deviation(item, az_std_dev, el_std_dev);
}

/**************************************************************************//**
 * Kept for backward compatibility
 *****************************************************************************/
static inline enum sl_rtl_error_code sl_rtl_aox_get_latest_aod_standard_deviation(sl_rtl_aox_libitem* item, float* az_std_dev, float* el_std_dev)
{
  return sl_rtl_aox_get_latest_aox_standard_deviation(item, az_std_dev, el_std_dev);
}

/**************************************************************************//**
 * Feed the expected angles back to the locator.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] expected_az Expected azimuth angle calculated by position algorithm.
 * @param[in] expected_el Expected elevation angle calculated by position algorithm.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * The position algorithm has a more complete view of the asset's location.
 * The direction it should be found can be calculated back and fed to the
 * locator, so that it can recover faster when,
 * for example, it has locked to a reflection rather than to the line of sight signal.
 * See also sl_rtl_loc_get_expected_direction().
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_set_expected_direction(sl_rtl_aox_libitem* item, float expected_az, float expected_el);

/**************************************************************************//**
 * Feed the expected angle deviations back to the locator.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @param[in] deviation_az Deviation of the expected azimuth angle calculated by position algorithm.
 * @param[in] deviation_el Deviation of the expected elevation angle calculated by position algorithm.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * Report expected deviations calculated by the position algorithm back to
 * the locator so that angles are calculated more accurately taking in account
 * the correctness of the expected directions. If sl_rtl_aox_set_expected_direction()
 * is called but this function is not called, the algorithm will use default values
 * for the expected deviations.
 * See also sl_rtl_loc_get_expected_deviation() and sl_rtl_aox_set_expected_direction().
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_set_expected_deviation(sl_rtl_aox_libitem* item, float deviation_az, float deviation_el);

/**************************************************************************//**
 * Clear the expected directions and deviations from the locator.
 *
 * @param[in] item Pointer to the initialized and configured AoX libitem.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *
 * See also sl_rtl_aox_set_expected_direction() and sl_rtl_aox_set_expected_deviation().
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_clear_expected_direction(sl_rtl_aox_libitem* item);

/**************************************************************************//**
 * Enable or disable outputting the pseudospectrum. This function should be called
 * before creating the estimator.
 *
 * @param[in] item Pointer to the initialized libitem.
 * @param[in] enable Set to true to enable outputting the pseudospectrum.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_enable_spectrum(sl_rtl_aox_libitem* item, bool enable);

/**************************************************************************//**
 * Get the size of the pseudospectrum. This function should be called after calling the AoX process
 * function.
 *
 * @param[in] item Pointer to the initialized libitem.
 * @param[out] rows Number of rows in the pseudospectrum data array.
 * @param[out] cols Number of columns in the pseudospecutrm data array.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_get_spectrum_size(sl_rtl_aox_libitem* item, uint32_t* rows, uint32_t* cols);

/**************************************************************************//**
 * Get the size of the polarization pseudospectrum.
 * This function should be called after calling the AoX process
 * function.
 *
 * @param[in] item Pointer to the initialized libitem.
 * @param[out] rows Number of rows in the polarization pseudospectrum data array.
 * @param[out] cols Number of columns in the pseudospecutrm data array.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_get_polarization_spectrum_size(sl_rtl_aox_libitem* item, uint32_t* rows, uint32_t* cols);

/**************************************************************************//**
 * Get the pseudospectrum. Memory for the output spectrum must be allocated by the
 * user, the size of the rows and columns are given by sl_rtl_aox_get_spectrum_size().
 *
 * @param[in] item Pointer to the initialized libitem.
 * @param[out] spectrum_out Outputted pseudospectrum.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_get_spectrum(sl_rtl_aox_libitem* item, float** spectrum_out);

/**************************************************************************//**
 * Get the polarization spectrum. Memory for the output spectrum must be allocated by the
 * user, the size of the rows and columns are given by sl_rtl_aox_get_polarization_spectrum_size().
 *
 * @param[in] item Pointer to the initialized libitem.
 * @param[out] spectrum_out Outputted polarization pseudospectrum.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_get_polarization_spectrum(sl_rtl_aox_libitem* item, float** spectrum_out);

/// Angle of Arrival / Departure antenna array radiation pattern
typedef void* sl_rtl_aox_antenna_pattern;

/**************************************************************************//**
 * Initialize the antenna radiation pattern item for the given array board.
 * The antenna pattern item can be shared by multiple different estimators.
 *
 * @param[in] pattern Pointer to the pattern item to initialize.
 * @param[in] array_type Array type of the board for which the pattern is loaded.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_antenna_pattern_init(sl_rtl_aox_antenna_pattern* pattern, enum sl_rtl_aox_array_type array_type);

/**************************************************************************//**
 * Set the given estimator to use the given antenna radiation pattern in the
 * model. The same antenna pattern can be shared between multiple estimators.
 * The antenna pattern can be cleared for an estimator by setting it to NULL.
 * The estimator will then use the default model.
 * @note: This function should be called only after the estimator is
 * created.
 *
 * @param[in] item Pointer to the initialized libitem.
 * @param[in] pattern Pointer to the initialized antenna pattern item.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_set_antenna_pattern(sl_rtl_aox_libitem* item, sl_rtl_aox_antenna_pattern* pattern);

/**************************************************************************//**
 * Deinitialize the antenna radiation pattern item.
 *
 * @param[in] pattern Pointer to the initialized pattern item.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_aox_antenna_pattern_deinit(sl_rtl_aox_antenna_pattern* pattern);

/** @} */ // end addtogroup sl_rtl_aox

/**
 * @addtogroup sl_rtl_loc Location Finding
 * @{
 *
 * @brief Location Finding
 *
 * These functions are related to the calculation of positions from angles
 * and distances.
 */

#define SL_RTL_LOC_ALL_LOCATORS (uint32_t)(-1)
#define SL_RTL_LOC_ALL_TAGS INT32_MAX

/// Locator-specific measurements
enum sl_rtl_loc_locator_measurement_field{
  SL_RTL_LOC_LOCATOR_MEASUREMENT_AZIMUTH,   ///< Measured azimuth from locator to tag
  SL_RTL_LOC_LOCATOR_MEASUREMENT_ELEVATION, ///< Measured elevation from locator to tag
  SL_RTL_LOC_LOCATOR_MEASUREMENT_DISTANCE,  ///< Measured distance from locator to tag

  SL_RTL_LOC_LOCATOR_MEASUREMENT_AZIMUTH_DEVIATION,   ///< Azimuth stdev
  SL_RTL_LOC_LOCATOR_MEASUREMENT_ELEVATION_DEVIATION, ///< Elevation stdev
  SL_RTL_LOC_LOCATOR_MEASUREMENT_DISTANCE_DEVIATION,  ///< Distance stdev

  SL_RTL_LOC_LOCATOR_LAST
};

/// Target-specific measurements - Not yet supported
enum sl_rtl_loc_target_measurement_field{
  SL_RTL_LOC_TARGET_MEASUREMENT_LAST
};

/// Estimation mode
enum sl_rtl_loc_estimation_mode{
  SL_RTL_LOC_ESTIMATION_MODE_TWO_DIM_FAST_RESPONSE, ///< Two-dimensional mode - Only X and Y plane considered, less filtered mode for fast moving assests
  SL_RTL_LOC_ESTIMATION_MODE_THREE_DIM_FAST_RESPONSE, ///< Three-dimensional mode - Covers X, Y and Z planes, less filtered mode for fast moving assests

  SL_RTL_LOC_ESTIMATION_MODE_TWO_DIM_HIGH_ACCURACY, ///< Two-dimensional mode - Only X and Y plane considered, more filtered mode for relatively static assets
  SL_RTL_LOC_ESTIMATION_MODE_THREE_DIM_HIGH_ACCURACY, ///< Three-dimensional mode - Covers X, Y and Z planes, more filtered mode for relatively static assets

  SL_RTL_LOC_ESTIMATION_MODE_LAST
};

/// Measurement validation method
enum sl_rtl_loc_measurement_validation_method{
  SL_RTL_LOC_MEASUREMENT_VALIDATION_MINIMUM,  ///< Only the basic validation integrated in the locationing algorithm (default)
  SL_RTL_LOC_MEASUREMENT_VALIDATION_MEDIUM,   ///< May discard the most inaccurate measurements with an additional calculation round
  SL_RTL_LOC_MEASUREMENT_VALIDATION_FULL,     ///< May discard the most inaccurate measurements with several calculation rounds
};

/// Locator-specific parameters related to locationing
enum sl_rtl_loc_locator_parameter {
  SL_RTL_LOC_LOCATOR_PARAMETER_X_COORDINATE,  ///< Set X-axis coordinate of the locator
  SL_RTL_LOC_LOCATOR_PARAMETER_Y_COORDINATE,  ///< Set Y-axis coordinate of the locator
  SL_RTL_LOC_LOCATOR_PARAMETER_Z_COORDINATE,  ///< Set Z-axis coordinate of the locator
  SL_RTL_LOC_LOCATOR_PARAMETER_X_ORIENTATION, ///< Set X-axis rotation of the locator (Euler angles) in degrees
  SL_RTL_LOC_LOCATOR_PARAMETER_Y_ORIENTATION, ///< Set Y-axis rotation of the locator (Euler angles) in degrees
  SL_RTL_LOC_LOCATOR_PARAMETER_Z_ORIENTATION, ///< Set Z-axis rotation of the locator (Euler angles) in degrees

  SL_RTL_LOC_LOCATOR_PARAMETER_LAST
};

/// Target-specific parameters related to locationing
enum sl_rtl_loc_target_parameter{
  SL_RTL_LOC_TARGET_PARAMETER_TARGET_HEIGHT, // Z-position when the target is statically on some x-y-plane
  SL_RTL_LOC_TARGET_CLEAR_TARGET_HEIGHT,     // Clear the previously set Z-position

  SL_RTL_LOC_TARGET_PARAMETER_LAST
};

/// Locationing state results
enum sl_rtl_loc_result{
  SL_RTL_LOC_RESULT_POSITION_X = 0, ///< Estimated X-axis position of the target
  SL_RTL_LOC_RESULT_POSITION_Y, ///< Estimated Y-axis position of the target
  SL_RTL_LOC_RESULT_POSITION_Z, ///< Estimated Z-axis position of the target

  SL_RTL_LOC_RESULT_POSITION_RADIUS, ///< The combined radius (stdev) of the location estimate
  SL_RTL_LOC_RESULT_POSITION_STDEV_X, ///< The accuracy (stdev) of the location x-coordinate estimate
  SL_RTL_LOC_RESULT_POSITION_STDEV_Y, ///< The accuracy (stdev) of the location y-coordinate estimate
  SL_RTL_LOC_RESULT_POSITION_STDEV_Z, ///< The accuracy (stdev) of the location z-coordinate estimate

  SL_RTL_LOC_RESULT_POSITION_VELOCITY_X, ///< Estimated X-axis velocity of the target
  SL_RTL_LOC_RESULT_POSITION_VELOCITY_Y, ///< Estimated Y-axis velocity of the target
  SL_RTL_LOC_RESULT_POSITION_VELOCITY_Z, ///< Estimated Z-axis velocity of the target
  SL_RTL_LOC_RESULT_POSITION_ACCELERATION_X, ///< Estimated X-axis acceleration of the target
  SL_RTL_LOC_RESULT_POSITION_ACCELERATION_Y, ///< Estimated Y-axis acceleration of the target
  SL_RTL_LOC_RESULT_POSITION_ACCELERATION_Z, ///< Estimated Z-axis acceleration of the target

  SL_RTL_LOC_RESULT_LAST ///< Number of results
};

/// Locator item which contains the position and orientation of the locator array
struct sl_rtl_loc_locator_item{
  float coordinate_x; ///< X-axis coordinate of the locator
  float coordinate_y; ///< Y-axis coordinate of the locator
  float coordinate_z; ///< Z-axis coordinate of the locator
  float orientation_x_axis_degrees; ///< X-axis rotation of the locator (Euler angles) in degrees
  float orientation_y_axis_degrees; ///< Y-axis rotation of the locator (Euler angles) in degrees
  float orientation_z_axis_degrees; ///< Z-axis rotation of the locator (Euler angles) in degrees
};

/// Locationing library item
typedef void* sl_rtl_loc_libitem;

// Initialization functions

/**************************************************************************//**
 * Initialize the locationing libitem instance.
 *
 * @param[in] item Pointer to the libitem to be initialized.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_init(sl_rtl_loc_libitem* item);

/**************************************************************************//**
 * Deinitialize the locationing libitem instance.
 *
 * @param[in] item Pointer to the libitem to be initialized.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_deinit(sl_rtl_loc_libitem* item);

/**************************************************************************//**
 * Reinitialize the locationing libitem instance.
 *
 * Reset the libitem's internal values to the starting point so that it can
 * start all over from the beginning. This can be used, for example, in testing
 * instead of deleting and re-creating the libitem object.
 *
 * @param[in] item Pointer to the libitem to be initialized.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_reinit(sl_rtl_loc_libitem* item);

/**************************************************************************//**
 * Set the locationing dimensionality mode. Possible choices are 2D or 3D
 * modes. Two-dimensional mode does not vary the z-position of the target and
 * assumes it is 0 at all times. When updating, for example, the distance
 * measurement from the locator to the target in 2D mode, note that
 * even if the locator and tag are on different z-planes, the distance should
 * be as if they were on the same z-plane.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] mode Estimation mode as enum.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_set_mode(sl_rtl_loc_libitem* item, enum sl_rtl_loc_estimation_mode mode);

/**************************************************************************//**
 * Set the measurement validation method. If not given, the minimum method is
 * used. Possible choices are minimum, medium, and full. The minimum does not
 * increase the calculation CPU load because it is integrated inside the algorithm's
 * normal operation. The medium and full methods use additionally one or
 * multiple calculation rounds respectively and may discard the most
 * inaccurate measurements from the calculation. However, they will always leave
 * enough of them to calculate the estimate.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] method Validation method as enum.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_set_measurement_validation(sl_rtl_loc_libitem* item, enum sl_rtl_loc_measurement_validation_method method);

/**************************************************************************//**
 * Add a locator item into the locationing estimator after setting its position
 * and orientation parameters. Add all locators using this
 * function before calling ::sl_rtl_loc_create_position_estimator to create
 * the estimator instance.
 *
 * @param[in] item Pointer to the libitem
 * @param[in] locator_item Locator item instance to be added
 * @param[out] locator_id_out ID of the locator assigned by the library
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_add_locator(sl_rtl_loc_libitem* item, struct sl_rtl_loc_locator_item* locator_item, uint32_t* locator_id_out);

/**************************************************************************//**
 * Add an asset tag item into the locationing estimator.
 *
 * @param[in] item Pointer to the libitem.
 * @param[out] tag_id_out ID of the tag assigned by the library.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_add_tag(sl_rtl_loc_libitem* item, int32_t *tag_id_out);

/**************************************************************************//**
 * Remove an asset tag item previously added with sl_rtl_loc_add_tag.
 *
 * @param[in] item Pointer to the libitem.
 * @param[out]  ID of the tag to be removed.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_remove_tag(sl_rtl_loc_libitem* item, int32_t tag_id);

/**************************************************************************//**
 * Create the position estimator instance after all locators have been added.
 *
 * @param[in] item Pointer to the libitem.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_create_position_estimator(sl_rtl_loc_libitem* item);

// Processing functions

/**************************************************************************//**
 * Set locator-specific parameters, such as the azimuth and elevation
 * measurement covariances.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of the locator for which the parameter will be set.
 * @param[in] parameter Parameter to be set (as enum).
 * @param[in] value New value for the parameter.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_set_locator_parameter(sl_rtl_loc_libitem* item, uint32_t locator_id, enum sl_rtl_loc_locator_parameter parameter, float value);

/**************************************************************************//**
 * Set target-specific parameters, such as the known target height.

 *
 * @param[in] item Pointer to the libitem.
 * @param[in] parameter Parameter to be set (as enum).
 * @param[in] value New value for the parameter.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_set_target_parameter(sl_rtl_loc_libitem* item, enum sl_rtl_loc_target_parameter parameter, float value);

/**************************************************************************//**
 * Set target-specific parameters, such as the known target height for a specified tag.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] parameter Parameter to be set (as enum).
 * @param[in] value New value for the parameter.
 * @param[in] tag_id TagID of the asset to be changed or ::SL_RTL_LOC_ALL_TAGS.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_set_target_parameter_tag(sl_rtl_loc_libitem* item, enum sl_rtl_loc_target_parameter parameter, float value, int32_t tag_id);

/**************************************************************************//**
 * Clear previous measurements. Call this function before setting
 * new measurements if the previous measurements are not valid for the next
 * iteration step.
 *
 * @param[in] item Pointer to the libitem.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_clear_measurements(sl_rtl_loc_libitem* item);

/**************************************************************************//**
 * Clear previous measurements for a specified tag. Call this function before setting
 * new measurements if the previous measurements are not valid for the next
 * iteration step.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] tag_id TagID of the asset to be changed or ::SL_RTL_LOC_ALL_TAGS.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_clear_measurements_tag(sl_rtl_loc_libitem* item, int32_t tag_id);

/**************************************************************************//**
 * Set a new measurement for a locator specified by the locator ID.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of the locator for which the measurement will be set.
 * @param[in] field Measurement to be updated (as enum).
 * @param[in] value New value for the measurement.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_set_locator_measurement(sl_rtl_loc_libitem* item, uint32_t locator_id, enum sl_rtl_loc_locator_measurement_field field, float value);

/**************************************************************************//**
 * Set a new measurement for a locator specified by the locator ID for a specified tag.
 *
 * @param[in] item Pointer to the libitem
 * @param[in] locator_id ID of the locator for which the measurement will be set
 * @param[in] field Measurement to be updated (as enum)
 * @param[in] value New value for the measurement
 * @param[in] tag_id TagID of the asset to be changed
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_set_locator_measurement_tag(sl_rtl_loc_libitem* item, uint32_t locator_id, enum sl_rtl_loc_locator_measurement_field field, float value, int32_t tag_id);

/**************************************************************************//**
 * Set a new measurement for the target. Because the current version does not support this
 * function, calling it has no effect.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] field Measurement to be updated (as enum).
 * @param[in] value New value for the measurement.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_set_target_measurement(sl_rtl_loc_libitem* item, enum sl_rtl_loc_target_measurement_field field, float value);

/**************************************************************************//**
 * Process the current time step of the locationing filter. Call this function
 * after old measurements are cleared and new
 * measurements are set for the current time step. The process will step
 * the filter forward and provide new state results and state covariance
 * values, which can be retrieved with separate function calls.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] time_step Process time interval in seconds. This is the time
 * separation between the previous step and this current step.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_process(sl_rtl_loc_libitem* item, float time_step);

/**************************************************************************//**
 * Process the current time step of the locationing filter for a specified tag.
 * Call this function after old measurements are cleared and new
 * measurements are set for the current time step. The process will step
 * the filter forward and provide new state results and state covariance
 * values, which can be retrieved with separate function calls.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] time_step Process time interval in seconds. This is the time
 * separation between the previous step and this current step.
 * @param[in] tag_id TagID of the asset to be processed or ::SL_RTL_LOC_ALL_TAGS.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_process_tag(sl_rtl_loc_libitem* item, float time_step, int32_t tag_id);

/**************************************************************************//**
 * Get result of the current state variables or state covariances.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] result The desired state variable result as enum.
 * @param[out] value The value output of the state variable.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_get_result(sl_rtl_loc_libitem* item, enum sl_rtl_loc_result result, float* value);

/**************************************************************************//**
 * Get result of the current state variables or state covariances for a specified tag.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] result The desired state variable result as enum.
 * @param[out] value The value output of the state variable.
 * @param[in] tag_id TagID of the asset to fetch the results for.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_get_result_tag(sl_rtl_loc_libitem* item, enum sl_rtl_loc_result result, float* value, int32_t tag_id);

/**************************************************************************//**
 * Get the latest measurement converted into the coordinate system of the
 * multi-locator system.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of the locator whose measurement will be retrieved.
 * @param[in] field Measurement to be retrieved (as enum).
 * @param[out] value_out The value output of the state variable.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_get_measurement_in_system_coordinates(sl_rtl_loc_libitem* item, uint32_t locator_id, enum sl_rtl_loc_locator_measurement_field field, float* value_out);

/**************************************************************************//**
 * Get latest measurement converted into the coordinate system of the
 * multi-locator system for a specified tag.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of the locator whose measurement will be retrieved.
 * @param[in] field Measurement to be retrieved (as enum).
 * @param[out] value_out The value output of the state variable.
 * @param[in] tag_id TagID of the asset.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_get_measurement_in_system_coordinates_tag(sl_rtl_loc_libitem* item, uint32_t locator_id, enum sl_rtl_loc_locator_measurement_field field, float* value_out, int32_t tag_id);

/**************************************************************************//**
 * Get the expected direction of the asset for the locator.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of the locator whose measurement will be retrieved.
 * @param[out] azimuth The expected azimuth value.
 * @param[out] elevation The expected elevation value.
 * @param[out] distance The expected distance value.
 * @return ::SL_RTL_ERROR_SUCCESS if the locator does not require correction.
 * @return ::SL_RTL_ERROR_INCORRECT_MEASUREMENT if the locator's previous measurement was ignored because the error is too large.
 * @returns Another error code in case of error
 *
 * Position calculation has a better overall view to the asset's location than
 * any individual locator. If the direction angles differ too much from
 * the expected direction, the locator can be instructed to correct its
 * internal state so that in can recover from this incorrectness faster.
 * See also sl_rtl_aox_set_expected_direction().
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_get_expected_direction(sl_rtl_loc_libitem* item, uint32_t locator_id, float *azimuth, float *elevation, float *distance);

/**************************************************************************//**
 * Get the expected direction of the asset for the locator for a specified tag.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of the locator whose measurement will be retrieved.
 * @param[out] azimuth The expected azimuth value.
 * @param[out] elevation The expected elevation value.
 * @param[out] distance The expected distance value.
 * @param[in] tag_id TagID of the asset.
 * @return ::SL_RTL_ERROR_SUCCESS if the locator does not require correction.
 * @return ::SL_RTL_ERROR_INCORRECT_MEASUREMENT if the locator's previous measurement was ignored because of too large error
 * @returns Another error code in case of error
 *
 * Position calculation has a better overall view to the asset's location than
 * any individual locator. If the direction angles differ too much from
 * the expected direction, the locator can be instructed to correct its
 * internal state so that in can recover from this incorrectness faster.
 * See also sl_rtl_aox_set_expected_direction()
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_get_expected_direction_tag(sl_rtl_loc_libitem* item, uint32_t locator_id, float *azimuth, float *elevation, float *distance, int32_t tag_id);

/**************************************************************************//**
 * Get the deviation values for expected direction of the asset for the locator.
 *
 * @param[in] locator_id ID of the locator whose measurement will be retrieved.
 * @param[out] azimuth The expected azimuth value's standard deviation.
 * @param[out] elevation The expected elevation value's standard deviation.
 * @param[in] item Pointer to the libitem.
 * @param[out] distance The expected distance value's standard deviation.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_get_expected_deviation(sl_rtl_loc_libitem* item, uint32_t locator_id, float *azimuth, float *elevation, float *distance);

/**************************************************************************//**
 * Get the deviation values for expected direction of the asset for the locator
 * for a specified tag.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of the locator whose measurement will be retrieved.
 * @param[out] azimuth The expected azimuth value's standard deviation.
 * @param[out] elevation The expected elevation value's standard deviation.
 * @param[out] distance The expected distance value's standard deviation.
 * @param[in] tag_id TagID of the asset.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_get_expected_deviation_tag(sl_rtl_loc_libitem* item, uint32_t locator_id, float *azimuth, float *elevation, float *distance, int32_t tag_id);

/**************************************************************************//**
 * Get the number of locators, which are disabled from position calculation because
 * the direction error is too large.
 *
 * @param[in] item Pointer to the libitem.
 * @returns Number of locators needing correction, or -1 in case of any other error
 *****************************************************************************/
int sl_rtl_loc_get_number_disabled(sl_rtl_loc_libitem* item);

/**************************************************************************//**
 * Get the number of locators, which are disabled from position calculation because
 * the direction error is too large, for a specified tag.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] tag_id TagID of the asset.
 * @returns Number of locators needing correction, or -1 in case of any other error
 *****************************************************************************/
int sl_rtl_loc_get_number_disabled_tag(sl_rtl_loc_libitem* item, int32_t tag_id);

/**************************************************************************//**
 * Check if the asset is in reach of a given locator according to the
 * position-based filtering.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of the locator to run the test.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
bool sl_rtl_loc_filter_in_reach(sl_rtl_loc_libitem* item, uint32_t locator_id);

/**************************************************************************//**
 * Clear all the position-based filters from a locator or from all
 * locators.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of a single locator or ::SL_RTL_LOC_ALL_LOCATORS.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_filter_clear(sl_rtl_loc_libitem* item, uint32_t locator_id);

/**************************************************************************//**
 * Add a position-based filter. This filter is based on the distance from a locator.
 * The filter can be added to one locator or all locators. A locator may also
 * have several combined filters.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of a single locator or ::SL_RTL_LOC_ALL_LOCATORS.
 * @param[in] radius Distance from the locator.
 * @param[in] exclude_region If true, define an excluded region instead.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_filter_sphere(sl_rtl_loc_libitem* item, uint32_t locator_id, float radius, bool exclude_region);

/**************************************************************************//**
 * Add a position-based filter. The two dimensional filter is based on
 * the distance from a locator.
 * The filter can be added to one locator or all locators. A locator may  also
 * have several combined filters.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of a single locator or ::SL_RTL_LOC_ALL_LOCATORS.
 * @param[in] radius Distance from the locator.
 * @param[in] exclude_region If true, define an excluded region instead.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_filter_circle(sl_rtl_loc_libitem* item, uint32_t locator_id, float radius, bool exclude_region);

/**************************************************************************//**
 * Add a position-based filter. This filter defines relative distances in
 * coordinate axes' directions.
 * The filter can be added to one locator or all locators. A locator may also
 * have several combined filters.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of a single locator or ::SL_RTL_LOC_ALL_LOCATORS.
 * @param[in] xDelta Maximum distance in X-coordinate direction.
 * @param[in] yDelta Maximum distance in Y-coordinate direction.
 * @param[in] zDelta Maximum distance in Z-coordinate direction.
 * @param[in] exclude_region If true, define an excluded region instead.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_filter_box(sl_rtl_loc_libitem* item, uint32_t locator_id, float xDelta, float yDelta, float zDelta, bool exclude_region);

/**************************************************************************//**
 * Add a position-based filter. This two dimensional filter defines relative
 * distances in coordinate axes' directions.
 * The filter can be added to one locator or all locators. A locator may also
 * have several combined filters.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of a single locator or ::SL_RTL_LOC_ALL_LOCATORS.
 * @param[in] xDelta Maximum distance in X-coordinate direction.
 * @param[in] yDelta Maximum distance in Y-coordinate direction.
 * @param[in] exclude_region If true, define an excluded region instead.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_filter_rect(sl_rtl_loc_libitem* item, uint32_t locator_id, float xDelta, float yDelta, bool exclude_region);

/**************************************************************************//**
 * Add a position-based filter. This filter defines an area in global
 * coordinates.
 * The filter can be added to one locator or all locators. A locator may also
 * have several combined filters.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of a single locator or ::SL_RTL_LOC_ALL_LOCATORS.
 * @param[in] minX Minimum X-coordinate value.
 * @param[in] maxX Maximum X-coordinate value.
 * @param[in] minY Minimum Y-coordinate value.
 * @param[in] maxY Maximum Y-coordinate value.
 * @param[in] minZ Minimum Z-coordinate value.
 * @param[in] maxZ Maximum Z-coordinate value.
 * @param[in] exclude_region If true, define an excluded region instead.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_filter_room(sl_rtl_loc_libitem* item, uint32_t locator_id, float minX, float maxX, float minY, float maxY, float minZ, float maxZ, bool exclude_region);

/**************************************************************************//**
 * Add a position-based filter. The two dimensional filter defines an area in
 * global coordinates.
 * The filter can be added to one locator or all locators. A locator may also
 * have several combined filters.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] locator_id ID of a single locator or ::SL_RTL_LOC_ALL_LOCATORS.
 * @param[in] minX Minimum X-coordinate value.
 * @param[in] maxX Maximum X-coordinate value.
 * @param[in] minY Minimum Y-coordinate value.
 * @param[in] maxY Maximum Y-coordinate value.
 * @param[in] exclude_region If true, define an excluded region instead.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_filter_floor(sl_rtl_loc_libitem* item, uint32_t locator_id, float minX, float maxX, float minY, float maxY, bool exclude_region);

/**************************************************************************//**
 * Enable trilateration method in location calculation.
 *
 * The trilateration method calculates the position based on the distances
 * from the locators rather than the direction angles. This function enables
 * the method, but the location calculation may still use the
 * direction if available to further increase the accuracy.
 *
 * Trilateration method is not turned on by default.
 *
 * @param[in] item Pointer to the libitem.
 * @param[in] value Turn the trilateration on or off.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_loc_enable_trilateration(sl_rtl_loc_libitem* item, bool value);

/** @} */ // end addtogroup sl_rtl_loc

/**
 * @addtogroup sl_rtl_util Utility Functionality
 * @{
 *
 * @brief Utility Functionality
 *
 * These functions provide supporting functionality such as conversions,
 * additional filtering, diagnostics, and setup.
 */

/// Utility library item
typedef void* sl_rtl_util_libitem;

/// Util parameter
enum sl_rtl_util_parameter{
  SL_RTL_UTIL_PARAMETER_AMOUNT_OF_FILTERING = 0 ///< Amount of filtering in range [0.0f, 1.0f]
};

/**************************************************************************//**
 * Initialize the Util libitem instance.
 *
 * @param[in] item Pointer to the libitem to be initialized.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_init(sl_rtl_util_libitem* item);

/**************************************************************************//**
 * Deinitialize a Util libitem instance.
 *
 * @param[in] item Pointer to the libitem to be deinitialized.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_deinit(sl_rtl_util_libitem* item);

/**************************************************************************//**
 * Set util parameters.
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[in] parameter Parameter to change.
 * @param[in] value Value of the parameter.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_set_parameter(sl_rtl_util_libitem* item, enum sl_rtl_util_parameter parameter, float value);

/**************************************************************************//**
 * Filter a value using the moving average.
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[in] value_in Value to be fed to the filter.
 * @param[out] value_out Pointer to the filtered value.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_filter(sl_rtl_util_libitem* item, float value_in, float* value_out);

/**************************************************************************//**
 * Convert an RSSI-value to distance in meters.
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[in] tx_power Reference RSSI value of the TX-device at 1.0 m distance in dBm, for example -45.0f.
 * @param[in] rssi Measured RSSI from the receiver.
 * @param[out] distance_out Distance in meters.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_rssi2distance(float tx_power, float rssi, float* distance_out);

/**************************************************************************//**
 * Set up the IQ sample quality analysis.
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[in] level Analysis level.
 * @param[in] num_antennas The number of antennas in the array.
 * @param[in] raw_samples Data contains raw samples instead of picked ones.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_iq_sample_qa_setup(sl_rtl_util_libitem *item, uint32_t level, uint32_t num_antennas, bool raw_samples);

/**************************************************************************//**
 * Set up parameters for the IQ sample quality analysis.
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[in] sample_rate The sampling rate.
 * @param[in] sample_offset The offset for the picked sample in the sampling slot.
 * @param[in] samples_in_slot Number of samples in the sampling slot.
 * @param[in] number_of_channels Number of radio channels.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_iq_sample_qa_set_parameters(sl_rtl_util_libitem* item, uint32_t sample_rate, uint8_t sample_offset, uint8_t samples_in_slot, uint8_t number_of_channels);

/**************************************************************************//**
 * Set up IQ sample QA downsampling ratio.
 *
 * Set the downsampling factor used during the antenna samples (related to the
 * sampling rate of the reference period).
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[in] dsf The downsampling factor.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_iq_sample_qa_set_downsampling_factor(sl_rtl_util_libitem* item, float dsf);

/**************************************************************************//**
 * Set data samples offset.
 *
 * Set the offset for the data samples from the start of the reference
 * period. The use of this function is optional and, if not given,
 * the default values are used.
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[in] offset The data samples' offset.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_iq_sample_qa_set_data_offset(sl_rtl_util_libitem* item, uint32_t offset);

/**************************************************************************//**
 * Set the switch pattern for the IQ sample quality.
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[in] size The size of the switch pattern.
 * @param[in] pattern Array of integers representing the switch pattern.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_iq_sample_qa_set_switch_pattern(sl_rtl_util_libitem* item, uint32_t size, uint32_t *pattern);

/**************************************************************************//**
 * Feed the IQ sample quality analysis the reference period data.
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[in] num_samples The size of data.
 * @param[in] i_samples I-part of the sample data.
 * @param[in] q_samples Q-part of the sample data.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_iq_sample_qa_add_reference(sl_rtl_util_libitem *item, uint32_t num_samples, float *i_samples, float *q_samples);
/**************************************************************************//**
 * Feed the IQ sample quality analysis the antenna data.
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[in] num_snapshots The size of the data.
 * @param[in] i_samples I-part of the sample data.
 * @param[in] q_samples Q-part of the sample data.
 * @param[in] channel Radio channel the packet is using.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_iq_sample_qa_add_data(sl_rtl_util_libitem *item, uint32_t num_snapshots, float **i_samples, float **q_samples, uint8_t channel);

/**************************************************************************//**
 * Get the overall results of the IQ sample quality analysis.
 *
 * @param[in] item Pointer to the initialized Util libitem.

 * @return Bitmask of the failing tests, zero if everything passed
 *****************************************************************************/
uint32_t sl_rtl_util_iq_sample_qa_get_results(sl_rtl_util_libitem *item);

/**************************************************************************//**
 * Get the detailed results of the IQ sample quality analysis.
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[out] iq_sample_qa_results The data structure with data related to the previous data packet.
 * @param[out] antenna_data The array of antenna-specific results.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_iq_sample_qa_get_details(sl_rtl_util_libitem *item, sl_rtl_clib_iq_sample_qa_dataset_t *iq_sample_qa_results, sl_rtl_clib_iq_sample_qa_antenna_data_t *antenna_data);

/**************************************************************************//**
 * Get the detailed results of the IQ sample quality analysis for the specified radio channel.
 *
 * @param[in] item Pointer to the initialized Util libitem.
 * @param[in] channel Radio channel to show results for.
 * @param[out] iq_sample_qa_results The data structure with data related to the last data packet using the requested channel.
 * @param[out] antenna_data The array of antenna specific results.
 * @return ::SL_RTL_ERROR_SUCCESS if successful
 *****************************************************************************/
enum sl_rtl_error_code sl_rtl_util_iq_sample_qa_get_channel_details(sl_rtl_util_libitem* item, uint8_t channel, sl_rtl_clib_iq_sample_qa_dataset_t *iq_sample_qa_results, sl_rtl_clib_iq_sample_qa_antenna_data_t *antenna_data);

/**************************************************************************//**
 * Write the IQ sample quality analysis code as human readable strings in the provided buffer.
 *
 * @param[in] buf Buffer for the results to be written.
 * @param[in] size Size of the buffer.
 * @param[in] code The results previously received from the sl_rtl_util_iq_sample_qa_get_results().
 * @returns pointer to the buffer containing data, or to a constant string
 *****************************************************************************/
char *sl_rtl_util_iq_sample_qa_code2string(char *buf, int size, uint32_t code);

/** @} */ // end addtogroup sl_rtl_util

#endif /*SL_RTL_CLIB_API_H*/
