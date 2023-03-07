#include "stdio.h"
#include "stdint.h"
#include "stdarg.h"
#include "string.h"
#include "platform_gpio.h"
#include "platform_device.h"
#include "ring_buffer.h"

#ifndef USE_UART_OUTPUT
#define USE_UART_OUTPUT     1
#endif

static uint8_t uart_dbg_disable=0;
#if ((defined BOOTROM) || (defined BL602_EFLASH_LOADER))
#define UART_DBG_ID     UART1_ID
#define UART_DBG_IRQN   UART1_IRQn
#else
#define UART_DBG_ID     UART0_ID
#define UART_DBG_IRQN   UART0_IRQn
#endif

#define BFLB_SYSTICK_INT_RATE   20

#define PLATFORM_INIT_FALSE     (0)
#define PLATFORM_INIT_TRUE      (1)

static const uint8_t hexTable[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
static volatile uint32_t systick_int_cnt=0;
#if !((defined BOOTROM) || (defined BL602_EFLASH_LOADER))
static Ring_Buffer_Type uartRB;
static uint8_t uartBuf[64];
#endif

static uint32_t init_flag = PLATFORM_INIT_FALSE;

/****************************************************************************//**
 * @brief  UART RX fifo ready interrupt call back function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#if !((defined BOOTROM) || (defined BL602_EFLASH_LOADER))
static void UART_RFR_Cbf(void)
{
    uint8_t tmpBuf[16];
    uint32_t len=UART_GetRxFifoCount(UART_DBG_ID);
    Ring_Buffer_Write_Callback(&uartRB,len,(ringBuffer_Write_Callback*)(uint32_t)UART_ReceiveData,
                                (void*)UART_DBG_ID);

    /* Check buf is full */
    len=UART_GetRxFifoCount(UART_DBG_ID);
    if(len!=0&&Ring_Buffer_Get_Empty_Length(&uartRB)==0){
        bflb_platform_printf("UART Recv Full\r\n");
        do{
            UART_ReceiveData(UART_DBG_ID,tmpBuf,sizeof(tmpBuf));
        }while(UART_GetRxFifoCount(UART_DBG_ID));
    }
}
#endif

/****************************************************************************//**
 * @brief  UART RX time-out interrupt call back function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#if !((defined BOOTROM) || (defined BL602_EFLASH_LOADER))
static void UART_RTO_Cbf(void)
{
    uint8_t tmpBuf[16];
    uint32_t len=UART_GetRxFifoCount(UART_DBG_ID);
    Ring_Buffer_Write_Callback(&uartRB,len,(ringBuffer_Write_Callback*)(uint32_t)UART_ReceiveData,
                                (void*)UART_DBG_ID);

    /* Check buf is full */
    len=UART_GetRxFifoCount(UART_DBG_ID);
    if(len!=0&&Ring_Buffer_Get_Empty_Length(&uartRB)==0){
        bflb_platform_printf("UART Recv Full\r\n");
        do{
            UART_ReceiveData(UART_DBG_ID,tmpBuf,sizeof(tmpBuf));
        }while(UART_GetRxFifoCount(UART_DBG_ID));
    }
}
#endif

void bflb_platform_uart_dbg_init(uint32_t bdrate)
{
    UART_CFG_Type uart_dbg_cfg={
                                    32*1000*1000,         /*UART clock*/
                                    2000000,              /* baudrate  */
                                    UART_DATABITS_8,      /* data bits  */
                                    UART_STOPBITS_1,      /* stop bits */
                                    UART_PARITY_NONE,     /* parity  */
                                    DISABLE,              /* Disable auto flow control */
                                    DISABLE,              /* rx input de-glitch function */
                                    DISABLE,              /* Disable RTS output SW control mode */
                                    UART_LSB_FIRST,       /* UART each data byte is send out LSB-first */
    };
    UART_FifoCfg_Type fifoCfg = {
                                    16,                   /* TX FIFO threshold */
                                    16,                   /* RX FIFO threshold */
                                    DISABLE,              /* Disable tx dma req/ack interface */
                                    DISABLE               /* Disable rx dma req/ack interface */
    };

    /* init debug uart gpio first */
    bflb_platform_init_uart_debug_gpio();
#if !((defined BOOTROM) || (defined BL602_EFLASH_LOADER))
    Ring_Buffer_Init(&uartRB,uartBuf,sizeof(uartBuf),NULL,NULL);
#endif

    /* Init UART clock*/
    GLB_Set_UART_CLK(ENABLE ,HBN_UART_CLK_160M,0);
    uart_dbg_cfg.uartClk=160*1000*1000;

    if(bdrate!=0){
        uart_dbg_cfg.baudRate=bdrate;
    }

    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_UART0+UART_DBG_ID);

    /* disable all interrupt */
    UART_IntMask(UART_DBG_ID, UART_INT_ALL, MASK);

    /* disable uart before config */
    UART_Disable(UART_DBG_ID,UART_TXRX);

    /* uart init with default configuration */
    UART_Init(UART_DBG_ID, &uart_dbg_cfg);

    /* UART fifo configuration */
    UART_FifoConfig(UART_DBG_ID,&fifoCfg);


    /* Enable tx free run mode */
    UART_TxFreeRun(UART_DBG_ID,ENABLE);

    /* Set rx time-out value */
    UART_SetRxTimeoutValue(UART_DBG_ID,80);

#if !((defined BOOTROM) || (defined BL602_EFLASH_LOADER))
    /* UART interrupt configuration */
    UART_IntMask(UART_DBG_ID,UART_INT_RX_FIFO_REQ,UNMASK);
    UART_IntMask(UART_DBG_ID,UART_INT_RTO,UNMASK);

    /* Install the interrupt callback function */
    UART_Int_Callback_Install(UART_DBG_ID,UART_INT_RX_FIFO_REQ,&UART_RFR_Cbf);
    UART_Int_Callback_Install(UART_DBG_ID,UART_INT_RTO,&UART_RTO_Cbf);

    /* Enable UART interrupt*/
    NVIC_EnableIRQ(UART_DBG_IRQN);
#endif
    /* enable uart */
    UART_Enable(UART_DBG_ID,UART_TXRX);
}

#ifdef BL602_EFLASH_LOADER
static uint8_t eflash_loader_logbuf[4096] __attribute__ ((section(".system_ram")));
static uint32_t log_len=0;
uint32_t bflb_platform_get_log(uint8_t *data,uint32_t maxlen)
{
    uint32_t len=log_len;
    if(len>maxlen){
        len=maxlen;
    }
    memcpy(data,eflash_loader_logbuf,len);
    return len;
}
#endif

void bflb_platform_usart_dbg_send(uint8_t *data,uint32_t len)
{
#ifdef BL602_EFLASH_LOADER
    if(log_len+len<sizeof(eflash_loader_logbuf)){
        memcpy(eflash_loader_logbuf+log_len,data,len);
        log_len+=len;
    }
#endif
    UART_SendData(UART_DBG_ID,data,len);
}

void bflb_platform_uart_dbg_deinit()
{
    UART_Disable(UART_DBG_ID,UART_TXRX);
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_UART0+UART_DBG_ID);
    bflb_platform_deinit_uart_debug_gpio();
}

void bflb_platform_printf(char *fmt,...)
{
    static char print_buf[128];
    va_list ap;
    if(!uart_dbg_disable){
        va_start(ap, fmt);
        vsnprintf(print_buf, sizeof(print_buf)-1, fmt, ap);
        va_end(ap);
        bflb_platform_usart_dbg_send((uint8_t*)print_buf,strlen(print_buf));
    }
}

void bflb_platform_dump(const void *data,uint32_t len)
{
    uint32_t i=0;
    uint8_t *p=(uint8_t *)data;

    for(i=0;i<len;i++){
        if(i%16==0){
            bflb_platform_printf("\r\n");
        }
        bflb_platform_printf("%02x ",p[i]);
    }
    bflb_platform_printf("\r\n");
}

void bflb_platform_prints(char *data)
{
    bflb_platform_usart_dbg_send((uint8_t*)data,strlen(data));
}

void bflb_platform_printx(uint32_t val)
{
    uint8_t print_buf[8];
    int32_t i;
    for(i=7;i>=0;i--){
        print_buf[i]=hexTable[(val&0xf)];
        val>>=4;
    }
    bflb_platform_usart_dbg_send((uint8_t*)print_buf,8);
}

void bflb_platform_printc(char c)
{
    UART_SendData(UART_DBG_ID,(uint8_t *)&c,1);
}

void bflb_platform_clear_time()
{
    *(volatile uint64_t*) (CLIC_CTRL_ADDR + CLIC_MTIME) = 0;
}

uint64_t  bflb_platform_get_time_ms()
{

    uint32_t tmpValLow,tmpValHigh,tmpValLow1,tmpValHigh1;
    uint32_t cnt=0,tmp;

    do{
        tmpValLow=*(volatile uint32_t*) (CLIC_CTRL_ADDR + CLIC_MTIME);
        tmpValHigh=*(volatile uint32_t*) (CLIC_CTRL_ADDR + CLIC_MTIME+4);
        tmpValLow1=*(volatile uint32_t*) (CLIC_CTRL_ADDR + CLIC_MTIME);
        tmpValHigh1=*(volatile uint32_t*) (CLIC_CTRL_ADDR + CLIC_MTIME+4);
        cnt++;
        if(cnt>4){
            break;
        }
    }while(tmpValLow>tmpValLow1||tmpValHigh>tmpValHigh1);

    tmp=(SystemCoreClockGet()/(GLB_Get_BCLK_Div()+1));
    tmp=(tmp>>3)/1000;

    if(tmpValHigh1==0){
        return(uint64_t)(tmpValLow1/tmp);
    }else{
        return (((uint64_t)tmpValHigh1<<32)+tmpValLow1)/tmp;
    }
}

void bflb_platform_start_time()
{
    *(volatile uint64_t*) (CLIC_CTRL_ADDR + CLIC_MTIME) = 0;
}

void bflb_platform_stop_time()
{

}

void bflb_platform_set_alarm_time(uint64_t time)
{
    uint32_t tmp;

    tmp=(SystemCoreClockGet()/(GLB_Get_BCLK_Div()+1));
    tmp=(tmp>>3)/1000;

    time = time*tmp;

    *(volatile uint64_t*) (CLIC_CTRL_ADDR + CLIC_MTIMECMP) = (*(volatile uint64_t*) (CLIC_CTRL_ADDR + CLIC_MTIME) + time) ;
    NVIC_EnableIRQ(MTIME_IRQn);
}

void bflb_platform_init_time()
{
    NVIC_DisableIRQ(MTIME_IRQn);
    /* Set MTimer the same frequency as SystemCoreClock */

    GLB_Set_MTimer_CLK(1,GLB_MTIMER_CLK_BCLK,7);

    bflb_platform_clear_time();
}

void bflb_platform_deinit_time()
{
    NVIC_DisableIRQ(MTIME_IRQn);
    bflb_platform_stop_time();
}

void bflb_platform_delay_ms(uint32_t time)
{
    uint64_t cnt=0;
    uint32_t clock=SystemCoreClockGet();

    bflb_platform_stop_time();
    bflb_platform_start_time();
    while(bflb_platform_get_time_ms()<time){
        cnt++;
        /* assume bflb_platform_get_time_ms take 32 cycles*/
        if(cnt>(time*(clock>>(10+5)))*2){
            break;
        }
    }
}

void bflb_platform_init(uint32_t baudrate)
{
    if(PLATFORM_INIT_FALSE!=init_flag){
        return ;
    }
    init_flag = PLATFORM_INIT_TRUE;
    
    bflb_platform_init_time();
    
    Sec_Eng_Trng_Enable();
    
    if(!uart_dbg_disable){
        bflb_platform_uart_dbg_init(baudrate);
        bflb_platform_printf("system clock=%dM\r\n",SystemCoreClockGet()/1000000);
    }
}


void bflb_platform_deinit()
{
    if(PLATFORM_INIT_FALSE==init_flag){
        return ;
    }
    init_flag = PLATFORM_INIT_FALSE;
    
    bflb_platform_deinit_time();
    
    Sec_Eng_Trng_Disable();
    
    if(!uart_dbg_disable){
        bflb_platform_uart_dbg_deinit();
    }
    bflb_platform_deinit_time();
}

void bflb_platform_print_set(uint8_t logDisable)
{
    uart_dbg_disable=logDisable;
}

int bflb_platform_get_random(uint8_t *data,uint32_t len)
{
    uint8_t tmpBuf[32];
    uint32_t readLen=0;
    uint32_t i=0,cnt=0;

    while(readLen<len){
        if(Sec_Eng_Trng_Read(tmpBuf)!=SUCCESS){
            return -1;
        }
        cnt=len-readLen;
        if(cnt>sizeof(tmpBuf)){
            cnt=sizeof(tmpBuf);
        }
        for(i=0;i<cnt;i++){
            data[readLen+i]=tmpBuf[i];
        }
        readLen+=cnt;
    }

    return 0;
}

#if !((defined BOOTROM) || (defined BL602_EFLASH_LOADER))
int bflb_platform_get_input(uint8_t *data,uint32_t maxLen)
{
    uint32_t retLen=Ring_Buffer_Get_Length(&uartRB);

    if(retLen>maxLen){
        retLen=maxLen;
    }

    return Ring_Buffer_Read(&uartRB,data,retLen);
}
#endif
