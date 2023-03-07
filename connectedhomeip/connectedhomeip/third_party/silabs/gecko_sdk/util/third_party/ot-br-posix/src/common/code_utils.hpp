/*
 *    Copyright (c) 2017, The OpenThread Authors.
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *    POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * This file includes utility macros for coding.
 */
#ifndef OTBR_COMMON_CODE_UTILS_HPP_
#define OTBR_COMMON_CODE_UTILS_HPP_

#ifndef OTBR_LOG_TAG
#define OTBR_LOG_TAG "UTILS"
#endif

#include <assert.h>
#include <memory>
#include <stdlib.h>

#include "common/logging.hpp"

/**
 *  This aligns the pointer to @p aAlignType.
 *
 *  @param[in] aMem        A pointer to arbitrary memory.
 *  @param[in] aAlignType  The type to align with and convert the pointer to this type.
 *
 *  @returns A pointer to aligned memory.
 *
 */
#define OTBR_ALIGNED(aMem, aAlignType) \
    reinterpret_cast<aAlignType>(      \
        ((reinterpret_cast<unsigned long>(aMem) + sizeof(aAlignType) - 1) / sizeof(aAlignType)) * sizeof(aAlignType))

#ifndef CONTAINING_RECORD
#define BASE 0x1
#define myoffsetof(s, m) (((size_t) & (((s *)BASE)->m)) - BASE)
#define CONTAINING_RECORD(address, type, field) ((type *)((uint8_t *)(address)-myoffsetof(type, field)))
#endif /* CONTAINING_RECORD */

/**
 *  This checks for the specified status, which is expected to
 *  commonly be successful, and branches to the local label 'exit' if
 *  the status is unsuccessful.
 *
 *  @param[in] aStatus  A scalar status to be evaluated against zero (0).
 *
 */
#define SuccessOrExit(aStatus) \
    do                         \
    {                          \
        if ((aStatus) != 0)    \
        {                      \
            goto exit;         \
        }                      \
    } while (false)

/**
 * This macro verifies a given error status to be successful (compared against value zero (0)), otherwise, it emits a
 * given error messages and exits the program.
 *
 * @param[in] aStatus   A scalar error status to be evaluated against zero (0).
 * @param[in] aMessage  A message (text string) to print on failure.
 *
 */
#define SuccessOrDie(aStatus, aMessage)                                      \
    do                                                                       \
    {                                                                        \
        if ((aStatus) != 0)                                                  \
        {                                                                    \
            otbrLogEmerg("FAILED %s:%d - %s", __FILE__, __LINE__, aMessage); \
            exit(-1);                                                        \
        }                                                                    \
    } while (false)

/**
 *  This checks for the specified condition, which is expected to
 *  commonly be true, and both executes @a ... and branches to the
 *  local label 'exit' if the condition is false.
 *
 *  @param[in] aCondition  A Boolean expression to be evaluated.
 *  @param[in] ...         An expression or block to execute when the
 *                         assertion fails.
 *
 */
#define VerifyOrExit(aCondition, ...) \
    do                                \
    {                                 \
        if (!(aCondition))            \
        {                             \
            __VA_ARGS__;              \
            goto exit;                \
        }                             \
    } while (false)

/**
 * This macro checks for the specified condition, which is expected to commonly be true,
 * and both prints the message and terminates the program if the condition is false.
 *
 * @param[in] aCondition  The condition to verify
 * @param[in] aMessage    A message (text string) to print on failure.
 *
 */
#define VerifyOrDie(aCondition, aMessage)                                    \
    do                                                                       \
    {                                                                        \
        if (!(aCondition))                                                   \
        {                                                                    \
            otbrLogEmerg("FAILED %s:%d - %s", __FILE__, __LINE__, aMessage); \
            exit(-1);                                                        \
        }                                                                    \
    } while (false)

/**
 *  This unconditionally executes @a ... and branches to the local
 *  label 'exit'.
 *
 *  @note The use of this interface implies neither success nor
 *        failure for the overall exit status of the enclosing
 *        function body.
 *
 *  @param[in] ...  An optional expression or block to execute
 *                  when the assertion fails.
 *
 */
#define ExitNow(...) \
    do               \
    {                \
        __VA_ARGS__; \
        goto exit;   \
    } while (false)

#define OTBR_NOOP
#define OTBR_UNUSED_VARIABLE(variable) ((void)(variable))

template <typename T, typename... Args> std::unique_ptr<T> MakeUnique(Args &&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * This class makes any class that derives from it non-copyable. It is intended to be used as a private base class.
 *
 */
class NonCopyable
{
public:
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;

protected:
    NonCopyable(void) = default;
};

template <typename T> class Optional
{
public:
    constexpr Optional(void) = default;

    Optional(T aValue) { SetValue(aValue); }

    ~Optional(void) { ClearValue(); }

    Optional(const Optional &aOther) { AssignFrom(aOther); }

    Optional &operator=(const Optional &aOther) { AssignFrom(aOther); }

    constexpr const T *operator->(void)const { return &GetValue(); }

    constexpr const T &operator*(void)const { return GetValue(); }

    constexpr bool HasValue(void) const { return mHasValue; }

private:
    T &GetValue(void) const
    {
        assert(mHasValue);
        return *const_cast<T *>(reinterpret_cast<const T *>(&mStorage));
    }

    void ClearValue(void)
    {
        if (mHasValue)
        {
            GetValue().~T();
            mHasValue = false;
        }
    }

    void SetValue(const T &aValue)
    {
        ClearValue();
        new (&mStorage) T(aValue);
        mHasValue = true;
    }

    void AssignFrom(const Optional &aOther)
    {
        ClearValue();
        if (aOther.mHasValue)
        {
            SetValue(aOther.GetValue());
        }
    }

    alignas(T) unsigned char mStorage[sizeof(T)];
    bool mHasValue = false;
};

#endif // OTBR_COMMON_CODE_UTILS_HPP_
