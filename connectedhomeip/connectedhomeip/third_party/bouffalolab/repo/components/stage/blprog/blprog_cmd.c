#include "blprog_cmd.h"
#include "bl_uart.h"
#include "bl_irq.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#if defined(BL602)
#include "bl602_glb.h"
#include "bl602_uart.h"
#include "bflb_hash.h"
#endif

#if defined(BL702)
#include "bl702_glb.h"
#include "bl702_uart.h"
#include "bflb_hash.h"
#endif


static SemaphoreHandle_t blprog_cmd_semphr = NULL;
static uint8_t blprog_cmd_uart_id = 0;
static uint8_t blprog_cmd_inited = 0;

static uint8_t blprog_cmd_rx_buf[BLPROG_CMD_RX_BUF_SIZE] = {0};
static uint16_t blprog_cmd_rx_len = 0;


static void blprog_cmd_rx_enable(void)
{
    UART_RxFifoClear(blprog_cmd_uart_id);
    
    UART_IntClear(blprog_cmd_uart_id, UART_INT_RX_FIFO_REQ);
    UART_IntClear(blprog_cmd_uart_id, UART_INT_RTO);
    
    UART_IntMask(blprog_cmd_uart_id, UART_INT_RX_FIFO_REQ, UNMASK);
    UART_IntMask(blprog_cmd_uart_id, UART_INT_RTO, UNMASK);
}

static void blprog_cmd_rx_disable(void)
{
    UART_IntMask(blprog_cmd_uart_id, UART_INT_RX_FIFO_REQ, MASK);
    UART_IntMask(blprog_cmd_uart_id, UART_INT_RTO, MASK);
}

static void blprog_cmd_uart_irq(void)
{
    uint32_t uartAddr[] = {UART0_BASE, UART1_BASE};
    uint32_t UARTx = uartAddr[blprog_cmd_uart_id];
    uint32_t tmpVal = BL_RD_REG(UARTx, UART_INT_STS);
    
    /* Rx fifo ready interrupt,auto-cleared when data is popped */
    if (BL_IS_REG_BIT_SET(tmpVal, UART_URX_FIFO_INT)) {
        blprog_cmd_rx_len += UART_ReceiveData(blprog_cmd_uart_id, blprog_cmd_rx_buf + blprog_cmd_rx_len, sizeof(blprog_cmd_rx_buf) - blprog_cmd_rx_len);
    }
    
    /* Rx time-out interrupt */
    if (BL_IS_REG_BIT_SET(tmpVal, UART_URX_RTO_INT)) {
        BL_WR_REG(UARTx, UART_INT_CLEAR, 0x10);
        
        blprog_cmd_rx_disable();
        blprog_cmd_rx_len += UART_ReceiveData(blprog_cmd_uart_id, blprog_cmd_rx_buf + blprog_cmd_rx_len, sizeof(blprog_cmd_rx_buf) - blprog_cmd_rx_len);
        
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(blprog_cmd_semphr, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void blprog_cmd_dump_data(uint8_t *data, uint16_t len, char seperator)
{
    int i;
    
    for(i=0; i<len; i++){
        printf("%02X", data[i]);
        if(i != len - 1){
            if(seperator != 0){
                printf("%c", seperator);
            }
        }else{
            printf("\r\n");
        }
    }
}

static void blprog_cmd_delay_ms(uint32_t n)
{
    if(n == 0){
        return;
    }else if(n == 1){
#if defined(BL602)
        BL602_Delay_MS(n);
#else
        arch_delay_ms(n);
#endif
    }else{
        vTaskDelay(n);
    }
}

static void blprog_cmd_calc_sha256(const uint8_t *data, uint32_t len, uint8_t *hash)
{
    bflb_hash_handle_t hash_handle;
    
    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();
    
    bflb_hash_init(&hash_handle, BFLB_HASH_TYPE_SHA256);
    bflb_hash_start(&hash_handle);
    bflb_hash_update(&hash_handle, data, len);
    bflb_hash_finish(&hash_handle, hash);
    bflb_hash_deinit(&hash_handle);
}


int blprog_cmd_init(uint8_t uart_id, uint8_t tx_pin, uint8_t rx_pin, uint32_t baudrate)
{
    if(blprog_cmd_semphr == NULL){
        blprog_cmd_semphr = xSemaphoreCreateCounting(1, 0);
        if(blprog_cmd_semphr == NULL){
            printf("Unable to create semaphore!\r\n");
            return -1;
        }
    }
    
    bl_uart_init(uart_id, tx_pin, rx_pin, 0, 0, baudrate);
    bl_irq_register(UART0_IRQn + uart_id, blprog_cmd_uart_irq);
    bl_irq_enable(UART0_IRQn + uart_id);
    
    blprog_cmd_uart_id = uart_id;
    blprog_cmd_inited = 1;
    
    return 0;
}

int blprog_cmd_tx_data(uint8_t *data, uint16_t len)
{
    configASSERT(data);
    configASSERT(len);
    
    if(blprog_cmd_inited == 0){
        printf("Unable to send data!");
        return -1;
    }
    
    if(UART_SendData(blprog_cmd_uart_id, data, len) != SUCCESS){
        printf("Fail to send data via uart!\r\n");
        return -2;
    }
    
    return 0;
}

int blprog_cmd_rx_data(uint8_t **data, uint16_t *len)
{
    configASSERT(data);
    configASSERT(len);
    
    if(blprog_cmd_inited == 0){
        printf("Unable to receive data!");
        return -1;
    }
    
RX_AGAIN:
    blprog_cmd_rx_len = 0;
    blprog_cmd_rx_enable();
    
    if(xSemaphoreTake(blprog_cmd_semphr, BLPROG_CMD_RX_TIMEOUT_MS) != pdPASS){
        printf("No data received via uart!\r\n");
        return -2;
    }
    
#if BLPROG_CMD_SHOW_RX_DATA
    printf("RX Length: %u, Data: ", blprog_cmd_rx_len);
    blprog_cmd_dump_data(blprog_cmd_rx_buf, blprog_cmd_rx_len, ' ');
#endif
    
    *data = blprog_cmd_rx_buf;
    *len = blprog_cmd_rx_len;
    
    /* Check erase pending */
    if(*len == 2 && (*data)[0] == 0x50 && (*data)[1] == 0x44){
        printf("Erase pending...\r\n");
        goto RX_AGAIN;
    }
    
    /* Check ack */
    if(*len < 2 || (*data)[0] != 0x4F || (*data)[1] != 0x4B){
#if !BLPROG_CMD_SHOW_RX_DATA
        printf("RX Length: %u, Data: ", blprog_cmd_rx_len);
        blprog_cmd_dump_data(blprog_cmd_rx_buf, blprog_cmd_rx_len, ' ');
#endif
        printf("Wrong Ack!\r\n");
        return -3;
    }
    
    /* Remove ack field */
    *data += 2;
    *len -= 2;
    
    /* Check length */
    if(*len != 0){
        if(*len < 2 || *len - 2 != (*data)[0] + ((uint16_t)(*data)[1] << 8)){
#if !BLPROG_CMD_SHOW_RX_DATA
            printf("RX Length: %u, Data: ", blprog_cmd_rx_len);
            blprog_cmd_dump_data(blprog_cmd_rx_buf, blprog_cmd_rx_len, ' ');
#endif
            printf("Wrong Length!\r\n");
            return -4;
        }
        
        /* Remove length field */
        *data += 2;
        *len -= 2;
    }
    
    if(*len == 0){
        printf("OK\r\n");
    }else{
        printf("OK -> ");
        blprog_cmd_dump_data(*data, *len, ' ');
    }
    
    return 0;
}

int blprog_cmd_handshake(void)
{
    uint8_t txbuf[32];
    uint8_t *data;
    uint16_t len;
    int cnt = 0;
    
    memset(txbuf, 0x55, sizeof(txbuf));
    
    while(1){
        printf("# handshake %d\r\n", ++cnt);
        
        blprog_cmd_tx_data(txbuf, sizeof(txbuf));
        
        if(blprog_cmd_rx_data(&data, &len) != 0){
            if(cnt < 1 + BLPROG_CMD_HANDSHAKE_RETRY){
                blprog_cmd_delay_ms(2200);
                continue;
            }else{
                return -1;
            }
        }else{
            return 0;
        }
    }
}

int blprog_cmd_custom(uint8_t cmd, uint8_t *in_data, uint16_t in_len, uint8_t **out_data, uint16_t *out_len)
{
    uint8_t txbuf[4];
    uint8_t *data;
    uint16_t len;
    
    configASSERT(in_data || !in_len);
    
    txbuf[0] = cmd;
    txbuf[1] = 0x00;
    txbuf[2] = in_len & 0xFF;
    txbuf[3] = in_len >> 8;
    
    blprog_cmd_delay_ms(BLPROG_CMD_TX_DELAY_MS);
    
    blprog_cmd_tx_data(txbuf, sizeof(txbuf));
    
    if(in_len > 0){
        blprog_cmd_tx_data(in_data, in_len);
    }
    
    if(blprog_cmd_rx_data(&data, &len) != 0){
        return -1;
    }
    
    if(out_data){
        *out_data = data;
    }
    
    if(out_len){
        *out_len = len;
    }
    
    return 0;
}

int blprog_cmd_get_bootinfo(uint8_t **out_data, uint16_t *out_len)
{
    printf("# get_bootinfo\r\n");
    
    return blprog_cmd_custom(0x10, NULL, 0, out_data, out_len);
}

#if !defined(NO_EFLASH_LOADER)
int blprog_cmd_load_bootheader(void)
{
    printf("# load_bootheader\r\n");
    
    return blprog_cmd_custom(0x11, (uint8_t *)eflash_loader_bin, 176, NULL, NULL);
}

int blprog_cmd_load_segheader(void)
{
    printf("# load_segheader\r\n");
    
    return blprog_cmd_custom(0x17, (uint8_t *)eflash_loader_bin + 176, 16, NULL, NULL);
}

int blprog_cmd_load_segdata(void)
{
    uint8_t *data;
    uint16_t len;
    uint32_t proc_size = 0;
    uint32_t total_size;
    
    total_size = eflash_loader_bin[180];
    total_size += (uint32_t)eflash_loader_bin[181] << 8;
    total_size += (uint32_t)eflash_loader_bin[182] << 16;
    total_size += (uint32_t)eflash_loader_bin[183] << 24;
    
    while(proc_size < total_size){
        data = (uint8_t *)eflash_loader_bin + 192 + proc_size;
        len = total_size - proc_size < 4080 ? total_size - proc_size : 4080;
        proc_size += len;
        
        printf("# load_segdata %lu/%lu\r\n", proc_size, total_size);
        
        if(blprog_cmd_custom(0x18, data, len, NULL, NULL) != 0){
            return -1;
        }
    }
    
    return 0;
}

int blprog_cmd_check_img(void)
{
    printf("# check_img\r\n");
    
    return blprog_cmd_custom(0x19, NULL, 0, NULL, NULL);
}

int blprog_cmd_run_img(void)
{
    printf("# run_img\r\n");
    
    return blprog_cmd_custom(0x1A, NULL, 0, NULL, NULL);
}
#endif

int blprog_cmd_read_flashid(uint8_t **out_data, uint16_t *out_len)
{
    printf("# read_flashid\r\n");
    
    return blprog_cmd_custom(0x36, NULL, 0, out_data, out_len);
}

int blprog_cmd_flash_erase(uint32_t start_addr, uint32_t end_addr)
{
    uint32_t arg[2] = {start_addr, end_addr};
    
    printf("# flash_erase\r\n");
    
    return blprog_cmd_custom(0x30, (uint8_t *)arg, 8, NULL, NULL);
}

int blprog_cmd_flash_write(uint32_t addr, uint8_t *img_data, uint32_t img_len)
{
    uint8_t txbuf[8] = {0x31, 0x00};
    uint8_t *data;
    uint16_t len;
    uint32_t proc_size = 0;
    uint32_t total_size = img_len;
    
    while(proc_size < total_size){
        data = img_data + proc_size;
        len = total_size - proc_size < 2048 ? total_size - proc_size : 2048;
        proc_size += len;
        
        txbuf[2] = (len + 4) & 0xFF;
        txbuf[3] = (len + 4) >> 8;
        txbuf[4] = addr & 0xFF;
        txbuf[5] = (addr >> 8) & 0xFF;
        txbuf[6] = (addr >> 16) & 0xFF;
        txbuf[7] = (addr >> 24) & 0xFF;
        addr += len;
        
        printf("# flash_write %lu/%lu {progress:%lu}\r\n", proc_size, total_size, proc_size * 100 / total_size);
        
        blprog_cmd_delay_ms(BLPROG_CMD_TX_DELAY_MS);
        
        blprog_cmd_tx_data(txbuf, sizeof(txbuf));
        blprog_cmd_tx_data(data, len);
        
        if(blprog_cmd_rx_data(&data, &len) != 0){
            return -1;
        }
    }
    
    return 0;
}

int blprog_cmd_flash_write_check(void)
{
    printf("# flash_write_check\r\n");
    
    return blprog_cmd_custom(0x3A, NULL, 0, NULL, NULL);
}

int blprog_cmd_flash_xip_readsha(uint32_t addr, uint32_t img_len, uint8_t **out_data, uint16_t *out_len)
{
    uint32_t arg[2] = {addr, img_len};
    
    printf("# flash_xip_readsha\r\n");
    
    return blprog_cmd_custom(0x3E, (uint8_t *)arg, 8, out_data, out_len);
}

int blprog_cmd_xip_read_start(void)
{
    printf("# xip_read_start\r\n");
    
    return blprog_cmd_custom(0x60, NULL, 0, NULL, NULL);
}

int blprog_cmd_xip_read_finish(void)
{
    printf("# xip_read_finish\r\n");
    
    return blprog_cmd_custom(0x61, NULL, 0, NULL, NULL);
}

#if !defined(NO_EFLASH_LOADER)
int blprog_cmd_load_eflash_loader(void)
{
    uint8_t *out_data;
    uint16_t out_len;
    
    if(blprog_cmd_get_bootinfo(&out_data, &out_len) != 0){
        return -1;
    }
    
    if(blprog_cmd_load_bootheader() != 0){
        return -1;
    }
    
    if(blprog_cmd_load_segheader() != 0){
        return -1;
    }
    
    if(blprog_cmd_load_segdata() != 0){
        return -1;
    }
    
    if(blprog_cmd_check_img() != 0){
        return -1;
    }
    
    if(blprog_cmd_run_img() != 0){
        return -1;
    }
    
    return 0;
}
#endif

int blprog_cmd_program_flash(uint32_t addr, uint8_t *img_data, uint32_t img_len)
{
    uint8_t hash[32];
    uint8_t *out_data;
    uint16_t out_len;
    int status;
    
    if(blprog_cmd_read_flashid(&out_data, &out_len) != 0){
        return -1;
    }
    
    if(blprog_cmd_flash_erase(addr, addr + img_len - 1) != 0){
        return -1;
    }
    
    if(blprog_cmd_flash_write(addr, img_data, img_len) != 0){
        return -1;
    }
    
    if(blprog_cmd_flash_write_check() != 0){
        return -1;
    }
    
    if(blprog_cmd_xip_read_start() != 0){
        return -1;
    }
    
    if(blprog_cmd_flash_xip_readsha(addr, img_len, &out_data, &out_len) != 0){
        return -1;
    }
    
    printf("SHA calculated by device: ");
    blprog_cmd_dump_data(out_data, out_len, 0);
    
    blprog_cmd_calc_sha256(img_data, img_len, hash);
    
    printf("SHA calculated by host: ");
    blprog_cmd_dump_data(hash, sizeof(hash), 0);
    
    if(out_len == sizeof(hash) && memcmp(out_data, hash, sizeof(hash)) == 0){
        printf("Verify Success!\r\n");
        status = 0;
    }else{
        printf("Verify Fail!\r\n");
        status = -2;
    }
    
    if(blprog_cmd_xip_read_finish() != 0){
        return -1;
    }
    
    return status;
}
