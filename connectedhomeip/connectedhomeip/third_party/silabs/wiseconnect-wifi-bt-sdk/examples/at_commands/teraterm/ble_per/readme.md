## BLE PER Mode Tera Term Script
This application demonstrates how to configure the RS9116W EVK in BLE PER mode.

Before continuing, ensure the RS9116W EVK is plugged into your computer and is connected via Tera Term as described in [Getting Started with PC using AT Commands](http://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-pc/). 

## Contents
  - [Setting up the RS9116](#setting-up)
  - [Transmit PER instructions](#transmit-per-instructions)
  - [Receive PER instructions](#receive-per-instructions)

## Setting up

**STEP 1.** Reset the RS9116W EVK.

**STEP 2.** In the Tera Term menu select `Control->Macro`.

![Tera Term Macro](./resources/tera-term-macro-1.png)
	
**STEP 3.** Navigate to the folder `<SDK>/examples/at_commands/teraterm/ble_per` containing the example script and select the file `ble_per.ttl`.
![Tera Term Script to be selected](./resources/tera-term-ble-per.png)

**STEP 4.** After selecting the `ble_per.ttl` script, a popup shows to indicate the BLE PER script is running. Two additional popups then show to indicate auto baud and the `opermode` command is successul.  
![Script execution start](./resources/script-start-pop-up.png)
![ABRD State](./resources/firmware-loading-done-2.png)
![opermode success](./resources/ble-opermode-3.png)

## Transmit PER Instructions
The following steps describe how to configure the RS9116W for BLE Transmit PER measurement.

**Step 1.** Enter `1` to run the script in transmit PER mode.
![select the tarnsmit/receive](./resources/ble-per-transmit-4.png)

**STEP 2.** Enter `1` to enable PER mode.
![Enable PER mode](./resources/ble-per-enable-5.png)

**Step 3.** Enter the 32-bit device address in hex format. In the following example, the access address of the BLE PER packet is `0x71764129`.
![Enter the device address](./resources/ble-devices-address-6.png)

**Step 4.** Enter the desired PHY data rate.
![Phy rate election](./resources/ble-phy-rate-7.png)

**Step 5.** Enter the Receive (Rx) and then Transmit (Tx) channel numbers.
![rx_channel_number](./resources/ble-rx-channel-8.png)

![tx_channel_number](./resources/ble-tx-channel-9.png)

**Step 6.** Enter the scrambler seed according to the PER mode.
![scrmbler-seed](./resources/ble-scrambler-seed-9.png)

**Step 7.** Enter the desired `le channel type`.
![Le_channel_type](./resources/ble-channel-type-10.png)

**Step 8.** Enter the desired `hopping type`.
![Hopping type](./resources/hopping-type-11.png)

**Step 9.** Enter the desired `antenna type`.
![Antenna_selection](./resources/ble-antenna-sel-11.png)

**Step 10.** Enter the desired `rf chain`.
![RFChain selection](./resources/ble-rf-chain-12.png)

**Step 11.** Enter the desired `payload type`.
![Packet Length](./resources/payload-type-13.png)

**Step 12.** Enter the desired `tx power`.
![tx_power_index](./resources/tx-power-index-14.png)

**Step 13.** Enter the desired PER transmission mode.
![tx_mode](./resources/ble-tx-mode-15.png)

**Step 14.** Enter the desired number of packets to transmit.
![number of packets](./resources/number-of-pkts.png)

With all inputs successfully entered, PER packet(s) are transmitted.  
![Final screen-shot](./resources/ble-transmit-executed-16.png)

### BLE CW-Mode Selection

**Step 15.** To run the BLE_CW mode all the parameters you need to enter as mentioned step by step procedure in above
method. But "Scrambler_seed" parameter you need to give input as 5.

![Scrambler_seed](./resources/ble-scrambler-cw-mode.png)

**Step 16.**  After "Scrambler_seed" parameter script will ask all the inputs as mentioned in above BLE_Trasmit method, you
need to enter .

**Step 17.** All the necessary inputs are completed , final input will ask for "Continuous Wave" selection you need to enter
2.Like as below pic.

![PER mode selection](./resources/ble-cw-mode-selection.png)

**Step 18.** After giving all the inputs BLE CW mode Transmit command executed successfully.

![CW mode success](./resources/ble-cw-mode-executed.png)

![CW mode Final](./resources/ble-cw-mode-final.png)

## Receive PER Instructions
The following steps describe how to configure the RS9116W for BLE Receive PER measurement.

**Step 1.** Enter `2` to run the script in PER receive mode.
![PER Receive](./resources/ble-per-rx-1.png)

**STEP 2.** Enter `1` to enable PER mode.
![Enable PER mode](./resources/ble-per-enable-5.png)

**Step 3.** Enter the 32-bit device address in hex format. In the following example, the access address of the BLE PER packet is `0x71764129`.

![Enter the device address](./resources/ble-devices-address-6.png)

**Step 4.** Enter the desired PHY data rate.
![Phy rate election](./resources/ble-phy-rate-7.png)

**Step 5.** Enter the Receive (Rx) and then Transmit (Tx) channel numbers.
![rx_channel_number](./resources/ble-rx-channel-8.png)
![tx_channel_number](./resources/ble-tx-channel-9.png)

**Step 6.** Enter the scrambler seed according to the PER mode.
![scrmbler-seed](./resources/ble-scrambler-seed-9.png)

**Step 7.** Enter the desired `le_channel type`.
![Le_channel_type](./resources/ble-channel-type-10.png)

**Step 8.** Enter the desired `hopping type`.
![Hopping type](./resources/hopping-type-11.png)

**Step 9.** Enter the desired `antenna type`.
![Antenna_selection](./resources/ble-antenna-sel-11.png)

**Step 10.** Enter the desired `rf_chain`.
![RFChain selection](./resources/ble-rf-chain-12.png)

**Step 11.** Enter the desired extended data length option.
![Extended data length ind](./resources/ble-extended-data-length.png)

**Step 12.** Enter the desired loop back mode option.
![Loop-back-mode](./resources/ble-loop-back-mode.png)

**Step 13.** Enter the desired duty cycling option.
![Duty cycling](./resources/ble-duty-cycling.png)

**Step 14.** With all inputs successfully entered, the RS9116W waits to receive packets.   
![Receive command](./resources/ble-receive-success.png)

![PER Stats command](./resources/ble-per-stats.png)

![Final per receive and stats command](./resources/ble-per-stats-final.png)

### BLE CW-Mode Selection

**Step 15.** To run the BLE_CW mode all the parameters you need to enter as mentioned step by step procedure in above
method. But "Scrambler_seed" parameter you need to give input as 5.

![Scrambler_seed](./resources/ble-scrambler-cw-mode.png)

**Step 16.**  After "Scrambler_seed" parameter script will ask all the inputs as mentioned in above BLE_Trasmit method, you
need to enter .

**Step 17.** All the necessary inputs are completed , final input will ask for "Continuous Wave" selection you need to enter
2.Like as below pic.

![PER mode selection](./resources/ble-cw-mode-selection.png)

**Step 18.** After giving all the inputs BLE PER CW mode Receive command executed and PER stats command also executes simultaneously.

![CW mode success](./resources/ble-cw-receive.png)

![CW mode Final](./resources/ble-cw-rx-stats-command.png)

Refer to the [BLE Programming Reference Manual](https://docs.silabs.com/rs9116/wiseconnect/rs9116w-ble-at-command-prm/latest/08-ble-commands) for further details on how to change the parameters.