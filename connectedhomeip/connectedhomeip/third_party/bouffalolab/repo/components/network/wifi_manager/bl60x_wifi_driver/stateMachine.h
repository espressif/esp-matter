
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

/**
 * \mainpage %stateMachine
 *
 * %stateMachine is a feature-rich, yet simple finite state machine
 * implementation. It supports grouped states, guarded transitions, events
 * with payload, entry and exit actions, transition actions and access to
 * user-defined state data from all actions.
 *
 * The user must build the state machine by linking together states and
 * transitions arrays with pointers. A pointer to an initial state and an
 * error state is given to stateM_init() to initialise a state machine object.
 * The state machine is run by passing events to it with the function
 * stateM_handleEvent(). The return value of stateM_handleEvent() will
 * give an indication to what has happened.
 *
 * \image html stateMachine.svg "Illustrating a stateMachine"
 */

/**
 * \defgroup stateMachine State machine
 *
 * \author Andreas Misje
 * \date 27.03.13
 *
 * \brief Finite state machine
 *
 * A finite state machine implementation that supports nested states, guards
 * and entry/exit routines. All state machine data is stored in separate
 * objects, and the state machine must be built by the user. States are
 * connected using pointers, and all data can be stored on either the stack,
 * heap or both.
 */

/**
 * \addtogroup stateMachine
 * @{
 *
 * \file
 * \example stateMachineExample.c Simple example of how to create a state
 * machine
 * \example nestedTest.c Simple example testing the behaviour of nested
 * parent states
 */

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stddef.h>
#include <stdbool.h>

/**
 * \brief Event
 *
 * Events trigger transitions from a state to another. Event types are defined
 * by the user. Any event may optionally contain a \ref #event::data
 * "payload".
 *
 * \sa state
 * \sa transition
 */
struct event
{
   /** \brief Type of event. Defined by user. */
   int type;
   /** 
    * \brief Event payload.
    *
    * How this is used is entirely up to the user. This data
    * is always passed together with #type in order to make it possible to
    * always cast the data correctly.
    */
   void *data;
};

struct state;

/**
 * \brief Transition between a state and another state
 *
 * All states that are not final must have at least one transition. The
 * transition may be guarded or not. Transitions are triggered by events. If
 * a state has more than one transition with the same type of event (and the
 * same condition), the first transition in the array will be run. An
 * unconditional transition placed last in the transition array of a state can
 * act as a "catch-all". A transition may optionally run an #action, which
 * will have the triggering event passed to it as an argument, along with the
 * current and new states' \ref state::data "data".
 *
 * It is perfectly valid for a transition to return to the state it belongs
 * to. Such a transition will not call the state's \ref state::entryAction
 * "entry action" or \ref state::exitAction "exit action". If there are no
 * transitions for the current event, the state's parent will be handed the
 * event.
 *
 * ### Examples ###
 * - An ungarded transition to a state with no action performed:
 * ~~~{.c}
 * {
 *    .eventType = Event_timeout,
 *    .condition = NULL,
 *    .guard = NULL,
 *    .action = NULL,
 *    .nextState = &mainMenuState,
 * },
 * ~~~
 * - A guarded transition executing an action
 * ~~~{.c}
 * {
 *    .eventType = Event_keyboard,
 *    .condition = NULL,
 *    .guard = &ensureNumericInput,
 *    .action = &addToBuffer,
 *    .nextState = &awaitingInputState,
 * },
 * ~~~
 * - A guarded transition using a condition
 * ~~~{.c}
 * {
 *    .eventType = Event_mouse,
 *    .condition = boxLimits,
 *    .guard = &coordinatesWithinLimits,
 * },
 * ~~~
 * By using \ref #condition "conditions" a more general guard function can be
 * used, operating on the supplied argument #condition. In this example,
 * `coordinatesWithinLimits` checks whether the coordinates in the mouse event
 * are within the limits of the "box".
 *
 * \sa event
 * \sa state
 */
struct transition
{
   /** \brief The event that will trigger this transition. */
   int eventType;
   /**
    * \brief Condition that event must fulfil
    *
    * This variable will be passed to the #guard (if #guard is non-NULL) and
    * may be used as a condition that the incoming event's data must fulfil in
    * order for the transition to be performed. By using this variable, the
    * number of #guard functions can be minimised by making them more general.
    */
   void *condition;
   /**
    * \brief Check if data passed with event fulfils a condition
    *
    * A transition may be conditional. If so, this function, if non-NULL, will
    * be called. Its first argument will be supplied with #condition, which
    * can be compared against the \ref event::data "payload" in the #event.
    * The user may choose to use this argument or not. Only if the result is
    * true, the transition will take place.
    *
    * \param condition event (data) to compare the incoming event against.
    * \param event the event passed to the state machine.
    *
    * \returns true if the event's data fulfils the condition, otherwise false.
    */
   bool ( *guard )( void *condition, struct event *event );
   /** 
    * \brief Function containing tasks to be performed during the transition
    *
    * The transition may optionally do some work in this function before
    * entering the next state. May be NULL.
    *
    * \param currentStateData the leaving state's \ref state::data "data"
    * \param event the event passed to the state machine.
    * \param newStateData the new state's (the \ref state::entryState
    * "entryState" of any (chain of) parent states, not the parent state
    * itself) \ref state::data "data"
    */
   void ( *action )( void *currentStateData, struct event *event,
         void *newStateData );
   /**
    * \brief The next state
    *
    * This must point to the next state that will be entered. It cannot be
    * NULL. If it is, the state machine will detect it and enter the \ref
    * stateMachine::errorState "error state".
    */
   const struct state *nextState;
};

/**
 * \brief State
 *
 * The current state in a state machine moves to a new state when one of the
 * #transitions in the current state triggers on an event. An optional \ref
 * #exitAction "exit action" is called when the state is left, and an \ref
 * #entryAction "entry action" is called when the state machine enters a new
 * state. If a state returns to itself, neither #exitAction nor #entryAction
 * will be called. An optional \ref transition::action "transition action" is
 * called in either case.
 *
 * States may be organised in a hierarchy by setting \ref #parentState
 * "parent states". When a group/parent state is entered, the state machine is
 * redirected to the group state's \ref #entryState "entry state" (if
 * non-NULL). If an event does not trigger a transition in a state and if the
 * state has a parent state, the event will be passed to the parent state.
 * This behaviour is repeated for all parents. Thus all children of a state
 * have a set of common #transitions. A parent state's #entryAction will not
 * be called if an event is passed on to a child state. 
 *
 * The following lists the different types of states that may be created, and
 * how to create them:
 *
 * ### Normal state ###
 * ~~~{.c}
 * struct state normalState = {
 *    .parentState = &groupState,
 *    .entryState = NULL,
 *    .transition = (struct transition[]){
 *       { Event_keyboard, (void *)(intptr_t)'\n', &compareKeyboardChar,
 *          NULL, &msgReceivedState },
 *    },
 *    .numTransitions = 1,
 *    .data = normalStateData,
 *    .entryAction = &doSomething,
 *    .exitAction = &cleanUp,
 * };
 * ~~~
 * In this example, `normalState` is a child of `groupState`, but the
 * #parentState value may also be NULL to indicate that it is not a child of
 * any group state.
 *
 * ### Group/parent state ###
 * A state becomes a group/parent state when it is linked to by child states
 * by using #parentState. No members in the group state need to be set in a
 * particular way. A parent state may also have a parent.
 * ~~~{.c}
 * struct state groupState = {
 *    .entryState = &normalState,
 *    .entryAction = NULL,
 * ~~~
 * If there are any transitions in the state machine that lead to a group
 * state, it makes sense to define an entry state in the group. This can be
 * done by using #entryState, but it is not mandatory. If the #entryState
 * state has children, the chain of children will be traversed until a child
 * with its #entryState set to NULL is found.
 *
 * \note If #entryState is defined for a group state, the group state's
 * #entryAction will not be called (the state pointed to by #entryState (after
 * following the chain of children), however, will have its #entryAction
 * called).
 *
 * \warning The state machine cannot detect cycles in parent chains and
 * children chains. If such cycles are present, stateM_handleEvent() will
 * never finish due to never-ending loops.
 *
 * ### Final state ###
 * A final state is a state that terminates the state machine. A state is
 * considered as a final state if its #numTransitions is 0:
 * ~~~{.c}
 * struct state finalState = {
 *    .transitions = NULL,
 *    .numTransitions = 0,
 * ~~~
 * The error state used by the state machine to indicate errors should be a
 * final state. Any calls to stateM_handleEvent() when the current state is a
 * final state will return #stateM_noStateChange.
 *
 * \sa event
 * \sa transition
 */
struct state
{
   /**
    * \brief If the state has a parent state, this pointer must be non-NULL.
    */
   const struct state *parentState;
   /**
    * \brief If this state is a parent state, this pointer may point to a
    * child state that serves as an entry point.
    */
   const struct state *entryState;
   /** 
    * \brief An array of transitions for the state.
    */
   struct transition *transitions;
   /** 
    * \brief Number of transitions in the #transitions array.
    */
   size_t numTransitions;
   /**
    * \brief Data that will be available for the state in its #entryAction and
    * #exitAction, and in any \ref transition::action "transition action"
    */
   void *data;
   /** 
    * \brief This function is called whenever the state is being entered. May
    * be NULL.
    *
    * \note If a state returns to itself through a transition (either directly
    * or through a parent/group sate), its #entryAction will not be called.
    *
    * \note A group/parent state with its #entryState defined will not have
    * its #entryAction called.
    *
    * \param stateData the state's #data will be passed.
    * \param event the event that triggered the transition will be passed.
    */
   void ( *entryAction )( void *stateData, struct event *event );
   /**
    * \brief This function is called whenever the state is being left. May be
    * NULL.
    *
    * \note If a state returns to itself through a transition (either directly
    * or through a parent/group sate), its #exitAction will not be called.
    *
    * \param stateData the state's #data will be passed.
    * \param event the event that triggered a transition will be passed.
    */
   void ( *exitAction )( void *stateData, struct event *event );
};

/**
 * \brief State machine
 *
 * There is no need to manipulate the members directly.
 */
struct stateMachine
{
   /** \brief Pointer to the current state */
   const struct state *currentState;
   /** 
    * \brief Pointer to previous state
    *
    * The previous state is stored for convenience in case the user needs to
    * keep track of previous states.
    */
   const struct state *previousState;
   /** 
    * \brief Pointer to a state that will be entered whenever an error occurs
    * in the state machine.
    *
    * See #stateM_errorStateReached for when the state machine enters the
    * error state.
    */
   const struct state *errorState;
};

/**
 * \brief Initialise the state machine
 *
 * This function initialises the supplied stateMachine and sets the current
 * state to \pn{initialState}. No actions are performed until
 * stateM_handleEvent() is called. It is safe to call this function numerous
 * times, for instance in order to reset/restart the state machine if a final
 * state has been reached.
 *
 * \note The \ref #state::entryAction "entry action" for \pn{initialState}
 * will not be called.
 * 
 * \note If \pn{initialState} is a parent state with its \ref
 * state::entryState "entryState" defined, it will not be entered. The user
 * must explicitly set the initial state.
 *
 * \param stateMachine the state machine to initialise.
 * \param initialState the initial state of the state machine.
 * \param errorState pointer to a state that acts a final state and notifies
 * the system/user that an error has occurred.
 */
void stateM_init( struct stateMachine *stateMachine,
      const struct state *initialState, const struct state *errorState );

/**
 * \brief stateM_handleEvent() return values
 */
enum stateM_handleEventRetVals
{
   /** \brief Erroneous arguments were passed */
   stateM_errArg = -2,
   /**
    * \brief The error state was reached
    *
    * This value is returned either when the state machine enters the error
    * state itself as a result of an error, or when the error state is the
    * next state as a result of a successful transition.
    *
    * The state machine enters the state machine if any of the following
    * happens:
    * - The current state is NULL
    * - A transition for the current event did not define the next state
    */
   stateM_errorStateReached,
   /** \brief The current state changed into a non-final state */
   stateM_stateChanged,
   /**
    * \brief The state changed back to itself
    *
    * The state can return to itself either directly or indirectly. An
    * indirect path may inlude a transition from a parent state and the use of
    * \ref state::entryState "entryStates".
    */
   stateM_stateLoopSelf,
   /**
    * \brief The current state did not change on the given event
    *
    * If any event passed to the state machine should result in a state
    * change, this return value should be considered as an error.
    */
   stateM_noStateChange,
   /** \brief A final state (any but the error state) was reached */
   stateM_finalStateReached,
};

/**
 * \brief Pass an event to the state machine
 *
 * The event will be passed to the current state, and possibly to the current
 * state's parent states (if any). If the event triggers a transition, a new
 * state will be entered. If the transition has an \ref transition::action
 * "action" defined, it will be called. If the transition is to a state other
 * than the current state, the current state's \ref state::exitAction
 * "exit action" is called (if defined). Likewise, if the state is a new
 * state, the new state's \ref state::entryAction "entry action" is called (if
 * defined).
 *
 * The returned value is negative if an error occurs.
 *
 * \param stateMachine the state machine to pass an event to.
 * \param event the event to be handled.
 *
 * \return #stateM_handleEventRetVals
 */
int stateM_handleEvent( struct stateMachine *stateMachine,
      struct event *event );

/**
 * \brief Get the current state
 *
 * \param stateMachine the state machine to get the current state from.
 *
 * \retval a pointer to the current state.
 * \retval NULL if \pn{stateMachine} is NULL.
 */
const struct state *stateM_currentState( struct stateMachine *stateMachine );

/**
 * \brief Get the previous state
 *
 * \param stateMachine the state machine to get the previous state from.
 *
 * \retval the previous state.
 * \retval NULL if \pn{stateMachine} is NULL.
 * \retval NULL if there has not yet been any transitions.
 */
const struct state *stateM_previousState( struct stateMachine *stateMachine );

/**
 * \brief Check if the state machine has stopped
 *
 * \param stateMachine the state machine to test.
 *
 * \retval true if the state machine has reached a final state.
 * \retval false if \pn{stateMachine} is NULL or if the current state is not a
 * final state.
 */
bool stateM_stopped( struct stateMachine *stateMachine );

#endif // STATEMACHINE_H

/**
 * @}
 */
