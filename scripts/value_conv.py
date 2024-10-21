#!/usr/bin/python3
import math
import sys

print("Please select:")
print("  '1' for voltage, hex to dec");
print("  '2' for voltage, dec to hex");
print("  '3' for temperature, hex to dec");
print("  '4' for temperature, dec to hex");
print()

types = input("Your selection: ")

if types == '1':
	number = input("Write the voltage in hex: ")
	i = int(number, 16)
	output = i * 1.812e-3
if types == '2':
	number = float(input("Write the voltage in volts: "))
	i = int(number / 1.812e-3)
	output = hex(i)
	output += f" ({int(output, 16)})"
if types == '3':
	number = int(input("Write the temperature in hex: "), 16)
	i = (number*(1.907e-5)-1.035)/(-5.5e-3)
	output = i
if types == '4':
	number = float(input("Write the temperature in degrees C: "))
	i = int(((number*(-5.5e-3))+1.035)/1.907e-5)
	output = hex(i)
	output += f" ({int(output, 16)})"

print(output)

