import glob, os
from struct import unpack_from, unpack
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

outch0 = []
outch16 = []
outch31 = []
outor = []
outtime = []
for filename in glob.glob('_HK.dat'):
    file = open(filename, 'r')
    data = file.readlines()
    for line in data:
        if '#' not in line and '\n' not in line[0]:
            split = line.split(',')
            outch0.append(int(split[8])) 
            outch16.append(int(split[9]))
            outch31.append(int(split[10]))
            outor.append(int(split[11]))
            outtime.append(int(split[0]))
        else:
            pass
    file.close()
orgtime = outtime[0]
outtime[:] = [x-orgtime for x in outtime]
printch0 = [x for _,x in sorted(zip(outtime, outch0))]
printch16 = [x for _,x in sorted(zip(outtime, outch16))]
printch31 = [x for _,x in sorted(zip(outtime, outch31))]
printor = [x for _,x in sorted(zip(outtime, outor))]
printtime = sorted(outtime)


plt.figure('HCR counts over time', figsize=(10,10))
plt.suptitle('HCR counts difference between runs \n Time of start: %i' %(orgtime))

ax1 = plt.subplot(221)
ax1.plot(printtime, printch0)
ax1.title.set_text('CH0')
plt.ylabel('Counts')
plt.xlabel('Time')
ax1.grid(True)

ax2 = plt.subplot(222)
ax2.plot(printtime, printch16)
ax2.title.set_text('CH16')
plt.ylabel('Counts')
plt.xlabel('Time')
ax2.grid(True)

ax3 = plt.subplot(223)
ax3.plot(printtime, printch31)
ax3.title.set_text('CH31')
plt.ylabel('Counts')
plt.xlabel('Time')
ax3.grid(True)

ax4 = plt.subplot(224)
ax4.plot(printtime, printor)
ax4.title.set_text('OR32')
plt.ylabel('Counts')
plt.xlabel('Time')
ax4.grid(True)

#plt.show()
plt.tight_layout()
plt.savefig('hcr.png')