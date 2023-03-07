/*
 * Copyright (c) 2021, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 */

/** @file "qvIO_IO.c"
 *
 *  IO functionality
 *
 *  Implementation of qvIO IO
*/

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
/* <CodeGenerator Placeholder> Includes */

#define GP_COMPONENT_ID GP_COMPONENT_ID_QVIO
#define GP_MODULE_ID    GP_COMPONENT_ID

#include "qvIO.h"

#include "hal.h"
#include "gpHal.h"

#include "gpSched.h"
#include "gpUtils.h"
#include "gpCom.h"

/* </CodeGenerator Placeholder> Includes */

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> Macro */

#define APP_MAX_LED 3

#define PWM_DUTY_CYCLE_MULT (HAL_PWM_MAX_DUTY_CYCLE_PC / 256)

#define APP_BUTTON_DEBOUNCE_PERIOD_MS 20

typedef struct IO_LedBlink_ {
    uint8_t ledNr;
    uint16_t onMs;
    uint16_t offMs;
    bool currentState;
} IO_LedBlink_t;

#ifdef GP_DIVERSITY_QPG6105DK_B01 
#define GP_BSP_GPIO0_CONFIG() do{ \
    GP_WB_WRITE_IOB_GPIO_0_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP); \
    GP_WB_WRITE_GPIO_GPIO0_OUTPUT_VALUE(0); \
    GP_WB_WRITE_GPIO_GPIO0_DIRECTION(1); /*Set as output*/ \
    HAL_LED_CLR_RED(); \
} while(false)

#define GP_BSP_BUTTON_2  GP_BSP_BUTTON_GP_PB2_PIN
#define GP_BSP_BUTTON_3  GP_BSP_BUTTON_GP_PB3_PIN
#define GP_BSP_BUTTON_4  GP_BSP_BUTTON_GP_PB1_PIN
#define GP_BSP_BUTTON_5  GP_BSP_BUTTON_GP_PB4_PIN
#define GP_BSP_BUTTON_7  GP_BSP_BUTTON_GP_SW_PIN
#else
#define GP_BSP_GPIO0_CONFIG()
#endif //GP_DIVERSITY_QPG6105DK_B01
/* </CodeGenerator Placeholder> Macro */

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> StaticData */
static IO_LedBlink_t IO_LedBlinkInfo[APP_MAX_LED];

static qvIO_pBtnCback IO_BtnCallback = NULL;

static uint8_t IO_UartRxData[256];
static gpUtils_CircularBuffer_t IO_UartRxBuffer;
/* </CodeGenerator Placeholder> StaticData */

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/* <CodeGenerator Placeholder> StaticFunctionDefinitions */

/*****************************************************************************
 * --- LED handling
 *****************************************************************************/

static void LedBlink_Timeout(void* pArg)
{
    // cast void pointer to led struct - IO_LedBlink_t
    IO_LedBlink_t* ledInfo = (IO_LedBlink_t*)pArg;

    ledInfo->currentState = !ledInfo->currentState;
    if(ledInfo->currentState == false)
    {
        qvIO_LedSet(ledInfo->ledNr, false);
        gpSched_ScheduleEventArg(ledInfo->offMs * 1000, LedBlink_Timeout, (void*)ledInfo);
    }
    else
    {
        qvIO_LedSet(ledInfo->ledNr, true);
        gpSched_ScheduleEventArg(ledInfo->onMs * 1000, LedBlink_Timeout, (void*)ledInfo);
    }
}

/*****************************************************************************
 * --- UART handling
 *****************************************************************************/

static void qvIO_cbRxData(UInt16 length, UInt8* pPayload, gpCom_CommunicationId_t communicationId)
{
    Bool writeSucces;

    HAL_DISABLE_GLOBAL_INT();
    writeSucces = gpUtils_CircBWriteData(&IO_UartRxBuffer, pPayload, length);
    HAL_ENABLE_GLOBAL_INT();

    GP_ASSERT_DEV_INT(writeSucces);
}

/*****************************************************************************
 * --- Button handling
 *****************************************************************************/

#define BTN_CALLBACK(gpioNum, btnNum)      \
    do                                     \
    {                                      \
        if(hal_gpioGet(gpios[gpioNum]))    \
        {                                  \
            IO_BtnCallback(btnNum, false); \
        }                                  \
        else                               \
        {                                  \
            IO_BtnCallback(btnNum, true);  \
        }                                  \
    } while(false)

/** @brief Function to handle any GPIO changes
*/
static void IO_PollGPIO(void)
{
    //Collect GPIO state - active low buttons
    if(IO_BtnCallback != NULL)
    {
#ifdef GP_DIVERSITY_QPG6105DK_B01
        BTN_CALLBACK(GP_BSP_BUTTON_2, BTN_SW1);
        BTN_CALLBACK(GP_BSP_BUTTON_7, BTN_SW2); // Slider switch
        BTN_CALLBACK(GP_BSP_BUTTON_3, BTN_SW3);
        BTN_CALLBACK(GP_BSP_BUTTON_4, BTN_SW4);
        BTN_CALLBACK(GP_BSP_BUTTON_5, BTN_SW5);
#else
#ifdef GP_BSP_BUTTON_1
        BTN_CALLBACK(GP_BSP_BUTTON_1, BTN_SW1);
#endif //GP_BSP_BUTTON_1
#ifdef GP_BSP_BUTTON_2
        BTN_CALLBACK(GP_BSP_BUTTON_2, BTN_SW2);
#endif
#ifdef GP_BSP_BUTTON_3
        BTN_CALLBACK(GP_BSP_BUTTON_3, BTN_SW3);
#endif
#ifdef GP_BSP_BUTTON_4
        BTN_CALLBACK(GP_BSP_BUTTON_4, BTN_SW4);
#endif
#ifdef GP_BSP_BUTTON_5
        BTN_CALLBACK(GP_BSP_BUTTON_5, BTN_SW5);
#endif
#endif // GP_DIVERSITY_QPG6105DK_B01
    }

    gpHal_EnableExternalEventCallbackInterrupt(true);
}

/** @brief Registered Callback from Qorvo stack to signal chip wakeup
*/
static void IO_cbExternalEvent(void)
{
    //Disable until handled
    gpHal_EnableExternalEventCallbackInterrupt(false);

    //Delay check for debouncing of button/signal
    if(!gpSched_ExistsEvent(IO_PollGPIO))
    {
        gpSched_ScheduleEvent(APP_BUTTON_DEBOUNCE_PERIOD_MS * 1000, IO_PollGPIO);
    }
}

/** @brief Extend initialization of GPIOs to allow wakeup from sleep on GPIO trigger
*/
static void IO_InitGPIOWakeUp(void)
{
    gpHal_ExternalEventDescriptor_t eventDesc;

    //-- Configure pins for wakeup
#ifdef GP_BSP_BUTTON_1
    hal_gpioSetWakeUpMode(GP_BSP_BUTTON_1, hal_WakeUpModeBoth);
#endif //GP_BSP_BUTTON_1
#ifdef GP_BSP_BUTTON_2
    hal_gpioSetWakeUpMode(GP_BSP_BUTTON_2, hal_WakeUpModeBoth);
#endif //GP_BSP_BUTTON_2
#ifdef GP_BSP_BUTTON_3
    hal_gpioSetWakeUpMode(GP_BSP_BUTTON_3, hal_WakeUpModeBoth);
#endif //GP_BSP_BUTTON_3
#ifdef GP_BSP_BUTTON_4
    hal_gpioSetWakeUpMode(GP_BSP_BUTTON_4, hal_WakeUpModeBoth);
#endif //GP_BSP_BUTTON_4
#ifdef GP_BSP_BUTTON_5
    hal_gpioSetWakeUpMode(GP_BSP_BUTTON_5, hal_WakeUpModeBoth);
#endif //GP_BSP_BUTTON_5
    //Configure External event block
    eventDesc.type = gpHal_EventTypeDummy; //Only ISR generation
    gpHal_ScheduleExternalEvent(&eventDesc);

    //Register handler function
    gpHal_RegisterExternalEventCallback(IO_cbExternalEvent);

    //Enable interrupt mask
    gpHal_EnableExternalEventCallbackInterrupt(true);
}

/** @brief Initialize GPIOs to use when awake
*/
static void IO_InitGPIO(void)
{
    //Init pins

    GP_BSP_GPIO0_CONFIG();

    //-- Set internal Pull-up for push buttons
#ifdef GP_BSP_BUTTON_1
    hal_gpioModePU(GP_BSP_BUTTON_1, true);
#endif //GP_BSP_BUTTON_1
#ifdef GP_BSP_BUTTON_2
    hal_gpioModePU(GP_BSP_BUTTON_2, true);
#endif //GP_BSP_BUTTON_2
#ifdef GP_BSP_BUTTON_3
    hal_gpioModePU(GP_BSP_BUTTON_3, true);
#endif //GP_BSP_BUTTON_3
#ifdef GP_BSP_BUTTON_4
    hal_gpioModePU(GP_BSP_BUTTON_4, true);
#endif //GP_BSP_BUTTON_4
#ifdef GP_BSP_BUTTON_5
    hal_gpioModePU(GP_BSP_BUTTON_5, true);
#endif //GP_BSP_BUTTON_5

#ifdef GP_BSP_PWM_GPIO_MAP
    //Init PWM
    hal_InitPWM();
#endif //GP_BSP_PWM_GPIO_MAP
}
/* </CodeGenerator Placeholder> StaticFunctionDefinitions */

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/** @brief Initialize qvIO interface for use.
 *
*/
void qvIO_Init(void)
{
    IO_InitGPIOWakeUp();
    IO_InitGPIO();
}

/*****************************************************************************
 * LED control
 *****************************************************************************/

/** @brief Set LED ON or OFF.
*
*   @param ledNr                     The index of the LED that is controlled.
*   @param state                     LED on (true) or off (false).
*/
bool qvIO_LedSet(uint8_t ledNr, bool state)
{
    /* <CodeGenerator Placeholder> Implementation_qvIO_LedSet */
    IO_LedBlink_t* localLedInfo;

    if(ledNr >= APP_MAX_LED)
    {
        return false;
    }

    /* if a blinking for this LED exists, stop it */
    localLedInfo = &IO_LedBlinkInfo[ledNr];
    if(gpSched_ExistsEventArg(LedBlink_Timeout, (void*)localLedInfo))
    {
        gpSched_UnscheduleEventArg(LedBlink_Timeout, (void*)localLedInfo);
        /* reset structure */
        MEMSET(localLedInfo, 0x00, sizeof(IO_LedBlink_t));
    }

    if(state == false)
    {
        if(ledNr == LED_WHITE)
        {
            qvIO_PWMSetLevel(PWM_CHANNEL_WHITE_COOL, 0);
#ifdef GP_BSP_PWM_GPIO_MAP
            hal_SetChannelEnabled(PWM_CHANNEL_WHITE_COOL, false);
#endif 
        }
        else if(ledNr == LED_GREEN)
        {
            HAL_LED_CLR_GRN();
        }
        else if(ledNr == LED_RED)
        {
            HAL_LED_CLR_RED();
        }
    }
    else
    {
        if(ledNr == LED_WHITE)
        {
#ifdef GP_BSP_PWM_GPIO_MAP            
            hal_SetChannelEnabled(PWM_CHANNEL_WHITE_COOL, true);
#endif            
            qvIO_PWMSetLevel(PWM_CHANNEL_WHITE_COOL, 255);
        }
        else if(ledNr == LED_GREEN)
        {
            HAL_LED_SET_GRN();
        }
        else if(ledNr == LED_RED)
        {
            HAL_LED_SET_RED();
        }
    }

    return true;
    /* </CodeGenerator Placeholder> Implementation_qvIO_LedSet */
}

/** @brief Blink a LED with specified on and off period.
*
*   @param ledNr                     The index of the LED that is controlled.
*   @param onMs                      How many msec should the ON state last.
*   @param offMs                     How many msec should the OFF state last.
*/
bool qvIO_LedBlink(uint8_t ledNr, uint16_t onMs, uint16_t offMs)
{
    /* <CodeGenerator Placeholder> Implementation_qvIO_LedBlink */
    if(ledNr < APP_MAX_LED)
    {
        /* store parameters in the local blinking info structure */
        IO_LedBlink_t* localLedInfo;

        localLedInfo = &IO_LedBlinkInfo[ledNr];

        /* only modify blinking if something is changed */
        if((localLedInfo->onMs != onMs) || (localLedInfo->offMs != offMs))
        {
            localLedInfo->ledNr = ledNr;
            localLedInfo->onMs = onMs;
            localLedInfo->offMs = offMs;
            localLedInfo->currentState = true;

            /* stop blinking for current LED if in progress */
            gpSched_UnscheduleEventArg(LedBlink_Timeout, (void*)localLedInfo);

            qvIO_LedSet(ledNr, true);
            gpSched_ScheduleEventArg(localLedInfo->onMs * 1000, LedBlink_Timeout, (void*)localLedInfo);
        }

        return true;
    }

    return false;
    /* </CodeGenerator Placeholder> Implementation_qvIO_LedBlink */
}

/*****************************************************************************
 * Button control
 *****************************************************************************/

/** @brief Store internally an upper layer callback for signaling button presses.
*
*   @param btnCback                  Pointer to the button handler to be stored internally.
*/
void qvIO_SetBtnCallback(qvIO_pBtnCback btnCback)
{
    /* <CodeGenerator Placeholder> Implementation_qvIO_SetBtnCallback */
    IO_BtnCallback = btnCback;
    /* </CodeGenerator Placeholder> Implementation_qvIO_SetBtnCallback */
}

/*****************************************************************************
 * UART control
 *****************************************************************************/

#ifndef GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
#error Raw gpCom output expected within IO builds. Generic serial terminal support required.
#endif //GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC

/** @brief Transmit data over the UART
 *
 *  @param length         Length of data to send
 *  @param txBuffer       Pointer to a buffer to transmit
*/
void qvIO_UartTxData(uint8_t length, const char* txBuffer)
{
    // Expecting gpCom to be used in a raw fashion
    gpCom_Flush();
    gpCom_DataRequest(GP_COMPONENT_ID_QVIO, length, (UInt8*)txBuffer, GP_COM_DEFAULT_COMMUNICATION_ID);
}

/** @brief Get received UART data
 *
 *  @param length          Max length of data to retrieve
 *  @param txBuffer        Pointer to a buffer to received the data in.
 *                         It is assumed to be large enough to hold 'length' bytes
 *
 *  @return receivedLength Amount of bytes returned in the buffer.
 *                         If more data was pending, only 'length' bytes are returned.
*/
uint8_t qvIO_UartReadRxData(uint8_t length, char* rxBuffer)
{
    // Expected to be called from other thread
    uint8_t available;

    HAL_DISABLE_GLOBAL_INT();
    available = gpUtils_CircBAvailableData(&IO_UartRxBuffer);
    gpUtils_CircBReadData(&IO_UartRxBuffer, (UInt8*)rxBuffer, (available > length) ? length : available);
    HAL_ENABLE_GLOBAL_INT();

    return (available > length) ? length : available;
}

/** @brief Initialize UART for use.
 *
*/
void qvIO_UartInit(void)
{
    // Initialize Tx Circular buffer
    HAL_DISABLE_GLOBAL_INT();
    gpUtils_CircBInit(&IO_UartRxBuffer, IO_UartRxData, sizeof(IO_UartRxData));
    HAL_ENABLE_GLOBAL_INT();

    // Hook up callback
    gpCom_RegisterModule(GP_MODULE_ID, qvIO_cbRxData);
}

/*****************************************************************************
 * PWM control
 *****************************************************************************/

/** @brief turns color LED on or off
*
*   @param onoff              true for on, false for off
*/
void qvIO_PWMColorOnOff(bool onoff)
{
#ifdef GP_BSP_PWM_GPIO_MAP
    /* <CodeGenerator Placeholder> Implementation_qvIO_PWMColorOnOff */
    if(onoff)
    {
        // Enable pwm channels for RGB LED
        hal_SetChannelEnabled(PWM_CHANNEL_RED, true);
        hal_SetChannelEnabled(PWM_CHANNEL_GREEN, true);
        hal_SetChannelEnabled(PWM_CHANNEL_BLUE, true);

        hal_EnablePwm(true);
    }
    else
    {
        // Disable pwm channels for RGB LED
        hal_EnablePwm(false);

        hal_SetChannelEnabled(PWM_CHANNEL_RED, false);
        hal_SetChannelEnabled(PWM_CHANNEL_GREEN, false);
        hal_SetChannelEnabled(PWM_CHANNEL_BLUE, false);
    }
    /* </CodeGenerator Placeholder> Implementation_qvIO_PWMColorOnOff */
#endif //GP_BSP_PWM_GPIO_MAP
}

/** @brief sets RGB color of led 255 == 100%
*
*   @param r                    intensity of red (0-255)
*   @param g                    intensity of green (0-255)
*   @param b                    intensity of blue (0-255)
*/
void qvIO_PWMSetColor(uint8_t r, uint8_t g, uint8_t b)
{
    /* <CodeGenerator Placeholder> Implementation_qvIO_PWMSetColor */
#ifdef GP_BSP_PWM_GPIO_MAP
    // map onto 1 - 10000 range
    hal_SetDutyCyclePercentage(PWM_CHANNEL_RED, (UInt32)r * PWM_DUTY_CYCLE_MULT);
    hal_SetDutyCyclePercentage(PWM_CHANNEL_GREEN, (UInt32)g * PWM_DUTY_CYCLE_MULT);
    hal_SetDutyCyclePercentage(PWM_CHANNEL_BLUE, (UInt32)b * PWM_DUTY_CYCLE_MULT);

#endif //GP_BSP_PWM_GPIO_MAP
    /* </CodeGenerator Placeholder> Implementation_qvIO_PWMSetColor */
}

/** @brief sets brightness of led 255 = 100%
*
*   @param channel              pwm channel
*   @param level                intensity of level (0-255)
*/
void qvIO_PWMSetLevel(uint8_t channel, uint8_t level)
{
#ifdef GP_BSP_PWM_GPIO_MAP
    hal_SetDutyCyclePercentage(channel, (UInt32)level*PWM_DUTY_CYCLE_MULT);
#endif //GP_BSP_PWM_GPIO_MAP
}
