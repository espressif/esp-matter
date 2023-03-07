/***************************************************************************//**
 * @file
 * @brief This file contains the plug-in manager for the EZRadio and
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "ustimer.h"

#include "ezradio_cmd.h"
#include "ezradio_prop.h"
#include "ezradio_hal.h"
#include "ezradio_api_lib.h"
#include "ezradio_api_lib_add.h"
#include "ezradio_plugin_manager.h"

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/* Radio configuration data array. */
static const uint8_t Radio_Configuration_Data_Array[]  = \
  RADIO_CONFIGURATION_DATA_ARRAY;

/* Radio interrupt receive flag */
static bool    ezradioIrqReceived = false;

#if defined(EZRADIO_PLUGIN_TRANSMIT)
Ecode_t ezradioHandleTransmitPlugin(EZRADIODRV_Handle_t radioHandle, EZRADIODRV_ReplyHandle_t radioReplyHandle);
#endif

#if defined(EZRADIO_PLUGIN_RECEIVE)
Ecode_t ezradioHandleReceivePlugin(EZRADIODRV_Handle_t radioHandle, EZRADIODRV_ReplyHandle_t radioReplyHandle);
#endif

#if defined(EZRADIO_PLUGIN_CRC_ERROR)
Ecode_t ezradioHandleCrcErrorPlugin(EZRADIODRV_Handle_t radioHandle, EZRADIODRV_ReplyHandle_t radioReplyHandle);
#endif

static void ezradioPowerUp(void);

static void GPIO_EZRadio_INT_IRQHandler(uint8_t pin);

/// @endcond

/**************************************************************************//**
 * @brief  Radio Initialization.
 *
 * @param[in] handle EzRadio driver instance handler.
 *****************************************************************************/
void ezradioInit(EZRADIODRV_Handle_t handle)
{
  (void)handle;

  /* Initialize radio GPIOs and SPI port */
#if defined(EZRADIO_PLUGIN_PTI)
  ezradio_hal_GpioInit(GPIO_EZRadio_INT_IRQHandler, true);
#else
  ezradio_hal_GpioInit(GPIO_EZRadio_INT_IRQHandler, false);
#endif
  ezradio_hal_SpiInit();

  /* Power Up the radio chip */
  ezradioPowerUp();

  /* Load radio configuration */
  while (EZRADIO_CONFIG_SUCCESS != ezradio_configuration_init(Radio_Configuration_Data_Array)) {
    /* Error hook */
#ifdef ERROR_HOOK
    ERROR_HOOK;
#else
    printf("ERROR: Radio configuration failed!\n");
#endif

    USTIMER_Init();
    /* Delay for 10ms time */
    USTIMER_Delay(10000u);
    /* Deinit ustimer */
    USTIMER_DeInit();

    /* Power Up the radio chip */
    ezradioPowerUp();
  }

  /* Read ITs, clear pending ones */
  ezradio_get_int_status(0u, 0u, 0u, NULL);
}

/**************************************************************************//**
 * @brief  EzRadio plugin manager state machine handler. This function should
 *         be called in the application in an infinite loop, in order to
 *         manage the EzRadio plug-ins properly.
 *
 * @param[in] radioHandle EzRadio driver instance handler.
 *****************************************************************************/
Ecode_t ezradioPluginManager(EZRADIODRV_Handle_t radioHandle)
{
  /* EZRadio response structure union */
  ezradio_cmd_reply_t radioReplyData;
  EZRADIODRV_ReplyHandle_t radioReplyHandle = &radioReplyData;

  if ( radioHandle == NULL ) {
    return ECODE_EMDRV_EZRADIODRV_ILLEGAL_HANDLE;
  }

  /* Check is radio interrupt is received. */
  if (ezradioIrqReceived) {
    /* Accept interrupt before clearing IT in the radio, so prevent race conditions. */
    ezradioIrqReceived = false;

    /* Read ITs, clear all pending ones */
    ezradio_get_int_status(0x0, 0x0, 0x0, radioReplyHandle);

#if defined(EZRADIO_PLUGIN_TRANSMIT)
    ezradioHandleTransmitPlugin(radioHandle, radioReplyHandle);
#endif

#if defined(EZRADIO_PLUGIN_RECEIVE)
    ezradioHandleReceivePlugin(radioHandle, radioReplyHandle);
#endif

#if defined(EZRADIO_PLUGIN_CRC_ERROR)
    ezradioHandleCrcErrorPlugin(radioHandle, radioReplyHandle);
#endif
  }

  return ECODE_EMDRV_EZRADIODRV_OK;
}

/**************************************************************************//**
 * @brief  Resets both the TX and RX FIFOs.
 *****************************************************************************/
void ezradioResetTRxFifo(void)
{
#if defined(EZRADIO_PLUGIN_RECEIVE)
  ezradio_fifo_info(EZRADIO_CMD_FIFO_INFO_ARG_FIFO_RX_BIT, NULL);
#endif //#if defined( EZRADIO_PLUGIN_RECEIVE )

#if defined(EZRADIO_PLUGIN_TRANSMIT)
  ezradio_fifo_info(EZRADIO_CMD_FIFO_INFO_ARG_FIFO_TX_BIT, NULL);
#endif //#if defined( EZRADIO_PLUGIN_TRANSMIT )
}

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/**************************************************************************//**
 * @brief  Power up the Radio.
 *****************************************************************************/
static void ezradioPowerUp(void)
{
  /* Hardware reset the chip */
  ezradio_reset();

  /* Initialize ustimer */
  USTIMER_Init();
  /* Delay for preconfigured time */
  USTIMER_Delay(RADIO_CONFIG_DATA_RADIO_DELAY_AFTER_RESET_US);
  /* Deinit ustimer */
  USTIMER_DeInit();
}

/**************************************************************************//**
 * @brief  Radio nIRQ GPIO interrupt.
 *****************************************************************************/
static void GPIO_EZRadio_INT_IRQHandler(uint8_t pin)
{
  (void)pin;

  /* Sign radio interrupt received */
  ezradioIrqReceived = true;
}

/// @endcond

/******** THE REST OF THE FILE IS DOCUMENTATION ONLY !**********************//**
 * @addtogroup ezradiodrv EZRADIODRV - EzRadio Driver
 * @brief EZR32 EzRadio Peripheral Interface Driver
 * @{

   @details
   The source files for the EzRadio driver library resides in the
   emdrv/ezradiodrv folder. The file hierarchy is shown in the
   @ref ezradiodrv_file_list.

   - @ref ezradiodrv_intro
   - @ref ezradiodrv_file_list
   - @ref ezradiodrv_layers
     - @ref ezradiodrv_plugin
       - @ref ezradiodrv_plugin_conf
       - @ref ezradiodrv_plugin_api
         - @ref ezradiodrv_plugin_manager
         - @ref ezradiodrv_plugin_transmit
         - @ref ezradiodrv_plugin_receive
         - @ref ezradiodrv_plugin_auto_ack
         - @ref ezradiodrv_plugin_rf_test
       - @ref ezradiodrv_plugin_example
     - @ref ezradiodrv_api
       - @ref ezradiodrv_api_example
     - @ref ezradiodrv_comm
     - @ref ezradiodrv_hal

   @n @section ezradiodrv_intro Introduction
   The EzRadio driver supports the on-chip EzRadio radio of EZR32 devices.
   The driver system provides several access layer levels to the radio. In the
   order of hierarchy they are the @ref ezradiodrv_plugin, the
   @ref ezradiodrv_api, the @ref ezradiodrv_comm and the @ref ezradiodrv_hal.
   Although, all these layers are supported and documented here, it is highly
   advised to use the plugin system as the primary access to the radio, since
   this is the highest and easiest interface to it.

   @n @section ezradiodrv_file_list File List

   - @ref ezradiodrv_plugin
     - @ref ezradiodrv_plugin_manager
       - plugins/src/ezradio_plugin_manager.c
       - plugins/inc/ezradio_plugin_manager.h
     - @ref ezradiodrv_plugin_transmit
       - plugins/src/ezradio_transmit_plugin.c
       - plugins/inc/ezradio_transmit_plugin.h
     - @ref ezradiodrv_plugin_receive
       - plugins/src/ezradio_receive_plugin.c
       - plugins/inc/ezradio_receive_plugin.h
       - plugins/src/ezradio_crcerror_plugin.c
       - plugins/inc/ezradio_crcerror_plugin.h
     - @ref ezradiodrv_plugin_auto_ack
       - plugins/src/ezradio_auto_ack_plugin.c
       - plugins/inc/ezradio_auto_ack_plugin.h
     - @ref ezradiodrv_plugin_rf_test
       - plugins/src/ezradio_unmod_carrier_plugin.c
       - plugins/inc/ezradio_unmod_carrier_plugin.h
       - plugins/src/ezradio_pn9_plugin.c
       - plugins/inc/ezradio_pn9_plugin.h
       - plugins/src/ezradio_direct_transmit_plugin.c
       - plugins/inc/ezradio_direct_transmit_plugin.h
       - plugins/src/ezradio_direct_receive_plugin.c
       - plugins/inc/ezradio_direct_receive_plugin.h
   - @ref ezradiodrv_api
     - common/src/ezradio_api_lib.c
     - common/inc/ezradio_api_lib.h
     - Si4x55
       - common/src/si4x55/ezradio_api_lib_add.c
       - common/inc/si4x55/ezradio_api_lib_add.h
       - Si4455
         - si4455/inc/ezradio_cmd.h
         - si4455/inc/ezradio_prop.h
     - Si4x6x
       - common/src/si4x6x/ezradio_api_lib_add.c
       - common/inc/si4x6x/ezradio_api_lib_add.h
       - Si4460
         - si4460/inc/ezradio_cmd.h
         - si4460/inc/ezradio_prop.h
       - Si4461
         - si4461/inc/ezradio_cmd.h
         - si4461/inc/ezradio_prop.h
       - Si4463
         - si4463/inc/ezradio_cmd.h
         - si4463/inc/ezradio_prop.h

   - @ref ezradiodrv_comm
     - common/src/ezradio_comm.c
     - common/inc/ezradio_comm.h
   - @ref ezradiodrv_hal
     - common/src/ezradio_hal.c
     - common/inc/ezradio_hal.h


   @n @section ezradiodrv_layers Driver Layers
   In the following subsections the different layers of the EzRadio driver
   hierarchy are shown. The @ref ezradiodrv_plugin is in the highest and the
   @ref ezradiodrv_hal is in the lowest level of the hierarchy.

   @n @subsection ezradiodrv_plugin Plugin System Layer
    This is the highest level control layer of the on-chip EzRadio device. In
    normal scenarios it is enough to use only this layer to manage and control
    the operation of the radio. In some special scenarios you might consider using
    lower level driver functionalities, such as @ref ezradiodrv_api,
    @ref ezradiodrv_comm or even @ref ezradiodrv_hal.
    The main concept of the EzRadio plugin system is to let the user easily
    configure and manage high level radio operations such as transmission,
    reception, CRC error or auto acknowledge management. Each functionalities
    provided by plugins can be used side by side.

    @subsubsection ezradiodrv_plugin_conf Plugin Manager Configuration
      There are two main compile-time configuration inputs of the plugin manager,
      they are the <b>plugin configuration</b> and the  <b>radio configuration</b>.
      @n The <b>plugin configuration</b> is a set of definitions that enables the required
      plugins. Plugin configuration normally resides in the app-config.h header
      file and generated by Simplicity Studio.
      @n The <b>radio configuration</b> is a set of generated configurations that configures
      the radio for the required operations. Radio configuration normally resides
      in a radio-config-wds-gen.h header file and generated by Simplicity Studio.

      Simplicity Studio can generate both the app-config.h and the
      radio-config-wds-gen.h header files using the EzRadio Configurator tool. You
      can use this tool to configure either the plugin system and the radio according
      to your needs.

      @note Each EzRadio sample application contains the EzRadio Configurator tool
      (radio-configurator_*.isc) and pre-generated configuration files with default
      values.

    @subsubsection ezradiodrv_plugin_api Plugin System API
      This section contains brief descriptions of the functions in the API of
      each plugins. You will find detailed information on input and output
      parameters and return values by clicking on the hyperlinked function names.
      Most functions return an error code, @ref ECODE_EMDRV_EZRADIODRV_OK is
      returned on success, see @ref ecode.h and @ref ezradio_plugin_manager.h
      for other error codes.

      @paragraph ezradiodrv_plugin_manager Plugin Manager
        The core of the plugin system is the plugin manager that handles the
        behavior of the enabled plugins.

        ezradioInit() @n
          This function initializes or deinitializes the EZRADIODRV driver.
          Typically @c ezradioInit() is called once in your startup code. Each
          enabled plugin is configured through its input parameter.

        ezradioPluginManager() @n
          This function manages the behavior of each enabled plugin. This has
          to be called in a cyclic manner in your code. It can be either
          repeated asyncronously or in a timed fashion.

      @paragraph ezradiodrv_plugin_transmit Transmit Plugin
          Start transmission functions. @n
        ezradioStartTransmitBasic(),  ezradioStartTransmitConfigured(),
        ezradioStartTransmitCustom(), ezradioStartTransmitDefault() and
        ezradioStartTransmitSmart() @n
          Each provides different level of configuration for the transmission.
          Each starts an asynchronous transmission. The registered callback function
          is called at the end of the transmission.

      @paragraph ezradiodrv_plugin_receive Receive & CRC Error Plugins
        ezradioStartRx() @n
          Starts an asynchronous reception. The registered callback function
          is called at the end of the transmission.
          @n If the CRC Error plugin is enabled its configured callback is
          called in case of a packet is received with CRC error.

      @paragraph ezradiodrv_plugin_auto_ack Auto Acknowledge Plugin
        ezradioEnableAutoAck(), ezradioDisableAutoAck() @n
          These functions enables or disables auto acknowledge transmission.
          The ACK packet is sent asynchronously after a properly received packet.

        ezradioSkipAutoAck() @n
          This function can be used to issue the plugin manager to skip transmit
          the acknowledge packet for the next cycle. Has to be used in links where
          both nodes transmits an ACK packet. See the corresponding example for
          more information.

      @paragraph ezradiodrv_plugin_rf_test RF Test Plugins
        This paragraph covers the Unmodulated Carrier, PN9, Direct Transmit and
        Direct Receive plugins. These plugins are used to issue transmission or
        reception that is mostly used to test the RF parameters of the radio.

        ezradioStartUnmodulatedCarrier(), ezradioStopUnmodulatedCarrier() @n
          These functions starts and stops transmitting an unmodulated carrier
          signal.

        ezradioStartPn9(), ezradioStopPn9() @n
          These functions starts and stops transmitting an RF signal that is
          modulated by the output of the on-chip pseudo-random number generator.

        ezradioStartDirectTransmit(), ezradioStopDirectTransmit() @n
          These functions starts and stops a direct transmission, where the
          carrier signal is modulated by the configured data source input.

        ezradioStartDirectReceive(), ezradioStopDirectReceive() @n
          These functions starts and stops a direct reception.


   @subsubsection ezradiodrv_plugin_example Plugin System Example
   @verbatim
 #include "ezradio_plugin_manager.h"

   //Buffers for packet reception and transmission.
   uint8_t radioRxPkt[EZRADIO_FIFO_SIZE];
   uint8_t radioTxPkt[EZRADIO_FIFO_SIZE] = RADIO_CONFIG_DATA_CUSTOM_PAYLOAD;

   //Packet transmit user callback.
   void appPacketTransmittedCallback ( EZRADIODRV_Handle_t handle, Ecode_t status ) {
     if ( status == ECODE_EMDRV_EZRADIODRV_OK ) {
      printf("Packet sent.\n");

      //Start reception after packet is sent properly.
      ezradioStartRx( handle );
     }
   }

   //Packet receive user callback.
   void appPacketReceivedCallback ( EZRADIODRV_Handle_t handle, Ecode_t status ) {
     if ( status == ECODE_EMDRV_EZRADIODRV_OK ) {
      printf("Packet received.\n");
     }
   }

   int main(void)
   {
     //Initialize EzRadio handler with default data.
     EZRADIODRV_HandleData_t appRadioInitData = EZRADIODRV_INIT_DEFAULT;

     //Add user callbacks for transmission and reception.
     appRadioInitData.packetTx.userCallback = &appPacketTransmittedCallback;
     appRadioInitData.packetRx.userCallback = &appPacketReceivedCallback;

     //Initialize EzRadio device.
     ezradioInit( &appRadioInitData );

     //Start a default transmission.
     ezradioStartTransmitDefault( &appRadioInitData, radioTxPkt );

     while (1) {} ;
   }
   @endverbatim

   @n @subsection ezradiodrv_api Radio API Layer

    The second radio access layer is the radio API layer. Basically, this layer
    implements the radio API documentation. Every command and property access
    that is documented in the radio API documentation is accessible through the
    radio API layer. @n
    There are two main scenarios where the radio API layer is useful for the
    user. The first is where a specific radio API command has to be called and
    the second is where a specific radio API property or several properties have
    to be written or read. @n
    In the ezradio_api.c and ezradio_api.h files contain those APIs that are
    common through all the supported parts. Also, @ref ezradio_api_lib_add.c and
    @ref ezradio_api_lib_add.h files contain those APIs that are specific to the
    corresponding radio. @n
    The auto generated command and property lists can be found in the
    @ref ezradio_cmd.h and @ref ezradio_prop.h files per part. @n
    For more information please refer to the radio API documentation.

   @subsubsection ezradiodrv_api_example Radio API Layer Example
   @verbatim
   // EZRadio response structure union
   ezradio_cmd_reply_t ezradioReply;

   // Print EZRadio device number.
   ezradio_part_info(&ezradioReply);
   printf("   Device: Si%04x\n\n", ezradioReply.PART_INFO.PART);

   // Read radio interrupts, clear pending ones
   ezradio_get_int_status(0u, 0u, 0u, NULL);
   @endverbatim

   @n @subsection ezradiodrv_comm Communication Layer

    The third layer is the radio communication layer. It implements the
    specialities of the EzRadio SPI communication and contains APIs that hide
    the direct SPI communication from higher layers. This layer is rarely used
    alone, it is advised to use higher level layers instead.

   @n @subsection ezradiodrv_hal Radio HAL Layer

    The fourth layer is the radio HAL layer. It implements direct SPI and radio
    pin management functionalities. This layer is rarely used alone, it is
    advised to use higher level layers instead.


 * @} end group ezradiodrv ****************************************************/
