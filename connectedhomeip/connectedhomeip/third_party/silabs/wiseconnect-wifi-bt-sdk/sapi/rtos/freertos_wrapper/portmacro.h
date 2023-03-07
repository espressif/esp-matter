#include "rsi_board_configuration.h"

#if defined(__ARMCC_VERSION)
#if defined(RSI_ARM_CM4F)
#include "RVDS/ARM_CM4F/portmacro.h"
#else
#error Unknown MCU type
#endif
#elif defined(__GNUC__) || defined(__clang__)
#if defined(RSI_ARM_CM4F)
#include "GCC/ARM_CM4F/portmacro.h"
#elif defined(RSI_ARM_CM33)
#include "GCC/ARM_CM33_NTZ/non_secure/portmacro.h"
#else
#error Unknown MCU type
#endif
#elif defined(__ICCARM__)
#if defined(RSI_ARM_CM4F)
#include "IAR/ARM_CM4F/portmacro.h"
#elif defined(RSI_ARM_CM33)
#include "IAR/ARM_CM33_NTZ/non_secure/portmacro.h"
#else
#error Unknown MCU type
#endif
#endif
