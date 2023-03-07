#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <openthread/platform/misc.h>

#include <bl_sys.h>

void otPlatReset(otInstance *aInstance) 
{
    bl_sys_reset_system();
}


otPlatResetReason otPlatGetResetReason(otInstance *aInstance)
{

    BL_RST_REASON_E rstinfo = bl_sys_rstinfo_get();

    switch (rstinfo) {
        case BL_RST_WDT:
        return OT_PLAT_RESET_REASON_WATCHDOG;
        case BL_RST_BOR:
        return OT_PLAT_RESET_REASON_OTHER;
        case BL_RST_HBN:
        return OT_PLAT_RESET_REASON_EXTERNAL;
        case BL_RST_POR:
        return OT_PLAT_RESET_REASON_EXTERNAL;
        case BL_RST_SOFTWARE:
        return OT_PLAT_RESET_REASON_SOFTWARE;
        default:
        return OT_PLAT_RESET_REASON_UNKNOWN;
    }
}

void otPlatAssertFail(const char *aFilename, int aLineNumber)
{
    printf("otPlatAssertFail, %s @ %d\r\n", aFilename, aLineNumber);
}

void otPlatWakeHost(void)
{}

otError otPlatSetMcuPowerState(otInstance *aInstance, otPlatMcuPowerState aState)
{
    return OT_ERROR_NONE;
}
otPlatMcuPowerState otPlatGetMcuPowerState(otInstance *aInstance)
{
    return OT_PLAT_MCU_POWER_STATE_ON;
}