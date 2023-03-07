/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== LoggerITM.h ========
 */

#ifndef ti_loggers_drivers_LoggerITM__include
#define ti_loggers_drivers_LoggerITM__include

#include <stdint.h>

#include <ti/utils/runtime/ILogger.h>

#if defined (__cplusplus)
extern "C" {
#endif

/*
 * ======== LoggerITM_Instance ========
 */
typedef enum {
    LoggerITM_STIM_RESV0  = 0,      //!< Port 0. Reserved for future use
    LoggerITM_STIM_RESV1,           //!< Port 1. Reserved for future use
    LoggerITM_STIM_RESV2,           //!< Port 2. Reserved for future use
    LoggerITM_STIM_RESV3,           //!< Port 3. Reserved for future use
    LoggerITM_STIM_RESV4,           //!< Port 4. Reserved for future use
    LoggerITM_STIM_RESV5,           //!< Port 5. Reserved for future use
    LoggerITM_STIM_RESV6,           //!< Port 6. Reserved for future use
    LoggerITM_STIM_RESV7,           //!< Port 7. Reserved for future use
    LoggerITM_STIM_RESV8,           //!< Port 8. Reserved for future use
    LoggerITM_STIM_RESV9,           //!< Port 9. Reserved for future use
    LoggerITM_STIM_RESV10,          //!< Port 10. Reserved for future use
    LoggerITM_STIM_TIME_SYNC = 11,  //!< Port 11. Reserved for logger time sync
    LoggerITM_STIM_LOGGER,          //!< Port 12. Used to notify logger states
    LoggerITM_STIM_RESV13,          //!< Port 13. Reserved for future use
    LoggerITM_STIM_HEADER,          //!< Port 14. Reserved for logger header messages
    LoggerITM_STIM_TRACE,           //!< Port 15. Reserved for logger main data transfer

    /* Ports 16-31 can be used for raw data output  */
    LoggerITM_STIM_RAW0 = 16,       //!< Port 16. Reserved for raw ITM data
    LoggerITM_STIM_RAW1,            //!< Port 17. Reserved for raw ITM data
    LoggerITM_STIM_RAW2,            //!< Port 18. Reserved for raw ITM data
    LoggerITM_STIM_RAW3,            //!< Port 19. Reserved for raw ITM data
    LoggerITM_STIM_RAW4,            //!< Port 20. Reserved for raw ITM data
    LoggerITM_STIM_RAW5,            //!< Port 21. Reserved for raw ITM data
    LoggerITM_STIM_RAW6,            //!< Port 22. Reserved for raw ITM data
    LoggerITM_STIM_RAW7,            //!< Port 23. Reserved for raw ITM data
    LoggerITM_STIM_RAW8,            //!< Port 24. Reserved for raw ITM data
    LoggerITM_STIM_RAW9,            //!< Port 25. Reserved for raw ITM data
    LoggerITM_STIM_RAW10,           //!< Port 26. Reserved for raw ITM data
    LoggerITM_STIM_RAW11,           //!< Port 27. Reserved for raw ITM data
    LoggerITM_STIM_RAW12,           //!< Port 28. Reserved for raw ITM data
    LoggerITM_STIM_RAW13,           //!< Port 29. Reserved for raw ITM data
    LoggerITM_STIM_RAW14,           //!< Port 30. Reserved for raw ITM data
    LoggerITM_STIM_RAW15            //!< Port 31. Reserved for raw ITM data
} LoggerITM_StimulusPorts;

/*
 *  ======== LoggerITM_Instance ========
 */
typedef struct LoggerITM_Instance {
    uint32_t            serial;
} LoggerITM_Instance;

/*
 *  ======== LoggerITM_Handle ========
 */
typedef LoggerITM_Instance *LoggerITM_Handle;

/*
 *  ======== LoggerITM_init ========
 */
extern void LoggerITM_init(void);

/*
 *  ======== LoggerITM_finalize ========
 */
extern void LoggerITM_finalize(void);

/*
 *  ======== ti_loggers_drivers_LoggerITM_event ========
 */
extern void ti_loggers_drivers_LoggerITM_event(ILogger_Handle handle,
                                               uintptr_t header,
                                               uintptr_t event,
                                               uintptr_t arg0,
                                               uintptr_t arg1,
                                               uintptr_t arg2,
                                               uintptr_t arg3);

/*
 *  ======== ti_loggers_drivers_LoggerITM_printf ========
 */
extern void  ti_loggers_drivers_LoggerITM_printf(ILogger_Handle handle,
                                                 uint32_t header,
                                                 uint32_t numArgs,
                                                 ...);

/*
 *  ======== ti_loggers_drivers_LoggerITM_buf ========
 */
extern void ti_loggers_drivers_LoggerITM_buf(ILogger_Handle handle,
                                             uint32_t header,
                                             const char* format,
                                             uint8_t *data,
                                             size_t size);

#if defined (__cplusplus)
}
#endif

#endif /* ti_loggers_drivers_LoggerITM__include */
