#ifndef _BTIF_MT7933_H_
#define _BTIF_MT7933_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "mt7933.h"
#include "timers.h"

/*
 * MTK_BT_DRV_CHIP_RESET
 * used to control subsys chip reset and whole chip reset
 * if defined, will open the feature; otherwise, close the feature
 */
//#define MTK_BT_DRV_CHIP_RESET

/*
 * MTK_BT_DRV_AUTO_PICUS
 * used to enable auto picus feature
 * if defined, open the feature, otherwise, close the feature
 */
#ifndef MTK_BT_MEM_SHRINK_RLS
#define MTK_BT_DRV_AUTO_PICUS
#endif

/******** chip HAL define*************************************/
#define BTIF_IRQ_ID (75)
#define BTIF_WAKEUP_IRQ_ID (79)
#ifdef BTIF_BASE
#undef BTIF_BASE
#define BTIF_BASE (0x38040000) // should modify the define in mt7933.h
#endif
#define BTIF_CG_CLR_REG 0x10001084
#define BTIF_CG_CLR_BIT (1UL << 31) // 0:enable 1:disable
#define BTIF_CG_SET_REG 0x10001080
#define BTIF_CG_SET_BIT (1UL << 31) // 0:disable(with clock) 1:enable(without clock)
#define BTIF_CG_STA_REG 0x10001090
#define BTIF_CG_STA_BIT (1UL << 31) // 0:disable(with clock) 1:enable(without clock)

#define BTIF_APDMA_TX_IRQ_ID  (71)
#define BTIF_APDMA_RX_IRQ_ID  (72)
#define BTIF_APDMA_TX_BASE    (0x34407480)
#define BTIF_APDMA_RX_BASE    (0x34407500)
#define BTIF_APDMA_CG_SET_REG 0x10001088
#define BTIF_APDMA_CG_SET_BIT (1UL << 18) // 0:disable(with clock) 1:enable(without clock)
#define BTIF_APDMA_CG_CLR_REG 0x1000108C
#define BTIF_APDMA_CG_CLR_BIT (1UL << 18) // 0:enable  1:disable
#define BTIF_APDMA_CG_STA_REG 0x10001094
#define BTIF_APDMA_CG_STA_BIT (1UL << 18) // 0:enable  1:disable

//---------------------------------------------------------------------------
#define PATCH_HCI_HEADER_SIZE 4
#define PATCH_WMT_HEADER_SIZE 5

#define PATCH_HEADER_SIZE (PATCH_HCI_HEADER_SIZE + PATCH_WMT_HEADER_SIZE + 1)

#define UPLOAD_PATCH_UNIT 2048
#define PATCH_INFO_SIZE 30

/* this for 79XX need download patch staus
 * 0:
 * patch download is not complete, BT driver need to download patch
 * 1:
 * patch is downloading by Wifi, BT driver need to retry until status = PATCH_READY
 * 2:
 * patch download is complete, BT driver no need to download patch
 */
#define PATCH_ERR -1
#define PATCH_NEED_DOWNLOAD 0
#define PATCH_IS_DOWNLOAD_BY_OTHER 1
#define PATCH_READY 2

/* 0:
 * using legacy wmt cmd/evt to download fw patch, usb/sdio just support 0 now
 * 1:
 * using DMA to download fw patch
 */
#define PATCH_DOWNLOAD_USING_WMT 0
#define PATCH_DOWNLOAD_USING_DMA 1

#define PATCH_DOWNLOAD_PHASE1_2_DELAY_TIME 10
#define PATCH_DOWNLOAD_PHASE1_2_RETRY      20
#define PATCH_DOWNLOAD_PHASE3_DELAY_TIME   500
#define PATCH_DOWNLOAD_PHASE3_RETRY        2

#define PATCH_PHASE1 1
#define PATCH_PHASE2 2
#define PATCH_PHASE3 3

#define FW_ROM_PATCH_HEADER_SIZE  32
#define FW_ROM_PATCH_GD_SIZE      64
#define FW_ROM_PATCH_SEC_MAP_SIZE 64
#define SEC_MAP_NEED_SEND_SIZE    52

#define SECTION_SPEC_NUM 13

enum dl_fw_phase_t {
	DL_FW_PHASE1 = 1,
	DL_FW_PHASE2 = 2
};

//---------------------------------------------------------------------------
#define BT_FIRMWARE_IN_FLASH_START 0x90348000 // default
#define BT_FIRMWARE_IN_PSRAM_START 0x10057800 // for testing
/*
 * CONSYS_BGF_COREDUMP_FLASH_ADDR is address for flash write and need 4K alignment
 * COREDUMP_FLASH_READ_ADDR is virtual address for flash read, so header byte is
 * 0x18 (ex:0x18EC0000 as XIP_BT_START address)
 */
#define CORE_DUMP_INFO_LEN 4096
#define CONSYS_BGF_COREDUMP_FLASH_ADDR                                         \
		((BT_BASE + BT_LENGTH - CORE_DUMP_INFO_LEN) & 0xFFFFF000)
#define COREDUMP_FLASH_READ_ADDR                                               \
		((XIP_BT_START + BT_LENGTH - CORE_DUMP_INFO_LEN) & 0xFFFFF000)

#define SD_SECTION_TYPE_SUBSYS_GENERAL  0x00000000
#define SD_SECTION_TYPE_SUBSYS_MCU      0x00010000
#define SD_SECTION_TYPE_SUBSYS_FM       0x00020000
#define SD_SECTION_TYPE_SUBSYS_BT       0x00030000
#define SD_SECTION_TYPE_SUBSYS_WIFI     0x00040000
#define SD_SECTION_TYPE_SUBSYS_GPS      0x00050000

#define SD_SECTION_GENERAL_GENERAL          0x00000000
#define SD_SECTION_GENERAL_RELEASE_INFO     0x00000001
#define SD_SECTION_GENERAL_BINARY_INFO      0x00000002
#define SD_SECTION_GENERAL_ENCRYPTION_INFO  0x00000003

#define SD_SECTION_BINARY_TYPE_BT_PATCH_TEXT        0x00000000 // BGF patch text
#define SD_SECTION_BINARY_TYPE_BT_PATCH_DATA        0x00000001 // BGF patch data
#define SD_SECTION_BINARY_TYPE_BT_PATCH_TEXT_DATA   0x00000002 // BGF patch text + data

// BT Binary load into EMI (oneshot)
#define SD_SECTION_BINARY_TYPE_BT_EMI_TEXT          0x00000010
// BT Binary download by driver via HIF (before power on)
#define SD_SECTION_BINARY_TYPE_BT_ILM_DATA          0x00000050
// BT Binary download by driver via HIF (oneshot)
#define SD_SECTION_BINARY_TYPE_BT_ILM_TEXT_EX9_DATA 0x00000080

#define GET_SECTION_TYPE(s) (s & 0x00FF0000)
#define GET_BINARY_TYPE(b)  (b & 0x000000FF)

/******** BGF Coredump *************************************/
/*
 * CONNSYS SYSRAM: 0x0040_0000 ~ 0x0041_FFFF
 *             AP: 0x1884_0000 ~ 0x1885_FFFF
 */
// CONNSYS: 0x0040_0000, AP: 0x1884_0000->0x6084_0000
#define BGF_SYSRAM_BASE_ADDR    0x60840000
#define BGF_COREDUMP_PLAIN_TEXT BGF_SYSRAM_BASE_ADDR
/*
 * CONNSYS DLM: 0x0200_0000 ~ 0x0203_FFFF
 *          AP: 0x188C_0000 ~ 0x188F_FFFF
 */
#define BGF_DLM_RAM_ADDR_CONNSYS_VIEW 0x02000000
#define BGF_DLM_RAM_ADDR_AP_VIEW      0x608C0000

/*
 * CONN_INFRA: 0x7C00_0000 ~ 0x7C3F_FFFF
 *         AP: 0x1800_0000 ~ 0x183F_FFFF
 */
// CONNSYS: 0x7C05_0080, AP: 0x1805_0080 --> 0x6005_0080
#define BGF_COREDUMP_CTRL_INFO_ADDR_ADDR 0x60050080
// CONNSYS: 0x7C05_0084, AP: 0x1805_0084 --> 0x6005_0084
#define BGF_COREDUMP_CTRL_INFO_ADDR_LEN  0x60050084
#define BGF_COREDUMP_CTRL_MEM_REGION_START 0x1C // EXP_CTRL_MEM_REGION_NAME_1

// CONNSYS: 0x7C05_0090, AP: 0x1805_0090 --> 0x6005_0090, (0x0040_0000 now)
#define BGF_COREDUMP_DUMP_BUFF_ADDR_ADDR 0x60050090
// CONNSYS: 0x7C05_0094, AP: 0x1805_0094 --> 0x6005_0094
#define BGF_COREDUMP_DUMP_BUFF_ADDR_LEN  0x60050094
// CONNSYS: 0x7C05_0098, AP: 0x1805_0098 --> 0x6005_0098
#define BGF_COREDUMP_CR_REGION_ADDR_ADDR 0x60050098
// CONNSYS: 0x7C05_009C, AP: 0x1805_009C --> 0x6005_009C
#define BGF_COREDUMP_CR_REGION_ADDR_LEN  0x6005009C

// CONNSYS: 0x8102_3010, AP: 0x1882_3010 --> 0x6082_3010
#define BGF_CONN2BGF_REMAP_0 0x60823010
// CONNSYS: 0x8102_3014, AP: 0x1882_3014 --> 0x6082_3014
#define BGF_CONN2BGF_REMAP_1 0x60823014
// CONNSYS: 0x8102_3018, AP: 0x1882_3018 --> 0x6082_3018
#define BGF_CONN2BGF_REMAP_2 0x60823018
// CONNSYS: 0x8102_301C, AP: 0x1882_301C --> 0x6082_301C
#define BGF_CONN2BGF_REMAP_3 0x6082301C
// CONNSYS: 0x8102_3020, AP: 0x1882_3020 --> 0x6082_3020
#define BGF_CONN2BGF_REMAP_SEG_0 0x60823020
// CONNSYS: 0x8102_3024, AP: 0x1882_3024 --> 0x6082_3024
#define BGF_CONN2BGF_REMAP_SEG_1 0x60823024
// AP: 0x1890_0000 --> 0x6090_0000
#define BGF_CONN2BGF_REMAP_SEG_0_ADDR 0x60900000
// AP: 0x18B0_0000 --> 0x60B0_0000
#define BGF_CONN2BGF_REMAP_SEG_1_ADDR 0x60B00000

//---------------------------------------------------------------------------
/* fw own and driver own address
 * CONNSYS: 0x6006_0030
 * AP:      0x1806_0030
 */
#define BGF_REG_LPCTL 0x60060030 // AP:0x18060030 --> 0x60060030

/* fw own and driver own bits */
#define BGF_HOST_SET_FW_OWN_B  BIT(0)
#define BGF_HOST_SET_DRV_OWN_B BIT(1)
#define BGF_OWNER_STATE_SYNC_B BIT(2)

/* fw and driver own type */
#define FW_OWN      0
#define DRIVER_OWN  1

enum own_type_place_t {
	OWN_TYPE_PLACE_UNKNOWN,
	OWN_TYPE_PLACE_TX,
	OWN_TYPE_PLACE_TX_DONE,
	OWN_TYPE_PLACE_RX,
	OWN_TYPE_PLACE_RX_DONE,
};

#define LPCR_POLLING_RETRY 512

/*
 * for debug driver own fail
 */
#define DRV_OWN_FAIL_LOOP_CNT 15
#define BGF_MCU_CUR_PC            (0x60060000 + 0x022C)
#define BGF_DRV_OWN_INTERRUPT_ST  (0x60001000 + 0x0040)
#define BGF_SYSSTRAP_RD_DBG       (0x60060000 + 0x0230)
#define BGF_MET_CFG_ON_EVENT_DATA (0x60060000 + 0x023c)
#define BTIF_DMA_WRPTR (0x60810000 + 0x0530)
#define BTIF_DMA_RDPTR (0x60810000 + 0x0534)
#define BTIF_DMA_FFCNT (0x60810000 + 0x0538)
#define BTIF_DMA_FFSTA (0x60810000 + 0x053C)

/* bt group name in nvdram */
#define BT_CFG_GRP_NAME     "BT_CFG"
#define BT_PWR_TBL_GRP_NAME "BT_PWR_TBL"

/* bt config item names */
#define BT_CFG_SUPPORT_DONGLE_RESET    "SUPPORT_DONGLE_RESET"
#define BT_CFG_SUPPORT_SINGLE_SKU      "SUPPORT_SINGLE_SKU"
#define BT_CFG_SUPPORT_AUTO_PICUS      "SUPPORT_AUTO_PICUS"
#define BT_CFG_WAIT_FW_DUMP_OVER       "WAIT_FW_DUMP_OVER"
#define BT_CFG_PICUS_LOG_LEVEL         "PICUS_LOG_LEVEL"
#define BT_CFG_PICUS_LOG_VIA           "PICUS_LOG_VIA"
#define BT_CFG_PICUS_LOG_UART_BAUDRATE "PICUS_UART_BAUDRATE"
#define BT_CFG_BASE_PARAM_LEN  1
#define BT_CFG_PARAM_CHAR_SIZE 4

/* table size 4k, which can save about 40 country info */
#define BT_CFG_ITEM_NAME_SIZE 32
#define BT_CFG_ITEM_VAL_SIZE  200
#define BT_CFG_ITEM_TOTAL     10
#define BT_PWR_ITEM_NAME_SIZE 4
#define BT_PWR_PARAM_CNT      6
#define BT_PWR_ITEM_VAL_SIZE  38
#define BT_PWR_ITEM_TOTAL     10

/* tx power cmd len and event len */
#define TX_POWER_CMD_LEN 16
#define TX_POWER_EVT_LEN 7

/*Low Power Command*/
#define LOW_POWER_COMMAND_LEN 11
#define LOW_POWER_EVENT_LEN   7

/* country code length */
#define COUNTRY_CODE_LEN 2

/* BR/EDR and LE power min and max value */
#define EDR_MIN        -32
#define EDR_MAX        17
#define EDR_MIN_LV9    12
#define BLE_MIN        -29
#define BLE_MAX        20
#define EDR_MIN_R1     -64
#define EDR_MAX_R1     34
#define EDR_MIN_LV9_R1 24
#define BLE_MIN_R1     -58
#define BLE_MAX_R1     40
#define EDR_MIN_R2     -128
#define EDR_MAX_R2     68
#define EDR_MIN_LV9_R2 48
#define BLE_MIN_R2     -116
#define BLE_MAX_R2     80

#define ERR_PWR -9999

#ifdef MTK_BT_DRV_CHIP_RESET
/* chip reset type */
enum chip_reset_type_t {
	SUBSYS_CHIP_RESET,
	WHOLE_CHIP_RESET,
};

enum chip_reset_state_t {
	CHIP_RESET_UNKNOWN,
	SUBSYS_CHIP_RESET_START,
	SUBSYS_CHIP_RESET_END,
	WHOLE_CHIP_RESET_START,
	WHOLE_CHIP_RESET_END,
};
#endif /* #ifdef MTK_BT_DRV_CHIP_RESET */

/* sku param enum */
enum {
	RES_1 = 0,
	RES_DOT_5,
	RES_DOT_25
};

enum {
	CHECK_SINGLE_SKU_PWR_MODE = 0,
	CHECK_SINGLE_SKU_EDR_MAX,
	CHECK_SINGLE_SKU_BLE,
	CHECK_SINGLE_SKU_BLE_2M,
	CHECK_SINGLE_SKU_BLE_LR_S2,
	CHECK_SINGLE_SKU_BLE_LR_S8,
	CHECK_SINGLE_SKU_ALL };

enum {
	DISABLE_LV9 = 0,
	ENABLE_LV9
};

enum {
	DIFF_MODE_3DB = 0,
	DIFF_MODE_0DB
};

enum bt_fw_coredump_state_t {
	BT_FW_COREDUMP_UNKNOWN,
	BT_FW_COREDUMP_INIT,
	BT_FW_COREDUMP_CMD_SEND_START,
	BT_FW_COREDUMP_CMD_SEND_END,
	BT_FW_COREDUMP_START,
	BT_FW_COREDUMP_END,
	BT_FW_COREDUMP_RESET_START,
	BT_FW_COREDUMP_RESET_END,
};

//---------------------------------------------------------------------------
#ifdef MTK_BT_DRV_CHIP_RESET
struct chip_rst_info {
	xSemaphoreHandle rst_mtx;
	xTaskHandle rst_hdl;
	enum chip_reset_type_t rst_type;
	enum chip_reset_state_t rst_state;
};
#endif /* #ifdef MTK_BT_DRV_CHIP_RESET */

struct bus_hang_info {
	int hang;
	uint32_t addr;
	int rd_wr;
	int master_id;
	int htrans;
	int hburst;
	uint32_t fake_rdate;
};

struct _Section_Map {
	uint32_t u4SecType;
	uint32_t u4SecOffset;
	uint32_t u4SecSize;
	union {
		uint32_t u4SecSpec[SECTION_SPEC_NUM];
		struct {
			uint32_t u4DLAddr;
			uint32_t u4DLSize;
			uint32_t u4SecKeyIdx;
			uint32_t u4AlignLen;
			uint32_t u4BinaryType;
			uint32_t reserved[8];
		} bin_info_spec;
	};
};

struct _Global_Descr {
	uint32_t u4PatchVer;
	uint32_t u4SubSys;
	uint32_t u4FeatureOpt;
	uint32_t u4SectionNum;
};

struct bt_power_setting {
	int8_t edr_max;
	int8_t lv9;
	int8_t dm; // BT diff mode
	int8_t ir; // BT indicator and power resolution
	int8_t ble_1m;
	int8_t ble_2m;
	int8_t ble_lr_s2;
	int8_t ble_lr_s8;
	char country_code[COUNTRY_CODE_LEN + 1];
};

typedef void (*btmtk_event_cb)(void);
typedef int (*btmtk_fwlog_recv_cb)(unsigned char *buf, unsigned int len);

#define HCI_MAX_EVENT_SIZE 256

#define BTMTK_RING_BUFFER_SIZE (1024 * 16)
struct btmtk_ring_buffer_t {
	xSemaphoreHandle mtx;
	unsigned char buffer[BTMTK_RING_BUFFER_SIZE];
	unsigned int read_p;
	unsigned int write_p;
};

#define HCI_PKT_CMD 0x01
#define HCI_PKT_ACL 0x02
#define HCI_PKT_SCO 0x03
#define HCI_PKT_EVT 0x04
#define HCI_PKT_ISO 0x05

#define HCI_EVT_HDR_LEN (3)
#define HCI_ACL_HDR_LEN (5)
#define HCI_ISO_HDR_LEN (5)

struct btmtk_pkt_paser_t {
	unsigned char hdr[5];   // packet header
	unsigned char *buf;     // malloc, for storing both hdr and payload
	unsigned int valid_len; // including valid hdr and payload length
	unsigned int exp_len;
};

struct bt_cfg {
#ifdef MTK_BT_DRV_CHIP_RESET
	uint8_t support_dongle_reset;
#endif /* #ifdef MTK_BT_DRV_CHIP_RESET */
	uint8_t support_single_sku;
	uint8_t wait_fw_dump_over;
#ifdef MTK_BT_DRV_AUTO_PICUS
	uint8_t support_auto_picus;
	uint8_t picus_log_level;     // 0:OFF 1:LOW_POWER 2:SQC 3:FULL
	uint8_t picus_log_via;       // 0:EVENT 1:EMI 2:ACL 3:UART
	uint8_t picus_uart_baudrate; // 1:115200 2:921600 3:3000000
#endif /* #ifdef MTK_BT_DRV_AUTO_PICUS */
};

struct bt_driver_info {
	struct bt_cfg cfg;
	struct bt_power_setting pw_sets;
	enum bt_fw_coredump_state_t fw_coredump_st;
};

struct coredump_region {
	unsigned char name[5];
	uint32_t base;
	uint32_t length;
};

//---------------------------------------------------------------------------
extern int btmtk_check_conninfra_ready(void);

int btmtk_send_fw_rom_patch(enum dl_fw_phase_t phase);
int btmtk_load_ilm_data(uint32_t start_addr);
int btmtk_start_calibration(void);
int btmtk_bgfsys_power_on(void);
int btmtk_bgfsys_power_off(void);
int btmtk_func_ctrl(int b_on);
int btmtk_init(void);
void btmtk_deinit(void);
int btmtk_open(void);
int btmtk_close(void);
void btmtk_register_event_cb(void (*func)(void));
int btmtk_read(unsigned char *buffer, unsigned int length);
int btmtk_write(const unsigned char *buffer, const unsigned int length);
void btmtk_loopback_ctrl(unsigned char enable);
void btmtk_dump_reg(void);
void btmtk_buffer_debug(void);
void btmtk_dump_bgfsys_hang_reg(void);
void btmtk_dump_sleep_fail_reg(void);
void btmtk_set_dbg_level(unsigned char level, int dump_buffer, int times);
void btmtk_flush_rx_queue(void);
bool btmtk_is_coredump_now(void);
int btmtk_trigger_fw_assert(void);
void btmtk_set_coredump_state(enum bt_fw_coredump_state_t state);
enum bt_fw_coredump_state_t btmtk_get_coredump_state(void);
void btmtk_register_fwlog_recv_cb(btmtk_fwlog_recv_cb cb);
void btmtk_enable_bperf(bool enable);
hal_uart_status_t bt_uart_init(int port, int speed);
hal_uart_status_t bt_uart_deinit(int fd);
uint32_t bt_uart_read(int fd, unsigned char *buf, int len);
uint32_t bt_uart_write(int fd, unsigned char *buf, int len);
void btmtk_set_ice_fwdl(char *param);
void btmtk_set_own_ctrl(uint8_t ctrl);
uint8_t btmtk_get_own_ctrl(void);
uint8_t btmtk_get_local_own_type(void);
void btmtk_set_local_own_type(unsigned char own_type);
uint8_t btmtk_get_own_type(void);
int btmtk_set_own_type(unsigned char own_type);
#ifdef MTK_BT_DRV_CHIP_RESET
void btmtk_chip_reset_task(void *p_data);
int btmtk_whole_chip_reset(void);
int btmtk_subsys_chip_reset(void);
void btmtk_trigger_chip_reset(void);
void btmtk_reset_notify(unsigned char from_isr);
int btmtk_get_chip_reset_state(void);
#endif /* #ifdef MTK_BT_DRV_CHIP_RESET */
void btmtk_fw_assert_cb_register(btmtk_event_cb assert_nty);
void btmtk_write_bt_cfg(char *key, char *val[], uint8_t len);
int btmtk_read_bt_cfg(void);
void btmtk_delete_bt_cfg(char *key);
void btmtk_set_bt_power_by_country_code(char *code);
void btmtk_write_country_bt_power(char *cty, char *pwr[]);
int btmtk_read_country_bt_power(void);
void btmtk_delete_country_bt_power(char *cty);
int btmtk_is_country_code_set(void);
int btmtk_send_low_power_cmd(void);
void btmtk_suspend(void);
void btmtk_resume(void);
int btmtk_get_bgfsys_power_state(void);
void btmtk_dump_driver_own_fail_regs(void);
#ifdef MTK_BT_DRV_AUTO_PICUS
void btmtk_enable_picus_log(void);
#endif /* #ifdef MTK_BT_DRV_AUTO_PICUS */
uint8_t *btmtk_get_fw_version(void);
uint32_t btmtk_get_fw_size(void);
void btmtk_read_coredump_info(void);
bool btmtk_check_HW_TRX_empty(void);

#endif
