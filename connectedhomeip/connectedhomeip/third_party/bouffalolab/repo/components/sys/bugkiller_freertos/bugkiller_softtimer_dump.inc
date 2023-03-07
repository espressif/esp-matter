#include <stdio.h>
#include <list.h>

extern char *softtimer_dump_string[];

void __attribute__((section(".bugkiller_code"))) traversal_list(List_t * pxList)
{
    Timer_t *timer = NULL;
    List_t *timer_list = pxList;
    List_t *tem_list = pxList;
    char reload   = 'N';
    char staticc  = 'N';
    char isactive = 'N';
    printf(softtimer_dump_string[0]);
    for(tem_list->pxIndex = timer_list->xListEnd.pxNext; tem_list->pxIndex != (ListItem_t *)&(tem_list->xListEnd); tem_list->pxIndex = tem_list->pxIndex->pxNext) {
        timer = tem_list->pxIndex->pvOwner;
        if (timer->ucStatus & tmrSTATUS_IS_AUTORELOAD) {
            reload = 'Y';
        }
        if (timer->ucStatus & tmrSTATUS_IS_STATICALLY_ALLOCATED) {
            staticc = 'Y';
        }
        if (timer->ucStatus & tmrSTATUS_IS_ACTIVE) {
            isactive = 'Y';
        }
        printf(softtimer_dump_string[1], timer->pcTimerName, timer->xTimerPeriodInTicks, timer->pvTimerID, timer->pxCallbackFunction, reload, staticc, isactive);
    }
}

void __attribute__((section(".bugkiller_code"))) bugkiller_softtimer_dump(void *res)
{
    Timer_t *timer = NULL;
    char reload   = 'N';
    char staticc  = 'N';
    char isactive = 'N';
    if (res != NULL) {
        printf(softtimer_dump_string[2]);
        timer = (Timer_t *)res;
        if (timer->ucStatus & tmrSTATUS_IS_AUTORELOAD) {
            reload = 'Y';
        }
        if (timer->ucStatus & tmrSTATUS_IS_STATICALLY_ALLOCATED) {
            staticc = 'Y';
        }
        if (timer->ucStatus & tmrSTATUS_IS_ACTIVE) {
            isactive = 'Y';
        }
        printf(softtimer_dump_string[3], timer->pcTimerName, timer->xTimerPeriodInTicks, timer->pvTimerID, timer->pxCallbackFunction, reload, staticc, isactive);
    } else {
        if (!listLIST_IS_EMPTY(pxCurrentTimerList)) {
            traversal_list(pxCurrentTimerList);
        }
        if (!listLIST_IS_EMPTY(pxOverflowTimerList)) {
            traversal_list(pxOverflowTimerList);
        }
    }
}

