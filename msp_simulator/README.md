# MSP Simulator config file

The software in obcsim is set up through the config file to do some voltage changes and a power cycle of the power supply.
The power cycle is set up to run through the default "ACTIVE", "SLEEP" and "POWER_OFF" commands.

To regenerate the obcsim folder, replace the config.txt file in https://gitlab.com/kth-mist/msp-simulator/tree/b1-addsim with the one found in this folder. Then run setup.py with Python3 and you will find then new folder in the target folder.