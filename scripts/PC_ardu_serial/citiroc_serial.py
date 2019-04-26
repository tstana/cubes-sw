

import time
import serial
import subprocess

def writeandreadData(s):
	ser.write(s.encode())		
	out = ''
	time.sleep(10)
	while ser.inWaiting() > 0:
		out+=(ser.read(1)).decode()
	if out != '':
		print ("<<"+out)
	print ("\n ----------------------------------------------------------------- \n")
		
def writeData(s):
	ser.write(s)
	

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
		print ("Citiroc configuration selected, please enter configuration data \n")
		data = input(">> ")
		print('a' + data)
	elif inp=='2':
		print ("Probe configuration selected, please enter configuration data \n")
		data = input(">> ")
		writeData('b' + data)
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
	