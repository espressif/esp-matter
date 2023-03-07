
#include <stdio.h>

#if defined(EF_USING_ENV) && !defined(EF_ENV_USING_LEGACY_MODE) && defined(EF_USING_ENVBUF)

#ifndef EF_USING_ENVBUG_SIZE
#error "Please configure flash env_buf size(in ef_cfg.h)"
#endif

#ifndef EF_USING_ENVBUG_GRAN
#error "Please configure flash env_buf gran size(in ef_cfg.h)"
#endif

#define ENVBUF_ADDR(pt)    (ENV_AREA_SIZE + pt * EF_USING_ENVBUG_GRAN)
#define ENVBUF_PT_MAX      (EF_USING_ENVBUG_SIZE/EF_USING_ENVBUG_GRAN)

#define EF_KEY_MAXSIZE      (32)
#define EF_VALUE_MAXSIZE    (200)

int ef_envbuf_set(uint32_t pt, const uint32_t *buf, size_t size)
{
    uint32_t addr = ENVBUF_ADDR(pt);

    if (pt > ENVBUF_PT_MAX) {
        return -1;
    }

    ef_port_erase(addr, size);
    ef_port_write(addr, buf, size);
    return 0;
}

int ef_envbuf_get(uint32_t pt, uint32_t *buf, size_t size)
{
    uint32_t addr = ENVBUF_ADDR(pt);

    if (pt > ENVBUF_PT_MAX) {
        return -1;
    }

    ef_port_read(addr, buf, size);
    return 0;
}

#if 0
int32_t ef_envbuf_save_string(uint8_t*  key, uint8_t*  value)
{
    char *tmpbuf = NULL;

    if ((NULL == key) || (NULL == value)) {
        log_error("arg error.\r\n");
        return -1;
    }

    if (strlen(key) < EF_KEY_MAXSIZE) {
        log_error("key too long.\r\n")
    }

    if (strlen(value) < EF_VALUE_MAXSIZE) {
        ef_set_env((const char *)key, (const char *)value);
    }

    /* search key */
    tmpbuf = (char *)ef_get_env((const char *)key);
    if *tmp

    return 0;
}
#endif

#endif

