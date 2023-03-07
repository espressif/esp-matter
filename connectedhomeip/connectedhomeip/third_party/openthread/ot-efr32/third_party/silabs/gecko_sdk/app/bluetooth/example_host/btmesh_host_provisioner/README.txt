Host Provisioner Example

The Host Provisioner example demonstrates the ability of using a NCP node connected to a PC as a provisioner.
Through this node the user can provision, configure and reset other nodes.
The BT Mesh network is created and handled by the NCP node so network management options are available as well.

Components required:
1. btmesh_host_provisioner host example
Example host application for demonstrating provisioner capabilities.

2. btmesh_ncp_empty target sample app
NCP Target sample app on the provisioner node.
Note: EFR32xG22 boards are not supported


Prerequisites for setting up the communication between host and target:
    - Must have a POSIX/Mac, MSYS2 or MinGW64 platform


Steps for setting up the communication:
1. NCP target
    – btmesh_ncp_empty Target sample app must be programmed to the EFR32 chip:
        - Connect your WSTK to the PC
        - Open Simplicity Studio (with Bt Mesh SDK installed)
        - Select btmesh_ncp_empty Target sample app from Demos to flash your EFR32 device
	
2. btmesh_host_provisioner
    - Open a new terminal (MSYS2 or MinGW64 terminal on Windows)
    - Navigate to app/bluetooth/example_host/btmesh_host_provisioner in Bt Mesh SDK
    - Build the sample app by typing 'make'
    - Run the sample app with appropriate parameters (e.g. ./exe/btmesh_host_provisioner.exe -u /dev/ttyS4 --scan)
        1st parameter: -u selects connecting to an UART serial port. Use -t for TCP/IP connection.
            note: on Windows, MinGW64 uses COMx, while MSYS2 and POSIX systems require /dev/ttyX
		2nd parameter: --scan selects the scanning function to check if there are unprovisioned beaconing nodes available


Usage:
The Host Provisioner example has two modes: CLI and UI.
1. UI mode
    The UI mode is accessible by starting the program without any Host Provisioner-related arguments,
    e.g. btmesh_host_provisioner.exe -u COM5.
    The user can choose from several commands in one session without exiting. In this case the host example's
    database stores information about the nodes in the BT Mesh network and those found during scanning.
    This database is updated while the program is running, but it is not guaranteed that it will be the same in the next run.

    On Windows using MinGW64, winpty might be required for user input handling. In this case
    just run the program as e.g. 'winpty exe/btmesh_host_provisioner.exe -u COM5'

2. CLI mode
    The CLI mode is one command per run, selected by the appropriate parameter.
    In this scenario the host database is not preserved between runs, but node identifiers (e.g. UUID) stay
    the same provided the affected node has not been reset in the meantime. This means that e.g. a UUID
    found in a scanning session can be used in a provisioning session.

    The following options are available for CLI mode:
    --scan               Scan for unprovisioned beaconing devices
    --provision <UUID>   Provision and configure the selected device
    --nodelist           List all nodes known to the provisioner (i.e. present in the provisioner's device database (DDB)
    --nodeinfo <UUID>    Print DCD information about the selected device
    --remove <UUID>      Unprovision the selected device
    --reset              Reset the provisioner to its factory state. It is recommended to remove all known devices
                         before resetting
    --help               Print a help message about usage