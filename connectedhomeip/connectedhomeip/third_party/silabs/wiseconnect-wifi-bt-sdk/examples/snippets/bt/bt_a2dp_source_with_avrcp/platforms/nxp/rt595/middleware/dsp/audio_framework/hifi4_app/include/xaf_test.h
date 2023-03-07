/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#define XAF_SHMEM_STRUCT_SIZE                12288
#define XAF_SIZEOFPROXY                     2272
#define XAF_SIZEOFHANDLE                    44

#include "xtensa/xos_errors.h"

#define XA_NUM_XOS_ERRS    32
#define XA_NUM_API_ERRS    5

typedef struct _xos_error_map_t{
#if 0
    xos_err_t    err
#else
    int         err;  /* Fix for RG 2017.8 tools' build problem due to renaming of <typedef enum xos_err_t{}xos_err_t> to <enum{}> in xos_errors.h */
#endif
    char        *perr;
}_XOS_ERR_MAP;

typedef struct _api_err_t{
    int     err;
    char    *perr;
}_XA_API_ERR_MAP;

typedef const char             *xf_id_t;

/* ...check the API call succeeds */
#define TST_CHK_API(cond, func_name)                                              \
({                                                                                \
    int __ret;                                                                    \
                                                                                  \
    if ((__ret = (int)(cond)) < 0)                                                \
    {                                                                             \
        extern _XOS_ERR_MAP error_map_table_xos[]; \
        extern _XA_API_ERR_MAP error_map_table_api[]; \
        int i;\
        for(i=0; i<XA_NUM_XOS_ERRS; i++){\
            if(error_map_table_xos[i].err == __ret){\
                FIO_PRINTF(stderr,"%s failed, Error code : %d (%s at %s:%d)\n\n", func_name, __ret, error_map_table_xos[i].perr, __FILE__, __LINE__);\
                xaf_adev_close(p_adev, XAF_ADEV_FORCE_CLOSE);                     \
                return __ret;                                                     \
            }\
        }\
        for(i=0; i<XA_NUM_API_ERRS; i++){\
            if(error_map_table_api[i].err == __ret){\
                FIO_PRINTF(stderr,"%s failed, Error code : %d (%s at %s:%d)\n\n", func_name, __ret, error_map_table_api[i].perr, __FILE__, __LINE__);\
                xaf_adev_close(p_adev, XAF_ADEV_FORCE_CLOSE);                     \
                return __ret;                                                     \
            }\
        }\
        FIO_PRINTF(stderr,"%s failed, Error code : %d\n\n", func_name, __ret);\
        xaf_adev_close(p_adev, XAF_ADEV_FORCE_CLOSE);                             \
        return __ret;                                                             \
    }                                                                             \
    __ret;                                                                        \
})

/* ...check null pointer */
#define TST_CHK_PTR(ptr, func_name)                                                    \
({                                                                                     \
    int __ret;                                                                         \
                                                                                       \
    if ((__ret = (int)(ptr)) == 0)                                                     \
    {                                                                                  \
        FIO_PRINTF(stderr,"%s failed, Null pointer error : %d\n\n", func_name, __ret);    \
        return XAF_PTR_ERROR;                                                          \
    }                                                                                  \
    __ret;                                                                             \
})
