/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_api.h"
#include "cmsis_os2.h"
#include "tfm_integ_test.h"
#include "tfm_ns_svc.h"
#include "tfm_ns_interface.h"
#ifdef TEST_FRAMEWORK_NS
#include "test/framework/test_framework_integ_test.h"
#endif
#ifdef PSA_API_TEST_NS
#include "psa_api_test.h"
#endif
#include "target_cfg.h"
#include "tfm_plat_ns.h"
#include "Driver_USART.h"
#include "device_cfg.h"
#ifdef TFM_MULTI_CORE_TOPOLOGY
#include "tfm_multi_core_api.h"
#include "tfm_ns_mailbox.h"
#endif
#include "log/tfm_assert.h"
#include "log/tfm_log.h"

/**
 * \brief Modified table template for user defined SVC functions
 *
 * \details RTX has a weak definition of osRtxUserSVC, which
 *          is overridden here
 */
#if (defined(__ARMCC_VERSION) && (__ARMCC_VERSION == 6110004))
/* Workaround needed for a bug in Armclang 6.11, more details at:
 * http://www.keil.com/support/docs/4089.htm
 */
__attribute__((section(".gnu.linkonce")))
#endif
extern void * const osRtxUserSVC[1+USER_SVC_COUNT];
       void * const osRtxUserSVC[1+USER_SVC_COUNT] = {
  (void *)USER_SVC_COUNT,

#define X(SVC_ENUM, SVC_HANDLER) (void*)SVC_HANDLER,

    /* SVC API for Services */
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    LIST_SVC_NSPM
#endif

#undef X

/*
 * (void *)user_function1,
 *  ...
 */
};

/**
 * \brief List of RTOS thread attributes
 */
#if defined(TEST_FRAMEWORK_NS) || defined(PSA_API_TEST_NS)
static uint64_t test_app_stack[(4u * 1024u) / (sizeof(uint64_t))]; /* 4KB */
static const osThreadAttr_t thread_attr = {
    .name = "test_thread",
    .stack_mem = test_app_stack,
    .stack_size = sizeof(test_app_stack),
};
#endif

/**
 * \brief Static globals to hold RTOS related quantities,
 *        main thread
 */
#if defined(TEST_FRAMEWORK_NS) || defined(PSA_API_TEST_NS)
static osThreadFunc_t thread_func;
#endif

#ifdef TFM_MULTI_CORE_TOPOLOGY
static struct ns_mailbox_queue_t ns_mailbox_queue;

static void tfm_ns_multi_core_boot(void)
{
    int32_t ret;

    LOG_MSG("Non-secure code running on non-secure core.");

    if (tfm_ns_wait_for_s_cpu_ready()) {
        LOG_MSG("Error sync'ing with secure core.");

        /* Avoid undefined behavior after multi-core sync-up failed */
        for (;;) {
        }
    }

    ret = tfm_ns_mailbox_init(&ns_mailbox_queue);
    if (ret != MAILBOX_SUCCESS) {
        LOG_MSG("Non-secure mailbox initialization failed.");

        /* Avoid undefined behavior after NS mailbox initialization failed */
        for (;;) {
        }
    }
}
#endif

/* For UART the CMSIS driver is used */
extern ARM_DRIVER_USART NS_DRIVER_STDIO;

int stdio_output_string(const unsigned char *str, uint32_t len)
{
    int32_t ret;

    ret = NS_DRIVER_STDIO.Send(str, len);
    if (ret != ARM_DRIVER_OK) {
        return 0;
    }
    /* Add a busy wait after sending. */
    while (NS_DRIVER_STDIO.GetStatus().tx_busy)
        ;

    return NS_DRIVER_STDIO.GetTxCount();
}

/**
 * \brief Platform peripherals and devices initialization.
 *        Can be overridden for platform specific initialization.
 *
 * \return  ARM_DRIVER_OK if the initialization succeeds
 */
__WEAK int32_t tfm_ns_platform_init(void)
{
    int32_t ret;

    ret = NS_DRIVER_STDIO.Initialize(NULL);
    TFM_ASSERT(ret == ARM_DRIVER_OK);

    ret = NS_DRIVER_STDIO.PowerControl(ARM_POWER_FULL);
    TFM_ASSERT(ret == ARM_DRIVER_OK);

    ret = NS_DRIVER_STDIO.Control(ARM_USART_MODE_ASYNCHRONOUS,
                                  DEFAULT_UART_BAUDRATE);
    TFM_ASSERT(ret == ARM_DRIVER_OK);

    (void)NS_DRIVER_STDIO.Control(ARM_USART_CONTROL_TX, 1);

    return ARM_DRIVER_OK;
}

/**
 * \brief Platform peripherals and devices de-initialization.
 *        Can be overridden for platform specific initialization.
 *
 * \return  ARM_DRIVER_OK if the de-initialization succeeds
 */
__WEAK int32_t tfm_ns_platform_uninit(void)
{
    int32_t ret;

    (void)NS_DRIVER_STDIO.PowerControl(ARM_POWER_OFF);

    ret = NS_DRIVER_STDIO.Uninitialize();
    TFM_ASSERT(ret == ARM_DRIVER_OK);

    return ARM_DRIVER_OK;
}

/**
 * \brief main() function
 */
#ifndef __GNUC__
__attribute__((noreturn))
#endif
int main(void)
{
#if defined(__ARM_ARCH_8_1M_MAIN__) || defined(__ARM_ARCH_8M_MAIN__)
    /* Set Main Stack Pointer limit */
    extern uint32_t Image$$ARM_LIB_STACK_MSP$$ZI$$Base;
    __set_MSPLIM((uint32_t)&Image$$ARM_LIB_STACK_MSP$$ZI$$Base);
#endif

    if (tfm_ns_platform_init() != ARM_DRIVER_OK) {
        /* Avoid undefined behavior if platform init failed */
        while(1);
    }

#ifdef TFM_MULTI_CORE_TOPOLOGY
    tfm_ns_multi_core_boot();
#endif

    (void) osKernelInitialize();

    /* Initialize the TFM NS interface */
    tfm_ns_interface_init();

#if defined(TEST_FRAMEWORK_NS)
    thread_func = test_app;
#elif defined(PSA_API_TEST_NS)
    thread_func = psa_api_test;
#endif

#if defined(TEST_FRAMEWORK_NS) || defined(PSA_API_TEST_NS)
    (void) osThreadNew(thread_func, NULL, &thread_attr);
#endif

    LOG_MSG("Non-Secure system starting...\r\n");
    (void) osKernelStart();

    /* Reached only in case of error */
    for (;;) {
    }
}
