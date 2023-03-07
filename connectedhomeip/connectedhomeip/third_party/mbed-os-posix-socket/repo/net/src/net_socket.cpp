#include "BSDSocket.h"
#include "EventFileHandle.h"
#include "FdControlBlock.h"
#include "OpenFileHandleAsFileDescriptor.h"
#include <net_socket.h>
#include <rtos/EventFlags.h>
#include <netsocket/MsgHeader.h>

#include <net_if.h>
#include <ifaddrs.h>

#include "net_common.h"

#include "mbed-trace/mbed_trace.h"

using namespace mbed;
using namespace rtos;

#define TRACE_GROUP "NETS"

#define NO_FREE_SOCKET_SLOT (-1)

static BSDSocket sockets[MBED_NET_SOCKET_MAX_NUMBER];

static BSDSocket * getBSDSocket(int fd)
{
    BSDSocket * socket = static_cast<BSDSocket *>(mbed_file_handle(fd));

    if (socket == nullptr)
    {
        return nullptr;
    }

    return socket;
}

static Socket * getSocket(int fd)
{
    BSDSocket * socket = static_cast<BSDSocket *>(mbed_file_handle(fd));

    if (socket == nullptr)
    {
        return nullptr;
    }

    return socket->getNetSocket();
}

struct mbed_socket_option_t
{
    int level;
    int optname;
};

static mbed_socket_option_t convert_socket_option(int level, int optname)
{
    if (level == SOL_SOCKET) {
        switch (optname)
        {
        case SO_REUSEADDR:
            return { NSAPI_SOCKET, NSAPI_REUSEADDR };
        case SO_KEEPALIVE:
            return { NSAPI_SOCKET, NSAPI_KEEPALIVE };
        case SO_BROADCAST:
            return { NSAPI_SOCKET, NSAPI_BROADCAST };
        case SO_BINDTODEVICE: 
            return { NSAPI_SOCKET, NSAPI_BIND_TO_DEVICE };
        default:
            tr_warning("Passing unknown socket option %d to socket", optname);
            return { level, optname };
        }        
    } else if (level == IPPROTO_IP) { 
        switch (optname)
        {
        case IP_ADD_MEMBERSHIP:
            return { NSAPI_SOCKET, NSAPI_ADD_MEMBERSHIP };
        case IP_DROP_MEMBERSHIP:
            return { NSAPI_SOCKET, NSAPI_DROP_MEMBERSHIP };
        case IP_PKTINFO:
            return { NSAPI_SOCKET, NSAPI_PKTINFO };
        default:
            tr_warning("Passing unknown IP4 option %d to socket", optname);
            return { level, optname };
        }
    } else if (level == IPPROTO_IPV6) { 
        switch (optname)
        {
        case IPV6_ADD_MEMBERSHIP:
            return { NSAPI_SOCKET, NSAPI_ADD_MEMBERSHIP };
        case IPV6_DROP_MEMBERSHIP:
            return { NSAPI_SOCKET, NSAPI_DROP_MEMBERSHIP };
        case IPV6_RECVPKTINFO:
            return { NSAPI_SOCKET, NSAPI_PKTINFO };
        default:
            tr_warning("Passing unknown IP6 option %d to socket", optname);
            return { level, optname };
        }
    } else {
        tr_warning("Passing unknown option %d to socket", optname);
        return { level, optname };
    }
}

int getFreeSocketSlotIndex()
{
    int index = NO_FREE_SOCKET_SLOT;
    for (int i = 0; i < MBED_NET_SOCKET_MAX_NUMBER; i++)
    {
        if (!sockets[i].isSocketOpen())
        {
            index = i;
            break;
        }
    }
    return index;
}

int mbed_socket(int family, int type, int proto)
{
    tr_info("Create socket family %d type %d proto %d", family, type, proto);
    int index = getFreeSocketSlotIndex();
    if (index == NO_FREE_SOCKET_SLOT)
    {
        tr_err("No free socket slot");
        set_errno(ENOBUFS);
        return -1;
    }

    BSDSocket * socket = &sockets[index];
    return socket->open(family, type);
}

int mbed_socketpair(int family, int type, int proto, int sv[2])
{
    set_errno(EAFNOSUPPORT);
    return -1;
}

int mbed_shutdown(int fd, int how)
{
    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (how != SHUT_RD && how != SHUT_WR && how != SHUT_RDWR)
    {
        set_errno(EINVAL);
        return -1;
    }

    tr_info("Shutdown fd %d how %d", fd, how);

    switch (how)
    {
    case SHUT_RD:
        socket->enable_input(false);
        break;
    case SHUT_WR:
        socket->enable_output(false);
        break;
    case SHUT_RDWR:
        socket->enable_input(false);
        socket->enable_output(false);
        break;
    }

    return 0;
}

int mbed_bind(int fd, const struct sockaddr * addr, socklen_t addrlen)
{
    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (addr == nullptr)
    {
        set_errno(EINVAL);
        return -1;
    }

    if (socket->socketName)
    {
        set_errno(EINVAL);
        return -1;
    }

    SocketAddress sockAddr;
    if (convert_bsd_addr_to_mbed(&sockAddr, (struct sockaddr *) addr))
    {
        set_errno(EINVAL);
        return -1;
    }

    tr_info("Bind fd %d address %s port %d", fd, sockAddr.get_ip_address(), sockAddr.get_port());

    auto ret = socket->getNetSocket()->bind(sockAddr);
    if ((ret != NSAPI_ERROR_OK) && (ret != NSAPI_ERROR_UNSUPPORTED))
    {
        tr_err("Bind failed [%d]", ret);
        set_errno(EIO);
        return -1;
    }

    socket->socketName = sockAddr;

    return 0;
}

int mbed_connect(int fd, const struct sockaddr * addr, socklen_t addrlen)
{
    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (addr == nullptr)
    {
        set_errno(EINVAL);
        return -1;
    }

    SocketAddress sockAddr;
    if (convert_bsd_addr_to_mbed(&sockAddr, (struct sockaddr *) addr))
    {
        set_errno(EINVAL);
        return -1;
    }

    tr_info("Connect fd %d address %s", fd, sockAddr.get_ip_address());
    auto ret = socket->connect(sockAddr);
    if ((ret != NSAPI_ERROR_OK) && (ret != NSAPI_ERROR_UNSUPPORTED))
    {
        tr_err("Connect failed [%d]", ret);
        switch (ret)
        {
        case NSAPI_ERROR_IN_PROGRESS:
            set_errno(EALREADY);
            break;
        case NSAPI_ERROR_NO_SOCKET:
            set_errno(EBADF);
            break;
        case NSAPI_ERROR_IS_CONNECTED:
            set_errno(EISCONN);
            break;
        default:
            set_errno(EIO);
        }

        return -1;
    }

    if (!socket->is_blocking())
    {
        tr_debug("Connect not blocking\n");
        set_errno(EINPROGRESS);
        return -1;
    }

    return 0;
}

int mbed_listen(int fd, int backlog)
{
    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (backlog < 0)
    {
        set_errno(EINVAL);
        return -1;
    }

    if (socket->getSocketType() != BSDSocket::MBED_TCP_SOCKET)
    {
        set_errno(EOPNOTSUPP);
        return -1;
    }

    tr_info("Listen fd %d backlog %d", fd, backlog);
    auto ret = socket->listen(backlog);
    if ((ret != NSAPI_ERROR_OK) && (ret != NSAPI_ERROR_UNSUPPORTED))
    {
        tr_err("Listen failed [%d]", ret);
        set_errno(EIO);
        return -1;
    }

    return 0;
}

int mbed_accept(int fd, struct sockaddr * addr, socklen_t * addrlen)
{
    nsapi_error_t error;
    Socket * newSocket = nullptr;
    int index;
    SocketAddress sockAddr;

    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (addr == nullptr || addrlen == nullptr)
    {
        set_errno(EINVAL);
        return -1;
    }

    if (socket->getSocketType() != BSDSocket::MBED_TCP_SOCKET)
    {
        set_errno(EOPNOTSUPP);
        return -1;
    }

    tr_info("Connection accept for fd %d socket", fd);
    newSocket = socket->accept(&error);
    if (error == NSAPI_ERROR_WOULD_BLOCK) { 
        tr_debug("Socket fd %d: Accept would block", fd);
        set_errno(EWOULDBLOCK);
        return -1;
    }
    if ((error != NSAPI_ERROR_OK) && (error != NSAPI_ERROR_UNSUPPORTED))
    {
        tr_err("Accept failed [%d]", error);
        set_errno(ENOBUFS);
        return -1;
    }

    error = newSocket->getpeername(&sockAddr);
    if (error != NSAPI_ERROR_OK)
    {
        tr_err("Get peer name failed [%d]", error);
        delete newSocket;
        switch (error)
        {
        case NSAPI_ERROR_NO_SOCKET:
            set_errno(ENOTSOCK);
            break;
        case NSAPI_ERROR_NO_CONNECTION:
            set_errno(ECONNABORTED);
            break;
        default:
            set_errno(ENOBUFS);
        }
        return -1;
    }

    if (convert_mbed_addr_to_bsd(addr, &sockAddr))
    {
        delete newSocket;
        set_errno(EINVAL);
        return -1;
    }

    index = getFreeSocketSlotIndex();
    if (index == NO_FREE_SOCKET_SLOT)
    {
        tr_err("No free socket slot");
        delete newSocket;
        set_errno(ENOBUFS);
        return -1;
    }

    return sockets[index].open(addr->sa_family, BSDSocket::MBED_TCP_SOCKET, (InternetSocket *) newSocket);
}

ssize_t mbed_send(int fd, const void * buf, size_t len, int flags)
{
    ssize_t ret;
    bool blockingState;
    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (buf == nullptr)
    {
        set_errno(EINVAL);
        return -1;
    }

    if (!socket->is_output_enable())
    {
        return 0;
    }

    if (flags & MSG_DONTWAIT)
    {
        blockingState = socket->is_blocking();
        socket->set_blocking(false);
    }

    tr_info("Socket fd %d send %d bytes", fd, len);
    ret = socket->send(buf, len);
    if (ret < 0)
    {
        if (ret == NSAPI_ERROR_WOULD_BLOCK) { 
            tr_debug("Socket fd %d: Send would block", fd);
        } else { 
            tr_err("Send failed [%d]", ret);
        }
        switch (ret)
        {
        case NSAPI_ERROR_NO_SOCKET:
            set_errno(ENOTSOCK);
            break;
        case NSAPI_ERROR_WOULD_BLOCK:
            set_errno(EWOULDBLOCK);
            break;
        case NSAPI_ERROR_NO_ADDRESS:
            set_errno(ENOTCONN);
            break;
        default:
            set_errno(ENOBUFS);
        }
        ret = -1;
    }

    if (flags & MSG_DONTWAIT)
    {
        socket->set_blocking(blockingState);
    }

    return ret;
}

ssize_t mbed_sendto(int fd, const void * buf, size_t len, int flags, const struct sockaddr * dest_addr, socklen_t addrlen)
{
    ssize_t ret;
    bool blockingState;
    SocketAddress sockAddr;

    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (buf == nullptr)
    {
        set_errno(EINVAL);
        return -1;
    }

    if (!socket->is_output_enable())
    {
        return 0;
    }

    if (flags & MSG_DONTWAIT)
    {
        blockingState = socket->is_blocking();
        socket->set_blocking(false);
    }

    if (dest_addr != nullptr)
    {
        if (convert_bsd_addr_to_mbed(&sockAddr, (struct sockaddr *) dest_addr) < 0)
        {
            set_errno(EINVAL);
            return -1;
        }
    }

    tr_info("Socket fd %d send %d bytes to %s", fd, len, sockAddr.get_ip_address());
    ret = socket->sendto(sockAddr, buf, len);
    if (ret < 0)
    {
        if (ret == NSAPI_ERROR_WOULD_BLOCK) { 
            tr_debug("Socket fd %d: Send to would block", fd);
        } else { 
            tr_err("Send to failed [%d]", ret);
        }
        switch (ret)
        {
        case NSAPI_ERROR_NO_SOCKET:
            set_errno(ENOTSOCK);
            break;
        case NSAPI_ERROR_WOULD_BLOCK:
            set_errno(EWOULDBLOCK);
            break;
        default:
            set_errno(ENOBUFS);
        }
        ret = -1;
    }

    if (flags & MSG_DONTWAIT)
    {
        socket->set_blocking(blockingState);
    }

    return ret;
}

ssize_t mbed_sendmsg(int fd, const struct msghdr * message, int flags)
{
    ssize_t ret;
    bool blockingState;
    SocketAddress sockAddr;
    nsapi_pktinfo pkt_info;
    nsapi_msghdr_t* control = nullptr;
    nsapi_size_t control_size = 0;

    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (message == nullptr)
    {
        set_errno(EINVAL);
        return -1;
    }

    if (!socket->is_output_enable())
    {
        return 0;
    }

    if (message->msg_iovlen != 1) { 
        // FIXME: concatenate buffers of the message
        set_errno(ENOTSUP);
        return -1;
    }

    // Parse header and retrieve associated packet info
    for (cmsghdr * controlHdr = CMSG_FIRSTHDR(message); controlHdr != nullptr; controlHdr = CMSG_NXTHDR(message, controlHdr))
    {
        // Prepare the packet info header
        pkt_info.hdr.len = sizeof(pkt_info);
        pkt_info.hdr.level = NSAPI_SOCKET;
        pkt_info.hdr.type = NSAPI_PKTINFO;

        if (controlHdr->cmsg_level == IPPROTO_IP && controlHdr->cmsg_type == IP_PKTINFO)
        {
            struct in_pktinfo * inPktInfo = reinterpret_cast<struct in_pktinfo *> CMSG_DATA(controlHdr);
            pkt_info.ipi_ifindex = inPktInfo->ipi_ifindex;
            pkt_info.ipi_addr = SocketAddress(inPktInfo->ipi_spec_dst.s4_addr16, NSAPI_IPv4).get_addr();
            control = reinterpret_cast<decltype(control)>(&pkt_info);
            control_size = sizeof(pkt_info);
            tr_info("Send packet IPv4: src: %s, interface: %d", tr_array(pkt_info.ipi_addr.bytes, NSAPI_IPv4_BYTES), pkt_info.ipi_ifindex);
            break;
        } else if (controlHdr->cmsg_level == IPPROTO_IPV6 && controlHdr->cmsg_type == IPV6_PKTINFO)
        {
            struct in6_pktinfo * in6PktInfo = reinterpret_cast<struct in6_pktinfo *> CMSG_DATA(controlHdr);
            pkt_info.ipi_ifindex = in6PktInfo->ipi6_ifindex;
            pkt_info.ipi_addr = SocketAddress(in6PktInfo->ipi6_addr.s6_addr, NSAPI_IPv6).get_addr();
            control = reinterpret_cast<decltype(control)>(&pkt_info);
            control_size = sizeof(pkt_info);
            tr_info("Send packet IPv6: src: %s, interface: %d", tr_ipv6(pkt_info.ipi_addr.bytes), pkt_info.ipi_ifindex);
            break;
        }
    }

    if (flags & MSG_DONTWAIT)
    {
        blockingState = socket->is_blocking();
        socket->set_blocking(false);
    }

    if (convert_bsd_addr_to_mbed(&sockAddr, (struct sockaddr *) message->msg_name) < 0)
    {
        set_errno(EINVAL);
        return -1;
    }

    tr_info("Socket fd %d send message to %s", fd, sockAddr.get_ip_address());
    ret = socket->sendmsg(sockAddr, (void *) message->msg_iov[0].iov_base, message->msg_iov[0].iov_len, control, control_size);
    if (ret < 0)
    {
        if (ret == NSAPI_ERROR_WOULD_BLOCK) { 
            tr_debug("Socket fd %d: Send msg would block", fd);
        } else { 
            tr_err("Send msg failed [%d]", ret);
        }

        switch (ret)
        {
        case NSAPI_ERROR_NO_SOCKET:
            set_errno(ENOTSOCK);
            break;
        case NSAPI_ERROR_WOULD_BLOCK:
            set_errno(EWOULDBLOCK);
            break;
        default:
            set_errno(ENOBUFS);
        }
    }

    if (flags & MSG_DONTWAIT)
    {
        socket->set_blocking(blockingState);
    }

    return ret;
}

ssize_t mbed_recv(int fd, void * buf, size_t max_len, int flags)
{
    ssize_t ret;
    bool blockingState;

    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (buf == nullptr)
    {
        set_errno(EINVAL);
        return -1;
    }

    if (!socket->is_input_enable())
    {
        return 0;
    }

    if (flags & MSG_DONTWAIT)
    {
        blockingState = socket->is_blocking();
        socket->set_blocking(false);
    }

    ret = socket->recv(buf, max_len);
    if (ret < 0)
    {
        if (ret == NSAPI_ERROR_WOULD_BLOCK) { 
            tr_debug("Socket fd %d: Receive would block", fd);
        } else { 
            tr_err("Receive failed [%d]", ret);
        }
        switch (ret)
        {
        case NSAPI_ERROR_NO_SOCKET:
            set_errno(ENOTSOCK);
            break;
        case NSAPI_ERROR_WOULD_BLOCK:
            set_errno(EWOULDBLOCK);
            break;
        default:
            set_errno(ENOBUFS);
        }
        ret = -1;
    }
    else
    {
        SocketAddress peerAddr;
        socket->getNetSocket()->getpeername(&peerAddr);
        tr_info("Socket fd %d received %d bytes from %s", fd, ret, peerAddr.get_ip_address());
    }

    if (flags & MSG_DONTWAIT)
    {
        socket->set_blocking(blockingState);
    }

    return ret;
}

ssize_t mbed_recvfrom(int fd, void * buf, size_t max_len, int flags, struct sockaddr * src_addr, socklen_t * addrlen)
{
    ssize_t ret;
    bool blockingState;
    SocketAddress sockAddr;

    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (buf == nullptr)
    {
        set_errno(EINVAL);
        return -1;
    }

    if (!socket->is_input_enable())
    {
        return 0;
    }

    if (flags & MSG_DONTWAIT)
    {
        blockingState = socket->is_blocking();
        socket->set_blocking(false);
    }

    ret = socket->recvfrom(&sockAddr, buf, max_len);
    if (ret < 0)
    {
        if (ret == NSAPI_ERROR_WOULD_BLOCK) { 
            tr_debug("Socket fd %d: Receive would block", fd);
        } else { 
            tr_err("Receive failed [%d]", ret);
        }
        switch (ret)
        {
        case NSAPI_ERROR_NO_SOCKET:
            set_errno(ENOTSOCK);
            break;
        case NSAPI_ERROR_WOULD_BLOCK:
            set_errno(EWOULDBLOCK);
            break;
        default:
            set_errno(ENOBUFS);
        }
        ret = -1;
    }
    else
    {
        tr_info("Socket fd %d recevied %d bytes from %s", fd, ret, sockAddr.get_ip_address());
        if (src_addr != nullptr)
        {
            if (convert_mbed_addr_to_bsd(src_addr, &sockAddr))
            {
                set_errno(EINVAL);
                ret = -1;
            }
        }
    }

    if (flags & MSG_DONTWAIT)
    {
        socket->set_blocking(blockingState);
    }

    return ret;
}

ssize_t mbed_recvmsg(int fd, struct msghdr * message, int flags)
{
    bool blockingState;
    SocketAddress sockAddr;
    nsapi_pktinfo_t pkt_info;
    nsapi_msghdr_t* control = nullptr;
    nsapi_size_t control_size = 0;

    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (message == nullptr)
    {
        set_errno(EINVAL);
        return -1;
    }

    if (!socket->is_input_enable())
    {
        return 0;
    }

    if (message->msg_iovlen != 1) { 
        // FIXME: concatenate buffers of the message
        set_errno(ENOTSUP);
        return -1;
    }

    if (message->msg_control && message->msg_controllen >= std::max(sizeof(in_pktinfo), sizeof(in6_pktinfo))) { 
        control = reinterpret_cast<decltype(control)>(&pkt_info);
        control_size = sizeof(pkt_info);
        memset(control, 0, control_size);
        memset(message->msg_control, 0, message->msg_controllen);
    }

    if (flags & MSG_DONTWAIT)
    {
        blockingState = socket->is_blocking();
        socket->set_blocking(false);
    }

    auto ret = socket->recvmsg(&sockAddr, (void *) message->msg_iov[0].iov_base, message->msg_iov[0].iov_len, control, control_size);
    if (ret < 0)
    {
        if (ret == NSAPI_ERROR_WOULD_BLOCK) { 
            tr_debug("Socket fd %d: Receive msg would block", fd);
        } else { 
            tr_err("Receive mag failed [%d]", ret);
        }
        switch (ret)
        {
        case NSAPI_ERROR_NO_SOCKET:
            set_errno(ENOTSOCK);
            break;
        case NSAPI_ERROR_WOULD_BLOCK:
            set_errno(EWOULDBLOCK);
            break;
        default:
            set_errno(ENOBUFS);
        }
    }
    tr_info("Socket fd %d received %d bytes message from %s", fd, ret, sockAddr.get_ip_address());

    if (ret != -1)
    {
        if (message->msg_name != nullptr)
        {
            if (convert_mbed_addr_to_bsd((sockaddr *) message->msg_name, &sockAddr))
            {
                set_errno(EINVAL);
                ret = -1;
            }
        }
    }

    if (flags & MSG_DONTWAIT)
    {
        socket->set_blocking(blockingState);
    }

    nsapi_pktinfo_t *pkt_info_ptr = nullptr;
    if (control) {
        MsgHeaderIterator it(control, control_size);
        while (it.has_next()) { 
            auto *hdr = it.next();
            if (hdr->level == NSAPI_SOCKET && hdr->type == NSAPI_PKTINFO) { 
                pkt_info_ptr = reinterpret_cast<nsapi_pktinfo_t*>(hdr);
                break;
            }
        }
    }

    // retrieve packet info and fill it in msg_control
    if (control && ret >= 0 && pkt_info_ptr) {
        pkt_info = *pkt_info_ptr;
        struct cmsghdr * hdr = CMSG_FIRSTHDR(message);

        if (pkt_info.ipi_addr.version == NSAPI_IPv4) { 
            hdr->cmsg_level = IPPROTO_IP;
            hdr->cmsg_type  = IP_PKTINFO;
            hdr->cmsg_len   = CMSG_LEN(sizeof(in_pktinfo));

            struct in_pktinfo * pktInfo = reinterpret_cast<struct in_pktinfo *> CMSG_DATA(hdr);
            pktInfo->ipi_ifindex  = static_cast<decltype(pktInfo->ipi_ifindex)>(pkt_info.ipi_ifindex);
            memcpy(pktInfo->ipi_spec_dst.s4_addr, pkt_info.ipi_addr.bytes, sizeof(pktInfo->ipi_spec_dst.s4_addr));

            message->msg_controllen = CMSG_SPACE(sizeof(in_pktinfo));
            tr_debug("Received packet: src: %s, interface: %d", tr_array(pkt_info.ipi_addr.bytes, NSAPI_IPv4_BYTES), pkt_info.ipi_ifindex);
        } else if (pkt_info.ipi_addr.version == NSAPI_IPv6) {
            hdr->cmsg_level = IPPROTO_IPV6;
            hdr->cmsg_type  = IPV6_PKTINFO;
            hdr->cmsg_len   = CMSG_LEN(sizeof(in6_pktinfo));

            struct in6_pktinfo * pktInfo = reinterpret_cast<struct in6_pktinfo *> CMSG_DATA(hdr);
            pktInfo->ipi6_ifindex  = static_cast<decltype(pktInfo->ipi6_ifindex)>(pkt_info.ipi_ifindex);
            memcpy(pktInfo->ipi6_addr.s6_addr, pkt_info.ipi_addr.bytes, sizeof(pktInfo->ipi6_addr.s6_addr));

            message->msg_controllen = CMSG_SPACE(sizeof(in6_pktinfo));
            tr_debug("Received packet: src: %s, interface: %d", tr_ipv6(pkt_info.ipi_addr.bytes), pkt_info.ipi_ifindex);
        }
    }

    return ret;
}

int mbed_getsockopt(int fd, int level, int optname, void * optval, socklen_t * optlen)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    // Use NSAPI options instead of POSIX one
    auto opt = convert_socket_option(level, optname);
    auto ret = socket->getsockopt(opt.level, opt.optname, optval, optlen);
    if (ret < 0)
    {
        tr_err("Get socket option %s [%d]", ret == NSAPI_ERROR_UNSUPPORTED ? "unsupported" : "failed", ret);
        switch (ret)
        {
        case NSAPI_ERROR_NO_SOCKET:
            set_errno(ENOTSOCK);
            break;
        case NSAPI_ERROR_UNSUPPORTED:
            if ((optname == SO_ERROR) || (optval != nullptr))
            {
                *(int *) optval = 0;
                return 0;
            }
            set_errno(ENOPROTOOPT);
            break;
        default:
            set_errno(ENOBUFS);
        }
        return -1;
    }
    tr_info("Get socket fd %d option level %d optname %d", fd, level, optname);
    return 0;
}

int mbed_setsockopt(int fd, int level, int optname, const void * optval, socklen_t optlen)
{
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    // Convert the option to NSAPI option alias
    auto opt = convert_socket_option(level, optname);

    int ret = -1;

    // Handle the conversion of arguments for options that requires it
    if (level == IPPROTO_IP && ((optname == IP_ADD_MEMBERSHIP) || (optname == IP_DROP_MEMBERSHIP))) {
        if (optval == nullptr || optlen != sizeof(ip_mreq)) { 
            tr_err("Set socket option invalid ip_mreq: level = %d, optname=%d, val=%p, len=%d => [%d]",
                    level, optname, optval, optlen, ret);
            set_errno(EINVAL);
            return -1;
        } 
        const ip_mreq* bsd_opt = reinterpret_cast<const ip_mreq*>(optval);
        nsapi_ip_mreq_t opt_val = {};
        opt_val.imr_multiaddr.version = NSAPI_IPv4;
        memcpy(opt_val.imr_multiaddr.bytes, bsd_opt->imr_multiaddr.s4_addr, sizeof(bsd_opt->imr_multiaddr.s4_addr));
        opt_val.imr_interface.version = NSAPI_IPv4;
        memcpy(opt_val.imr_interface.bytes, bsd_opt->imr_interface.s4_addr, sizeof(bsd_opt->imr_interface.s4_addr));

        ret = socket->setsockopt(opt.level, opt.optname, &opt_val, sizeof(opt_val));
    } else if (level == IPPROTO_IPV6 && ((optname == IPV6_ADD_MEMBERSHIP) || (optname == IPV6_DROP_MEMBERSHIP))) {
        if (optval == nullptr || optlen != sizeof(ipv6_mreq)) { 
            tr_err("Set socket option invalid ipv6_mreq: level = %d, optname=%d, val=%p, len=%d expected len=%d => [%d]",
                    level, optname, optval, optlen, sizeof(ipv6_mreq), ret);
            set_errno(EINVAL);
            return -1;
        } 
        const ipv6_mreq* bsd_opt = reinterpret_cast<const ipv6_mreq*>(optval);

        // Initialize the socket option and copy the multicast address in it 
        nsapi_ip_mreq_t opt_val = {};
        opt_val.imr_multiaddr.version = NSAPI_IPv6;
        memcpy(opt_val.imr_multiaddr.bytes, bsd_opt->ipv6mr_multiaddr.s6_addr, sizeof(bsd_opt->ipv6mr_multiaddr.s6_addr));

        // The POSIX and Mbed socket differ from here: The POSIX socket API contains 
        // the interface ID while the Mbed API contains the interface IP address.
        // The IP address of the interface is retrieved with the if_ functions.

        // Retrieve the interface name 
        char ifname[IF_NAMESIZE];
        if (if_indextoname(bsd_opt->ipv6mr_interface, ifname) == nullptr) {
            tr_error("Cannot retrieve network interface %d", bsd_opt->ipv6mr_interface);
            set_errno(EINVAL);
            return -1;
        }

        // Retrieve the interface address 
        struct ifaddrs* ifap;
        if (mbed_getifaddrs(&ifap)) { 
            tr_error("Cannot retrieve list of network interfaces");
            set_errno(EINVAL);
            return -1;
        }

        while (ifap) { 
            if (ifap->ifa_name && strcmp(ifap->ifa_name, ifname) == 0 && 
                ifap->ifa_addr && ifap->ifa_addr->sa_family == AF_INET6
            ) { 
                opt_val.imr_interface.version = NSAPI_IPv6;
                struct sockaddr_in6* addr = reinterpret_cast<struct sockaddr_in6*>(ifap->ifa_addr);
                memcpy(opt_val.imr_interface.bytes, addr->sin6_addr.s6_addr, sizeof(addr->sin6_addr.s6_addr));
                tr_debug("Sending interface address %s as part of socket option", 
                        SocketAddress((void*)opt_val.imr_interface.bytes, NSAPI_IPv6).get_ip_address()
                );
                break;
            }
            ifap = ifap->ifa_next;
        }
        mbed_freeifaddrs(ifap);

        // Return of the ip of the interface hasn't been set
        if (opt_val.imr_interface.version != NSAPI_IPv6) { 
            tr_error("Cannot retrieve IPv6 address for interface %d", bsd_opt->ipv6mr_interface);
            set_errno(EINVAL);
            return -1;
        }

        ret = socket->setsockopt(opt.level, opt.optname, &opt_val, sizeof(opt_val));
    }else {
        ret = socket->setsockopt(opt.level, opt.optname, optval, optlen);
    }

    if (ret < 0)
    {
        tr_err("Set socket option %s: level = %d, optname=%d, val=%p, len=%d => [%d]",
               ret == NSAPI_ERROR_UNSUPPORTED ? "unsupported" : "failed", level, optname, optval, optlen, ret);
        switch (ret)
        {
        case NSAPI_ERROR_NO_SOCKET:
            set_errno(ENOTSOCK);
            break;
        case NSAPI_ERROR_UNSUPPORTED:
            set_errno(ENOPROTOOPT);
            break;
        default:
            set_errno(ENOBUFS);
        }
        return -1;
    }
    tr_info("Set socket fd %d option level %d optname %d", fd, level, optname);
    return 0;
}

int mbed_getsockname(int fd, struct sockaddr * addr, socklen_t * addrlen)
{
    auto * socket = getBSDSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (addr == nullptr || addrlen == nullptr)
    {
        set_errno(EINVAL);
        return -1;
    }

    if (socket->socketName)
    {
        set_errno(EINVAL);
        return -1;
    }

    tr_info("Get socket fd %d name", fd);
    if (socket->socketName.get_ip_version() == NSAPI_IPv4)
    {
        if (*addrlen < sizeof(sockaddr_in))
        {
            *addrlen = sizeof(sockaddr_in);
            set_errno(ENOBUFS);
            return -1;
        }
    }
    else if (socket->socketName.get_ip_version() == NSAPI_IPv6)
    {
        if (*addrlen < sizeof(sockaddr_in6))
        {
            *addrlen = sizeof(sockaddr_in6);
            set_errno(ENOBUFS);
            return -1;
        }
    }

    if (convert_mbed_addr_to_bsd(addr, &socket->socketName) < 0)
    {
        set_errno(ENOBUFS);
        return -1;
    }

    return 0;
}

int mbed_getpeername(int fd, struct sockaddr * addr, socklen_t * addrlen)
{
    SocketAddress sockAddr;
    auto * socket = getSocket(fd);
    if (socket == nullptr)
    {
        set_errno(EBADF);
        return -1;
    }

    if (addr == nullptr || addrlen == nullptr)
    {
        set_errno(EINVAL);
        return -1;
    }

    auto ret = socket->getpeername(&sockAddr);
    if (ret < 0)
    {
        tr_err("Get peer name failed [%d]", ret);
        switch (ret)
        {
        case NSAPI_ERROR_NO_SOCKET:
            set_errno(ENOTSOCK);
            break;
        case NSAPI_ERROR_NO_CONNECTION:
            set_errno(ENOTCONN);
            break;
        default:
            set_errno(ENOBUFS);
        }

        return -1;
    }
    tr_info("Get socket fd %d peer name", fd);
    convert_mbed_addr_to_bsd(addr, &sockAddr);
    return 0;
}

static int get_max_select_fd(int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds)
{
    int max;

    max = nfds;

    for (int fd = max; fd < FD_SETSIZE; ++fd)
    {
        if (FD_ISSET(fd, readfds) || FD_ISSET(fd, writefds) || FD_ISSET(fd, exceptfds))
        {
            max++;
        }
    }

    return max;
}

int mbed_select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval * timeout)
{
    nfds = get_max_select_fd(nfds, readfds, writefds, exceptfds);

    auto control_blocks = std::unique_ptr<FdControlBlock[]>{ new (std::nothrow) FdControlBlock[nfds] };
    if (!control_blocks)
    {
        errno = ENOMEM;
        return -1;
    }
    size_t fd_count = 0;
    rtos::EventFlags flag;
    const uint32_t event_flag = 1;
    int fd_processed          = 0;

    // Convert input into FdControlBlock which are more manageable.
    for (int i = 0; i < nfds; ++i)
    {
        auto cb = FdControlBlock(i, readfds, writefds, exceptfds);
        if (cb.handle)
        {
            control_blocks[fd_count] = cb;
            ++fd_count;
        }
    }

    // Install handler
    bool must_wait = true;
    for (size_t i = 0; i < fd_count; ++i)
    {
        auto & cb = control_blocks[i];
        if (cb.poll())
        {
            // One event is set, we don't need to wait to process the FD
            must_wait = false;
            break;
        }
        else
        {
            cb.handle->sigio([&cb, &flag]() {
                if (cb.poll())
                {
                    flag.set(event_flag);
                }
            });
        }
    }

    // Wait operation
    if (fd_count && must_wait)
    {
        if (!timeout)
        {
            // Wait forever
            flag.wait_any(event_flag);
        }
        else if (timeout->tv_sec || timeout->tv_usec)
        {
            // wait for the expected
            rtos::Kernel::Clock::duration_u32 duration{ timeout->tv_sec * 1000 + timeout->tv_usec / 1000 };
            flag.wait_any_for(event_flag, duration);
        }
        else
        {
            // No timeout value set and no file descriptor ready, return
            // immediately, no fd processed
            for (size_t i = 0; i < fd_count; ++i)
            {
                auto & cb = control_blocks[i];
                cb.handle->sigio(nullptr);
            }

            // Update output file descriptors
            for (auto & fds : { readfds, writefds, exceptfds })
            {
                if (fds)
                {
                    FD_ZERO(fds);
                }
            }

            return fd_processed;
        }
    }

    // Update output file descriptors
    for (auto & fds : { readfds, writefds, exceptfds })
    {
        if (fds)
        {
            FD_ZERO(fds);
        }
    }

    // Update fds watch and watch list
    for (size_t i = 0; i < fd_count; ++i)
    {
        auto & cb   = control_blocks[i];
        auto events = cb.poll();
        if (cb.read && (events & POLLIN))
        {
            FD_SET(cb.fd, readfds);
            ++fd_processed;
        }
        if (cb.write && (events & POLLOUT))
        {
            FD_SET(cb.fd, writefds);
            ++fd_processed;
        }
        if (cb.err && (events & POLLERR))
        {
            FD_SET(cb.fd, exceptfds);
            ++fd_processed;
        }
        // remove temporary sigio
        cb.handle->sigio(nullptr);
    }

    return fd_processed;
}

int mbed_eventfd(unsigned int initval, int flags)
{
    if (initval || flags)
    {
        return -1;
    }
    return open_fh_as_fd<EventFileHandle>();
}

int mbed_eventfd_read(int fd, eventfd_t * value)
{
    return read(fd, value, sizeof(*value));
}

int mbed_eventfd_write(int fd, eventfd_t value)
{
    return write(fd, &value, sizeof(value));
}
