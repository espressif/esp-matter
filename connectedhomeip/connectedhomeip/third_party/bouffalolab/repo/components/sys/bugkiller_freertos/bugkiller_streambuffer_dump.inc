#include <stdio.h>
#include <task.h>
#include <stream_buffer.h>
#include <message_buffer.h>
#include "bugkiller_sem_dump.h"

extern char *streambuffer_dump_string[];

static size_t __attribute__((section(".bugkiller_code"))) streamBuffer_spaces_available( StreamBufferHandle_t xStreamBuffer )
{
    const StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
    size_t xSpace;

	configASSERT( pxStreamBuffer );

	xSpace = pxStreamBuffer->xLength + pxStreamBuffer->xTail;
	xSpace -= pxStreamBuffer->xHead;
	xSpace -= ( size_t ) 1;

	if( xSpace >= pxStreamBuffer->xLength )
	{
		xSpace -= pxStreamBuffer->xLength;
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}

	return xSpace;
}

static size_t __attribute__((section(".bugkiller_code"))) bytes_inBuffer( const StreamBuffer_t * const pxStreamBuffer )
{
    size_t xCount;

	xCount = pxStreamBuffer->xLength + pxStreamBuffer->xHead;
	xCount -= pxStreamBuffer->xTail;
	if ( xCount >= pxStreamBuffer->xLength )
	{
		xCount -= pxStreamBuffer->xLength;
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}

	return xCount;
}

static size_t __attribute__((section(".bugkiller_code"))) streamBuffer_bytes_available( StreamBufferHandle_t xStreamBuffer )
{
    const StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
    size_t xReturn;

	configASSERT( pxStreamBuffer );

	xReturn = bytes_inBuffer( pxStreamBuffer );
	return xReturn;
}

static BaseType_t __attribute__((section(".bugkiller_code"))) streambuffer_is_full(StreamBufferHandle_t xStreamBuffer)
{
    BaseType_t xReturn;
    size_t xBytesToStoreMessageLength;
    const StreamBuffer_t * const pxStreamBuffer = ( StreamBuffer_t * ) xStreamBuffer; 

	configASSERT( pxStreamBuffer );

	if( ( pxStreamBuffer->ucFlags & sbFLAGS_IS_MESSAGE_BUFFER ) != ( uint8_t ) 0 )
	{
		xBytesToStoreMessageLength = sbBYTES_TO_STORE_MESSAGE_LENGTH;
	}
	else
	{
		xBytesToStoreMessageLength = 0;
	}

	if( streamBuffer_spaces_available( xStreamBuffer ) <= xBytesToStoreMessageLength )
	{
		xReturn = pdTRUE;
	}
	else
	{
		xReturn = pdFALSE;
	}

	return xReturn;
}

static BaseType_t __attribute__((section(".bugkiller_code"))) streambuffer_is_empty( StreamBufferHandle_t xStreamBuffer )
{
    const StreamBuffer_t * const pxStreamBuffer = ( StreamBuffer_t * ) xStreamBuffer; 
    BaseType_t xReturn;
    size_t xTail;

	configASSERT( pxStreamBuffer );

	xTail = pxStreamBuffer->xTail;
	if( pxStreamBuffer->xHead == xTail )
	{
		xReturn = pdTRUE;
	}
	else
	{
		xReturn = pdFALSE;
	}

	return xReturn;
}

static size_t __attribute__((section(".bugkiller_code"))) readbytesfrombuffer( StreamBuffer_t *pxStreamBuffer, uint8_t *pucData, size_t xMaxCount, size_t xBytesAvailable )
{
size_t xCount, xFirstLength, xNextTail;

	xCount = configMIN( xBytesAvailable, xMaxCount );

	if( xCount > ( size_t ) 0 )
	{
		xNextTail = pxStreamBuffer->xTail;

		xFirstLength = configMIN( pxStreamBuffer->xLength - xNextTail, xCount );

		configASSERT( xFirstLength <= xMaxCount );
		configASSERT( ( xNextTail + xFirstLength ) <= pxStreamBuffer->xLength );
		( void ) memcpy( ( void * ) pucData, ( const void * ) &( pxStreamBuffer->pucBuffer[ xNextTail ] ), xFirstLength ); 

		if( xCount > xFirstLength )
		{
			configASSERT( xCount <= xMaxCount );
			( void ) memcpy( ( void * ) &( pucData[ xFirstLength ] ), ( void * ) ( pxStreamBuffer->pucBuffer ), xCount - xFirstLength ); 
		}
		else
		{
			mtCOVERAGE_TEST_MARKER();
		}

		xNextTail += xCount;

		if( xNextTail >= pxStreamBuffer->xLength )
		{
			xNextTail -= pxStreamBuffer->xLength;
		}

		pxStreamBuffer->xTail = xNextTail;
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}

	return xCount;
}

static size_t __attribute__((section(".bugkiller_code"))) streambuffer_nextmsg_len( StreamBufferHandle_t xStreamBuffer )
{
    StreamBuffer_t * const pxStreamBuffer = xStreamBuffer;
    size_t xReturn, xBytesAvailable, xOriginalTail;
    configMESSAGE_BUFFER_LENGTH_TYPE xTempReturn;

	configASSERT( pxStreamBuffer );

	if( ( pxStreamBuffer->ucFlags & sbFLAGS_IS_MESSAGE_BUFFER ) != ( uint8_t ) 0 )
	{
		xBytesAvailable = bytes_inBuffer( pxStreamBuffer );
		if( xBytesAvailable > sbBYTES_TO_STORE_MESSAGE_LENGTH )
		{
			xOriginalTail = pxStreamBuffer->xTail;
			( void ) readbytesfrombuffer( pxStreamBuffer, ( uint8_t * ) &xTempReturn, sbBYTES_TO_STORE_MESSAGE_LENGTH, xBytesAvailable );
			xReturn = ( size_t ) xTempReturn;
			pxStreamBuffer->xTail = xOriginalTail;
		}
		else
		{
			configASSERT( xBytesAvailable == 0 );
			xReturn = 0;
		}
	}
	else
	{
		xReturn = 0;
	}

	return xReturn;
}

void __attribute__((section(".bugkiller_code"))) dump_streambuffer(void *res)
{
    StreamBufferHandle_t sb;
    sb = (StreamBufferHandle_t)res;
    TaskHandle_t waitingrecv_tcb = NULL;
    TaskHandle_t waitingtosend_tcb = NULL;
    waitingrecv_tcb = sb->xTaskWaitingToReceive;
    waitingtosend_tcb = sb->xTaskWaitingToSend;
    char ismb     = 'N';
    char isstatic = 'N';
    char isfull   = 'N';
    char isempty  = 'N';
    size_t spaceavailable = streamBuffer_spaces_available(sb);
    size_t bytesavailable = streamBuffer_bytes_available(sb);
    if (sb->ucFlags & sbFLAGS_IS_MESSAGE_BUFFER) {
        ismb = 'Y';
    }
    if (sb->ucFlags & sbFLAGS_IS_STATICALLY_ALLOCATED) {
        isstatic = 'Y';
    }
    
    if (streambuffer_is_full(sb) == pdTRUE) {
        isfull = 'Y';
    }
    if (streambuffer_is_empty(sb) == pdTRUE) {
        isempty = 'Y';
    }
    printf(streambuffer_dump_string[0], sb->pucBuffer, sb->xLength, bytesavailable, spaceavailable, sb->xTail, sb->xHead, sb->xTriggerLevelBytes, ismb, isstatic, isfull, isempty);
    if (sb->ucFlags & sbFLAGS_IS_MESSAGE_BUFFER) {
        size_t nextlen = streambuffer_nextmsg_len(sb);
        printf(streambuffer_dump_string[1], nextlen);
    }
    if (waitingrecv_tcb == NULL) {
        printf(streambuffer_dump_string[2]);
    } else {
        printf(streambuffer_dump_string[3], waitingrecv_tcb->pcTaskName);
    }
    if (waitingtosend_tcb == NULL) {
        printf(streambuffer_dump_string[4]);
    } else {
        printf(streambuffer_dump_string[5], waitingtosend_tcb->pcTaskName);
    }
}

void __attribute__((section(".bugkiller_code"))) bugkiller_streambuffer_dump(void *res)
{
    if (res != NULL) {
        dump_streambuffer(res);
    }
}
