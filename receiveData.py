
# coding: utf-8

# In[ ]:


import paho.mqtt.client as mqtt
from serial import Serial
import time
import random

client = mqtt.Client()
ser = Serial('COM7', 9600)


def on_connect(client, userdata, flags, rc):
    print("Connected: ", str(rc))
    client.subscribe("pinboards/ledline/percent")

def on_message(client, userdata, msg):
    print(msg.topic, msg.payload.decode())
    ser.write(msg.payload + b'\n')


client.on_connect = on_connect
client.on_message = on_message

client.connect("simurg.iszf.irk.ru", 1883, 60)
client.loop_forever()


# In[4]:


ser.close()

