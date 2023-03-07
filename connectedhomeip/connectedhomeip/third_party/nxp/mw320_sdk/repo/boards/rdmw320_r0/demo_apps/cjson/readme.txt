Overview
========
The cjson_construct example demonstrates how to construct and parse JSON using cJSON.
This example constructs a JSON stucture using cJSON and then print it to string,
then parses the string as JSON and checks whether the data is correct

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
No special settings.

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
When the demo runs successfully, the log would be seen on the Terminal like:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This is cJSON example.
This example constructs a JSON stucture using cJSON, print it to string and then parses the string as JSON and checks the info.
JSON created successfully:
{
	"sdk version":	2,
	"cJSON version":	"x.y.z",
	"example info":	{
		"category":	"demo_apps",
		"name":	"cjson"
	},
	"demo strings":	["This", "is", "cJSON", "demo"]
}
JSON parsed successfully.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
