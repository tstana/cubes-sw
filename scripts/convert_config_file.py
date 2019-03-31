#!/usr/bin/python3
import math
import binascii

def Generator(s, n):
	for start in range(0, len(s)-1, n):
		yield s[start:start+n]

def invert_bits(s):
	print(s)
	t = s[::-1]
	print(t)
	print("-")
	return t

inputname = input("Enter input file name: ")
while (1):
	sel = int(input("What will you be configuring? (1: ASIC, 2: Probes) "))
	if (sel == 1):
		outputname = "asic.cfg"
		break;
	elif (sel == 2):
		outputname = "probe.cfg"
		break;
	else:
		print("ERROR: Please select (1: ASIC) or (2: Probes)\n");

f = open(inputname, 'r') #Name of input file
g = open(outputname, 'wb') #Name of output file 
fline = f.readline()
for y in Generator(fline, 8):
	y=(int(invert_bits(y), 2))
	g.write(y.to_bytes(1, 'little'))
f.close()
g.close()
print("Wrote successfully to " + outputname);
#
