Overview
========
The i2c_interrupt_b2b_transfer_slave example shows how to use i2c driver
as slave to do board to board transfer with master in interrupt way:

In this example, one i2c instance is used as slave and another i2c instance on the other board is used
as master. Slave receives a piece of data from master and sends them back, master checks whether the
data received from slave is correct.

Toolchain supported
===================
- IAR embedded Workbench  8.50.9
- GCC ARM Embedded  10.2.1

Hardware requirements
=====================
- Mini USB cable
- RDMW320-R0 board
- J-Link Debug Probe
- Personal Computer

Board settings
==============
The example is configured 2 instances I2C , one as I2C master , another as I2C slave.
The connection should be set as following:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Pin Name    Master Board   connect to     Pin Name    slave Board
I2C0_SDA    HD2   IO_4     ----------     I2C0_SDA    HD2   IO_4
I2C0_SCL    HD2   IO_5     ----------     I2C0_SCL    HD2   IO_5
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prepare the Demo
================
1.  Connect a USB cable between the host PC and Mini USB port on the target board.
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3. Download the program to the target board.
4. Launch the debugger in your IDE to begin running the example.

Running the demo
================
When the example runs successfully, you can see the similar information from the terminal as below.

~~~~~~~~~~~~~~~~~~~~~
I2C board2board interrupt example -- Slave transfer.


Slave received data :
0x 0  0x 1  0x 2  0x 3  0x 4  0x 5  0x 6  0x 7
0x 8  0x 9  0x a  0x b  0x c  0x d  0x e  0x f
0x10  0x11  0x12  0x13  0x14  0x15  0x16  0x17
0x18  0x19  0x1a  0x1b  0x1c  0x1d  0x1e  0x1f


End of I2C example .
~~~~~~~~~~~~~~~~~~~~~
