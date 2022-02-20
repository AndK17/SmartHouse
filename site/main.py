from flask import Flask, render_template, request
from paho.mqtt import publish

temperature = 20
humidity = 30
is_fire = 'Нет'
br = 0
clr = '#000000'
heating = 0
ventilation = 0

app = Flask(__name__)

@app.route('/', methods=['GET', "POST"])
def index():
    global br, clr, heating, ventilation
    if request.method == 'POST':
        br = request.form.get('brightness')
        clr = request.form.get('color')
        if request.form.get('heating') == 'on':
            heating = 1
        else:
            heating = 0
            
        if request.form.get('ventilation') == 'on':
            ventilation = 1
        else:
            ventilation = 0
            
        if request.form.get('action3') == "ok":
            h = request.form.get('color').lstrip('#')
            c = '/' + '/'.join([str(int(h[i:i+2], 16)) for i in (0, 2, 4)]) + '/' + request.form.get('brightness') + '/' + str(heating) + '/' + str(ventilation) + '/'
            print(c)
            data =  c
            publish.single("user_33e0a052/color", data, hostname="srv1.clusterfly.ru", port=9124, auth={'username':'user_33e0a052','password':'pass_88cf2f67'})
        elif request.form.get('avtolight') == "авто":
            h = request.form.get('color').lstrip('#')
            c = '/' + '/'.join([str(int(h[i:i+2], 16)) for i in (0, 2, 4)]) + '/' + '-1' + '/' + str(heating) + '/' + str(ventilation) + '/'
            print(c)
            data =  c
            publish.single("user_33e0a052/color", data, hostname="srv1.clusterfly.ru", port=9124, auth={'username':'user_33e0a052','password':'pass_88cf2f67'})
    return render_template('index.html', temperature=temperature, humidity=humidity, is_fire=is_fire, br=br, clr=clr, heating=heating, ventilation=ventilation)


@app.route('/getd', methods=['GET', "POST"])
def getd():
    if request.method == 'POST':
        with open('d.txt', 'r') as f:
            data = f.read().split()
            return ' '.join(data)


app.run(debug=True)