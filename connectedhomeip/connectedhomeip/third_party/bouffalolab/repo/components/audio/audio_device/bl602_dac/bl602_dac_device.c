#include <stdint.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <portable.h>
#include <audio_framework.h>

#include <bl_dac_audio.h>

//TODO auto allocate DMA channel

#define AUDIO_BUFFER_SIZE_TX_MAX    (4096 * 4)
#define AUDIO_BUFFER_SIZE_RX_MAX    (4096 * 4)

#define AUDIO_BUFFER_SIZE_TX_MIN    (1024 * 1)
#define AUDIO_BUFFER_SIZE_RX_MIN    (1024 * 1)

#define WIDTH_DEFINE_8BIT           (0)
#define WIDTH_DEFINE_16BIT          (1)
#define WIDTH_DEFINE_24BIT          (2)
#define WIDTH_DEFINE_32BIT          (3)

typedef struct bl602_dac_device_env
{
    bl_audio_dac_dev_t dev;

    AUDIO_FRAMEWORK_SAMPLERATE_T samplerate;
    AUDIO_FRAMEWORK_FORMAT_T format;

    /*buffer holder*/
    int size_buffer_rx;
    int size_buffer_tx;
    void *buffer_ptr_tx;
    void *buffer_ptr_rx;
    audio_framework_callback_rx cb_rx;
    audio_framework_callback_rx cb_tx;

    /*audio play statistics*/
    uint64_t frames_tx;
    uint64_t frames_rx;
    uint64_t overrun_rx;
    uint64_t overrun_tx;
} bl602_dac_device_env_t;

static bl602_dac_device_env_t env;

static int bl602_dac_init(audio_framework_device_t *device)
{
    bl602_dac_device_env_t *p_env = (bl602_dac_device_env_t *)device->env;

    return bl_audio_dac_init(&p_env->dev);
}

static int bl602_dac_config(audio_framework_device_t *device)
{
    int size, ret = -1;
    void **size_ptr;

    bl602_dac_device_env_t *p_env = (bl602_dac_device_env_t *)device->env;

    size = p_env->size_buffer_tx;
    size_ptr = &(((bl602_dac_device_env_t*)(device->env))->buffer_ptr_tx);
    if ((ret = bl_audio_dac_tx_buffer_config(&p_env->dev, size_ptr, size))) {
        goto failed;
    }

    /*config rx buffer*/
    size = p_env->size_buffer_rx;
    size_ptr = &(((bl602_dac_device_env_t*)(device->env))->buffer_ptr_rx);
    if ((ret = bl_audio_dac_rx_buffer_config(&p_env->dev, size_ptr, size))) {
        goto failed;
    }

    /*samplerate config*/
    bl_audio_dac_samplerate_set(&p_env->dev, GPIP_DAC_MOD_16K);

    return 0;

failed:
    /*release lowlevel resource*/
    bl_audio_dac_deinit(&p_env->dev);

    return ret;
}

static int cb_tx(void *usrdata, uint8_t *audiodata, int len, int is_overflow)
{
    //FIXME critical section protect
    audio_framework_device_t *device;

    device = (audio_framework_device_t*)usrdata;
    if (((bl602_dac_device_env_t*)(device->env))->cb_tx) {
        return ((bl602_dac_device_env_t*)(device->env))->cb_tx(device, usrdata, audiodata, len, is_overflow);
    }

    return 0;
}

static int cb_rx(void *usrdata, uint8_t *audiodata, int len, int is_overflow)
{
    //FIXME critical section protect

    audio_framework_device_t *device;

    device = (audio_framework_device_t*)usrdata;
    if (((bl602_dac_device_env_t*)(device->env))->cb_rx) {
        return ((bl602_dac_device_env_t*)(device->env))->cb_rx(device, usrdata, audiodata, len, is_overflow);
    }

    return 0;
}

static int bl602_dac_start(audio_framework_device_t *device)
{
    bl602_dac_device_env_t *p_env = (bl602_dac_device_env_t *)device->env;

    if (((bl602_dac_device_env_t*)(device->env))->cb_tx) {
        bl_audio_dac_tx_ready_config(&p_env->dev, cb_tx, device);
    }
    if (((bl602_dac_device_env_t*)(device->env))->cb_rx) {
        bl_audio_dac_rx_ready_config(&p_env->dev, cb_rx, device);
    }

    bl_audio_dac_start(&p_env->dev);
    return 0;
}

static int bl602_dac_stop(audio_framework_device_t *device)
{
    bl602_dac_device_env_t *p_env = (bl602_dac_device_env_t *)device->env;

    bl_audio_dac_tx_ready_config(&p_env->dev, NULL, NULL);
    bl_audio_dac_rx_ready_config(&p_env->dev, NULL, NULL);
    bl_audio_dac_stop(&p_env->dev);

    return 0;
}

static int bl602_dac_pause(audio_framework_device_t *device)
{
    return 0;
}

static int bl602_dac_unint(audio_framework_device_t *device)
{
    bl602_dac_device_env_t *p_env = (bl602_dac_device_env_t *)device->env;

    bl_audio_dac_deinit(&p_env->dev);
    return 0;
}

static int bl602_dac_set_format(audio_framework_device_t *device, AUDIO_FRAMEWORK_FORMAT_T format)
{
    /*16bit*/
    switch (format) {
        case AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH16BIT_MONO:
        case AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH16BIT_STERO:
        case AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH32BIT_MONO:
        case AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH32BIT_STERO:
        case AUDIO_FRAMEWORK_FORMAT_DATA24BIT_WIDTH32BIT_MONO:
        case AUDIO_FRAMEWORK_FORMAT_DATA24BIT_WIDTH32BIT_STERO:
        case AUDIO_FRAMEWORK_FORMAT_DATA32BIT_WIDTH32BIT_MONO:
        case AUDIO_FRAMEWORK_FORMAT_DATA32BIT_WIDTH32BIT_STERO:
        {
            ((bl602_dac_device_env_t*)(device->env))->format = format;
        }
        break;
        case AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH24BIT_MONO:
        case AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH24BIT_STERO:
        case AUDIO_FRAMEWORK_FORMAT_DATA24BIT_WIDTH24BIT_MONO:
        case AUDIO_FRAMEWORK_FORMAT_DATA24BIT_WIDTH24BIT_STERO:
        default:
        {
            return -1;
        }
    }

    return 0;
}

static int bl602_dac_set_samplerate(audio_framework_device_t *device, AUDIO_FRAMEWORK_SAMPLERATE_T samplerate)
{
    bl602_dac_device_env_t *p_env = (bl602_dac_device_env_t *)device->env;

    switch (samplerate) {
        case AUDIO_FRAMEWORK_SAMPLERATE_8K:
        {
            p_env->samplerate = AUDIO_FRAMEWORK_SAMPLERATE_8K;
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_16K:
        {
            p_env->samplerate = AUDIO_FRAMEWORK_SAMPLERATE_16K;
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_32K:
        {
            p_env->samplerate = AUDIO_FRAMEWORK_SAMPLERATE_32K;
        }
        break;
        default:
        {
            return -2;
        }
    }

    return 0;
}

static int bl602_dac_set_rxcallback(audio_framework_device_t *device, audio_framework_callback_rx cb_rx)
{
    ((bl602_dac_device_env_t*)(device->env))->cb_rx = cb_rx;

    return 0;
}

static int bl602_dac_set_txcallback(audio_framework_device_t *device, audio_framework_callback_tx cb_tx)
{
    ((bl602_dac_device_env_t*)(device->env))->cb_tx = cb_tx;

    return 0;
}

static int bl602_dac_set_buffersize_rx(audio_framework_device_t *device, int size, int *size_result)
{
    size = size > AUDIO_BUFFER_SIZE_RX_MAX ? AUDIO_BUFFER_SIZE_RX_MAX : size;
    size = size < AUDIO_BUFFER_SIZE_RX_MIN ? AUDIO_BUFFER_SIZE_RX_MIN : size;
    ((bl602_dac_device_env_t*)(device->env))->size_buffer_rx = size;

    if (size_result) {
        *size_result = size;
    }

    return 0;
}

static int bl602_dac_set_buffersize_tx(audio_framework_device_t *device, int size, int *size_result)
{
    size = size > AUDIO_BUFFER_SIZE_TX_MAX ? AUDIO_BUFFER_SIZE_TX_MAX : size;
    size = size < AUDIO_BUFFER_SIZE_TX_MIN ? AUDIO_BUFFER_SIZE_TX_MIN : size;
    ((bl602_dac_device_env_t*)(device->env))->size_buffer_tx = size;

    if (size_result) {
        *size_result = size;
    }

    return 0;
}

static int bl602_dac_reset_samplerate(audio_framework_device_t *device, AUDIO_FRAMEWORK_SAMPLERATE_T samplerate)
{
    bl602_dac_set_samplerate(device, samplerate);
    bl602_dac_start(device);

    return 0;
}

static int bl602_dac_set_mclk(audio_framework_device_t *device, uint32_t mclk)
{
    return 0;
}

const static audio_framework_device_t __g_bl602_dac_device ATTR_AUDIO_DEVICE_TABLE = {
    .name = "bl602_dac",
    .init = bl602_dac_init,
    .config = bl602_dac_config,
    .start = bl602_dac_start,
    .stop = bl602_dac_stop,
    .pause = bl602_dac_pause,
    .unint = bl602_dac_unint,
    .set_format = bl602_dac_set_format,
    .set_samplerate = bl602_dac_set_samplerate,
    .set_rxcallback = bl602_dac_set_rxcallback,
    .set_txcallback = bl602_dac_set_txcallback,
    .set_buffersize_rx = bl602_dac_set_buffersize_rx,
    .set_buffersize_tx = bl602_dac_set_buffersize_tx,
    .reset_samplerate = bl602_dac_reset_samplerate,
    .set_mclk = bl602_dac_set_mclk,
    .env = &env,
};

int bl602_dac_device_fake(void)
{
    return 0;
}
