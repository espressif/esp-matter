#ifndef _SL_BTCTRL_LINKLAYER_H_
#define _SL_BTCTRL_LINKLAYER_H_
#include "sl_status.h"
#include <stdint.h>

void sl_bt_controller_init(void);

void sl_bt_controller_deinit(void);

void sl_btctrl_init(void);

/**
 * Allocate memory buffers for controller
 *
 * @param memsize size of memory to allocate
 * @returns number of memory buffers allocated
 */
uint32_t sl_btctrl_init_mem(uint32_t memsize);

/**
 * Release all memory allocated by controller
 */
void sli_btctrl_deinit_mem(void);

void sli_btctrl_set_interrupt_priorities();

sl_status_t sl_btctrl_init_ll(void);

void sli_btctrl_set_address(uint8_t *address);

//Initialize memory objects used by LinkLayer
//In future these should be configured individually
sl_status_t sl_btctrl_init_basic(uint8_t connections, uint8_t adv_sets, uint8_t whitelist);

void sli_btctrl_events_init(void);

enum sl_btctrl_channelmap_flags{
  SL_BTCTRL_CHANNELMAP_FLAG_ACTIVE_ADAPTIVITY = 0x01,
  SL_BTCTRL_CHANNELMAP_FLAG_PASSIVE_ADAPTIVITY= 0x02,
};
/**
 * Initialize and enable adaptive frequency hopping
 */

sl_status_t sl_btctrl_init_afh(uint32_t flags);

/**
 * @brief Initilize periodic advertiser
 */
void sl_btctrl_init_periodic_adv();

/**
 * @brief Initilize periodic advertiser
 */
void sl_btctrl_init_periodic_scan();

/**
 * @brief Enable and initialize support for the PAWR advertiser.
 * @param[in] num_adv Number of advertising sets supporting PAWR.
 *       If set to zero, previously allocated PAWR sets are only freed.
 * @return SL_STATUS_OK, or an appropriate error code.
 */
sl_status_t sl_btctrl_pawr_advertiser_configure(uint8_t max_pawr_sets);

/**
 * @brief Enable and initialize support for PAWR sync/receiver.
 * @return SL_STATUS_OK, or an appropriate error code. */
sl_status_t sl_btctrl_pawr_synchronizer_configure(void);

/**
 * @brief Allocate memory for synchronized scanners
 *
 * @param num_scan Number of Periodic Scanners Allowed
 * @return SL_STATUS_OK if allocation was succesfull, failure reason otherwise
 */
sl_status_t sl_btctrl_alloc_periodic_scan(uint8_t num_scan);

/**
 * @brief Allocate memory for periodic advertisers
 *
 * @param num_adv Number of advertisers to allocate
 */
sl_status_t sl_btctrl_alloc_periodic_adv(uint8_t num_adv);

/**
 * Call to enable the even connection scheduling algorithm.
 * This function should be called before link layer initialization.
 */
void sl_btctrl_enable_even_connsch();

/**
 * Call to initialize multiprotocol
 * in bluetooth controller
 */
void sl_btctrl_init_multiprotocol();

/**
 * Link with symbol to enable radio watchdog
 */
void sl_btctrl_enable_radio_watchdog();

/**
 * Initialize CTE receiver
 */
sl_status_t sl_btctrl_init_cte_receiver();

/**
 * Initialize CTE transmitter
 */
sl_status_t sl_btctrl_init_cte_transmitter();

/**
 * Initialize both CTE receiver and transmitter
 *
 * Note: This is for backward compatibility. It is recommend to
 * use sl_btctrl_init_cte_receiver and sl_btctrl_init_cte_transmitter
 * functions instead.
 */
sl_status_t sl_btctrl_init_cte();

/**
 * Check if event bitmap indicates pending events
 * @return bool pending events
 */
bool sli_pending_btctrl_events(void);

/**
 * Initialize adv component
 */
void sl_btctrl_init_adv(void);

void sl_btctrl_init_conn(void);

void sl_btctrl_init_phy(void);

void sl_btctrl_init_adv_ext(void);

/**
 * @brief Initialize extended scanner state
 *
 */
void sl_btctrl_init_scan_ext(void);

void sl_btctrl_init_scan(void);

/**
 * @brief return true if controller is initialized
 *
 */
bool sl_btctrl_is_initialized();

#endif
