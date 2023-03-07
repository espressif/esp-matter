/*******************************************************************************
* @file  rsi_apis_rom.h
* @brief 
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

#ifndef __SAPIS_ROM_H__
#define __SAPIS_ROM_H__

#include "rsi_driver.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_m4.h"
#endif

#define IS_LITTLE_ENDIAN 0
#define IS_BIG_ENDIAN    1
struct rsi_driver_cb_s;
struct rsi_socket_info_s;
struct rsi_m4ta_desc_s;

#define SAPIS_M4_DEBUG_OUT             0x0000
#define SAPIS_M4_RX_BUFF_ADDR_NULL     0x0001
#define SAPIS_M4_RX_BUFF_ALREDY_VALID  0x0002
#define SAPIS_M4_RX_BUFF_NULL_RECIEVED 0x0004
#define SAPIS_M4_ISR_UNEXPECTED_INTR   0x0005
#define SAPIS_M4_TX_INVALID_DESC       0x0006

#define RSI_WLAN_SET_STATUS(x) global_cb_p->rsi_driver_cb->wlan_cb->status = x
#define RSI_CHECK_WLAN_STATE() rsi_driver_cb->wlan_cb->state
#define RSI_WLAN_GET_STATUS()  rsi_driver_cb->wlan_cb->status
#define TCP_LISTEN_MIN_BACKLOG global_cb_p->rsi_tcp_listen_min_backlog
#define NUMBER_OF_SOCKETS      global_cb_p->no_of_sockets

#ifdef APIS_ROM
#define RSI_MUTEX_CREATE            global_cb_p->rom_apis_p->rsi_mutex_create
#define RSI_MUTEX_LOCK              global_cb_p->rom_apis_p->rsi_mutex_lock
#define RSI_MUTEX_UNLOCK            global_cb_p->rom_apis_p->rsi_mutex_unlock
#define RSI_SEMAPHORE_CREATE        global_cb_p->rom_apis_p->rsi_semaphore_create
#define RSI_SEMAPHORE_WAIT          global_cb_p->rom_apis_p->rsi_semaphore_wait
#define RSI_SEMAPHORE_POST          global_cb_p->rom_apis_p->rsi_semaphore_post
#define RSI_CRITICAL_SECTION_ENTRY  global_cb_p->rom_apis_p->rsi_critical_section_entry
#define RSI_CRITICAL_SECTION_EXIT   global_cb_p->rom_apis_p->rsi_critical_section_exit
#define RSI_DRIVER_WLAN_SEND_CMD    global_cb_p->rom_apis_p->rsi_driver_wlan_send_cmd
#define RSI_DRIVER_SEND_DATA        global_cb_p->rom_apis_p->rsi_driver_send_data
#define RSI_DELAY_MS                global_cb_p->rom_apis_p->rsi_delay_ms
#define RSI_ASSERTION               global_cb_p->rom_apis_p->rsi_assertion
#define RSI_SEMAPHORE_POST_FROM_ISR global_cb_p->rom_apis_p->rsi_semaphore_post_from_isr
#define RSI_MASK_TA_INTERRUPT       global_cb_p->rom_apis_p->rsi_mask_ta_interrupt
#define RSI_UNMASK_TA_INTERRUPT     global_cb_p->rom_apis_p->rsi_unmask_ta_interrupt

#else
#define RSI_MUTEX_CREATE            rsi_mutex_create
#define RSI_MUTEX_LOCK              rsi_mutex_lock
#define RSI_MUTEX_UNLOCK            rsi_mutex_unlock
#define RSI_SEMAPHORE_CREATE        rsi_semaphore_create
#define RSI_SEMAPHORE_WAIT          rsi_semaphore_wait
#define RSI_SEMAPHORE_POST          rsi_semaphore_post
#define RSI_CRITICAL_SECTION_ENTRY  rsi_critical_section_entry
#define RSI_CRITICAL_SECTION_EXIT   rsi_critical_section_exit
#define RSI_DRIVER_WLAN_SEND_CMD    rsi_driver_wlan_send_cmd
#define RSI_DRIVER_SEND_DATA        rsi_driver_send_data
#define RSI_DELAY_MS                rsi_delay_ms
#define RSI_ASSERTION               rsi_assertion
#define RSI_SEMAPHORE_POST_FROM_ISR rsi_semaphore_post_from_isr
#define RSI_MASK_TA_INTERRUPT       rsi_mask_ta_interrupt
#define RSI_UNMASK_TA_INTERRUPT     rsi_unmask_ta_interrupt
#endif

struct global_cb_s;

typedef struct rom_apis_s {
  // OS porting APIs
  rsi_error_t (*rsi_mutex_create)(rsi_mutex_handle_t *mutex);

  rsi_error_t (*rsi_mutex_lock)(volatile rsi_mutex_handle_t *mutex);

  rsi_error_t (*rsi_mutex_unlock)(volatile rsi_mutex_handle_t *mutex);

  rsi_error_t (*rsi_semaphore_create)(rsi_semaphore_handle_t *semaphore, uint32_t count);

  rsi_error_t (*rsi_semaphore_wait)(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms);

  rsi_error_t (*rsi_semaphore_post)(rsi_semaphore_handle_t *semaphore);

  rsi_reg_flags_t (*rsi_critical_section_entry)(void);

  void (*rsi_critical_section_exit)(rsi_reg_flags_t xflags);

  int32_t (*rsi_driver_wlan_send_cmd)(rsi_wlan_cmd_request_t cmd, rsi_pkt_t *pkt);

  int32_t (*rsi_driver_send_data)(uint32_t sockID, uint8_t *buffer, uint32_t length, struct rsi_sockaddr *destAddr);

  void (*rsi_delay_ms)(uint32_t delay_ms);

  void (*rsi_assertion)(uint16_t assertion_val, const char *);

  int32_t (*ROM_WL_rsi_socket_create)(struct global_cb_s *global_cb_p, int32_t sockID, int32_t type, int32_t backlog);

  uint16_t (*ROM_WL_calculate_length_to_send)(struct global_cb_s *global_cb_p, uint8_t type, uint8_t buffers);

  rsi_error_t (*rsi_semaphore_post_from_isr)(rsi_semaphore_handle_t *semaphore);

  void (*rsi_mask_ta_interrupt)(void);

  void (*rsi_unmask_ta_interrupt)(void);

} rom_apis_t;

typedef struct global_cb_s {
  struct rsi_driver_cb_s *rsi_driver_cb;

  // Socket information pool pointer
  struct rsi_socket_info_s *rsi_socket_pool;

  rom_apis_t *rom_apis_p;

  uint8_t no_of_sockets;

  uint8_t endian;

  uint8_t rx_buffer_mem_copy;

  uint8_t rsi_tcp_listen_min_backlog;

  uint8_t os_enabled;

  uint8_t rsi_max_num_events;

  struct rsi_m4ta_desc_s *tx_desc;

  struct rsi_m4ta_desc_s *rx_desc;

  volatile uint8_t submit_rx_pkt_to_ta;

} global_cb_t;

extern global_cb_t *global_cb_p;
extern rom_apis_t *rom_apis_p;
extern rsi_socket_info_t *rsi_socket_pool;
#ifdef RSI_M4_INTERFACE
extern rsi_m4ta_desc_t tx_desc[2];
extern rsi_m4ta_desc_t rx_desc[2];
#endif

#if defined(A11_ROM)
const typedef struct ROM_WL_API_S {

#ifdef RSI_WLAN_ENABLE
  // Socket APIs
  int32_t (*rsi_get_application_socket_descriptor)(global_cb_t *global_cb_p, int32_t sock_id);
  uint8_t (*calculate_buffers_required)(global_cb_t *global_cb_p, uint8_t type, uint16_t length);
  uint16_t (*calculate_length_to_send)(struct global_cb_s *global_cb_p, uint8_t type, uint8_t buffers);
#endif

  // Scheduler
  void (*rsi_scheduler_init)(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb);
  uint32_t (*rsi_get_event)(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb);
  void (*rsi_scheduler)(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb);

  // Packet allocation management
  int32_t (*rsi_pkt_pool_init)(global_cb_t *global_cb_p,
                               rsi_pkt_pool_t *pool_cb,
                               uint8_t *buffer,
                               uint32_t total_size,
                               uint32_t pkt_size);
  rsi_pkt_t *(*rsi_pkt_alloc)(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb);
  int32_t (*rsi_pkt_free)(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt);
  uint32_t (*rsi_is_pkt_available)(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb);

  // Queue Management
  void (*rsi_queues_init)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
  void (*rsi_enqueue_pkt)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue, rsi_pkt_t *pkt);
  rsi_pkt_t *(*rsi_dequeue_pkt)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
  uint32_t (*rsi_check_queue_status)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
  void (*rsi_block_queue)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
  void (*rsi_unblock_queue)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
  void (*rsi_enqueue_pkt_from_isr)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue, rsi_pkt_t *pkt);

  // Events
  void (*rsi_set_event)(global_cb_t *global_cb_p, uint32_t event_num);
  void (*rsi_clear_event)(global_cb_t *global_cb_p, uint32_t event_num);
  void (*rsi_mask_event)(global_cb_t *global_cb_p, uint32_t event_num);
  void (*rsi_unmask_event)(global_cb_t *global_cb_p, uint32_t event_num);
  uint32_t (*rsi_find_event)(global_cb_t *global_cb_p, uint32_t event_map);
  uint16_t (*rsi_register_event)(global_cb_t *global_cb_p, uint32_t event_id, void (*event_handler_ptr)(void));
  void (*rsi_set_event_from_isr)(global_cb_t *global_cb_p, uint32_t event_num);
  void (*rsi_unmask_event_from_isr)(global_cb_t *global_cb_p, uint32_t event_num);

  // Utils
  void (*rsi_uint16_to_2bytes)(global_cb_t *global_cb_p, uint8_t *dBuf, uint16_t val);
  void (*rsi_uint32_to_4bytes)(global_cb_t *global_cb_p, uint8_t *dBuf, uint32_t val);
  uint16_t (*rsi_bytes2R_to_uint16)(global_cb_t *global_cb_p, uint8_t *dBuf);
  uint32_t (*rsi_bytes4R_to_uint32)(global_cb_t *global_cb_p, uint8_t *dBuf);
#ifdef RSI_M4_INTERFACE
  // M4 HAL
  void (*rsi_m4_interrupt_isr)(global_cb_t *global_cb_p);
  void (*mask_ta_interrupt)(uint32_t interrupt_no);
  void (*unmask_ta_interrupt)(uint32_t interrupt_no);
  rsi_pkt_t *(*rsi_frame_read)(global_cb_t *global_cb_p);
  int16_t (*rsi_frame_write)(global_cb_t *global_cb_p,
                             rsi_frame_desc_t *uFrameDscFrame,
                             uint8_t *payloadparam,
                             uint16_t size_param);
  int (*rsi_submit_rx_pkt)(global_cb_t *global_cb_p);
#endif

} ROM_WL_API_T;

#ifndef ROMAPI_WL
#define ROMAPI_WL ((ROM_WL_API_T *)(*(uint32_t *)(0x300100 + (18 * 4))))
#endif

#else
typedef const struct ROM_WL_API_S {

  // Socket APIs
  int32_t (*socket_async)(global_cb_t *global_cb_p,
                          int32_t protocolFamily,
                          int32_t type,
                          int32_t protocol,
                          void (*callback)(uint32_t sock_no, uint8_t *buffer, uint32_t length));
  int32_t (*socket)(global_cb_t *global_cb_p, int32_t protocolFamily, int32_t type, int32_t protocol);
  int32_t (*bind)(global_cb_t *global_cb_p, int32_t sockID, struct rsi_sockaddr *localAddress, int32_t addressLength);
  int32_t (*connect)(global_cb_t *global_cb_p,
                     int32_t sockID,
                     struct rsi_sockaddr *remoteAddress,
                     int32_t addressLength);
  int32_t (*listen)(global_cb_t *global_cb_p, int32_t sockID, int32_t backlog);
  int32_t (*accept)(global_cb_t *global_cb_p,
                    int32_t sockID,
                    struct rsi_sockaddr *ClientAddress,
                    int32_t *addressLength);
  int32_t (*recvfrom)(global_cb_t *global_cb_p,
                      int32_t sockID,
                      int8_t *buffer,
                      int32_t buffersize,
                      int32_t flags,
                      struct rsi_sockaddr *fromAddr,
                      int32_t *fromAddrLen);
  int32_t (*recv)(global_cb_t *global_cb_p, int32_t sockID, void *rcvBuffer, int32_t bufferLength, int32_t flags);
  int32_t (*sendto)(global_cb_t *global_cb_p,
                    int32_t sockID,
                    int8_t *msg,
                    int32_t msgLength,
                    int32_t flags,
                    struct rsi_sockaddr *destAddr,
                    int32_t destAddrLen);
  int32_t (*sendto_async)(global_cb_t *global_cb_p,
                          int32_t sockID,
                          int8_t *msg,
                          int32_t msgLength,
                          int32_t flags,
                          struct rsi_sockaddr *destAddr,
                          int32_t destAddrLen,
                          void (*data_transfer_complete_handler)(uint8_t sockID, uint16_t length));
  int32_t (*send)(global_cb_t *global_cb_p, int32_t sockID, const int8_t *msg, int32_t msgLength, int32_t flags);
  int32_t (*send_async)(global_cb_t *global_cb_p,
                        int32_t sockID,
                        const int8_t *msg,
                        int32_t msgLength,
                        int32_t flags,
                        void (*data_transfer_complete_handler)(uint8_t sockID, uint16_t length));
  int32_t (*shutdown)(global_cb_t *global_cb_p, int32_t sockID, int32_t how);
  int32_t (*rsi_get_application_socket_descriptor)(global_cb_t *global_cb_p, int32_t sock_id);
  void (*rsi_clear_sockets)(global_cb_t *global_cb_p, uint8_t sockID);
  uint8_t (*calculate_buffers_required)(global_cb_t *global_cb_p, uint8_t type, uint16_t length);
  int32_t (*rsi_socket_create)(struct global_cb_s *global_cb_p, int32_t sockID, int32_t type, int32_t backlog);
  uint16_t (*calculate_length_to_send)(struct global_cb_s *global_cb_p, uint8_t type, uint8_t buffers);

  // Scheduler
  void (*rsi_scheduler_init)(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb);
  uint32_t (*rsi_get_event)(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb);
  void (*rsi_scheduler)(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb);

  // Packet allocation management
  int32_t (*rsi_pkt_pool_init)(global_cb_t *global_cb_p,
                               rsi_pkt_pool_t *pool_cb,
                               uint8_t *buffer,
                               uint32_t total_size,
                               uint32_t pkt_size);
  rsi_pkt_t *(*rsi_pkt_alloc)(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb);
  int32_t (*rsi_pkt_free)(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt);
  uint32_t (*rsi_is_pkt_available)(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb);

  // Queue Management
  void (*rsi_queues_init)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
  void (*rsi_enqueue_pkt)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue, rsi_pkt_t *pkt);
  rsi_pkt_t *(*rsi_dequeue_pkt)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
  uint32_t (*rsi_check_queue_status)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
  void (*rsi_block_queue)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
  void (*rsi_unblock_queue)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);

  int32_t (*rsi_driver_send_data)(global_cb_t *global_cb_p,
                                  uint32_t sockID,
                                  uint8_t *buffer,
                                  uint32_t length,
                                  struct rsi_sockaddr *destAddr);
  int32_t (*rsi_driver_process_recv_data)(global_cb_t *global_cb_p, rsi_pkt_t *pkt);

  // Events
  void (*rsi_set_event)(global_cb_t *global_cb_p, uint32_t event_num);
  void (*rsi_clear_event)(global_cb_t *global_cb_p, uint32_t event_num);
  void (*rsi_mask_event)(global_cb_t *global_cb_p, uint32_t event_num);
  void (*rsi_unmask_event)(global_cb_t *global_cb_p, uint32_t event_num);
  uint32_t (*rsi_find_event)(global_cb_t *global_cb_p, uint32_t event_map);
  uint16_t (*rsi_register_event)(global_cb_t *global_cb_p, uint32_t event_id, void (*event_handler_ptr)(void));

  // Utils
  void (*rsi_uint16_to_2bytes)(global_cb_t *global_cb_p, uint8_t *dBuf, uint16_t val);
  void (*rsi_uint32_to_4bytes)(global_cb_t *global_cb_p, uint8_t *dBuf, uint32_t val);
  uint16_t (*rsi_bytes2R_to_uint16)(global_cb_t *global_cb_p, uint8_t *dBuf);
  uint32_t (*rsi_bytes4R_to_uint32)(global_cb_t *global_cb_p, uint8_t *dBuf);
#ifdef RSI_M4_INTERFACE
  // M4 HAL
  void (*rsi_m4_interrupt_isr)(global_cb_t *global_cb_p);
  void (*mask_ta_interrupt)(uint32_t interrupt_no);
  void (*unmask_ta_interrupt)(uint32_t interrupt_no);
  rsi_pkt_t *(*rsi_frame_read)(global_cb_t *global_cb_p);
  int16_t (*rsi_frame_write)(global_cb_t *global_cb_p,
                             rsi_frame_desc_t *uFrameDscFrame,
                             uint8_t *payloadparam,
                             uint16_t size_param);
  int (*rsi_submit_rx_pkt)(global_cb_t *global_cb_p);
#endif

  void (*rsi_enqueue_pkt_from_isr)(global_cb_t *global_cb_p, rsi_queue_cb_t *queue, rsi_pkt_t *pkt);
  void (*rsi_set_event_from_isr)(global_cb_t *global_cb_p, uint32_t event_num);
  void (*rsi_unmask_event_from_isr)(global_cb_t *global_cb_p, uint32_t event_num);

} ROM_WL_API_T;

#define ROMAPI_WL ((ROM_WL_API_T *)(*(uint32_t *)(0x300100 + (20 * 4))))
#endif
extern ROM_WL_API_T *api_wl;
// Proto types
rsi_error_t rsi_mutex_create(rsi_mutex_handle_t *mutex);
rsi_error_t rsi_mutex_lock(volatile rsi_mutex_handle_t *mutex);
void rsi_mutex_lock_from_isr(volatile rsi_mutex_handle_t *mutex);
rsi_error_t rsi_mutex_unlock(volatile rsi_mutex_handle_t *mutex);
void rsi_mutex_unlock_from_isr(volatile rsi_mutex_handle_t *mutex);
rsi_error_t rsi_semaphore_create(rsi_semaphore_handle_t *semaphore, uint32_t count);
rsi_error_t rsi_semaphore_wait(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms);
rsi_error_t rsi_semaphore_post(rsi_semaphore_handle_t *semaphore);
rsi_error_t rsi_semaphore_post_from_isr(rsi_semaphore_handle_t *semaphore);
rsi_reg_flags_t rsi_critical_section_entry(void);
void rsi_critical_section_exit(rsi_reg_flags_t xflags);
void rsi_mask_ta_interrupt(void);
void rsi_unmask_ta_interrupt(void);

rsi_pkt_t *rsi_pkt_alloc(rsi_pkt_pool_t *pool_cb);
int32_t rsi_pkt_free(rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt);
int32_t rsi_driver_wlan_send_cmd(rsi_wlan_cmd_request_t cmd, rsi_pkt_t *pkt);
int32_t rsi_driver_send_data(uint32_t sockID, uint8_t *buffer, uint32_t length, struct rsi_sockaddr *destAddr);
uint32_t rsi_find_event(uint32_t event_map);
void rsi_delay_ms(uint32_t delay_ms);
void rsi_assertion(uint16_t assertion_val, const char *string);

int32_t ROM_WL_rsi_get_application_socket_descriptor(global_cb_t *global_cb_p, int32_t sock_id);
int32_t ROM_WL_rsi_get_primary_socket_id(global_cb_t *global_cb_p, uint16_t port_number);
uint8_t ROM_WL_calculate_buffers_required(global_cb_t *global_cb_p, uint8_t type, uint16_t length);
uint16_t ROM_WL_calculate_length_to_send(global_cb_t *global_cb_p, uint8_t type, uint8_t buffers);

// Scheduler
void ROM_WL_rsi_scheduler_init(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb);
uint32_t ROM_WL_rsi_get_event(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb);
void ROM_WL_rsi_scheduler(global_cb_t *global_cb_p, rsi_scheduler_cb_t *scheduler_cb);

// Packet allocation management
int32_t ROM_WL_rsi_pkt_pool_init(global_cb_t *global_cb_p,
                                 rsi_pkt_pool_t *pool_cb,
                                 uint8_t *buffer,
                                 uint32_t total_size,
                                 uint32_t pkt_size);
rsi_pkt_t *ROM_WL_rsi_pkt_alloc(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb);
int32_t ROM_WL_rsi_pkt_free(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt);
uint32_t ROM_WL_rsi_is_pkt_available(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb);

// Queue Management
void ROM_WL_rsi_queues_init(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
void ROM_WL_rsi_enqueue_pkt(global_cb_t *global_cb_p, rsi_queue_cb_t *queue, rsi_pkt_t *pkt);
rsi_pkt_t *ROM_WL_rsi_dequeue_pkt(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
uint32_t ROM_WL_rsi_check_queue_status(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
void ROM_WL_rsi_block_queue(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
void ROM_WL_rsi_unblock_queue(global_cb_t *global_cb_p, rsi_queue_cb_t *queue);
void ROM_WL_rsi_enqueue_pkt_from_isr(global_cb_t *global_cb_p, rsi_queue_cb_t *queue, rsi_pkt_t *pkt);

int32_t ROM_WL_rsi_driver_send_data(global_cb_t *global_cb_p,
                                    uint32_t sockID,
                                    uint8_t *buffer,
                                    uint32_t length,
                                    struct rsi_sockaddr *destAddr);
int32_t ROM_WL_rsi_driver_process_recv_data(global_cb_t *global_cb_p, rsi_pkt_t *pkt);

// Events
void ROM_WL_rsi_set_event(global_cb_t *global_cb_p, uint32_t event_num);
void ROM_WL_rsi_clear_event(global_cb_t *global_cb_p, uint32_t event_num);
void ROM_WL_rsi_mask_event(global_cb_t *global_cb_p, uint32_t event_num);
void ROM_WL_rsi_unmask_event(global_cb_t *global_cb_p, uint32_t event_num);
uint32_t ROM_WL_rsi_find_event(global_cb_t *global_cb_p, uint32_t event_map);
uint16_t ROM_WL_rsi_register_event(global_cb_t *global_cb_p, uint32_t event_id, void (*event_handler_ptr)(void));
void ROM_WL_rsi_set_event_from_isr(global_cb_t *global_cb_p, uint32_t event_num);
void ROM_WL_rsi_unmask_event_from_isr(global_cb_t *global_cb_p, uint32_t event_num);
// Utils
void ROM_WL_rsi_uint16_to_2bytes(global_cb_t *global_cb_p, uint8_t *dBuf, uint16_t val);
void ROM_WL_rsi_uint32_to_4bytes(global_cb_t *global_cb_p, uint8_t *dBuf, uint32_t val);
uint16_t ROM_WL_rsi_bytes2R_to_uint16(global_cb_t *global_cb_p, uint8_t *dBuf);
uint32_t ROM_WL_rsi_bytes4R_to_uint32(global_cb_t *global_cb_p, uint8_t *dBuf);
void rom_init(void);

#ifdef RSI_M4_INTERFACE
// M4 HAL
void ROM_WL_rsi_m4_interrupt_isr(global_cb_t *global_cb_p);
void ROM_WL_rsi_raise_pkt_pending_interrupt_to_ta(void);
void ROM_WL_mask_ta_interrupt(uint32_t interrupt_no);
void ROM_WL_unmask_ta_interrupt(uint32_t interrupt_no);
rsi_pkt_t *ROM_WL_rsi_frame_read(global_cb_t *global_cb_p);
int16_t ROM_WL_rsi_frame_write(global_cb_t *global_cb_p,
                               rsi_frame_desc_t *uFrameDscFrame,
                               uint8_t *payloadparam,
                               uint16_t size_param);
int ROM_WL_rsi_submit_rx_pkt(global_cb_t *global_cb_p);
void ROM_WL_rsi_receive_from_ta_done_isr(global_cb_t *global_cb_p);
void ROM_WL_rsi_pkt_pending_from_ta_isr(global_cb_t *global_cb_p);
void ROM_WL_rsi_transfer_to_ta_done_isr(global_cb_t *global_cb_p);

#endif
rsi_pkt_t *ROM_WL_rsi_pkt_alloc_non_blocking(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb);
int32_t ROM_WL_rsi_pkt_free_non_blocking(global_cb_t *global_cb_p, rsi_pkt_pool_t *pool_cb, rsi_pkt_t *pkt);
#endif
