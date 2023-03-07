#ifndef __XMODEM_H__
#define __XMODEM_H__

#include <stdint.h>

/**
 * This is the callback function prototype of xmodem_block_rx(). Implement
 * a function in this type and supply it to xmodem_block_rx(). See description
 * of xmodem_block_rx() for more information.
 *
 * @param buf the pointer to the buffer.
 */
typedef void (*xmodem_block_rx_callback_t)(void *ptr, uint8_t *buffer, int len);


/**
 * This is the block-based, callback-enabled implementation of xmodemReceive.
 *
 * @param ptr This is an arbitrary pointer that user want to be carried to
 *            callback function as first parameter.
 *
 * @retval destsz   if data is received successfully.
 * @retval -1       if cancelled by remote.
 * @retval -2       if sync error.
 * @retval -3       if too many retry error.
 */
int xmodem_block_rx(void                        *ptr,
                    uint8_t                     *dest,
                    int                         destsz,
                    xmodem_block_rx_callback_t  function_pointer);

uint8_t xmodem_send_csi_data(uint32_t    addr,
                             uint32_t    size);

#endif /* #ifndef __XMODEM_H__ */
