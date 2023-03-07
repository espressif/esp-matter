/******************************************************************************
*  Filename:       aux_dac.h
*  Revised:        2021-01-25 12:15:55 +0200 (mon., 25 jan 2021)
*  Revision:       58927
*
*  Description:    Defines and prototypes for the AUX Digital-to-Analog
*                  Converter
*
*  Copyright (c) 2015 - 2021, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

//*****************************************************************************
//
//! \addtogroup aux_group
//! @{
//! \addtogroup auxdac_api
//! @{
//
//*****************************************************************************

#ifndef __AUX_DAC_H__
#define __AUX_DAC_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stdint.h>
#include "../inc/hw_types.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_adi_4_aux.h"
#include "../inc/hw_aux_anaif.h"
#include "../inc/hw_aux_sysif.h"
#include "../inc/hw_fcfg1.h"


//*****************************************************************************
//
// Defines for COMPA_IN connection to IO pins.
//
//*****************************************************************************
#define AUXDAC_COMPA_IN_NC             0x00
#define AUXDAC_COMPA_IN_AUXIO26        0x01
#define AUXDAC_COMPA_IN_AUXIO25        0x02
#define AUXDAC_COMPA_IN_AUXIO24        0x04
#define AUXDAC_COMPA_IN_AUXIO23        0x08
#define AUXDAC_COMPA_IN_AUXIO22        0x10
#define AUXDAC_COMPA_IN_AUXIO21        0x20
#define AUXDAC_COMPA_IN_AUXIO20        0x40
#define AUXDAC_COMPA_IN_AUXIO19        0x80
#define AUXDAC_COMPA_IN_AUXIO7         AUXDAC_COMPA_IN_AUXIO26
#define AUXDAC_COMPA_IN_AUXIO6         AUXDAC_COMPA_IN_AUXIO25
#define AUXDAC_COMPA_IN_AUXIO5         AUXDAC_COMPA_IN_AUXIO24
#define AUXDAC_COMPA_IN_AUXIO4         AUXDAC_COMPA_IN_AUXIO23
#define AUXDAC_COMPA_IN_AUXIO3         AUXDAC_COMPA_IN_AUXIO22
#define AUXDAC_COMPA_IN_AUXIO2         AUXDAC_COMPA_IN_AUXIO21
#define AUXDAC_COMPA_IN_AUXIO1         AUXDAC_COMPA_IN_AUXIO20
#define AUXDAC_COMPA_IN_AUXIO0         AUXDAC_COMPA_IN_AUXIO19

//*****************************************************************************
//
// Defines for DAC reference sources.
//
//*****************************************************************************
#define AUXDAC_VREF_SEL_VDDS           ADI_4_AUX_MUX2_DAC_VREF_SEL_VDDS
#define AUXDAC_VREF_SEL_ADCREF         ADI_4_AUX_MUX2_DAC_VREF_SEL_ADCREF
#define AUXDAC_VREF_SEL_DCOUPL         ADI_4_AUX_MUX2_DAC_VREF_SEL_DCOUPL
#define AUXDAC_VREF_SEL_NC             ADI_4_AUX_MUX2_DAC_VREF_SEL_NC

//*****************************************************************************
//
// API Functions and prototypes
//
//*****************************************************************************


//*****************************************************************************
//
//! \brief Sets the voltage reference source.
//!
//! The DAC's output range depends on the selection of a voltage reference
//! source. This function must be called before enabling the DAC using
//! \ref AUXDACEnable() or before setting a DAC code using
//! \ref AUXDACCalcCode().
//!
//! \param refSource
//!     DAC voltage reference source:
//!     - \ref AUXDAC_VREF_SEL_VDDS
//!     - \ref AUXDAC_VREF_SEL_ADCREF
//!     - \ref AUXDAC_VREF_SEL_DCOUPL
//!     - \ref AUXDAC_VREF_SEL_NC
//
//*****************************************************************************
extern void AUXDACSetVref(uint8_t refSource);

//*****************************************************************************
//
//! \brief Enables DAC precharge.
//!
//! If \ref AUXDAC_VREF_SEL_DCOUPL has been selected as a voltage reference
//! source, the DAC's output voltage range can be further controlled by
//! enabling or disabling precharge.
//!
//! When precharge is enabled the DAC's output voltage will range from 1.28 V
//! to 2.56 V.
//!
//! \note This function must only be called if \ref AUXDAC_VREF_SEL_DCOUPL
//!       has been selected as a voltage reference source and while the DAC is
//!       disabled, before calling \ref AUXDACEnable().
//
//*****************************************************************************
extern void AUXDACEnablePreCharge(void);

//*****************************************************************************
//
//! \brief Disables DAC precharge.
//!
//! If \ref AUXDAC_VREF_SEL_DCOUPL has been selected as a voltage reference
//! source, the DAC's output voltage range can be further controlled by
//! enabling or disabling precharge.
//!
//! When precharge is disabled the DAC's output voltage will range from 0 V
//! to 1.28 V.
//!
//! \note This function must only be called if \ref AUXDAC_VREF_SEL_DCOUPL
//!       has been selected as a voltage reference source and while the DAC is
//!       disabled, before calling \ref AUXDACEnable().
//
//*****************************************************************************
extern void AUXDACDisablePreCharge(void);

//*****************************************************************************
//
//! \brief Calculates the upper limit voltage the DAC can output.
//!
//! This function uses calibration coefficients stored in the factory
//! configuration area (FCFG1) to determine the DAC's output voltage that
//! corresponds to DAC code 255. This voltage depends on the selected voltage
//! reference source.
//!
//! \note This function must only be called after \ref AUXDACSetVref has been
//! called.
//!
//! \return
//!     The maximum voltage in mV that the DAC can output given a specific
//!     reference voltage.
//
//*****************************************************************************
extern uint32_t AUXDACCalcMax(void);

//*****************************************************************************
//
//! \brief Calculates the lower limit voltage the DAC can output.
//!
//! This function uses calibration coefficients stored in the factory
//! configuration area (FCFG1) to determine the DAC's output voltage that
//! corresponds to DAC code 1. This voltage depends on the selected voltage
//! reference source.
//!
//! DAC Code 0 corresponds to 0 V. Therefore, the DAC can only output 0 V and
//! values between the lower limit defined by this function and the upper limit
//! defined by \ref AUXDACCalcMax.
//!
//! \note This function must only be called after \ref AUXDACSetVref has been
//! called.
//!
//! \return
//!     The minimum voltage (other than 0 V) in mV that the DAC can output
//!     given a specific reference voltage.
//
//*****************************************************************************
extern uint32_t AUXDACCalcMin(void);

//*****************************************************************************
//
//! \brief Calculates the appropriate DAC code for a specific voltage value.
//!
//! Once a voltage reference source has been selected with \ref AUXDACSetVref
//! and the DAC's output voltage range has been defined with \ref AUXDACCalcMin
//! and \ref AUXDACCalcMax, a DAC code can be derived.
//!
//! \param uVoltOut
//!     The desired output voltage in millivolts [mV].
//! \param uVoltOutMin
//!     The minimum output voltage in millivolts [mV] which can be output by
//!     the DAC in the current configuration. It should be the value returned
//!     by \ref AUXDACCalcMin.
//! \param uVoltOutMax
//!     The maximum output voltage in millivolts [mV] which can be output by
//!     the DAC in the current configuration. It should be the value returned
//!     by \ref AUXDACCalcMax.
//! \return
//!     The dac code corresponding to the desired output value.
//
//*****************************************************************************
extern uint32_t AUXDACCalcCode(uint32_t uVoltOut, uint32_t uVoltOutMin, uint32_t uVoltOutMax);

//*****************************************************************************
//
//! \brief Configures and sets the DAC sample clock.
//!
//! This function determines the sample clock base frequency considering that
//! the operational rate for the DAC sample clock state machine is the AUX Bus
//! Rate.
//!
//! AUX Bus Rate divided by (dacClkDiv + 1) determines the sample clock base
//! frequency.
//!
//! \note This function must be called before \ref AUXDACEnable.
//!
//! \param dacClkDiv
//!     The clock division factor
//
//*****************************************************************************
extern void AUXDACSetSampleClock(uint8_t dacClkDiv);

//*****************************************************************************
//
//! \brief Updates the current DAC code.
//!
//! This function is meant to be used after \ref AUXDACEnable has been
//! called. It allows to update the DAC code by rapidly disabling the DAC,
//! setting the new DAC code, and then rapidly re-enabling the DAC.
//!
//! \param dacCode
//!     The new DAC code
//
//*****************************************************************************
extern void AUXDACSetCode(uint8_t dacCode);

//*****************************************************************************
//
//! \brief Enables the DAC.
//!
//! This function enables the DAC sample clock and the DAC buffer. It selects
//! COMPA_IN as output, and it routes the DAC's output to the given I/O pin.
//!
//! \param dacPinId
//!     Analog capable I/O pin:
//!     - \ref AUXDAC_COMPA_IN_AUXIO26
//!     - \ref AUXDAC_COMPA_IN_AUXIO25
//!     - \ref AUXDAC_COMPA_IN_AUXIO24
//!     - \ref AUXDAC_COMPA_IN_AUXIO23
//!     - \ref AUXDAC_COMPA_IN_AUXIO22
//!     - \ref AUXDAC_COMPA_IN_AUXIO21
//!     - \ref AUXDAC_COMPA_IN_AUXIO20
//!     - \ref AUXDAC_COMPA_IN_AUXIO19
//
//*****************************************************************************
extern void AUXDACEnable(uint8_t dacPinId);

//*****************************************************************************
//
//! \brief Disables the DAC.
//!
//! This function disables the DAC sample clock, the DAC buffer, the DAC.
//! It also disconnects COMPA_IN internally.
//!
//! \note This function must be called only after \ref AUXDACEnable has been
//! called.
//
//*****************************************************************************
extern void AUXDACDisable(void);


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __AUX_DAC_H__

//*****************************************************************************
//
//! Close the Doxygen group.
//! @}
//! @}
//
//*****************************************************************************
