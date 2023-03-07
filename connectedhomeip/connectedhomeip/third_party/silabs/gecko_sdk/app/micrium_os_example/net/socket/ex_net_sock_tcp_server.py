 #!/usr/bin/env python
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

import asyncio
import time
import socket
import struct

class EchoServerClientProtocol(asyncio.Protocol):
    def connection_made(self, transport):
        self.transport = transport

        peername = transport.get_extra_info('peername')
        print('Connection from {}'.format(peername))

        sock = transport.get_extra_info('socket')
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack('ii', 1, 0))


    def data_received(self, data):
        message = data.decode()
        print('Data received: {!r}'.format(message))

        print('Send: {!r}'.format(message))
        self.transport.write(data)

        time.sleep(0.02)

        print('Close the client socket')
        self.transport.close()


loop = asyncio.get_event_loop()
# Each client connection will create a new protocol instance
coro = loop.create_server(EchoServerClientProtocol, '0.0.0.0', 10001)
server = loop.run_until_complete(coro)

# Serve requests until CTRL+c is pressed
print('Serving on {}'.format(server.sockets[0].getsockname()))
try:
    loop.run_forever()
except KeyboardInterrupt:
    pass

# Close the server
server.close()
loop.run_until_complete(server.wait_closed())
loop.close()
