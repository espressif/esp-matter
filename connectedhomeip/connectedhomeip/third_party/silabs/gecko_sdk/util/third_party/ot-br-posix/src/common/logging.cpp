/*
 *    Copyright (c) 2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *    POSSIBILITY OF SUCH DAMAGE.
 */

#define OTBR_LOG_TAG "LOG"

#ifndef OTBR_SYSLOG_FACILITY_ID
#define OTBR_SYSLOG_FACILITY_ID LOG_USER
#endif

#include "common/logging.hpp"

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <syslog.h>

#include <sstream>

#include "common/code_utils.hpp"
#include "common/time.hpp"

static otbrLogLevel sLevel            = OTBR_LOG_INFO;
static const char   sLevelString[][8] = {
    "[EMERG]", "[ALERT]", "[CRIT]", "[ERR ]", "[WARN]", "[NOTE]", "[INFO]", "[DEBG]",
};

/** Get the current debug log level */
otbrLogLevel otbrLogGetLevel(void)
{
    return sLevel;
}

/**
 * Set current log level.
 */
void otbrLogSetLevel(otbrLogLevel aLevel)
{
    assert(aLevel >= OTBR_LOG_EMERG && aLevel <= OTBR_LOG_DEBUG);
    sLevel = aLevel;
}

/** Initialize logging */
void otbrLogInit(const char *aIdent, otbrLogLevel aLevel, bool aPrintStderr)
{
    assert(aIdent);
    assert(aLevel >= OTBR_LOG_EMERG && aLevel <= OTBR_LOG_DEBUG);

    openlog(aIdent, (LOG_CONS | LOG_PID) | (aPrintStderr ? LOG_PERROR : 0), OTBR_SYSLOG_FACILITY_ID);
    sLevel = aLevel;
}

static const char *GetPrefix(const char *aLogTag)
{
    // Log prefix format : -xxx-----
    const uint8_t kMaxTagSize = 7;
    const uint8_t kBufferSize = kMaxTagSize + 3;
    static char   prefix[kBufferSize];
    uint8_t       tagLength = strlen(aLogTag) > kMaxTagSize ? kMaxTagSize : strlen(aLogTag);
    int           index     = 0;

    if (strlen(aLogTag) > 0)
    {
        prefix[0] = '-';
        memcpy(&prefix[1], aLogTag, tagLength);

        index = tagLength + 1;

        memset(&prefix[index], '-', kMaxTagSize - tagLength + 1);
        index += kMaxTagSize - tagLength + 1;
    }

    prefix[index++] = '\0';

    return prefix;
}

/** log to the syslog or log file */
void otbrLog(otbrLogLevel aLevel, const char *aLogTag, const char *aFormat, ...)
{
    const uint16_t kBufferSize = 1024;
    va_list        ap;
    char           buffer[kBufferSize];

    va_start(ap, aFormat);

    if ((aLevel <= sLevel) && (vsnprintf(buffer, sizeof(buffer), aFormat, ap) > 0))
    {
        syslog(static_cast<int>(aLevel), "%s%s: %s", sLevelString[aLevel], GetPrefix(aLogTag), buffer);
    }

    va_end(ap);

    return;
}

/** log to the syslog or log file */
void otbrLogv(otbrLogLevel aLevel, const char *aFormat, va_list aArgList)
{
    assert(aFormat);

    if (aLevel <= sLevel)
    {
        otbrLogvNoFilter(aLevel, aFormat, aArgList);
    }
}

void otbrLogvNoFilter(otbrLogLevel aLevel, const char *aFormat, va_list aArgList)
{
    vsyslog(static_cast<int>(aLevel), aFormat, aArgList);
}

/** Hex dump data to the log */
void otbrDump(otbrLogLevel aLevel, const char *aLogTag, const char *aPrefix, const void *aMemory, size_t aSize)
{
    static const char kHexChars[] = "0123456789abcdef";
    assert(aPrefix && (aMemory || aSize == 0));
    const uint8_t *pEnd;
    const uint8_t *p8;
    int            addr;

    if (aLevel >= sLevel)
    {
        return;
    }

    /* break hex dumps into 16byte lines
     * In the form ADDR: XX XX XX XX ...
     */

    // we pre-increment... so subtract
    addr = -16;

    while (aSize > 0)
    {
        size_t this_size;
        char   hex[16 * 3 + 1];

        addr = addr + 16;
        p8   = (const uint8_t *)(aMemory) + addr;

        /* truncate line to max 16 bytes */
        this_size = aSize;
        if (this_size > 16)
        {
            this_size = 16;
        }
        aSize = aSize - this_size;

        char *ch = hex - 1;

        for (pEnd = p8 + this_size; p8 < pEnd; p8++)
        {
            *++ch = kHexChars[(*p8) >> 4];
            *++ch = kHexChars[(*p8) & 0x0f];
            *++ch = ' ';
        }
        *ch = 0;

        otbrLog(aLevel, aLogTag, "%s: %04x: %s", aPrefix, addr, hex);
    }
}

const char *otbrErrorString(otbrError aError)
{
    const char *error;

    switch (aError)
    {
    case OTBR_ERROR_NONE:
        error = "OK";
        break;

    case OTBR_ERROR_ERRNO:
        error = strerror(errno);
        break;

    case OTBR_ERROR_DBUS:
        error = "DBUS error";
        break;

    case OTBR_ERROR_MDNS:
        error = "MDNS error";
        break;

    case OTBR_ERROR_OPENTHREAD:
        error = "OpenThread error";
        break;

    case OTBR_ERROR_NOT_FOUND:
        error = "Not found";
        break;

    case OTBR_ERROR_PARSE:
        error = "Parse error";
        break;

    case OTBR_ERROR_NOT_IMPLEMENTED:
        error = "Not implemented";
        break;

    case OTBR_ERROR_INVALID_ARGS:
        error = "Invalid arguments";
        break;

    case OTBR_ERROR_DUPLICATED:
        error = "Duplicated";
        break;

    case OTBR_ERROR_ABORTED:
        error = "Aborted";
        break;

    case OTBR_ERROR_INVALID_STATE:
        error = "Invalid state";
        break;

    default:
        error = "Unknown";
    }

    return error;
}

void otbrLogDeinit(void)
{
    closelog();
}
