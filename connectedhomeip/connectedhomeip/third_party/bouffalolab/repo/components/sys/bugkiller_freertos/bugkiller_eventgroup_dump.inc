#include <stdio.h>
#include "bugkiller_eventgroup_dump.h"
#include <task.h>

extern char *event_dump_string[];

void __attribute__((section(".bugkiller_code"))) bugkiller_eventgroup_dump(void *res)
{
    if(res == NULL)
    {
        printf(event_dump_string[0]);
        return;
    }
    tskTCB_dump *pxEventGroupTCB;
    tskTCB_dump *pxEvtTCB;
    EventGroup_t *event_group_handle;
    EventBits_t event_group_value = 0 ,uxControlBits = 0,uxBitsWaitedFor = 0;
    List_t *pxList;
    List_t *evt_list;
    ListItem_t *pxListItem;
    ListItem_t const *pxListEnd;
    ListItem_t *pxNext;
    printf(event_dump_string[1]);
    event_group_handle = (EventGroup_t*)res;
    pxList = &( event_group_handle->xTasksWaitingForBits );
    if(pxList == NULL){
        printf(event_dump_string[2]);
        return;
    }
    pxListItem = listGET_HEAD_ENTRY( pxList );
    if(pxListItem == NULL){
        printf(event_dump_string[3]);
        return;
    }

    pxEventGroupTCB = ( tskTCB_dump * ) listGET_LIST_ITEM_OWNER( pxListItem );
    printf(event_dump_string[4],pxEventGroupTCB);

    /***event_group_value is the value of event_group_handle->uxEventBits***/
    /***if eventgroup handle set eventCLEAR_EVENTS_ON_EXIT_BIT TRUE,event_group_value must be 0***/
    event_group_value = event_group_handle->uxEventBits;
    printf(event_dump_string[5],event_group_value);
    pxListEnd = listGET_END_MARKER( pxList );
    while( pxListItem != pxListEnd ){
        pxNext = listGET_NEXT( pxListItem );
        /***function xEventGroupWaitBits() will store the EventBits_t value in pxListItem->xItemValue ***/
        uxBitsWaitedFor = listGET_LIST_ITEM_VALUE( pxListItem );
        printf(event_dump_string[6],pxListItem,uxBitsWaitedFor);
        uxControlBits = uxBitsWaitedFor & eventEVENT_BITS_CONTROL_BYTES;
    
        if((uxControlBits & eventWAIT_FOR_ALL_BITS) == (EventBits_t) 0 ){
            printf(event_dump_string[7]);
        }
        else{
            printf(event_dump_string[8]);
        }

        if((uxControlBits & eventCLEAR_EVENTS_ON_EXIT_BIT) == (EventBits_t) 0 ){
            printf(event_dump_string[9]);
        }
        else{
            printf(event_dump_string[10]);
        }

        if((uxControlBits & eventUNBLOCKED_DUE_TO_BIT_SET) == (EventBits_t) 0 ){
            printf(event_dump_string[11]);
        }
        else{
            printf(event_dump_string[12]);
        }
        pxListItem = pxNext;
    }
    printf(event_dump_string[13], listCURRENT_LIST_LENGTH(pxList));
    if (!listLIST_IS_EMPTY(pxList)) {
        evt_list = pxList;
        for(evt_list->pxIndex = pxList->xListEnd.pxNext; evt_list->pxIndex != (ListItem_t *)&(evt_list->xListEnd); evt_list->pxIndex = evt_list->pxIndex->pxNext) {
            pxEvtTCB = evt_list->pxIndex->pvOwner;
            printf(event_dump_string[14], pxEvtTCB->pcTaskName);
        }
    }
}


