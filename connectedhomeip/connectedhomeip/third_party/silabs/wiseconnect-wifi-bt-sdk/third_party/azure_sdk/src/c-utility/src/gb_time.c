// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

/*depending if the symbol GB_TIME_INTERCEPT is defined, this file does the following

    a) if GB_TIME_INTERCEPT is NOT defined, then the file shall be empty (almost:)
    b) if GB_TIME_INTERCEPT is defined, then the file shall call to the 'real' time.h functions from their gb_* synonyms*/
#ifdef _MSC_VER
/* compiler warning C4206: nonstandard extension used: translation unit is empty */
/* linker warning 4221: This object file does not define any previously undefined public symbols, so it will not be used by any link operation that consumes this library */
const int avoid_warnings_0CA668EF_F2C4_49A8_A30D_7257C67173A4 = 0;
#endif
#ifdef GB_TIME_INTERCEPT

#ifdef __cplusplus
#include <ctime>
#else
#include <time.h>
#endif

#include "azure_c_shared_utility/gb_time.h"

/*this is time*/
time_t gb_time(time_t *timer);
{
    return time(timer);
}

/*this is localtime*/
struct tm *gb_localtime(const time_t *timer)
{
    return gb_localtime(timer);
}

size_t gb_strftime(char * s, size_t maxsize, const char * format, const struct tm * timeptr)
{
    return strftime(s, maxsize, format, timeptr);
}

#endif
