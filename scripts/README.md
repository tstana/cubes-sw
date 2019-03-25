# value_conv.py

## Purpose

This script provides a means of converting Hamamatsu HVPS C11204 configuration
data between decimal and hex values.

## Use

The following choices are available:

1. Convert voltage from hex to decimal;
2. Convert voltage from decimal to hex;
3. Convert temperature from hex to decimal;
4. Convert temperature from decimal to hex.

The values output by this script can be used to make sense of the data actually
sent to or retrieved by the HVPS module.

# gen_asic_probes.py

## Purpose

This script can be used to generate an ASCII file that contains configuration
data for the Citiroc ASIC probes.

## Requirements

-	The program asks the user for the following input:
    - Enable out_probe?
    - If the answer to the previous is “Yes”:
        - out_probe Channel Number
        - out_probe input
    - Enable digital_probe?
    - If the answer to the previous is “Yes”:
        - Digital_probe Channel Number
        - Digital_probe input
    - Enable Out_probe_dac_5V?
    - If the answer to the previous is “Yes”:
        - Out_probe_dac_5V Channel Number:
- The program creates an output file containing 256 ASCII characters of 1’s and
0’s. A ‘1’ signals that the probe bit is set, a ‘0’ that it is cleared.
- The program ensures that only a single bit is set within bits [0..63],
[96..127], [160..223].
(Only a single block from the ASIC can be routed to the out_probe, not more.)
- The program ensures that only a single bit is set within bits [64..95],
[128..159]. (Only a single block from the ASIC can be routed to the
digital_probe, not more.)
- The program ensures that only a single bit is set within bits [224..255].
(Only a single block from the ASIC can be routed to the digital_probe, not more.)
- The program resides on the cubes-sw repository (https://github.com/tstana/cubes-sw)
