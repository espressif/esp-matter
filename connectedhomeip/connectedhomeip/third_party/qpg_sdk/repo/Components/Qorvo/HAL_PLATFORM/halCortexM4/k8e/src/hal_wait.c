#include "hal.h"
#include "hal_defs.h"
/** Wait a number of us.
*/
void hal_Waitus(UInt16 us)
{
    UInt8   speed = GP_WB_READ_STANDBY_PRESCALE_UCCORE();
    UInt32  waitLoops;

    // Convert wait duration to number of delay loops.
    // This is only correct when executing from flash. Timing for RAM/ROM may be different.
    switch (speed)
    {
        case GP_WB_ENUM_CLOCK_SPEED_M64:
            // 64 MHz CPU freq
            waitLoops = ((UInt32)us) * 16;
            break;
        default:
            // Assume 32 MHz CPU freq
            waitLoops = ((UInt32)us) * 8;
            break;
    }

    if (waitLoops > 6)
    {
        // Subtract 6 loops (approximate overhead of this function).
        waitLoops -= 6;

        // Call assembler function to execute wait loop.
        hal_wait_loop(waitLoops);
    }
}

/** Wait a number of ms.
 */
void hal_Waitms(UInt16 ms)
{
    while (ms--)
    {
        hal_Waitus(1000);
    }
}
