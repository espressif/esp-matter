#ifndef _MSC_INTERNAL_H_
#define _MSC_INTERNAL_H_

#include "wifi_api.h"
#include "msc_api.h"

#define msc_debug_log 1

/**@brief Smart connection finish flag.
 */
#define SMTCN_FLAG_FIN  (0xAA)

/**@brief Smart connection fail flag.
 */
#define SMTCN_FLAG_FAIL  (0xAE)

/**@brief Smart connection stop flag.
 */
#define SMTCN_FLAG_EXIT  (0xAF)

/**@brief Scan finished flag, which means successfully scan the target AP
 * information.
 */
#define SCAN_FLAG_FIN (0xBB)

/**@brief Scan finish flag, which means not found the target AP information.
 */
#define SCAN_FLAG_NOT_FOUND (0xCC)

/**@brief for init purpose
 */
#define SCAN_FLAG_NONE   (0x00)

#define MAX_SCAN_LOOP_COUNT (5)

#define SWITCH_CHANNEL_TIMES   (200) 

/**@brief For FreeRTOS compatibility.
 */
#define tmr_nodelay         ( TickType_t ) 0

typedef enum SUB_PROTO_SM {
    SUB_INIT,
    SUB_SYNC_SUCC,
    SUB_CH_LOCKED = SUB_SYNC_SUCC,
    SUB_FIN,
}sub_proto_sm_t;

typedef enum MSC_SM {
    MSC_GET_AP_CH,
    MSC_PROB_PROTO,
    MSC_RCV_INFO,
    MSC_SUCC,
    MSC_TIMEOUT,
} msc_sm_t;

typedef sub_proto_sm_t (*sub_proto_input_func)(char *, int);

typedef struct {
    int (*sub_proto_init)(const unsigned char *key, const unsigned char key_length);
    void (*sub_proto_cleanup)(void);
    int (*sub_proto_rst_channel)(void);
    sub_proto_input_func sub_proto_rcv;
    void (*sub_proto_rx_timeout)(void);
} msc_sub_proto_ops;

typedef struct _msc_proto_adapter{
    struct _msc_proto_adapter *next;   
    msc_sub_proto_ops *proto_ops;
    int32_t proto_id;
} msc_proto_adapter_t;

typedef struct {
    int (*init)(const unsigned char *key, const unsigned char key_length);     /**< protocol specific initialize */
    void (*cleanup)(void); /**< protocol specific cleanup */
    int (*switch_channel_rst)(void);  /**< protocol specific reset operation when switch to next channel */
    int (*rx_handler)(char *, int);   /**< protocol specific packet handler */
} multi_smtcn_proto_ops;

#define BSMTCN_MAX_CUSTOM_LEN (640)

typedef struct {
    unsigned char                   pwd[WIFI_LENGTH_PASSPHRASE]; /**< store the password you got */
    unsigned char                   ssid[WIFI_MAX_LENGTH_OF_SSID]; /**< store the ssid you got */
    unsigned char                   pmk[WIFI_LENGTH_PMK]; /**< store the PMK if you have any */
    uint8_t                         tlv_data[BSMTCN_MAX_CUSTOM_LEN];

    int                             tlv_data_len;
    wifi_auth_mode_t                auth_mode; /**< deprecated */
    unsigned char                   ssid_len; /**< ssid length */
    unsigned char                   pwd_len;  /**< password length */

    wifi_encrypt_type_t             encrypt_type; /**< Not used */
    unsigned char                   channel;      /**< Not used */
    unsigned char                   smtcn_flag;   /**< Flag to spicify whether smart connection finished. */
    unsigned char                   scan_flag;    /**< Flag to spicify whether scan finished. */
} multi_smtcn_info_t;


int msc_ctl_register_multi_proto(smnt_type_e ctl_flg);

int32_t msc_save_info(void);
int32_t msc_scan_connect_ap(unsigned char *ssid, unsigned char *passwd);
void msc_continue_switch_channel(void);
void msc_write_flag(uint8_t flag_value);
void msc_stop_switch_channel(void);
uint8_t msc_get_current_channel(void);

void msc_aes_decrypt(uint8_t *cipher_blk, uint32_t cipher_blk_size,
                              uint8_t *key, uint8_t key_len,
                              uint8_t *plain_blk, uint32_t *plain_blk_size);


#endif
