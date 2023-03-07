#include "BSDSocket.h"
#include "platform/CriticalSectionLock.h"
#include "mbed-trace/mbed_trace.h"

#define TRACE_GROUP "BSDS"

namespace mbed {

int BSDSocket::open(int family, int type, InternetSocket * socket)
{
    if (socket != nullptr)
    {
        _socket            = socket;
        _factory_allocated = true;
    }
    else
    {
        switch (type)
        {
        case MBED_TCP_SOCKET: {
            _socket = new TCPSocket();
        }
        break;
        case MBED_UDP_SOCKET: {
            _socket = new UDPSocket();
        }
        break;
        default:
            tr_err("Socket type not supported");
            set_errno(ESOCKTNOSUPPORT);
            return -1;
        };

        if (_socket->open(get_mbed_net_if()) != NSAPI_ERROR_OK)
        {
            close();
            tr_err("Open socket failed");
            set_errno(ENOBUFS);
            return -1;
        }
    }

    _type = type;

    _socket->sigio([&]() {
        tr_debug("Socket %d event", _fd);
        // We don't know what changed. Set both POLLIN and POLLOUT
        update_flags(POLLIN | POLLOUT);
        
        if (_callback)
        {
            _callback();
        }
    });

    _socket->set_blocking(true);

    _fd = bind_to_fd(this);
    if (_fd < 0)
    {
        close();
        tr_err("Bind socket to fd failed");
        set_errno(ENFILE);
        return -1;
    }

    // Mark the socket as ready for read and write we will 
    // find out if it actually is with the first operation
    update_flags(POLLIN | POLLOUT);

    tr_info("Open %s socket with fd %d", type == MBED_TCP_SOCKET ? "TCP" : "UDP", _fd);

    return _fd;
}

int BSDSocket::close()
{
    if (_factory_allocated)
    {
        _socket->close();
    }
    else
    {
        delete _socket;
    }

    _socket = nullptr;

    tr_info("Close %s socket fd %d", _type == MBED_TCP_SOCKET ? "TCP" : "UDP", _fd);

    _fd                = -1;
    _callback          = nullptr;
    _factory_allocated = false;
    reset_flags();
    if (socketName)
    {
        socketName.set_ip_bytes(nullptr, NSAPI_UNSPEC);
    }
    return 0;
}

ssize_t BSDSocket::read(void * buffer, size_t size)
{
    // placeholder for the file API
    return 0;
}

BSDSocket::counter_type BSDSocket::get_poll_counter()
{
    mbed::CriticalSectionLock lock;
    return _counter;
}

bool BSDSocket::set_read_as_blocking(counter_type counter)
{
    auto current = _flags;
    return try_update_flags(current & ~POLLIN, counter);
}

bool BSDSocket::set_read_write_as_blocking(counter_type counter)
{
    auto current = _flags;
    return try_update_flags(current & ~(POLLIN | POLLOUT), counter);
}


ssize_t BSDSocket::write(const void * buffer, size_t size)
{
    // placeholder for the file API
    return 0;
}

bool BSDSocket::set_write_as_blocking(counter_type counter)
{
    auto current = _flags;
    return try_update_flags(current & ~POLLOUT, counter);
}

off_t BSDSocket::seek(off_t offset, int whence)
{
    return -ESPIPE;
}

int BSDSocket::set_blocking(bool blocking)
{
    tr_info("Set socket fd %d blocking: %s", _fd, blocking ? "true" : "false");
    _blocking = blocking;
    if (_socket != nullptr)
    {
        _socket->set_blocking(_blocking);
    }

    return 0;
}

bool BSDSocket::is_blocking() const
{
    return _blocking;
}

int BSDSocket::enable_input(bool enabled)
{
    tr_info("Set socket fd %d input enable: %s\n", _fd, enabled ? "true" : "false");
    _inputEnable = enabled;
    return 0;
}

bool BSDSocket::is_input_enable()
{
    return _inputEnable;
}

int BSDSocket::enable_output(bool enabled)
{
    tr_info("Set socket fd %d output enable: %s", _fd, enabled ? "true" : "false");
    _outputEnable = enabled;
    return 0;
}

bool BSDSocket::is_output_enable()
{
    return _outputEnable;
}

short BSDSocket::poll(short events) const
{
    mbed::CriticalSectionLock lock;
    auto state = _flags;
    return (state & events);
}

void BSDSocket::sigio(Callback<void()> func)
{
    _callback = func;
    if (_callback && poll(POLLIN | POLLOUT))
    {
        _callback();
    }
}

bool BSDSocket::isSocketOpen()
{
    return _fd >= 0;
}

InternetSocket * BSDSocket::getNetSocket()
{
    return _socket;
}

int BSDSocket::getSocketType()
{
    return _type;
}

nsapi_size_or_error_t BSDSocket::recv(void *buffer, nsapi_size_t size)
{
    auto counter = get_poll_counter();
    auto result = getNetSocket()->recv(buffer, size);
    if (result == NSAPI_ERROR_WOULD_BLOCK) { 
        set_read_as_blocking(counter);
    }
    return result;
}

nsapi_size_or_error_t BSDSocket::recvfrom(SocketAddress *address, void *buffer, nsapi_size_t size)
{
    auto counter = get_poll_counter();
    auto result = getNetSocket()->recvfrom(address, buffer, size);
    if (result == NSAPI_ERROR_WOULD_BLOCK) { 
        set_read_as_blocking(counter);
    }
    return result;
}

nsapi_size_or_error_t BSDSocket::recvmsg(SocketAddress *address, void *buffer, nsapi_size_t size, nsapi_msghdr_t* control, nsapi_size_t control_size)
{
    auto counter = get_poll_counter();
    auto result = getNetSocket()->recvfrom_control(address, buffer, size, control, control_size);
    if (result == NSAPI_ERROR_WOULD_BLOCK) { 
        set_read_as_blocking(counter);
    }
    return result;
}

nsapi_error_t BSDSocket::listen(int backlog) {
    auto counter = get_poll_counter();
    auto result = getNetSocket()->listen(backlog);
    if (result == NSAPI_ERROR_OK) { 
        set_read_write_as_blocking(counter);
    }
    return result;
}

Socket *BSDSocket::accept(nsapi_error_t *error)
{
    auto counter = get_poll_counter();
    auto result = getNetSocket()->accept(error);
    if (*error == NSAPI_ERROR_WOULD_BLOCK) { 
        set_read_as_blocking(counter);
    }
    return result;
}

nsapi_size_or_error_t BSDSocket::send(const void *data, nsapi_size_t size)
{
    auto counter = get_poll_counter();
    auto result = getNetSocket()->send(data, size);
    if (result == NSAPI_ERROR_WOULD_BLOCK) { 
        set_write_as_blocking(counter);
    }
    return result;
}

nsapi_size_or_error_t BSDSocket::sendto(const SocketAddress &address, const void *data, nsapi_size_t size)
{
    auto counter = get_poll_counter();
    auto result = getNetSocket()->sendto(address, data, size);
    if (result == NSAPI_ERROR_WOULD_BLOCK) { 
        set_write_as_blocking(counter);
    }
    return result;
}

nsapi_size_or_error_t BSDSocket::sendmsg(const SocketAddress &address, const void *data, nsapi_size_t size, nsapi_msghdr_t* control, nsapi_size_t control_size)
{
    auto counter = get_poll_counter();
    auto result = getNetSocket()->sendto_control(address, data, size, control, control_size);
    if (result == NSAPI_ERROR_WOULD_BLOCK) { 
        set_write_as_blocking(counter);
    }
    return result;
}

nsapi_error_t BSDSocket::connect(const SocketAddress &address)
{
    auto counter = get_poll_counter();
    auto result = getNetSocket()->connect(address);
    if ((result == NSAPI_ERROR_OK || result == NSAPI_ERROR_UNSUPPORTED) && !is_blocking()) { 
        set_write_as_blocking(counter);
    }
    return result;
}

void BSDSocket::reset_flags()
{
    mbed::CriticalSectionLock lock;
    _flags = 0;
    _counter = 0;
}

bool BSDSocket::try_update_flags(flags_type flags, counter_type counter)
{
    mbed::CriticalSectionLock lock;
    if (counter != _counter) { 
        return false;
    } 

    _flags = flags;
    ++_counter;
    return true;
}

void BSDSocket::update_flags(flags_type flags)
{
    mbed::CriticalSectionLock lock;
    _flags = flags;
    ++_counter;
}

} // namespace mbed
