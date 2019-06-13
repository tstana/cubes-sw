#!/usr/bin/python3

import time
import serial
import subprocess
import fileconv
import sys
from struct import unpack_from

from proto_cubes_cmds import *

def writeandreadData(s):
	print(s)
	print("length:")
	print(ser.write(s))
	out = bytearray()
	time.sleep(5)
	while ser.inWaiting() > 0:
		out += (ser.read(1))
	if s == CMD_REQ_HK:
		return out
		
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
		try:
			d = CMD_CITIROC_CONF + fileconv.converter()
			writeandreadData(d)
		except Exception as e:
			print()
			print(e)
			print()
	elif inp=='2':
		print ("Probe configuration selected, please select input file \n")
		try:
			d = CMD_PROBE_CONF + fileconv.converter()
			writeandreadData(d)
		except Exception as e:
			print()
			print(e)
			print()
	elif inp=='3':
		print ("Power supply configuration selected, please enter configuration data \n")
		data = input(">> ")
		writeData(CMD_HVPS_CONF + data)
	elif inp=='4':
		print ("Housekeeping request selected, please wait for data \n")
		returned = writeandreadData(CMD_REQ_HK)
		decodeddata = returned.decode('utf-8', 'ignore')
		offset = decodeddata.find("Unix time: ")+23 #Offset unix time data
		if offset != -1:
			try:
				channels = unpack_from('>LLLL', returned, offset)
				print("Hit count \n------------------------")
				print("Channel  0: %i (0x%08X)" % (channels[0], channels[0]))
				print("Channel 16: %i (0x%08X)" % (channels[1], channels[1]))
				print("Channel 31: %i (0x%08X)" % (channels[2], channels[2]))
				print("Channel 21: %i (0x%08X)" % (channels[3], channels[3]))
			except Exception as ex:
				print(ex)

			
		else:
			print("Unix time not found")
	elif inp=='5':
		print ("Payload request selected, please wait for data \n")
		writeandreadData(CMD_REQ_PAYLOAD)
	elif inp=='6':
		print ("DAQ duration configuration received, please enter time:")
		data = input(">> ")
		writeData('f' + data)
	elif inp=='7':
		print ("DAQ On command received")
		writeData('g')
	elif inp=='8':
		print ("DAQ off command received")
		writeData('x')
	else:
		print("ERROR: Command not recognized, please try again")
	print ("\n ----------------------------------------------------------------- \n")

