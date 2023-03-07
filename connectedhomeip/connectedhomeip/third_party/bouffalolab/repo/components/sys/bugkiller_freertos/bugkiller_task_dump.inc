#include <stdio.h>

#if ( configUSE_TRACE_FACILITY == 1 )

static char * __attribute__((section(".bugkiller_code"))) write_name_to_buffer( char *pcBuffer, const char *pcTaskName )
{
    size_t x;

    /* Start by copying the entire string. */
    strcpy( pcBuffer, pcTaskName );

    /* Pad the end of the string with spaces to ensure columns line up when
    printed out. */
    for( x = strlen( pcBuffer ); x < ( size_t ) ( configMAX_TASK_NAME_LEN - 1 ); x++ )
    {
        pcBuffer[ x ] = ' ';
    }

    /* Terminate. */
    pcBuffer[ x ] = ( char ) 0x00;

    /* Return the new end of string. */
    return &( pcBuffer[ x ] );
}

static eTaskState __attribute__((section(".bugkiller_code"))) task_get_state( TaskHandle_t xTask )
{
	eTaskState eReturn;
	List_t const * pxStateList, *pxDelayedList, *pxOverflowedDelayedList;
	const TCB_t * const pxTCB = xTask;

    configASSERT( pxTCB );

    if( pxTCB == pxCurrentTCB )
    {
        /* The task calling this function is querying its own state. */
        eReturn = eRunning;
    }
    else
    {
        pxStateList = listLIST_ITEM_CONTAINER( &( pxTCB->xStateListItem ) );
        pxDelayedList = pxDelayedTaskList;
        pxOverflowedDelayedList = pxOverflowDelayedTaskList;

        if( ( pxStateList == pxDelayedList ) || ( pxStateList == pxOverflowedDelayedList ) )
        {
            /* The task being queried is referenced from one of the Blocked
            lists. */
            eReturn = eBlocked;
        }

        #if ( INCLUDE_vTaskSuspend == 1 )
            else if( pxStateList == &xSuspendedTaskList )
            {
                /* The task being queried is referenced from the suspended
                list.  Is it genuinely suspended or is it blocked
                indefinitely? */
                if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) == NULL )
                {
                    #if( configUSE_TASK_NOTIFICATIONS == 1 )
                    {
                        /* The task does not appear on the event list item of
                        and of the RTOS objects, but could still be in the
                        blocked state if it is waiting on its notification
                        rather than waiting on an object. */
                        if( pxTCB->ucNotifyState == taskWAITING_NOTIFICATION )
                        {
                            eReturn = eBlocked;
                        }
                        else
                        {
                            eReturn = eSuspended;
                        }
                    }
                    #else
                    {
                        eReturn = eSuspended;
                    }
                    #endif
                }
                else
                {
                    eReturn = eBlocked;
                }
            }
        #endif

        #if ( INCLUDE_vTaskDelete == 1 )
            else if( ( pxStateList == &xTasksWaitingTermination ) || ( pxStateList == NULL ) )
            {
                /* The task being queried is referenced from the deleted
                tasks list, or it is not referenced from any lists at
                all. */
                eReturn = eDeleted;
            }
        #endif

        else /*lint !e525 Negative indentation is intended to make use of pre-processor clearer. */
        {
            /* If the task is not in any other state, it must be in the
            Ready (including pending ready) state. */
            eReturn = eReady;
        }
    }

    return eReturn;
} /*lint !e818 xTask cannot be a pointer to const because it is a typedef. */

static configSTACK_DEPTH_TYPE __attribute__((section(".bugkiller_code"))) task_check_free_stackspace( const uint8_t * pucStackByte )
{
    uint32_t ulCount = 0U;

    while( *pucStackByte == ( uint8_t ) tskSTACK_FILL_BYTE )
    {
        pucStackByte -= portSTACK_GROWTH;
        ulCount++;
    }

    ulCount /= ( uint32_t ) sizeof( StackType_t ); /*lint !e961 Casting is not redundant on smaller architectures. */

    return ( configSTACK_DEPTH_TYPE ) ulCount;
}

static void __attribute__((section(".bugkiller_code"))) task_get_info( TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState )
{
	TCB_t *pxTCB;

    /* xTask is NULL then get the state of the calling task. */
    pxTCB = prvGetTCBFromHandle( xTask );

    pxTaskStatus->xHandle = ( TaskHandle_t ) pxTCB;
    pxTaskStatus->pcTaskName = ( const char * ) &( pxTCB->pcTaskName [ 0 ] );
    pxTaskStatus->uxCurrentPriority = pxTCB->uxPriority;
    pxTaskStatus->pxStackBase = pxTCB->pxStack;
    pxTaskStatus->xTaskNumber = pxTCB->uxTCBNumber;

    #if ( configUSE_MUTEXES == 1 )
    {
        pxTaskStatus->uxBasePriority = pxTCB->uxBasePriority;
    }
    #else
    {
        pxTaskStatus->uxBasePriority = 0;
    }
    #endif

    #if ( configGENERATE_RUN_TIME_STATS == 1 )
    {
        pxTaskStatus->ulRunTimeCounter = pxTCB->ulRunTimeCounter;
    }
    #else
    {
        pxTaskStatus->ulRunTimeCounter = 0;
    }
    #endif

    /* Obtaining the task state is a little fiddly, so is only done if the
    value of eState passed into this function is eInvalid - otherwise the
    state is just set to whatever is passed in. */
    if( eState != eInvalid )
    {
        if( pxTCB == pxCurrentTCB )
        {
            pxTaskStatus->eCurrentState = eRunning;
        }
        else
        {
            pxTaskStatus->eCurrentState = eState;

            #if ( INCLUDE_vTaskSuspend == 1 )
            {
                /* If the task is in the suspended list then there is a
                chance it is actually just blocked indefinitely - so really
                it should be reported as being in the Blocked state. */
                if( eState == eSuspended )
                {
                    if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
                    {
                        pxTaskStatus->eCurrentState = eBlocked;
                    }
                }
            }
            #endif /* INCLUDE_vTaskSuspend */
        }
    }
    else
    {
        pxTaskStatus->eCurrentState = task_get_state( pxTCB );
    }

    /* Obtaining the stack space takes some time, so the xGetFreeStackSpace
    parameter is provided to allow it to be skipped. */
    if( xGetFreeStackSpace != pdFALSE )
    {
        #if ( portSTACK_GROWTH > 0 )
        {
            pxTaskStatus->usStackHighWaterMark = task_check_free_stackspace( ( uint8_t * ) pxTCB->pxEndOfStack );
        }
        #else
        {
            pxTaskStatus->usStackHighWaterMark = task_check_free_stackspace( ( uint8_t * ) pxTCB->pxStack );
        }
        #endif
    }
    else
    {
        pxTaskStatus->usStackHighWaterMark = 0;
    }
}

static UBaseType_t __attribute__((section(".bugkiller_code"))) list_tasks_within_singlelist( TaskStatus_t *pxTaskStatusArray, List_t *pxList, eTaskState eState )
{
    configLIST_VOLATILE TCB_t *pxNextTCB, *pxFirstTCB;
    UBaseType_t uxTask = 0;

    if( listCURRENT_LIST_LENGTH( pxList ) > ( UBaseType_t ) 0 )
    {
        listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList ); /*lint !e9079 void * is used as this macro is used with timers and co-routines too.  Alignment is known to be fine as the type of the pointer stored and retrieved is the same. */

        /* Populate an TaskStatus_t structure within the
        pxTaskStatusArray array for each task that is referenced from
        pxList.  See the definition of TaskStatus_t in task.h for the
        meaning of each TaskStatus_t structure member. */
        do
        {
            listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList ); /*lint !e9079 void * is used as this macro is used with timers and co-routines too.  Alignment is known to be fine as the type of the pointer stored and retrieved is the same. */
            task_get_info( ( TaskHandle_t ) pxNextTCB, &( pxTaskStatusArray[ uxTask ] ), pdTRUE, eState );
            uxTask++;
        } while( pxNextTCB != pxFirstTCB );
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    return uxTask;
}


static UBaseType_t __attribute__((section(".bugkiller_code"))) task_getsystem_state( TaskStatus_t * const pxTaskStatusArray, const UBaseType_t uxArraySize, uint32_t * const pulTotalRunTime )
{
	UBaseType_t uxTask = 0, uxQueue = configMAX_PRIORITIES;

    /* Is there a space in the array for each task in the system? */
    if( uxArraySize >= uxCurrentNumberOfTasks )
    {
        /* Fill in an TaskStatus_t structure with information on each
        task in the Ready state. */
        do
        {
            uxQueue--;
            uxTask += list_tasks_within_singlelist( &( pxTaskStatusArray[ uxTask ] ), &( pxReadyTasksLists[ uxQueue ] ), eReady );

        } while( uxQueue > ( UBaseType_t ) tskIDLE_PRIORITY ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */

        /* Fill in an TaskStatus_t structure with information on each
        task in the Blocked state. */
        uxTask += list_tasks_within_singlelist( &( pxTaskStatusArray[ uxTask ] ), ( List_t * ) pxDelayedTaskList, eBlocked );
        uxTask += list_tasks_within_singlelist( &( pxTaskStatusArray[ uxTask ] ), ( List_t * ) pxOverflowDelayedTaskList, eBlocked );

        #if( INCLUDE_vTaskDelete == 1 )
        {
            /* Fill in an TaskStatus_t structure with information on
            each task that has been deleted but not yet cleaned up. */
            uxTask += list_tasks_within_singlelist( &( pxTaskStatusArray[ uxTask ] ), &xTasksWaitingTermination, eDeleted );
        }
        #endif

        #if ( INCLUDE_vTaskSuspend == 1 )
        {
            /* Fill in an TaskStatus_t structure with information on
            each task in the Suspended state. */
            uxTask += list_tasks_within_singlelist( &( pxTaskStatusArray[ uxTask ] ), &xSuspendedTaskList, eSuspended );
        }
        #endif
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    return uxTask;
}

extern char *task_dump_string[];

void __attribute__((section(".bugkiller_code"))) bugkiller_task_dump()
{
    char cStatus;
    char pcWriteBuffer[100];
    //const char *const pcHeader = "Task         State   Priority  Stack    #          Base\r\n********************************************************\r\n";
    TaskStatus_t pxTaskStatusArray[uxCurrentNumberOfTasks * sizeof( TaskStatus_t )];
    UBaseType_t uxArraySize, x;
    TCB_t *pxTCB = NULL;

    printf(task_dump_string[0]);
    uxArraySize = uxCurrentNumberOfTasks;

    uxArraySize = task_getsystem_state( pxTaskStatusArray, uxArraySize, NULL );

    for( x = 0; x < uxArraySize; x++ ) {
        switch( pxTaskStatusArray[ x ].eCurrentState )
        {
            case eRunning:      cStatus = tskRUNNING_CHAR;
                                break;

            case eReady:        cStatus = tskREADY_CHAR;
                                break;

            case eBlocked:      cStatus = tskBLOCKED_CHAR;
                                break;

            case eSuspended:    cStatus = tskSUSPENDED_CHAR;
                                break;

            case eDeleted:      cStatus = tskDELETED_CHAR;
                                break;

            case eInvalid:      /* Fall through. */
            default:            /* Should not get here, but it is included
                                to prevent static checking errors. */
                                cStatus = ( char ) 0x00;
                                break;
        }

        memset(pcWriteBuffer, 0, 100);
        write_name_to_buffer( pcWriteBuffer, pxTaskStatusArray[ x ].pcTaskName );
        printf(task_dump_string[1], pcWriteBuffer);

#if ( INCLUDE_xTaskGetHandle == 1 )
	char * pcNameToQuery = pxTaskStatusArray[ x ].pcTaskName;
        UBaseType_t uxQueue = configMAX_PRIORITIES;

        /* Task names will be truncated to configMAX_TASK_NAME_LEN - 1 bytes. */
        configASSERT( strlen( pcNameToQuery ) < configMAX_TASK_NAME_LEN );

        {
            /* Search the ready lists. */
            do
            {
                uxQueue--;
                pxTCB = prvSearchForNameWithinSingleList( ( List_t * ) &( pxReadyTasksLists[ uxQueue ] ), pcNameToQuery );

                if( pxTCB != NULL )
                {
                    /* Found the handle. */
                    break;
                }
            } while( uxQueue > ( UBaseType_t ) tskIDLE_PRIORITY ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */

            /* Search the delayed lists. */
            if( pxTCB == NULL )
            {
                pxTCB = prvSearchForNameWithinSingleList( ( List_t * ) pxDelayedTaskList, pcNameToQuery );
            }

            if( pxTCB == NULL )
            {
                pxTCB = prvSearchForNameWithinSingleList( ( List_t * ) pxOverflowDelayedTaskList, pcNameToQuery );
            }

            #if ( INCLUDE_vTaskSuspend == 1 )
                {
                    if( pxTCB == NULL )
                    {
                        /* Search the suspended list. */
                        pxTCB = prvSearchForNameWithinSingleList( &xSuspendedTaskList, pcNameToQuery );
                    }
                }
            #endif

            #if ( INCLUDE_vTaskDelete == 1 )
                {
                    if( pxTCB == NULL )
                    {
                        /* Search the deleted list. */
                        pxTCB = prvSearchForNameWithinSingleList( &xTasksWaitingTermination, pcNameToQuery );
                    }
                }
            #endif
        }

        sprintf( pcWriteBuffer, task_dump_string[3], cStatus, ( unsigned int ) pxTaskStatusArray[ x ].uxCurrentPriority, ( unsigned int ) pxTaskStatusArray[ x ].usStackHighWaterMark, ( unsigned int ) pxTaskStatusArray[ x ].xTaskNumber, pxTaskStatusArray[ x ].pxStackBase, pxTCB, pxTCB->pxTopOfStack, *( unsigned int * )pxTCB->pxTopOfStack );
#else
        sprintf( pcWriteBuffer, task_dump_string[3], cStatus, ( unsigned int ) pxTaskStatusArray[ x ].uxCurrentPriority, ( unsigned int ) pxTaskStatusArray[ x ].usStackHighWaterMark, ( unsigned int ) pxTaskStatusArray[ x ].xTaskNumber, pxTaskStatusArray[ x ].pxStackBase, 0, 0, 0);
#endif /* INCLUDE_xTaskGetHandle */

        printf(task_dump_string[2], pcWriteBuffer);
    }
}

#endif /* configUSE_TRACE_FACILITY */
