/***************************************************************************//**
 * @file app_dci_swd.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_dci_swd.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Reads from an AP or DP register.
 *
 * @param ap True to read from AP register, false to read from DP register.
 * @param reg The register number [0-3] to read from.
 * @param data Pointer to parameter the register value is written to.
 * @returns Returns error code.
 ******************************************************************************/
static uint32_t read_reg(bool ap, int reg, uint32_t *data);

/***************************************************************************//**
 * Writes to an AP or DP register.
 *
 * @param ap True to write to AP register, false to write to DP register.
 * @param reg The register number [0-3] to write to.
 * @param data The value to write to the register.
 * @param ignore_ack True to ignore ACK.
 * @returns Returns error code.
 ******************************************************************************/
static uint32_t write_reg(bool ap, int reg, uint32_t data, bool ignore_ack);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Sends the JTAG-to-SWD sequence.
 ******************************************************************************/
void jtag_to_swd_sequence(void)
{
  int i;
  int b;

  SWDIO_SET_OUTPUT();

  // First reset line with > 50 cycles with SWDIO high
  SWDIO_SET();
  for (i = 0; i < 60; i++) {
    SWCLK_CYCLE();
  }

  // Transmit 16-bit JTAG-to-SWD sequence
  for (i = 0; i < 16; i++) {
    b = (JTAG_TO_SWD >> i) & 0x01;
    WRITE_BIT(b);
  }

  // Do another reset to make sure SW-DP is in reset state
  SWDIO_SET();
  for (i = 0; i < 60; i++) {
    SWCLK_CYCLE();
  }

  // Insert a 16 cycle idle period
  SWDIO_CLR();
  for (i = 0; i < 16; i++) {
    SWCLK_CYCLE();
  }
}

/***************************************************************************//**
 * Writes to one of the four AP registers in the currently selected AP bank.
 ******************************************************************************/
void write_ap(int reg, uint32_t data)
{
  uint32_t swd_status;
  uint32_t retry = SWD_RETRY_COUNT;

  do {
    swd_status = write_reg(true, reg, data, false);
    retry--;
  } while ((swd_status == SWD_ERROR_WAIT) && (retry > 0));

  if (swd_status != SWD_ERROR_OK) {
    RAISE(swd_status);
  }
}

/***************************************************************************//**
 * Writes to one of the four DP registers.
 ******************************************************************************/
void write_dp(int reg, uint32_t data)
{
  uint32_t swd_status;
  uint32_t retry = SWD_RETRY_COUNT;

  do {
    swd_status = write_reg(false, reg, data, false);
    retry--;
  } while ((swd_status == SWD_ERROR_WAIT) && (retry > 0));

  if (swd_status != SWD_ERROR_OK) {
    RAISE(swd_status);
  }
}

/***************************************************************************//**
 * Reads one of the four AP registers in the currently selected AP bank.
 ******************************************************************************/
void read_ap(int reg, uint32_t *data)
{
  uint32_t swd_status;
  uint32_t retry = SWD_RETRY_COUNT;

  do {
    swd_status = read_reg(true, reg, data);
    retry--;
  } while ((swd_status == SWD_ERROR_WAIT) && (retry > 0));

  if (swd_status != SWD_ERROR_OK) {
    RAISE(swd_status);
  }
}

/***************************************************************************//**
 * Reads one of the four DP registers.
 ******************************************************************************/
void read_dp(int reg, uint32_t *data)
{
  uint32_t swd_status;
  uint32_t retry = SWD_RETRY_COUNT;

  do {
    swd_status = read_reg(false, reg, data);
    retry--;
  } while (swd_status == SWD_ERROR_WAIT && retry > 0);

  if (swd_status != SWD_ERROR_OK) {
    RAISE(swd_status);
  }
}

/***************************************************************************//**
 * Initialized the SW-DP.
 ******************************************************************************/
uint32_t init_dp(void)
{
  uint32_t dp_id;
  uint32_t retry;
  uint32_t status;

  // Send the JTAG-to-SWD switching sequence
  jtag_to_swd_sequence();

  // Read IDCODE to get the DAP
  read_dp(DP_IDCODE, &dp_id);

  // Debug power up request
  write_dp(DP_CTRL, DP_CTRL_CSYSPWRUPREQ | DP_CTRL_CDBGPWRUPREQ);

  // Wait until we receive power up ACK
  retry = PWRUP_TIMEOUT;
  while (retry > 0) {
    read_dp(DP_CTRL, &status);
    if ((status & (DP_CTRL_CDBGPWRUPACK | DP_CTRL_CSYSPWRUPACK))
        == (DP_CTRL_CDBGPWRUPACK | DP_CTRL_CSYSPWRUPACK)) {
      break;
    }
    retry--;
  }

  // Throw error if we failed to power up the debug interface
  if ((status & (DP_CTRL_CDBGPWRUPACK | DP_CTRL_CSYSPWRUPACK))
      != (DP_CTRL_CDBGPWRUPACK | DP_CTRL_CSYSPWRUPACK)) {
    RAISE(SWD_ERROR_DEBUG_POWER);
  }

  // Select first AP bank
  write_dp(DP_SELECT, 0x00);

  // Verify that the DP returns the correct ID
  if (dp_id != S2_DP_ID) {
    RAISE(SWD_ERROR_INVALID_IDCODE);
  }
  return dp_id;
}

/***************************************************************************//**
 * Reads the ID of AP #0 - This will be S2 AHBAP ID if device is unlocked.
 ******************************************************************************/
uint32_t read_ap_id(void)
{
  uint32_t ap_id;

  // Select last AP bank
  write_dp(DP_SELECT, 0xf0);

  // Dummy read AP ID
  read_ap(AP_IDR, &ap_id);

  // Read AP ID
  read_dp(DP_RDBUFF, &ap_id);

  // Select first AP bank again
  write_dp(DP_SELECT, 0x00);

  // Verify that the AP returns the correct ID
  if (ap_id != S2_AHBAP_ID) {
    RAISE(SWD_ERROR_INVALID_IDR);
  }
  return ap_id;
}

/***************************************************************************//**
 * Halts the target device.
 ******************************************************************************/
void halt_target(void)
{
  uint32_t dhcr_state;
  int timeout = DEBUG_EVENT_TIMEOUT;

  write_ap(AP_TAR, (uint32_t)&(CoreDebug->DHCSR));
  write_ap(AP_DRW, STOP_CMD);

  do {
    write_ap(AP_TAR, (uint32_t)&(CoreDebug->DHCSR));
    read_ap(AP_DRW, &dhcr_state);
    read_dp(DP_RDBUFF, &dhcr_state);
    timeout--;
  } while (!(dhcr_state & CoreDebug_DHCSR_S_HALT_Msk) && (timeout > 0));

  if (!(dhcr_state & CoreDebug_DHCSR_S_HALT_Msk)) {
    RAISE(SWD_ERROR_TIMEOUT_HALT);
  }
}

/***************************************************************************//**
 * Resets the target device by using the AIRCR register (does not reset the
 * debug interface).
 ******************************************************************************/
void soft_reset_target(void)
{
  uint32_t dhcsr;
  int timeout = DEBUG_EVENT_TIMEOUT;

  // Clear the VC_CORERESET bit
  write_mem((uint32_t)&(CoreDebug->DEMCR), 0);

  // Do a dummy read of sticky bit to make sure it is cleared
  read_mem((uint32_t)&(CoreDebug->DHCSR));
  dhcsr = read_mem((uint32_t)&(CoreDebug->DHCSR));

  // Reset device
  write_mem((uint32_t)&(SCB->AIRCR), AIRCR_RESET_CMD);

  // Wait for reset to complete
  sl_udelay_wait(RESET_DELAY);

  // First wait until sticky bit is set, this means it is or has been in reset
  do {
    sl_udelay_wait(10);
    dhcsr = read_mem((uint32_t)&(CoreDebug->DHCSR));
    timeout--;
  } while (!(dhcsr & CoreDebug_DHCSR_S_RESET_ST_Msk) && (timeout > 0));

  // Throw error if sticky bit is never set
  if (!(dhcsr & CoreDebug_DHCSR_S_RESET_ST_Msk)) {
    RAISE(SWD_ERROR_TIMEOUT_WAITING_RESET);
  }

  // Wait for sticky bit to be cleared, out of reset if bit is cleared
  timeout = DEBUG_EVENT_TIMEOUT;
  do {
    sl_udelay_wait(10);
    dhcsr = read_mem((uint32_t)&(CoreDebug->DHCSR));
    timeout--;
  } while ((dhcsr & CoreDebug_DHCSR_S_RESET_ST_Msk) && (timeout > 0));

  // Throw error if bit is never cleared
  if (dhcsr & CoreDebug_DHCSR_S_RESET_ST_Msk) {
    RAISE(SWD_ERROR_TIMEOUT_WAITING_RESET);
  }
}

/***************************************************************************//**
 * Resets the target device by using the RESET pin.
 ******************************************************************************/
void hard_reset_target(void)
{
  GPIO_PinOutClear((GPIO_Port_TypeDef)RESET_PORT, RESET_PIN);
  sl_udelay_wait(RESET_PULSE);
  GPIO_PinOutSet((GPIO_Port_TypeDef)RESET_PORT, RESET_PIN);
  sl_udelay_wait(RESET_DELAY);
}

/***************************************************************************//**
 * Connect to the DCI.
 ******************************************************************************/
void connect_to_dci(void)
{
  uint32_t dp_id;

  // Send the JTAG-to-SWD switching sequence
  jtag_to_swd_sequence();

  // Read IDCODE to get the DAP
  read_dp(DP_IDCODE, &dp_id);

  // Verify that the DP returns the correct ID
  if (dp_id != S2_DP_ID) {
    RAISE(SWD_ERROR_INVALID_IDCODE);
  }

  // Clear error and sticky flag conditions
  write_dp(DP_ABORT, DP_ABORT_ORUNERRCLR | DP_ABORT_WDERRCLR
           | DP_ABORT_STKERRCLR | DP_ABORT_STKCMPCLR);

  // Power up system and debug
  write_dp(DP_CTRL, DP_CTRL_CSYSPWRUPREQ | DP_CTRL_CDBGPWRUPREQ);

  // Select DCI AP register bank 0
  write_dp(DP_SELECT, DCI_AP_REG);

  // Set up AP defaults
  init_ahb_ap();
}

/***************************************************************************//**
 * Write DCI register to pass commands to the SE.
 ******************************************************************************/
void write_dci_command(uint32_t *command)
{
  uint32_t value;
  uint32_t count;
  uint32_t retry = DCI_RETRY_COUNT;

  // Get the total length of the command word (total in bytes/4)
  count = *command >> 2;

  // Write the command words to DCI register
  while (count--) {
    do {
      // Poll status to wait WPENDING is low
      write_ap(1, DCI_STATUS);
      read_ap(3, &value);
      read_dp(3, &value);
      if ((value & RDATAVALID) != 0) {
        RAISE(DCI_ERROR_WRITE_COMMAND);
      }
      retry--;
    } while (((value & WPENDING) != 0) && (retry > 0));

    if (retry == 0) {
      RAISE(DCI_ERROR_WRITE_TIMEOUT);
    }

    // Write 32-bit command word
    write_ap(1, DCI_WDATA);
    write_ap(3, *command++);
  }
}

/***************************************************************************//**
 * Read DCI register to retrieve responses from the SE.
 ******************************************************************************/
void read_dci_response(uint32_t *resp)
{
  uint32_t count;
  uint32_t retry = DCI_RETRY_COUNT;

  // Poll status to wait RDATAVALID to high
  do {
    write_ap(1, DCI_STATUS);
    read_ap(3, resp);
    read_dp(3, resp);
    retry--;
  } while (((*resp & RDATAVALID) != RDATAVALID) && (retry > 0));

  if (retry == 0) {
    RAISE(DCI_ERROR_READ_TIMEOUT);
  }

  // Read the first response word from DCI register
  write_ap(1, DCI_RDATA);
  read_ap(3, resp);
  read_dp(3, resp);

  // Check response code, raise error if not 0
  if ((*resp >> 16) != 0) {
    RAISE((*resp >> 16) + DCI_RESPONSE_OK);
  }

  // Get the total length of the response word (total in bytes/4)
  count = (*resp & 0x00ff) >> 2;

  // Read the sequential response words
  while (--count != 0) {
    // Poll status to wait RDATAVALID to high
    resp++;
    do {
      write_ap(1, DCI_STATUS);
      read_ap(3, resp);
      read_dp(3, resp);
      retry--;
    } while (((*resp & RDATAVALID) != RDATAVALID) && (retry > 0));

    if (retry == 0) {
      RAISE(DCI_ERROR_READ_TIMEOUT);
    }

    // Read 32-bit response word
    write_ap(1, DCI_RDATA);
    read_ap(3, resp);
    read_dp(3, resp);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Reads from an AP or DP register.
 ******************************************************************************/
static uint32_t read_reg(bool ap, int reg, uint32_t *data)
{
  int i;
  uint32_t b;
  uint32_t cb = 0;
  uint32_t ack = 0;
  uint32_t parity = 0;
  uint32_t ret = SWD_ERROR_OK;

  // Initialize output variable
  *data = 0;

  // Convert to integer
  int int_ap = (int)ap;
  int int_read = (int)1;

  int addr2 = reg & 0x01;
  int addr3 = (reg >> 1) & 0x01;

  // Calculate parity
  parity = (int_ap + int_read + addr2 + addr3) & 0x01;

  SWDIO_SET_OUTPUT();

  // Send request
  WRITE_BIT(1);
  WRITE_BIT(int_ap);
  WRITE_BIT(int_read);
  WRITE_BIT(addr2);
  WRITE_BIT(addr3);
  WRITE_BIT(parity);
  WRITE_BIT(0);
  WRITE_BIT(1);

  // Turn-around
  SWDIO_SET_INPUT();
  SWCLK_CYCLE();

  // Read ACK
  for (i = 0; i < 3; i++) {
    READ_BIT(b);
    ack |= b << i;
  }

  // Verify that ACK is OK
  if (ack == ACK_OK) {
    for (i = 0; i < 32; i++) {
      // Read bit
      READ_BIT(b);
      *data |= b << i;

      // Keep track of expected parity
      if (b) {
        cb = !cb;
      }
    }
    // Read parity bit
    READ_BIT(parity);
    // Verify parity
    if (cb == parity) {
      ret = SWD_ERROR_OK;
    } else {
      ret = SWD_ERROR_PARITY;
    }
  } else if (ack == ACK_WAIT) {
    ret = SWD_ERROR_WAIT;
  } else if (ack == ACK_FAULT) {
    ret = SWD_ERROR_FAULT;
  } else {
    // Line not driven, protocol error
    ret = SWD_ERROR_PROTOCOL;
  }

  // Turn-around
  SWCLK_CYCLE();

  // The 8-cycle idle period to make sure transaction is clocked through DAP
  SWDIO_SET_OUTPUT();
  for (i = 0; i < 8; i++) {
    WRITE_BIT(0);
  }
  return ret;
}

/***************************************************************************//**
 * Writes to an AP or DP register.
 ******************************************************************************/
static uint32_t write_reg(bool ap, int reg, uint32_t data, bool ignore_ack)
{
  int i;
  uint32_t b;
  uint32_t ack = 0;
  uint32_t parity = 0;
  uint32_t ret = SWD_ERROR_OK;

  // Convert to integer
  int int_ap = (int)ap;
  int int_read = (int)0;

  // Calculate address bits
  int addr2 = reg & 0x01;
  int addr3 = (reg >> 1) & 0x01;

  // Calculate parity
  parity = (int_ap + int_read + addr2 + addr3) & 0x01;

  SWDIO_SET_OUTPUT();

  // Write request
  WRITE_BIT(1);
  WRITE_BIT(int_ap);
  WRITE_BIT(int_read);
  WRITE_BIT(addr2);
  WRITE_BIT(addr3);
  WRITE_BIT(parity);
  WRITE_BIT(0);
  WRITE_BIT(1);

  SWDIO_SET_INPUT();

  // Turn-around
  SWCLK_CYCLE();

  // Read acknowledge
  for (i = 0; i < 3; i++) {
    READ_BIT(b);
    ack |= b << i;
  }

  if (ack == ACK_OK || ignore_ack) {
    // Turn-around
    SWCLK_CYCLE();

    SWDIO_SET_OUTPUT();

    // Write data
    parity = 0;
    for (i = 0; i < 32; i++) {
      b = (data >> i) & 0x01;
      WRITE_BIT(b);
      if (b) {
        parity = !parity;
      }
    }
    // Write parity bit
    WRITE_BIT(parity);
  } else if (ack == ACK_WAIT) {
    ret = SWD_ERROR_WAIT;
  } else if (ack == ACK_FAULT) {
    ret = SWD_ERROR_FAULT;
  } else {
    // Line not driven, protocol error
    ret = SWD_ERROR_PROTOCOL;
  }

  // The 8-cycle idle period to make sure transaction is clocked through DAP
  SWDIO_SET_OUTPUT();
  for (i = 0; i < 8; i++) {
    WRITE_BIT(0);
  }
  return ret;
}
