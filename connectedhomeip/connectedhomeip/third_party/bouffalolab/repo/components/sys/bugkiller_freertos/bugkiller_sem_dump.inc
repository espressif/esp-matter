#include <stdio.h>
#include <list.h>
#include <queue.h>
#include <task.h>
#include <semphr.h>
#include "bugkiller_sem_dump.h"

extern char *sem_dump_string[];

void __attribute__((section(".bugkiller_code"))) dump_sem(void *res)
{
    Queue_t *sem = (Queue_t *)res;
    List_t *recv_list = &(sem->xTasksWaitingToReceive);
    List_t *send_list = &(sem->xTasksWaitingToSend);
    List_t *tem_list = NULL;
    TCB_t *pxTCB = NULL;
    printf(sem_dump_string[0], (void *)(sem->pcHead), (void *)(sem->pcWriteTo), sem->uxMessagesWaiting, sem->uxLength, sem->uxItemSize, sem->cRxLock, sem->cTxLock);
#if ( configUSE_TRACE_FACILITY == 1 )
    switch(sem->ucQueueType) {
        case 0:
            printf(sem_dump_string[1]);
            break;
        case 1:
            printf(sem_dump_string[2]);
            break;
        case 2:
            printf(sem_dump_string[3]);
            break;
        case 3:
            printf(sem_dump_string[4]);
            break;
        case 4:
            printf(sem_dump_string[5]);
            break;
        default:
            break;
    }
#endif
    printf(sem_dump_string[6], listCURRENT_LIST_LENGTH(recv_list));
    if (!listLIST_IS_EMPTY(recv_list)) {
        tem_list = recv_list;
        for(tem_list->pxIndex = recv_list->xListEnd.pxNext; tem_list->pxIndex != (ListItem_t *)&(tem_list->xListEnd); tem_list->pxIndex = tem_list->pxIndex->pxNext) {
            pxTCB = tem_list->pxIndex->pvOwner;
            printf(sem_dump_string[7], pxTCB->pcTaskName);
        }
    }
    printf(sem_dump_string[8], listCURRENT_LIST_LENGTH(send_list));
    if (!listLIST_IS_EMPTY(send_list)) {
        tem_list = send_list;
        for(tem_list->pxIndex = recv_list->pxIndex->pxNext; tem_list->pxIndex != (ListItem_t *)&(tem_list->xListEnd); tem_list->pxIndex = tem_list->pxIndex->pxNext) {
            pxTCB = tem_list->pxIndex->pvOwner;
            printf(sem_dump_string[9], pxTCB->pcTaskName);
        }
    }
}

void __attribute__((section(".bugkiller_code"))) bugkiller_sem_dump(void *res)
{
    if (res != NULL) {
        /* Specified semaphore*/
        dump_sem(res);
        return;
    }
}

