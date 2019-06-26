#!/usr/bin/python3
import time
import datetime
import subprocess
import sys
import binascii
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from struct import unpack

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
filename = filename[:-3]
filename = filename + 'png'
fig = plt.figure()
ax = plt.subplot(111)
ax.plot(datavalues)
plt.ylabel('Energy levels')
plt.xlabel('ADC channels')
plt.title('DAQ retrieved')
plt.annotate("Unix time: %i" %unixtime, xy=(0.65, 0.96), xycoords='axes fraction')
plt.annotate("Bins: %i" %binno, xy=(0.01, 0.95), xycoords='axes fraction')
plt.grid(True)
fig.savefig(filename)
plt.close(fig)
print("DONE")