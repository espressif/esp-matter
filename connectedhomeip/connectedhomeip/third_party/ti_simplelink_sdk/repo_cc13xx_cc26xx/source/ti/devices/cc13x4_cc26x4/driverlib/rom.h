/******************************************************************************
*  Filename:       rom.h
*  Revised:        $Date$
*  Revision:       $Revision$
*
*  Description:    Prototypes for the ROM utility functions.
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

#ifndef __ROM_H__
#define __ROM_H__

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

#include "../inc/hw_types.h"

#ifndef __HAPI_H__
#define __HAPI_H__

// Start address of the ROM hard API access table (located after the ROM FW rev field)
#define ROM_HAPI_TABLE_ADDR 0x10000048

// ROM Hard-API function interface types
typedef uint32_t     (* FPTR_CRC32_T)              ( uint8_t*     /* pui8Data        */,\
                                                     uint32_t     /* ui32ByteCount   */,\
                                                     uint32_t     /* ui32RepeatCount */);

typedef uint32_t     (* FPTR_GETFLSIZE_T)          ( void );

typedef uint32_t     (* FPTR_GETCHIPID_T)          ( void );

typedef uint32_t     (* FPTR_SECTORERASE_T)        ( uint32_t     /* ui32FlashAddr   */);

typedef uint32_t     (* FPTR_RESERVED2_T)          ( void );

typedef uint32_t     (* FPTR_PROGFLASH_T)          ( uint8_t*     /* pui8RamData     */,\
                                                     uint32_t     /* ui32FlashAdr    */,\
                                                     uint32_t     /* ui32ByteCount   */);

typedef void         (* FPTR_RESETDEV_T)           ( void );

typedef uint32_t     (* FPTR_FLETCHER32_T)         ( uint16_t*    /* pui16Data       */,\
                                                     uint16_t     /* ui16WordCount   */,\
                                                     uint16_t     /* ui16RepeatCount */);

typedef uint32_t     (* FPTR_MINVAL_T)             ( uint32_t*    /* ulpDataBuffer   */,\
                                                     uint32_t     /* ui32DataCount   */);

typedef uint32_t     (* FPTR_MAXVAL_T)             ( uint32_t*    /* pui32DataBuffer */,\
                                                     uint32_t     /* ui32DataCount   */);

typedef uint32_t     (* FPTR_MEANVAL_T)            ( uint32_t*    /* pui32DataBuffer */,\
                                                     uint32_t     /* ui32DataCount   */);

typedef uint32_t     (* FPTR_STDDVAL_T)            ( uint32_t*    /* pui32DataBuffer */,\
                                                     uint32_t     /* ui32DataCount   */);

typedef void         (* FPTR_HFSOURCESAFESWITCH_T) ( void );

typedef void         (* FPTR_RESERVED4_T)          ( uint32_t                          );

typedef void         (* FPTR_RESERVED5_T)          ( uint32_t                          );

typedef void         (* FPTR_COMPAIN_T)            ( uint8_t      /* ut8Signal       */);

typedef void         (* FPTR_COMPAREF_T)           ( uint8_t      /* ut8Signal       */);

typedef void         (* FPTR_ADCCOMPBIN_T)         ( uint8_t      /* ut8Signal       */);

typedef void         (* FPTR_DACVREF_T)            ( uint8_t      /* ut8Signal       */);

// ROM Hard-API access table type
typedef struct
{
    FPTR_CRC32_T                    Crc32;
    FPTR_GETFLSIZE_T                FlashGetSize;
    FPTR_GETCHIPID_T                GetChipId;
    FPTR_SECTORERASE_T              EraseSector;
    FPTR_RESERVED2_T                ReservedLocation2;
    FPTR_PROGFLASH_T                ProgramFlash;
    FPTR_RESETDEV_T                 ResetDevice;
    FPTR_FLETCHER32_T               Fletcher32;
    FPTR_MINVAL_T                   MinValue;
    FPTR_MAXVAL_T                   MaxValue;
    FPTR_MEANVAL_T                  MeanValue;
    FPTR_STDDVAL_T                  StandDeviationValue;
    FPTR_RESERVED4_T                ReservedLocation4;
    FPTR_RESERVED5_T                ReservedLocation5;
    FPTR_HFSOURCESAFESWITCH_T       HFSourceSafeSwitch;
    FPTR_COMPAIN_T                  SelectCompAInput;
    FPTR_COMPAREF_T                 SelectCompARef;
    FPTR_ADCCOMPBIN_T               SelectADCCompBInput;
    FPTR_DACVREF_T                  SelectDACVref;
} HARD_API_T;

// Pointer to the ROM HAPI table
#define P_HARD_API                   ((HARD_API_T*) ROM_HAPI_TABLE_ADDR)

#define HapiCrc32(a,b,c)             P_HARD_API->Crc32(a,b,c)
#define HapiGetFlashSize()           P_HARD_API->FlashGetSize()
#define HapiGetChipId()              P_HARD_API->GetChipId()
#define HapiSectorErase(a)           P_HARD_API->EraseSector(a)
#define HapiProgramFlash(a,b,c)      P_HARD_API->ProgramFlash(a,b,c)
#define HapiResetDevice()            P_HARD_API->ResetDevice()
#define HapiFletcher32(a,b,c)        P_HARD_API->Fletcher32(a,b,c)
#define HapiMinValue(a,b)            P_HARD_API->MinValue(a,b)
#define HapiMaxValue(a,b)            P_HARD_API->MaxValue(a,b)
#define HapiMeanValue(a,b)           P_HARD_API->MeanValue(a,b)
#define HapiStandDeviationValue(a,b) P_HARD_API->StandDeviationValue(a,b)
#define HapiHFSourceSafeSwitch()     P_HARD_API->HFSourceSafeSwitch()
#define HapiSelectCompAInput(a)      P_HARD_API->SelectCompAInput(a)
#define HapiSelectCompARef(a)        P_HARD_API->SelectCompARef(a)
#define HapiSelectADCCompBInput(a)   P_HARD_API->SelectADCCompBInput(a)
#define HapiSelectDACVref(a)         P_HARD_API->SelectDACVref(a)

// Defines for input parameter to the HapiSelectCompAInput function.
#define COMPA_IN_NC            0x00
// Defines used in CC13x0/CC26x0 devices
#define COMPA_IN_AUXIO7        0x09
#define COMPA_IN_AUXIO6        0x0A
#define COMPA_IN_AUXIO5        0x0B
#define COMPA_IN_AUXIO4        0x0C
#define COMPA_IN_AUXIO3        0x0D
#define COMPA_IN_AUXIO2        0x0E
#define COMPA_IN_AUXIO1        0x0F
#define COMPA_IN_AUXIO0        0x10
// Defines used in CC13x2/CC26x2 devices
#define COMPA_IN_AUXIO26       COMPA_IN_AUXIO7
#define COMPA_IN_AUXIO25       COMPA_IN_AUXIO6
#define COMPA_IN_AUXIO24       COMPA_IN_AUXIO5
#define COMPA_IN_AUXIO23       COMPA_IN_AUXIO4
#define COMPA_IN_AUXIO22       COMPA_IN_AUXIO3
#define COMPA_IN_AUXIO21       COMPA_IN_AUXIO2
#define COMPA_IN_AUXIO20       COMPA_IN_AUXIO1
#define COMPA_IN_AUXIO19       COMPA_IN_AUXIO0

// Defines for input parameter to the HapiSelectCompARef function.
#define COMPA_REF_NC           0x00
#define COMPA_REF_DCOUPL       0x01
#define COMPA_REF_VSS          0x02
#define COMPA_REF_VDDS         0x03
#define COMPA_REF_ADCVREFP     0x04
// Defines used in CC13x0/CC26x0 devices
#define COMPA_REF_AUXIO7       0x09
#define COMPA_REF_AUXIO6       0x0A
#define COMPA_REF_AUXIO5       0x0B
#define COMPA_REF_AUXIO4       0x0C
#define COMPA_REF_AUXIO3       0x0D
#define COMPA_REF_AUXIO2       0x0E
#define COMPA_REF_AUXIO1       0x0F
#define COMPA_REF_AUXIO0       0x10
// Defines used in CC13x2/CC26x2 devices
#define COMPA_REF_AUXIO26      COMPA_REF_AUXIO7
#define COMPA_REF_AUXIO25      COMPA_REF_AUXIO6
#define COMPA_REF_AUXIO24      COMPA_REF_AUXIO5
#define COMPA_REF_AUXIO23      COMPA_REF_AUXIO4
#define COMPA_REF_AUXIO22      COMPA_REF_AUXIO3
#define COMPA_REF_AUXIO21      COMPA_REF_AUXIO2
#define COMPA_REF_AUXIO20      COMPA_REF_AUXIO1
#define COMPA_REF_AUXIO19      COMPA_REF_AUXIO0

// Defines for input parameter to the HapiSelectADCCompBInput function.
#define ADC_COMPB_IN_NC        0x00
#define ADC_COMPB_IN_DCOUPL    0x03
#define ADC_COMPB_IN_VSS       0x04
#define ADC_COMPB_IN_VDDS      0x05
// Defines used in CC13x0/CC26x0 devices
#define ADC_COMPB_IN_AUXIO7    0x09
#define ADC_COMPB_IN_AUXIO6    0x0A
#define ADC_COMPB_IN_AUXIO5    0x0B
#define ADC_COMPB_IN_AUXIO4    0x0C
#define ADC_COMPB_IN_AUXIO3    0x0D
#define ADC_COMPB_IN_AUXIO2    0x0E
#define ADC_COMPB_IN_AUXIO1    0x0F
#define ADC_COMPB_IN_AUXIO0    0x10
// Defines used in CC13x2/CC26x2 devices
#define ADC_COMPB_IN_AUXIO26   ADC_COMPB_IN_AUXIO7
#define ADC_COMPB_IN_AUXIO25   ADC_COMPB_IN_AUXIO6
#define ADC_COMPB_IN_AUXIO24   ADC_COMPB_IN_AUXIO5
#define ADC_COMPB_IN_AUXIO23   ADC_COMPB_IN_AUXIO4
#define ADC_COMPB_IN_AUXIO22   ADC_COMPB_IN_AUXIO3
#define ADC_COMPB_IN_AUXIO21   ADC_COMPB_IN_AUXIO2
#define ADC_COMPB_IN_AUXIO20   ADC_COMPB_IN_AUXIO1
#define ADC_COMPB_IN_AUXIO19   ADC_COMPB_IN_AUXIO0

// Defines for input parameter to the HapiSelectDACVref function.
// The define values can not be changed!
#define DAC_REF_NC             0x00
#define DAC_REF_DCOUPL         0x01
#define DAC_REF_VSS            0x02
#define DAC_REF_VDDS           0x03

#endif // __HAPI_H__

//*****************************************************************************
//
// Pointers to the main API tables.
//
//*****************************************************************************
#define ROM_API_TABLE           ((uint32_t *) 0x10000180)
#define ROM_VERSION             (ROM_API_TABLE[0])


#define ROM_API_FLASH_TABLE ((uint32_t*) (ROM_API_TABLE[10]))

// FLASH FUNCTIONS
#define ROM_FlashPowerModeSet \
    ((void (*)(uint32_t ui32PowerMode, uint32_t ui32BankGracePeriod, uint32_t ui32PumpGracePeriod)) \
    ROM_API_FLASH_TABLE[0])

#define ROM_FlashPowerModeGet \
    ((uint32_t (*)(void)) \
    ROM_API_FLASH_TABLE[1])

#define ROM_FlashProtectionSet \
    ((void (*)(uint32_t ui32SectorAddress, uint32_t ui32ProtectMode)) \
    ROM_API_FLASH_TABLE[2])

#define ROM_FlashProtectionGet \
    ((uint32_t (*)(uint32_t ui32SectorAddress)) \
    ROM_API_FLASH_TABLE[3])

#define ROM_FlashProtectionSave \
    ((uint32_t (*)(uint32_t ui32SectorAddress)) \
    ROM_API_FLASH_TABLE[4])

#define ROM_FlashSectorErase \
    ((uint32_t (*)(uint32_t ui32SectorAddress)) \
    ROM_API_FLASH_TABLE[5])

#define ROM_FlashProgram \
    ((uint32_t (*)(uint8_t *pui8DataBuffer, uint32_t ui32Address, uint32_t ui32Count)) \
    ROM_API_FLASH_TABLE[6])

#define ROM_FlashEfuseReadRow \
    ((bool (*)(uint32_t *pui32EfuseData, uint32_t ui32RowAddress)) \
    ROM_API_FLASH_TABLE[8])

#define ROM_FlashDisableSectorsForWrite \
    ((void (*)(void)) \
    ROM_API_FLASH_TABLE[9])

#define ROM_FlashProgram4X \
    ((uint32_t (*)(uint8_t *pui8DataBuffer, uint32_t ui32Address, uint32_t ui32Count)) \
    ROM_API_FLASH_TABLE[10])



//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __ROM_H__
