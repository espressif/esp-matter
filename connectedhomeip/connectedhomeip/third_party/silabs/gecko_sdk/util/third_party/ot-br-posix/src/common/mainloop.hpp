/*
 *  Copyright (c) 2019, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes definitions for mainloop processor.
 */

#ifndef OTBR_COMMON_OTBR_MAINLOOP_HPP_
#define OTBR_COMMON_OTBR_MAINLOOP_HPP_

#include <openthread-br/config.h>

#include <openthread/openthread-system.h>

namespace otbr {

/**
 * This type defines the context data for running a mainloop.
 *
 */
using MainloopContext = otSysMainloopContext;

/**
 * This abstract class defines the interface of a mainloop processor
 * which adds fds to the mainloop context and handles fds events.
 *
 */
class MainloopProcessor
{
public:
    MainloopProcessor(void);

    virtual ~MainloopProcessor(void);

    /**
     * This method updates the mainloop context.
     *
     * @param[in,out] aMainloop  A reference to the mainloop to be updated.
     *
     */
    virtual void Update(MainloopContext &aMainloop) = 0;

    /**
     * This method processes mainloop events.
     *
     * @param[in] aMainloop  A reference to the mainloop context.
     *
     */
    virtual void Process(const MainloopContext &aMainloop) = 0;
};

} // namespace otbr

#endif // OTBR_COMMON_OTBR_MAINLOOP_HPP_
