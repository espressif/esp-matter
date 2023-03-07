/***************************************************************************//**
 * @file
 * @brief  EFM internal SPI Protocol implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "serial/serial.h"
#include "../inc/spi-protocol.h"
#include "spidrv.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "../inc/spi-protocol-device.h"
//#include "sl_spi_ncp_config.h"

// Buffer for RX data (sent from the Host to the NCP)
static uint8_t commandBuffer[SPIP_BUFFER_SIZE];
// Buffer for TX data (to send from the NCP to the host)
static uint8_t responseBuffer[SPIP_BUFFER_SIZE];
// Buffer for override frame to send in response to an error or reset
static uint8_t spipErrorResponseBuffer[SPIP_ERROR_RESPONSE_SIZE];

// Provide the stack with its own buffer to isolate it from the lower level SPIP
// buffers. We keep the old pointer (halHostFrame) for type compatibility.
uint8_t stackBuffer[SPIP_MAX_WITH_PADDING];
uint8_t * halHostFrame = stackBuffer;

// Keep track of where we think the host data starts in commandBuffer.
// The vast majority of the time, this should be at the beginning (offset 0).
// However, in some rare cases (notably when our nSSEL ISR cannot run for an
// extended period of time that corresponds to a packet transition), the command
// bytes may be pulled out of the USART peripheral by the SPIDRV transfer that
// was set up to transmit the previous response. This means the command will
// start later in the buffer.
static uint32_t commandOffset = 0;

// legacy support
bool spipFlagWakeFallingEdge;  //flag for detecting a falling edge on nWAKE

struct {
  volatile enum { spipNcpIdle = 0,     \
                  spipNcpCommand = 1,  \
                  spipNcpWait = 2,     \
                  spipNcpResponse = 3, \
                  spipNcpDone = 4        } state;
  bool overrideResponse;      //flag for indicating errors or SPIP boot
  bool wakeup;                //flag for detecting a falling edge on nWAKE
  bool idleHostInt;           //flag for idling nHOST_INT at proper time
  uint8_t responseLength; //The true length of the Response Section
} spipNcpState;

static SPIDRV_HandleData_t spiHandleData;
static SPIDRV_Handle_t spiHandle = &spiHandleData;

static bool halInternalHostSerialTick(bool responseReady);
static void nSSEL_ISR(uint8_t pin);
static void nWAKE_ISR(uint8_t pin);
static void processSpipCommandAndRespond(uint8_t spipResponse);
static void setSpipErrorBuffer(uint8_t spiByte);

void halHostCallback(bool haveData)
{
  if (haveData) {
    //only assert nHOST_INT if we are outside a wake handshake (wake==1)
    //and outside of a current transaction (nSSEL=1)
    //if inside a wake handshake or transaction, delay asserting nHOST_INT
    //until the SerialTick
    if ( nWAKE_IS_NEGATED() && nSSEL_IS_NEGATED() ) {
      CLR_nHOST_INT();
    }
    spipNcpState.idleHostInt = false;
  } else {
    spipNcpState.idleHostInt = true;
  }
}

bool halHostSerialBusy(void)
{
  return ((nSSEL_IS_ASSERTED()) || (spipNcpState.state >= spipNcpWait));
}

void halHostSerialInit(void)
{
  ////---- Initialize Flags ----////
  spipFlagWakeFallingEdge = false; //start with no edge on nWAKE
  spipNcpState.idleHostInt = true; //idle nHOST_INT after telling host we booted
  //load error response buffer with the "EM260 Reset" message + reset cause
  //we do not use the setSpipErrorBuffer() function here since that function
  //assumes the second byte in the buffer is reserved (0)
  spipNcpState.overrideResponse = true; //set a flag indicating we just booted
  spipErrorResponseBuffer[0] = SPIP_RESET;
  spipErrorResponseBuffer[1] = halGetEm2xxResetInfo(); //inject reset cause
  spipNcpState.responseLength = 0;           //default length of zero

  MEMSET(commandBuffer, 0xFF, SPIP_BUFFER_SIZE);
  MEMSET(responseBuffer, 0xFF, SPIP_BUFFER_SIZE);

  halHostSerialPowerup();
}

void halHostSerialPowerup(void)
{
  //---- Configure SPI ----//
  SPIDRV_Init_t initData = SPI_NCP_USART_INIT;
  SPIDRV_Init(spiHandle, &initData);

  // Initialize nHOST_INT as output
  GPIO_PinModeSet(BSP_SPINCP_NHOSTINT_PORT,
                  BSP_SPINCP_NHOSTINT_PIN,
                  gpioModePushPull,
                  1);

  GPIOINT_Init();

  #if (!defined(DISABLE_NWAKE)) && (!defined(HAL_CONFIG) || defined(BSP_SPINCP_NWAKE_PIN))
  // Disable the interrupt before configuration in case there's a conflict in
  // interrupt numbering.
  GPIO_IntDisable(1 << BSP_SPINCP_NWAKE_PIN);
  // Initialize nWAKE as input with falling edge interrupt.
  GPIO_PinModeSet(BSP_SPINCP_NWAKE_PORT,
                  BSP_SPINCP_NWAKE_PIN,
                  gpioModeInputPullFilter,
                  1);
  GPIO_ExtIntConfig(BSP_SPINCP_NWAKE_PORT,
                    BSP_SPINCP_NWAKE_PIN,
                    BSP_SPINCP_NWAKE_PIN,
                    false,
                    true,
                    true);
  GPIOINT_CallbackRegister(BSP_SPINCP_NWAKE_PIN, nWAKE_ISR);
  #endif

  // Disable the interrupt before configuration in case there's a conflict in
  // interrupt numbering.
  GPIO_IntDisable(1 << SPI_NCP_CS_PIN);
  // Initialize nSSEL as input with rising/falling edge interrupts
  GPIO_PinModeSet(SPI_NCP_CS_PORT, SPI_NCP_CS_PIN, gpioModeInputPullFilter, 1);
  GPIOINT_CallbackRegister(SPI_NCP_CS_PIN, nSSEL_ISR);
  GPIO_ExtIntConfig(SPI_NCP_CS_PORT, SPI_NCP_CS_PIN, SPI_NCP_CS_PIN, true, true, true);

  // ----- Account for Noise and Crosstalk ------ //
  // on some hardware configurations there is a lot of noise and bootloading can fail
  // due to crosstalk. to avoid this, the slewrate is lowered here from 6 to 4, and the
  // drivestrength is lowered from 10mA to 1mA. if noise related errors still occur,
  // the slewrate can be lowered further
  GPIO_SlewrateSet(BSP_SPINCP_NHOSTINT_PORT, 4, 4);
  // the drivestrength is lowered from 10mA to 1mA by setting DRIVESTRENGTH to 1
#if defined (_GPIO_P_CTRL_DRIVEMODE_MASK)
  GPIO_DriveStrengthSet(BSP_SPINCP_NHOSTINT_PORT, 1);
#endif
}

void halHostSerialPowerdown(void)
{
  //we need to block (interrupts are off), until transmission is done
  while (nSSEL_IS_ASSERTED()) {
    halResetWatchdog();
  }

  // Disable the interrupt on CS first so that it won't be triggered by the pin
  // configuration done by SPIDRV_DeInit().
  GPIO_IntDisable(1 << SPI_NCP_CS_PIN);
  // Deinitialize spidrv to remove requirement on em1.
  SPIDRV_DeInit(spiHandle);
}

bool halHostSerialTick(bool responseReady)
{
  bool validCommand;
  //Processing a potential premature nSSEL deactivation inside of the Tick
  //function will generate a lot of strange conditions that are best prevented
  //insteaded of handled.
  //Normal calls to halInternalHostSerialTick are <10us.  Worst case is <30us.
  {
    DECLARE_INTERRUPT_STATE;
    DISABLE_INTERRUPTS();
    validCommand = halInternalHostSerialTick(responseReady);
    RESTORE_INTERRUPTS();
  }

  return validCommand;
}

void halNcpClearWakeFlag(void)
{
  spipFlagWakeFallingEdge = false;
}

static uint8_t getHostByte(uint32_t index)
{
  return commandBuffer[commandOffset + index];
}

static uint8_t getHostCommandLength(void)
{
  return getHostByte(SPIP_LENGTH_INDEX) + SPIP_OVERHEAD;
}

static void shiftHostCommand(void)
{
  uint32_t lcv;
  uint32_t length = getHostCommandLength();

  // Verify that the computed start and length won't make us try to index off
  // the end of the buffer. This _should_ already be guaranteed by the size of
  // the buffer and earlier (non-assert) checks, so violating this is a
  // programming error and needs to be looked at more closely (it could also
  // indicate the start or length have changed due to memory corruption since
  // they were checked).
  assert(commandOffset + length <= SPIP_BUFFER_SIZE);

  // Shift the received command so it's where the stack expects it
  for (lcv = 0; lcv < length; lcv++) {
    halHostFrame[lcv] = getHostByte(lcv + SPIP_LENGTH_INDEX);
  }

  commandOffset = 0;
}

static int itemsTransferred, itemsRemaining;
static bool findHostCommand(void)
{
  bool validCommand = false;

  SPIDRV_GetTransferStatus(spiHandle, &itemsTransferred, &itemsRemaining);

  // Skip any leading idle bytes. These could be present because the host is
  // actually sending idle bytes, but it's more likely that we weren't able to
  // turn the SPI driver around from the previous response transmission to start
  // a new receive in time for this command and we're processing those idle
  // bytes. We keep looking until we find a non-idle byte, we run out of data,
  // or the message is potentially too big to fit in our buffer.
  while (getHostByte(SPIP_PREFIX_INDEX) == 0xFF
         && commandOffset < (uint32_t)itemsTransferred
         && commandOffset < (uint32_t)SPIP_MAX_WITH_PADDING) {
    commandOffset++;
  }

  // Are we still waiting for the length byte? If so, try again later after the
  // host has sent more data
  if ((uint32_t)itemsTransferred <= (commandOffset + SPIP_LENGTH_INDEX)) {
    return validCommand;
  }

  switch (getHostByte(SPIP_PREFIX_INDEX)) {
    case 0x0A:
      processSpipCommandAndRespond(SPIP_VERSION);
      break;
    case 0x0B:
      processSpipCommandAndRespond(SPIP_ALIVE);
      break;
    case 0xFD: //The Command is a Bootloader Frame
    //Fall into EZSP Frame since processing the rest of the command is
    //the same. The only difference is responding with the Unsupported
    //SPI Command error
    case 0xFE: //The Command is an EZSP Frame
      if (getHostByte(SPIP_LENGTH_INDEX) > MAX_PAYLOAD_FRAME_LENGTH) {
        // If the received length is too big, send an error response.
        setSpipErrorBuffer(SPIP_OVERSIZED_EZSP);
        spipNcpState.state = spipNcpWait;
        halInternalHostSerialTick(true); //respond immediately!
      } else if (((uint32_t)itemsTransferred) >= (getHostCommandLength() + commandOffset)) {
        // The above check means that we've gotten as many bytes of the command
        // as we expect to given the length field, so stop receiving and start
        // processing it.
        SPIDRV_AbortTransfer(spiHandle);

        // We enable RX blocking here after the command is fully received and
        // disable it when we start sending the response (or, if the frame is
        // aborted before that point, when we start the receive for the next
        // command). See the comment when the SPIDRV transfer is started in the
        // spipNcpWait case of halInternalHostSerialTick's switch statement for
        // the rationale.
        spiHandle->peripheral.usartPort->CMD = USART_CMD_RXBLOCKEN;

        // Transition state to wait for TX buffer ready
        spipNcpState.state = spipNcpWait;

        //check for Frame Terminator, it must be there!
        if (spipNcpState.overrideResponse) {
          halInternalHostSerialTick(true); //respond immediately!
        } else if (getHostByte(getHostCommandLength() - SPIP_FRAME_TERMINATOR_SIZE)
                   != SPIP_FRAME_TERMINATOR) {
          //no frame terminator found!  report missing F.T.
          setSpipErrorBuffer(SPIP_MISSING_FT);
          halInternalHostSerialTick(true); //respond immediately!
        } else if (getHostByte(SPIP_PREFIX_INDEX) == 0xFD) {
          //load error response buffer with Unsupported SPI Command error
          setSpipErrorBuffer(SPIP_UNSUPPORTED_COMMAND);
          halInternalHostSerialTick(true); //respond immediately!
        } else {
          shiftHostCommand();
          responseBuffer[SPIP_PREFIX_INDEX] = 0xFE; //mark the response EZSP Frame
          validCommand = true; //there is a valid command
        }
      }
      break;
    case 0xFF: // Idle byte
    // Intentional fall through since 0xFF is not a supported command
    // and we weren't able to find anything else in a full message's
    // worth of bytes (which should account for the possibility
    // mentioned above that we're processing idle bytes from sending the
    // the previous response). Might also be acceptable to send
    // SPIP_OVERSIZED_EZSP
    default:
      // Load error response buffer with Unsupported SPI Command error
      setSpipErrorBuffer(SPIP_UNSUPPORTED_COMMAND);
      spipNcpState.state = spipNcpWait;
      halInternalHostSerialTick(true); //respond immediately!
      break;
  }

  return validCommand;
}

//One layer of indirection is used so calling the public function will actually
//result in the real Tick function (this internal one) being wrapped between a
//DISABLE_INTERRUPTS() and RESTORE_INTERRUPTS() to prevent potential corruption
//from the nSSEL interrupt.
static bool halInternalHostSerialTick(bool responseReady)
{
  bool validCommand = false;

  if (spipNcpState.overrideResponse && nSSEL_IS_NEGATED()) {
    CLR_nHOST_INT();
  }
  switch (spipNcpState.state) {
    case spipNcpIdle:
      itemsTransferred = 0;
      itemsRemaining = 0;
      if (spipNcpState.wakeup) {
        CLR_nHOST_INT();
        while ( nWAKE_IS_ASSERTED() ) {
          halResetWatchdog();
        }
        SET_nHOST_INT();
        spipNcpState.wakeup = false;
        //The wake handshake is complete, but spipFlagIdleHostInt is saying
        //that there is a callback pending.
        if (!spipNcpState.idleHostInt) {
          halCommonDelayMicroseconds(50); //delay 50us so Host can get ready
          //indicate the pending callback
          CLR_nHOST_INT();
        }
      }

      if (spiHandle->state == spidrvStateIdle) {
        // Clear out anything remaining in the USART's FIFOs
        spiHandle->peripheral.usartPort->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
        SPIDRV_SReceive(spiHandle,
                        commandBuffer,
                        SPIP_BUFFER_SIZE,
                        NULL,
                        SPI_NCP_TIMEOUT);
        // Disable RX blocking so we can receive the next command. See the
        // comment when the SPIDRV transfer is started in the spipNcpWait case
        // of halInternalHostSerialTick's switch statement for the rationale.
        spiHandle->peripheral.usartPort->CMD = USART_CMD_RXBLOCKDIS;
        break;
      } else if (nSSEL_IS_ASSERTED()) {
        SET_nHOST_INT();
        spipNcpState.state = spipNcpCommand;
        commandOffset = 0;
        // fall through to the spiNcpCommand case immediately
      } else {
        break;
      }
    case spipNcpCommand:
      validCommand = findHostCommand();
      break;
    case spipNcpWait:
      if (spipNcpState.idleHostInt) {
        //the nHOST_INT signal can be asynchronously
        SET_nHOST_INT();
      }
      if (responseReady == true) {
        if (spipNcpState.overrideResponse) {
          spipNcpState.overrideResponse = false; //we no longer need to override
          //override whatever was sent with the error response message
          MEMCOPY(responseBuffer,
                  spipErrorResponseBuffer,
                  SPIP_ERROR_RESPONSE_SIZE);
        }

        int previousResponseLength = spipNcpState.responseLength;
        // Set the true response length and copy bytes if needed
        if ( responseBuffer[SPIP_PREFIX_INDEX] <= SPIP_MAX_ERROR_PREFIX ) {
          // Error response.
          spipNcpState.responseLength = SPIP_ERROR_RESPONSE_SIZE
                                        + SPIP_FRAME_TERMINATOR_SIZE;
        } else if ((responseBuffer[SPIP_PREFIX_INDEX] == 0xFE)
                   || (responseBuffer[SPIP_PREFIX_INDEX] == 0xFD)) {
          // EZSP or bootloader payload.
          // Guard against oversized messages.
          assert(halHostFrame[0] <= MAX_PAYLOAD_FRAME_LENGTH);
          spipNcpState.responseLength = halHostFrame[0] + SPIP_OVERHEAD;
          MEMCOPY(responseBuffer + SPIP_LENGTH_INDEX,
                  halHostFrame,
                  halHostFrame[0] + SPIP_LENGTH_SIZE);
        } else {
          // SPIP command (VERSION or ALIVE).
          spipNcpState.responseLength = SPIP_COMMAND_SIZE
                                        + SPIP_FRAME_TERMINATOR_SIZE;
        }

        //add Frame Terminator
        responseBuffer[spipNcpState.responseLength
                       - SPIP_FRAME_TERMINATOR_SIZE] = SPIP_FRAME_TERMINATOR;

        // Clear the remainder (if any) of the previous response to prevent
        // potential data leakage.
        for (int i = spipNcpState.responseLength;
             i < previousResponseLength;
             i++) {
          responseBuffer[i] = 0xFF;
        }

        // CLEARRX and CLEARTX do what the the names imply, clearing the
        // respective FIFOs (although apparently not the shift register, so
        // there is an extra 0xFF byte sent when switching from receiving the
        // command to sending the response).
        spiHandle->peripheral.usartPort->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

        // Start a new transfer to send the response and also double as a
        // backstop receive for the next command (in case the nSSEL ISR can't
        // run in time to set up a new receive).
        (void) SPIDRV_STransfer(spiHandle,
                                responseBuffer,
                                commandBuffer,
                                SPIP_BUFFER_SIZE,
                                NULL,
                                SPI_NCP_TIMEOUT);

        // Disable RXBLOCK, which was enabled in findHostCommand or
        // processSpipCommandAndRespond after the full command was received.
        // This feature stops bytes that the USART receives from entering the RX
        // FIFO without losing count of the clock pulses like disabling the RX
        // chain would.
        // We have to do this because SPIDRV uses the count from the RX DMA to
        // determine how many bytes actually went out across the line (as
        // opposed to just being loaded into the FIFO), but it sets up the RX
        // DMA before the TX. If the host is clocking out bytes around the time
        // this happens, it's possible a byte is received and pulled out by the
        // RX DMA but the TX DMA is set up too late to get its byte sent. This
        // throws off the count and causes us to abort the transfer one byte too
        // early. Using RXBLOCK prevents this by delaying the RX until after the
        // TX DMA has been set up.
        // It's conceivable that blocking might cause the opposite problem (an
        // extra, uncounted, TX), but this is unlikely and has never been
        // observed. It would also be less impactful even if it did happen
        // because either the transfer would eventually be aborted anyway due to
        // the rising edge of the chip select line or we'd capture the next
        // command as part of the backstop receive set up above.
        spiHandle->peripheral.usartPort->CMD = USART_CMD_RXBLOCKDIS;
        spipNcpState.state = spipNcpResponse;
        // Indicate to the host that it should start clocking out the response
        CLR_nHOST_INT();
      }
      break;
    case spipNcpResponse:
      SPIDRV_GetTransferStatus(spiHandle, &itemsTransferred, &itemsRemaining);

      if (itemsTransferred > 0) {
        // Deassert nHOST_INT once a byte has been sent
        SET_nHOST_INT();
        if (itemsTransferred >= (spipNcpState.responseLength + 1)) {
          // Start a pure receive for the next command when all bytes have been
          // transmitted. The extra byte in the calculation above accounts for the
          // extra 0xFF byte sent when transitioning between command and response.
          // With a fresh receive, we won't have to search through the bytes we
          // received while transmitting our response to find the next command,
          // which decreases the probability of finding a false command byte.
          SPIDRV_AbortTransfer(spiHandle);
          SPIDRV_SReceive(spiHandle,
                          commandBuffer,
                          SPIP_BUFFER_SIZE,
                          NULL,
                          SPI_NCP_TIMEOUT);
          spipNcpState.state = spipNcpDone;
        }
      }
      break;
    case spipNcpDone:
    default:
      break;
  }

  return validCommand;
}

// nSSEL signal (rising and falling edge triggered)
static void nSSEL_ISR(uint8_t pin)
{
  // Are we in a transaction?
  if (nSSEL_IS_ASSERTED()) {
    // If we are starting a transaction, deassert nHOST_INT and reset the tick
    // state machine immediately (it's possible we missed the rising edge due to
    // interrupts being disabled).
    SET_nHOST_INT();
    spipNcpState.state = spipNcpIdle;
  } else {
    // If we are not in a transaction, we have more time to prepare for the next
    // one. We should still stop and restart the transfer as soon as possible,
    // though, so as to avoid possibly spilling over into it and interrupting a
    // receive that's already progress. We do, however, still need to get the
    // status of the previous transfer so we can detect aborted transactions.
    if (spipNcpState.state < spipNcpDone) {
      SPIDRV_AbortTransfer(spiHandle);
      SPIDRV_GetTransferStatus(spiHandle, &itemsTransferred, &itemsRemaining);

      // Clear out anything remaining in the USART's FIFOs
      spiHandle->peripheral.usartPort->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
      SPIDRV_SReceive(spiHandle,
                      commandBuffer,
                      SPIP_BUFFER_SIZE,
                      NULL,
                      SPI_NCP_TIMEOUT);

      // Disable RX blocking so we can receive the next command. See the comment
      // when the SPIDRV transfer is started in the spipNcpWait case of
      // halInternalHostSerialTick's switch statement for the rationale.
      spiHandle->peripheral.usartPort->CMD = USART_CMD_RXBLOCKDIS;

      if ((spipNcpState.state >= spipNcpResponse)
          && (itemsTransferred < spipNcpState.responseLength)) {
        setSpipErrorBuffer(SPIP_ABORTED_TRANSACTION);
      }
    }

    // Indicate to the host when we still have more to say. We check nSSEL again
    // in case it's changed.
    if (!spipNcpState.idleHostInt && nSSEL_IS_NEGATED()) {
      CLR_nHOST_INT();
    }

    spipNcpState.state = spipNcpIdle;
    halInternalHostSerialTick(false);
  }
}

// nWAKE signal (falling edge-triggered)
static void nWAKE_ISR(uint8_t pin)
{
  spipNcpState.wakeup = true;
  spipFlagWakeFallingEdge = true;
}

static void processSpipCommandAndRespond(uint8_t spipResponse)
{
  // Disable reception while processing
  SPIDRV_AbortTransfer(spiHandle);

  // We enable RX blocking here after the command is fully received and disable
  // it when we start sending the response (or, if the frame is aborted before
  // that point, when we start the receive for the next command). See the
  // comment when the SPIDRV transfer is started in the spipNcpWait case of
  // halInternalHostSerialTick's switch statement for the rationale.
  spiHandle->peripheral.usartPort->CMD = USART_CMD_RXBLOCKEN;

  //check for Frame Terminator, it must be there!
  if (getHostByte(1) == SPIP_FRAME_TERMINATOR) {
    //override with the supplied spipResponse
    responseBuffer[SPIP_PREFIX_INDEX] = spipResponse;
  } else {
    //no frame terminator found!  report missing F.T.
    setSpipErrorBuffer(SPIP_MISSING_FT);
  }
  commandOffset = 0;
  spipNcpState.state = spipNcpWait;
  halInternalHostSerialTick(true); //respond immediately!
}

static void setSpipErrorBuffer(uint8_t spiByte)
{
  if (!spipNcpState.overrideResponse) {
    //load error response buffer with the error supplied in spiByte
    spipNcpState.overrideResponse = true;      //set a flag indicating override
    spipErrorResponseBuffer[0] = spiByte; //set the SPI Byte with the error
    spipErrorResponseBuffer[1] = 0;       //this byte is currently reserved
  }
}
