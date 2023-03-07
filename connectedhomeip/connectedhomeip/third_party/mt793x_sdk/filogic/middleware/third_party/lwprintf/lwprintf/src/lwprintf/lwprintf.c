/**
 * \file            lwprintf.c
 * \brief           Lightweight stdio manager
 */

/*
 * Copyright (c) 2020 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwPRINTF - Lightweight stdio manager library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.0.3
 */
#include <limits.h>
#include <float.h>
#include <stdint.h>
#include "lwprintf/lwprintf.h"

#if LWPRINTF_CFG_OS
#include "system/lwprintf_sys.h"
#endif /* LWPRINTF_CFG_OS */

/* Static checks */
#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING && !LWPRINTF_CFG_SUPPORT_TYPE_FLOAT
#error "Cannot use engineering type without float!"
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING && !LWPRINTF_CFG_SUPPORT_TYPE_FLOAT */

#define CHARISNUM(x)                    ((x) >= '0' && (x) <= '9')
#define CHARTONUM(x)                    ((x) - '0')
#define IS_PRINT_MODE(p)                ((p)->out_fn == prv_out_fn_print)

/* Define custom types */
#if LWPRINTF_CFG_SUPPORT_LONG_LONG
typedef long long int float_long_t;
#define FLOAT_MAX_B_ENG                 (1E18)
#else
typedef long int float_long_t;
#define FLOAT_MAX_B_ENG                 (1E09)
#endif /* LWPRINTF_CFG_SUPPORT_LONG_LONG */

/**
 * \brief           Float number splitted by parts
 */
typedef struct {
    float_long_t integer_part;                  /*!< Integer type of double number */
    double decimal_part_dbl;                    /*!< Decimal part of double number multiplied by 10^precision */
    float_long_t decimal_part;                  /*!< Decimal part of double number in integer format */
    double diff;                                /*!< Difference between decimal parts (double - int) */

    short digits_cnt_integer_part;              /*!< Number of digits for integer part */
    short digits_cnt_decimal_part;              /*!< Number of digits for decimal part */
    short digits_cnt_decimal_part_useful;       /*!< Number of useful digits to print */
} float_num_t;

#if LWPRINTF_CFG_SUPPORT_TYPE_FLOAT
/* Powers of 10 from beginning up to precision level */
static const float_long_t
powers_of_10[] = { 1E00, 1E01, 1E02, 1E03, 1E04, 1E05, 1E06, 1E07, 1E08, 1E09,
#if LWPRINTF_CFG_SUPPORT_LONG_LONG
                    1E10, 1E11, 1E12, 1E13, 1E14, 1E15, 1E16, 1E17, 1E18
#endif /* LWPRINTF_CFG_SUPPORT_LONG_LONG */
};
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_FLOAT */

/**
 * \brief           Outputs any integer type to stream
 * Implemented as big macro since `d`, `digit` and `num` are of different types vs int size
 */
#define OUTPUT_ANY_INT_TYPE(ttype, num) {                               \
    ttype d, digit;                                                     \
    uint8_t digits_cnt;                                                 \
    char c;                                                             \
                                                                        \
    /* Check if number is zero */                                       \
    p->m.flags.is_num_zero = (num) == 0;                                \
    if ((num) == 0) {                                                   \
        prv_out_str_before(p, 1);                                       \
        p->out_fn(p, '0');                                              \
        prv_out_str_after(p, 1);                                        \
    } else {                                                            \
        /* Start with digits length */                                  \
        for (digits_cnt = 0, d = (num); d > 0; ++digits_cnt, d /= p->m.base) {} \
        for (d = 1; ((num) / d) >= p->m.base; d *= p->m.base) {}        \
                                                                        \
        prv_out_str_before(p, digits_cnt);                              \
        for (; d > 0; ) {                                               \
            digit = (num) / d;                                          \
            num = (num) % d;                                            \
            d = d / p->m.base;                                          \
            c = (char)digit + (char)(digit >= 10 ? ((p->m.flags.uc ? 'A' : 'a') - 10) : '0');   \
            p->out_fn(p, c);                                            \
        }                                                               \
        prv_out_str_after(p, digits_cnt);                               \
    }                                                                   \
}

/**
 * \brief           Check for negative input number before outputting signed integers
 */
#define SIGNED_CHECK_NEGATIVE(pp, nnum) {   \
    if ((nnum) < 0) {                       \
        (pp)->m.flags.is_negative = 1;      \
        nnum = -(nnum);                     \
    }                                       \
}

/**
 * \brief           Forward declaration
 */
struct lwprintf_int;

/**
 * \brief           Private output function declaration
 * \param[in]       lwi: Internal working structure
 * \param[in]       c: Character to print
 */
typedef int (*prv_output_fn)(struct lwprintf_int* p, const char c);

/**
 * \brief           Internal structure
 */
typedef struct lwprintf_int {
    lwprintf_t* lw;                             /*!< Instance handle */
    const char* fmt;                            /*!< Format string */
    char* const buff;                           /*!< Pointer to buffer when not using print option */
    const size_t buff_size;                     /*!< Buffer size of input buffer (when used) */
    int n;                                      /*!< Full length of formatted text */
    prv_output_fn out_fn;                       /*!< Output internal function */
    uint8_t is_print_cancelled;                 /*!< Status if print should be cancelled */

    /* This must all be reset every time new % is detected */
    struct {
        struct {
            uint8_t left_align : 1;             /*!< Minus for left alignment */
            uint8_t plus : 1;                   /*!< Prepend + for positive numbers on the output */
            uint8_t space : 1;                  /*!< Prepend spaces. Not used with plus modifier */
            uint8_t zero : 1;                   /*!< Zero pad flag detection, add zeros if number length is less than width modifier */
            uint8_t thousands : 1;              /*!< Thousands has grouping applied */
            uint8_t alt : 1;                    /*!< Alternate form with hash */
            uint8_t precision : 1;              /*!< Precision flag has been used */

            /* Length modified flags */
            uint8_t longlong : 2;               /*!< Flag indicatin long-long number, used with 'l' (1) or 'll' (2) mode */
            uint8_t char_short : 2;             /*!< Used for 'h' (1 = short) or 'hh' (2 = char) length modifier */
            uint8_t sz_t : 1;                   /*!< Status for size_t length integer type */
            uint8_t umax_t : 1;                 /*!< Status for uintmax_z length integer type */

            uint8_t uc : 1;                     /*!< Uppercase flag */
            uint8_t is_negative : 1;            /*!< Status if number is negative */
            uint8_t is_num_zero : 1;            /*!< Status if input number is zero */
        } flags;                                /*!< List of flags */
        int precision;                          /*!< Selected precision */
        int width;                              /*!< Text width indicator */
        uint8_t base;                           /*!< Base for number format output */
        char type;                              /*!< Format type */
    } m;                                        /*!< Block that is reset on every start of format */
} lwprintf_int_t;

/**
 * \brief           Get LwPRINTF instance based on user input
 * \param[in]       p: LwPRINTF instance.
 *                      Set to `NULL` for default instance
 */
#define LWPRINTF_GET_LW(p)              ((p) != NULL ? (p) : (&lwprintf_default))

/**
 * \brief           LwPRINTF default structure used by application
 */
static lwprintf_t
lwprintf_default;

#if 0
/**
 * \brief           Rotate string of the input buffer in place
 * It rotates string from "abcdef" to "fedcba".
 *
 * \param[in,out]   str: Input and output string to be rotated
 * \param[in]       len: String length, optional parameter if
 *                      length is known in advance. Use `0` if not used
 * \return          `1` on success, `0` otherwise
 */
static int
prv_rotate_string(char* str, size_t len) {
    /* Get length if 0 */
    if (len == 0) {
        len = strlen(str);
    }

    /* Rotate string */
    for (size_t i = 0; i < len / 2; ++i) {
        char t = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = t;
    }

    return 1;
}
#endif /* 0 */

/**
 * \brief           Output function to print data
 * \param[in]       p: LwPRINTF internal instance
 * \param[in]       c: Character to print
 * \return          `1` on success, `0` otherwise
 */
static int
prv_out_fn_print(lwprintf_int_t* p, const char c) {
    if (p->is_print_cancelled) {
        return 0;
    }
    if (!p->lw->out_fn(c, p->lw)) {             /*!< Send character to output */
        p->is_print_cancelled = 1;
    }
    if (c != '\0' && !p->is_print_cancelled) {
        ++p->n;
    }
    return 1;
}

/**
 * \brief           Output function to generate buffer data
 * \param[in]       p: LwPRINTF internal instance
 * \param[in]       c: Character to write
 * \return          `1` on success, `0` otherwise
 */
static int
prv_out_fn_write_buff(lwprintf_int_t* p, const char c) {
    if (c != '\0' && p->n < (p->buff_size - 1)) {
        p->buff[p->n++] = c;
        return 1;
    }
    return 0;
}

/**
 * \brief           Output function to drop data
 * \param[in]       p: LwPRINTF internal instance
 * \param[in]       c: Character to write
 * \return          `1` on success, `0` otherwise
 */
static int
prv_out_fn_write_null(lwprintf_int_t* p, const char c) {
    p->n++;
    return 1;
}

/**
 * \brief           Parse number from input string
 * \param[in,out]   format: Input text to process
 * \return          Parsed number
 */
static int
prv_parse_num(const char** format) {
    const char* fmt = *format;
    int n = 0;

    for (; CHARISNUM(*fmt); ++fmt) {
        n = 10 * n + CHARTONUM(*fmt);
    }
    *format = fmt;
    return n;
}

/**
 * \brief           Format data that are printed before actual value
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       buff_size: Expected buffer size of output string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_out_str_before(lwprintf_int_t* p, size_t buff_size) {
    /* Check for width */
    if (p->m.width > 0
        /* If number is negative, add negative sign or if positive and has plus sign forced */
        && (p->m.flags.is_negative || p->m.flags.plus)) {
        --p->m.width;
    }

    /* Check for alternate mode */
    if (p->m.flags.alt && !p->m.flags.is_num_zero) {
        if (p->m.base == 8) {
            if (p->m.width > 0) {
                --p->m.width;
            }
        } else if (p->m.base == 16 || p->m.base == 2) {
            if (p->m.width >= 2) {
                p->m.width -= 2;
            } else {
                p->m.width = 0;
            }
        }
    }

    /* Add negative sign (or positive in case of + flag or space in case of space flag) before when zeros are used to fill width */
    if (p->m.flags.zero) {
        if (p->m.flags.is_negative) {
            p->out_fn(p, '-');
        } else if (p->m.flags.plus) {
            p->out_fn(p, '+');
        } else if (p->m.flags.space) {
            p->out_fn(p, ' ');
        }
    }

    /* Check for flags output */
    if (p->m.flags.alt && !p->m.flags.is_num_zero) {
        if (p->m.base == 8) {
            p->out_fn(p, '0');
        } else if (p->m.base == 16) {
            p->out_fn(p, '0');
            p->out_fn(p, p->m.flags.uc ? 'X' : 'x');
        } else if (p->m.base == 2) {
            p->out_fn(p, '0');
            p->out_fn(p, p->m.flags.uc ? 'B' : 'b');
        }
    }

    /* Right alignment, spaces or zeros */
    if (!p->m.flags.left_align && p->m.width > 0) {
        for (size_t i = buff_size; !p->m.flags.left_align && i < p->m.width; ++i) {
            p->out_fn(p, p->m.flags.zero ? '0' : ' ');
        }
    }

    /* Add negative sign here when spaces are used for width */
    if (!p->m.flags.zero) {
        if (p->m.flags.is_negative) {
            p->out_fn(p, '-');
        } else if (p->m.flags.plus) {
            p->out_fn(p, '+');
        } else if (p->m.flags.space && buff_size >= p->m.width) {
            p->out_fn(p, ' ');
        }
    }

    return 1;
}

/**
 * \brief           Format data that are printed after actual value
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       buff_size: Expected buffer size of output string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_out_str_after(lwprintf_int_t* p, size_t buff_size) {
    /* Left alignment, but only with spaces */
    if (p->m.flags.left_align) {
        for (size_t i = buff_size; i < p->m.width; ++i) {
            p->out_fn(p, ' ');
        }
    }
    return 1;
}

/**
 * \brief           Output raw string without any formatting
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       buff: Buffer string
 * \param[in]       buff_size: Length of buffer to output
 * \return          `1` on success, `0` otherwise
 */
static int
prv_out_str_raw(lwprintf_int_t* p, const char* buff, size_t buff_size) {
    for (size_t i = 0; i < buff_size; ++i) {
        p->out_fn(p, buff[i]);
    }
    return 1;
}

/**
 * \brief           Output generated string from numbers/digits
 * Paddings before and after are applied at this stage
 *
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       buff: Buffer string
 * \param[in]       buff_size: Length of buffer to output
 * \return          `1` on success, `0` otherwise
 */
static int
prv_out_str(lwprintf_int_t* p, const char* buff, size_t buff_size) {
    /* Output string */
    if (buff_size == 0) {
        buff_size = strlen(buff);
    }
    prv_out_str_before(p, buff_size);           /* Implement pre-format */
    prv_out_str_raw(p, buff, buff_size);        /* Print actual string */
    prv_out_str_after(p, buff_size);            /* Implement post-format */

    return 1;
}

/**
 * \brief           Convert `unsigned int` to string
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       num: Number to convert to string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_unsigned_int_to_str(lwprintf_int_t* p, unsigned int num) {
    OUTPUT_ANY_INT_TYPE(unsigned int, num);
    return 1;
}

/**
 * \brief           Convert `unsigned long` to string
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       num: Number to convert to string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_unsigned_long_int_to_str(lwprintf_int_t* p, unsigned long int num) {
    OUTPUT_ANY_INT_TYPE(unsigned long int, num);
    return 1;
}

#if LWPRINTF_CFG_SUPPORT_LONG_LONG

/**
 * \brief           Convert `unsigned long-long` to string
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       num: Number to convert to string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_unsigned_longlong_int_to_str(lwprintf_int_t* p, unsigned long long int num) {
    OUTPUT_ANY_INT_TYPE(unsigned long long int, num);
    return 1;
}

#endif /* LWPRINTF_CFG_SUPPORT_LONG_LONG */

#if LWPRINTF_CFG_SUPPORT_TYPE_POINTER

/**
 * \brief           Convert `uintptr_t` to string
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       num: Number to convert to string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_uintptr_to_str(lwprintf_int_t* p, uintptr_t num) {
    OUTPUT_ANY_INT_TYPE(uintptr_t, num);
    return 1;
}

#endif /* LWPRINTF_CFG_SUPPORT_TYPE_POINTER */

/**
 * \brief           Convert `size_t` number to string
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       num: Number to convert to string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_sizet_to_str(lwprintf_int_t* p, size_t num) {
    OUTPUT_ANY_INT_TYPE(size_t, num);
    return 1;
}

/**
 * \brief           Convert `uintmax_t` number to string
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       num: Number to convert to string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_umaxt_to_str(lwprintf_int_t* p, uintmax_t num) {
    OUTPUT_ANY_INT_TYPE(uintmax_t, num);
    return 1;
}

/**
 * \brief           Convert signed int to string
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       num: Number to convert to string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_signed_int_to_str(lwprintf_int_t* p, signed int num) {
    SIGNED_CHECK_NEGATIVE(p, num);
    return prv_unsigned_int_to_str(p, num);
}

/**
 * \brief           Convert signed long int to string
 * \param[in,out]   p: LwPRINTF instance
 * \param[in]       num: Number to convert to string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_signed_long_int_to_str(lwprintf_int_t* p, signed long int num) {
    SIGNED_CHECK_NEGATIVE(p, num);
    return prv_unsigned_long_int_to_str(p, num);
}

#if LWPRINTF_CFG_SUPPORT_LONG_LONG

/**
 * \brief           Convert signed long-long int to string
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       num: Number to convert to string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_signed_longlong_int_to_str(lwprintf_int_t* p, signed long long int num) {
    SIGNED_CHECK_NEGATIVE(p, num);
    return prv_unsigned_longlong_int_to_str(p, num);
}

#endif /* LWPRINTF_CFG_SUPPORT_LONG_LONG */

#if LWPRINTF_CFG_SUPPORT_TYPE_FLOAT

/**
 * \brief           Calculate necessary parameters for input number
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       n: Float number instance
 * \param[in]       num: Input number
 * \param[in]       e: Exponent number (to normalize)
 * \param[in]       type: Format type
 */
static void
prv_calculate_dbl_num_data(lwprintf_int_t* p, float_num_t* n, double num, uint8_t e, const char type) {
    memset(n, 0x00, sizeof(*n));

    if (p->m.precision >= LWPRINTF_ARRAYSIZE(powers_of_10)) {
        p->m.precision = LWPRINTF_ARRAYSIZE(powers_of_10) - 1;
    }

    /*
     * Get integer and decimal parts, both in integer formats
     *
     * As an example, with input number of 12.345678 and precision digits set as 4, then result is the following:
     *
     * integer_part = 12            -> Actual integer part of the double number
     * decimal_part_dbl = 3456.78   -> Decimal part multiplied by 10^precision, keeping it in double format
     * decimal_part = 3456          -> Integer part of decimal number
     * diff = 0.78                  -> Difference between actual decimal and integer part of decimal
     *                                  This is used for rounding of last digit (if necessary)
     */
    num += 0.000000000000005;
    n->integer_part = (float_long_t)num;
    n->decimal_part_dbl = (num - (double)n->integer_part) * (double)powers_of_10[p->m.precision];
    n->decimal_part = (float_long_t)n->decimal_part_dbl;
    n->diff = n->decimal_part_dbl - (float_long_t)n->decimal_part;

    /* Rounding check of last digit */
    if (n->diff > 0.5f) {
        ++n->decimal_part;
        if (n->decimal_part >= powers_of_10[p->m.precision]) {
            n->decimal_part = 0;
            ++n->integer_part;
        }
    } else if (n->diff < 0.5f) {
        /* Used in separate if, since comparing float to == will certainly result to false */
    } else {
        /* Difference is exactly 0.5 */
        if (n->decimal_part == 0) {
            ++n->integer_part;
        } else {
            ++n->decimal_part;
        }
    }

    /* Calculate number of digits for integer and decimal parts */
    if (n->integer_part == 0) {
        n->digits_cnt_integer_part = 1;
    } else {
        float_long_t tmp;
        for (n->digits_cnt_integer_part = 0, tmp = n->integer_part; tmp > 0; ++n->digits_cnt_integer_part, tmp /= 10) {}
    }
    n->digits_cnt_decimal_part = p->m.precision;

#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
    /* Calculate minimum useful digits for decimal (excl last useless zeros) */
    if (type == 'g') {
        float_long_t tmp = n->decimal_part;
        int adder, i;
        for (adder = 0, i = 0; tmp > 0 || i < p->m.precision; tmp /= 10, n->digits_cnt_decimal_part_useful += adder, ++i) {
            if (adder == 0 && (tmp % 10) > 0) {
                adder = 1;
            }
        }
    } else
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */
    {
        n->digits_cnt_decimal_part_useful = p->m.precision;
    }
}

/**
 * \brief           Convert double number to string
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       num: Number to convert to string
 * \return          `1` on success, `0` otherwise
 */
static int
prv_double_to_str(lwprintf_int_t* p, double in_num) {
    float_num_t dblnum;
    size_t i;
    double orig_num = in_num;
    int digits_cnt, chosen_precision, exp_cnt = 0;
    char def_type = p->m.type;

#if LWPRINTF_CFG_SUPPORT_LONG_LONG
    char str[22];
#else
    char str[11];
#endif /* LWPRINTF_CFG_SUPPORT_LONG_LONG */

    /*
     * Check for corner cases
     *
     * - Print "nan" if number is not valid
     * - Print negative infinity if number is less than absolute minimum
     * - Print negative infinity if number is less than -FLOAT_MAX_B_ENG and engineering mode is disabled
     * - Print positive infinity if number is greater than absolute minimum
     * - Print positive infinity if number is greater than FLOAT_MAX_B_ENG and engineering mode is disabled
     * - Go to engineering mode if it is enabled and `in_num < -FLOAT_MAX_B_ENG` or `in_num > FLOAT_MAX_B_ENG`
     */
    if (in_num != in_num) {
        return prv_out_str(p, p->m.flags.uc ? "NAN" : "nan", 3);
    } else if (in_num < -DBL_MAX
#if !LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
            || in_num < -FLOAT_MAX_B_ENG
#endif /* !LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */
    ) {
        return prv_out_str(p, p->m.flags.uc ? "-INF" : "-inf", 4);
    } else if (in_num > DBL_MAX
#if !LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
            || in_num > FLOAT_MAX_B_ENG
#endif /* !LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */
    ) {
        char str[5], *s_ptr = str;
        if (p->m.flags.plus) {
            *s_ptr++ = '+';
        }
        strcpy(s_ptr, p->m.flags.uc ? "INF" : "inf");
        return prv_out_str(p, str, p->m.flags.plus ? 4 : 3);
#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
    } else if ((in_num < -FLOAT_MAX_B_ENG || in_num > FLOAT_MAX_B_ENG) && def_type != 'g') {
        p->m.type = def_type = 'e';             /* Go to engineering mode */
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */
    }

    /* Check sign of the number */
    SIGNED_CHECK_NEGATIVE(p, in_num);
    orig_num = in_num;

#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
    /* Engineering mode check for number of exponents */
    if (def_type == 'e' || def_type == 'g'
        || in_num > (powers_of_10[LWPRINTF_ARRAYSIZE(powers_of_10) - 1])) { /* More vs what float can hold */
        if (p->m.type != 'g') {
            p->m.type = 'e';
        }

        /* Normalize number to be between 0 and 1 and count decimals for exponent */
        if (in_num < 1) {
            for (exp_cnt = 0; in_num < 1 && in_num > 0; in_num *= 10, --exp_cnt) {}
        } else {
            for (exp_cnt = 0; in_num >= 10; in_num /= 10, ++exp_cnt) {}
        }
    }
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */

    /* Check precision data */
    chosen_precision = p->m.precision;          /* This is default value coming from app */
    if (p->m.precision >= LWPRINTF_ARRAYSIZE(powers_of_10)) {
        p->m.precision = LWPRINTF_ARRAYSIZE(powers_of_10) - 1;  /* Limit to maximum precision */
        /*
         * Precision is lower than the one selected by app (or user).
         * It means that we have to append ending zeros for precision when printing data
         */
    } else if (!p->m.flags.precision) {
        p->m.precision = LWPRINTF_CFG_FLOAT_DEFAULT_PRECISION;  /* Default precision when not used */
        chosen_precision = p->m.precision;      /* There was no precision, update chosen precision */
    } else if (p->m.flags.precision && p->m.precision == 0) {
#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
        /* Precision must be set to 1 if set to 0 by default */
        if (def_type == 'g') {
            p->m.precision = 1;
        }
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */
    }

    /* Check if type is g and decide if final output should be 'f' or 'e' */
    /*
     * For 'g/G' specifier
     *
     * A double argument representing a floating-point number is converted
     * in style 'f' or 'e' (or in style 'F' or 'E' in the case of a 'G' conversion specifier),
     * depending on the value converted and the precision.
     * Let 'P' equal the precision if nonzero, '6' if the precision is omitted, or '1' if the precision is zero.
     * Then, if a conversion with style 'E' would have an exponent of 'X':
     *
     * if 'P > X >= -4', the conversion is with style 'f' (or 'F') and precision 'P - (X + 1)'.
     * otherwise, the conversion is with style 'e' (or 'E') and precision 'P - 1'.
     *
     * Finally, unless the '#' flag is used,
     * any trailing zeros are removed from the fractional portion of the result
     * and the decimal-point character is removed if there is no fractional portion remaining.
     *
     * A double argument representing an infinity or 'NaN' is converted in the style of an 'f' or 'F' conversion specifier.
     */

    /* Calculate data for number */
    prv_calculate_dbl_num_data(p, &dblnum, def_type == 'e' ? in_num : orig_num, def_type == 'e' ? 0 : exp_cnt, def_type);
    //prv_calculate_dbl_num_data(p, &dblnum, orig_num, exp_cnt, def_type);

#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
    /* Set type G */
    if (def_type == 'g') {
        /* As per standard to decide level of precision */
        if (exp_cnt >= -4 && exp_cnt < p->m.precision) {
            if (p->m.precision > exp_cnt) {
                p->m.precision -= exp_cnt + 1;
                chosen_precision -= exp_cnt + 1;
            } else {
                p->m.precision = 0;
                chosen_precision = 0;
            }
            p->m.type = 'f';
            in_num = orig_num;
        } else {
            p->m.type = 'e';
            if (p->m.precision > 0) {
                --p->m.precision;
                --chosen_precision;
            }
        }
        prv_calculate_dbl_num_data(p, &dblnum, in_num, 0, def_type);
    }
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */

    /* Set number of digits to display */
    digits_cnt = dblnum.digits_cnt_integer_part;
#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
    if (def_type == 'g' && p->m.precision > 0) {
        digits_cnt += dblnum.digits_cnt_decimal_part_useful;
        if (dblnum.digits_cnt_decimal_part_useful > 0) {
            ++digits_cnt;
        }
    } else
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */
    if (chosen_precision > 0 && p->m.flags.precision) {
        /* Add precision digits + dot separator */
        digits_cnt += chosen_precision + 1;
    }

#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
    /* Increase number of digits to display */
    if (p->m.type == 'e') {
        /* Format is +Exxx, so add 4 or 5 characters (max is 307, min is 00 for exponent) */
        digits_cnt += 4 + (exp_cnt >= 100 || exp_cnt <= -100);
    }
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */

    /* Output strings */
    prv_out_str_before(p, digits_cnt);

    /* Output integer part of number */
    if (dblnum.integer_part == 0) {
        p->out_fn(p, '0');
    } else {
        for (i = 0; dblnum.integer_part > 0; dblnum.integer_part /= 10, ++i) {
            str[i] = '0' + (char)(dblnum.integer_part % 10);
        }
        for (; i > 0; --i) {
            p->out_fn(p, str[i - 1]);
        }
    }

    /* Output decimal part */
    if (p->m.precision > 0) {
        int x;
        if (dblnum.digits_cnt_decimal_part_useful > 0) {
            p->out_fn(p, '.');
        }
        for (i = 0; dblnum.decimal_part > 0; dblnum.decimal_part /= 10, ++i) {
            str[i] = '0' + (dblnum.decimal_part % 10);
        }

        /* Output relevant zeros first, string to print is opposite way */
#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
        if (def_type == 'g') {
            /* TODO: This is to be checked */
            for (x = 0; x < p->m.precision - i && dblnum.digits_cnt_decimal_part_useful > 0; ++x, --dblnum.digits_cnt_decimal_part_useful) {
                p->out_fn(p, '0');
            }
        } else
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */
        {
            for (x = i; x < p->m.precision; ++x) {
                p->out_fn(p, '0');
            }
        }

        /* Now print string itself */
        for (; i > 0; --i) {
            p->out_fn(p, str[i - 1]);
#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
            if (def_type == 'g' && --dblnum.digits_cnt_decimal_part_useful == 0) {
                break;
            }
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */
        }

        /* Print ending zeros if selected precision is bigger than maximum supported */
        if (def_type != 'g') {
            for (; x < chosen_precision; ++x) {
                p->out_fn(p, '0');
            }
        }
    }

#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
    /* Engineering mode output, add exponent part */
    if (p->m.type == 'e') {
        p->out_fn(p, p->m.flags.uc ? 'E' : 'e');
        p->out_fn(p, exp_cnt >= 0 ? '+' : '-');
        if (exp_cnt < 0) {
            exp_cnt = -exp_cnt;
        }
        if (exp_cnt >= 100) {
            p->out_fn(p, '0' + (char)(exp_cnt / 100));
            exp_cnt /= 100;
        }
        p->out_fn(p, '0' + (char)(exp_cnt / 10));
        p->out_fn(p, '0' + (char)(exp_cnt % 10));
    }
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */
    prv_out_str_after(p, digits_cnt);

    return 1;
}

#endif /* LWPRINTF_CFG_SUPPORT_TYPE_FLOAT */

/**
 * \brief           Process format string and parse variable parameters
 * \param[in,out]   p: LwPRINTF internal instance
 * \param[in]       arg: Variable parameters list
 * \return          `1` on success, `0` otherwise
 */
static uint8_t
prv_format(lwprintf_int_t* p, va_list arg) {
    uint8_t detected = 0;
    const char* fmt = p->fmt;

#if LWPRINTF_CFG_OS
    if (IS_PRINT_MODE(p) &&                     /* OS protection only for print */
        (!lwprintf_sys_mutex_isvalid(&p->lw->mutex) /* Invalid mutex handle */
            || !lwprintf_sys_mutex_wait(&p->lw->mutex))) {  /* Cannot acquire mutex */
        return 0;
    }
#endif /* LWPRINTF_CFG_OS */

    while (fmt != NULL && *fmt != '\0') {
        /* Check if we should stop processing */
        if (p->is_print_cancelled) {
            break;
        }

        /* Detect beginning */
        if (*fmt != '%') {
            p->out_fn(p, *fmt);                 /* Output character */
            ++fmt;
            continue;
        }
        ++fmt;
        memset(&p->m, 0x00, sizeof(p->m));      /* Reset structure */

        /* Parse format */
        /* %[flags][width][.precision][length]type */
        /* Go to https://docs.majerle.eu for more info about supported features */

        /* Check [flags] */
        /* It can have multiple flags in any order */
        detected = 1;
        do {
            switch (*fmt) {
                case '-':
                    p->m.flags.left_align = 1;
                    break;
                case '+':
                    p->m.flags.plus = 1;
                    break;
                case ' ':
                    p->m.flags.space = 1;
                    break;
                case '0':
                    p->m.flags.zero = 1;
                    break;
                case '\'':
                    p->m.flags.thousands = 1;
                    break;
                case '#':
                    p->m.flags.alt = 1;
                    break;
                default:
                    detected = 0;
                    break;
            }
            if (detected) {
                ++fmt;
            }
        } while (detected);

        /* Check [width] */
        p->m.width = 0;
        if (CHARISNUM(*fmt)) {                  /* Fixed width check */
            /* If number is negative, it has been captured from previous step (left align) */
            p->m.width = prv_parse_num(&fmt);   /* Number from string directly */
        } else if (*fmt == '*') {               /* Or variable check */
            const int w = (int)va_arg(arg, int);
            if (w < 0) {
                p->m.flags.left_align = 1;      /* Negative width means left aligned */
                p->m.width = -w;
            } else {
                p->m.width = w;
            }
            ++fmt;
        }

        /* Check [.precision] */
        p->m.precision = 0;
        if (*fmt == '.') {                      /* Precision flag is detected */
            p->m.flags.precision = 1;
            if (*++fmt == '*') {                /* Variable check */
                const int pr = (int)va_arg(arg, int);
                p->m.precision = pr > 0 ? pr : 0;
                ++fmt;
            } else if (CHARISNUM(*fmt)) {       /* Directly in the string */
                p->m.precision = prv_parse_num(&fmt);
            }
        }

        /* Check [length] */
        detected = 1;
        switch (*fmt) {
            case 'h':
                p->m.flags.char_short = 1;      /* Single h detected */
                if (*++fmt == 'h') {            /* Does it follow by another h? */
                    p->m.flags.char_short = 2;  /* Second h detected */
                    ++fmt;
                }
                break;
            case 'l':
                p->m.flags.longlong = 1;        /* Single l detected */
                if (*++fmt == 'l') {            /* Does it follow by another l? */
                    p->m.flags.longlong = 2;    /* Second l detected */
                    ++fmt;
                }
                break;
            case 'L':
                break;
            case 'z':
                p->m.flags.sz_t = 1;            /* Size T flag */
                ++fmt;
                break;
            case 'j':
                p->m.flags.umax_t = 1;          /* uintmax_t flag */
                ++fmt;
                break;
            case 't':
                break;
            default:
                detected = 0;
        }

        /* Check type */
        p->m.type = *fmt + ((*fmt >= 'A' && *fmt <= 'Z') ? 0x20 : 0x00);
        if (*fmt >= 'A' && *fmt <= 'Z') {
            p->m.flags.uc = 1;
        }
        switch (*fmt) {
            case 'a':
            case 'A':
                /* Double in hexadecimal notation */
                (void)va_arg(arg, double);      /* Read argument to ignore it and move to next one */
                prv_out_str_raw(p, "NaN", 3);   /* Print string */
                break;
            case 'c':
                p->out_fn(p, (char)va_arg(arg, int));
                break;
#if LWPRINTF_CFG_SUPPORT_TYPE_INT
            case 'd':
            case 'i': {
                /* Check for different length parameters */
                p->m.base = 10;
                if (p->m.flags.longlong == 0) {
                    prv_signed_int_to_str(p, (signed int)va_arg(arg, signed int));
                } else if (p->m.flags.longlong == 1) {
                    prv_signed_long_int_to_str(p, (signed long int)va_arg(arg, signed long int));
#if LWPRINTF_CFG_SUPPORT_LONG_LONG
                } else if (p->m.flags.longlong == 2) {
                    prv_signed_longlong_int_to_str(p, (signed long long int)va_arg(arg, signed long long int));
#endif /* LWPRINTF_CFG_SUPPORT_LONG_LONG */
                }
                break;
            }
            case 'b':
            case 'B':
            case 'o':
            case 'u':
            case 'x':
            case 'X':
                if (*fmt == 'b' || *fmt == 'B') {
                    p->m.base = 2;
                } else if (*fmt == 'o') {
                    p->m.base = 8;
                } else if (*fmt == 'u') {
                    p->m.base = 10;
                } else if (*fmt == 'x' || *fmt == 'X') {
                    p->m.base = 16;
                }
                p->m.flags.space = 0;           /* Space flag has no meaning here */

                /* Check for different length parameters */
                if (0) {

                } else if (p->m.flags.sz_t) {
                    prv_sizet_to_str(p, (size_t)va_arg(arg, size_t));
                } else if (p->m.flags.umax_t) {
                    prv_umaxt_to_str(p, (uintmax_t)va_arg(arg, uintmax_t));
                } else if (p->m.flags.longlong == 0 || p->m.base == 2) {
                    unsigned int v;
                    switch (p->m.flags.char_short) {
                        case 2:
                            v = (unsigned int)((unsigned char)va_arg(arg, unsigned int));
                            break;
                        case 1:
                            v = (unsigned int)((unsigned short int)va_arg(arg, unsigned int));
                            break;
                        default:
                            v = (unsigned int)((unsigned int)va_arg(arg, unsigned int));
                            break;
                    }
                    prv_unsigned_int_to_str(p, v);
                } else if (p->m.flags.longlong == 1) {
                    prv_unsigned_long_int_to_str(p, (unsigned long int)va_arg(arg, unsigned long int));
#if LWPRINTF_CFG_SUPPORT_LONG_LONG
                } else if (p->m.flags.longlong == 2) {
                    prv_unsigned_longlong_int_to_str(p, (unsigned long long int)va_arg(arg, unsigned long long int));
#endif /* LWPRINTF_CFG_SUPPORT_LONG_LONG */
                }
                break;
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_INT */
#if LWPRINTF_CFG_SUPPORT_TYPE_STRING
            case 's': {
                const char* b = va_arg(arg, const char*);
                size_t len = strlen(b);

                /* Precision gives maximum output len */
                if (p->m.flags.precision) {
                    if (len > p->m.precision) {
                        len = p->m.precision;
                    }
                }
                prv_out_str(p, b, len);
                break;
            }
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_STRING */
#if LWPRINTF_CFG_SUPPORT_TYPE_POINTER
            case 'p': {
                p->m.base = 16;                 /* Go to hex format */
                p->m.flags.uc = 0;              /* Uppercase numbers */
                p->m.flags.zero = 1;            /* Zero padding */
                p->m.flags.alt = 1;
                p->m.width = sizeof(uintptr_t) * 2; /* Number is in hex format and byte is represented with 2 letters */

                prv_uintptr_to_str(p, (uintptr_t)va_arg(arg, uintptr_t));
                break;
            }
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_POINTER */
#if LWPRINTF_CFG_SUPPORT_TYPE_FLOAT
            case 'f':
            case 'F':
#if LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING
            case 'e':
            case 'E':
            case 'g':
            case 'G':
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_ENGINEERING */
                /* Double number in different format. Final output depends on type of format */
                prv_double_to_str(p, (double)va_arg(arg, double));
                break;
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_FLOAT */
            case 'n': {
                int* ptr = (void*)va_arg(arg, int*);
                *ptr = p->n;                    /* Write current length */

                break;
            }
            case '%':
                p->out_fn(p, '%');
                break;
#if LWPRINTF_CFG_SUPPORT_TYPE_BYTE_ARRAY
            /*
             * This is to print unsigned-char formatted pointer in hex string
             *
             * char arr[] = {0, 1, 2, 3, 255};
             * "%5K" would produce 00010203FF
             */
            case 'k':
            case 'K': {
                unsigned char* ptr = (void *)va_arg(arg, unsigned char *);  /* Get input parameter as unsigned char pointer */
                int len = p->m.width, full_width;
                uint8_t is_space = p->m.flags.space == 1;

                if (ptr == NULL || len == 0) {
                    break;
                }

                p->m.flags.zero = 1;            /* Prepend with zeros if necessary */
                p->m.width = 0;                 /* No width parameter */
                p->m.base = 16;                 /* Hex format */
                p->m.flags.space = 0;           /* Delete any flag for space */

                /* Full width of digits to print */
                full_width = len * (2 + (int)is_space);
                if (is_space && full_width > 0) {
                    --full_width;               /* Remove space after last number */
                }

                /* Output byte by byte w/o hex prefix */
                prv_out_str_before(p, full_width);
                for (int i = 0; i < len; ++i, ++ptr) {
                    uint8_t d;

                    d = (*ptr >> 0x04) & 0x0F;  /* Print MSB */
                    p->out_fn(p, (char)(d) + (d >= 10 ? ((p->m.flags.uc ? 'A' : 'a') - 10) : '0'));
                    d = *ptr & 0x0F;            /* Print LSB */
                    p->out_fn(p, (char)(d) + (d >= 10 ? ((p->m.flags.uc ? 'A' : 'a') - 10) : '0'));

                    if (is_space && i < (len - 1)) {
                        p->out_fn(p, ' ');      /* Generate space between numbers */
                    }
                }
                prv_out_str_after(p, full_width);
                break;
            }
#endif /* LWPRINTF_CFG_SUPPORT_TYPE_BYTE_ARRAY */
            default:
                p->out_fn(p, *fmt);
        }
        ++fmt;
    }
    p->out_fn(p, '\0');                         /* Output last zero number */
#if LWPRINTF_CFG_OS
    if (IS_PRINT_MODE(p)) {                     /* Mutex only for print operation */
        lwprintf_sys_mutex_release(&p->lw->mutex);
    }
#endif /* LWPRINTF_CFG_OS */
    return 1;
}

/**
 * \brief           Initialize LwPRINTF instance
 * \param[in,out]   lw: LwPRINTF working instance
 * \param[in]       out_fn: Output function used for print operation
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwprintf_init_ex(lwprintf_t* lw, lwprintf_output_fn out_fn) {
    LWPRINTF_GET_LW(lw)->out_fn = out_fn;

#if LWPRINTF_CFG_OS
    /* Create system mutex */
    if (lwprintf_sys_mutex_isvalid(&LWPRINTF_GET_LW(lw)->mutex)
        || !lwprintf_sys_mutex_create(&LWPRINTF_GET_LW(lw)->mutex)) {
        return 0;
    }
#endif /* LWPRINTF_CFG_OS */
    return 1;
}

/**
 * \brief           Print formatted data from variable argument list to the output
 * \param[in,out]   lw: LwPRINTF instance. Set to `NULL` to use default instance
 * \param[in]       format: C string that contains the text to be written to output
 * \param[in]       arg: A value identifying a variable arguments list initialized with `va_start`.
 *                      `va_list` is a special type defined in `<cstdarg>`.
 * \return          The number of characters that would have been written if `n` had been sufficiently large,
 *                      not counting the terminating null character.
 */
int
lwprintf_vprintf_ex(lwprintf_t* const lw, const char* format, va_list arg) {
    lwprintf_int_t f = {
        .lw = LWPRINTF_GET_LW(lw),
        .out_fn = prv_out_fn_print,
        .fmt = format,
        .buff = NULL,
        .buff_size = 0
    };
    /* For direct print, output function must be set by user */
    if (f.lw->out_fn == NULL) {
        return 0;
    }
    prv_format(&f, arg);
    return f.n;
}

/**
 * \brief           Print formatted data to the output
 * \param[in,out]   lw: LwPRINTF instance. Set to `NULL` to use default instance
 * \param[in]       format: C string that contains the text to be written to output
 * \param[in]       ...: Optional arguments for format string
 * \return          The number of characters that would have been written if `n` had been sufficiently large,
 *                      not counting the terminating null character.
 */
int
lwprintf_printf_ex(lwprintf_t* const lw, const char* format, ...) {
    va_list va;
    int n;

    va_start(va, format);
    n = lwprintf_vprintf_ex(lw, format, va);
    va_end(va);

    return n;
}

/**
 * \brief           Write formatted data from variable argument list to sized buffer
 * \param[in,out]   lw: LwPRINTF instance. Set to `NULL` to use default instance
 * \param[in]       s: Pointer to a buffer where the resulting C-string is stored.
 *                      The buffer should have a size of at least `n` characters
 * \param[in]       n: Maximum number of bytes to be used in the buffer.
 *                      The generated string has a length of at most `n - 1`,
 *                      leaving space for the additional terminating null character
 * \param[in]       format: C string that contains a format string that follows the same specifications as format in printf
 * \param[in]       arg: A value identifying a variable arguments list initialized with `va_start`.
 *                      `va_list` is a special type defined in `<cstdarg>`.
 * \return          The number of characters that would have been written if `n` had been sufficiently large,
 *                      not counting the terminating null character.
 */
int
lwprintf_vsnprintf_ex(lwprintf_t* const lw, char* s, size_t n, const char* format, va_list arg) {
    lwprintf_int_t f = {
        .lw = LWPRINTF_GET_LW(lw),
        .out_fn = prv_out_fn_write_buff,
        .fmt = format,
        .buff = s,
        .buff_size = n
    };

    if(s == NULL && n == 0) {
        f.out_fn = prv_out_fn_write_null;
    } else if(s == NULL) {
        return 0;
    }

    prv_format(&f, arg);

    if(s != NULL) {
        s[f.n] = '\0';
    }

    return f.n;
}

/**
 * \brief           Write formatted data from variable argument list to sized buffer
 * \param[in,out]   lw: LwPRINTF instance. Set to `NULL` to use default instance
 * \param[in]       s: Pointer to a buffer where the resulting C-string is stored.
 *                      The buffer should have a size of at least `n` characters
 * \param[in]       n: Maximum number of bytes to be used in the buffer.
 *                      The generated string has a length of at most `n - 1`,
 *                      leaving space for the additional terminating null character
 * \param[in]       format: C string that contains a format string that follows the same specifications as format in printf
 * \param[in]       ...: Optional arguments for format string
 * \return          The number of characters that would have been written if `n` had been sufficiently large,
 *                      not counting the terminating null character.
 */
int
lwprintf_snprintf_ex(lwprintf_t* const lw, char* s, size_t n, const char* format, ...) {
    va_list va;
    int len;

    va_start(va, format);
    len = lwprintf_vsnprintf_ex(lw, s, n, format, va);
    va_end(va);

    return len;
}
