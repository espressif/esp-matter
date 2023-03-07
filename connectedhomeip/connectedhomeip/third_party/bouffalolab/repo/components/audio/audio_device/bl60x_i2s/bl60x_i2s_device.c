#include <stdint.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <portable.h>
#include <audio_framework.h>

#include <bl_i2s.h>

//TODO auto allocate DMA channel

#define AUDIO_BUFFER_SIZE_TX_MAX    (4096 * 4)
#define AUDIO_BUFFER_SIZE_RX_MAX    (4096 * 4)

#define AUDIO_BUFFER_SIZE_TX_MIN    (4096 * 1)
#define AUDIO_BUFFER_SIZE_RX_MIN    (4096 * 1)

#define WIDTH_DEFINE_8BIT           (0)
#define WIDTH_DEFINE_16BIT          (1)
#define WIDTH_DEFINE_24BIT          (2)
#define WIDTH_DEFINE_32BIT          (3)

typedef struct bl60x_i2s_device_env
{
    AUDIO_FRAMEWORK_SAMPLERATE_T samplerate;
    AUDIO_FRAMEWORK_FORMAT_T format;
    uint32_t mclk;

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
} bl60x_i2s_device_env_t;

static bl60x_i2s_device_env_t env;

static int bl60x_i2s_init(audio_framework_device_t *device)
{
    /*Init DMA LLI structure*/
    return 0;
}

static int bl60x_i2s_config(audio_framework_device_t *device)
{
    int size, ret = -1;
    void **size_ptr;

    /*Init I2S hardware env*/
    if ((ret = bl_i2s_init(1, 1, pvPortMallocDMA, vPortFreeDMA))) {
        goto failed;
    }

    /*config tx buffer*/
    size = ((bl60x_i2s_device_env_t*)(device->env))->size_buffer_tx;
    size_ptr = &(((bl60x_i2s_device_env_t*)(device->env))->buffer_ptr_tx);
    if ((ret = bl_i2s_tx_buffer_config(size, size_ptr, pvPortMallocDMA))) {
        goto failed;
    }
    /*config rx buffer*/
    size = ((bl60x_i2s_device_env_t*)(device->env))->size_buffer_rx;
    size_ptr = &(((bl60x_i2s_device_env_t*)(device->env))->buffer_ptr_rx);
    if ((ret = bl_i2s_rx_buffer_config(size, size_ptr, pvPortMallocDMA))) {
        goto failed;
    }

    /*samplerate config*/
    switch (((bl60x_i2s_device_env_t*)(device->env))->samplerate) {
        case AUDIO_FRAMEWORK_SAMPLERATE_8K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_8K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_11P025K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_11P025K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_12K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_12K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_16K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_16K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_22P05K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_22P05K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_24K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_24K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_32K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_32K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_44P1K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_44P1K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_48K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_48K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_96K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_96K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_192K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_192K);
        }
        break;
        default:
        {
            ret = -2;
            goto failed;
        }
    }

    switch (((bl60x_i2s_device_env_t*)(device->env))->format) {
        case AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH16BIT_MONO:
        {
            bl_i2s_configure(BL_AUDIO_FORMAT_DATA16BIT_WIDTH16BIT_MONO);
        }
        break;
        case AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH16BIT_STERO:
        {
            bl_i2s_configure(BL_AUDIO_FORMAT_DATA16BIT_WIDTH16BIT_STERO);
        }
        break;
        case AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH32BIT_MONO:
        {
            bl_i2s_configure(BL_AUDIO_FORMAT_DATA16BIT_WIDTH32BIT_MONO);
        }
        break;
        case AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH32BIT_STERO:
        {
            bl_i2s_configure(BL_AUDIO_FORMAT_DATA16BIT_WIDTH32BIT_STERO);
        }
        break;
        case AUDIO_FRAMEWORK_FORMAT_DATA24BIT_WIDTH32BIT_MONO:
        {
            bl_i2s_configure(BL_AUDIO_FORMAT_DATA24BIT_WIDTH32BIT_MONO);
        }
        break;
        case AUDIO_FRAMEWORK_FORMAT_DATA24BIT_WIDTH32BIT_STERO:
        {
            bl_i2s_configure(BL_AUDIO_FORMAT_DATA24BIT_WIDTH32BIT_STERO);
        }
        break;
        case AUDIO_FRAMEWORK_FORMAT_DATA32BIT_WIDTH32BIT_MONO:
        {
            bl_i2s_configure(BL_AUDIO_FORMAT_DATA32BIT_WIDTH32BIT_MONO);
        }
        break;
        case AUDIO_FRAMEWORK_FORMAT_DATA32BIT_WIDTH32BIT_STERO:
        {
            bl_i2s_configure(BL_AUDIO_FORMAT_DATA32BIT_WIDTH32BIT_STERO);
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

failed:
    /*release lowlevel resource*/
    bl_i2s_unint(vPortFree);

    return ret;
}

static int cb_tx(void *usrdata, uint8_t *audiodata, int len, int is_overflow)
{
    //FIXME critical section protect
    audio_framework_device_t *device;

    device = (audio_framework_device_t*)usrdata;
    if (((bl60x_i2s_device_env_t*)(device->env))->cb_tx) {
        ((bl60x_i2s_device_env_t*)(device->env))->cb_tx(device, usrdata, audiodata, len, is_overflow);
    }

    return 0;
}

static int cb_rx(void *usrdata, uint8_t *audiodata, int len, int is_overflow)
{
    //FIXME critical section protect

    audio_framework_device_t *device;

    device = (audio_framework_device_t*)usrdata;
    if (((bl60x_i2s_device_env_t*)(device->env))->cb_rx) {
        ((bl60x_i2s_device_env_t*)(device->env))->cb_rx(device, usrdata, audiodata, len, is_overflow);
    }

    return 0;
}

static int bl60x_i2s_start(audio_framework_device_t *device)
{
    if (((bl60x_i2s_device_env_t*)(device->env))->cb_tx) {
        bl_i2s_tx_ready_config(cb_tx, device);
    }
    if (((bl60x_i2s_device_env_t*)(device->env))->cb_rx) {
        bl_i2s_rx_ready_config(cb_rx, device);
    }
    bl_i2s_start();
    return 0;
}

static int bl60x_i2s_stop(audio_framework_device_t *device)
{
    bl_i2s_tx_ready_config(NULL, device);
    bl_i2s_rx_ready_config(NULL, device);
    bl_i2s_stop();

    return 0;
}

static int bl60x_i2s_pause(audio_framework_device_t *device)
{
    return 0;
}

static int bl60x_i2s_unint(audio_framework_device_t *device)
{
    return 0;
}

static int bl60x_i2s_set_format(audio_framework_device_t *device, AUDIO_FRAMEWORK_FORMAT_T format)
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
            ((bl60x_i2s_device_env_t*)(device->env))->format = format;
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

static int bl60x_i2s_set_samplerate(audio_framework_device_t *device, AUDIO_FRAMEWORK_SAMPLERATE_T samplerate)
{
    switch (samplerate) {
        case AUDIO_FRAMEWORK_SAMPLERATE_8K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_8K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_11P025K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_11P025K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_12K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_12K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_16K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_16K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_22P05K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_22P05K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_24K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_24K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_32K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_32K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_44P1K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_44P1K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_48K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_48K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_96K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_96K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_192K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_192K);
        }
        break;
        default:
        {
            return -2;
        }
    }

    return 0;
}

static int bl60x_i2s_set_rxcallback(audio_framework_device_t *device, audio_framework_callback_rx cb_rx)
{
    ((bl60x_i2s_device_env_t*)(device->env))->cb_rx = cb_rx;

    return 0;
}

static int bl60x_i2s_set_txcallback(audio_framework_device_t *device, audio_framework_callback_tx cb_tx)
{
    ((bl60x_i2s_device_env_t*)(device->env))->cb_tx = cb_tx;

    return 0;
}

static int bl60x_i2s_set_buffersize_rx(audio_framework_device_t *device, int size, int *size_result)
{
    size = size > AUDIO_BUFFER_SIZE_RX_MAX ? AUDIO_BUFFER_SIZE_RX_MAX : size;
    size = size < AUDIO_BUFFER_SIZE_RX_MIN ? AUDIO_BUFFER_SIZE_RX_MIN : size;
    ((bl60x_i2s_device_env_t*)(device->env))->size_buffer_rx = size;
    *size_result = size;

    return 0;
}

static int bl60x_i2s_set_buffersize_tx(audio_framework_device_t *device, int size, int *size_result)
{
    size = size > AUDIO_BUFFER_SIZE_TX_MAX ? AUDIO_BUFFER_SIZE_TX_MAX : size;
    size = size < AUDIO_BUFFER_SIZE_TX_MIN ? AUDIO_BUFFER_SIZE_TX_MIN : size;
    ((bl60x_i2s_device_env_t*)(device->env))->size_buffer_tx = size;
    *size_result = size;

    return 0;
}

static int bl60x_i2s_reset_samplerate(audio_framework_device_t *device, AUDIO_FRAMEWORK_SAMPLERATE_T samplerate)
{
    switch (samplerate) {
        case AUDIO_FRAMEWORK_SAMPLERATE_8K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_8K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_11P025K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_11P025K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_12K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_12K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_16K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_16K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_22P05K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_22P05K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_24K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_24K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_32K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_32K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_44P1K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_44P1K);
        }
        break;
        case AUDIO_FRAMEWORK_SAMPLERATE_48K:
        {
            bl_i2s_sample_rate_config(BL_I2S_SAMPLE_RATE_48K);
        }
        break;
        default:
        {
            return -2;
        }
    }
    bl_i2s_start();

    return 0;
}

static int bl60x_i2s_set_mclk(audio_framework_device_t *device, uint32_t mclk)
{
    bl_i2s_set_mclk(mclk);

    return 0;
}

static audio_framework_device_t bl60x_i2s_device ATTR_AUDIO_DEVICE_TABLE = {
    .name = "bl60x_i2s",
    .init = bl60x_i2s_init,
    .config = bl60x_i2s_config,
    .start = bl60x_i2s_start,
    .stop = bl60x_i2s_stop,
    .pause = bl60x_i2s_pause,
    .unint = bl60x_i2s_unint,
    .set_format = bl60x_i2s_set_format,
    .set_samplerate = bl60x_i2s_set_samplerate,
    .set_rxcallback = bl60x_i2s_set_rxcallback,
    .set_txcallback = bl60x_i2s_set_txcallback,
    .set_buffersize_rx = bl60x_i2s_set_buffersize_rx,
    .set_buffersize_tx = bl60x_i2s_set_buffersize_tx,
    .reset_samplerate = bl60x_i2s_reset_samplerate,
    .set_mclk = bl60x_i2s_set_mclk,
    .env = &env,
};

int bl60x_i2s_device_fake(void)
{
    return 0;
}
