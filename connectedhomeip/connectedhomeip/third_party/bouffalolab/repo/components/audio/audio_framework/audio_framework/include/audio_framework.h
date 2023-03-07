#ifndef __AUDIO_FRAMEWORK_H__
#define __AUDIO_FRAMEWORK_H__
#include <stdint.h>

/*TODO dispatch audio event to worker thread*/
typedef struct audio_framework_device audio_framework_device_t;

/*Build an audio container*/
typedef int (*audio_framework_callback_rx)(audio_framework_device_t *device, void *uerdata, uint8_t *audiodata, int len, int is_overflow);
typedef int (*audio_framework_callback_tx)(audio_framework_device_t *device, void *uerdata, uint8_t *audiodata, int len, int is_underrun);
int audio_framework_factory_build(const char *name, audio_framework_device_t **device);
int audio_framework_factory_destory(audio_framework_device_t *device);

typedef enum audio_framework_format {
    /*16bit*/
    AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH16BIT_MONO,
    AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH24BIT_MONO,
    AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH32BIT_MONO,
    AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH16BIT_STERO,
    AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH24BIT_STERO,
    AUDIO_FRAMEWORK_FORMAT_DATA16BIT_WIDTH32BIT_STERO,

    /*24bit*/
    AUDIO_FRAMEWORK_FORMAT_DATA24BIT_WIDTH24BIT_MONO,
    AUDIO_FRAMEWORK_FORMAT_DATA24BIT_WIDTH32BIT_MONO,
    AUDIO_FRAMEWORK_FORMAT_DATA24BIT_WIDTH24BIT_STERO,
    AUDIO_FRAMEWORK_FORMAT_DATA24BIT_WIDTH32BIT_STERO,

    /*32bit*/
    AUDIO_FRAMEWORK_FORMAT_DATA32BIT_WIDTH32BIT_MONO,
    AUDIO_FRAMEWORK_FORMAT_DATA32BIT_WIDTH32BIT_STERO,
} AUDIO_FRAMEWORK_FORMAT_T;

typedef enum audio_framework_samplerate {
    AUDIO_FRAMEWORK_SAMPLERATE_8K,
    AUDIO_FRAMEWORK_SAMPLERATE_11P025K,
    AUDIO_FRAMEWORK_SAMPLERATE_12K,
    AUDIO_FRAMEWORK_SAMPLERATE_16K,
    AUDIO_FRAMEWORK_SAMPLERATE_22P05K,
    AUDIO_FRAMEWORK_SAMPLERATE_24K,
    AUDIO_FRAMEWORK_SAMPLERATE_32K,
    AUDIO_FRAMEWORK_SAMPLERATE_44P1K,
    AUDIO_FRAMEWORK_SAMPLERATE_48K,
    AUDIO_FRAMEWORK_SAMPLERATE_96K,
    AUDIO_FRAMEWORK_SAMPLERATE_192K,
} AUDIO_FRAMEWORK_SAMPLERATE_T;

struct audio_framework_device {
    char name[16];
    int (*init)(audio_framework_device_t *device);
    int (*config)(audio_framework_device_t *device);
    int (*start)(audio_framework_device_t *device);
    int (*stop)(audio_framework_device_t *device);
    int (*pause)(audio_framework_device_t *device);
    int (*unint)(audio_framework_device_t *device);

    /*configure audio format*/
    int (*set_format)(audio_framework_device_t *device, AUDIO_FRAMEWORK_FORMAT_T format);
    /*configure audio sample rate*/
    int (*set_samplerate)(audio_framework_device_t *device, AUDIO_FRAMEWORK_SAMPLERATE_T samplerate);
    /*configure TX callback*/
    int (*set_rxcallback)(audio_framework_device_t *device, audio_framework_callback_rx cb_rx);
    /*configure RX callback*/
    int (*set_txcallback)(audio_framework_device_t *device, audio_framework_callback_tx cb_tx);
    /*configure rx buffer*/
    int (*set_buffersize_rx)(audio_framework_device_t *device, int size, int *size_result);
    /*configure tx buffer*/
    int (*set_buffersize_tx)(audio_framework_device_t *device, int size, int *size_result);

    /*reset audio sample rate*/
    int (*reset_samplerate)(audio_framework_device_t *device, AUDIO_FRAMEWORK_SAMPLERATE_T samplerate);
    /*set audio mclk */
    int (*set_mclk)(audio_framework_device_t *device, uint32_t mclk);

    /*Private pointer holder for private used data in device driver*/
    void *env;
};
#define ATTR_AUDIO_DEVICE_TABLE __attribute__((section(".framework.audio_device")))  __attribute__((used))
#endif
