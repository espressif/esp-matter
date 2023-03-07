#include <stdio.h>
#include <string.h>
#include "product.h"

#define USE_86_243
//#define USE_86_270

//#define USE_82_203
#define USE_82_048


//TODO: update these product info
#define product_model           "OPENALINK_LIVING_LIGHT_SDS_TEST"
#define product_key             "1L6ueddLqnRORAQ2sGOL"
#define product_secret          "qfxCLoc1yXEk9aLdx5F74tl1pdxl0W0q7eYOvvuo"
#define product_debug_key       "dpZZEpm9eBfqzK7yVeLq"
#define product_debug_secret    "THnfRRsU5vu6g6m9X6uFyAjUWflgZ0iyGjdEneKm"
#if (PRODUCT_VERSION == 7682)
    #ifdef USE_82_203
    #define device_key              "2ERE5iMsadW6SiBoMEmE"
    #define device_secret           "cPuv1lyYvimouVEjvHSyh2i2cxGjSIj3"
    #endif
    
    #ifdef USE_82_048
    #define device_key              "imSuUhRBLkTRzh5nMG9T"
    #define device_secret           "k75UkmRvhESATr3nfJIJkc10SU2EFpy3"
    #endif
#elif (PRODUCT_VERSION == 7686)
    #ifdef USE_86_243
    #define device_key              "qTBboRPrrAePOriR5WNL"
    #define device_secret           "6yquiZKqnqJwIzbVvvLE5L5bNwsHd7qK"
    #endif
    
    #ifdef USE_86_270
    #define device_key              "8blEpMSG1cSkGYGotQGA"
    #define device_secret           "3oxMt7GB284ddRJcwKNVsatoNZ60LYmv"
    #endif
#elif (PRODUCT_VERSION == 7697)
    #define device_key              "Fd3u5rtvfXK350E2nWl7"
    #define device_secret           "gFjRELQXErTv4yFMQBnpIxs8AiWxdoLl"
#else
    #define device_key              "SlJpPT0XlvbExJaoB4Qj"
    #define device_secret           "p3hYq25UjgucJ8UlLDBtScRaT53DarUP"
#endif

char *product_get_name(char name_str[PRODUCT_NAME_LEN])
{
	return strncpy(name_str, "alink_product", PRODUCT_NAME_LEN);
}

char *product_get_version(char ver_str[PRODUCT_VERSION_LEN])
{
	return strncpy(ver_str, "1.0.0", PRODUCT_VERSION_LEN);
}

char *product_get_model(char model_str[PRODUCT_MODEL_LEN])
{
	return strncpy(model_str, product_model, PRODUCT_MODEL_LEN);
}

char *product_get_key(char key_str[PRODUCT_KEY_LEN])
{
	return strncpy(key_str, product_key, PRODUCT_KEY_LEN);
}

char *product_get_device_key(char key_str[DEVICE_KEY_LEN])
{
	return strncpy(key_str, device_key, DEVICE_KEY_LEN);
}

char *product_get_device_secret(char secret_str[DEVICE_SECRET_LEN])
{
	return strncpy(secret_str, device_secret, DEVICE_SECRET_LEN);
}

char *product_get_secret(char secret_str[PRODUCT_SECRET_LEN])
{
	return strncpy(secret_str, product_secret, PRODUCT_SECRET_LEN);
}

char *product_get_debug_key(char key_str[PRODUCT_KEY_LEN])
{
	return strncpy(key_str, product_debug_key, PRODUCT_KEY_LEN);
}

char *product_get_debug_secret(char secret_str[PRODUCT_SECRET_LEN])
{
	return strncpy(secret_str, product_debug_secret, PRODUCT_SECRET_LEN);
}

char *product_get_sn(char sn_str[PRODUCT_SN_LEN])
{
#if (PRODUCT_VERSION == 7682)
    #ifdef USE_82_203
	return strncpy(sn_str, "7024350656861023", PRODUCT_SN_LEN);
    #endif
#ifdef USE_82_048
	return strncpy(sn_str, "7024350656861048", PRODUCT_SN_LEN);
#endif

#elif (PRODUCT_VERSION == 7686)
    #ifdef USE_86_243
	return strncpy(sn_str, "7024350656861243", PRODUCT_SN_LEN);
    #endif
    
    #ifdef USE_86_270
	return strncpy(sn_str, "7024350656861270", PRODUCT_SN_LEN);
    #endif
#elif (PRODUCT_VERSION == 7697)
    return strncpy(sn_str, "7024350656861097", PRODUCT_SN_LEN);
#else
	return strncpy(sn_str, "7024350656861309", PRODUCT_SN_LEN);
#endif
}

