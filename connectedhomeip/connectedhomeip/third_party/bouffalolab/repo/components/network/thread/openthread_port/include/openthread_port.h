/*
 *  Copyright (c) 2019, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
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
 */

/**
 * @file
 *   This file includes interface definitions for FreeRTOS.
 *
 */

#ifndef OPENTHREAD_PORT_H
#define OPENTHREAD_PORT_H

#if OT_FREERTOS_ENABLE
#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>
#endif

#include <openthread/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OT_TASK_SIZE
#define OT_TASK_SIZE 4096
#endif

#ifndef OT_TASK_PRORITY
#define OT_TASK_PRORITY 20
#endif

#ifndef OT_UART_RX_BUFFSIZE
#define OT_UART_RX_BUFFSIZE 384
#endif

typedef enum _ot_system_event {
    OT_SYSTEM_EVENT_NONE                                = 0,

    OT_SYSTEM_EVENT_OT_TASKLET                          = 0x00000001,

    OT_SYSTEM_EVENT_ALARM_MS_EXPIRED                    = 0x00000002,
    OT_SYSTEM_EVENT_ALARM_US_EXPIRED                    = 0x00000004,
    OT_SYSTEM_EVENT_ALARM_ALL_MASK                      = OT_SYSTEM_EVENT_ALARM_MS_EXPIRED | OT_SYSTEM_EVENT_ALARM_US_EXPIRED,

    OT_SYSTEM_EVENT_UART_TXR                            = 0x00000010,
    OT_SYSTEM_EVENT_UART_TXD                            = 0x00000020,
    OT_SYSTEM_EVENT_UART_RXD                            = 0x00000040,
    OT_SYSETM_EVENT_UART_ALL_MASK                       = OT_SYSTEM_EVENT_UART_TXR | OT_SYSTEM_EVENT_UART_TXD | OT_SYSTEM_EVENT_UART_RXD,

    OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ            = 0x00000100,
    OT_SYSTEM_EVENT_RADIO_TX_ERROR                      = 0x00000200,
    OT_SYSTEM_EVENT_RADIO_TX_ACKED                      = 0x00000400,
    OT_SYSTEM_EVENT_RADIO_TX_NO_ACK                     = 0x00000800,
    OT_SYSTEM_EVENT_RADIO_TX_ALL_MASK                   = OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ | 
        OT_SYSTEM_EVENT_RADIO_TX_ERROR | OT_SYSTEM_EVENT_RADIO_TX_ACKED | OT_SYSTEM_EVENT_RADIO_TX_NO_ACK,

    OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF                    = 0x00001000,
    OT_SYSTEM_EVENT_RADIO_RX_DONE                       = 0x00002000,
    OT_SYSTEM_EVENT_RADIO_RX_CRC_FIALED                 = 0x00004000,
    OT_SYSTEM_EVENT_RADIO_RX_ALL_MASK                   = OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF | 
        OT_SYSTEM_EVENT_RADIO_RX_DONE | OT_SYSTEM_EVENT_RADIO_RX_CRC_FIALED,
    OT_SYSTEM_EVENT_RADIO_ALL_MASK                      = OT_SYSTEM_EVENT_RADIO_TX_ALL_MASK | OT_SYSTEM_EVENT_RADIO_RX_ALL_MASK,

    OT_SYSTEM_EVENT_APP                                 = 0xff000000,

    OT_SYSTEM_EVENT_ALL                                 = 0xffffffff,
} ot_system_event_t;

extern ot_system_event_t ot_system_event_var;

/****************************************************************************//**
 * @brief  Init openthread tack.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrStackInit(void);

/****************************************************************************//**
 * @brief  Start OpenThread task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrStart(void);

/****************************************************************************//**
 * @brief  Initializes user code with OpenThread related before OpenThread 
 *          main event loop execution. This function is called after
 *          Openthread instance created and by OpenThread task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrInitUser(otInstance * instance);

/****************************************************************************//**
 * @brief  Get current OpenThread instance.
 *
 * @param  None
 *
 * @return otInstance instance
 *
*******************************************************************************/
otInstance *otrGetInstance();

/****************************************************************************//**
 * @brief  Handle UART task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_uartTask (ot_system_event_t sevent);

/****************************************************************************//**
 * @brief  A wrapper to call otPlatUartReceived.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_uartRecieved(uint8_t * rxbuf, uint32_t rxlen);
void ot_uartSetFd(int fd);
void ot_uartLog(const char *fmt, va_list argp);

void ot_alarmInit(void);

/****************************************************************************//**
 * @brief  Handle alarm timer task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_alarmTask(ot_system_event_t sevent);

/****************************************************************************//**
 * @brief  Init radio.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_radioInit(void);

/****************************************************************************//**
 * @brief  Handle radio task.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ot_radioTask(ot_system_event_t trxEvent);

/****************************************************************************//**
 * @brief  init for openthread cli command.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
extern void otAppCliInit(otInstance *aInstance);

/****************************************************************************//**
 * @brief  init for openthread ncp command.
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
extern void otAppNcpInit(otInstance *aInstance);

#define OT_CLI_UART_OUTPUT_LOCK()
#define OT_CLI_UART_OUTPUT_UNLOCK()

#if OT_FREERTOS_ENABLE

/****************************************************************************//**
 * @brief  Give semphore to OpenThread task
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otSysEventSignalPending(void);

/****************************************************************************//**
 * @brief  Lock OpenThread task to acquire ciritcal section access
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrLock(void);

/****************************************************************************//**
 * @brief  Unlock OpenThread task to release ciritcal section access
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void otrUnlock(void);

/****************************************************************************//**
 * @brief  Macro OT_THREAD_SAFE provides a method to access OpenThread with 
 * thread-safe in other tasks running context.
 * 
 * ot_initUser and OpenThread callback functions already are thread-safe protected.
 * 
 * Note, do NOT call return in this macro block.
 * 
 * @param ...  OpenThread api call statement
 *
*******************************************************************************/
#define OT_THREAD_SAFE(...)                 \
    otrLock();                              \
    do                                      \
    {                                       \
        __VA_ARGS__;                        \
    } while (0);                            \
    otrUnlock();                            

/****************************************************************************//**
 * @brief  Macro OT_THREAD_SAFE_RET provides a method to access OpenThread with 
 * thread-safe in other tasks running context.
 * 
 * ot_initUser and OpenThread callback functions already are thread-safe protected.
 * 
 * Note, do NOT call return in this macro block.
 *
 * @param ret   return value
 * @param ...   OpenThread api call statement
 *
*******************************************************************************/
#define OT_THREAD_SAFE_RET(ret, ...)        \
    do                                      \
    {                                       \
        otrLock();                          \
        (ret) = __VA_ARGS__;                \
        otrUnlock();                        \
    } while (0)                             


#define OT_ENTER_CRITICAL()                 taskENTER_CRITICAL()
#define OT_ENTER_CRITICAL_ISR()             taskENTER_CRITICAL_FROM_ISR()
#define OT_EXIT_CRITICAL()                  taskEXIT_CRITICAL()
#define OT_EXIT_CRITICAL_ISR(x)             taskEXIT_CRITICAL_FROM_ISR(x)

#define OT_NOTIFY_ISR(ebit)                 (ot_system_event_var |= ebit); otSysEventSignalPending()
#define OT_NOTIFY(ebit)                     OT_ENTER_CRITICAL(); ot_system_event_var |= ebit; OT_EXIT_CRITICAL(); otSysEventSignalPending()
#define OT_GET_NOTIFY(ebit)                 OT_ENTER_CRITICAL(); ebit = ot_system_event_var; ot_system_event_var = OT_SYSTEM_EVENT_NONE; OT_EXIT_CRITICAL()



/****************************************************************************//**
 * @brief  An weak function explore to applicaton layer to execute some application code.
 *          Note,
 *              1, this function is running in openthread main task, it is thread safed and
 *              it doesn't need OT_THREAD_SAFE/OT_THREAD_SAFE_RET protection.
 *              2, as it running with main task event let and radio/timer driver,
 *                  this function should be implemented very light without too
 *                  much execution time cost and delay or pending. And also do NOT
 *                  suspend and stop this task.
 *                  heavy execution or any delay or pending for some state transition should be
 *                  executed in another task.
 *
 * @param  sevent, event bit called by OT_APP_NOTIFY_ISR/OT_APP_NOTIFY from application code
 *
 * @return None
 *
*******************************************************************************/
void otrAppProcess(ot_system_event_t sevent);


/****************************************************************************//**
 * @brief  An interface to application to invoke openthread task to execute otrAppProcess, 
 *          which is used to call in application interrupt context.
 *
 * @param  ebit, event bit for otrAppProcess. 
 *              please reference to OT_SYSTEM_EVENT_APP for valid bits.
 *
 * @return None
 *
*******************************************************************************/
#define OT_APP_NOTIFY_ISR(ebit)             (ot_system_event_var |= ((ot_system_event_t)ebit & OT_SYSTEM_EVENT_APP)); otSysEventSignalPending()


/****************************************************************************//**
 * @brief  An interface to application to invoke openthread task to execute otrAppProcess, 
 *          which is used to call in application task context.
 *
 * @param  ebit, event bit for otrAppProcess. 
 *              please reference to OT_SYSTEM_EVENT_APP for valid bits.
 *
 * @return None
 *
*******************************************************************************/
#define OT_APP_NOTIFY(ebit)                 OT_ENTER_CRITICAL(); ot_system_event_var |=  ((ot_system_event_t)ebit & OT_SYSTEM_EVENT_APP); OT_EXIT_CRITICAL(); otSysEventSignalPending()

#else 
/** openthread without rtos */

#define OT_ENTER_CRITICAL()                 __asm volatile( "csrc mstatus, 8" )
#define OT_EXIT_CRITICAL()                  __asm volatile( "csrs mstatus, 8" )
#define OT_ENTER_CRITICAL_ISR()             OT_ENTER_CRITICAL()
#define OT_EXIT_CRITICAL_ISR(x)             OT_EXIT_CRITICAL(x)

#define OT_NOTIFY_ISR(ebit)                 (ot_system_event_var |= ebit)
#define OT_NOTIFY(ebit)                     OT_HW_INT_DIS(); (ot_system_event_var |= ebit); OT_HW_INT_EN()
#define OT_GET_NOTIFY(ebit)                 OT_HW_INT_DIS(); (ebit = ot_system_event_var; ot_system_event_var = 0;); OT_HW_INT_EN()

#define OT_THREAD_SAFE(x)
#define OT_THREAD_SAFE_RET(x)

#endif

#ifdef __cplusplus
}
#endif

#endif // OPENTHREAD_PORT_H
