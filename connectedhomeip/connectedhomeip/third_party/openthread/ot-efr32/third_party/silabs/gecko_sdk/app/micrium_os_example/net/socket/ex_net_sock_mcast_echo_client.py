#! /usr/bin/env python
"""
 @file
 @brief TODO

 # License
 <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>

 The licensor of this software is Silicon Laboratories Inc. Your use of this
 software is governed by the terms of Silicon Labs Master Software License
 Agreement (MSLA) available at
 www.silabs.com/about-us/legal/master-software-license-agreement. This
 software is distributed to you in Source Code format and is governed by the
 sections of the MSLA applicable to Source Code.
"""
"""
********************************************************************************************************

                                       MULTICAST EXAMPLE

 Filename      : ex_net_sock_mcast_echo_client.py
********************************************************************************************************
 Description   : TODO

 Argument(s)   : IPv4 Address of the remote host

                 IPv4 Multicast group address

                 Remote Port

 Note(s)       : TODO
********************************************************************************************************
"""

#------------------
# MODULE TO IMPORT
#------------------
import time
import struct
import socket
import sys


#----------------------
# ARGUMENTS VALIDATION
#----------------------
if (len(sys.argv) > 3):
    sys.stderr.write('Invalid number of arguments!\n')
    sys.exit(-1)
#end if


GROUP       = sys.argv[1]
PORT        = sys.argv[2]

ANY         = "0.0.0.0"

# Datagram (udp) socket
try :
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print('Socket created')

except socket.error :
    print('Failed to create socket. Error Code : ' + str(socket.error[0]) + ' Message ' + socket.error[1])
    sys.exit()
#end try


try:
    # Set a timeout so the socket does not block indefinitely when trying to receive data.
    s.settimeout(1)

    # Set the time-to-live for messages to 1 so they do not go past the local network segment.
    ttl = struct.pack('b', 1)
    s.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)

    #The sender is bound on (0.0.0.0:1501)
    s.bind((ANY, int(PORT)))
    print("The sender is bound: (" + ANY + ":" + PORT + ")")

except socket.error :
    print('Failed to create socket. Error Code : ' + str(socket.error[0]) + ' Message ' + socket.error[1])
    s.close()
    sys.exit()
#end try


multicast_group = (GROUP, int(PORT))

while(1) :
    msg       = 'Hello World'
    msg_bytes =  bytes(msg, 'UTF-8')

    try :
        #Set the whole string
        s.sendto(msg_bytes, multicast_group)
        print("Message sent: " + msg)

        # receive data from client (data, addr)
        d     = s.recvfrom(1024)
        reply = d[0]
        addr  = d[1]
        print('Server reply : ' + reply.decode('UTF-8'))

    except socket.timeout as msg_error:
        print('Timeout')

    except socket.error as msg_error:
        print('Socket Error : ' + msg_error[0])
        s.close()
        sys.exit()
    #end try
#end while
