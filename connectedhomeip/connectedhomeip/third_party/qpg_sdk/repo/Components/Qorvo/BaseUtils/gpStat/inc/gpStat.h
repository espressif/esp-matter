
/*
 * Copyright (c) 2009-2010, 2012, 2014-2016, GreenPeak Technologies
 * Copyright (c) 2017-2019, Qorvo Inc
 *
 * gpStat.h
 *
 * This file defines the generic statistics component api
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * Alternatively, this software may be distributed under the terms of the
 * modified BSD License or the 3-clause BSD License as published by the Free
 * Software Foundation @ https://directory.fsf.org/wiki/License:BSD-3-Clause
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */


#ifndef _GPSTAT_H_
#define _GPSTAT_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifdef GP_FILENAME
#if defined(GP_DIVERSITY_LOG) 
//Define a FileName only once for use - name comes from make environment
static const char ROM gpStatFileName[] FLASH_PROGMEM = { XSTRINGIFY(GP_FILENAME) };
#define GP_STAT_FILENAME gpStatFileName
#endif //GP_DIVERSITY_LOG || defined(GP_STAT_DIVERSITY_GPCOM_SERVER)
#else
#define GP_STAT_FILENAME __FILE__
#endif //GP_FILENAME

#ifndef GP_STAT_NUMBER_OF_COUNTERS
#define GP_STAT_NUMBER_OF_COUNTERS      5
#endif

#ifndef GP_STAT_NUMBER_OF_MINMAX
#define GP_STAT_NUMBER_OF_MINMAX        5
#endif

#ifndef GP_STAT_NUMBER_OF_STOPWATCHES
#define GP_STAT_NUMBER_OF_STOPWATCHES   5
#endif
#ifndef GP_STAT_NUMBER_OF_SAMPLES
#define GP_STAT_NUMBER_OF_SAMPLES       10
#endif
#ifndef GP_STAT_NUMBER_OF_HEAP_SAMPLES
#define GP_STAT_NUMBER_OF_HEAP_SAMPLES  1
#endif

#define GP_STAT_MAX_NAME_CHARS          20
#define GP_STAT_MAX_NAME_LENGTH         10



#define GP_STAT_INIT()
#define GP_STAT_RESET_COUNTER(id)
#define GP_STAT_START_STOPWATCH(id)
#define GP_STAT_STOP_STOPWATCH(id)                                      0

#define GP_STAT_CLEAR()

#define GP_STAT_REGISTER_COUNTER(id,name,incrementStep)
#define GP_STAT_FREE_COUNTER(id)
#define GP_STAT_INCREASE_COUNTER(id, value)
#define GP_STAT_INCREMENT_COUNTER(id)
#define GP_STAT_GET_COUNTER(id)                                         0

#define GP_STAT_DECLARE_COUNTERID(name)                                 void gpStat_NonExistent(void) /*Declare something unused - syntax error when only ; on line*/

#define GP_STAT_REGISTER_MINMAX(id , name)
#define GP_STAT_FREE_MINMAX(id)
#define GP_STAT_SET_MINMAX( id ,  value)                                false
#define GP_STAT_GET_COUNT( id )                                         0
#define GP_STAT_GET_MIN( id )                                           0
#define GP_STAT_GET_MAX( id  )                                          0
#define GP_STAT_GET_SUM( id  )                                          0
#define GP_STAT_GET_AVERAGE( id  )                                      0
#define GP_STAT_GET_VARIANCE( id  )                                     0
#define GP_STAT_RESET_MINMAX( id )

#define GP_STAT_DECLARE_MINMAXID(name)                                  void gpStat_NonExistent(void) /*Declare something unused - syntax error when only ; on line*/

#define GP_STAT_REGISTER_STOPWATCH(id , name)
#define GP_STAT_FREE_STOPWATCH(id)

#define GP_STAT_DECLARE_STOPWATCHID(name)                               void gpStat_NonExistent(void) /*Declare something unused - syntax error when only ; on line*/

#define GP_STAT_SAMPLE_TIME()
#define GP_STAT_RETURN_SAMPLING()

#define GP_STAT_SAMPLE_HEAP(heapInUse)
#define GP_STAT_RETURN_SAMPLING_HEAP()


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt8 gpStat_CounterID_t;
typedef UInt8 gpStat_MinMaxID_t;
typedef UInt8 gpStat_StopwatchID_t;

// Main statistic counter type
typedef struct gpStat_StatisticsCounter {
    UInt32  counter;
    char    name[GP_STAT_MAX_NAME_LENGTH];
    Int8    incrementStep;
} gpStat_StatisticsCounter_t;

// Main statistic min max type
typedef struct gpStat_StatisticsMinMax {
    UInt16  count;
    UInt32  min;
    UInt32  max;
    UInt32  sum;
    UInt32  sum2; //sum of square values
    char    name[GP_STAT_MAX_NAME_LENGTH];
} gpStat_StatisticsMinMax_t;

// Main statistic min max type
typedef struct gpStat_StatisticsStopwatch {
    UInt32  time;
    char    name[GP_STAT_MAX_NAME_LENGTH];
} gpStat_StatisticsStopwatch_t;


// Serial packet typedefs
#define gpStat_CommandIDGeneralStatisticsIndication        0x02
typedef UInt8 gpStat_CommandID_t;

#if GP_STAT_NUMBER_OF_COUNTERS > 0
typedef struct gpStat_StatisticInformation {
    /*All counters available in gpStat*/
    UInt32 counters[GP_STAT_NUMBER_OF_COUNTERS];
} gpStat_StatisticInformation_t;

typedef struct gpStat_StatisticInformationPacket{
    gpStat_CommandID_t  commandID;
    UInt16              appID;
    gpStat_StatisticInformation_t  statisticInformation;
}gpStat_StatisticInformationPacket_t;
#endif

typedef struct {
    UInt32 time;
    UInt16 line;
    UInt8 compId;
    char funcName[GP_STAT_MAX_NAME_CHARS];
    char fileName[GP_STAT_MAX_NAME_CHARS];
} gpStat_Sample_t;

typedef struct {
    UInt32 time;
    UInt32 heapInUse;
} gpStat_HeapSample_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void   gpStat_Init(void);
void   gpStat_Clear(void);

gpStat_CounterID_t      gpStat_RegisterCounter(FLASH_STRING name, Int8 incrementStep);
void                    gpStat_FreeCounter(gpStat_CounterID_t id);
void                    gpStat_IncreaseCounter(gpStat_CounterID_t id, Int32 value);
void                    gpStat_IncrementCounter(gpStat_CounterID_t id);
UInt32                  gpStat_GetCounter(gpStat_CounterID_t id);
void                    gpStat_ResetCounter(gpStat_CounterID_t id);


gpStat_MinMaxID_t       gpStat_RegisterMinMax(FLASH_STRING name);
void                    gpStat_FreeMinMax(gpStat_MinMaxID_t id);
Bool                    gpStat_SetMinMax(gpStat_MinMaxID_t id , UInt32 value);
UInt32                  gpStat_GetCount(gpStat_MinMaxID_t id);
UInt32                  gpStat_GetMin(gpStat_MinMaxID_t id);
UInt32                  gpStat_GetMax(gpStat_MinMaxID_t id);
UInt32                  gpStat_GetSum(gpStat_MinMaxID_t id);
UInt32                  gpStat_GetAverage(gpStat_MinMaxID_t id);
UInt32                  gpStat_GetVariance(gpStat_MinMaxID_t id);
void                    gpStat_ResetMinMax(gpStat_MinMaxID_t id);

gpStat_StopwatchID_t    gpStat_RegisterStopwatch(FLASH_STRING name);
void                    gpStat_FreeStopwatch(gpStat_StopwatchID_t id);
void                    gpStat_StartStopwatch(gpStat_StopwatchID_t id);
UInt32                  gpStat_StopStopwatch(gpStat_StopwatchID_t id);

void                    gpStat_SampleTime(UInt8 componentID, FLASH_STRING filename, FLASH_STRING funcName, UInt16 line);
void                    gpStat_ReturnSamples(void);
void                    gpStat_cbReturnSample(gpStat_Sample_t sample);

void                    gpStat_SampleHeap(UInt32 heapInUse);
void                    gpStat_ReturnSamplesHeap(void);
void                    gpStat_cbReturnHeapSample(gpStat_HeapSample_t sample);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif // _GPSTAT_H_


