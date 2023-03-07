/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "bl_irq.h"
#include <bl602_timer.h>
#include <utils_list.h>
#include <blog.h>
#include <hal_hwtimer.h>
#include <bl602_glb.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define HW_TIMER_CHANNEL   TIMER_CH0  
#define HW_TIMER_IRQn      TIMER_CH0_IRQn
#define HD_MS_TO_VAL       40000 

#ifdef HAL_USE_HW_TIMER
struct hw_timer_ctx {
    SemaphoreHandle_t hwtimer_mux;
    utils_dlist_t *pstqueue;
};

static void hwtimer_process(void *ctx)
{
    hw_timer_t *node;
    void (*handler)(void) = NULL;
    hw_timer_t tmpnode;
    struct hw_timer_ctx *pstctx;
    
    pstctx = (struct hw_timer_ctx *)ctx;
    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_ALL, MASK);
    TIMER_ClearIntStatus(HW_TIMER_CHANNEL, TIMER_COMP_ID_0);

    
    utils_dlist_for_each_entry(pstctx->pstqueue, node, hw_timer_t, dlist_item) {
        node->calc_time = node->calc_time + 1; 
        if (node->triggle_time <= node->calc_time) {
            handler = node->handler;
            if (node->repeat == 0) {
                memcpy(&tmpnode, node, sizeof(hw_timer_t));
                utils_dlist_del(&(node->dlist_item));
                vPortFree(node);
                node = &tmpnode;
            } else {
                node->calc_time = 0;
            }
            handler();
        }
    }

    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_COMP_0, UNMASK);
}

int hal_hwtimer_init(void)
{
    struct hw_timer_ctx *pstctx;
    TIMER_CFG_Type hw_timercfg = 
    {
        TIMER_CH0,                           /* timer channel 1 */
        TIMER_CLKSRC_XTAL,                    /* timer clock source:bus clock */
        TIMER_PRELOAD_TRIG_COMP0,            /* reaload on comaparator 0  */
        TIMER_COUNT_PRELOAD,                 /* preload when match occur */
        0,                                   /* Timer clock divison value */
        HD_MS_TO_VAL * 1 - 2,                 /* match value 0  */
        32000000,                            /* match value 1 */
        32000000,                            /* match value 2 */
        0,                                   /* preload value */
    };
    

    pstctx = pvPortMalloc(sizeof(struct hw_timer_ctx));
    if (pstctx == NULL) {
        blog_error("create ctx failed \r\n");
        return -1;
    }

    pstctx->pstqueue = pvPortMalloc(sizeof(utils_dlist_t));
    if (pstctx->pstqueue == NULL) {
        vPortFree(pstctx);
        blog_error("create queue failed \r\n");

        return -1;
    }

    pstctx->hwtimer_mux = xSemaphoreCreateMutex();
    if (pstctx->hwtimer_mux == NULL) {
        vPortFree(pstctx->pstqueue);
        vPortFree(pstctx);
        blog_error("create mux failed \r\n");

        return -1;
    }

    /* if reset here, wtd will reset too, so we will do reset in bl_sys_early_init() in bl_sys.c */
    //GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_TMR);
    TIMER_IntMask(hw_timercfg.timerCh,TIMER_INT_ALL, MASK);
    TIMER_Disable(hw_timercfg.timerCh);
    TIMER_Init(&hw_timercfg);

    INIT_UTILS_DLIST_HEAD(pstctx->pstqueue);
    bl_irq_register_with_ctx(HW_TIMER_IRQn, hwtimer_process, pstctx);
    bl_irq_enable(HW_TIMER_IRQn);
    TIMER_Enable(HW_TIMER_CHANNEL);

    return 0;
}

hw_timer_t *hal_hwtimer_create(uint32_t period, hw_t handler, int repeat)
{ 
    hw_timer_t *pstnode;
    struct hw_timer_ctx *pstctx;

    if (period <= 0 || handler == NULL || (repeat != 0 && repeat != 1)) {
        blog_error("para illegal , create timer failed \r\n");

        return NULL;
    }
    
    bl_irq_ctx_get(TIMER_CH0_IRQn, (void **)&pstctx);
    if( xSemaphoreTake(pstctx->hwtimer_mux, portMAX_DELAY) == pdTRUE ) {
        blog_info("get mux success \r\n");
    }

    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_ALL, MASK); 
     
    pstnode = pvPortMalloc(sizeof(hw_timer_t));
    pstnode->triggle_time = period;
    pstnode->repeat = repeat;
    pstnode->calc_time = 0;
    pstnode->handler = handler;
    utils_dlist_add(&(pstnode->dlist_item), pstctx->pstqueue);
    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_COMP_0, UNMASK);
    xSemaphoreGive(pstctx->hwtimer_mux);

    return pstnode;
}

int hal_hwtimer_delete(hw_timer_t *pstnode)
{
    hw_timer_t *node;
    int ret = 0;
    struct hw_timer_ctx *pstctx;
    
    bl_irq_ctx_get(TIMER_CH0_IRQn, (void **)&pstctx);
    if( xSemaphoreTake(pstctx->hwtimer_mux, portMAX_DELAY) == pdTRUE ) {
        blog_info("get mux success \r\n");
    }

    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_ALL, MASK);
    utils_dlist_for_each_entry(pstctx->pstqueue, node, hw_timer_t, dlist_item) {
        if (pstnode == node) {
            break;
        }
    }

    if (&node->dlist_item == pstctx->pstqueue) {
        blog_error("not find node \r\n");
        ret  = -1;
    }

    if (ret == 0) {
        utils_dlist_del(&(node->dlist_item));
        vPortFree(node);
    }
   
    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_COMP_0, UNMASK);
    xSemaphoreGive(pstctx->hwtimer_mux);
    return ret;
}

int hal_hwtimer_change_period(hw_timer_t *pstnode, uint32_t period)
{
    hw_timer_t *node;
    int ret = 0;
    struct hw_timer_ctx *pstctx;

    if (period <= 0) {
        blog_error("period illegal , change period failed \r\n");
        return -1;
    }

    bl_irq_ctx_get(TIMER_CH0_IRQn, (void **)&pstctx);
    if( xSemaphoreTake(pstctx->hwtimer_mux, portMAX_DELAY) == pdTRUE ) {
        blog_info("get mux success \r\n");
    }

    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_ALL, MASK);
    utils_dlist_for_each_entry(pstctx->pstqueue, node, hw_timer_t, dlist_item) {
        if (pstnode == node) {
            break;
        }
    }

    if (&node->dlist_item == pstctx->pstqueue) {
        blog_error("not find node \r\n");
        ret  = -1;
    }

    if (ret == 0) {
        node->triggle_time = period;
        node->calc_time = 0;
    }

    TIMER_IntMask(HW_TIMER_CHANNEL, TIMER_INT_COMP_0, UNMASK);
    xSemaphoreGive(pstctx->hwtimer_mux);
    return ret;
}
#else
/*HW Timer based on FreeRTOS Software timer which using mtimer on RISC-V platform*/
int hal_hwtimer_init(void)
{
    return 0;
}

hw_timer_t *hal_hwtimer_create(uint32_t period, hw_t handler, int repeat)
{
    TimerHandle_t handle;

    handle = xTimerCreate(
                "hw_timer",
                pdMS_TO_TICKS(period),
                pdTRUE,
                &handle,//use stack ptr as timer ID
                (TimerCallbackFunction_t)handler
    );
    if (handle) {
         xTimerStart(handle, portMAX_DELAY);
    }
    return (hw_timer_t*)handle;
}

int hal_hwtimer_delete(hw_timer_t *pstnode)
{
    if (pdFALSE == xTimerDelete((TimerHandle_t)pstnode, portMAX_DELAY)) {
        return -1;
    }
    return 0;
}

int hal_hwtimer_change_period(hw_timer_t *pstnode, uint32_t period)
{
    if (pdFALSE == xTimerChangePeriod( (TimerHandle_t)pstnode, pdMS_TO_TICKS(period), portMAX_DELAY)) {
        return -1;
    }
    return 0;
}

#endif
