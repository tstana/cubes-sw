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

#-------------------------------------------------------------------------------
# 	Linear plot
#-------------------------------------------------------------------------------
plt.figure("Linear Plot", figsize=(6, 9))
plt.suptitle("Bins: %i   Unix time: %i" %(binno[0],unixtime))

ax00 = plt.subplot(321)
ax00.plot(datavalues[0:2047])
ax00.title.set_text('Ch 0 HG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax01 = plt.subplot(322)
ax01.plot(datavalues[2048:4095])
ax01.title.set_text('Ch 0 LG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax02 = plt.subplot(323)
ax02.plot(datavalues[4096:6143])
ax02.title.set_text('Ch 16 HG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax03 = plt.subplot(324)
ax03.plot(datavalues[6144:8191])
ax03.title.set_text('Ch 16 LG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax04 = plt.subplot(325)
ax04.plot(datavalues[8192:10239])
ax04.title.set_text('Ch 31 HG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax05 = plt.subplot(326)
ax05.plot(datavalues[10240:12287])
ax05.title.set_text('Ch 31 LG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)


#-------------------------------------------------------------------------------
#   Log. plot 
#-------------------------------------------------------------------------------
plt.figure("Logarithmic Plot", figsize=(6, 9))
plt.suptitle("Bins: %i   Unix time: %i" %(binno[0],unixtime))

ax10 = plt.subplot(321)
ax10.semilogy()
ax10.plot(datavalues[0:2047])
ax10.title.set_text('Ch 0 HG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)


ax11 = plt.subplot(322)
ax11.semilogy()
ax11.plot(datavalues[2048:4095])
ax11.title.set_text('Ch 0 LG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax12 = plt.subplot(323)
ax12.semilogy()
ax12.plot(datavalues[4096:6143])
ax12.title.set_text('Ch 16 HG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax13 = plt.subplot(324)
ax13.semilogy()
ax13.plot(datavalues[6144:8191])
ax13.title.set_text('Ch 16 LG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax14 = plt.subplot(325)
ax14.semilogy()
ax14.plot(datavalues[8192:10239])
ax14.title.set_text('Ch 31 HG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

ax15 = plt.subplot(326)
ax15.semilogy()
ax15.plot(datavalues[10240:12287])
ax15.title.set_text('Ch 31 LG')
plt.ylabel('Counts')
plt.xlabel('ADC channels')
plt.grid(True)

#-------------------------------------------------------------------------------
# Finally, show the plots...
#-------------------------------------------------------------------------------
print("Waiting for the user to close the plots...")

plt.tight_layout()

plt.show()

