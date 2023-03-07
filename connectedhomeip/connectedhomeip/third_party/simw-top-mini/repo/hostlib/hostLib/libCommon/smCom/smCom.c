/*
 *
 * Copyright 2016-2020 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @par Description
 * Implements installable communication layer to exchange APDU's between Host and Secure Module.
 * Allows the top half of the Host Library to be independent of the actual interconnect
 * between Host and Secure Module
 */
#include <stdio.h>
#include "smCom.h"
#include "nxLog_smCom.h"

#if defined(USE_RTOS) && (USE_RTOS == 1)
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#if defined(SMCOM_JRCP_V2)
#include "smComJRCP.h"
#endif

#if defined(USE_RTOS) && (USE_RTOS == 1)
static SemaphoreHandle_t gSmComlock;
#elif (__GNUC__ && !AX_EMBEDDED)
#include<pthread.h>
    /* Only for base session with os */
    static pthread_mutex_t gSmComlock;
#endif

#if (__GNUC__ && !AX_EMBEDDED) || (USE_RTOS)
#define USE_LOCK 1
#else
#define USE_LOCK 0
#endif

#if defined(USE_RTOS) && (USE_RTOS == 1)
#define LOCK_TXN()                                             \
    LOG_D("Trying to Acquire Lock");                           \
    if (xSemaphoreTake(gSmComlock, portMAX_DELAY) == pdTRUE) { \
        LOG_D("LOCK Acquired");                                \
    }                                                          \
    else {                                                     \
        LOG_D("LOCK Acquisition failed");                      \
    }
#define UNLOCK_TXN()                            \
    LOG_D("Trying to Released Lock");           \
    if (xSemaphoreGive(gSmComlock) == pdTRUE) { \
        LOG_D("LOCK Released");                 \
    }                                           \
    else {                                      \
        LOG_D("LOCK Releasing failed");         \
    }
#elif (__GNUC__ && !AX_EMBEDDED)
#define LOCK_TXN()                                               \
    LOG_D("Trying to Acquire Lock thread: %ld", pthread_self()); \
    pthread_mutex_lock(&gSmComlock);                             \
    LOG_D("LOCK Acquired by thread: %ld", pthread_self());

#define UNLOCK_TXN()                                                 \
    LOG_D("Trying to Released Lock by thread: %ld", pthread_self()); \
    pthread_mutex_unlock(&gSmComlock);                               \
    LOG_D("LOCK Released by thread: %ld", pthread_self());
#else
#define LOCK_TXN() LOG_D("no lock mode");
#define UNLOCK_TXN() LOG_D("no lock mode");
#endif

static ApduTransceiveFunction_t pSmCom_Transceive = NULL;
static ApduTransceiveRawFunction_t pSmCom_TransceiveRaw = NULL;

/**
 * Install interconnect and protocol specific implementation of APDU transfer functions.
 *
 */
U16 smCom_Init(ApduTransceiveFunction_t pTransceive, ApduTransceiveRawFunction_t pTransceiveRaw)
{
    U16 ret = SMCOM_COM_INIT_FAILED;
#if defined(USE_RTOS) && (USE_RTOS == 1)
    gSmComlock = xSemaphoreCreateMutex();
    if (gSmComlock == NULL) {
        LOG_E("\n xSemaphoreCreateMutex failed");
        return ret;
    }
#elif (__GNUC__ && !AX_EMBEDDED)
    if (pthread_mutex_init(&gSmComlock, NULL) != 0)
    {
        LOG_E("\n mutex init has failed");
        return ret;
    }
#endif
    pSmCom_Transceive = pTransceive;
    pSmCom_TransceiveRaw = pTransceiveRaw;
    ret = SMCOM_OK;
    return ret;
}

void smCom_DeInit(void)
{
#if defined(USE_RTOS) && (USE_RTOS == 1)
    if (gSmComlock != NULL) {
    	vSemaphoreDelete(gSmComlock);
        gSmComlock = NULL;
    }
#elif (__GNUC__ && !AX_EMBEDDED)
    pthread_mutex_destroy(&gSmComlock);
#endif
    pSmCom_Transceive = NULL;
    pSmCom_TransceiveRaw = NULL;
}

/**
 * Exchanges APDU without interpreting the message exchanged
 *
 * @param[in,out] pApdu        apdu_t datastructure
 *
 * @retval ::SMCOM_OK          Operation successful
 * @retval ::SMCOM_SND_FAILED  Send Failed
 * @retval ::SMCOM_RCV_FAILED  Receive Failed
 */
U32 smCom_Transceive(void *conn_ctx, apdu_t * pApdu)
{
    U32 ret = SMCOM_NO_PRIOR_INIT;
    if (pSmCom_Transceive != NULL)
    {
        LOCK_TXN();
        ret = pSmCom_Transceive(conn_ctx, pApdu);
        UNLOCK_TXN();
    }
    return ret;
}

/**
 * Exchanges APDU without interpreting the message exchanged
 *
 * @param[in] pTx          Command to be sent to secure module
 * @param[in] txLen        Length of command to be sent
 * @param[in,out] pRx      IN: Buffer to contain response; OUT: Response received from secure module
 * @param[in,out] pRxLen   IN: [TBD]; OUT: Length of response received
 *
 * @retval ::SMCOM_OK          Operation successful
 * @retval ::SMCOM_SND_FAILED  Send Failed
 * @retval ::SMCOM_RCV_FAILED  Receive Failed
 */
U32 smCom_TransceiveRaw(void *conn_ctx, U8 * pTx, U16 txLen, U8 * pRx, U32 * pRxLen)
{
    U32 ret = SMCOM_NO_PRIOR_INIT;
    if (pSmCom_TransceiveRaw != NULL)
    {
        LOCK_TXN();
        ret = pSmCom_TransceiveRaw(conn_ctx, pTx, txLen, pRx, pRxLen);
        UNLOCK_TXN();
    }
    return ret;
}

#if defined(SMCOM_JRCP_V2)
void smCom_Echo(void *conn_ctx, const char *comp, const char *level, const char *buffer)
{
#if USE_LOCK
    /* If this function is called before smcom init
    then Lock fails, return without echo */
    if (pSmCom_TransceiveRaw == NULL) {
        return;
    }
#endif
    LOCK_TXN();
    smComJRCP_Echo(conn_ctx, comp, level, buffer);
    UNLOCK_TXN();
}
#endif