############################
Musca-B1/S1 OTP Provisioning
############################

This document assumes that the user has set up the build environment, is
capable of compiling the TF-M project, and is in possession of at least one
Musca-S1 board, or a Musca-B1 board that has not been provisioned before.

*********************************
Provisioning on Musca HW variants
*********************************
Even though most of the process is identical between the boards, there is
a difference between them because of the underlying hardware.
While the B1 has actual OTP memory, the S1 only has volatile registers in its
place. Because a power cyclce wipes that area, the S1 merely imitates OTP
memory by copying provisioned data to its non-volatile MRAM.

Once the provisioning process is done, the CryptoCell-312 enters the Secure
Enabled lifecycle, which it recognizes by the contents of the OTP.
Debug and board reprogramming through USB is locked down in this state.
This lock can be forced open, but only with DAPLink firmware v34 or above.

Because the S1 "OTP" content is lost on a power cycle, it can not only be
provisioned several times through simply repeating the process, but it is also
unaffected by the lockdown.

For further information on the DAPLink firmware and update process, please
refer to the `Musca-B1 Technical Reference Manual
<https://developer.arm.com/docs/101312/0000>`__ or the `Musca-S1 Technical
Reference Manual <https://developer.arm.com/docs/101835/0000>`__.

Provisioning steps
==================
1. Build TF-M with -DCRYPTO_HW_ACCELERATOR_OTP_STATE=PROVISIONING given on
the CMake command line.

2. Load the tfm.hex file to the board and open the serial output.
Once you see something like the following snippet:

.. code-block:: bash

    ...
    First cycle: Attestation key is provisioned successfully
    First cycle: HUK is provisioned successfully
    Please reset the board to program ROTPK
    ...

3. Reset the board once by pressing the nSRST button and wait until you see
something like this:

::

    ...
    Second cycle: ROTPK is provisioned successfully
    Provisioning finished, reset the board to get secure enabled lifecycle
    ...

4. Reset the board once more.
If the provisioning process was succesful, you should see an output like:

::

    ...
    Board is in invalid lifecycle for provisioning: 5
    ...

Using the provisioned data
==========================
1. Build TF-M with -DCRYPTO_HW_ACCELERATOR_OTP_STATE=ENABLED given
on the CMake command line.

2. Load the tfm.hex file to
the board.

--------------

*Copyright (c) 2020, Arm Limited. All rights reserved.*