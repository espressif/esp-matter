#ifndef __BLPROG_CMD_H__
#define __BLPROG_CMD_H__


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


extern const uint8_t eflash_loader_bin[];


#define BLPROG_CMD_RX_BUF_SIZE          36    // at least 36 bytes
#define BLPROG_CMD_RX_TIMEOUT_MS        1000
#define BLPROG_CMD_HANDSHAKE_RETRY      2
#define BLPROG_CMD_TX_DELAY_MS          1
#define BLPROG_CMD_SHOW_RX_DATA         0


// usage:
// step 1: call blprog_cmd_init
// setp 2: set prog target to uart boot mode
// step 3: simply add delay to ensure prog target is ready for handshake (optional)
#if !defined(NO_EFLASH_LOADER)
// step 4: call blprog_cmd_handshake
// step 5: call blprog_cmd_load_eflash_loader
// step 6: simply add delay to ensure prog target is ready for handshake (optional)
#endif
// step 7: call blprog_cmd_handshake
// step 8: call blprog_cmd_program_flash one or more times
// step 9: set prog target to flash boot mode


int blprog_cmd_init(uint8_t uart_id, uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate);
int blprog_cmd_tx_data(uint8_t *data, uint16_t len);
int blprog_cmd_rx_data(uint8_t **data, uint16_t *len);

int blprog_cmd_handshake(void);
int blprog_cmd_custom(uint8_t cmd, uint8_t *in_data, uint16_t in_len, uint8_t **out_data, uint16_t *out_len);
int blprog_cmd_get_bootinfo(uint8_t **out_data, uint16_t *out_len);
#if !defined(NO_EFLASH_LOADER)
int blprog_cmd_load_bootheader(void);
int blprog_cmd_load_segheader(void);
int blprog_cmd_load_segdata(void);
int blprog_cmd_check_img(void);
int blprog_cmd_run_img(void);
#endif
int blprog_cmd_read_flashid(uint8_t **out_data, uint16_t *out_len);
int blprog_cmd_flash_erase(uint32_t start_addr, uint32_t end_addr);
int blprog_cmd_flash_write(uint32_t addr, uint8_t *img_data, uint32_t img_len);
int blprog_cmd_flash_write_check(void);
int blprog_cmd_flash_xip_readsha(uint32_t addr, uint32_t img_len, uint8_t **out_data, uint16_t *out_len);
int blprog_cmd_xip_read_start(void);
int blprog_cmd_xip_read_finish(void);

#if !defined(NO_EFLASH_LOADER)
// a package of get_bootinfo, load_bootheader, load_segheader, load_segdata, check_img, and run_img
int blprog_cmd_load_eflash_loader(void);
#endif

// a package of read_flashid, flash_erase, flash_write, flash_write_check, xip_read_start, flash_xip_readsha, and xip_read_finish
int blprog_cmd_program_flash(uint32_t addr, uint8_t *img_data, uint32_t img_len);


#endif
