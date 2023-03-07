#include "audio_tts.h"

static audio_tts_dev_t *gp_audio_dac_dev = NULL;

static audio_tts_item_t *__next_transfer (audio_framework_device_t *device)
{
    audio_tts_event_t *now_ev, *next_ev;
    audio_tts_item_t *now, *next;

    if ( NULL == gp_audio_dac_dev) {
        return NULL;
    }

    now = (audio_tts_item_t *)gp_audio_dac_dev->dma_irq_item;
    now_ev = (audio_tts_event_t *)gp_audio_dac_dev->dma_irq_ev;

    if (now) {
        now->cb(now->arg);
    }
    if ((next = (audio_tts_item_t *)utils_list_pop_front(&now_ev->item)) == NULL) {
        if (now_ev->cb) {
            now_ev->cb(now_ev->arg);
        }
        if ((next_ev = (audio_tts_event_t *)utils_list_pop_front(&gp_audio_dac_dev->dma_list_head)) != NULL) {
            next = (audio_tts_item_t *)utils_list_pop_front(&next_ev->item);
            gp_audio_dac_dev->dma_irq_ev = next_ev;
        }
    }

    gp_audio_dac_dev->dma_irq_item = next;

    if (next == NULL) {
        gp_audio_dac_dev->dma_irq_item = NULL;
    }
    return gp_audio_dac_dev->dma_irq_item;
}

static void __audio_data_clear (uint8_t *ptr, size_t size)
{
    uint16_t *p_16 = (uint16_t *)ptr;
    int i;

    for (i = 0; i < (size >> 1); i++) {
        p_16[i] = 1UL << (DAC_AUDIO_BITS - 1);
    }
}

static void __audio_dual (void *p_audio, const void *p_data, size_t size)
{
    uint16_t *p_audio16 = (uint16_t *)p_audio;
    uint16_t *p_data16 = (uint16_t *)p_data;

    size >>= 1;

    while (size--) {
        *p_audio16 = *p_data16;
        *(p_audio16 + 1) = *p_data16;
        p_audio16 += 2;
        p_data16++;
    }
}

static int __transfer_complete(audio_framework_device_t *device,
                                void *uerdata,
                                uint8_t *audiodata,
                                int len,
                                int is_underrun)
{
    uint32_t size, dual_len;
    int rem_size;
    audio_tts_item_t *now = (audio_tts_item_t *)gp_audio_dac_dev->dma_irq_item;

    if (is_underrun) {
        device->stop(device);
        return 0;
    }
    printf("audio %p r_size %ld now %p\r\n", audiodata, gp_audio_dac_dev->r_size, now);

    dual_len = len >> 1;
    if (now == NULL) {
        __audio_data_clear(audiodata, len);
        if (++gp_audio_dac_dev->end_flag >= 2) {
            gp_audio_dac_dev->end_flag = 0;
            return -1;
        }
        return 0;
    }
    rem_size = now->num_file.bufsize - gp_audio_dac_dev->r_size;

    if (rem_size == 0) {
        gp_audio_dac_dev->r_size = 0;
        now = __next_transfer(device);
    }
    if (now) {
        rem_size = now->num_file.bufsize - gp_audio_dac_dev->r_size;
        if (rem_size > 0) {
            size = (rem_size > dual_len) ? dual_len : rem_size;
            __audio_dual(audiodata, &now->num_file.buf[gp_audio_dac_dev->r_size], size);
            if (size < dual_len) {
                if ((now = __next_transfer(device)) != NULL) {
                    __audio_dual(&audiodata[size << 1], now->num_file.buf, dual_len - size);
                    gp_audio_dac_dev->r_size = dual_len - size;
                } else {
                    __audio_data_clear(&audiodata[size << 1], dual_len - size);
                    gp_audio_dac_dev->r_size = 0;
                    gp_audio_dac_dev->end_flag = 0;
                }
            } else {
                gp_audio_dac_dev->r_size += size;
            }
        }
    } else {
        __audio_data_clear(audiodata, len);
    }

    return 0;
}

static void __audio_item_trans_complete (void *p_arg)
{
    audio_tts_item_t *now = (audio_tts_item_t *)p_arg;

    printf("DMA item trans completed num %d\r\n", now->num);

    vPortFree(p_arg);
}

static void __audio_event_callback (void *p_arg)
{
    audio_tts_event_t *p_ev = (audio_tts_event_t *)p_arg;

    printf("audio event trans completed\r\n");

    if (p_ev->usr_cb) {
        p_ev->usr_cb(p_ev->p_usr_arg);
    }
    vPortFree(p_arg);
}

static void __add_num_to_list (audio_tts_event_t *p_ev,
                               audio_tts_dev_t *p_anum,
                               uint8_t num)
{
    audio_tts_item_t *p_item;

    if (num >= DAC_AUDIO_NUM_END) {
        return;
    }

    p_item = pvPortMalloc(sizeof(audio_tts_item_t));
    if (NULL == p_item) {
        printf("p_item malloc failed !\r\n");
        return;
    }
    memset(p_item, 0, sizeof(audio_tts_item_t));

    p_item->cb  = __audio_item_trans_complete;
    p_item->arg = (void *)p_item;

    p_item->num = num;
    p_item->num_file = p_anum->num_filebuf[num];
    utils_list_push_front(&p_ev->item, &(p_item->item));
}

static void __add_num_thousand (audio_tts_event_t *p_ev, audio_tts_dev_t *p_anum, int num)
{
    int i;
    uint8_t first_flag, zero_flag, num_unit;

    zero_flag  = 0;
    first_flag = 0;
    i = DAC_AUDIO_NUM_TEN - 1;

    do {
        num_unit = num % 10;

        /* add unit */
        if ((i >= DAC_AUDIO_NUM_TEN) && (num_unit != 0)) {
            if (i <= DAC_AUDIO_NUM_THOUSAND) {
                __add_num_to_list(p_ev, p_anum, i);
            } else {
                /* not support */
                printf("not support\r\n");
            }
        }

        /* add 0 */
        if ((i == DAC_AUDIO_NUM_TEN - 1) && (num == 0)) {
            __add_num_to_list(p_ev, p_anum, num_unit);
        }

        /* 第一次遇到不为0的数，则add*/
        if ((first_flag == 1) || (num_unit != 0)) {
            first_flag = 1;

            /* 连续遇到0，则合并成一个0 */
            if ((!zero_flag) || (num_unit != 0)) {
                __add_num_to_list(p_ev, p_anum, num_unit);
            }
            zero_flag = num_unit ? 0 : 1;
        }

        num /= 10;
        i++;
    } while (num);
}

static void __add_num_ten_thousand (audio_tts_event_t *p_ev, audio_tts_dev_t *p_anum, uint64_t num)
{
    int n1 = num / 10000;
    int n2 = num % 10000;

    if (n2 != 0) {
        __add_num_thousand(p_ev, p_anum, n2);
        if (n2 < 1000) {
            __add_num_to_list(p_ev, p_anum, 0);
        }
    }
    if (n1 != 0) {
        __add_num_to_list(p_ev, p_anum, DAC_AUDIO_NUM_TEN_THOUSAND);
        __add_num_thousand(p_ev, p_anum, n1);
    }
}

static void __add_num_billion (audio_tts_event_t *p_ev, audio_tts_dev_t *p_anum, uint64_t num)
{
    int n1 = num / 100000000;
    int n2 = num % 100000000;

    if (n2 != 0) {
        __add_num_ten_thousand(p_ev, p_anum, n2);
//        if (n2 < 1000) {
//            __add_num_to_list(p_ev, p_anum, 0);
//        }
    }
    if (n1 != 0) {
        __add_num_to_list(p_ev, p_anum, DAC_AUDIO_NUM_BILLION);
        __add_num_thousand(p_ev, p_anum, n1);
    }
}

int audio_tts_num_play(audio_tts_dev_t *p_dev,
                       uint64_t num_int,
                       uint8_t num_decimal,
                       audio_dac_pfn_t cb,
                       void *p_arg)
{
    audio_tts_event_t *p_ev = pvPortMalloc(sizeof(audio_tts_event_t));

    if (NULL == p_ev) {
        printf("p_item malloc failed !\r\n");
        return -1;
    }
    memset(p_ev, 0, sizeof(audio_tts_event_t));

    utils_list_init(&p_ev->item);

    p_ev->usr_cb = cb;
    p_ev->p_usr_arg = p_arg;
    p_ev->cb  = __audio_event_callback;
    p_ev->arg = (void *)p_ev;
    utils_list_push_back(&p_dev->dma_list_head, &(p_ev->hdr));

    /* 保留两位小数 */
    if (num_decimal) {
        num_decimal = (num_decimal > 99) ? (num_decimal / 10) : num_decimal;
        __add_num_to_list(p_ev, p_dev, num_decimal % 10);
        __add_num_to_list(p_ev, p_dev, num_decimal / 10);
        __add_num_to_list(p_ev, p_dev, DAC_AUDIO_NUM_POINT);
    }

    if (num_int < 10000) {
        __add_num_thousand(p_ev, p_dev, num_int);
    } else if (num_int < 100000000) {
        __add_num_ten_thousand(p_ev, p_dev, num_int);
    } else {
        __add_num_billion(p_ev, p_dev, num_int);
    }

    return 0;
}

int audio_tts_characters_play (audio_tts_dev_t *p_dev,
                               romfs_filebuf_t *p_file,
                               audio_dac_pfn_t cb,
                               void *p_arg)
{
    audio_tts_event_t *p_ev = pvPortMalloc(sizeof(audio_tts_event_t));
    audio_tts_item_t *p_item = pvPortMalloc(sizeof(audio_tts_item_t));

    if (NULL == p_ev || NULL == p_item) {
        printf("p_item malloc failed !\r\n");
        return -1;
    }
    memset(p_ev, 0, sizeof(audio_tts_event_t));

    utils_list_init(&p_ev->item);

    p_ev->usr_cb = cb;
    p_ev->p_usr_arg = p_arg;
    p_ev->cb  = __audio_event_callback;
    p_ev->arg = (void *)p_ev;

    memset(p_item, 0, sizeof(audio_tts_item_t));

    p_item->num = 0xff;
    p_item->num_file = *p_file;
    p_item->cb = __audio_item_trans_complete;
    p_item->arg = (void *)p_item;
    utils_list_push_front(&p_ev->item, &p_item->item);
    utils_list_push_back(&p_dev->dma_list_head, &p_ev->hdr);

    return 0;
}

int audio_tts_start (audio_tts_dev_t *p_dev)
{
    audio_tts_event_t *f_ev;
    audio_tts_item_t *first;

    if (NULL == p_dev) {
        return -1;
    }

    f_ev = (audio_tts_event_t *)utils_list_pop_front(&p_dev->dma_list_head);
    if (NULL == f_ev) {
        printf("here for empty chain\r\n");
        return -1;
    }
    first = (audio_tts_item_t *)utils_list_pop_front(&f_ev->item);
    if (NULL == f_ev) {
        printf("here for empty chain\r\n");
        return -1;
    }
    gp_audio_dac_dev->dma_irq_ev = f_ev;
    gp_audio_dac_dev->dma_irq_item = first;
    gp_audio_dac_dev->end_flag = 0;

    p_dev->p_audio_dev->start(p_dev->p_audio_dev);

    return 0;
}

int audio_tts_stop (audio_tts_dev_t *p_dev)
{
    return 0;
}

int audio_tts_init (audio_tts_dev_t *p_dev, const char *name)
{
    int i, fd_audio, ret;
    char path[20] = {0};

    if (NULL == p_dev) {
        return -1;
    }
    gp_audio_dac_dev = p_dev;

    memset(p_dev, 0, sizeof(audio_tts_dev_t));

    ret = audio_framework_factory_build(name, &p_dev->p_audio_dev);
    if (ret != 0) {
        printf("audio_tts_init error \r\n");
        return ret;
    }

    for (i = 0; i < NUM_FILEBUF_MAX_NUMS; i++) {
        sprintf(path, "/romfs/%d", i);
        fd_audio = aos_open(path, 0);
        if (fd_audio < 0) {
            printf("open %d failed \r\n", i);
            continue;
        }
        aos_ioctl(fd_audio,
                  IOCTL_ROMFS_GET_FILEBUF,
                  (long unsigned int)&p_dev->num_filebuf[i]);
        aos_close(fd_audio);
        printf("%d num_buf = %p, bufsize = %lu\r\n",
                i,
                p_dev->num_filebuf[i].buf,
                p_dev->num_filebuf[i].bufsize);
    }

    utils_list_init(&p_dev->dma_list_head);

    p_dev->p_audio_dev->init(p_dev->p_audio_dev);
    p_dev->p_audio_dev->set_buffersize_tx(p_dev->p_audio_dev, 4096, NULL);
    p_dev->p_audio_dev->config(p_dev->p_audio_dev);
    p_dev->p_audio_dev->set_txcallback(p_dev->p_audio_dev, __transfer_complete);

    return 0;
}

int audio_tts_deinit (audio_tts_dev_t *p_dev)
{
    p_dev->p_audio_dev->unint(p_dev->p_audio_dev);
    return 0;
}
