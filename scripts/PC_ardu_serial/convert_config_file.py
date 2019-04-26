import math
import binascii

def Generator(s, n):
	for start in range(0, len(s)-1, n):
		yield s[start:start+n]

def inverter(s):
	t = s[::-1]
	return t

def converter():
	filename = input("Enter input file name: ")
	outputname = "conffil.txt"
	f = open(filename, 'r') #Name of input file
	g = open(outputname, 'r+b') #Name of output file 
	fline = f.readline()
	for y in Generator(fline, 8):
		y=(int(inverter(y), 2))
		g.write(y.to_bytes(1, 'little'))
	
	f.close()
	g.seek(0,0)
	gline = g.readline()
	g.close()
	return  gline