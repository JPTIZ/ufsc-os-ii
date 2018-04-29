from time import time
import serial

mote = serial.Serial(baudrate = 115200, port='/dev/ttyACM0', timeout=0.5)

# write the data
epoch = '{time}X'.format(time=time())
mote.write(bytes(epoch, 'UTF-8'))

while True:
	print (str(mote.read(100), errors='ignore'))
