
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


/* 
 * Copyright (c) 2013 Andreas Misje
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#include <stdint.h>
#include <stdio.h>

#include "stateMachine.h"

/* This simple example checks keyboad input against the two allowed strings
 * "ha\n" and "hi\n". If an unrecognised character is read, a group state will
 * handle this by printing a message and returning to the idle state. If the
 * character '!' is encountered, a "reset" message is printed, and the group
 * state's entry state will be entered (the idle state). 
 *
 *                   print 'reset'
 *       o      +---------------------+
 *       |      |                     | '!'
 *       |      v     group state     |
 * +-----v-----------------------------------+----+
 * |  +------+  'h'  +---+  'a'  +---+  '\n'      |
 * +->| idle | ----> | h | ----> | a | ---------+ |
 * |  +------+       +---+\      +---+          | |
 * |   ^ ^ ^               \'i'  +---+  '\n'    | |
 * |   | | |                \--> | i | ------+  | |
 * |   | | |                     +---+       |  | |
 * +---|-|-|----------------+----------------|--|-+
 *     | | |                |                |  |
 *     | | |                | '[^hai!\n]'    |  |
 *     | | | print unrecog. |                |  |
 *     | | +----------------+   print 'hi'   |  |
 *     | +-----------------------------------+  |
 *     |               print 'ha'               |
 *     +----------------------------------------+
 */

/* Types of events */
enum eventType {
   Event_keyboard,
};

/* Compare keyboard character from transition's condition variable against
 * data in event. */
static bool compareKeyboardChar( void *ch, struct event *event );

static void printRecognisedChar( void *stateData, struct event *event );
static void printUnrecognisedChar( void *oldStateData, struct event *event,
      void *newStateData );
static void printReset( void *oldStateData, struct event *event,
      void *newStateData );
static void printHiMsg( void *oldStateData, struct event *event,
      void *newStateData );
static void printHaMsg( void *oldStateData, struct event *event,
      void *newStateData );
static void printErrMsg( void *stateData, struct event *event );
static void printEnterMsg( void *stateData, struct event *event );
static void printExitMsg( void *stateData, struct event *event );

/* Forward declaration of states so that they can be defined in an logical
 * order: */
static struct state checkCharsGroupState, idleState, hState, iState, aState;

/* All the following states (apart from the error state) are children of this
 * group state. This way, any unrecognised character will be handled by this
 * state's transition, eliminating the need for adding the same transition to
 * all the children states. */
static struct state checkCharsGroupState = {
   .parentState = NULL,
   /* The entry state is defined in order to demontrate that the 'reset'
    * transtition, going to this group state, will be 'redirected' to the
    * 'idle' state (the transition could of course go directly to the 'idle'
    * state): */
   .entryState = &idleState,
   .transitions = (struct transition[]){
      { Event_keyboard, (void *)(intptr_t)'!', &compareKeyboardChar, &printReset, &idleState, },
      { Event_keyboard, NULL, NULL, &printUnrecognisedChar, &idleState, },
   },
   .numTransitions = 2,
   .data = "group",
   .entryAction = &printEnterMsg,
   .exitAction = &printExitMsg,
};

static struct state idleState = {
   .parentState = &checkCharsGroupState,
   .entryState = NULL,
   .transitions = (struct transition[]){
      { Event_keyboard, (void *)(intptr_t)'h', &compareKeyboardChar, NULL, &hState },
   },
   .numTransitions = 1,
   .data = "idle",
   .entryAction = &printEnterMsg,
   .exitAction = &printExitMsg,
};

static struct state hState = {
   .parentState = &checkCharsGroupState,
   .entryState = NULL,
   .transitions = (struct transition[]){
      { Event_keyboard, (void *)(intptr_t)'a', &compareKeyboardChar, NULL, &aState },
      { Event_keyboard, (void *)(intptr_t)'i', &compareKeyboardChar, NULL, &iState },
   },
   .numTransitions = 2,
   .data = "H",
   .entryAction = &printRecognisedChar,
   .exitAction = &printExitMsg,
};

static struct state iState = {
   .parentState = &checkCharsGroupState,
   .entryState = NULL,
   .transitions = (struct transition[]){
      { Event_keyboard, (void *)(intptr_t)'\n', &compareKeyboardChar, &printHiMsg, &idleState }
   },
   .numTransitions = 1,
   .data = "I",
   .entryAction = &printRecognisedChar,
   .exitAction = &printExitMsg,
};

static struct state aState = {
   .parentState = &checkCharsGroupState,
   .entryState = NULL,
   .transitions = (struct transition[]){
      { Event_keyboard, (void *)(intptr_t)'\n', &compareKeyboardChar, &printHaMsg, &idleState }
   },
   .numTransitions = 1,
   .data = "A",
   .entryAction = &printRecognisedChar,
   .exitAction = &printExitMsg
};

static struct state errorState = {
   .entryAction = &printErrMsg
};


int main_machine()
{
   struct stateMachine m;
   stateM_init( &m, &idleState, &errorState );

   int ch;
   while ( ( ch = getc( stdin ) ) != EOF )
      stateM_handleEvent( &m, &(struct event){ Event_keyboard,
            (void *)(intptr_t)ch } );

   return 0;
}

static bool compareKeyboardChar( void *ch, struct event *event )
{
   if ( event->type != Event_keyboard )
      return false;

   return (intptr_t)ch == (intptr_t)event->data;
}

static void printRecognisedChar( void *stateData, struct event *event )
{
   printEnterMsg( stateData, event );
   rt_kprintf( "parsed: %c\r\n", (char)(intptr_t)event->data );
}

static void printUnrecognisedChar( void *oldStateData, struct event *event,
      void *newStateData )
{
   rt_kprintf( "unrecognised character: %c\n",
         (char)(intptr_t)event->data );
}

static void printReset( void *oldStateData, struct event *event,
      void *newStateData )
{
   rt_kprintf( "Resetting" );
}

static void printHiMsg( void *oldStateData, struct event *event,
      void *newStateData )
{
   rt_kprintf( "Hi!" );
}

static void printHaMsg( void *oldStateData, struct event *event,
      void *newStateData )
{
   rt_kprintf( "Ha-ha" );
}

static void printErrMsg( void *stateData, struct event *event )
{
   rt_kprintf( "ENTERED ERROR STATE!" );
}

static void printEnterMsg( void *stateData, struct event *event )
{
   rt_kprintf( "Entering %s state\r\n", (char *)stateData );
}

static void printExitMsg( void *stateData, struct event *event )
{
   rt_kprintf( "Exiting %s state\r\n", (char *)stateData );
}
