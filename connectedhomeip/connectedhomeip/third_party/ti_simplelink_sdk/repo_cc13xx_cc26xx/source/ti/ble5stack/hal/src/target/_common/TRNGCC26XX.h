/******************************************************************************

 @file  TRNGCC26XX.h

 @brief This file contains header information for a True Random
        Number Generator driver.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/** ============================================================================
 *  @defgroup TRNGCC26XX TRNG driver
 *  @brief TRNG driver implementation for a CC26XX device
 *
 *  @{
 *
 *  # Driver Include #
 *  The TRNG header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/trng/TRNGCC26XX.h>
 *  @endcode
 *
 *  # Overview #
 *
 *  The TRNGCC26XX driver provides reentrant access to the TRNG module within
 *  the CC26XX.  Reentrant access is controlled by creating a global critical
 *  section when requesting a random number.  This critical section disables all
 *  Hwi's from running.
 *
 * ## General Behavior #
 * For code examples, see the use cases below.
 *
 *  ### Initialing the TRNGCC26XX Driver
 *  - TRNGCC26XX_init must be called prior to calling any other TRNGCC26xx API.
 *  - This function need only be called once per reset, but may be called
 *    multiple times without affect.
 *
 *  ### Opening the TRNGCC26XX Driver
 *  - TRNGCC26XX_open is used to open the driver prior to requesting a number.
 *  - This function powers on the TRNG peripheral.
 *  - Calling this function is optional, but will increase the latency of the
 *    first call to TRNGCC26XX_getNumber().
 *  - This function need only be called once per reset, but may be called
 *    multiple times without affect.
 *
 *  @code
 *  // Open the driver
 *  TRNGCC26XX_open(Board_TRNG);
 *  @endcode
 *
 *  ### Initializing client parameters #
 *  - Client parameters are optional.  The Driver may be called with a NULL
 *    pointer to the client parameters, in which case it will use the default
 *    parameters.  The default parameters configure the TRNG to produce 256
 *    samples per cycle at 1 clock per sample.  a call to TRNGCC26XX_getNumber
 *    with default values has a latency of 14us.  Increasing any of the
 *    configuration parameters will result in an increase in latency.
 *  - The application initializes its parameters by calling
 *    TRNGCC26XX_Params_init() and passing a pointer to a TRNGCC26XX_Params
 *    variable as an argument to it.
 *  - This function can be called once or more to each initialization of
 *    parameters.
 *  - This function can be called once or more with the same TRNGCC26XX_Params
 *    variable.
 *  - The client parameter are initialized to valid default values, but may be
 *    changed by the client after this function returns.  See trng.h for full
 *    details.
 *
 *  @code
 *  TRNGCC26XX_Params trngParams;
 *
 *  // Initialize parameters.
 *  TRNGCC26XX_Params_init(&trngParams);
 *
 *  // Optionally modify trngParams
 *  ...
 *  @endcode
 *
 *
 *  ### Validating client parameters
 *  - a client may optionally choose to validate its configuration parameters
 *    before attempting to generate a number.
 *  - when a client passes configuration parameters to TRNGCC26XX_getNumber
 *    these parameters will automatically be checked.
 *
 *  @code
 *  TRNGCC26XX_Params trngParams;
 *
 *  // Initialize parameters.
 *  TRNGCC26XX_Params_init(&trngParams);
 *
 *  // Set client parameters.
 *  trngParams.minSamplesPerCycle = 256;
 *  trngParams.maxSamplesPerCycle = 256;
 *  trngParams.clocksPerSample = 1;
 *
 *  // Validate parameters.
 *  if (TRNGCC26XX_isParamValid(&trngParams) == TRNGCC26XX_STATUS_SUCCESS)
 *  {
 *     // Parameters are valid.
 *  }
 *  @endcode
 *
 *  ### Generating a random number
 *  - The TRNG may be called without client configuration parameters and pass a
 *    NULL pointer as an argument instead.  This will result in the default values
 *    being used.
 *  - Using the initialized client configuration parameters, a random number
 *    can be generated.
 *  - an optional status flag pointer may be passed as an argument to
 *    check if the call successfully generated a random number.
 *
 *  @code
 *  TRNGCC26XX_Params trngParams;
 *  uint8_t trngStatusFlag;
 *  uint32_t trngNum1;
 *  uint32_t trngNum2;
 *
 *  // Init driver.
 *  TRNGCC26XX_init();
 *
 *  // Open TRNG peripheral.
 *  TRNGCC26XX_open(Board_TRNG);
 *
 *  // Initialize parameters.
 *  TRNGCC26XX_Params_init(&trngParams);
 *
 *  // Generate a random number.  Use optional status flag.
 *  trngNum1 = TRNGCC26XX_getNumber(&trngParams, &trngStatusFlag);
 *
 *  // Check if status was okay
 *  if (trngStatusFlag == TRNGCC26XX_STATUS_SUCCESS)
 *  {
 *    // Random number was generated successfully
 *  }
 *
 *  // Generate a second random number using the default values and no
 *  // status flag.
 *  trngNum2 = TRNGCC26XX_getNumber(NULL, NULL);
 *
 *  @endcode
 *
 *  ### Supported transaction modes #
 *  - all calls to TRNGCC26XX_getNumber are blocking until the TRNG has a new
 *    random number available.
 *
 *  ## Error handling ##
 *  If an error occurs during a call to TRNGCC26XX_getNumber, the error status
 *  will be set into the status flag and the returned random number shall be
 *  0. passing the status flag is optional.
 *
 *  ## Supported Functions ##
 *  | API function                   | Description                                       |
 *  |------------------------------- |---------------------------------------------------|
 *  | TRNGCC26XX_open()              | Open driver (stub)                                |
 *  | TRNGCC26XX_close()             | Close driver (stub)                               |
 *  | TRNGCC26XX_Params_init()       | Initialize parameters for random number generation|
 *  | TRNGCC26XX_init()              | Generates a random number                         |
 *  | TRNGCC26XX_isParamValid        | Validate client configuration parameters          |
 *
 *  ============================================================================
 *
 *  @file  TRNGCC26XX.h
 *
 *  @brief      TRNG driver implementation for a CC26XX device
 */

#ifndef ti_drivers_TRNGCC26XX__include
#define ti_drivers_TRNGCC26XX__include

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/**
 *  @addtogroup TRNG_STATUS
 *  TRNGCC26XX_STATUS_* macros are command codes only defined in the
 *  TRNGCC26XX.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/trng/TRNGCC26XX.h>
 *  @endcode
 *  @{
 */

/* Add ECCROMCC26XX_STATUS_* macros here */
#define TRNGCC26XX_STATUS_SUCCESS        0 //!< Success
#define TRNGCC26XX_STATUS_ILLEGAL_PARAM -1 //!< Illegal parameter

/** @} End TRNG_STATUS */

/*!
 * Minimum samples per cycle range, minimum value
 *
 * Minimum may be 0, or greater than or equal to 2^6 and less than 2^14.
 */
#define TRNGCC26XX_MIN_SAMPLES_MIN            64
/*!
 * Minimum samples per cycle range, maximum value
 *
 * Minimum may be 0, or greater than or equal to 2^6 and less than 2^14.
 */
#define TRNGCC26XX_MIN_SAMPLES_MAX            16384

/**
 * Maximum samples per cycle range, minimum value
 *
 * Maximum may be 0, or greater than or equal to 2^8 and less than or equal
 * to 2^24.
 */
#define TRNGCC26XX_MAX_SAMPLES_MIN            256
/**
 * Maximum samples per cycle range, maximum value
 *
 * Maximum may be 0, or greater than or equal to 2^8 and less than or equal
 * to 2^24.
 */
#define TRNGCC26XX_MAX_SAMPLES_MAX            16777216

/*!
 * Number of clocks cycles per sample must be between 1 (0) and 16 (15),
 * inclusive.
 */
#define TRNGCC26XX_CLOCKS_PER_SAMPLES_MAX     15

/*!
 * Number returned from TRNGCC26XX_getNumber when invalid parameters are used.
 *
 */
#define TRNGCC26XX_ILLEGAL_PARAM_RETURN_VALUE 0

/*!
 *  @brief      TRNGCC26XX Parameters
 *
 *  This holds the TRNG configuration parameters to be used.
 *  see the functions description of TRNGConfigure() in trng.h for
 *  more information.
 */
typedef struct TRNGCC26XX_Params
{
  uint32_t                        minSamplesPerCycle; //!< min samples per cycle
  uint32_t                        maxSamplesPerCycle; //!< max samples per cycle
  uint32_t                        clocksPerSample;    //!< clocks per sample
} TRNGCC26XX_Params;

/*!
 *  @brief      TRNGCC26XX Hardware Attributes.
 *
 *  Hardware Attribute structure for TRNG peripherals.
 */
typedef struct TRNGCC26XX_HWAttrs {
    /*! TRNG Peripheral's power manager ID */
    int        powerMngrId;
} TRNGCC26XX_HWAttrs;

/*!
 *  @brief      TRNGCC26XX Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct TRNGCC26XX_Object {
  uint8_t           state; //!< state
} TRNGCC26XX_Object;

/*! @brief TRNGCC26XX Global Configuration */
typedef struct TRNGCC26XX_Config {
    /*! Pointer to a driver specific data object */
    void                   *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void          const    *hwAttrs;
} TRNGCC26XX_Config;

/*!
 *  @brief      A handle that is returned from a TRNGCC26XX_open() call.
 */
typedef struct TRNGCC26XX_Config      *TRNGCC26XX_Handle;

/*!
 * @brief       TRNG Driver initialization.
 *
 * @pre         Calling context: Hwi, Swi or Task.
 */
void TRNGCC26XX_init(void);

/*!
 * @brief       Open the TRNGCC26XX peripheral specified by the index value.
 *              This peripheral will be configured as specified by pParams.
 *              Alternatively, if pParams is NULL, default values will be used.
 *
 * @pre         Calling context: Hwi, Swi or Task.
 *
 * @param       index  Logical peripheral Number indexed in the HWAttrs table.
 *
 * @return      a TRNGCC26XX_Handle
 */
TRNGCC26XX_Handle TRNGCC26XX_open(uint8_t index);

/*!
 * @brief       Close the TRNG driver.
 *
 * @pre         Calling context: Hwi, Swi or Task.
 *
 * @param       handle a TRNGCC26XX_Handle returned from TRNGCC26XX_open().
 */
void TRNGCC26XX_close(TRNGCC26XX_Handle handle);

/*!
 * @brief       Initialize TRNG configuration parameters to their defaults.
 *
 * @pre         Calling context: Hwi, Swi or Task.
 *
 * @param       params - TRNG configuration parameters. input parameter.
 *
 * @return      TRNGCC26XX_STATUS_SUCCESS if successful.
 *              TRNGCC26XX_STATUS_ILLEGAL_PARAM if params is NULL.
 */
int8_t TRNGCC26XX_Params_init(TRNGCC26XX_Params *params);

/*!
 * @brief       This routine returns a 32 bit TRNG number.
 *
 * @pre         params must be initialized with valid configurations.
 *              Calling context: Hwi, Swi or Task.
 *
 * @param       params - caller's configuration parameters. input parameter.
 *
 * @param       status - a user provided pointer to a status flag in case of
 *                       failure.  This parameter may be left NULL. output
 *                       parameter.
 *                       TRNGCC26XX_STATUS_SUCCESS if successful.
 *                       TRNGCC26XX_STATUS_ILLEGAL_PARAM if params is null or
 *                       configuration is illegal.
 *
 * @param       handle a TRNGCC26XX_Handle returned from @ref TRNGCC26XX_open
 *
 * @return      A 32 bit TRNG number.
 */
uint32_t TRNGCC26XX_getNumber(TRNGCC26XX_Handle handle,
                              TRNGCC26XX_Params *params, int8_t *status);

/*!
 * @brief       Check that the parameters used are valid configurations.
 *
 * @pre         params must be initialized with valid configurations.
 *              Calling context: Hwi, Swi or Task.
 *
 * @param       params - caller's configuration parameters. input parameter.
 *
 * @return      TRNGCC26XX_STATUS_SUCCESS if params is a valid configuration.
 *              TRNGCC26XX_STATUS_ILLEGAL_PARAM if params is an invalid
 *              configuration.
 */
int8_t TRNGCC26XX_isParamValid(TRNGCC26XX_Params *params);

/** @} End TRNGCC26XX */

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_TRNGCC26XX__include */
