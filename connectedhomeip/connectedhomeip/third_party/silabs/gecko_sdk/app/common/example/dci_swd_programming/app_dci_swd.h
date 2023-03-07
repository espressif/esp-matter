/***************************************************************************//**
 * @file app_dci_swd.h
 * @brief Low level DCI and SWD interface functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef APP_DCI_SWD_H
#define APP_DCI_SWD_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "em_gpio.h"
#include "app_swd_task.h"
#include "app_prog_error.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// JTAG to SWD bit sequence, transmitted LSB first
#define JTAG_TO_SWD     (0xE79E)

/// ACK responses
#define ACK_OK          1
#define ACK_WAIT        2
#define ACK_FAULT       4

/// Address of DP read registers
#define DP_IDCODE       0
#define DP_CTRL         1
#define DP_RESEND       2
#define DP_RDBUFF       3

/// Addresses of DP write registers
#define DP_ABORT        0
#define DP_STAT         1
#define DP_SELECT       2

/// AHB-AP registers
#define AP_CSW          0
#define AP_TAR          1
#define AP_DRW          3
#define AP_IDR          3       // In bank 0xf

/// Write command to DHCSR to stop CPU run
#define STOP_CMD                (0xA05F0003)

/// Valid values for the DP IDCODE register
#define S2_DP_ID                (0x6BA02477)

/// Valid values for the AHB-AP IDR register
#define S2_AHBAP_ID             (0x84770001)

/// Value to write to AIRCR in order to do a soft reset of the target
#define AIRCR_RESET_CMD         (0x05FA0006)

/// Bit fields for the CSW register
#define AP_CSW_32BIT_TRANSFER   (0x02)
#define AP_CSW_AUTO_INCREMENT   (0x10)
#define AP_CSW_MASTERTYPE_DEBUG (1 << 29)
#define AP_CSW_HPROT            (1 << 25)
#define AP_CSW_DEFAULT          (AP_CSW_32BIT_TRANSFER | AP_CSW_MASTERTYPE_DEBUG | AP_CSW_HPROT)

/// Bit fields for the ABORT register
#define DP_ABORT_ORUNERRCLR     (1U << 4)
#define DP_ABORT_WDERRCLR       (1U << 3)
#define DP_ABORT_STKERRCLR      (1U << 2)
#define DP_ABORT_STKCMPCLR      (1U << 1)

/// Power up request and acknowledge bits in CTRL/STAT
#define DP_CTRL_CDBGPWRUPREQ    (1U << 28)
#define DP_CTRL_CDBGPWRUPACK    (1U << 29)
#define DP_CTRL_CSYSPWRUPREQ    (1U << 30)
#define DP_CTRL_CSYSPWRUPACK    (1U << 31)

/// Number of times to retry a SWD operation when receiving a WAIT response
#define SWD_RETRY_COUNT         200

/// Number of times to retry reading status registers while waiting for a debug event (such as a halt of soft reset)
#define DEBUG_EVENT_TIMEOUT     200

/// Number of times to retry reading the CTLR/STAT register while waiting for power up acknowledge
#define PWRUP_TIMEOUT   100

/// Number of times to wait for MSC operation to complete
#define MSC_TIMEOUT     100

/// Pin reset pulse width in microseconds
#define RESET_PULSE     1000

/// Delay in microseconds for soft or hard reset
#define RESET_DELAY     50000

/// Delay in microseconds when switching between DCI and SWD
#define SWITCH_DELAY    50000

/// DCI AP register bank 0
#define DCI_AP_REG      (0x01000000)

/// DCI register to write command
#define DCI_WDATA       (0x00001000)

/// DCI register to read response
#define DCI_RDATA       (0x00001004)

/// DCI register to read status
#define DCI_STATUS      (0x00001008)

/// Response from the DCI is valid
#define RDATAVALID      (0x0100)

/// Write Request to the DCI is pending
#define WPENDING        (0x01)

/// Number of times to retry a DCI operation
#define DCI_RETRY_COUNT (1001000)

/// SWCLK & SWDIO must on same GPIO port and RESET is NOT connected to this port
#define SWCLK_PORT      3       // PD02
#define SWCLK_PIN       2
#define SWDIO_PORT      3       // PD03
#define SWDIO_PIN       3
#define RESET_PORT      2       // PC03
#define RESET_PIN       3

/// Mask values to set and clear SWDIO and SWCLK
#define SWDIO_SET_CLR_MASK      (1 << SWDIO_PIN)
#define SWCLK_SET_CLR_MASK      (1 << SWCLK_PIN)
#define SWD_SWC_SET_CLR_MASK    SWDIO_SET_CLR_MASK | SWCLK_SET_CLR_MASK

/// Mask values to set SWDIO to input and output
#define SWDIO_INPUT_MASK_LOW    (0x01 << (4 * SWDIO_PIN)) | (0x04 << (4 * SWCLK_PIN))
#define SWDIO_OUTPUT_MASK_LOW   (0x04 << (4 * SWDIO_PIN)) | (0x04 << (4 * SWCLK_PIN))

/// Set SWDIO to input
#define SWDIO_SET_INPUT()                              \
  do {                                                 \
    GPIO->P[SWDIO_PORT].MODEL = SWDIO_INPUT_MASK_LOW;  \
    GPIO->P_CLR[SWDIO_PORT].DOUT = SWDIO_SET_CLR_MASK; \
  } while (0)

/// Set SWDIO to output
#define SWDIO_SET_OUTPUT()                             \
  do {                                                 \
    GPIO->P[SWDIO_PORT].MODEL = SWDIO_OUTPUT_MASK_LOW; \
    GPIO->P_CLR[SWDIO_PORT].DOUT = SWDIO_SET_CLR_MASK; \
  } while (0)

/// Set and clear SWDIO and SWCLK
#define SWDIO_SET() GPIO->P_SET[SWDIO_PORT].DOUT = SWDIO_SET_CLR_MASK
#define SWDIO_CLR() GPIO->P_CLR[SWDIO_PORT].DOUT = SWDIO_SET_CLR_MASK
#define SWCLK_SET() GPIO->P_SET[SWCLK_PORT].DOUT = SWCLK_SET_CLR_MASK
#define SWCLK_CLR() GPIO->P_CLR[SWCLK_PORT].DOUT = SWCLK_SET_CLR_MASK

/// Set SWCLK to low then high
#define SWCLK_CYCLE() \
  do {                \
    SWCLK_CLR();      \
    SWCLK_SET();      \
  } while (0)

/// Set SWDIO to high then low
#define SWDIO_CYCLE() \
  do {                \
    SWDIO_SET();      \
    SWDIO_CLR();      \
  } while (0)

/// Write one bit to SWDIO per SWCLK cycle, data writes to the entire GPIO port
#define WRITE_BIT(bit)                                 \
  do {                                                 \
    if ( bit ) {                                       \
      GPIO->P[SWDIO_PORT].DOUT = SWDIO_SET_CLR_MASK;   \
      GPIO->P[SWDIO_PORT].DOUT = SWD_SWC_SET_CLR_MASK; \
    } else {                                           \
      GPIO->P[SWDIO_PORT].DOUT = 0;                    \
      GPIO->P[SWDIO_PORT].DOUT = SWCLK_SET_CLR_MASK;   \
    }                                                  \
  } while (0)

/// Read one bit from SWDIO per SWCLK cycle, data writes to the entire GPIO port
#define READ_BIT(bit)                                     \
  do {                                                    \
    GPIO->P[SWCLK_PORT].DOUT = 0;                         \
    bit = ((GPIO->P[SWDIO_PORT].DIN >> SWDIO_PIN) & 0x1); \
    GPIO->P[SWCLK_PORT].DOUT = SWCLK_SET_CLR_MASK;        \
  } while (0)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Sends the JTAG-to-SWD sequence.
 *
 * @note This must be performed at the very beginning of every debug session and
 *       again in case of a protocol error.
 ******************************************************************************/
void jtag_to_swd_sequence(void);

/***************************************************************************//**
 * Writes to one of the four AP registers in the currently selected AP bank.
 *
 * @param reg The register number [0-3] to write to.
 * @param data The value to write to the register.
 ******************************************************************************/
void write_ap(int reg, uint32_t data);

/***************************************************************************//**
 * Writes to one of the four DP registers.
 *
 * @param reg The register number [0-3] to write to.
 * @param data The value to write to the register.
 ******************************************************************************/
void write_dp(int reg, uint32_t data);

/***************************************************************************//**
 * Reads one of the four AP registers in the currently selected AP bank.
 *
 * @param reg The register number [0-3] to read from.
 * @param data Pointer to parameter the register value is written to.
 ******************************************************************************/
void read_ap(int reg, uint32_t *data);

/***************************************************************************//**
 * Reads one of the four DP registers.
 *
 * @param reg The register number [0-3] to read from.
 * @param data Pointer to parameter the register value is written to.
 ******************************************************************************/
void read_dp(int reg, uint32_t *data);

/***************************************************************************//**
 * Initialized the SW-DP.
 *
 * @returns The value of the IDCODE register.
 *
 * @note This function first sends the JTAG-to-SWD sequence and then reads the
 *       IDCODE register.
 ******************************************************************************/
uint32_t init_dp(void);

/***************************************************************************//**
 * Reads the ID of AP #0 - This will be S2 AHBAP ID if device is unlocked.
 *
 * @returns The value of IDR register (address 0xFC) for AP #0.
 ******************************************************************************/
uint32_t read_ap_id(void);

/***************************************************************************//**
 * Halts the target device.
 ******************************************************************************/
void halt_target(void);

/***************************************************************************//**
 * Resets the target device by using the AIRCR register (does not reset the
 * debug interface).
 ******************************************************************************/
void soft_reset_target(void);

/***************************************************************************//**
 * Resets the target device by using the RESET pin.
 ******************************************************************************/
void hard_reset_target(void);

/***************************************************************************//**
 * Connect to the DCI.
 ******************************************************************************/
void connect_to_dci(void);

/***************************************************************************//**
 * Write DCI register to pass commands to the SE.
 *
 * @param command Pointer to command buffer.
 ******************************************************************************/
void write_dci_command(uint32_t *command);

/***************************************************************************//**
 * Read DCI register to retrieve responses from the SE.
 *
 * @param resp Pointer to response buffer.
 ******************************************************************************/
void read_dci_response(uint32_t *resp);

/***************************************************************************//**
 * Initialize the AHB-AP.
 *
 * @note The transfer size must be set to 32-bit before trying to access any
 *       internal memory.
 ******************************************************************************/
__STATIC_INLINE void init_ahb_ap(void)
{
  // Set transfer size to 32 bit
  write_ap(AP_CSW, AP_CSW_DEFAULT);
}

/***************************************************************************//**
 * Reads one word from internal memory.
 *
 * @param addr The address to read from.
 * @returns The value at @param addr
 ******************************************************************************/
__STATIC_INLINE uint32_t read_mem(uint32_t addr)
{
  uint32_t ret;

  write_ap(AP_TAR, addr);
  read_ap(AP_DRW, &ret);
  read_dp(DP_RDBUFF, &ret);
  return ret;
}

/***************************************************************************//**
 * Writes one word to internal memory.
 *
 * @param addr The address to write to.
 * @param data The value to write.
 ******************************************************************************/
__STATIC_INLINE void write_mem(uint32_t addr, uint32_t data)
{
  write_ap(AP_TAR, addr);
  write_ap(AP_DRW, data);
}

#endif  // APP_DCI_SWD_H
