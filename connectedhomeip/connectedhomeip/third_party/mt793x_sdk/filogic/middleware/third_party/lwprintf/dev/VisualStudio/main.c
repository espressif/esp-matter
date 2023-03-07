#include <stdio.h>
#include "lwprintf/lwprintf.h"
#include <string.h>
#include <stdint.h>
#include "windows.h"

typedef struct {
    char* format;                   /*!< Input format */
    char* input_data;               /*!< Input parameters */
    char* p_orig;                   /*!< Result with built-in library */
    size_t c_orig;                  /*!< Count returned by original lib */
    char* p_lwpr;                   /*!< Result with lwprintf library */
    size_t c_lwpr;                  /*!< Count returned by lwprintf lib */
    char* e_resu;                   /*!< Expected result (if used) */
    uint8_t pass;                   /*!< Status if test is pass or fail */
} test_data_t;

/* Array of tests */
static test_data_t
tests[500];

/* Number of tests done so far */
static size_t tests_cnt;

/**
 * \brief           Output function for lwprintf printf function
 * \param[in]       ch: Character to print
 * \param[in]       lw: LwPRINTF instance
 * \return          `ch` value on success, `0` otherwise
 */
int
lwprintf_output(int ch, lwprintf_t* lw) {
    if (ch != '\0') {
        printf("%c", (char)ch);
    }
    return ch;
}

/**
 * \brief           Tests numbers
 */
size_t tests_passed, tests_failed;

#define MY_MACRO " "
const char* text_fmt, *text_params;
#define printf_run(exp, fmt, ...) do {          \
text_fmt = fmt;                               \
text_params = # __VA_ARGS__;                    \
printf_run_fn(exp, fmt, ## __VA_ARGS__);        \
} while (0);

/**
 * \brief           Run printf with built-in and custom implementation.
 * Compare results on returned length and actual content
 *
 * \param[in]       expected: Expected result
 * \param[in]       fmt: Format to use
 * \param[in]       ...: Optional parameters
 */
static void
printf_run_fn(const char* expected, const char* fmt, ...) {
    HANDLE console;
    va_list va;
    test_data_t* test;
    
    /* Temporary strings array */
    char b1[255] = { 0 }, b2[255] = { 0 };
    int l1, l2;

    console = GetStdHandle(STD_OUTPUT_HANDLE);  /* Get console */

    /* Generate strings with original and custom printf */
    va_start(va, fmt);
    l1 = vsnprintf(b1, sizeof(b1), fmt, va);
    l2 = lwprintf_vsnprintf(b2, sizeof(b2), fmt, va);
    va_end(va);

    /* Get test handle */
    test = &tests[tests_cnt++];

    /* Dynamic allocation, we assume if didn't fail anywhere for the sake of example... */
    test->p_orig = malloc(sizeof(char) * (strlen(b1) + 1));
    test->p_lwpr = malloc(sizeof(char) * (strlen(b2) + 1));
    test->format = malloc(sizeof(char) * (strlen(fmt) + 1));
    test->input_data = malloc(sizeof(char) * (strlen(text_params) + 1));
    
    /* Copy data */
    strcpy(test->p_orig, b1);
    strcpy(test->p_lwpr, b2);
    strcpy(test->format, fmt);
    strcpy(test->input_data, text_params);
    test->c_orig = l1;
    test->c_lwpr = l2;

    /* Expected result is optional */
    if (expected != NULL && strlen(expected) > 0) {
        test->e_resu = malloc(sizeof(char) * (strlen(expected) + 1));
        strcpy(test->e_resu, expected);
    }

    /* Check for pass */
    /* When expected parameter is used, then compare expected data vs generated data only */
    if (expected != NULL) {
        test->pass = !strcmp(expected, b2);
    } else {
        test->pass = !(strcmp(b1, b2) || l1 != l2);
    }
    if (test->pass) {
        ++tests_passed;
    } else {
        ++tests_failed;
    }

/*
    printf("Format: \"%s\"\r\n", fmt);
    printf("R: Len: %3d, result: \"%s\"\r\n", l1, b1);
    printf("L: Len: %3d, result: \"%s\"\r\n", l2, b2);
    if (strcmp(b1, b2) || l1 != l2) {
        SetConsoleTextAttribute(console, FOREGROUND_RED);
        printf("Test failed!\r\n");
        ++tests_failed;
    } else {
        SetConsoleTextAttribute(console, FOREGROUND_GREEN);
        printf("Test passed!\r\n");
        ++tests_passed;
    }
    SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    printf("----\r\n");
*/
}

void
output_test_result(test_data_t* t) {
    printf("----\n");
    printf("Format: \"%s\"\n", t->format);
    printf("Params: \"%s\"\n", t->input_data);
    if (t->e_resu != NULL) {
        printf("Result expected: \"%s\"\nLength expected: %d\n", t->e_resu, (int)strlen(t->e_resu));
    } else {
        printf("Result VSprintf: \"%s\"\nLength VSprintf: %d\n", t->p_orig, (int)t->c_orig);
    }
    printf("Result LwPRINTF: \"%s\"\nLength LwPRINTF: %d\n", t->p_lwpr, (int)t->c_lwpr);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), t->pass ? FOREGROUND_GREEN : FOREGROUND_RED);
    printf("Test result: %s\n", t->pass ? "Pass" : "Fail");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

int n;
int
main(void) {
    double num = 2123213213142.032;
    
    lwprintf_init(lwprintf_output);

    printf_run(NULL, "Precision: %3d, %.*g", 17, 17, 0.0001234567);
    for (int i = 0; i < 20; ++i) {
        printf_run(NULL, "Precision: %3d, %20.*g", i, i, 432432423.342321321);
    }
    for (int i = 0; i < 20; ++i) {
        printf_run(NULL, "Precision: %3d, %20.*g", i, i, 0.0001234567);
    }
    printf_run(NULL, "%.4f", 3.23321321);
    printf_run(NULL, "%.45f", 3.23321321);
    printf_run(NULL, "%.4F", 3.23321321);
    printf_run(NULL, "%.45F", 3.23321321);

    printf_run(NULL, "%g", 1.23342);
    printf_run(NULL, "%g", 12334.2);
    printf_run(NULL, "%.8g", 0.000000123342);
    printf_run(NULL, "%.8G", 0.000000123342);

    printf_run(NULL, "%.4f", 323243432432432.432);

    /* Engineering tests */
    printf_run(NULL, "%e", -123.456);
    printf_run(NULL, "%e", 0.000001);
    printf_run(NULL, "%e", 0.123456);
    printf_run(NULL, "%e", -0.123456);
    printf_run(NULL, "%.4e", 123.456);
    printf_run(NULL, "%.4e", -123.456);
    printf_run(NULL, "%.4e", 0.123456);
    printf_run(NULL, "%.4e", -0.123456);
    printf_run(NULL, "%.0e", 123.456);
    printf_run(NULL, "%.0e", -123.456);
    printf_run(NULL, "%.0e", 0.123456);
    printf_run(NULL, "%.0e", -0.123456);
    printf_run(NULL, "%22.4e", 123.456);
    printf_run(NULL, "%22.4e", -123.456);
    printf_run(NULL, "%22.4e", 0.123456);
    printf_run(NULL, "%22.4e", -0.123456);
    printf_run(NULL, "%022.4e", 123.456);
    printf_run(NULL, "%022.4e", -123.456);
    printf_run(NULL, "%022.4e", 0.123456);
    printf_run(NULL, "%e", 0.00000000123456);
    printf_run(NULL, "%022.4e", -0.123456);

    /* Add zeros if unused... tbd */
    printf_run(NULL, "%22.33e", 123.456);
    printf_run(NULL, "%22.33e", -123.456);
    printf_run(NULL, "%22.33e", 0.123456);
    printf_run(NULL, "%22.33e", -0.123456);
    printf_run(NULL, "%.4E", -123.456);

    printf_run(NULL, "% 3u", (unsigned)28);
    printf_run(NULL, "% 3u", (unsigned)123456);
    printf_run(NULL, "%03d", 28);
    printf_run(NULL, "%+03d", 28);
    printf_run(NULL, "%+3d", 28);
    printf_run(NULL, "%03d", -28);
    printf_run(NULL, "%+03d", -28);
    printf_run(NULL, "%+3d", -28);
    printf_run(NULL, "%03u", (unsigned)123456);
    printf_run(NULL, "%-010uabc", (unsigned)123456);
    printf_run(NULL, "%010uabc", (unsigned)123456);
    printf_run(NULL, "%-10d", -123);
    printf_run(NULL, "%10d", -123);
    printf_run(NULL, "%-06d", -1234567);
    printf_run(NULL, "%06d", -1234567);
    printf_run(NULL, "%-10d", -1234567);
    printf_run(NULL, "%10d", -1234567);
    printf_run(NULL, "%-010d", -1234567);
    printf_run(NULL, "%010d", -1234567);
    printf_run(NULL, "%s", "This is my string");
    printf_run(NULL, "%10s", "This is my string");
    printf_run(NULL, "%0*d", 10, -123);
    printf_run(NULL, "%zu", (size_t)10);
    printf_run(NULL, "%ju", (uintmax_t)10);
    printf_run(NULL, "% d", 1024);
    printf_run(NULL, "% 4d", 1024);
    printf_run(NULL, "% 3d", 1024);
    printf_run(NULL, "% 3f", 32.687);

    /* string */
    printf_run(NULL, "%*.*s", 8, 12, "This is my string");
    printf_run(NULL, "%*.*s", 8, 12, "Stri");
    printf_run(NULL, "%-6.10s", "This is my string");
    printf_run(NULL, "%6.10s", "This is my string");
    printf_run(NULL, "%-6.10s", "This is my string");
    printf_run(NULL, "%6.10s", "Th");
    printf_run(NULL, "%-6.10s", "Th");
    printf_run(NULL, "%*.*s", -6, 10, "Th");
    printf_run(NULL, "%*.*s", 6, 10, "Th");

    printf_run(NULL, "%.4s", "This is my string");
    printf_run(NULL, "%.6s", "1234");
    printf_run(NULL, "%.4s", "stri");
    printf_run(NULL, "%.4s%.2s", "123456", "abcdef");
    printf_run(NULL, "%.4.2s", "123456");
    printf_run(NULL, "%.*s", 3, "123456");
    printf_run(NULL, "%.3s", "");
    printf_run(NULL, "%yunknown", "");

    /* Alternate form */
    printf_run(NULL, "%#2X", 123);
    printf_run(NULL, "%#2x", 123);
    printf_run(NULL, "%#2o", 123);
    printf_run(NULL, "%#2X", 1);
    printf_run(NULL, "%#2x", 1);
    printf_run(NULL, "%#2o", 1);
    printf_run(NULL, "%#2X", 0);
    printf_run(NULL, "%#2x", 0);
    printf_run(NULL, "%#2o", 0);

    /* Pointers */
    printf_run(NULL, "%p", &tests_passed);
    printf_run(NULL, "0X%p", &tests_passed);
    printf_run(NULL, "0x%p", &tests_passed);

    /* Those are additional, not supported in classic printf implementation */

    /* Binary */
    printf_run("1111011 abc", "%llb abc", 123);
    printf_run("100", "%b", 4);
    printf_run("0B1", "%#2B", 1);
    printf_run("0b1", "%#2b", 1);
    printf_run(" 0", "%#2B", 0);
    printf_run(" 0", "%#2b", 0);
    printf_run("0", "%#B", 0);
    printf_run("0", "%#b", 0);
    printf_run("0B110", "%#B", 6);
    printf_run("0b110", "%#b", 6);

    /* Array test */
    uint8_t my_arr[] = { 0x01, 0x02, 0xB5, 0xC6, 0xD7 };
    printf_run("0102B5C6D7", "%5K", my_arr);
    printf_run("0102B5", "%*K", 3, my_arr);
    printf_run("01 02 B5", "% *K", 3, my_arr);
    printf_run("0102b5c6d7", "%5k", my_arr);
    printf_run("0102b5", "%*k", 3, my_arr);
    printf_run("01 02 b5", "% *k", 3, my_arr);

    /* Print final output */
    printf("------------------------\n");
    printf("Number of tests run: %d\n", (int)(tests_passed + tests_failed));
    printf("Number of tests passed: %d\n", (int)tests_passed);
    printf("Number of tests failed: %d\n", (int)tests_failed);
    printf("Coverage: %f %%\n", (float)((tests_passed * 100) / ((float)(tests_passed + tests_failed))));

    /* Tests that failed */
    printf("------------------------\n\n");
    printf("Negative tests\n\n");
    for (size_t i = 0; i < tests_cnt; ++i) {
        test_data_t* t = &tests[i];

        if (!t->pass) {
            output_test_result(t);
        }
    }

    /* Tests that went through */
    printf("------------------------\n\n");
    printf("Positive tests\n\n");
    for (size_t i = 0; i < tests_cnt; ++i) {
        test_data_t* t = &tests[i];

        if (t->pass) {
            output_test_result(t);
        }
    }
    return 0;
}
