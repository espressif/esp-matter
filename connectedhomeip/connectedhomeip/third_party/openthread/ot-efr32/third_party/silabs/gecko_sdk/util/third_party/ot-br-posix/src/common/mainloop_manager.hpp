/*
 *    Copyright (c) 2021, The OpenThread Authors.
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
 *   This file includes definitions for mainloop manager.
 */

#ifndef OTBR_COMMON_MAINLOOP_MANAGER_HPP_
#define OTBR_COMMON_MAINLOOP_MANAGER_HPP_

#include <openthread-br/config.h>

#include <openthread/openthread-system.h>

#include <list>

#include "common/code_utils.hpp"
#include "common/mainloop.hpp"
#include "ncp/ncp_openthread.hpp"

namespace otbr {

/**
 * This class implements the mainloop manager.
 *
 */
class MainloopManager : private NonCopyable
{
public:
    /**
     * The constructor to initialize the mainloop manager.
     *
     */
    MainloopManager() = default;

    /**
     * This method returns the singleton instance of the mainloop manager.
     *
     */
    static MainloopManager &GetInstance(void)
    {
        static MainloopManager sMainloopManager;
        return sMainloopManager;
    }

    /**
     * This method adds a mainloop processors to the mainloop managger.
     *
     * @param[in] aMainloopProcessor  A pointer to the mainloop processor.
     *
     */
    void AddMainloopProcessor(MainloopProcessor *aMainloopProcessor);

    /**
     * This method removes a mainloop processors from the mainloop managger.
     *
     * @param[in] aMainloopProcessor  A pointer to the mainloop processor.
     *
     */
    void RemoveMainloopProcessor(MainloopProcessor *aMainloopProcessor);

    /**
     * This method updates the mainloop context of all mainloop processors.
     *
     * @param[in,out] aMainloop  A reference to the mainloop to be updated.
     *
     */
    void Update(MainloopContext &aMainloop);

    /**
     * This method processes mainloop events of all mainloop processors.
     *
     * @param[in] aMainloop  A reference to the mainloop context.
     *
     */
    void Process(const MainloopContext &aMainloop);

private:
    std::list<MainloopProcessor *> mMainloopProcessorList;
};
} // namespace otbr
#endif // OTBR_COMMON_MAINLOOP_MANAGER_HPP_
