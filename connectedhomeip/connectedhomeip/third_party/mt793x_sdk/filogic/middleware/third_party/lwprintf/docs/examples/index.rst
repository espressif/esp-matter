.. _examples:

Examples and demos
==================

Various examples are provided for fast library evaluation on embedded systems. These are prepared and maintained for ``2`` platforms, but could be easily extended to more platforms:

* WIN32 examples, prepared as `Visual Studio Community <https://visualstudio.microsoft.com/vs/community/>`_ projects
* ARM Cortex-M examples for STM32, prepared as `STM32CubeIDE <https://www.st.com/en/development-tools/stm32cubeide.html>`_ GCC projects

.. warning::
	Library is platform independent and can be used on any platform.

Debug for STM32L4
*****************

Simple example is available, that runs on *STM32L432KC-Nucleo* board and shows basic confiuration for library.
On-board *Virtual-COM-Port* through embedded ST-Link provides communication to MCU via UART peripheral.

Output function writes data to PC using `USART2` hardware IP.

.. toctree::
    :maxdepth: 2
