Instructions for ncp_daemon and empty_btmesh examples

Components for secure BGAPI communication
1. ncp_daemon host example
All security logic is handled by the security component on the host side.

2. empty_btmesh host example
Example host application for demonstrating BGAPI communication.

3. btmesh_ncp_empty Target sample app
NCP Target sample app

Security of BGAPI is implemented by encrypting the communication between NCP target and Host. To minimize the changes needed for the host application the security is implemented in a separate component (NCP Daemon).
The host application (empty_btmesh) runs in the separate task from the security component. This allows the different applications accessing the secure NCP easily. All security logic is handled by this security component.

Prerequisites for setting up secure BGAPI communication:
- You must have a POSIX/Mac, MSYS2 or Cygwin platform
- You must have openssl-devel package installed

Steps for setting up secure BGAPI communication

1.    NCP – btmesh_ncp_empty Target Sample app must be programmed to the EFR32 chip
    - Connect your WSTK to the PC
    - Open Simplicity Studio (with Bt Mesh SDK installed)
    - Select btmesh_ncp_empty Target sample app from Demos to flash your EFR32 device
    - Add Secure-ncp component to the example, then compile everything together, and flash it to the EFR32 device

2.   Compile and start ncp_daemon in a new terminal
    - Open a new terminal (on POSIX/Mac: any terminal; on Windows: Cygwin or MSYS2; Please note: Mingw32/64 won't work)
    - Navigate to app/bluetooth/example_host/ncp_daemon in the Bt Mesh SDK
    - Build the sample app by typing 'make'
    - run the sample app with appropriate parameters (eg  exe/ncp_daemon.exe /dev/ttyS13 115200 encrypted unencrypted )
        1st parameter: serial port
        2nd parameter: serial port speed
        3rd parameter: file descriptor for encrypted domain socket (it can be any string)
        4th parameter: file descriptor for unencrypted domain socket (it can be any string)

3.    Compile and start empty_btmesh in a new terminal
    - Open a new terminal (cygwin/msys2 terminal in windows platform)
    - Navigate to app/bluetooth/example_host/empty_btmesh in the Bt Mesh SDK
    - Build the sample app by typing 'make'
    - run the sample app with appropriate parameters (eg ./exe/empty_btmesh.exe -n ../ncp_daemon/encrypted )
        1st parameter: -n connect to a named socket instead of connecting the standard UART/TCP
        2nd parameter: file descriptor for encrypted or unencrypted  domain socket (it can be any string, but must match appropriate file descriptor string used in Step 2)
    - After running mesh-secure-ncp the 'Reset event' and the 'Node Initialized event' should arrive
    - Note that message encryption will happen in the ncp_daemon automatically by connecting to the encrypted socket.