
#include <phy_rftlv.h>

/*
 * return : 1-valid, other-invalid
 */
int rftlv_valid(uint32_t tlv_addr)
{
    if ((RFTLF_TYPE_MAGIC_FW_HEAD1 == (*((uint64_t *)tlv_addr))) && \
        (RFTLF_TYPE_MAGIC_FW_HEAD2 == (*((uint64_t *)(tlv_addr + RFTLV_SIZE_HEAD1))))) {
        return 1;
    }
    return 0;
}

/*
 * input:
 *        type: type
 *        value_len: value max len
 *        value: point value
 * return : ">0"-success, "<0"-invalid and end, "==0"-invalid and can next
 */
int rftlv_get(uint32_t tlv_addr, uint16_t type, uint32_t value_len, void *value)
{
    uint16_t type_tmp;
    uint16_t length_tmp;
    uint32_t addr_tmp;

    addr_tmp = tlv_addr + RFTLV_SIZE_HEAD;

    if ((RFTLV_TYPE_INVALID == type) || (!value)) {
        rftlv_print("rftlv_get arg error, type = 0x%x, value = %p\r\n", type, value);
        return -1;
    }
    while (1) {
        if ((addr_tmp - tlv_addr) >= (RFTLV_SIZE_PARTION - RFTLV_SIZE_TL)) {
            rftlv_print("rftlv_get overflow !!!\r\n");
            return -1;
        }
        RFTLV_TYPE((void *)&type_tmp, (void *)addr_tmp, RFTLV_SIZE_TYPE);
        RFTLV_LENGTH((void *)&length_tmp, (void *)(addr_tmp + RFTLV_SIZE_TYPE), RFTLV_SIZE_LENGTH);
        if (type_tmp == RFTLV_TYPE_INVALID) {
            return -1;
        } else if (type_tmp != type) {
            addr_tmp += (RFTLV_SIZE_TL + length_tmp);
            continue;
        }
        if (length_tmp > value_len) {
            return 0;
        }

        //XXX potential access out of aera
        RFTLV_VALUE((void *)value, (void *)(addr_tmp + RFTLV_SIZE_TL), length_tmp);
        return 1;
    }
    return 0;
}

#if RFTLV_DEBUG

#define RFTLV_MAXLEN_XTAL_MODE          (2)
#define RFTLV_MAXLEN_XTAL               (5)
#define RFTLV_MAXLEN_PWR_MODE           (2)
#define RFTLV_MAXLEN_PWR_TABLE          (64)
#define RFTLV_MAXLEN_PWR_TABLE_11B      (4)
#define RFTLV_MAXLEN_PWR_TABLE_11G      (8)
#define RFTLV_MAXLEN_PWR_TABLE_11N      (8)
#define RFTLV_MAXLEN_PWR_OFFSET         (14)
#define RFTLV_MAXLEN_CHAN_DIV_TAB       (60)
#define RFTLV_MAXLEN_CHAN_CNT_TAB       (28)
#define RFTLV_MAXLEN_LO_FCAL_DIV        (4)
#define RFTLV_MAXLEN_EN_TCAL            (1)
#define RFTLV_MAXLEN_LINEAR_OR_FOLLOW   (1)
#define RFTLV_MAXLEN_TCHANNELS          (5)
#define RFTLV_MAXLEN_TCHANNEL_OS        (5)
#define RFTLV_MAXLEN_TCHANNEL_OS_LOW    (5)
#define RFTLV_MAXLEN_TROOM_OS           (1)
#define RFTLV_MAXLEN_PWR_TABLE_BLE      (1)

const char g_rftlv_buf[] = {
    0x42, 0x4C, 0x52, 0x46, 0x50, 0x41, 0x52, 0x41, 
    0x6B, 0x31, 0x62, 0x58, 0x6B, 0x44, 0x36, 0x4F,
    0x01, 0x00, 0x02, 0x00, 0x11, 0x11,
    0x02, 0x00, 0x05, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22,
    0x03, 0x00, 0x02, 0x00, 0x33, 0x33,
    0x04, 0x00, 0x40, 0x00,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44,
    0x05, 0x00,   14, 0x00, 0x55, 0x55, 0x55, 0x55,
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
    0x20, 0x00, 0x01, 0x00, 0x66,
    0x21, 0x00, 0x01, 0x00, 0x77,
    0x22, 0x00, 0x05, 0x00, 0x88, 0x88, 0x88, 0x88, 0x88,
    0x23, 0x00, 0x05, 0x00, 0x99, 0x99, 0x99, 0x99, 0x99,
    0x24, 0x00, 0x05, 0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0x25, 0x00, 0x01, 0x00, 0xBB,
    0x30, 0x00, 0x01, 0x00, 0xCC,
};

void rftlv_out(char *str, uint8_t *buf, uint32_t buf_len)
{
    uint32_t i;

    rftlv_print("%s[%ld]: ", str, buf_len);

    for (i = 0; i < buf_len; i++) {
        rftlv_print("%02X ", buf[i]);
    }

    rftlv_print("\r\n");
}

void rftlv_test(void)
{
    uint8_t s_XTAL_MODE[RFTLV_MAXLEN_XTAL_MODE];
    uint8_t s_XTAL[RFTLV_MAXLEN_XTAL];
    uint8_t s_PWR_MODE[RFTLV_MAXLEN_PWR_MODE];
    uint8_t s_PWR_TABLE[RFTLV_MAXLEN_PWR_TABLE];
    uint8_t s_PWR_OFFSET[RFTLV_MAXLEN_PWR_OFFSET];
    uint8_t s_EN_TCAL[RFTLV_MAXLEN_EN_TCAL];
    uint8_t s_LINEAR_OR_FOLLOW[RFTLV_MAXLEN_LINEAR_OR_FOLLOW];
    uint8_t s_TCHANNELS[RFTLV_MAXLEN_TCHANNELS];
    uint8_t s_TCHANNEL_OS[RFTLV_MAXLEN_TCHANNEL_OS];
    uint8_t s_TCHANNEL_OS_LOW[RFTLV_MAXLEN_TCHANNEL_OS_LOW];
    uint8_t s_TROOM_OS[RFTLV_MAXLEN_TROOM_OS];
    uint8_t s_PWR_TABLE_BLE[RFTLV_MAXLEN_PWR_TABLE_BLE];

    if (rftlv_valid() == 0) {
        rftlv_print("rftlv_valid ret = invlid.\r\n");
        return;
    }

    memset(s_XTAL_MODE, 0, sizeof(s_XTAL_MODE));
    memset(s_XTAL, 0, sizeof(s_XTAL));
    memset(s_PWR_MODE, 0, sizeof(s_PWR_MODE));
    memset(s_PWR_TABLE, 0, sizeof(s_PWR_TABLE));
    memset(s_PWR_OFFSET, 0, sizeof(s_PWR_OFFSET));
    memset(s_EN_TCAL, 0, sizeof(s_EN_TCAL));
    memset(s_LINEAR_OR_FOLLOW, 0, sizeof(s_LINEAR_OR_FOLLOW));
    memset(s_TCHANNELS, 0, sizeof(s_TCHANNELS));
    memset(s_TCHANNEL_OS, 0, sizeof(s_TCHANNEL_OS));
    memset(s_TCHANNEL_OS_LOW, 0, sizeof(s_TCHANNEL_OS_LOW));
    memset(s_TROOM_OS, 0, sizeof(s_TROOM_OS));
    memset(s_PWR_TABLE_BLE, 0, sizeof(s_PWR_TABLE_BLE));

    if (rftlv_get(RFTLV_TYPE_XTAL_MODE, RFTLV_MAXLEN_XTAL_MODE, s_XTAL_MODE) > 0) {
        rftlv_out("XTAL_MODE", s_XTAL_MODE, RFTLV_MAXLEN_XTAL_MODE);
    }
    if (rftlv_get(RFTLV_TYPE_XTAL, RFTLV_MAXLEN_XTAL, s_XTAL) > 0) {
        rftlv_out("XTAL", s_XTAL, RFTLV_MAXLEN_XTAL);
    }
    if (rftlv_get(RFTLV_TYPE_PWR_MODE, RFTLV_MAXLEN_PWR_MODE, s_PWR_MODE) > 0) {
        rftlv_out("PWR_MODE", s_PWR_MODE, RFTLV_MAXLEN_PWR_MODE);
    }
    if (rftlv_get(RFTLV_TYPE_PWR_TABLE, RFTLV_MAXLEN_PWR_TABLE, s_PWR_TABLE) > 0) {
        rftlv_out("PWR_TABLE", s_PWR_TABLE, RFTLV_MAXLEN_PWR_TABLE);
    }
    if (rftlv_get(RFTLV_TYPE_PWR_OFFSET, RFTLV_MAXLEN_PWR_OFFSET, s_PWR_OFFSET) > 0) {
        rftlv_out("PWR_OFFSET", s_PWR_OFFSET, RFTLV_MAXLEN_PWR_OFFSET);
    }
    if (rftlv_get(RFTLV_TYPE_EN_TCAL, RFTLV_MAXLEN_EN_TCAL, s_EN_TCAL) > 0) {
        rftlv_out("EN_TCAL", s_EN_TCAL, RFTLV_MAXLEN_EN_TCAL);
    }
    if (rftlv_get(RFTLV_TYPE_LINEAR_OR_FOLLOW, RFTLV_MAXLEN_LINEAR_OR_FOLLOW, s_LINEAR_OR_FOLLOW) > 0) {
        rftlv_out("LINEAR_OR_FOLLOW", s_LINEAR_OR_FOLLOW, RFTLV_MAXLEN_LINEAR_OR_FOLLOW);
    }
    if (rftlv_get(RFTLV_TYPE_TCHANNELS, RFTLV_MAXLEN_TCHANNELS, s_TCHANNELS) > 0) {
        rftlv_out("TCHANNELS", s_TCHANNELS, RFTLV_MAXLEN_TCHANNELS);
    }
    if (rftlv_get(RFTLV_TYPE_TCHANNEL_OS, RFTLV_MAXLEN_TCHANNEL_OS, s_TCHANNEL_OS) > 0) {
        rftlv_out("TCHANNEL_OS", s_TCHANNEL_OS, RFTLV_MAXLEN_TCHANNEL_OS);
    }
    if (rftlv_get(RFTLV_TYPE_TCHANNEL_OS_LOW, RFTLV_MAXLEN_TCHANNEL_OS_LOW, s_TCHANNEL_OS_LOW) > 0) {
        rftlv_out("TCHANNEL_OS_LOW", s_TCHANNEL_OS_LOW, RFTLV_MAXLEN_TCHANNEL_OS_LOW);
    }
    if (rftlv_get(RFTLV_TYPE_TROOM_OS, RFTLV_MAXLEN_TROOM_OS, s_TROOM_OS) > 0) {
        rftlv_out("TROOM_OS", s_TROOM_OS, RFTLV_MAXLEN_TROOM_OS);
    }
    if (rftlv_get(RFTLV_TYPE_PWR_TABLE_BLE, RFTLV_MAXLEN_PWR_TABLE_BLE, s_PWR_TABLE_BLE) > 0) {
        rftlv_out("PWR_TABLE_BLE", s_PWR_TABLE_BLE, RFTLV_MAXLEN_PWR_TABLE_BLE);
    }
}
#endif
