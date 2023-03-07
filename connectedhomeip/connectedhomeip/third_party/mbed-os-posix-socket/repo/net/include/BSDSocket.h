/* mbed Microcontroller Library
 * Copyright (c) 2021 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MBED_NET_BSD_SOCKET_H
#define MBED_NET_BSD_SOCKET_H

#include "OpenFileHandleAsFileDescriptor.h"
#include "net_common.h"
#include <mstd_atomic>
#include <netsocket/TCPSocket.h>
#include <netsocket/UDPSocket.h>

namespace mbed {

struct BSDSocket : public FileHandle
{
    using counter_type = uint32_t;
    using flags_type = uint32_t;
    enum
    {
        MBED_TCP_SOCKET = SOCK_STREAM,
        MBED_UDP_SOCKET = SOCK_DGRAM
    };

    enum
    {
        MBED_IPV4_SOCKET = AF_INET,
        MBED_IPV6_SOCKET = AF_INET6
    };
    BSDSocket(){};
    ~BSDSocket(){};

    int open(int family, int type, InternetSocket * socket = nullptr);

    int close() override;
    ssize_t read(void * buffer, size_t size) override;
    ssize_t write(const void * buffer, size_t size) override;
    off_t seek(off_t offset, int whence = SEEK_SET) override;
    int set_blocking(bool blocking) override;
    bool is_blocking() const override;
    int enable_input(bool enabled) override;
    bool is_input_enable();
    int enable_output(bool enabled) override;
    bool is_output_enable();
    short poll(short events) const override;
    void sigio(Callback<void()> func) override;
    bool isSocketOpen();
    InternetSocket * getNetSocket();
    int getSocketType();

    // Implement some bit of the socket API to preserve the state of flags 
    // Do not use getSocket to directly call them.
    nsapi_size_or_error_t recv(void *buffer, nsapi_size_t size);
    nsapi_size_or_error_t recvfrom(SocketAddress *address, void *buffer, nsapi_size_t size);
    nsapi_size_or_error_t recvmsg(SocketAddress *address, void *buffer, nsapi_size_t size, nsapi_msghdr_t* control, nsapi_size_t control_size);
    nsapi_error_t listen(int backlog);
    Socket *accept(nsapi_error_t *error);
    nsapi_size_or_error_t send(const void *data, nsapi_size_t size);
    nsapi_size_or_error_t sendto(const SocketAddress &address, const void *data, nsapi_size_t size);
    nsapi_size_or_error_t sendmsg(const SocketAddress &address, const void *data, nsapi_size_t size, nsapi_msghdr_t* control, nsapi_size_t control_size);
    nsapi_error_t connect(const SocketAddress &address);

    SocketAddress socketName;

private:

    // FLAGS STATE NOTES
    // -----------------
    // To prevent the socket as beeing seen as active, the POLLIN or POLLOUT flags 
    // are unset when the operation reurns NSAPI_WOULD_BLOCK.  
    // When the sigio callback of the socket is called, the flags are set to 
    // POLLIN | POLLOUT as we now _some_ operation is available on the socket 
    // but we don't know which.
    // There could be a race condition if the sigio callback is called between 
    // after a send or recv return and before the flag in this object has been 
    // cleared. 
    // The natural option would be to hold a mutex during the call to send/recv
    // but this is not applicable as the sigio callback would have to acquire 
    // the same mutex. This can cause a deadlock if the sigio callback is called 
    // from another thread. 
    // Instead we use an optimistic approach. This class maintains a counter 
    // which is used to detect if the poll flags have been updated while doing 
    // another operation.
    // The sigio callbacks set them unconditionaly while the send/recv function
    // sets them only they haven't been touched.
    // At worst, the socket will appear as being ready while it doesn't which 
    // will trigger another recv/send that will correctly mark the operation 
    // as blocked. 

    // return the counter associated with the last change to flags 
    // returned by poll 
    counter_type get_poll_counter();

    // mark read and write as blocking
    bool set_write_as_blocking(counter_type counter);
    bool set_read_as_blocking(counter_type counter);
    bool set_read_write_as_blocking(counter_type counter);

    void reset_flags();
    bool try_update_flags(flags_type flags, counter_type counter);
    void update_flags(flags_type flags);

    InternetSocket * _socket = nullptr;
    int _fd                  = -1;
    int _type;
    Callback<void()> _callback      = nullptr;
    flags_type _flags               = 0;
    counter_type _counter           = 0;
    bool _blocking                  = true;
    bool _inputEnable               = true;
    bool _outputEnable              = true;
    bool _factory_allocated         = false;
};

} // namespace mbed

#endif // MBED_NET_BSD_SOCKET_H
