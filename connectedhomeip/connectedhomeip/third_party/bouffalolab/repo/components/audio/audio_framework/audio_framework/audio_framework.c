#include <string.h>
#include <audio_framework.h>

extern audio_framework_device_t _ld_symbol_rom_framework_audio_device_start;
extern audio_framework_device_t _ld_symbol_rom_framework_audio_device_end;
int fake_call = 0;

int audio_framework_factory_build(const char *name, audio_framework_device_t **device)
{
    audio_framework_device_t *start, *end;

    start = &_ld_symbol_rom_framework_audio_device_start;
    end = &_ld_symbol_rom_framework_audio_device_end;
    while (start < end) {
        if (0 == strcmp(name, start->name)) {
            *device = start;
            return 0;
        }
        start++;
    }


    if (fake_call) {
        //FIXME very ugly hack for linker issue?
//int bl60x_i2s_device_fake(void);
//        bl60x_i2s_device_fake();
    }

    return -1;
}

int audio_framework_factory_destory(audio_framework_device_t *device)
{
    return -1;
}

