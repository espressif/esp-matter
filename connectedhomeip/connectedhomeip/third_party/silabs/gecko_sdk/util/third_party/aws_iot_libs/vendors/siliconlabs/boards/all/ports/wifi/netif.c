/* Temporary Network Interface driver until wifi driver is implemented.
 *
 * This driver implements an ethernet network interface on top of RTT.
 * A script should be running at the host side to pick up packets
 * from this driver using RTT and forward through to host's network
 * interface.
 *
 * Send: packets are written to RTT output terminal. Host reads
 *       the packets and puts them on host's main network interface.
 *
 * Retrieve: host keeps monitoring its main network interface for
 *           packets. When a packet is received by the host, the host
 *           writes it to RTT input terminal, and this driver picks
 *           them up.
 *
 * That being said, the driver communicates with the network of the host
 * through RTT. It also implements FreeRTOS+ TCP/IP Network Interface API.
 * This allows FreeRTOS to have internet connectivity through this driver,
 * until wifi is implemented.
 */

#include "string.h"

#include "FreeRTOSConfig.h"

#include "SEGGER_RTT.h"

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

#define RTT_ETH_MAGIC_NUMBER   0x55AA55AA

static void prvNetworkInterfaceListen( void * pvParameters );

BaseType_t xNetworkInterfaceInitialise(void)
{
  uint32_t ulMagicSend = RTT_ETH_MAGIC_NUMBER;
  uint32_t ulMagicRecv = 0;
  BaseType_t taskCreated = pdFALSE;
  configPRINT_STRING("Initializing RTT network interface...\n");

  /* initialize rtt */
  SEGGER_RTT_Init();

  /* block on send */
  SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);

  /* write magic number as an ack */
  configPRINT_STRING("Send and wait for a handshake to/from the host...\n");
  SEGGER_RTT_Write(0, (char *) &ulMagicSend, sizeof(ulMagicSend));

  /* wait for RTT ACK to be received */
  while (ulMagicSend != ulMagicRecv) {
    SEGGER_RTT_Read(0, (char *) &ulMagicRecv, sizeof(ulMagicRecv));
  }
  configPRINT_STRING("Handshake received successfully.\n\n");

  /* create thread to receive packets */
  taskCreated = xTaskCreate( prvNetworkInterfaceListen,
                             "NETIF LISTENER",
                             configMINIMAL_STACK_SIZE*2,
                             NULL,
                             tskIDLE_PRIORITY,
                             NULL );

  /* make sure task was created successfully */
  if (taskCreated  != pdPASS) {
    configPRINT_STRING("Couldn't create listener task!\n");
    return pdFAIL;
  }

  /* done */
  return pdPASS;
}

BaseType_t xNetworkInterfaceOutput(NetworkBufferDescriptor_t * const pxNetworkBuffer,
                                   BaseType_t xReleaseAfterSend)
{
  int i;
  uint32_t ulSize = pxNetworkBuffer->xDataLength;
  uint8_t *pucSendBuf = pxNetworkBuffer->pucEthernetBuffer;

  /* print info */
  //UnityPrintf("Sending packet... %d\n", ulSize);

  /* first we send the size of the packet */
  SEGGER_RTT_Write(0, (char *) &ulSize, sizeof(ulSize));

  /* then we send the ethernet packet itself */
  SEGGER_RTT_Write(0, pucSendBuf, ulSize);

  /* deallocate the buffer if it is not used anymore */
  if (xReleaseAfterSend != pdFALSE) {
    vReleaseNetworkBufferAndDescriptor(pxNetworkBuffer);
  }

  /* done */
  return pdPASS;
}

static void prvNetworkInterfaceListen( void * pvParameters )
{
  int i;
  uint32_t ulSize = 0;
  uint32_t ulOff = 0;
  uint32_t ulDrop = 0;
  uint8_t *pucRecvBuf = NULL;
  NetworkBufferDescriptor_t * pxNetworkBuffer;
  IPStackEvent_t xRxEvent = { eNetworkRxEvent, NULL };
  BaseType_t xEventSuccess = pdFAIL;

  /* unused parameters */
  (void) pvParameters;

  /* infinite loop */
  while (1) {
    /* read next packet size */
    for (i = 0; i < sizeof(ulSize); i++) {
      while (SEGGER_RTT_Read(0, ((char *) &ulSize)+i, 1) < 1) {
        vTaskDelay(20);
      }
    }

    /* ignore magic number */
    if (ulSize == RTT_ETH_MAGIC_NUMBER) {
      continue;
    }

    /* allocate buffer */
    pxNetworkBuffer = pxGetNetworkBufferWithDescriptor( ulSize, 0 );

    /* buffer allocated successfully? */
    if (pxNetworkBuffer != NULL) {
      /* print info */
      //UnityPrintf("Receiving packet... %d\n", ulSize);

      /* set pucRecvBuf to point to the allocated network buffer */
      pucRecvBuf = pxNetworkBuffer->pucEthernetBuffer;

      /* set buffer data length */
      pxNetworkBuffer->xDataLength = ( size_t ) ulSize;

      /* read the packet */
      ulOff = 0;
      while (ulOff < ulSize) {
        ulOff += SEGGER_RTT_Read(0, pucRecvBuf+ulOff, ulSize-ulOff);
        if (ulOff < ulSize) {
          //UnityPrintf("Stuck %d\n", ulOff);
          vTaskDelay(20);
        }
      }

      /* create event */
      xRxEvent.pvData = ( void * ) pxNetworkBuffer;
      xEventSuccess = xSendEventStructToIPTask( &xRxEvent, ( TickType_t ) 0 );

      /* if event was not successfully created, release the buffer */
      if (xEventSuccess == pdFAIL) {
        vReleaseNetworkBufferAndDescriptor( pxNetworkBuffer );
      }
    } else {
      /* print info */
      //UnityPrintf("Dropping packet... %d\n", ulSize);

      /* receive the packet and drop it */
      ulOff = 0;
      while (ulOff < ulSize) {
        ulOff += SEGGER_RTT_Read(0, (char *) &ulDrop, sizeof(ulDrop));
      }
    }
  }
}
