#pragma once

#include <stdio.h>
#include <stdlib.h>

#if 0
#define IOT_DEBUG(format, ...) printf(format "\r\n", ##__VA_ARGS__)
#define IOT_INFO(format, ...) printf(format "\r\n", ##__VA_ARGS__)
#define IOT_WARN(format, ...) printf(format "\r\n", ##__VA_ARGS__)
#define IOT_ERROR(format, ...) printf(format "\r\n", ##__VA_ARGS__)

/* Function tracing macros used in AWS IoT SDK,
   mapped to "verbose" level output
*/
#define FUNC_ENTRY printf("FUNC_ENTRY:   %s L#%d \r\n", __func__, __LINE__)
#define FUNC_EXIT_RC(x) \
    do {                                                                \
        printf("FUNC_EXIT:   %s L#%d Return Code : %d \r\n", __func__, __LINE__, (int)x); \
        return x; \
    } while(0)
#else
#define IOT_DEBUG(format, ...)
#define IOT_INFO(format, ...)
#define IOT_WARN(format, ...)
#define IOT_ERROR(format, ...)

#define FUNC_ENTRY 
#define FUNC_EXIT_RC(x) \
    do {                                                                \
        return x; \
    } while(0)
#endif
