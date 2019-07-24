#!/usr/bin/python3
import time
import datetime
import subprocess
import sys
import binascii
import matplotlib
import matplotlib.pyplot as plt
from struct import unpack

print("NOTE: This script prints graphs to a separate window. It requires a " +
      "windowing system to run.\n" +
      "      Command-line-only systems will not work.")

print()

filename = input("Enter file name: ")
f = open(filename, 'rb')
filecontent = bytearray(f.read())
start = filecontent.find(("Unix time: ").encode())+11 #Offsetting "Unix time: "
unixtime = int(filecontent[start:start+10])
binno = unpack('>h',filecontent[start+266:start+268])
i = 0
datavalues = list(range(0, 12287, 1))
for x in range(start+269, len(filecontent)-146, 2):
    try:
        datavalues[i] = unpack('=H', filecontent[x:x+2])[0]
    except:
        print("Value at location %i in the file has an issue" %x)
    i=i+1

f.close()

plt.figure(figsize=(6, 9))
plt.suptitle("Bins: %i   Unix time: %i" %(binno[0],unixtime))

ax0 = plt.subplot(321)
ax0.plot(datavalues[0:2047])
ax0.title.set_text('Ch 0 HG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax1 = plt.subplot(323)
ax1.plot(datavalues[2048:4095])
ax1.title.set_text('Ch 16 HG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax2 = plt.subplot(325)
ax2.plot(datavalues[4096:6143])
ax2.title.set_text('Ch 31 HG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax3 = plt.subplot(322)
ax3.plot(datavalues[6144:8191])
ax3.title.set_text('Ch 0 LG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax4 = plt.subplot(324)
ax4.plot(datavalues[8192:10239])
ax4.title.set_text('Ch 16 LG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax5 = plt.subplot(326)
ax5.plot(datavalues[10240:12287])
ax5.title.set_text('Ch 31 LG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

plt.tight_layout()

print("Waiting for the user to close the plot...")

plt.show()
