/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 */

/*
 *  ======== bigtime.cpp ========
 */

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

/* Example/Board Header files */
#include <ti/drivers/Board.h>

#define TASKSTACKSIZE   512

class Clock {
    private:
         // data
         int id;
         double ticks;
         int microsecond;
         int millisecond;
         int second;
         int minute;
         int hour;
         int day;
         int month;
         int year;
         int century;
         int millenium;
         Diags_Mask clockLog;

    public:
        // methods
        Clock(int newId);  // Constructor
        ~Clock();          // Destructor
        void tick();
        long getTicks();
        int getId();
        int getMicrosecond();
        int getMillisecond();
        int getSecond();
        int getMinute();
        int getHour();
        int getDay();
        int getMonth();
        int getYear();
        int getCentury();
        int getMillenium();
        void setMicrosecond();
        void setMillisecond();
        void setMillisecond(int nMilliseconds);
        void setSecond();
        void setMinute();
        void setHour();
        void setDay();
        void setMonth();
        void setYear();
        void setCentury();
        void setMillenium();
};

const char *months[12] = { "January", "February", "March",
                           "April",   "May",      "June",
                           "July",    "August",   "September",
                           "October", "November", "December" };

/*
 * Extern "C" block to prevent name mangling
 * of functions called within the Configuration
 * Tool
 */
extern "C" {

/* Wrapper function to terminate the example */
void clockTerminate(UArg arg);

/* Wrapper functions to call Clock::tick() */
void clockTask(UArg arg);
void clockPrd(UArg arg);
void clockIdle(void);

} // end extern "C"

/* Global clock objects */
Clock cl0(0);  /* idle loop clock */
Clock cl1(1);  /* periodic clock, period = 1 ms */
Clock cl2(2);  /* periodic clock, period = 1 sec */
Clock cl3(3);  /* task clock */
Clock cl4(4);  /* task clock */

Task_Struct task0Struct, task1Struct;
Char task0Stack[TASKSTACKSIZE], task1Stack[TASKSTACKSIZE];
Semaphore_Struct sem0Struct, sem1Struct;
Semaphore_Handle sem0Handle, sem1Handle;
Clock_Struct clk0Struct, clk1Struct;

/*
 *  ======== main ========
 */
int main()
{

    /* Construct BIOS objects */
    Task_Params taskParams;
    Semaphore_Params semParams;
    Clock_Params clkParams;

    /* Construct clock Task thread */
    Task_Params_init(&taskParams);
    taskParams.arg0 = (UArg)&cl3;
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)clockTask, &taskParams, NULL);

    taskParams.stack = &task1Stack;
    taskParams.arg0 = (UArg)&cl4;
    Task_construct(&task1Struct, (Task_FuncPtr)clockTask, &taskParams, NULL);

    /* Construct Semaphores for clock thread to pend on, initial count 1 */
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&sem0Struct, 1, &semParams);
    /* Re-use default params */
    Semaphore_construct(&sem1Struct, 1, &semParams);

    /* Obtain instance handles */
    sem0Handle = Semaphore_Handle(&sem0Struct);
    sem1Handle = Semaphore_Handle(&sem1Struct);

    Clock_Params_init(&clkParams);
    clkParams.period = 100;
    clkParams.startFlag = true;
    clkParams.arg = (UArg)&cl1;
    Clock_construct(&clk0Struct, (Clock_FuncPtr)clockPrd,
                    1, &clkParams);

    clkParams.period = 1000;
    clkParams.startFlag = true;
    clkParams.arg = (UArg)&cl2;
    Clock_construct(&clk1Struct, (Clock_FuncPtr)clockPrd,
                    1, &clkParams);

    System_printf("bigTime started.\n");

    BIOS_start();    /* does not return */
    return(0);
}

/*
 *  ======== clockTerminate ========
 *  This function simply terminates the example
 */
void clockTerminate(UArg arg)
{
    System_printf("bigTime ended.\n");
    BIOS_exit(0);
}

/*
 *  ======== clockTask ========
 *  Wrapper function for Task objects calling
 *  Clock::tick()
 */
void clockTask(UArg arg)
{
    Clock *clock = (Clock *)arg;
    int count = 0;

    if (clock->getId() == 3) {
        for(;;) {             // task id = 3
            Semaphore_pend(sem0Handle, BIOS_WAIT_FOREVER);
            clock->tick();
            if(count == 50) {
                Task_sleep(25);
                count = 0;
            }
            count++;
            Semaphore_post(sem1Handle);
        }
    }
    else {
        for(;;) {             // task id = 4
            Semaphore_pend(sem1Handle, BIOS_WAIT_FOREVER);
            if(count == 50) {
                Task_sleep(25);
                count = 0;
            }
            clock->tick();
            count++;
            Semaphore_post(sem0Handle);
        }
    }
}

/*
 * ======== clockPrd ========
 * Wrapper function for PRD objects calling
 * Clock::tick()
 */
void clockPrd(UArg arg)
{
    Clock *clock = (Clock *)arg;

    clock->tick();
    return;
}

/*
 * ======== clockIdle ========
 * Wrapper function for IDL objects calling
 * Clock::tick()
 * It also calls System_flush() to periodically
 * print the contents in the SysMin buffer
 */
void clockIdle(void)
{
    cl0.tick();
    System_flush();
    return;
}

/*
 * Clock methods
 */
Clock::Clock(int newId)
{
    id = newId;
    ticks = 0;
    microsecond = 0;
    millisecond = 0;
    second = 0;
    minute = 0;
    hour = 0;
    day = 19;
    month = 8;
    year = 10;
    century = 20;
    millenium = 0;
}

Clock::~Clock()
{
}

void Clock::tick()
{
    ticks++;

    if (getId() == 1) {
        System_printf("id %d : %d:%d:%d.%d\n", getId(), hour, minute, second, millisecond / 100);
        System_printf("id %d : %s %d, %d%d\n", getId(), (IArg)months[month-1], day, century, year);
        /*
         * id 1 expires every 100 ticks (and each tick is 1 millisecond)
         */
        setMillisecond(100);
    }
    if (getId() == 2) {
        System_printf("id %d : %d:%d:%d\n", getId(), hour, minute, second);
        System_printf("id %d : %s %d, %d%d\n", getId(), (IArg)months[month-1], day, century, year);
        /*
         * Change selected function to alter clock rate
         */
//      setMicrosecond();
//      setMillisecond();
        setSecond();
//      setMinute();
//      setDay();
        if (ticks == 2) {
            clockTerminate(0);
        }
    }

    return;
}

void Clock::setMicrosecond()
{
    if (microsecond >= 999) {
        setMillisecond();
        microsecond = 0;
    }
    else {
        microsecond++;
    }

    return;
}

void Clock::setMillisecond()
{
    if (millisecond >= 999) {
        setSecond();
        millisecond = 0;
    }
    else {
        millisecond++;
    }

    return;
}

void Clock::setMillisecond(int nMilliseconds)
{
    int secs;

    millisecond += nMilliseconds;
    secs = millisecond / 1000;
    millisecond %= 1000;

    while (secs--) {
        setSecond();
    }

    return;
}

void Clock::setSecond()
{
    if (second == 59) {
        setMinute();
        second = 0;
    }
    else {
        second++;
    }

    return;
}

void Clock::setMinute()
{
    if (minute == 59) {
        setHour();
        minute = 0;
    }
    else {
        minute++;
    }

    return;
}

void Clock::setHour()
{
    if (hour == 23) {
        setDay();
        hour = 0;
    }
    else {
        hour++;
    }

    return;
}

void Clock::setDay()
{
    bool thirtydays = false;
    bool feb = false;
    bool leap = false;

    if (month == 4 || month == 6 || month == 9 || month == 11) {
        // April, June, September, November.
        thirtydays = true;
    }

    if (month == 2) {  // Test for February
        feb = true;
    }

    /*
     * A year is a leap year if it is divisible by 4, but not by 100.
     *
     * If a year is divisible by 4 and by 100, it is a leap year only
     * if it is also divisible by 400.
     */
    if ((year%4 == 0 && year%100 != 0) ||
            (year%4 == 0 && year%100 == 0 && year%400 == 0)) {
        leap = true;
    }

    if ((day == 28) && (feb) && (!leap)) {
        setMonth();
        day = 1;
    }
    else if ((day == 29) && (feb) && (leap)) {
        setMonth();
        day = 1;
    }
    else if ((day == 30) && (thirtydays == true)) {
        setMonth();
        day = 1;
    }
    else if ((day == 31) && (thirtydays == false)) {
        setMonth();
        day = 1;
    }
    else {
        day++;
    }

    return;
}

void Clock::setMonth()
{
    if (month >= 12) {
        setYear();
        month = 1;
    }
    else {
        month++;
    }

    return;
}

void Clock::setYear()
{
    year++;
    if ((year%100) == 0) {
        setCentury();
    }

    return;
}

void Clock::setCentury()
{
    century++;
    if ((century%10) == 0) {
        setMillenium();
    }

    return;
}

void Clock::setMillenium()
{
    millenium++;

    return;
}

long Clock::getTicks()
{
    return ticks;
}

int Clock::getId()
{
    return id;
}

int Clock::getMicrosecond()
{
    return microsecond;
}

int Clock::getMillisecond()
{
    return millisecond;
}

int Clock::getSecond()
{
    return second;
}

int Clock::getMinute()
{
    return minute;
}

int Clock::getHour()
{
    return hour;
}

int Clock::getDay()
{
    return day;
}

int Clock::getMonth()
{
    return month;
}

int Clock::getYear()
{
    return year;
}

int Clock::getCentury()
{
    return century;
}

int Clock::getMillenium()
{
    return millenium;
}
