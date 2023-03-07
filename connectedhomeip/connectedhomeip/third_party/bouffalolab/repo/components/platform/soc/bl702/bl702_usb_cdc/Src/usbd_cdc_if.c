#include "usbd_cdc_if.h"

/* Define size for the receive and transmit buffer over CDC */
/* It's up to user to redefine and/or remove those define */
#if 0
#define APP_RX_DATA_SIZE  1000
#define APP_TX_DATA_SIZE  1000
#else
// mzhu optimization
#define APP_RX_DATA_SIZE  64
#define APP_TX_DATA_SIZE  64
#endif


USBD_CDC_LineCodingTypeDef cdc_line_coding = {
    .bitrate = 2000000,
    .format = 0,
    .paritytype = 0,
    .datatype = 8
};


/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];


extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_Transmit_FS(uint8_t* pbuf, uint32_t *Len);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = {
    CDC_Init_FS,
    CDC_DeInit_FS,
    CDC_Control_FS,
    CDC_Receive_FS,
	CDC_Transmit_FS,
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
    /* Set Application Buffers */
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);

    return (USBD_OK);
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
    return (USBD_OK);
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
    switch(cmd){
        case CDC_SEND_ENCAPSULATED_COMMAND:
            USBD_DbgLog("send_encapsulated\r\n");
            break;

        case CDC_GET_ENCAPSULATED_RESPONSE:
            USBD_DbgLog("get_encapsulated\r\n");
            break;

        case CDC_SET_COMM_FEATURE:
            USBD_DbgLog("set_comm_feature\r\n");
            break;

        case CDC_GET_COMM_FEATURE:
            USBD_DbgLog("get_comm_feature\r\n");
            break;

        case CDC_CLEAR_COMM_FEATURE:
            USBD_DbgLog("clear_comm_feature\r\n");
            break;

        /*******************************************************************************/
        /* Line Coding Structure                                                       */
        /*-----------------------------------------------------------------------------*/
        /* Offset | Field       | Size | Value  | Description                          */
        /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
        /* 4      | bCharFormat |   1  | Number | Stop bits                            */
        /*                                        0 - 1 Stop bit                       */
        /*                                        1 - 1.5 Stop bits                    */
        /*                                        2 - 2 Stop bits                      */
        /* 5      | bParityType |  1   | Number | Parity                               */
        /*                                        0 - None                             */
        /*                                        1 - Odd                              */
        /*                                        2 - Even                             */
        /*                                        3 - Mark                             */
        /*                                        4 - Space                            */
        /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
        /*******************************************************************************/
        case CDC_SET_LINE_CODING:
            //USBD_DbgLog("set_line_coding\r\n");
            cdc_line_coding.bitrate = ((USBD_CDC_LineCodingTypeDef *)pbuf)->bitrate;
            cdc_line_coding.format = ((USBD_CDC_LineCodingTypeDef *)pbuf)->format;
            cdc_line_coding.paritytype = ((USBD_CDC_LineCodingTypeDef *)pbuf)->paritytype;
            cdc_line_coding.datatype = ((USBD_CDC_LineCodingTypeDef *)pbuf)->datatype;
            break;

        case CDC_GET_LINE_CODING:
            //USBD_DbgLog("get_line_coding\r\n");
            memcpy(pbuf,(uint8_t *)&cdc_line_coding,7);
            break;

        case CDC_SET_CONTROL_LINE_STATE:
            //USBD_DbgLog("set_control_line_state\r\n");
            break;

        case CDC_SEND_BREAK:
            //USBD_DbgLog("send_break\r\n");
            break;

        default:
            break;
    }

    return (USBD_OK);
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len)
{
    return (USBD_OK);
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
static int8_t CDC_Transmit_FS(uint8_t* pbuf, uint32_t *Len)
{
	return (USBD_OK);
}


