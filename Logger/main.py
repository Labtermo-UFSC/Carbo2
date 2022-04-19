from xml.sax import xmlreader
from paho.mqtt import client as mqtt_client
from threading import Thread 
import time 
import pygsheets
import pandas as pd
import numpy as np
#authorization
gc = pygsheets.authorize(service_file='C:/Users/lucas/Documents/GitHub/Carbo2/Logger/carbo2-e79fef51432f.json')
sh = gc.open('Abacaxi')
wks = sh[0]
broker = '54.232.245.44'
port = 1883
topics = [("0001-vega/sensor/mh-z19_co2_value/state",0), ("0002-sirius/sensor/mh-z19_co2_value/state",0), ("0003-sol/sensor/mh-z19_co2_value/state",0), ("0004-arcturus/sensor/mh-z19_co2_value/state",0), ("0005-alphacentauri/sensor/mh-z19_co2_value/state",0), ("0006-carina/sensor/mh-z19_co2_value/state",0), ("0007-perseus/sensor/mh-z19_co2_value/state",0), ("0008-draco/sensor/mh-z19_co2_value/state",0), ("0009-hydra/sensor/mh-z19_co2_value/state",0), ("000X-ComFiltro/sensor/mh-z19_co2_value/state",0), ("0010-leo/sensor/mh-z19_co2_value/state",0), ("0011-lyra/sensor/mh-z19_co2_value/state",0), ("0012-virgo/sensor/mh-z19_co2_value/state",0), ("0013-hercules/sensor/mh-z19_co2_value/state",0), ("0014-andromeda/sensor/mh-z19_co2_value/state",0), ("0015-ara/sensor/mh-z19_co2_value/state",0), ("0016-lynx/sensor/mh-z19_co2_value/state",0), ("0017-canis/sensor/mh-z19_co2_value/state",0), ("0018-castor/sensor/mh-z19_co2_value/state",0), ("0019-alzir/sensor/mh-z19_co2_value/state",0), ("0020-antares/sensor/mh-z19_co2_value/state",0),
("0021-scorpius/sensor/mh-z19_co2_value/state",0), ("0022-grus/sensor/mh-z19_co2_value/state",0), ("0023-gemini/sensor/mh-z19_co2_value/state",0), ("0024-lepus/sensor/mh-z19_co2_value/state",0), ("0025-orion/sensor/mh-z19_co2_value/state",0), ("0026-aries/sensor/mh-z19_co2_value/state",0), ("0027-formax/sensor/mh-z19_co2_value/state",0), ("0028-auriga/sensor/mh-z19_co2_value/state",0), ("0029-vela/sensor/mh-z19_co2_value/state",0), ("0030-libra/sensor/mh-z19_co2_value/state",0)]
client_id = f'python-mqtt'
username = 'paiva404'
password = 'Paiva123'
sheetsInterations = 2
sensorListRaw = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
sensorListInterations = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
sensorListCurrent = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
googleHeader = [[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]]

def initSheet():
    wks.update_value('A1', "Data")
    sensorHeader = [[1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30]]
    print(sensorHeader)
    wks.update_values('B1', sensorHeader)

class calcAndLog(Thread): 
    def __init__(self, seconds): 
        '''Note that when you override __init__, you must  
           use super() to call __init__() in the base class 
           so you'll get all the "chocolately-goodness" of 
           threading (i.e., the magic that sets up the thread 
           within the OS) or it won't work. 
        '''         
        super().__init__()  
        self.delay = seconds 
        self.is_done = False 
 
    def done(self): 
        self.is_done = True 
 
    def run(self): 
        while not self.is_done: 
            global sheetsInterations
            time.sleep(self.delay) 
            for i in range(1, 30):
                if sensorListInterations[i]!=0:
                    sensorListCurrent[i] = sensorListRaw[i]/sensorListInterations[i]
                sensorListInterations[i] = 0
                sensorListRaw[i] = 0
                googleHeader[0][i] = sensorListCurrent[i]
            rowName = 'B'+str(sheetsInterations)
            sheetsInterations = sheetsInterations + 1
            wks.update_values(rowName, googleHeader)
        print('thread done') 

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")
        sensorNumber = int(msg.topic[0:4])
        print(f"Sensor number: {sensorNumber}")
        sensorListRaw[sensorNumber] += int(msg.payload.decode())
        sensorListInterations[sensorNumber] += 1
        print(sensorListRaw)

    client.subscribe(topics)
    client.on_message = on_message

def run():
    initSheet()
    client = connect_mqtt()
    subscribe(client)
    t = calcAndLog(10) 
    t.start()
    client.loop_forever() 

if __name__ == '__main__':
    run()