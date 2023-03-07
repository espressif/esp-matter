.. _module-pw_sys_io_stm32cube:

-------------------
pw_sys_io_stm32cube
-------------------

``pw_sys_io_stm32cube`` implements the ``pw_sys_io`` facade over UART using
the STM32Cube HAL to support an assortment of boards from STMicroelectronics.

The UART baud rate is fixed at 115200 (8N1).

Setup
=====
This module requires relatively minimal setup:

  1. Write code against the ``pw_sys_io`` facade.
  2. Specify the ``dir_pw_sys_io_backend`` GN global variable to point to this
     backend.
  3. Call ``pw_sys_io_Init()`` during init so the UART is properly initialized
     and configured.

For devices other than the STM32F429I-DISC1, this module will need to be
configured to use the appropriate GPIO pins and USART peripheral.

Module Configuration Options
============================
The following configurations can be adjusted via compile-time configuration of
this module, see the
:ref:`module documentation <module-structure-compile-time-configuration>` for
more details.

.. c:macro:: PW_SYS_IO_STM32CUBE_USART_NUM

  The USART peripheral number to use. (1 for USART1, 2 for USART2, etc.)

.. c:macro:: PW_SYS_IO_STM32CUBE_GPIO_PORT

  The port that the USART peripheral TX/RX pins are on. (e.g. to use A9/A10
  pins for TX and RX, respectively, set this to A)

.. c:macro:: PW_SYS_IO_STM32CUBE_GPIO_TX_PORT

  The port for the USART peripheral TX pin, if different from
  PW_SYS_IO_STM32CUBE_GPIO_PORT.

.. c:macro:: PW_SYS_IO_STM32CUBE_GPIO_RX_PORT

  The port for the USART peripheral RX pin, if different from
  PW_SYS_IO_STM32CUBE_GPIO_PORT.

.. c:macro:: PW_SYS_IO_STM32CUBE_GPIO_TX_PIN

  The pin index to use for USART transmission within the port set by
  ``PW_SYS_IO_STM32CUBE_GPIO_PORT``.

.. c:macro:: PW_SYS_IO_STM32CUBE_GPIO_RX_PIN

  The pin index to use for USART reception within the port set by
  ``PW_SYS_IO_STM32CUBE_GPIO_PORT``.

.. c:macro:: PW_SYS_IO_STM32CUBE_GPIO_AF

  The alternate function index to use for USART reception within the port set by
  ``PW_SYS_IO_STM32CUBE_GPIO_PORT``.

.. c:macro:: PW_SYS_IO_STM32CUBE_USART_PREFIX

  The peripheral name prefix (either UART or USART) for the peripheral selected
  by ``PW_SYS_IO_STM32CUBE_USART_NUM``. Defaults to USART.

Module usage
============
After building an executable that utilizes this backend, flash the
produced .elf binary to the development board. Then, using a serial
communication terminal like minicom/screen (Linux/Mac) or TeraTerm (Windows),
connect to the device at a baud rate of 115200 (8N1). If you're not using a
STM32F429I-DISC1 development board, manually connect a USB-to-serial TTL adapter
to pins ``PA9`` (MCU TX) and ``PA10`` (MCU RX), making sure to match logic
levels (e.g. 3.3V versus 1.8V).

Sample connection diagram
-------------------------

.. code-block:: text

  --USB Serial--+    +-----STM32F429 MCU-----
                |    |
             TX o--->o PA10/USART1_RX
                |    |
             RX o<---o PA9/USART1_TX
                |    |
  --------------+    +-----------------------
