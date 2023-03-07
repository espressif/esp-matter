#include "rsi_board_configuration.h"

#if defined(__ARMCC_VERSION)
#if defined(RSI_ARM_CM4F)
#include "RVDS/ARM_CM4F/port.c"
#else
#error Unknown MCU type
#endif
#elif defined(__GNUC__) || defined(__clang__)
#if defined(RSI_ARM_CM4F)
#include "GCC/ARM_CM4F/port.c"
#elif defined(RSI_ARM_CM33)
#include "GCC/ARM_CM33_NTZ/non_secure/port.c"
#else
#error Unknown MCU type
#endif
#endif
