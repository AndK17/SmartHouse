import serial
import datetime
import time
from paho.mqtt import publish
ser = serial.Serial('/dev/ttyACM0',9600)
ser.reset_input_buffer()
now = time
while True:
    try:
        read_serial = ser.readline()
        read_serial = read_serial.decode('UTF-8')
        print (' '+read_serial)
        publish.single("user_33e0a052/SmartHouse", str(read_serial)+'\n', hostname="srv1.clusterfly.ru", port=9124, auth={'username':'user_33e0a052','password':'pass_88cf2f67'}) 


    except:
        continue
