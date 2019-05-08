#!/usr/bin/python3

import time
import serial
import subprocess
import fileconv

def writeandreadData(s):
	print(s)
	print("length:")
	print(ser.write(s))
	out = ''
	time.sleep(10)
	while ser.inWaiting() > 0:
		out+=(ser.read(1)).decode()
	if out != '':
		print ("<<"+out)
		f = open("output.txt", 'w')
		f.write(out)
		f.close()
	print ("\n ----------------------------------------------------------------- \n")
		
def writeData(s):
	print(s)
	ser.write(s.encode())
	
def startSerial():
	
	serport = input("Enter serial port number \n >> ")
	serport = '/dev/ttyS' + serport
	subprocess.call(['sudo', 'chmod', '666', serport])
	ser.port =  serport
	ser.baudrate = 115200
	ser.open()

ser = serial.Serial()
startSerial()
ser.isOpen()

def printMenu():
	print ("""Enter command of your selection \n
		1. Citiroc Configuration \n
		2. Probe configuration \n
		3. Power supply configuration \n 
		4. Housekeeping request \n 
		5. Payload data request \n
		6. DAQ Duration \n
		7. Turn on DAQ \n
		8. Turn off DAQ \n
		Write 'exit' to exit the application""")
		
inp = '1'
data = "data"

while 1:
	printMenu()
	inp = input(">> ")
	if inp =='exit':
		ser.close()
		exit()
	elif inp=='1':
		print ("Citiroc configuration selected, please select input file \n")
		writeandreadData(b'a' + fileconv.converter())
	elif inp=='2':
		print ("Probe configuration selected, please select input file \n")
		writeandreadData(b'b' + fileconv.converter())
	elif inp=='3':
		print ("Power supply configuration selected, please enter configuration data \n")
		data = input(">> ")
		writeData('c' + data)
	elif inp=='4':
		print ("Housekeeping request selected, please wait for data \n")
		writeandreadData('d')
	elif inp=='5':
		print ("Payload request selected, please wait for data \n")
		writeandreadData('e')
	elif inp=='6':
		print ("DAQ duration configuration received, please enter time:")
		data = input(">> ")
		writeData('f' + data)
	elif inp=='7':
		print ("DAQ On command received")
		writeDate('g')
	elif inp=='8':
		print ("DAQ off command received")
		writeData('h')
	else:
		print("ERROR: Command not recognized, please try again")

