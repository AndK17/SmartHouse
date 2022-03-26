import serial
import datetime
import time
import paho.mqtt.client as mqtt
ser = serial.Serial('/dev/ttyACM0',9600)
ser.reset_input_buffer()

f = open("oooo.txt", 'w')
f.close()
def on_connect(client, userdata, flags, rc):
    print(f'Connected code: {rc}')
    subscriber.subscribe("user_33e0a052/color")
def on_message(client, userdata, msg):
    data = msg.payload
    print(data)
    ser.write(data)
subscriber = mqtt.Client()
subscriber.on_message = on_message
subscriber.on_connect = on_connect

subscriber.username_pw_set('user_33e0a052', 'pass_88cf2f67')
subscriber.connect("srv1.clusterfly.ru", 9124)
subscriber.loop_forever()
