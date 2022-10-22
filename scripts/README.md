# CUBES General-Purpose Scripts

This folder is a collection of scripts for general-purpose use on CUBES. Some
of the scripts are documented below.

- [value_conv.py](#value_convpy)
  - [Purpose](#purpose)
  - [Usage](#usage)
- [hk-from-dat.py](#hk-from-datpy)
  - [Purpose](#purpose-1)
  - [Usage](#usage-1)
 
## value_conv.py

### Purpose

This script provides a means of converting Hamamatsu HVPS C11204 configuration
data between decimal and hex values.

### Usage

The following choices are available:

1. Convert voltage from hex to decimal;
2. Convert voltage from decimal to hex;
3. Convert temperature from hex to decimal;
4. Convert temperature from decimal to hex.

The values output by this script can be used to make sense of the data actually
sent to or retrieved by the HVPS module.

## hk-from-dat.py

### Purpose

This script parses and prints fields in the histogram header from DAQ _.dat_ files.

### Usage

This script requires Python 3 to run.

```
usage: hk-from-dat.py [-h] filename

Print out HK values from histogram header.

positional arguments:
  filename    DAQ .dat file

optional arguments:
  -h, --help  show this help message and exit
```

Process to run it from Windows:

1. For ease of use, copy the `hk-from-dat.py` script to the folder containing data
   files.
1. Open up a PowerShell window in the data files folder (_Shift + right-click_ in a
   Windows Explorer window, then click **Open PowerShell window here**).
1. Type in `python .\hk-from-dat.py .\<daq-file-name.dat>`.
   - Note: You can use TAB completion or copy-and-paste in PowerShell to avoid from
     typing everything out, e.g., type in `python hk-`, then hit the TAB key and
     PowerShell should fill in the `.\hk-from-dat.py` for you.
1. If the DAQ file is formatted correctly, the contents of the histogram header are
   printed to `stdout`:
   
   ```
   PS C:\Projects\cubes\cubes-sw\scripts> python hk-from-dat.py dataCITI_2022-10-12_14-06-42.dat
   
   Reading from data file ".\dataCITI_2022-10-12_14-06-42.dat"...
   
   Board ID: B2
   
   DAQ start...
     Unix time     : 1665583576
     Citiroc temp. : 31.000 C
     HVPS temp.    : 29.634 C
     HVPS voltage  : 39.998 V
     HVPS current  : 0.000 mA
   
   DAQ end...
     Unix time     : 1665583600
     DAQ time      : 20.0 s
     Live time     : 18.3 s
     Ch. 0 Hits    : 40002
     Ch. 16 Hits   : 40016
     Ch. 31 Hits   : 40020
     OR32 Hits     : 40030
     HVPS temp.    : 29.682 C
     HVPS voltage  : 39.996 V
     HVPS current  : 0.000 mA
   
   Bin config's...
     HG, Ch. 0     : 0
     LG, Ch. 0     : 0
     HG, Ch. 16    : 0
     LG, Ch. 16    : 0
     HG, Ch. 31    : 0
     LG, Ch. 31    : 0
   ```
