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

datavalues = list(range(0, 2047, 1))
datavalues1 = list(range(2048, 4095, 1))
datavalues2 = list(range(4096, 6143, 1))
datavalues3 = list(range(6144, 8191, 1))
datavalues4 = list(range(8192, 10239, 1))
datavalues5 = list(range(10240, 12287, 1))


f.close()
filename = filename[:-3]
filename = filename + 'png'
fig = plt.figure(figsize=(6, 9))
plt.suptitle("Bins: %i   Unix time: %i" %(binno[0],unixtime))


ax = plt.subplot(321)
ax.plot(datavalues)
ax.title.set_text('Ch 0 HG')
plt.grid(True)
ax1 = plt.subplot(323)
ax1.plot(datavalues1)
ax1.title.set_text('Ch 16 HG')
plt.grid(True)
ax2 = plt.subplot(325)
ax2.plot(datavalues2)
ax2.title.set_text('Ch 31 HG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)
ax3 = plt.subplot(322)
ax3.plot(datavalues3)
ax3.title.set_text('Ch 0 LG')
plt.grid(True)
ax4 = plt.subplot(324)
ax4.plot(datavalues4)
ax4.title.set_text('Ch 16 LG')
plt.grid(True)
ax5 = plt.subplot(326)
ax5.plot(datavalues5)
ax5.title.set_text('Ch 31 LG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

fig = plt.tight_layout()
plt.show()
