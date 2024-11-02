#!/usr/bin/python3

stat = input("HVPS Status Code (prepend with '0x' to input hex code): ")

hexinp = False
if (stat[:2] == "0x"):
    stat = stat[2:]
    hexinp = True
    
if (hexinp):
    print(f"Processing status input: 0x{stat} ({int(stat, 16)})")
    stat = int(stat, 16)
else:
    print(f"Processing status input: {stat} (0x{int(stat):>04X})")
    stat = int(stat)
    
statlist = [
    ["High voltage output:          ", "OFF", "ON"],
    ["Overcurrent protection:       ", "Not working", "Working"],
    ["Output current value:         ", "Within specification", "Outside specification"],
    ["Temperature sensor connected: ", "No", "Yes" ],
    ["Operating temperature limit:  ", "Within specification", "Outside specification"],
    ["Reserve 5:                    ", "-", "-"],
    ["Temperature correction:       ", "Enabled", "Disabled"],
    ["Reserve 7:                    ", "-", "-"],
    ["Reserve 8:                    ", "-", "-"],
    ["Reserve 9:                    ", "-", "-"],
    ["Automatic restoration:        ", "Inactive", "On-going"],
    ["Voltage suppression:          ", "Inactive", "On-going"],
    ["Output voltage:               ", "ON", "OFF"],
    ["Reserve 13:                   ", "-", "-"],
    ["Voltage stability:            ", "Unstable", "Stable"],
    ["Reserve 15:                   ", "-", "-"],
]

bit = 0
while (bit < 16):
    s = f"  [bit {bit:>2}: {(stat&(1<<bit))>>bit}] "
    s += statlist[bit][0]
    if (stat & (1 << bit)):
        s += statlist[bit][2]
    else:
        s += statlist[bit][1]
    print(s)
    bit += 1
