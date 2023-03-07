#!/usr/bin/env python3
#
#  Copyright (c) 2021, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#
#
#  This script generates and sends MLD Query message on the given interface.
#
#  Multicast Listener Query Message (RFC3810):
#
#   0                   1                   2                   3
#   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
#  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#  |  Type = 130   |      Code     |           Checksum            |
#  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#  |    Maximum Response Code      |           Reserved            |
#  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#  |                                                               |
#  *                                                               *
#  |                                                               |
#  *                       Multicast Address                       *
#  |                                                               |
#  *                                                               *
#  |                                                               |
#  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#  | Resv  |S| QRV |     QQIC      |     Number of Sources (N)     |
#  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#
#  IPv6 Router Alert option (RFC2711) in a Hop-by-Hop Options header:
#
#   0                   1                   2                   3
#   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
#  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#  |  Next Header  |  Hdr Ext Len  |0 0 0|0 0 1 0 1|0 0 0 0 0 0 1 0|
#  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#  |        Value (2 octets)       |             PadN              |
#  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#

import sys
import socket
import syslog
import struct

ICMP6_TYPE_MLD_QUERY = 130
IPV6_EXT_HBH_ROUTER_ALERT = 5

def main():
    interface = sys.argv[1]
    dst = sys.argv[2]

    log = '====otbr-agent=send_mld_query===  send_mld_query.py started'
    syslog.syslog(syslog.LOG_ERR, log)
    print(log)

    log = 'interface %s' % interface
    syslog.syslog(syslog.LOG_ERR, log)
    print(log)

    log = 'dst %s' % dst
    syslog.syslog(syslog.LOG_ERR, log)
    print(log)

    try:
        sock = socket.socket(socket.AF_INET6, socket.SOCK_RAW, socket.IPPROTO_ICMPV6)

        # Configure SO_BINDTODEVICE to bind the network interface.
        sock.setsockopt(socket.SOL_SOCKET, 25, interface.encode('utf-8'))
        sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_HOPS, 1)

        # Build ICMPv6 MLD Query message header
        query  = struct.pack("B", ICMP6_TYPE_MLD_QUERY) # Type: 130
        query += struct.pack("B", 0) # Code
        query += struct.pack("!H", 0) # Checksum

        # Query Response Interval: 10 seconds (RFC 3810, section 9.3)
        query += struct.pack("!H", 10000)

        # Reserved field
        query += struct.pack("!H", 0)

        # Multicast address (unspecified)
        query += struct.pack("!16s", ''.encode())

        # Querier's Robustness Variable: 2 (RFC 3810, section 9.1)
        query += struct.pack("B", 2)

        # Querier's Query Interval Code: 125 (RFC 3810, section 9.2)
        query += struct.pack("B", 125)

        # Number of sources
        query += struct.pack("!H", 0)

        # Build IPv6 Hop-by-Hop header
        ext_hdr  = struct.pack("B", socket.getprotobyname('ipv6-icmp'))
        ext_hdr += struct.pack("B", 0)

        # Include Router Alert option (RFC 2711) 
        ext_hdr += struct.pack("B", IPV6_EXT_HBH_ROUTER_ALERT)
        ext_hdr += struct.pack("B", 2)  # Length
        ext_hdr += struct.pack("!H", 0) # MLD message

        # Insert PadN option to keep alignment (2-bytes)
        ext_hdr += struct.pack("B", 1)
        ext_hdr += struct.pack("B", 0)

        # Send MLD Query message
        sock.sendmsg([query], [(socket.IPPROTO_IPV6, socket.IPV6_HOPOPTS, ext_hdr)], 0, (dst, 0, 0))

    except Exception as e:
        log = '====otbr-agent=send_mld_query=== sendmsg %s' % str(e)
        syslog.syslog(syslog.LOG_ERR, log)
        print(log)

    sock.close()
    log = '====otbr-agent=send_mld_query=== close'
    syslog.syslog(syslog.LOG_ERR, log)
    print(log)

if __name__ == '__main__':
    main()
