#include <inttypes.h>
#include <stdio.h>

#include <bl702.h>
#include <bl_irq.h>
#include <lmac154.h>
#include <bl_wireless.h>
#include <utils_list.h>

#include <openthread/config.h>
#include <openthread/tasklet.h>
#include <openthread/platform/time.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>
#include <openthread_port.h>

#include <platforms/utils/mac_frame.h>

#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE

#define DEBUG_ENABLE 0 

#define OTRADIO_MAC_HEADER_ACK_REQUEST_MASK (1 << 5)
#define OTRADIO_MAX_PSDU                    128
#define OTRADIO_RX_FRAME_BUFFER_NUM         16

typedef struct _otRadio_rxFrame_t {
    utils_dlist_t       dlist;
    otRadioFrame        frame;
} otRadio_rxFrame_t;

#define ALIGNED_RX_FRAME_SIZE  ((sizeof(otRadio_rxFrame_t) + 3) & 0xfffffffc)
#define TOTAL_RX_FRAME_SIZE (ALIGNED_RX_FRAME_SIZE + OTRADIO_MAX_PSDU)

typedef struct _otRadio_t {
    otInstance              *aInstance;
    utils_dlist_t           rxFrameList;
    utils_dlist_t           frameList;
    otRadioFrame            *pTxFrame;
    otRadioFrame            *pAckFrame;
    uint32_t                isCoexEnabled;

    uint64_t                tstx;
    uint64_t                tsIsr;

    uint32_t                dbgRxFrameNum;
    uint32_t                dbgFrameNum;
    uint32_t                dbgMaxAckFrameLenth;
    uint32_t                dbgMaxPendingFrameNum;
    uint8_t                 buffPool[TOTAL_RX_FRAME_SIZE * (OTRADIO_RX_FRAME_BUFFER_NUM + 2)];
} otRadio_t;

otRadio_t otRadio_var;

void ot_radioInit(void) 
{
    otRadio_rxFrame_t *pframe = NULL;

    memset(&otRadio_var, 0, offsetof(otRadio_t, buffPool));

    otRadio_var.pAckFrame = (otRadioFrame *)(otRadio_var.buffPool + TOTAL_RX_FRAME_SIZE);
    otRadio_var.pAckFrame->mPsdu = ((uint8_t *)otRadio_var.pAckFrame) + ALIGNED_RX_FRAME_SIZE;

    OT_ENTER_CRITICAL();
    utils_dlist_init(&otRadio_var.frameList);
    utils_dlist_init(&otRadio_var.rxFrameList);

    for (int i = 0; i < OTRADIO_RX_FRAME_BUFFER_NUM; i ++) {
        pframe = (otRadio_rxFrame_t *) (otRadio_var.buffPool + TOTAL_RX_FRAME_SIZE * (i + 2));
        pframe->frame.mPsdu = ((uint8_t *)pframe) + ALIGNED_RX_FRAME_SIZE;
        utils_dlist_add_tail(&pframe->dlist, &otRadio_var.frameList);
    }

    otRadio_var.dbgFrameNum = OTRADIO_RX_FRAME_BUFFER_NUM;
    OT_EXIT_CRITICAL();
}

void ot_radioTask(ot_system_event_t trxEvent) 
{
    otRadio_rxFrame_t *pframe;
    otRadioFrame        *txframe;

    if (!(OT_SYSTEM_EVENT_RADIO_ALL_MASK & trxEvent)) {
        return;
    }

    if (otRadio_var.pTxFrame) {

        if ((OT_SYSTEM_EVENT_RADIO_TX_ALL_MASK & trxEvent)) {
            txframe = otRadio_var.pTxFrame;
            otRadio_var.pTxFrame = NULL;
            otRadio_var.tstx = 0;

            if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ) {
                otPlatRadioTxDone(otRadio_var.aInstance, txframe, NULL, OT_ERROR_NONE);
            }
            else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_ACKED) {
                otPlatRadioTxDone(otRadio_var.aInstance, txframe, otRadio_var.pAckFrame, OT_ERROR_NONE);
            }
            else if (trxEvent & OT_SYSTEM_EVENT_RADIO_TX_NO_ACK) {
                otPlatRadioTxDone(otRadio_var.aInstance, txframe, NULL, OT_ERROR_NO_ACK);
            }
        }
    }
    
    if (trxEvent & OT_SYSTEM_EVENT_RADIO_RX_DONE ) {

        pframe = NULL;
        OT_ENTER_CRITICAL();
        if (!utils_dlist_empty(&otRadio_var.rxFrameList)) {
            pframe = (otRadio_rxFrame_t *)otRadio_var.rxFrameList.next;
            otRadio_var.dbgRxFrameNum --;
            utils_dlist_del(&pframe->dlist);
        }
        OT_EXIT_CRITICAL();

        configASSERT(pframe);
        if (pframe) {
            otPlatRadioReceiveDone(otRadio_var.aInstance, &pframe->frame, OT_ERROR_NONE);

            OT_ENTER_CRITICAL();
            utils_dlist_add_tail(&pframe->dlist, &otRadio_var.frameList);
            otRadio_var.dbgFrameNum ++;
            OT_EXIT_CRITICAL();

            if (!utils_dlist_empty(&otRadio_var.rxFrameList)) {
                /**
                 * [weiyin], issue OT_SYSTEM_EVENT_RADIO_RX_DONE to handle next pending packet
                 */
                OT_NOTIFY(OT_SYSTEM_EVENT_RADIO_RX_DONE);
            }
        }
    }
    else if (trxEvent & OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF) {
        otPlatRadioReceiveDone(otRadio_var.aInstance, NULL, OT_ERROR_NO_BUFS);
    }
}

otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance) 
{
    otRadio_var.aInstance = aInstance;

    otRadioFrame * txframe = (otRadioFrame *)otRadio_var.buffPool;
    txframe->mPsdu = otRadio_var.buffPool + ALIGNED_RX_FRAME_SIZE;

    return txframe;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame) 
{
    if (otRadio_var.pTxFrame == NULL) {
        otRadio_var.pTxFrame = aFrame;
#if OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
        lmac154_setTxRetry(0);
#else
        lmac154_setTxRetry(aFrame->mInfo.mTxInfo.mMaxFrameRetries);
#endif
        lmac154_setChannel((lmac154_channel_t)(aFrame->mChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN));
        lmac154_triggerTx(aFrame->mPsdu, aFrame->mLength - 2, aFrame->mInfo.mTxInfo.mCsmaCaEnabled);
        otPlatRadioTxStarted(aInstance, aFrame);

        otRadio_var.tstx = otPlatTimeGet() / 1000 + 1000;

#if DEBUG_ENABLE
        printf("otPlatRadioTransmit = %d @ %lld\r\n", aFrame->mPsdu[2], otPlatTimeGet() / 1000);
#endif
        return OT_ERROR_NONE;
    }

#if DEBUG_ENABLE
    printf("otPlatRadioTransmit = %d, invalid state. %p, %d\r\n", aFrame->mPsdu[2], otRadio_var.pTxFrame, lmac154_getRFState());
#endif

    return OT_ERROR_INVALID_STATE;
}


void lmac154_txDoneEvent (lmac154_tx_status_t tx_status)
{
    if (otRadio_var.pTxFrame) {
        if (LMAC154_TX_STATUS_TX_FINISHED == tx_status) {
            if (!(otRadio_var.pTxFrame->mPsdu[0] & OTRADIO_MAC_HEADER_ACK_REQUEST_MASK)) {
#if DEBUG_ENABLE
                printf("otPlatRadioTransmit_done = %d @ %lld, %x\r\n", otRadio_var.pTxFrame->mPsdu[2], otPlatTimeGet() / 1000, tx_status);
#endif
                OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_TX_DONE_NO_ACK_REQ);
            }
        }
        else {
#if DEBUG_ENABLE
            printf("otPlatRadioTransmit_done = %d @ %lld, %x\r\n", otRadio_var.pTxFrame->mPsdu[2], otPlatTimeGet() / 1000, tx_status);
#endif
            OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_TX_NO_ACK);
        }
    }
#if DEBUG_ENABLE
    else {
        printf("lmac154_txDoneEvent, %d, invalid\r\n", tx_status);
    }
#endif
}

void lmac154_ackFrameEvent(uint8_t ack_received, uint8_t *rx_buf, uint8_t len)
{
    if (otRadio_var.pTxFrame) {
#if DEBUG_ENABLE
        printf("otPlatRadioTransmit_done_ack = %d @ %lld\r\n", otRadio_var.pTxFrame->mPsdu[2], otPlatTimeGet() / 1000);
#endif
    if (ack_received) {
        memcpy(otRadio_var.pAckFrame->mPsdu, rx_buf, len);
        otRadio_var.pAckFrame->mLength = len;

        otRadio_var.pAckFrame->mChannel = lmac154_getChannel() + OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;
        otRadio_var.pAckFrame->mInfo.mRxInfo.mRssi = lmac154_getRSSI();
        otRadio_var.pAckFrame->mInfo.mRxInfo.mLqi = lmac154_getLQI();

        otRadio_var.pAckFrame->mInfo.mRxInfo.mTimestamp = (lmac154_getRxEndSymb() - (len + 1) * 2) * OT_RADIO_SYMBOL_TIME;

        if (otRadio_var.dbgMaxAckFrameLenth < len) {
            otRadio_var.dbgMaxAckFrameLenth = len;
        }
        OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_TX_ACKED);
    }
    else {
        OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_TX_NO_ACK);
    }
}
#if DEBUG_ENABLE
    else {
        if (rx_buf) {
            printf("lmac154_txDoneEvent, %d, invalid, seq = %d\r\n", ack_received, rx_buf[2]);
        }
        else {
            printf("lmac154_txDoneEvent, %d, invalid, no_ack\r\n", ack_received);
        }
    }
#endif
}

static void ot_radioIsr(void) 
{
    otRadio_var.tsIsr = otPlatTimeGet();
    lmac154_getInterruptHandler()();
}

void lmac154_rxDoneEvent(uint8_t *rx_buf, uint8_t rx_len, uint8_t crc_fail)
{
    otRadio_rxFrame_t *p = NULL;

    if (crc_fail) {
        return;
    }

    if (lmac154_isRxPromiscuousModeEnabled() && (rx_buf[0] & (1 << 5))) {
        lmac154_enableRx();
    }

    UBaseType_t uxSavedInterruptStatus = OT_ENTER_CRITICAL_ISR();
    if (!utils_dlist_empty(&otRadio_var.frameList)) {
        p = (otRadio_rxFrame_t *)otRadio_var.frameList.next;
        otRadio_var.dbgFrameNum --;
        utils_dlist_del(&p->dlist);
    }
    OT_EXIT_CRITICAL_ISR( uxSavedInterruptStatus );

    if (p) {
        memcpy(p->frame.mPsdu, rx_buf, rx_len);
        p->frame.mLength = rx_len;
        p->frame.mChannel = lmac154_getChannel() + OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;
        p->frame.mInfo.mRxInfo.mRssi = lmac154_getRSSI();
        p->frame.mInfo.mRxInfo.mLqi = lmac154_getLQI();

        p->frame.mInfo.mRxInfo.mTimestamp = (lmac154_getRxEndSymb() - (p->frame.mLength + 1) * 2) * OT_RADIO_SYMBOL_TIME;

        p->frame.mInfo.mRxInfo.mAckedWithFramePending = false;
        if (rx_buf[0] & OTRADIO_MAC_HEADER_ACK_REQUEST_MASK) {
            otMacAddress addr;
            uint8_t pending = 0;
            lmac154_fpt_status_t status = LMAC154_FPT_STATUS_SUCCESS;

            otMacFrameGetSrcAddr(&(p->frame), &addr);

            if (addr.mType == OT_MAC_ADDRESS_TYPE_EXTENDED) {
                status = lmac154_fptGetLongAddrPending(addr.mAddress.mExtAddress.m8, &pending);
            }
            else if (addr.mType == OT_MAC_ADDRESS_TYPE_SHORT) {
                status = lmac154_fptGetShortAddrPending(addr.mAddress.mShortAddress, &pending);
            }

            if (status == LMAC154_FPT_STATUS_ADDR_NOT_FOUND || pending) {
                p->frame.mInfo.mRxInfo.mAckedWithFramePending = 1;
            }
        }

        uxSavedInterruptStatus = OT_ENTER_CRITICAL_ISR();
        utils_dlist_add_tail(&p->dlist, &otRadio_var.rxFrameList);
        otRadio_var.dbgRxFrameNum ++;
        if (otRadio_var.dbgMaxPendingFrameNum < otRadio_var.dbgRxFrameNum) {
            otRadio_var.dbgMaxPendingFrameNum = otRadio_var.dbgRxFrameNum;
        }
        OT_EXIT_CRITICAL_ISR( uxSavedInterruptStatus ); 

        OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_RX_DONE);
    }
    else {
        OT_NOTIFY_ISR(OT_SYSTEM_EVENT_RADIO_RX_NO_BUFF);
    }
}

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance) 
{
#if OPENTHREAD_CONFIG_MAC_SOFTWARE_RETRANSMIT_ENABLE
    return OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_CSMA_BACKOFF | OT_RADIO_CAPS_SLEEP_TO_TX;
#else
    return OT_RADIO_CAPS_ACK_TIMEOUT | OT_RADIO_CAPS_CSMA_BACKOFF | OT_RADIO_CAPS_SLEEP_TO_TX | OT_RADIO_CAPS_TRANSMIT_RETRIES;
#endif
}
const char *otPlatRadioGetVersionString(otInstance *aInstance) 
{
    return lmac154_getLibVersion();
}
int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance) 
{
    return -103;
}
void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64) 
{
    bl_wireless_mac_addr_get(aIeeeEui64);
}

void otPlatRadioSetPanId(otInstance *aInstance, otPanId aPanId) 
{
    lmac154_setPanId(aPanId);
}
void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aExtAddress) 
{
    lmac154_setLongAddr((uint8_t *) aExtAddress->m8);
} 

void otPlatRadioSetShortAddress(otInstance *aInstance, otShortAddress aShortAddress) 
{
    lmac154_setShortAddr(aShortAddress);
}

otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower) 
{
    *aPower = lmac154_getTxPower();
    return OT_ERROR_NONE;
}
otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower) 
{
    if (aPower == LMAC154_TX_POWER_0dBm) {
        aPower = LMAC154_TX_POWER_0dBm;
    }
    else if (aPower < LMAC154_TX_POWER_14dBm) {
        aPower = LMAC154_TX_POWER_10dBm;
    }
    else {
        aPower = LMAC154_TX_POWER_14dBm;
    }

    lmac154_setTxPower(aPower);
    return OT_ERROR_NONE;
}

otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold) 
{
    *aThreshold = lmac154_getEDThreshold();
    return OT_ERROR_NONE;
}
otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold) 
{
    lmac154_setEDThreshold(aThreshold);
    return OT_ERROR_NONE;
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance) 
{
    return lmac154_isRxPromiscuousModeEnabled() == 1;
}
void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable) 
{
    if (aEnable) {
        lmac154_enableRxPromiscuousMode(true, false);
        // lmac154_disableAckEvent();
        // lmac154_disableHwAutoTxAck();
    }
    else {
        lmac154_disableRxPromiscuousMode();
        // lmac154_enableAckEvent();
        // lmac154_enableHwAutoTxAck();
    }
}

void otPlatRadioSetMacKey(otInstance *            aInstance,
                          uint8_t                 aKeyIdMode,
                          uint8_t                 aKeyId,
                          const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey,
                          const otMacKeyMaterial *aNextKey,
                          otRadioKeyType          aKeyType){}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter) {}

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    return UINT64_MAX;
}

otRadioState otPlatRadioGetState(otInstance *aInstance) 
{
    otRadioState state = 0;
    lmac154_rf_state_t rfstate = 0;

    if(lmac154_isDisabled()) 
    {
        state = OT_RADIO_STATE_DISABLED;
    }
    else {
        if (otRadio_var.pTxFrame) {
            /** mac layer retring doesn't get done */
            state = OT_RADIO_STATE_TRANSMIT;
        }
        else {
        rfstate = lmac154_getRFState();
        switch (rfstate)
        {
        case LMAC154_RF_STATE_RX:
        case LMAC154_RF_STATE_RX_DOING:
            state = OT_RADIO_STATE_RECEIVE;
            break;
            case LMAC154_RF_STATE_TX:
            state = OT_RADIO_STATE_TRANSMIT;
            break;
        case LMAC154_RF_STATE_IDLE:
            state = OT_RADIO_STATE_SLEEP;
            break;
        default:
            state = OT_RADIO_STATE_INVALID;
        }
    }
    }

    return state;
}

otError otPlatRadioEnable(otInstance *aInstance) 
{
    uint64_t ex = 0xffffffffffffffff;

    bl_irq_register(M154_IRQn, ot_radioIsr);
    lmac154_init();
    otPlatRadioSetCoexEnabled(aInstance, true);

    bl_irq_enable(M154_IRQn);

    /**
     * [weiyin], reset source address to all 1
     */
    lmac154_setPanId(0xffff);
    lmac154_setShortAddr(0xffff);
    memset(&ex, 0xff, sizeof(ex));
    lmac154_setLongAddr((uint8_t *) &ex);

    lmac154_enableFrameTypeFiltering(LMAC154_FRAME_TYPE_BEACON | LMAC154_FRAME_TYPE_DATA | LMAC154_FRAME_TYPE_ACK | LMAC154_FRAME_TYPE_CMD);

    return OT_ERROR_NONE;
}
otError otPlatRadioDisable(otInstance *aInstance) 
{
    bl_irq_disable(M154_IRQn);
    lmac154_disableRx();
    return OT_ERROR_NONE;
}
bool otPlatRadioIsEnabled(otInstance *aInstance) 
{
    return !lmac154_isDisabled();
}

otError otPlatRadioSleep(otInstance *aInstance) 
{
    lmac154_disableRx();
    return OT_ERROR_NONE;
}
otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel) 
{
    uint8_t ch = aChannel - OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;

    lmac154_setChannel((lmac154_channel_t)ch);
    lmac154_enableRx();
    return OT_ERROR_NONE;
}


int8_t otPlatRadioGetRssi(otInstance *aInstance) 
{
    int8_t rssi =  lmac154_getRSSI();
    return rssi;
}
otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration) 
{
    return OT_ERROR_NOT_IMPLEMENTED;
}
//extern void otPlatRadioEnergyScanDone(otInstance *aInstance, int8_t aEnergyScanMaxRssi) {}
void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable) 
{
    lmac154_fptForcePending(!aEnable);
}
otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, otShortAddress aShortAddress) 
{
    return lmac154_fptSetShortAddrPending(aShortAddress, 1) == LMAC154_FPT_STATUS_SUCCESS? OT_ERROR_NONE : OT_ERROR_FAILED;
}
otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress) 
{
    return lmac154_fptSetLongAddrPending((uint8_t *)aExtAddress->m8, 1) == LMAC154_FPT_STATUS_SUCCESS? OT_ERROR_NONE : OT_ERROR_FAILED;
}
otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, otShortAddress aShortAddress) 
{
    return lmac154_fptRemoveShortAddr(aShortAddress) == LMAC154_FPT_STATUS_SUCCESS ? OT_ERROR_NONE:OT_ERROR_NO_ADDRESS;
}
otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress) 
{
    return lmac154_fptRemoveLongAddr((uint8_t *)aExtAddress->m8) == LMAC154_FPT_STATUS_SUCCESS ? OT_ERROR_NONE:OT_ERROR_NO_ADDRESS;
}
void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance) 
{
    uint8_t num = 128;
    uint16_t * plist = (uint16_t *)pvPortMalloc(sizeof(uint16_t) * num);

    lmac154_fpt_GetShortAddrList(plist, &num);
    for (uint32_t i = 0; i < num; i ++) 
    {
        lmac154_fptRemoveShortAddr(plist[i]);
    }
    vPortFree(plist);
}
void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance) 
{
    uint8_t num = 32;
    uint8_t * plist = (uint8_t *)pvPortMalloc(sizeof(uint8_t) * 8 * num);

    lmac154_fptGetLongAddrList(plist, &num);
    for (uint32_t i = 0; i < num; i ++) 
    {
        lmac154_fptRemoveLongAddr(plist + i * 8);
    }
    vPortFree(plist);
}
uint32_t otPlatRadioGetSupportedChannelMask(otInstance *aInstance) 
{
    return OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MASK;
}
uint32_t otPlatRadioGetPreferredChannelMask(otInstance *aInstance) 
{
    return OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MASK;
}

otError otPlatRadioSetCoexEnabled(otInstance *aInstance, bool aEnabled) 
{
    if (aEnabled) {
        lmac154_enableCoex();
    }
    else {
        lmac154_disableCoex();
    }

    otRadio_var.isCoexEnabled = aEnabled;
    return OT_ERROR_NONE;
}
bool otPlatRadioIsCoexEnabled(otInstance *aInstance) 
{
    return otRadio_var.isCoexEnabled != 0;
}
// otError otPlatRadioGetCoexMetrics(otInstance *aInstance, otRadioCoexMetrics *aCoexMetrics) 
// {
//     return OT_ERROR_NONE;
// }


// otError otPlatRadioReceiveAt(otInstance *aInstance, uint8_t aChannel, uint32_t aStart, uint32_t aDuration);
// otError otPlatRadioEnableCsl(otInstance *        aInstance,
//                              uint32_t            aCslPeriod,
//                              otShortAddress      aShortAddr,
//                              const otExtAddress *aExtAddr);
// void otPlatRadioUpdateCslSampleTime(otInstance *aInstance, uint32_t aCslSampleTime);
// uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance);
// uint8_t otPlatRadioGetCslClockUncertainty(otInstance *aInstance);
// otError otPlatRadioSetChannelMaxTransmitPower(otInstance *aInstance, uint8_t aChannel, int8_t aMaxPower);

// otError otPlatRadioConfigureEnhAckProbing(otInstance *        aInstance,
//                                           otLinkMetrics       aLinkMetrics,
//                                           otShortAddress      aShortAddress,
//                                           const otExtAddress *aExtAddress);


// otError otPlatRadioSetRegion(otInstance *aInstance, uint16_t aRegionCode);
// otError otPlatRadioGetRegion(otInstance *aInstance, uint16_t *aRegionCode);
