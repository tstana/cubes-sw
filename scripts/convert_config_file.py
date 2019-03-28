import math
import binascii

def Generator(s, n):
	for start in range(0, len(s)-1, n):
		yield s[start:start+n]

def inverter(s):
	print(s)
	t = s[::-1]
	print(t)
	print("-")
	return t

filename = input("Enter input file name: ")
outputname = input("Enter output file name on format 8.3: ")	
f = open(filename, 'r') #Name of input file
g = open(outputname, 'wb') #Name of output file 
fline = f.readline()
for y in Generator(fline, 8):
	y=(int(inverter(y), 2))
	g.write(y.to_bytes(1, 'little'))
f.close()
g.close()

