/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 *  ======== System.c ========
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/runtime/Startup.h>
#include <ti/sysbios/runtime/System.h>
#include <ti/sysbios/runtime/SystemSupport.h>
#include <ti/sysbios/runtime/Types.h>

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

/*
 *  ======== {U}IntMax ========
 *  Maximum sized (un)signed integer that can be formated by System_printf
 *
 *  {I|U}intptr_t can't be used because we need to be able to handle a long or
 *  a pointer (not just an int or a pointer).
 *  
 *  These internal declarations are here rather than in System.h to ensure 
 *  that targets _without_ 64-bit support can still use System
 */
#if ((xdc_target__bitsPerChar * xdc_target__sizeof_Ptr) > 32)
typedef uint64_t UIntMax;
typedef int64_t IntMax;
#else
typedef uint32_t UIntMax;
typedef int32_t IntMax;
#endif

/*
 *  ======== OUTMAX ========
 *  The maximum length of the output of a base 8 number produced by 
 *  System_formatNum plus 5 to accomodate the decimal point and 4 digits
 *  after the decimal point.
 */
#if ((xdc_target__bitsPerChar * xdc_target__sizeof_Ptr) > 32)
#define OUTMAX      ((64 + 2) / 3) + 5
#define PTRZPAD     16
#else
#define OUTMAX      ((32 + 2) / 3) + 5
#define PTRZPAD     8
#endif

System_AtexitHandler System_atexitHandlers[System_maxAtexitHandlers_D];

System_Module_State System_Module_state = {
    .atexitHandlers = System_atexitHandlers,
    .numAtexitHandlers = 0,
    .initDone = 0
};

const int System_maxAtexitHandlers = System_maxAtexitHandlers_D;

const System_AbortFxn System_abortFxn = System_abortFxn_D;

const System_ExitFxn System_exitFxn = System_exitFxn_D;

char *System_formatNum(char *ptr, UIntMax un, int zpad, int base);

int System_printfExtend(char **pbuf, const char * *pfmt, va_list *pva,
        System_ParseData *parse);

/*
 *  ======== vaRef ========
 *  Return the address of a va_list (aka va_list)
 *
 *  If va_list is an array type, taking the address of the va_list va simply
 *  returns va itself.  Moreover, when such a va_list is passed to a
 *  function, C implicitly passes the address rather than the va_list array
 *  "value" itself.  Taken together, this means we can "safely" cast a
 *  va_list value passed to a function as a (va_list *) when passing it on
 *  to functions expecting a (va_list *).
 *
 *  Ignoring performance concerns, we can be squeaky clean and copy the
 *  va_list value to a local variable and pass the address of this local
 *  variable; for example:
 *    void vprint(char * fmt, va_list va)
 *    {
 *        int ret;
 *        va_list nva;
 *        va_copy(nva, va);
 *        ret = System_doPrint(NULL, (size_t)-1, fmt, &nva, true);
 *        va_end(nva);
 *        return (ret);
 *    }
 *  But this wastes stack space and CPU time to initialize a copy of something
 *  that already exists and for which we already have a legitimate reference.
 *
 *  Of course, if va_list is not an array type, we must use the '&' operator.
 */
#if xdc_target__arraytype_VaList
# define vaRef(va)    ((va_list *)(va))
#else
# define vaRef(va)    (&(va))
#endif

/*
 *  ======== System_init ========
 */
void System_init(void)
{
    /* Interrupts are disabled at this point */
    if (System_module->initDone) {
        return;
    }
    System_module->initDone = true;

    SystemSupport_init();
}

/*
 *  ======== System_abort ========
 */
/* REQ_TAG(SYSBIOS-1069) */
void System_abort(const char * str)
{
    (void)Hwi_disable();

    SystemSupport_abort(str);

    /* REQ_TAG(SYSBIOS-899) */
    System_abortFxn();
}

/*
 *  ======== System_atexit ========
 */
/* REQ_TAG(SYSBIOS-910) */
bool System_atexit(System_AtexitHandler handler)
{
    unsigned int key;
    bool status = true;

    key = Hwi_disable();

    if (System_module->numAtexitHandlers < System_maxAtexitHandlers) {
        System_module->atexitHandlers[System_module->numAtexitHandlers] = handler;
        System_module->numAtexitHandlers++;
    }
    else {
        status = false;
    }

    Hwi_restore(key);

    return (status);
}

/*
 *  ======== System_exit ========
 */
/* REQ_TAG(SYSBIOS-1070) */
void System_exit(int stat)
{
    System_processAtExit(stat);

    /* REQ_TAG(SYSBIOS-906) */
    System_exitFxn(stat);
}

/*
 *  ======== System_abortStd ========
 */
/* REQ_TAG(SYSBIOS-901) */
void System_abortStd(void)
{
    abort();
}

/*
 *  ======== System_abortSpin ========
 */
/* REQ_TAG(SYSBIOS-900) */
/* LCOV_EXCL_START */
void __attribute__ ((noinline))System_abortSpin(void)
{
/* LCOV_EXCL_STOP */
    for (;;) {
        __asm("");
    }
}

/*
 *  ======== System_exitStd ========
 */
/* REQ_TAG(SYSBIOS-908) */
void System_exitStd(int stat)
{
    exit(stat);
}

/*
 *  ======== System_exitSpin ========
 */
/* REQ_TAG(SYSBIOS-907) */
/* LCOV_EXCL_START */
void __attribute__ ((noinline)) System_exitSpin(int stat)
{
/* LCOV_EXCL_STOP */
    for (;;) {
        __asm("");
    }
}

/*
 *  ======== System_processAtExit ========
 */
/* REQ_TAG(SYSBIOS-911) */
void System_processAtExit(int stat)
{
    int i;

    (void)Hwi_disable();

    for (i = System_module->numAtexitHandlers; i > 0; i--) {
        (System_module->atexitHandlers[i - 1])(stat);
    }

    SystemSupport_exit(stat);
}

/*
 *  ======== System_flush ========
 */
/* REQ_TAG(SYSBIOS-909) */
void System_flush(void)
{
    SystemSupport_flush();
}


/*
 *  ======== System_putch ========
 */
/* REQ_TAG(SYSBIOS-912) */
void System_putch(char ch)
{
    if (SystemSupport_ready() == true) {
        SystemSupport_putch(ch);
    }
}

/*
 *  ======== System_aprintf_va ========
 */
int System_aprintf_va(const char * fmt, va_list va)
{
    return (System_avprintf(fmt, va));
}

/*
 *  ======== System_avprintf ========
 *  -1 indicates infinite output
 */
/* REQ_TAG(SYSBIOS-902), REQ_TAG(SYSBIOS-903) */
int System_avprintf(const char * fmt, va_list va)
{
    return ((SystemSupport_ready() == true)
            ? System_doPrint((char *)NULL, (size_t)-1, fmt, vaRef(va), true)
            : -1);
}

/*
 *  ======== System_asprintf_va ========
 */
int System_asprintf_va(char buf[], const char * fmt, va_list va)
{
    return (System_avsprintf(buf, fmt, va));
}

/*
 *  ======== System_avsprintf ========
 *  -1 indicates infinite output
 */
/* REQ_TAG(SYSBIOS-902), REQ_TAG(SYSBIOS-903), REQ_TAG(SYSBIOS-904) */
int System_avsprintf(char buf[], const char * fmt, va_list va)
{
    return (System_doPrint(buf, (size_t)-1, fmt, vaRef(va), true));
}

/*
 *  ======== System_printf_va ========
 */
int System_printf_va(const char * fmt, va_list va)
{
    return (System_vprintf(fmt, va));
}

/*
 *  ======== System_printf ========
 */
int System_printf(const char * fmt, ...)
{
    int retval;

    va_list arg__va;
    (void)va_start(arg__va, fmt);
    retval = System_printf_va(fmt, arg__va);

    va_end(arg__va);
    return retval;
}

/*
 *  ======== System_vprintf ========
 *  -1 indicates infinite output
 */
/* REQ_TAG(SYSBIOS-902) */
int System_vprintf(const char * fmt, va_list va)
{
    return ((SystemSupport_ready() == true)
            ? System_doPrint((char *)NULL, (size_t)-1, fmt, vaRef(va), false)
            : -1);
}

/*
 *  ======== System_sprintf_va ========
 */
int System_sprintf_va(char buf[], const char * fmt, va_list va)
{
    return (System_vsprintf(buf, fmt, va));
}

/*
 *  ======== System_sprintf ========
 */
int System_sprintf(char buf[], const char * fmt, ...)
{
    int retval;

    va_list arg__va;
    (void)va_start(arg__va, fmt);
    retval = System_sprintf_va(buf, fmt, arg__va);

    va_end(arg__va);
    return retval;
}

/*
 *  ======== System_vsprintf ========
 *  -1 indicates infinite output
 */
/* REQ_TAG(SYSBIOS-902), REQ_TAG(SYSBIOS-904) */
int System_vsprintf(char buf[], const char * fmt, va_list va)
{
    return (System_doPrint(buf, (size_t)-1, fmt, vaRef(va), false));
}

/*
 *  ======== System_snprintf_va ========
 */
int System_snprintf_va(char buf[], size_t n, const char * fmt, va_list va)
{
    return (System_vsnprintf(buf, n, fmt, va));
}

/*
 *  ======== System_snprintf ========
 */
int System_snprintf(char buf[], size_t n, const char * fmt, ...)
{
    int retval;

    va_list arg__va;
    (void)va_start(arg__va, fmt);
    retval = System_snprintf_va(buf, n, fmt, arg__va);

    va_end(arg__va);
    return retval;
}

/*
 *  ======== System_vsnprintf ========
 */
/* REQ_TAG(SYSBIOS-902), REQ_TAG(SYSBIOS-904) */
int System_vsnprintf(char buf[], size_t n, const char * fmt, va_list va)
{
    return (System_doPrint(buf, n, fmt, vaRef(va), false));
}

/*
 *  ======== System_doPrint ========
 *  Internal function
 *
 *  If buf == NULL, characters are sent to SystemSupport_putch();
 *  otherwise, they are written into buf. Atmost `n` - 1 characters are written
 *  excluding '\0'.
 *
 *  The return value is the number of characters that would have
 *  been written had `n` been sufficiently large, not counting the terminating
 *  '\0' character.
 *
 *  Use 'weak' definition so Agama ROM version can be used if aliased
 *  within linker command file
 */
int __attribute__((weak))
System_doPrint(char *buf, size_t n, const char * fmt, va_list *pva, bool aFlag)
{
    /* temp vars */
    int     base;
    char    c;
    int     res;
    int     temp_res;
    char    outbuf[OUTMAX];
    ptrdiff_t diff;

    /* vars passed to System_extendFxn. Also keep track in while loop */
    struct System_ParseData parse;

    parse.aFlag = aFlag;

    res = 0;

    if (fmt == (char *)NULL) {
        return (res);
    }

    c = *fmt;
    fmt++;
    while (c != '\0') {
        if (c != '%') {
            System_putchar(&buf, c, &n);
            res++;
        }
        else {
            c = *fmt;
            fmt++;
            /* check for - flag (pad on right) */
            if (c == '-') {
                parse.lJust = true;
                c = *fmt;
                fmt++;
            }
            else {
                parse.lJust = false;
            }
            /* check for leading 0 pad */
            if (c == '0') {
                parse.zpad = 1;
                c = *fmt;
                fmt++;
            }
            else {
                parse.zpad = 0;
            }

            /* allow optional field width/precision specification */
            parse.width = 0;
            parse.precis = -1;

            /* note: dont use isdigit (very large for C30) */
            if (c == '*') {
                parse.width = (parse.aFlag == true)
                    ? (int)va_arg(*pva, intptr_t) : (int)va_arg(*pva, int);
                c = *fmt;
                fmt++;
                if (parse.width < 0) {
                    parse.lJust = true;
                    parse.width = -parse.width;
                }
            }
            else {
                while ((c >= '0') && (c <= '9')) {
                    parse.width = (parse.width * 10) + c - '0';
                    c = *fmt;
                    fmt++;
                }
            }

            /* allow optional field precision specification */
            if (c == '.') {
                parse.precis = 0;
                c = *fmt;
                fmt++;
                if (c == '*') {
                    parse.precis = (parse.aFlag == true)
                        ? (int)va_arg(*pva, intptr_t) : (int)va_arg(*pva, int);
                    if (parse.precis < 0) {
                        parse.precis = 0;
                    }

                    c = *fmt;
                    fmt++;
                }
                else {
                    while ((c >= '0') && (c <= '9')) {
                        parse.precis = (parse.precis * 10) + c - '0';
                        c = *fmt;
                        fmt++;
                    }
                }
            }

            /* setup for leading zero padding */
            if (parse.zpad != 0) {
                parse.zpad = parse.width;
            }

            /* check for presence of l flag (e.g., %ld) */
            if ((c == 'l') || (c == 'L')) {
                parse.lFlag = true;
                c = *fmt;
                fmt++;
            }
            else {
                parse.lFlag = false;
            }

            parse.ptr = outbuf;
            parse.end = outbuf + OUTMAX;
            parse.len = 0;

            if ((c == 'd') || (c == 'i')) {
                /* signed decimal */
                IntMax val =
                    (parse.aFlag == true) ? (IntMax)va_arg(*pva, intptr_t) :
                    (parse.lFlag == true) ? (IntMax)va_arg(*pva, long int) :
                    (IntMax)va_arg(*pva, int);

                if (parse.precis > parse.zpad) {
                    parse.zpad = parse.precis;
                }
                parse.ptr = System_formatNum(parse.end, (UIntMax)val, 
                        parse.zpad, -10);
                /* We know that parse.end > parse.ptr, so it is safe to use
                 * casts and assign to unsigned int. All this to avoid MISRA warnings.
                 * This same comment applies to assignments to parse.len later
                 * in this function.
                 */
                diff = parse.end - parse.ptr;
                parse.len = (unsigned int)diff;
            }
            else if (c == 'u' || c == 'x' || c == 'o') {
                UIntMax val =
                    (parse.aFlag == true) ? (UIntMax)va_arg(*pva, intptr_t) :
                    (parse.lFlag == true) ? (UIntMax)va_arg(*pva, unsigned long)
                        : (UIntMax)va_arg(*pva, unsigned);

                base = (c == 'u') ? 10 : ((c == 'x') ? 16 : 8);
                if (parse.precis > parse.zpad) {
                    parse.zpad = parse.precis;
                }
                parse.ptr = System_formatNum(parse.end, val, parse.zpad, base);
                diff = parse.end - parse.ptr;
                parse.len = (unsigned int)diff;
            }
            else if (c == 'p') {
                base = 16;
                parse.zpad = PTRZPAD;                   /* ptrs are 0 padded */
                parse.ptr = System_formatNum(
                    parse.end,
                    (parse.aFlag == true) ? (UIntMax)va_arg(*pva, intptr_t) :
                        (UIntMax)(uintptr_t)va_arg(*pva, void *), parse.zpad, base);
                parse.ptr--;
                *(parse.ptr) = '@';
                diff = parse.end - parse.ptr;
                parse.len = (unsigned int)diff;
            }
            else if (c == 'c') {
                /* character */
                *parse.ptr = (parse.aFlag == true)
                    ? (char)va_arg(*pva, intptr_t) : (char)va_arg(*pva, int);
                parse.len = 1;
            }
            else if (c == 's') {
                /* string */
                parse.ptr = (parse.aFlag == true)
                    ? (char *)Types_iargToPtr(va_arg(*pva, intptr_t))
                    : (char *)va_arg(*pva, void *);

                /* substitute (null) for NULL pointer */
                if (parse.ptr == (char *)NULL) {
                    parse.ptr = "(null)";
                }
                parse.len = (unsigned int)(strlen(parse.ptr));
                if ((parse.precis != -1) && ((unsigned int)parse.precis < parse.len)) {
                    parse.len = (unsigned int)parse.precis;
                }
            }
            else {
                fmt--;

                 /* check if enough buffer space available */
                if (n > 1U) {
                    /* parse.precis should account for the buffer size. We are
                     * assuming that SIZE_MAX>=INT_MAX because it's true for
                     * the compilers we care about. A longer discussion is here:
                     * https://stackoverflow.com/questions/46508831/is-the-max-value-of-size-t-size-max-defined-relative-to-the-other-integer-type?rq=1
                    */
                    if ((parse.precis == -1) || ((size_t)parse.precis >= n)) {
                         if (n < (size_t)INT_MAX) {
                             parse.precis = (int)n;
                         }
                         else {
                             parse.precis = INT_MAX;
                         }
                    }
                    else {
                         /* Have enough space, increment to account for '\0' */
                         parse.precis++;
                    }
                    temp_res = System_printfExtend(&buf, &fmt, pva, &parse);
                    /* temp_res is how many were printed. We are losing here the
                     * info about how many characters would be printed, but that
                     * info is lost by calling various Text functions that don't
                     * report how many characters would be printed if there were
                     * enough space.
                     */
                    res += temp_res;
                    n = n - (size_t)temp_res;
                }
            }

            /* compute number of characters left in field */
            parse.width -= (int)parse.len;

            if (parse.lJust == false) {
                /* pad with blanks on left */
                while (--parse.width >= 0) {
                    System_putchar(&buf, ' ', &n);
                    res++;
                }
            }

            /* output number, character or string */
            while (parse.len-- != 0U) {
                System_putchar(&buf, *parse.ptr, &n);
                parse.ptr++;
                res++;
            }
            /* pad with blanks on right */
            if (parse.lJust == true) {
                while (--parse.width >= 0) {
                    System_putchar(&buf, ' ', &n);
                    res++;
                }
            }
        } /* if */
        c = *fmt;
        fmt++;
    } /* while */

    if (buf != NULL) {
        *buf = '\0';
    }

    return (res);
}

/*
 *  ======== System_formatNum ========
 *  Internal function
 *
 *  Format unsigned long number in specified base, returning pointer to
 *  converted output.
 *
 *  Note: ptr points PAST end of the buffer, and is decremented as digits
 *  are converted from right to left!
 *
 *  Note: base is negative if n is signed else n unsigned!
 *
 *  ptr  - Pointer to the end of the working buffer where the string version
 *         of the number will be placed.
 *  un   - The unsigned number to be formated
 *  base - The base to format the number into. TODO - signed?
 */
char *System_formatNum(char *ptr, UIntMax un, int zpad, int base)
{
    int i = 0;
    char sign = '\0';

    UIntMax n;
    n = un;

    if (base < 0) {
        /* handle signed long case */
        base = -base;
        if ((IntMax)n < 0) {
            n = (UIntMax)(-(IntMax)n);

            /* account for sign '-': ok since zpad is signed */
            --zpad;
            sign = '-';
        }
    }

    /* compute digits in number from right to left */
    do {
        ptr--;
        *ptr = "0123456789abcdef"[n % (unsigned int)base];
        n = n / (unsigned int)base;
        ++i;
    } while (n != 0U);

    /* pad with leading 0s on left */
    while (i < zpad) {
        ptr--;
        *ptr = '0';
        ++i;
    }

    /* add sign indicator */
    if (sign != '\0') {
        ptr--;
        *ptr = sign;
    }
    return (ptr);
}

/*
 *  ======== System_putchar ========
 *  Internal function
 *
 *  Write character `c` to the buffer and, if the buffer pointer is
 *  non-NULL, update the buffer pointer.
 *
 *  Keeps track of the number of characters written into the buffer by
 *  modifying bufsize `n`. Atmost, `n` - 1 characters are written.
 */
void System_putchar(char **bufp, char c, size_t *n)
{
    /* if the size == 1, don't write so we can '\0' terminate buffer */
    if ((*n) > 1U) {

        /* decrement n to keep track of the number of chars written */
        (*n)--;

        /*
         *  If the buffer is non-NULL, use it, otherwise call the
         *  proxy's putch function (if it is ready).
         */
        if (*bufp != NULL) {
            **bufp = c;
            (*bufp)++;
        }
        else if (SystemSupport_ready() == true) {
            SystemSupport_putch(c);
        }
        else {
            return;
        }
    }
}

/*
 *  ======== System_printfExtend ========
 *  This function processes optional extended formats of printf.
 *
 *  It returns the number of characters added to the result.
 */
int System_printfExtend(char **pbuf, const char * *pfmt,
    va_list *pva, System_ParseData *parse)
{
    int     res;
    char    c;
    bool    found = false;

    /*
     * Create a local variable 'va' to ensure that the dereference of
     * pva only occurs once.
     */
    va_list va = *pva;

    res = 0;

    c = **pfmt;
    *pfmt = *pfmt + 1;

    if (System_supportPercentF_D && c == 'f') {
        double d, tmp;
        UIntMax fract;
        int    negative;

        if (parse->aFlag == true) {
            Assert_isTrue((sizeof(float) <= sizeof(intptr_t)),
                System_A_cannotFitIntoArg);

            d = Types_argToFloat(va_arg(va, intptr_t));
        }
        else {
            d = va_arg(va, double);
        }

        if (d < 0.0) {
            d = -d;
            negative = true;
            parse->zpad--;
        }
        else {
            negative = false;
        }

        /*
         * output (error) if we can't print correct value
         */
        if (d > (double) LONG_MAX) {
            parse->ptr = "(error)";
            parse->len = 7;                /* strlen("(error)"); */
            goto end;
        }

        /* Assumes four digits after decimal point. We are using a temporary
         * double variable to force double-precision computations without
         * using --fp_mode=strict flag. See the description of that flag in
         * the compiler's doc for a further explanation.
         */
        tmp = (d - (IntMax)d) * 1e4;
        fract = (UIntMax)tmp;

        parse->ptr = System_formatNum(parse->end, fract, 4, 10);
        *(--parse->ptr) = '.';

#if 0
        /* eliminate trailing zeros */
        do {
        } while (*(--parse->end) == '0');
        ++parse->end;
#endif
        parse->len = (unsigned int)(parse->end - parse->ptr);
        /* format integer part (right to left!) */
        parse->ptr = System_formatNum(parse->ptr,
            (IntMax)d, parse->zpad - parse->len, 10);
        if (negative) {
            *(--parse->ptr) = '-';
        }

        parse->len = (unsigned int)(parse->end - parse->ptr);
        found = true;
    }

    if (found == false) {
        /* other character (like %) copy to output */
        *(parse->ptr) = c;
        parse->len = 1;
    }

    /*
     * Before returning, we must update the value of pva. We use a label here
     * so that all return points will go through this update.
     * The 'goto end' is here to ensure that there is always a reference to the
     * label (to avoid the compiler complaining).
     */
    goto end;
end:
    *pva = va;
    return (res);
}

/*
 *  Agama ROM's extern function pointer table.
 *
 *  To save flash footprint, BIOS 7 uses the ROM's System_doPrint().
 *  The ROM's System_doPrint() calls out to SystemSupport_putch(),
 *  SystemSupport_ready() and System_formatNum() through this
 *  function pointer table.
 *
 *  The Agama linker command file must provide an alias for
 *  System_doPrint() as well as bring in 'xdcRomExternFuncPtr'
 *  and place it at 0x20000104.
 */
__attribute__ ((section (".data:ROM_externFuncPtrs")))
uintptr_t ROM_externFuncPtrs[] = {
    0,
    0,
    0,
    0,
    0,
    (uintptr_t)SystemSupport_putch,
    (uintptr_t)SystemSupport_ready,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (uintptr_t)System_formatNum
};

__attribute__ ((section (".data:xdcRomExternFuncPtr")))
uintptr_t *xdcRomExternFuncPtr = ROM_externFuncPtrs;
