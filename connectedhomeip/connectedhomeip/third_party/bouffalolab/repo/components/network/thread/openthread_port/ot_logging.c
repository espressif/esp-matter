#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <openthread/config.h>
#include <openthread/platform/logging.h>
#include <openthread_port.h>


void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    va_list argp;

    va_start(argp, aFormat);
    ot_uartLog(aFormat, argp);
    va_end(argp);
}



void otPlatLogLine(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aLogLine)
{
    otPlatLog(aLogLevel, aLogRegion, "%s\r\n", aLogLine);
}
