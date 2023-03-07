
# -*- coding: utf-8 -*-

"""

Overview
--------
This Python package contains a static source generation of the peripheral,
register, fields, and enums found in the ARM `CMSIS System View Description (SVD)
XML <http://www.keil.com/pack/doc/CMSIS/SVD/html/index.html>`_.

The static source code provides auto-completion in the Python console to review
register map documentation interactively as well as read and write the part's
registers.

Features
--------
* Complete review of the peripherals, registers, fields, and enum documentation from \
  within the console

  .. code-block:: text

        rm.RAC
        0x40084000  RAC: RAC (74 registers)
            0x40084000  RXENSRCEN: RXEN Source Enable <read-write>
            0x40084004  STATUS: Radio State Machine Status <read-only>
            0x40084008  CMD: Radio Commands <write-only>
            0x4008400c  CTRL: Radio Control Register <read-write>
            0x40084010  FORCESTATE: Force state transition <read-write>

        rm.RAC.STATUS
            0x40084004  STATUS: Radio State Machine Status <read-only>
                [31] RXENS: RXEN Status <read-only>
                [30] TXENS: TXEN Status <read-only>
                [29] DEMODENS: Demodulator Enable Status <read-only>
                [27:24] STATE: Radio State <read-only>
                [21] TXAFTERFRAMEACTIVE: TX After Frame Active <read-only>
                [20] TXAFTERFRAMEPEND: TX After Frame Pending <read-only>
                [19] FORCESTATEACTIVE: FSM state force active <read-only>
                [15:0] RXMASK: Receive Enable Mask <read-only>

        rm.RAC.STATUS.STATE
            0x40084004 STATUS[27:24] contains field STATE: Radio State <read-only> ENUMERATED

        rm.RAC.STATUS.STATE.enum
                0  OFF: Radio is off
                1  RXWARM: Radio is enabling receiver
                2  RXSEARCH: Radio is listening for incoming frames
                3  RXFRAME: Radio is receiving a frame
                4  RXPD: Radio is powering down receiver and going to OFF state
                5  RX2RX: Radio is powering down receiver and going to OFF state
                6  RXOVERFLOW: Received data was lost due to full receive buffer
                7  RX2TX: Radio is disabling receiver and enabling transmitter
                8  TXWARM: Radio is enabling transmitter
                9  TX: Radio is transmitting data
                10  TXPD: Radio is powering down transmitter and going to OFF state
                11  TX2RX: Radio is disabling transmitter and enabling reception
                12  TX2TX: Radio is preparing for a transmission after the previous transmission was ended
                13  SHUTDOWN: Radio is powering down receiver and going to OFF state

* Register and field level read/writes by rm.<PERIPHERAL>.<REGISTER>.io and rm.<PERIPHERAL>.<REGISTER>.<FIELD>.io \
  properties
* Ability to read/write by the register and field names
* Ability to track written registers and/or fields with accessed flag logic
* Ability to get register or field names that are written
* Ability to get address by name and name by address
* Ability to get list of register names or register fields that have been written
* Ability to read/write/diff dictionaries of register/field names and values
* Ability to dump registers as a Python source dictionary, with detailed field notes with values and \
  corresponding enum name and description

  .. code-block:: py

        REGISTER_DUMP = {
        'RAC.STATUS': 0x00000000,
          # 0x40084004  STATUS: Radio State Machine Status <read-only>
            # [31] RXENS: 0x0
            # [30] TXENS: 0x0
            # [29] DEMODENS: 0x0
            # [27:24] STATE: 0x0 (enum OFF: Radio is off)
            # [21] TXAFTERFRAMEACTIVE: 0x0
            # [20] TXAFTERFRAMEPEND: 0x0
            # [19] FORCESTATEACTIVE: 0x0
            # [15:0] RXMASK: 0x0
        'RAC.CTRL': 0x00000000,
          # 0x4008400c  CTRL: Radio Control Register <read-write>
            # [30:27] PRSCLRSEL: 0x0 (enum PRSCH0: PRS Channel 0 selected as input)
            # [25:22] PRSTXENSEL: 0x0 (enum PRSCH0: PRS Channel 0 selected as input)
            # [20:17] PRSFORCETXSEL: 0x0 (enum PRSCH0: PRS Channel 0 selected as input)
            # [16] PRSFORCETX: 0x0
            # [14:11] PRSRXDISSEL: 0x0 (enum PRSCH0: PRS Channel 0 selected as input)
            # [10] PRSRXDIS: 0x0
            # [9] LNAENPOL: 0x0
            # [8] PAENPOL: 0x0
            # [7] ACTIVEPOL: 0x0
            # [6] TXPOSTPONE: 0x0
            # [5] PRSCLR: 0x0
            # [3] PRSMODE: 0x0
            # [2] TXAFTERRX: 0x0
            # [1] PRSTXEN: 0x0
            # [0] FORCEDISABLE: 0x0
        }

API
---
This section defines the top-level device class, common components, and
major interfaces to the components in the top-level device class.

.. automodule:: host_py_rm_studio_internal.full.efr32xg13.revA1.device

.. automodule:: static

"""

from . _version import __version__

from . static import RegisterMapInterface

from . device import *