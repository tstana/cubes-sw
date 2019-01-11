import math
import sys


types = input("Select 1 for from hex to decimal, 2 for decimal to hex, 3 for temperature from hex, 4 for temperature to hex: ")
if types == 1:
	number = raw_input("Write the number in hex: ")
	i = int(number, 16)
	output = i * 1.812e-3
if types == 2:
	number = int(raw_input("Write the number in decimal: "))
	i = int(number / 1.812e-3)
	output = hex(i)
if types == 3:
	number = int(raw_input("Write the temperature in hex: "), 16)
	i = (number*(1.907e-5)-1.035)/(-5.5e-3)
	output = i
if types == 4:
	number = int(raw_input("Write the temperature in degrees: "))
	i = int(((number*(-5.5e-3))+1.035)/1.907e-5)
	output = hex(i)
print output
