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

import sys
import socket
import binascii
import syslog


def main():
    interface = sys.argv[1]
    dst = sys.argv[2]
    port = sys.argv[3]
    payload = sys.argv[4]

    log = '====otbr-agent=send_udp===  send_udp.py started'
    syslog.syslog(syslog.LOG_ERR, log)
    print(log)

    log = 'dst %s' % dst
    syslog.syslog(syslog.LOG_ERR, log)
    print(log)

    log = 'interface %s' % interface
    syslog.syslog(syslog.LOG_ERR, log)
    print(log)

    log = 'port %s' % port
    syslog.syslog(syslog.LOG_ERR, log)
    print(log)

    log = 'payload %s' % payload
    syslog.syslog(syslog.LOG_ERR, log)
    print(log)

    byte_payload = binascii.unhexlify(payload)

    try:
        sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
        # Configure SO_BINDTODEVICE to bind the network interface.
        sock.setsockopt(socket.SOL_SOCKET, 25, interface.encode('utf-8'))
        sock.sendto(byte_payload, (dst, int(port)))
    except Exception as e:
        log = '====otbr-agent=send_udp=== sendto %s' % str(e)
        syslog.syslog(syslog.LOG_ERR, log)
        print(log)

    sock.close()
    log = '====otbr-agent=send_udp=== close'
    syslog.syslog(syslog.LOG_ERR, log)
    print(log)


if __name__ == '__main__':
    main()
