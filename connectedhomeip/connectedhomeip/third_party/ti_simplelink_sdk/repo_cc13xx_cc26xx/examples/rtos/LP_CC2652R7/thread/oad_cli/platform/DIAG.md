## Diagnostic module

Platform specific extensions to support FCC/ETSI testing with SmartRF Studio 7.

### Commands

 * [diag transmit](#diag-transmit-start)
 * [diag receive](#diag-receive-start)
 * [diag tone](#diag-tone-start)

### diag transmit start

Start a continuous transmission of raw frames.

### diag transmit start \[interval\] \[size\] \[count\]

Transmit raw frames from the radio. To conform with SmartRF Studio 7's packet
reception, the first two octets of the frame are a sequence number.

```
> diag transmit start 100 30 500
packet transmission is started
status 0x00
```

If `count` is not specified, then transmission will continue until stopped by
`diag transmit stop`.

If `size` or `interval` are not specified, the default values are used.

### diag transmit stop

Stop a previously started transmission operation.

```
> diag transmit stop
packet transmission is stopped
transmitted: 0x01F4
status 0x00
```

### diag receive start

Start reception of raw frames. The frames are assumed to conform with the
SmartRF Studio 7 format. This will track the first two octets as a sequence
number for Packet Error Rate testing.

```
> diag receive start
packet reception is started
status 0x00
```

### diag receive start \[count\]

Start reception and tracking of a specific number of frames.

```
> diag receive start 100
packet reception is started
status 0x00
```

### diag receive stop

Stops a currently running reception operation and prints out the PER results.

```
> diag receive stop
packet reception is stopped
received OK: 0x0064
received nOK: 0x0000
lost: 0x0000
Packet Error Rate: 0%
status 0x00
```

### diag tone start

Start an unmodulated TX test tone on the current channel with the current power.

```bash
> diag tone start
continuous unmodulated tone started
status 0x00
```

### diag tone start mod

Start an modulated TX test tone on the current channel with the current power.

```bash
> diag tone start mod
continuous modulated tone started
status 0x00
```

### diag tone stop

Stop transmission of the TX test tone.

```bash
> diag tone stop
continuous tone stopped
status 0x00
```

### diag shield start \<channel\>

Start shielding the stack from radio frames and discarding transmitted frames.

This diagnostic function is intended to allow for automation of the Thread Test
Harness. A set of the test cases require the DUT to be placed within a shield
chamber. This function will change the operating channel of the radio and
disallow the stack from switching channels. The idea is to change the channel
to an unoccupied channel to simulate a shield box.

This command and corresponding radio functions may be removed in the future in
favor of a physical isolation device.

### diag shield stop \<channel\>

End the shielding functionality started by `diag shield start`.

