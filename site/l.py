import paho.mqtt.client as mqtt

temperature = 20
humidity = 30

def on_connect(client, userdata, flags, rc):
    print(f'Connected code: {rc}')
    subscriber.subscribe("user_33e0a052/SmartHouse")

def on_message(client, userdata, msg):
    data = msg.payload.decode('UTF-8').split('/')
    if len(data)>=4:
        with open('d.txt', 'w') as f:
            print(data)
            temperature = data[0]
            humidity = data[1]
            brightness = data[2]
            is_fire = data[3]
            f.write(temperature + ' ' + humidity + ' ' + brightness + ' ' + is_fire)
    


subscriber = mqtt.Client()
subscriber.on_message = on_message
subscriber.on_connect = on_connect

subscriber.username_pw_set('user_33e0a052', 'pass_88cf2f67')
subscriber.connect("srv1.clusterfly.ru", 9124)
subscriber.loop_forever()