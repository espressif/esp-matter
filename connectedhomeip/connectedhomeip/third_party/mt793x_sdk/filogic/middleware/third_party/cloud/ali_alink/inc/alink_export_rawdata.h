#ifndef _ALINK_EXPORT_RAWDATA_H_
#define _ALINK_EXPORT_RAWDATA_H_

#include "alink_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/***
 * desc:     uart rx/tx func
 * in_rawdata: input raw buffer
 * in_len:     input raw buffer len
 * out_rawdata:  output buffer
 * out_len:      INOUT param: stand for output buffer-size when used as input while output buffer-len when used as output
 * retc:     0--success, -1--error
 */
typedef int (*alink_uart_get_t)(const char *in_rawdata, int in_len, char *out_rawdata, int *out_len);
/***
 * desc:     uart rx/tx func
 * rawdata:  buffer which hold the rawdata
 * len:      buffer len
 * retc:     0--success, -1--error
 */
typedef int (*alink_uart_put_t)(char *rawdata, int len);

/***
 * @desc:    Start ALINK-SDK. Note alink_config should be called first if there's any special settings.
 * @para:    dev: Device info. this func will block until alink server is connected.
 * @retc:    ALINK_OK/ALINK_ERR
 */
    int alink_start_rawdata(struct device_info *dev, alink_uart_get_t get, alink_uart_put_t put);


/***
 * @desc:    Post device raw data. used in uart_recv thread.
 * @para:    rawdata: hex-data gotten from the uart.
 *           len: len of rawdata
 *           Memory referred by rawdata is ready to release right after the function returns.
 * @retc:    ALINK_OK/ALINK_ERR
 */
    int alink_post_device_rawdata(const char *rawdata, int len);

/***
 * example
 *
 * alink_set_config(&config);
 * alink_start_rawdata(dev, vendor_uart_get, vendor_uart_put);
 *
 * void uart_recv_thread(void *arg)
 * {
 * 	while (1) {
 * 		uart_recv(uart_data, uart_data_len);
 * 		...
 *		//uart_data MUST follow alink serial protocol
 * 		alink_post_device_rawdata(uart_data, uart_data_len);
 * 	}
 * }
 */

#ifdef __cplusplus
}
#endif

#endif
