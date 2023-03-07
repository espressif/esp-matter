/**
 * @file
 * @brief SyncEvent (Synchronous event aka. callback object, delegate, functor).
 *
 * @details Generic synchronous event that can be bound/unbound and
 *          supports binding to method with or without a required
 *          argument (typically an object pointer).
 *          Invocation of unbound SyncEvent is harmless.
 *          SyncEvent bind/unbind/invoke is not threadsafe.
 *
 * To bind:
 * SyncEventBind(&MySyncEvent, MyCallback)
 * SyncEventBindObject(&MySyncEvent, MyCallback, pThis)
 * Note that there is NO type checking of if passed function
 * pointer takes the correct arg number and arg types in the
 * BindOBject methods (but it will only accept function pointers).
 * Note there is no type checking of if object pointer type
 * and function pointer object matches in the BindObject methods.
 *
 * To Invoke:
 * SyncEventInvoke(&MySyncEvent)
 * SyncEventArg1Invoke(&MySyncEvent, Arg1)
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */

#ifndef _SYNCEVENT_H_
#define _SYNCEVENT_H_

#include <stdbool.h>
#include <stdint.h>


//===============================   void SyncEvent(void)   ===============================


typedef union USyncEventFunctor
{
  void(*pFunction)(void);
  void(*pClassFunction)(void*);
} USyncEventFunctor;


// SyncEvent object (all members are private)
typedef struct  SSyncEvent
{
  USyncEventFunctor uFunctor;
  void* pObject;
} SSyncEvent;


/**
* \brief Returns if SyncEvent is bound
*
* \param pThis SyncEvent object structure.
*/
bool SyncEventIsBound(const SSyncEvent* pThis);

/**
* \brief Unbind the SyncEvent
* \details Unbinds the SyncEvent. Once unbound there will be no action when invoked.
* It is crucial that Bind, BindObject or Unbind is called before 
* first invoke (my kingdom for a constructor).
*
* \param pThis SyncEvent object structure.
*/
void SyncEventUnbind(SSyncEvent* pThis);

/**
* \brief Bind the SyncEvent to a function
* \details Binds the SyncEvent to a function. Once bound invocation will call function.
*
* \param pThis SyncEvent object structure.
* \param pFunction Function pointer to function of type: void Foo(void)
*/
void SyncEventBind(SSyncEvent* pThis, void(*pFunction)(void));

/**
* \brief Bind the SyncEvent to a "class" function
* \details Binds the SyncEvent to a function wich takes an object pointer or similar.
* Once bound invocation will call function with pointer as argument.
*
* \param pThis SyncEvent object structure.
* \param pFunction Function pointer to function of type: void Foo(FooObject*)
* \param pObject Object pointer to pass as argument to bound function
*/
void SyncEventBindObject(SSyncEvent* pThis, void(*pFunction)(), void* pObject);

/**
* \brief Invoke the SyncEvent
* \details Invokes the SyncEvent. If the SyncEvent is not bound, nothing happens.
* If bound invocation will call function or Function with pointer as argument.
*
* \param pThis SyncEvent object structure.
*/
void SyncEventInvoke(const SSyncEvent* pThis);


//===============================   uint32_t SyncEvent(void)   ===============================

typedef union USyncEventRetFunctor
{
  uint32_t(*pFunction)(void);
  uint32_t(*pClassFunction)(void*);
} USyncEventRetFunctor;

// SyncEventRet object (all members are private)
typedef struct  SSyncEventRet
{
  USyncEventRetFunctor uFunctor;
  void* pObject;
  uint32_t Default;
} SSyncEventRet;


/**
* \brief Returns if SyncEvent is bound
*
* \param pThis SyncEvent object structure.
*/
bool SyncEventRetIsBound(const SSyncEventRet* pThis);

/**
* \brief Unbind the SyncEvent
* \details Unbinds the SyncEvent. Once unbound there will be no action when invoked.
* It is crucial that Bind, BindObject or Unbind is called before
* first invoke (my kingdom for a constructor).
*
* \param pThis SyncEvent object structure.
*/
void SyncEventRetUnbind(SSyncEventRet* pThis);

/**
* \brief Bind the SyncEvent to a function
* \details Binds the SyncEvent to a function. Once bound invocation will call function.
*
* \param pThis SyncEvent object structure.
* \param pFunction Function pointer to function of type: uint32_t Foo(void)
*/
void SyncEventRetBind(SSyncEventRet* pThis, uint32_t(*pFunction)(void));

/**
* \brief Bind the SyncEvent to a "class" function
* \details Binds the SyncEvent to a function wich takes an object pointer or similar.
* Once bound invocation will call function with pointer as argument.
*
* \param pThis SyncEvent object structure.
* \param pFunction Function pointer to function of type: uint32_t Foo(FooObject*)
* \param pObject Object pointer to pass as argument to bound function
*/
void SyncEventRetBindObject(SSyncEventRet* pThis, uint32_t(*pFunction)(), void* pObject);

/**
* \brief Invoke the SyncEvent
* \details Invokes the SyncEvent. If theSyncEvent is not bound, nothing happens.
* If bound invocation will call function or Function with pointer as argument.
*
* \param pThis SyncEvent object structure.
*/
uint32_t SyncEventRetInvoke(const SSyncEventRet* pThis);

/**
* \brief Configure default return
* \details Configure the default return value that is returned when the SyncEvent
* is invoked while unbound.
* \param pThis SyncEvent object structure.
* \param DefaultReturnValue Value returned when SyncEvent is invoked while unbound.
*/
void SyncEventRetSetDefault(SSyncEventRet* pThis, uint32_t DefaultReturnValue);


//===============================   void SyncEvent(uint32_t)   ===============================

typedef union USyncEventArg1Functor
{
  void(*pFunction)(uint32_t);
  void(*pClassFunction)(void *, uint32_t);
} USyncEventArg1Functor;

// SyncEventArg1 object (all members are private)
typedef struct  SSyncEventArg1
{
  USyncEventArg1Functor uFunctor;
  void* pObject;
} SSyncEventArg1;


/**
* \brief Returns if SyncEvent is bound
*
* \param pThis SyncEvent object structure.
*/
bool SyncEventArg1IsBound(const SSyncEventArg1* pThis);

/**
* \brief Unbind the SyncEvent
* \details Unbinds the SyncEvent. Once unbound there will be no action when invoked.
* It is crucial that Bind, BindObject or Unbind is called before
* first invoke (my kingdom for a constructor).
*
* \param pThis SyncEvent object structure.
*/
void SyncEventArg1Unbind(SSyncEventArg1* pThis);

/**
* \brief Bind the SyncEvent to a function
* \details Binds the SyncEvent to a function. Once bound invocation will call function.
*
* \param pThis SyncEvent object structure.
* \param pFunction Function pointer to function of type: void Foo(uint32_t)
*/
void SyncEventArg1Bind(SSyncEventArg1* pThis, void(*pFunction)(uint32_t));

/**
* \brief Bind the SyncEvent to a "class" function
* \details Binds the SyncEvent to a function wich takes an object pointer or similar.
* Once bound invocation will call function with pointer as argument.
*
* \param pThis SyncEvent object structure.
* \param pFunction Function pointer to function of type: void Foo(FooObject*, uint32_t)
* \param pObject Object pointer to pass as argument to bound function
*/
void SyncEventArg1BindObject(SSyncEventArg1* pThis, void(*pFunction)(), void* pObject);

/**
* \brief Invoke the SyncEvent
* \details Invokes the SyncEvent. If theSyncEvent is not bound, nothing happens.
* If bound invocation will call function or Function with pointer as argument.
*
* \param pThis SyncEvent object structure.
* \param Arg1
*/
void SyncEventArg1Invoke(const SSyncEventArg1* pThis, uint32_t Arg1);


//===============================   void SyncEvent(uint32_t, uint32_t)   =====================

typedef union USyncEventArg2Functor
{
  void(*pFunction)(uint32_t, uint32_t);
  void(*pClassFunction)(void *, uint32_t, uint32_t);
} USyncEventArg2Functor;

// SyncEventArg1 object (all members are private)
typedef struct  SSyncEventArg2
{
  USyncEventArg2Functor uFunctor;
  void* pObject;
} SSyncEventArg2;


/**
* \brief Returns if SyncEvent is bound
*
* \param pThis SyncEvent object structure.
*/
bool SyncEventArg2IsBound(const SSyncEventArg2* pThis);

/**
* \brief Unbind the SyncEvent
* \details Unbinds the SyncEvent. Once unbound there will be no action when invoked.
* It is crucial that Bind, BindObject or Unbind is called before
* first invoke (my kingdom for a constructor).
*
* \param pThis SyncEvent object structure.
*/
void SyncEventArg2Unbind(SSyncEventArg2* pThis);

/**
* \brief Bind the SyncEvent to a function
* \details Binds the SyncEvent to a function. Once bound invocation will call function.
*
* \param pThis SyncEvent object structure.
* \param pFunction Function pointer to function of type: void Foo(uint32_t)
*/
void SyncEventArg2Bind(SSyncEventArg2* pThis, void(*pFunction)(uint32_t, uint32_t));

/**
* \brief Bind the SyncEvent to a "class" function
* \details Binds the SyncEvent to a function wich takes an object pointer or similar.
* Once bound invocation will call function with pointer as argument.
*
* \param pThis SyncEvent object structure.
* \param pFunction Function pointer to function of type: void Foo(FooObject*, uint32_t)
* \param pObject Object pointer to pass as argument to bound function
*/
void SyncEventArg2BindObject(SSyncEventArg2* pThis, void(*pFunction)(), void* pObject);

/**
* \brief Invoke the SyncEvent
* \details Invokes the SyncEvent. If theSyncEvent is not bound, nothing happens.
* If bound invocation will call function or Function with pointer as argument.
*
* \param pThis SyncEvent object structure.
* \param Arg1
* \param Arg2
*/
void SyncEventArg2Invoke(const SSyncEventArg2* pThis, uint32_t Arg1, uint32_t Arg2);

#endif	// _SYNCEVENT_H_

